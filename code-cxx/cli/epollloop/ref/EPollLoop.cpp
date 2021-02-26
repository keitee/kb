/*
 * EPollLoop.cpp
 *
 */

#include "EPollLoop.h"

#include <Logging.h>
#include <Mutex.h>
#include <ConditionVariable.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>

#include <cinttypes>


EPollLoop::EPollLoop(const std::string& name, int priority)
    : mName(name)
    , mPriority(priority)
    , mEpollFd(-1)
    , mDeathEventFd(-1)
    , mTagCounter(1)
{
}

EPollLoop::~EPollLoop()
{
    AI_LOG_FN_ENTRY();

    if (mEpollThread.joinable())
        stop();

    if ((mEpollFd >= 0) && (close(mEpollFd) != 0))
        AI_LOG_SYS_ERROR(errno, "failed to close epoll fd");
    if ((mDeathEventFd >= 0) && (close(mDeathEventFd) != 0))
        AI_LOG_SYS_ERROR(errno, "failed to close event fd");

    cleanUpTimers();

    AI_LOG_FN_EXIT();
}

// -----------------------------------------------------------------------------
/**
 * @brief Removes all timers from epoll and closes any open timerfds
 *
 */
void EPollLoop::cleanUpTimers()
{
    AI_LOG_FN_ENTRY();

    std::map<int64_t, TimerDetails>::iterator it = mTimers.begin();
    while (it != mTimers.end())
    {
        int fd = it->second.fd;

        if ((fd >= 0) && (close(fd) != 0))
            AI_LOG_SYS_ERROR(errno, "failed to close timer fd");

        it = mTimers.erase(it);
    }

    AI_LOG_FN_EXIT();
}

// -----------------------------------------------------------------------------
/**
 * @brief Returns true if the poll loop is running.
 *
 *
 */
bool EPollLoop::isRunning() const
{
    return mEpollThread.joinable();
}

// -----------------------------------------------------------------------------
/**
 * @brief Returns the thread id of the poll loop thread.
 *
 * If the poll loop /thread is not current running, a default constructed
 * std::thread::id is returned.
 *
 */
std::thread::id EPollLoop::threadId() const
{
    return mEpollThread.get_id();
}

// -----------------------------------------------------------------------------
/**
 *  @brief Creates and starts the poll loop
 *
 *
 *  @return true on success, false on failure.
 */
bool EPollLoop::start()
{
    AI_LOG_FN_ENTRY();

    if (mEpollThread.joinable())
    {
        AI_LOG_ERROR_EXIT("poll loop already running");
        return true;
    }

    // create an eventfd to signal death
    mDeathEventFd = eventfd(0, EFD_CLOEXEC | EFD_SEMAPHORE);
    if (mDeathEventFd < 0)
    {
        AI_LOG_SYS_FATAL_EXIT(errno, "failed to create death eventfd");
        return false;
    }

    // create the epoll fd
    mEpollFd = epoll_create1(EPOLL_CLOEXEC);
    if (mEpollFd < 0)
    {
        AI_LOG_SYS_FATAL_EXIT(errno, "failed to create epoll device");
        close(mDeathEventFd);
        mDeathEventFd = -1;
        return false;
    }

    // add the eventfd to the epoll loop
    struct epoll_event event;
    bzero(&event, sizeof(event));
    event.data.u64 = 0;
    event.events = EPOLLIN;

    if (epoll_ctl(mEpollFd, EPOLL_CTL_ADD, mDeathEventFd, &event) < 0)
    {
        AI_LOG_SYS_ERROR_EXIT(errno, "failed to add death eventfd to epoll");
        close(mDeathEventFd);
        close(mEpollFd);
        mDeathEventFd = mEpollFd = -1;
        return false;
    }

    // finally spawn the thread that runs the poll loop
    mEpollThread = std::thread(&EPollLoop::pollLoop, this);
    if (!mEpollThread.joinable())
    {
        AI_LOG_FATAL_EXIT("failed to create epoll thread");
        close(mDeathEventFd);
        close(mEpollFd);
        mDeathEventFd = mEpollFd = -1;
        return false;
    }

    AI_LOG_FN_EXIT();
    return true;
}

// -----------------------------------------------------------------------------
/**
 *  @brief Stops the poll loop and cleans up any handlers / timers
 *
 *  It is not necessary to cancel timers or remove file descriptor listeners
 *  before or after calling stop.  The act of calling stop will clean up any
 *  handlers / timers already installed.
 *
 */
void EPollLoop::stop()
{
    AI_LOG_FN_ENTRY();

    if (!mEpollThread.joinable())
    {
        AI_LOG_ERROR_EXIT("poll loop not running");
        return;
    }

    // signal the eventfd which should cause the epoll thread to wake and drop out
    if (mDeathEventFd < 0)
    {
        AI_LOG_FATAL("invalid death eventfd, this is bad");
    }
    else
    {
        uint64_t doesntMatter = 1;
        ssize_t wr = TEMP_FAILURE_RETRY(write(mDeathEventFd, &doesntMatter, sizeof(doesntMatter)));
        if (wr != sizeof(doesntMatter))
        {
            AI_LOG_SYS_ERROR(errno, "failed to signal death of epoll thread");
        }
        else
        {
            mEpollThread.join();
        }

        if (close(mDeathEventFd) != 0)
        {
            AI_LOG_SYS_ERROR(errno, "failed to close eventfd");
        }
        mDeathEventFd = -1;
    }

    // close epoll
    if ((mEpollFd >= 0) && (close(mEpollFd) != 0))
    {
        AI_LOG_SYS_ERROR(errno, "failed to close epoll fd");
    }
    mEpollFd = -1;

    // remove any installed handlers
    mGenericFds.clear();

    // remove any timers
    cleanUpTimers();

    AI_LOG_FN_EXIT();
}

// -----------------------------------------------------------------------------
/**
 *  @brief Installs the timerfd into the epoll loop.
 *
 *
 *
 *
 *  @param[in]  fd          The timerfd to install.
 *  @param[in]  handler     The handler function to call.
 *  @param[in]  its         The timeout values to use for the timer.
 *  @param[in]  singleShot  If true the time is a singleshot timer.
 *
 *  @return on success returns true, otherwise false.
 */
int64_t EPollLoop::installTimer(int fd, const struct itimerspec &its,
                                const TimerHandler& handler, bool singleShot)
{
    AI_LOG_FN_ENTRY();

    if (mEpollFd < 0)
    {
        AI_LOG_ERROR_EXIT("poll loop not running");
        return -1;
    }

    // create the event for epoll
    struct epoll_event event;
    bzero(&event, sizeof(event));
    event.events = EPOLLIN;

    // take the lock before trying to add to epoll
    std::lock_guard<std::recursive_mutex> locker(mEpollLock);

    // generate a tag for the timer and store the handler against the fd
    int64_t tag = mTagCounter++;
    event.data.u64 = tag;

    // add to epoll
    if (epoll_ctl(mEpollFd, EPOLL_CTL_ADD, fd, &event) < 0)
    {
        AI_LOG_SYS_ERROR_EXIT(errno, "failed to add timerfd to epoll");
        return -1;
    }

    // generate a tag for the timer and store the handler against the fd
    TimerDetails details;
    details.fd = fd;
    details.singleShot = singleShot;
    details.handler = handler;
    details.timeout = its;

    mTimers.emplace(tag, details);

    return tag;
}

// -----------------------------------------------------------------------------
/**
 *  @brief Starts a single shot timer that will call the given handler
 *
 *  Single shot timers don't need to (and can't) be explicitly cancelled, once
 *  they fire they will automatically be cleaned up.
 *
 *
 *  @param[in]  timeout     The initial value of the time-out, if the type
 *                          is periodic the timer will then fire at this period
 *                          after the initial timeout.
 *  @param[in]  handler     The callback handler to call on timeout.
 *
 *  @return on success returns true, otherwise false.
 */
bool EPollLoop::singleShotTimer(const std::chrono::nanoseconds& timeout,
                                const TimerHandler& handler)
{
    AI_LOG_FN_ENTRY();

    if (timeout.count() < 0)
    {
        AI_LOG_ERROR_EXIT("timer duration must be positive");
        return -1;
    }

    // create the timer
    int fd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
    if (fd < 0)
    {
        AI_LOG_SYS_ERROR_EXIT(errno, "failed to create timerfd device");
        return -1;
    }

    // set the timeout values
    struct itimerspec its;
    its.it_value.tv_sec = std::chrono::duration_cast<std::chrono::seconds>(timeout).count();
    its.it_value.tv_nsec = (timeout % std::chrono::seconds(1)).count();

    // we allow the user to pass a zero timeout, however that will disable the
    // timerfd so we make the timeout have at least 1ns which is close enough
    if ((its.it_value.tv_sec == 0) && (its.it_value.tv_nsec == 0))
    {
        its.it_value.tv_nsec = 1;
    }

    // singleshot timers should have a zero interval time
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;


    // singleshot timers start immediately
    if (timerfd_settime(fd, 0, &its, NULL) != 0)
    {
        AI_LOG_SYS_ERROR_EXIT(errno, "failed to set timerfd device");
        return -1;
    }

    // try and install the timer
    int64_t tag = installTimer(fd, its, handler, true);
    if (tag < 0)
    {
        if (close(fd) != 0)
            AI_LOG_SYS_ERROR(errno, "failed to close timerfd");
    }

    AI_LOG_FN_EXIT();
    return (tag >= 0);
}

// -----------------------------------------------------------------------------
/**
 *  @brief Adds a periodic timer to the poll loop but doesn't start it.
 *
 *  Use EPollLoop::startTimer() and EPollLoop::stopTimer() to start and stop
 *  the timer.
 *
 *  @param[in]  timeout     The initial value of the time-out, if the type
 *                          is periodic the timer will then fire at this period
 *                          after the initial timeout.
 *  @param[in]  handler     The callback handler to call on timeout.
 *  @param[in]  type        The type of the timer, either one shot or periodic.
 *
 *  @return on success a positive tag number of the timer, on failure -1.
 */
int64_t EPollLoop::addTimer(const std::chrono::nanoseconds& initial,
                            const std::chrono::nanoseconds& interval,
                            const TimerHandler& handler)
{
    AI_LOG_FN_ENTRY();

    if (interval.count() <= 0)
    {
        AI_LOG_ERROR_EXIT("timer interval must be positive");
        return -1;
    }

    // create the timer
    int fd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
    if (fd < 0)
    {
        AI_LOG_SYS_ERROR_EXIT(errno, "failed to create timerfd device");
        return -1;
    }


    // set the timeout values
    struct itimerspec its;
    its.it_value.tv_sec = std::chrono::duration_cast<std::chrono::seconds>(initial).count();
    its.it_value.tv_nsec = (initial % std::chrono::seconds(1)).count();

    its.it_interval.tv_sec = std::chrono::duration_cast<std::chrono::seconds>(interval).count();
    its.it_interval.tv_nsec = (interval % std::chrono::seconds(1)).count();


    // we allow the user to pass a zero initial timeout, however that will
    // disable the timerfd so we make the timeout have at least 1ns which is
    // close enough
    if ((its.it_value.tv_sec == 0) && (its.it_value.tv_nsec == 0))
    {
        its.it_value.tv_nsec = 1;
    }


    // nb: don't call timerfd_settime(...) as that will start the timer, instead
    // we wait to EPollLoop::startTimer is called.

    // try and install the timer
    int64_t tag = installTimer(fd, its, handler, false);
    if (tag < 0)
    {
        if (close(fd) != 0)
            AI_LOG_SYS_ERROR(errno, "failed to close timerfd");
    }

    AI_LOG_FN_EXIT();
    return tag;
}

// -----------------------------------------------------------------------------
/**
 *  @brief Removes a timer from the poll loop
 *
 *  If the timer is currently running it will be stopped then removed.
 *
 *  @param[in]  tag         The tag of the timer returned by addTimer
 *
 *  @return true if the timer was cancelled, otherwise false.
 */
bool EPollLoop::removeTimer(int64_t tag)
{
    AI_LOG_FN_ENTRY();

    // take the lock before trying to find the timer
    std::lock_guard<std::recursive_mutex> locker(mEpollLock);

    std::map<int64_t, TimerDetails>::iterator it = mTimers.find(tag);
    if (it == mTimers.end())
    {
        AI_LOG_ERROR_EXIT("failed to find timer with tag %" PRId64 "", tag);
        return false;
    }
    if (it->second.singleShot)
    {
        AI_LOG_ERROR_EXIT("trying to remove singleshot timer with tag %" PRId64 "", tag);
        return false;
    }

    // get the timerfd fd
    const int fd = it->second.fd;

    // remove it from the handler list
    mTimers.erase(it);

    // and then remove from epoll
    if (mEpollFd >= 0)
    {
        if (epoll_ctl(mEpollFd, EPOLL_CTL_DEL, fd, NULL) < 0)
        {
            AI_LOG_SYS_ERROR(errno, "failed to delete timer from epoll");
        }
    }

    // and finally close the timer descriptor
    if (close(fd) != 0)
    {
        AI_LOG_SYS_ERROR(errno, "failed to close timerfd");
    }

    AI_LOG_FN_EXIT();
    return true;
}

// -----------------------------------------------------------------------------
/**
 *  @brief Starts a timer added with EPollLoop::addTimer()
 *
 *  It is safe to call this method from a pollloop callback.
 *
 *  This will start or restart the timer if it's currently running.  The time
 *  intervals are set when the timer was initially added with addTimer().
 *
 *  @param[in]  tag     The tag of the timer returned by addTimer
 *
 *  @return true if the timer was (re)started, otherwise false.
 */
bool EPollLoop::startTimer(int64_t tag)
{
    AI_LOG_FN_ENTRY();

    // take the lock before trying to find the timer
    std::lock_guard<std::recursive_mutex> locker(mEpollLock);

    std::map<int64_t, TimerDetails>::iterator it = mTimers.find(tag);
    if (it == mTimers.end())
    {
        AI_LOG_ERROR_EXIT("failed to find timer with tag %" PRId64 "", tag);
        return false;
    }

    // get the timerfd fd
    const int fd = it->second.fd;

    // set the timer's values
    if (timerfd_settime(fd, 0, &(it->second.timeout), NULL) != 0)
    {
        AI_LOG_SYS_ERROR_EXIT(errno, "failed to set timerfd device");
        return false;
    }

    AI_LOG_FN_EXIT();
    return true;
}

// -----------------------------------------------------------------------------
/**
 *  @brief Starts a timer added with EPollLoop::addTimer()
 *
 *  It is safe to call this method from a pollloop callback.
 *
 *  This will start or restart the timer if it's currently running.  The time
 *  intervals are set when the timer was initially added with addTimer().
 *
 *  @param[in]  tag     The tag of the timer returned by addTimer
 *
 *  @return true if the timer was (re)started, otherwise false.
 */
bool EPollLoop::stopTimer(int64_t tag)
{
    AI_LOG_FN_ENTRY();

    // take the lock before trying to find the timer
    std::lock_guard<std::recursive_mutex> locker(mEpollLock);

    std::map<int64_t, TimerDetails>::iterator it = mTimers.find(tag);
    if (it == mTimers.end())
    {
        AI_LOG_ERROR_EXIT("failed to find timer with tag %" PRId64 "", tag);
        return false;
    }

    // get the timerfd fd
    const int fd = it->second.fd;

    // clear the timer's values to disarm the timer
    static const struct itimerspec its = { { 0, 0L }, { 0, 0L } };
    if (timerfd_settime(fd, 0, &its, NULL) != 0)
    {
        AI_LOG_SYS_ERROR_EXIT(errno, "failed to set timerfd device");
        return false;
    }

    AI_LOG_FN_EXIT();
    return true;
}

// -----------------------------------------------------------------------------
/**
 *  @brief Adds a handler for a given file descriptor
 *
 *  This adds the fd to the epoll loop for the given events and installs the
 *  supplied handler for when event(s) are received.
 *
 *  @param[in]  fd      The file descriptor to poll on.
 *  @param[in]  events  Bitmask of events to be woken on (see EPollLoop::Events)
 *  @param[in]  handler The handler function to call on the event.
 *
 *  @return on success an tag to be used when you want to remove the handler,
 *  on failure -1.
 */
int64_t EPollLoop::addFileDescriptor(int fd, unsigned events, const Handler& handler)
{
    AI_LOG_FN_ENTRY();

    if (mEpollFd < 0)
    {
        AI_LOG_ERROR_EXIT("poll loop not running");
        return -1;
    }

    // create the event for epoll
    struct epoll_event event;
    bzero(&event, sizeof(event));

    event.events = 0;
    if (events & EPollLoop::Readable)
        event.events |= EPOLLIN;
    if (events & EPollLoop::Writeable)
        event.events |= EPOLLOUT;

    // take the lock before trying to add to epoll
    std::lock_guard<std::recursive_mutex> locker(mEpollLock);

    // get a tag for the handler
    int64_t tag = mTagCounter++;
    event.data.u64 = tag;

    // add to epoll
    if (epoll_ctl(mEpollFd, EPOLL_CTL_ADD, fd, &event) < 0)
    {
        AI_LOG_SYS_ERROR_EXIT(errno, "failed to add fd to epoll");
        return -1;
    }

    // store the handler against the fd
    GenericDetails details;
    details.fd = fd;
    details.events = events;
    details.handler = handler;

    mGenericFds.emplace(tag, details);

    AI_LOG_FN_EXIT();
    return tag;
}

// -----------------------------------------------------------------------------
/**
 *  @brief Removes a previously installed file descritor handler.
 *
 *  @param[in]  tag     The value returned by @a addFileDescriptor.
 *
 *  @return true if the handler was uninstalled, false on failure.
 */
bool EPollLoop::removeFileDescriptor(int64_t tag)
{
    AI_LOG_FN_ENTRY();

    // take the lock before trying to find the timer
    std::lock_guard<std::recursive_mutex> locker(mEpollLock);

    std::map<int64_t, GenericDetails>::iterator it = mGenericFds.find(tag);
    if (it == mGenericFds.end())
    {
        AI_LOG_ERROR_EXIT("failed to find tag %" PRId64 "", tag);
        return false;
    }

    // get the matching fd
    const int fd = it->second.fd;

    // remove it from the handler list
    mGenericFds.erase(it);

    // and then remove from epoll
    if (mEpollFd >= 0)
    {
        if (epoll_ctl(mEpollFd, EPOLL_CTL_DEL, fd, NULL) < 0)
        {
            AI_LOG_SYS_ERROR(errno, "failed to delete fd from epoll");
        }
    }

    AI_LOG_FN_EXIT();
    return true;
}

// -----------------------------------------------------------------------------
/**
 *  @brief Utility to run a function within the event loop thread
 *
 *  Installs a single eventfd in the pollloop and then waits till the handler
 *  is called and then returns.
 *
 *  @param  func        The function to call in the event loop
 *
 *  @return true of success, false on failure.
 */
bool EPollLoop::executeInPollloop(const Executor& func)
{
    int eventFd = eventfd(1, EFD_CLOEXEC);
    if (eventFd < 0)
    {
        AI_LOG_SYS_ERROR(errno, "failed to create eventfd");
        return false;
    }

    // condition variable and mutex to wait for the lambda to be called
    volatile bool done = false;
    AICommon::Mutex mutex;
    AICommon::ConditionVariable cond;

    // lambda called from the event loop thread
    Handler lambda = [func, &done, &mutex, &cond](int fd, unsigned events)
    {
        // read the fd to stop if being called repeatdly
        uint64_t ignore;
        if (TEMP_FAILURE_RETRY(read(fd, &ignore, sizeof(ignore))) != sizeof(ignore))
        {
            AI_LOG_SYS_ERROR(errno, "failed to read eventfd");
        }

        // call the handler
        func();

        // wake the parent thread
        std::lock_guard<AICommon::Mutex> locker(mutex);
        done = true;
        cond.notify_all();
    };

    // install the handler on the eventfd which should trigger immediately
    int64_t id = addFileDescriptor(eventFd, EPollLoop::Readable, lambda);
    if (id < 0)
    {
        AI_LOG_ERROR("failed to add the eventfd");
        close(eventFd);
        return false;
    }

    // wait for the lambda to be called
    std::unique_lock<AICommon::Mutex> locker(mutex);
    while (!done)
    {
        if (cond.wait_for(locker, std::chrono::seconds(1)) == std::cv_status::timeout)
        {
            AI_LOG_ERROR("timed-out waiting for handler to be called by poll loop");
        }
    }

    // remove the eventfd watch and shut it down
    removeFileDescriptor(id);
    if (close(eventFd) != 0)
    {
        AI_LOG_SYS_ERROR(errno, "failed to close eventfd");
    }

    return true;
}

// -----------------------------------------------------------------------------
/**
 *  @brief The main epoll loop thread
 *
 *
 *
 */
void EPollLoop::pollLoop()
{
    AI_LOG_FN_ENTRY();

    // set the name of the thread, may need to truncate
    std::string threadName = mName;
    if (threadName.size() >= 16)
    {
        threadName.resize(15);
        AI_LOG_WARN("truncated thread name to '%s'", threadName.c_str());
    }
    pthread_setname_np(pthread_self(), threadName.c_str());

    // and (optionally) set the priority of the thread
    if (mPriority > 0)
    {
        struct sched_param param = { mPriority };
        pthread_setschedparam(pthread_self(), SCHED_RR, &param);
    }


    AI_LOG_MILESTONE("starting '%s' poll loop", mName.c_str());

    //
    struct epoll_event events[32];

    //
    bool stopLoop = false;
    while (!stopLoop)
    {
        // wait for any events
        int nEvents = TEMP_FAILURE_RETRY(epoll_wait(mEpollFd, events, 32, -1));
        if (nEvents < 0)
        {
            AI_LOG_SYS_ERROR(errno, "epoll_wait failed");
            continue;
        }

        // process the events
        for (int n = 0; n < nEvents; n++)
        {
            const int64_t tag = events[n].data.u64;
            const uint32_t flags = events[n].events;

            // -----------------------------------
            // check if a death event
            if (tag == 0)
            {
                // read the event fd to clear the state
                uint64_t ignore;
                if (TEMP_FAILURE_RETRY(read(mDeathEventFd, &ignore, sizeof(ignore))
                                       != sizeof(ignore)))
                {
                    AI_LOG_SYS_ERROR(errno, "failed to read death eventfd");
                }

                // set the stop flag and break out of the processing loop
                stopLoop = true;
                break;
            }
            else
            {
                // take the lock and then check for user installed callbacks
                std::lock_guard<std::recursive_mutex> locker(mEpollLock);

                // -----------------------------------
                // check if a generic fd
                std::map<int64_t, GenericDetails>::const_iterator it = mGenericFds.find(tag);
                if (it != mGenericFds.end())
                {
                    // convert the epoll event flags
                    unsigned eventFlags = 0;
                    if (flags & (EPOLLRDHUP | EPOLLERR | EPOLLHUP))
                        eventFlags |= Exception;
                    if (flags & EPOLLIN)
                        eventFlags |= Readable;
                    if (flags & EPOLLOUT)
                        eventFlags |= Writeable;

                    // call the handler
                    const int fd = it->second.fd;
                    const Handler &handler = it->second.handler;
                    if (handler)
                        handler(fd, eventFlags);
                }
                else
                {
                    // -----------------------------------
                    // check if a timer has expired
                    std::map<int64_t, TimerDetails>::const_iterator jt = mTimers.find(tag);
                    if (jt != mTimers.end())
                    {
                        // get the timerfd
                        const int fd = jt->second.fd;

                        // read the timer fd to clear the state
                        uint64_t ignore;
                        if (TEMP_FAILURE_RETRY(read(fd, &ignore, sizeof(ignore))
                                               != sizeof(ignore)))
                        {
                            // check if the timer read failed because it hasn't
                            // expired, this can happen if we stop the timer
                            // after epoll has woken us
                            if (errno == EAGAIN)
                            {
                                AI_LOG_INFO("timer woke without expiry");
                                continue;
                            }

                            AI_LOG_SYS_ERROR(errno, "failed to read timerfd");
                        }

                        // store the singleshot status
                        const bool singleShot = jt->second.singleShot;

                        // call the handler
                        const TimerHandler &handler = jt->second.handler;
                        if (handler)
                            handler(tag);

                        // check if the timer was a oneshot or not
                        if (singleShot)
                        {
                            // was a one-shot timer so remove it and close
                            // the file descriptor
                            mTimers.erase(tag);

                            if (epoll_ctl(mEpollFd, EPOLL_CTL_DEL, fd, NULL) != 0)
                                AI_LOG_SYS_ERROR(errno, "failed to delete timerfd from epoll");

                            if (close(fd) != 0)
                                AI_LOG_SYS_ERROR(errno, "failed to close timerfd");
                        }
                    }
                }


            }
        }
    }

    AI_LOG_MILESTONE("stopped '%s' poll loop", mName.c_str());

    AI_LOG_FN_EXIT();
}

