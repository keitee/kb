/* ={==========================================================================
reference code:
AppInfrastructure/Common/lib/source/PollLoop.cpp
AppInfrastructure/InputHandler/test/source/EPollLoopTests.cpp
AppInfrastructure/InputHandler/lib/source/EPollLoop.cpp

TODO:
not able to find real use of EPOLLDEFERRED in the reference code tree and may be
no real use case.

that means that readline and pollloop are only for using readline library
feature.

*/

#include "pollloop.h"
// #include "lpi_error.h"
#include "slog.h"
#include <fcntl.h>
#include <map>
#include <memory>
#include <mutex>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/syscall.h> /* For SYS_xxx definitions */
#include <sys/timerfd.h>
#include <unistd.h>

namespace
{
  // to convert ms to us
  const int MILLISEC_PER_NANOSEC{1000L * 1000L};
} // namespace

/* ={==========================================================================
@param[in] long deffered_time_interval  the time interval in milliseconds

struct timespec {
    time_t tv_sec;                // Seconds
    long   tv_nsec;               // Nanoseconds
};

struct itimerspec {
    struct timespec it_interval;  // Interval for periodic timer
    struct timespec it_value;     // Initial expiration
};

"value" is initial value to expire and use "interval" after that

*/
PollLoop::PollLoop(const std::string &name,
                   unsigned int max_source,
                   long deferred_time_interval)
    : m_name(name)
    , m_threadid(-1)
    , m_epollfd(-1)
    , m_max_source(max_source)
    , m_defer_timespec({.it_interval = {.tv_sec  = 0,
                                        .tv_nsec = (deferred_time_interval *
                                                    MILLISEC_PER_NANOSEC)},
                        .it_value    = {.tv_sec  = 0,
                                     .tv_nsec = (deferred_time_interval *
                                                 MILLISEC_PER_NANOSEC)}})
{
  LOG_MSG("check on EPOLLWAKEUP {%u}", EPOLLWAKEUP);
  LOG_MSG("check on EPOLLDEFERRED {%u}", EPOLLDEFERRED);
  LOG_MSG("pollloop is ctored");
}

PollLoop::~PollLoop()
{
  stop();

  LOG_MSG("pollloop is dtored");
}

void PollLoop::enableDeferredTimer_()
{
  // TODO
  // Should be called with the lock held, sanity check on debug builds
  // #if (AI_BUILD_TYPE == AI_DEBUG)
  //     if (mLock.try_lock() == true)
  //     {
  //         AI_LOG_ERROR("mutex lock not held in %s", __FUNCTION__);
  //         mLock.unlock();
  //     }
  // #endif

  // if timerfd was created
  if (m_defer_timerfd >= 0)
  {
    // set new time and
    if (timerfd_settime(m_defer_timerfd,
                        TFD_TIMER_ABSTIME,
                        &m_defer_timespec,
                        nullptr) < 0)
    {
      LOG_MSG("failed to set time to timerfd");
    }
    else
    {
      // print in ms (10-3)
      LOG_MSG("deferred time {interval:%0.03f, value:%0.03f}",
              (m_defer_timespec.it_interval.tv_sec * 1000.f) +
                (m_defer_timespec.it_interval.tv_nsec / 1000000.0f),
              (m_defer_timespec.it_value.tv_sec * 1000.f) +
                (m_defer_timespec.it_value.tv_nsec / 1000000.0f));
    }
  }
}

void PollLoop::disableDeferredTimer_()
{
  // TODO
  // Should be called with the lock held, sanity check on debug builds
  // #if (AI_BUILD_TYPE == AI_DEBUG)
  //     if (mLock.try_lock() == true)
  //     {
  //         AI_LOG_ERROR("mutex lock not held in %s", __FUNCTION__);
  //         mLock.unlock();
  //     }
  // #endif

  if (m_defer_timerfd >= 0)
  {
    struct itimerspec spec_{};

    // bzero() is deprecated so use memset
    //
    // why no error on casting?
    // memset(reinterpret_cast<void *>(&spec_), 0, sizeof(spec_));
    memset(&spec_, 0, sizeof(spec_));

    if (timerfd_settime(m_defer_timerfd, 0, &spec_, nullptr) < 0)
    {
      LOG_MSG("failed to disble time to timerfd");
    }
    else
    {
      LOG_MSG("timerfd is disabled");
    }
  }
}

/* ={==========================================================================
poll thread function which does all the epoll stuff
*/
void PollLoop::run_(const std::string &name, int priority)
{
  // save up thread id
  m_threadid.store(syscall(SYS_gettid));

  // TODO: skips the code to block SIGPIPE. what would happen then?

  // 0-99 : realtime priority
  if (priority > 0)
  {
    struct sched_param param_
    {
      priority
    };

    pthread_setschedparam(pthread_self(), SCHED_RR, &param_);
  }

  // allocates event list array. use calloc since max num of sources is given
  // from ctor.
  struct epoll_event *events_ = reinterpret_cast<struct epoll_event *>(
    calloc(m_max_source, sizeof(struct epoll_event)));

  // map of all the sources that are triggerred.
  std::map<std::shared_ptr<IPollSource>, uint32_t> triggered_{};

  unsigned int failures_{};
  bool done{false};

  LOG_MSG("pollloop loop runs\n");

  while (false == done)
  {
    // On success, epoll_wait() returns `the number of items` that have been
    // placed in the array evlist, or 0 if no file descriptors were ready within
    // the interval specified by timeout. On error, epoll_wait() returns -1,
    // with errno set to indicate the error.

    auto num =
      TEMP_FAILURE_RETRY(epoll_wait(m_epollfd, events_, m_max_source, -1));
    if (num < 0)
    {
      LOG_MSG("epoll_wait failed");

      if (++failures_ > 5)
      {
        LOG_MSG("too many errors occurred on epoll. shutting down the lopp");
        break;
      }
    }

    // iterate through all the events that are ready
    for (int i = 0; i < num; ++i)
    {
      // gets an event
      const struct epoll_event &event = events_[i];

      // when death eventfd is fired
      if (event.data.fd == m_death_eventfd)
      {
        done = true;
        break;
      }
      // when timer is fired
      else if (event.data.fd == m_defer_timerfd)
      {
        uint64_t expiration{};

        // read the timer to clear the expire count
        if (sizeof(expiration) !=
            TEMP_FAILURE_RETRY(
              read(m_defer_timerfd, &expiration, sizeof(expiration))))
        {
          LOG_MSG("failed to read timerfd");
        }

        // take the lock to access m_sources
        std::lock_guard<SpinLock> lock(m_lock);

        // add any deferred source to the "triggerred" list
        for (const auto &e : m_sources)
        {
          if (e.m_events & EPOLLDEFERRED)
          {
            auto source_ = e.m_source.lock();
            if (source_)
            {
              LOG_MSG("source{%s} has DEFERRED and added to triggered set",
                      e.m_name.c_str());

              triggered_[source_] |= EPOLLDEFERRED;
            }
          }
        }
      }
      // another event. iterate through the sources, check, and add it to the
      // "triggerred" list
      else
      {
        // take the lock to access m_sources
        std::lock_guard<SpinLock> lock(m_lock);

        for (const auto &e : m_sources)
        {
          if (event.data.fd == e.m_fd)
          {
            // TODO Perform another check to see if the events epoll gave us
            // still "match" the ones in the PollSource set. These can get out of
            // sync due to (valid) race conditions between epoll wake-up and
            // taking the "lock"

            if (event.events & (e.m_events | EPOLLRDHUP | EPOLLERR | EPOLLHUP))
            {
              // get sp from wp of source
              auto source_ = e.m_source.lock();
              if (source_)
              {
                triggered_[source_] |= event.events;
              }
              else
              {
                // TODO Failed to get the shared_ptr, should we remove
                // it from the list of sources ?
                LOG_MSG("failed to get source shared ptr");
              }
            }
          }
        }
      }
    } // for

    // Ok, have all triggered events, triggerred_, and let's process it.
    //
    // m_lock is no longer held which is ok as we now have a list of shared
    // pointers and their events, triggerred_, ensuring other threads can now
    // add / delete sources without affecting us

    // iterate through "triggerred" sources and call process() with the events
    // triggerred so let them process the events.
    // std::map<std::shared_ptr<IPollSource>, uint32_t> triggered_{};
    for (const auto &e : triggered_)
    {
      e.first->process(shared_from_this(), e.second);
    }

    // ok, we're done and clear it off.
    triggered_.clear();

    // go back around the loop
  } // while

  LOG_MSG("pollloop run ends");

  // free events
  free(events_);

  // clear thread id
  m_threadid.store(-1);
}

/* ={==========================================================================
o interestingly, sources in m_sources are added to epool either from
addSource() or start().

o if the poll loop is already running, it is stopped and restarted.

o start the poll thread

o parm priority
  The SCHED_RR in which to run the poll thread. -1 then it is inherited from
  the calling thread.

*/
bool PollLoop::start(int priority /* priority = -1 */)
{
  // call stop() in case it's already running
  stop();

  // init value 0 and use semaphore flag
  m_death_eventfd = eventfd(0, EFD_CLOEXEC | EFD_SEMAPHORE);
  if (m_death_eventfd < 0)
  {
    // to show "errno" as well
    LOG_MSG("failed to create death eventfd");
    goto eventfd_failed;
  }

  m_defer_timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC);
  if (m_defer_timerfd < 0)
  {
    LOG_MSG("failed to create defer timerfd");
    goto timerfd_failed;
  }

  m_epollfd = epoll_create1(EPOLL_CLOEXEC);
  if (m_epollfd < 0)
  {
    LOG_MSG("failed to create epoll");
    goto epollfd_failed;
  }

  // add eventfd and timerfd to the epoll

  struct epoll_event event_;

  // seems unnecessary since fills up event struct anyway.
  // memset(&event, 0, sizeof (event));

  event_.data.fd = m_death_eventfd;
  event_.events  = EPOLLIN;

  if (epoll_ctl(m_epollfd, EPOLL_CTL_ADD, m_death_eventfd, &event_) < 0)
  {
    LOG_MSG("failed to add death eventfd to the epoll");
    goto epolladd_failed;
  }

  event_.data.fd = m_defer_timerfd;
  event_.events  = EPOLLIN;

  if (epoll_ctl(m_epollfd, EPOLL_CTL_ADD, m_defer_timerfd, &event_) < 0)
  {
    LOG_MSG("failed to add deferred timerfd to the epoll");
    goto epolladd_failed;
  }

  // add all sources if there are to the epoll
  {
    std::lock_guard<SpinLock> lock(m_lock);

    if (m_sources.size())
    {
      for (const auto &e : m_sources)
      {
        // if source is still around from weak pointer, then add it to the epoll
        auto source_ = e.m_source.lock();
        if (source_)
        {
          event_.data.fd = e.m_fd;
          event_.events  = e.m_events & (EPOLLIN | EPOLLOUT | EPOLLRDHUP);

          if (epoll_ctl(m_epollfd, EPOLL_CTL_ADD, e.m_fd, &event_) < 0)
          {
            LOG_MSG("failed to add fd to epoll");
            // but do not return since it's in the loop.
          }
        }
      } // for

      // if any of the sources are deferred from add/mod/delSource() then
      // start the timerfd.

      if (m_deferred_sources > 0)
        enableDeferredTimer_();
    }
  } // lock

  // finally spawn the thread

  m_thread = std::thread(&PollLoop::run_, this, m_name, priority);
  if (false == m_thread.joinable())
  {
    LOG_MSG("failed to create a epoll thread");
    // TODO: Q: should be "goto epolladd_failed:"
    goto epollfd_failed;
  }

  return true;

// "goto"
epolladd_failed:
  close(m_epollfd);
  m_epollfd = -1;

epollfd_failed:
  close(m_defer_timerfd);
  m_defer_timerfd = -1;

timerfd_failed:
  close(m_death_eventfd);
  m_death_eventfd = -1;

eventfd_failed:
  return false;
}

/* ={==========================================================================
stop the poll loop and clean up resources. have covered case when stop() called
more than once.

*/
void PollLoop::stop()
{
  // signal eventfd which should cause the epoll thread to drop out
  if (m_death_eventfd >= 0)
  {
    uint64_t set_{1};

    if (sizeof(set_) !=
        TEMP_FAILURE_RETRY(write(m_death_eventfd, &set_, sizeof(set_))))
    {
      LOG_MSG("failed to write on eventfd");
    }
    else if (m_thread.joinable())
    {
      m_thread.join();
      LOG_MSG("pollloop thread ends");
    }

    close(m_death_eventfd);
    m_death_eventfd = -1;
  }

  if (m_defer_timerfd >= 0)
  {
    close(m_defer_timerfd);
    m_defer_timerfd = -1;
  }

  if (m_epollfd >= 0)
  {
    close(m_epollfd);
    m_epollfd = -1;
  }
}

/* ={==========================================================================
A source is a file descriptor, a bitmask of events to wait for and a
IPollSource object that will be called when any of the events in the bitmask
occur on the file descriptor.

This method may fail if the number of sources installed exceeds the maximum
allowed.

@param[in]  source     The source object to call process() on when an event
                       occurs
@param[in]  fd         The file descriptor to poll on
@param[in]  events     A bitmask of events to listen on

o added "name" for logging which is not in the reference

*/
bool PollLoop::addSource(const std::string &name,
                         const std::shared_ptr<IPollSource> &source,
                         int fd,
                         uint32_t events)
{
  // 5.2 File Control Operations: fcntl()
  //
  // The fcntl() system call performs a range of control operations on an open
  // file descriptor.
  //
  // NOTE on "open file descriptor"
  //
  // https://stackoverflow.com/questions/12340695/how-to-check-if-a-given-file-descriptor-stored-in-a-variable-is-still-valid
  //
  // fcntl(fd, F_GETFD) is the canonical cheapest way to check that fd is a
  // valid open file descriptor. If you need to batch-check a lot, using poll
  // with a zero timeout and the events member set to 0 and checking for
  // POLLNVAL in revents after it returns is more efficient.

  if ((fd < 0) || (fcntl(fd, F_GETFD) < 0))
  {
    LOG_MSG("invalid file desc");
    return false;
  }

  std::lock_guard<SpinLock> lock(m_lock);

  if (m_sources.size() >= (m_max_source - 2))
  {
    LOG_MSG("too many epoll sources");
    return false;
  }

  // check if event bit flag is one of them in the set
  events &= (EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLDEFERRED);

  // store the source
  m_sources.push_back(PollSourceWrapper(name, source, fd, events));

  // check if it has the deffered flag set.
  // TODO Q: why since it sets up above anyway?
  auto source_ = m_sources.back();

  if (source_.m_events & EPOLLDEFERRED)
  {
    if (++m_deferred_sources == 1)
    {
      LOG_MSG("source{%s} has enabled DEFERRED timer", source_.m_name.c_str());
      enableDeferredTimer_();
    }
  }

  // add it to epoll
  if (m_epollfd >= 0)
  {
    struct epoll_event event_;

    event_.data.fd = source_.m_fd;
    event_.events  = source_.m_events & (EPOLLIN | EPOLLOUT | EPOLLRDHUP);

    if (epoll_ctl(m_epollfd, EPOLL_CTL_ADD, fd, &event_) < 0)
    {
      LOG_MSG("failed to add fd to epoll");
      m_sources.pop_back();
      return false;
    }
  }

  return true;
}

/* ={==========================================================================
Modifies the events bitmask for the source
*/
bool PollLoop::modSource(const std::shared_ptr<IPollSource> &source,
                         uint32_t events)
{
  // TODO:
  return true;
}

/* ={==========================================================================
It's important to note that even after the source has been removed and this
function returns, it's possible for the source's process() method to be called.
This is because "triggered_" that is used to process events can be different
from "m_sources" which is protected; that is triggerred_ is not protected.

*/

bool PollLoop::delSource(const std::shared_ptr<IPollSource> &source)
{
  std::lock_guard<SpinLock> lock(m_lock);

  auto it = m_sources.begin();
  auto last = m_sources.end();

  for (; it != last; ++it)
  {
    // only when found the source to remove
    if (it->m_source.lock() == source)
    {
      if (it->m_events & EPOLLDEFERRED)
      {
        LOG_MSG("DEFERRED is set");

        if (--m_deferred_sources == 0)
        {
          LOG_MSG("source{%s} has disabled DEFERRED timer", it->m_name.c_str());
          disableDeferredTimer_();
        }
      }

      if (m_epollfd > 0)
      {
        if (epoll_ctl(m_epollfd, EPOLL_CTL_DEL, it->m_fd, nullptr) < 0)
        {
          LOG_MSG("failed to delete source{%s} from epoll", it->m_name.c_str());
          // may use LOG_EXIT()
          return false;
        }
      }

      m_sources.erase(it);
      // no need to continue searching so return.
      return true;
    }
  }

  // since it failed to find match
  LOG_MSG("failed to find source{%s} from epoll", it->m_name.c_str());
  return false;
}
