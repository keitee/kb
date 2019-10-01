/*
 * File:   timer_queue.cpp
 *
 * Copyright (C) BSKYB 2017+
 */

#include "timer_queue.h"

#include <list>

#include <poll.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <sys/eventfd.h>

#ifdef USE_HOST_BUILD

#include "slog.h"
#define AS_LOG_ERROR LOG_MSG
#define AS_LOG_SYS_ERROR(err, fmt) \
  LOG_MSG(fmt)

#else

#include "AS_Diag.h"
extern AS_DIAG::Context *dbusitf_logging_ctx;
#undef  AS_DIAG_CONTEXT_DEFAULT
#define AS_DIAG_CONTEXT_DEFAULT (dbusitf_logging_ctx)

#define AS_LOG_SYS_ERROR(err, fmt) \
    AS_LOG_ERROR(fmt " (%d)", err)

#endif


TimerQueue::TimerQueue()
    : mTimerFd(-1)
    , mEventFd(-1)
    , mExecCallbacks(false)
    , mInCallbackTag(-1)
    , mTagCounter(1)
{
    mTimerFd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC);
    if (mTimerFd < 0)
    {
        AS_LOG_SYS_ERROR(errno, "failed to create timerfd");
        return;
    }

    mEventFd = eventfd(0, EFD_CLOEXEC);
    if (mEventFd < 0)
    {
        AS_LOG_SYS_ERROR(errno, "failed to create eventFd");
        return;
    }

    mThread = std::thread(&TimerQueue::timerThread, this);
}

TimerQueue::~TimerQueue()
{
    // ensure the thread is stopped
    stop();

    // clean up the timer and event fds
    if ((mTimerFd >= 0) && (close(mTimerFd)) != 0)
        AS_LOG_SYS_ERROR(errno, "failed to close timerfd");
    if ((mEventFd >= 0) && (close(mEventFd)) != 0)
        AS_LOG_SYS_ERROR(errno, "failed to close eventfd");
}

// -----------------------------------------------------------------------------
/**
 *  @brief Stops the poll loop thread and cancels all timers
 *
 *
 *
 */
void TimerQueue::stop()
{
    // if the thread is still running, terminate by triggering the eventfd
    if (mThread.joinable())
    {
        // write to the eventfd to wake the poll loop
        uint64_t doesntMatter = 1;
        if (TEMP_FAILURE_RETRY(write(mEventFd, &doesntMatter, sizeof(uint64_t))) != sizeof(uint64_t))
        {
            AS_LOG_SYS_ERROR(errno, "failed to write to eventfd");
        }

        // wait for the thread to die
        mThread.join();
    }

    mTimersQueue.clear();
}

// -----------------------------------------------------------------------------
/**
 *  @brief Calculates the a new time value based on the time now and the
 *  supplied millisecond offset.
 *
 *  @param[in]  base        The base time to calculate the new offset from
 *  @param[in]  offset      The milliseconds offset
 *
 *  @return a timespec that is the base value plus the offset.
 */
struct timespec TimerQueue::calcAbsTime(const struct timespec& base,
                                        const std::chrono::milliseconds& offset) const
{
    #define NSECS_PER_SEC   1000000000L
    #define NSECS_PER_MSEC  1000000L

    struct timespec ts;

    ts.tv_sec = base.tv_sec + std::chrono::duration_cast<std::chrono::seconds>(offset).count();
    ts.tv_nsec = base.tv_nsec + ((offset % std::chrono::seconds(1)).count() * NSECS_PER_MSEC);

    if (ts.tv_nsec > NSECS_PER_SEC)
    {
        ts.tv_nsec -= NSECS_PER_SEC;
        ts.tv_sec += 1;
    }

    return ts;
}

// -----------------------------------------------------------------------------
/**
 *  @brief Adds a new timer to the timer queue
 *
 *
 *  @param[in]  timeout     The time after which to call the supplied handler.
 *  @param[in]  oneShot     If true the timer is automatically removed after
 *                          it times out the first time.
 *  @param[in]  handler     The handler function to call when the timer times
 *                          out.
 *
 *  @return on success returns a (greater than zero) timer tag integer which
 *  identifies the timer, on failure a negative value is returned.
 */
int64_t TimerQueue::add(const std::chrono::milliseconds& timeout, bool oneShot,
                        const std::function<bool()>& handler)
{
    // check the timer thread is running
    if (!mThread.joinable())
    {
        AS_LOG_ERROR("timer thread not running");
        return -1LL;
    }

    // get a new id, will fail if exhausted
    int64_t tag = mTagCounter++;

    // convert the time point to a *nix style timespec
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    // create the entry
    TimerEntry entry;
    entry.tag = tag;
    entry.oneshot = oneShot;
    entry.func = handler;
    entry.timeout = timeout;
    entry.expiry = calcAbsTime(now, timeout);

    // take the lock and push the timer into the prioity queue
    std::lock_guard<std::mutex> locker(mLock);

    std::multiset<TimerEntry, TimerEntryCompare>::iterator it = mTimersQueue.emplace(entry);

    // if the new timer was added to the head of the queue then update the
    // timerfd
    if (it == mTimersQueue.begin())
        updateTimerFd();

    return tag;
}

// -----------------------------------------------------------------------------
/**
 *  @brief Removes the given timer from the timer queue
 *
 *  Once this method returns (successfully) you are guaranteed that the timer
 *  handler will not be called, i.e. this is synchronisation point.
 *
 *  This method can be called from the context of a timer handler, however if
 *  you want to cancel a repeating timer then the recomeneded way is to just
 *  return false in the handler.
 *
 *  @param[in]  tag         The tag of the timer to remove as returned by the
 *                          add() method
 *
 *  @return true if the timer was found and was removed from the queue,
 *  otherwise false.
 */
bool TimerQueue::remove(int64_t tag)
{
    // sanity check the tag number
    if (tag <= 0)
    {
        AS_LOG_ERROR("invalid timer tag number %lld", tag);
        return false;
    }

    // take the lock and try and find the timerid in the queue
    std::unique_lock<std::mutex> locker(mLock);

    // check the timer thread is running
    if (!mThread.joinable())
    {
        AS_LOG_ERROR("timer thread not running");
        return false;
    }

    // removing a timer gets a bit complicated because TimerQueue::remove() is
    // allowed to be called from a timer callback from the timer being removed
    // or another one

    // so first check is to see if the thread is not currently executing any
    // callbacks
    if (mExecCallbacks)
    {
        // we are executing the callbacks so add the timer to the remove set,
        // this means the thread will remove the timer after executing the
        // callbacks
        mRemovedSet.insert(tag);

        // the next special case is if the timer callback currently being called
        // is the one we are removing, because we want to gaurantee that on
        // the return of this function the callback will never be called
        // (if being called from the timer thread then we will deadlock waiting
        // on the conditional, in such situations it's safe just to add the
        // timer to the removed set).
        if ((mInCallbackTag == tag) && (std::this_thread::get_id() != mThread.get_id()))
        {
            // wait till the callback completes
            while (mInCallbackTag == tag)
            {
                mCallbackComplete.wait(locker);
            }
        }

        // unfortunatly we don't actually know if the tag was valid or not at
        // this point, so for now just assume it was ... FIXME
        return true;
    }
    else
    {
        // not currently executing any callbacks so we can just call doRemove
        return doRemove(tag);
    }
}

// -----------------------------------------------------------------------------
/**
 *  @brief Removes the time with tag from the timer queue
 *
 *  This method must be called with the mLock held.
 *
 *  @param[in]  tag         The tag of the timer to remove as returned by the
 *                          add() method
 *
 *  @return true if the timer was found and was removed from the queue,
 *  otherwise false.
 */
bool TimerQueue::doRemove(int64_t tag)
{
    // find the timer in the queue
    std::multiset<TimerEntry, TimerEntryCompare>::iterator it = mTimersQueue.begin();
    for (; it != mTimersQueue.end(); ++it)
    {
        if (it->tag == tag)
        {
            // if we removing the item from the head of the queue then need to
            // update the expiry time in the timerfd
            bool requiresUpdate = (it == mTimersQueue.begin());

            // remove the timer from the queue
            mTimersQueue.erase(it);

            // update the timerfd if required
            if (requiresUpdate)
                updateTimerFd();

            return true;
        }
    }

    AS_LOG_ERROR("failed to find timer with tag %lld to remove", tag);
    return false;
}

// -----------------------------------------------------------------------------
/**
 *  @brief Writes the item on the head of the expiry queue into the timerfd
 *  for the next wake-up time
 *
 *
 *
 */
void TimerQueue::updateTimerFd() const
{
    struct itimerspec its;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;

    if (mTimersQueue.empty())
    {
        // this will disable the timerfd
        its.it_value.tv_sec = 0;
        its.it_value.tv_nsec = 0;
    }
    else
    {
        // set it's expiry value
        its.it_value = mTimersQueue.begin()->expiry;
    }

    if (timerfd_settime(mTimerFd, TFD_TIMER_ABSTIME, &its, NULL) != 0)
    {
        AS_LOG_SYS_ERROR(errno, "failed to set timerfd value");
    }
}

// -----------------------------------------------------------------------------
/**
 *  @brief The thread function that runs the timer poll loop
 *
 *  This simply polls on an timerfd and eventfd.  The timerfd is obviously for
 *  waking up and calling any installed timers at the right time.  The eventfd
 *  is used to kill the poll loop at shutdown time.
 *
 *
 */
void TimerQueue::timerThread()
{
    pthread_setname_np(pthread_self(), "BT_TimerThread");

    if (mTimerFd < 0)
    {
        AS_LOG_ERROR("no timerfd available");
        return;
    }
    if (mEventFd < 0)
    {
        AS_LOG_ERROR("no eventfd available");
        return;
    }

    while (true)
    {
        struct pollfd fds[2];

        fds[0].fd = mEventFd;
        fds[0].events = POLLIN;
        fds[0].revents = 0;

        fds[1].fd = mTimerFd;
        fds[1].events = POLLIN;
        fds[1].revents = 0;

        int ret = TEMP_FAILURE_RETRY(poll(fds, 2, -1));
        if (ret < 0)
        {
            AS_LOG_SYS_ERROR(errno, "poll failed");
        }
        else if (ret > 0)
        {
            // check if the eventfd has fired
            if (fds[0].revents != 0)
            {
                // check for any error conditions
                if (fds[0].revents & (POLLERR | POLLHUP | POLLNVAL))
                {
                    AS_LOG_ERROR("received error events on eventfd (0x%04x)",
                                 fds[0].revents);
                }

                // read the eventfd to clear the value
                uint64_t ignore;
                if (TEMP_FAILURE_RETRY(read(mEventFd, &ignore, sizeof(uint64_t))) != sizeof(uint64_t))
                {
                    AS_LOG_SYS_ERROR(errno, "failed to read from eventfd");
                }

                // break out of the poll loop
                break;
            }

            // check if the timerfd fired
            if (fds[1].revents != 0)
            {
                // check for any error conditions
                if (fds[1].revents & (POLLERR | POLLHUP | POLLNVAL))
                {
                    AS_LOG_ERROR("received error events on timerfd (0x%04x)",
                                 fds[1].revents);
                }

                // read the timerfd to clear the value
                uint64_t ignore;
                if (TEMP_FAILURE_RETRY(read(mTimerFd, &ignore, sizeof(uint64_t))) != sizeof(uint64_t))
                {
                    AS_LOG_SYS_ERROR(errno, "failed to read from timerfd");
                }


                // get the current monotonic time and check if anyone has expired
                struct timespec now;
                clock_gettime(CLOCK_MONOTONIC, &now);


                // take the lock and then move all the expired timers into a
                // separate set for processing, we then optionally put any back
                // that want to be rescheduled
                std::unique_lock<std::mutex> locker(mLock);

                // put a marker here so that if anyone calls TimerQueue::remove
                // then the timer to remove is added to the mRemovedSet rather
                // than looking up in the mTimersQueue
                mExecCallbacks = true;

                std::list<TimerEntry> expired;
                std::multiset<TimerEntry, TimerEntryCompare>::iterator it = mTimersQueue.begin();
                while ((it != mTimersQueue.end()) && (it->isLessThanOrEqualTo(now)))
                {
                    // put in the expired set and remove from the master set
                    expired.emplace_back(std::move(*it));
                    it = mTimersQueue.erase(it);
                }

                // go through the expired timers and call their handlers, we
                // then reschedule the non-removed / non-oneshot timers
                for (TimerEntry entry : expired)
                {
                    // quick check the timer hasn't been removed
                    if (mRemovedSet.count(entry.tag) != 0)
                    {
                        // in the removed set so skip this callback
                        continue;
                    }

                    // set the callback we're within and then drop the lock
                    mInCallbackTag = entry.tag;
                    locker.unlock();

                    // call the callback function, if it returns true and it's
                    // not a one shot timer then reschedule
                    bool reschedule = entry.func && entry.func() && !entry.oneshot;

                    // re-take the lock and clear the callback we're within
                    locker.lock();
                    mInCallbackTag = -1;

                    // signal the conditional that TimerQueue::remove may be
                    // waiting on
                    mCallbackComplete.notify_all();

                    // reschedule the timer if needed and not in the removed set
                    if (reschedule && (mRemovedSet.count(entry.tag) == 0))
                    {
                        entry.expiry = calcAbsTime(now, entry.timeout);
                        mTimersQueue.emplace(std::move(entry));
                    }
                }

                // finished executing any callbacks, so check the mRemovedSet
                // and if not empty then we need to remove those timers from
                // the queue now
                mExecCallbacks = false;

                for (int64_t tag : mRemovedSet)
                {
                    doRemove(tag);
                }

                // finally update the timerfd for the next item that is on the
                // head of the timer queue (if any)
                updateTimerFd();
            }
        }
        
    }
    
}

