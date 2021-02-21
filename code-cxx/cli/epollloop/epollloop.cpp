#include "epollloop.h"
#include "slog.h"

#include <iostream>
#include <sys/epoll.h>
#include <sys/eventfd.h>
// #include <sys/syscall.h> /* For SYS_xxx definitions */
#include <condition_variable>
#include <mutex>
#include <sys/timerfd.h>
#include <unistd.h>

/* ={==========================================================================
*/
EPollLoop::EPollLoop(const std::string &name, int priority)
    : m_name(name)
    , m_priority(priority)
    , m_epollfd(-1)
    , m_death_eventfd(-1)
    , m_tag_counter(1)
{
  // > TFD_TIMER_ABSTIME {1}
  // LOG_MSG("> TFD_TIMER_ABSTIME {%d}", TFD_TIMER_ABSTIME);
}

EPollLoop::~EPollLoop()
{
  if (m_thread.joinable())
    stop();

  if ((m_epollfd > 0) && (close(m_epollfd) < 0))
  {
    LOG_MSG("failed to close epoll in epoll {%s}", m_name.c_str());
  }

  if ((m_death_eventfd > 0) && (close(m_death_eventfd) < 0))
  {
    LOG_MSG("failed to close eventfd in epoll {%s}", m_name.c_str());
  }

  cleanupTimers_();
}

/* ={==========================================================================
*/
void EPollLoop::run_()
{
  // set the name of the thread within the limit
  // std::string name_{m_name};

  std::string name_ = m_name;

  if (name_.size() >= 16)
  {
    name_.resize(15);
    LOG_MSG("set thread name{%s} in epoll {%s}", name_.c_str(), m_name.c_str());
  }
  pthread_setname_np(pthread_self(), name_.c_str());

  // set priority
  if (m_priority > 0)
  {
    struct sched_param param_
    {
      m_priority
    };
    pthread_setschedparam(pthread_self(), SCHED_RR, &param_);
  }

  LOG_MSG("starts thread name{%s} in epoll {%s}",
          name_.c_str(),
          m_name.c_str());

  std::cout << "thread name : " << name_ << ", m_name : " << m_name
            << std::endl;

  struct epoll_event events_[32]{};

  bool exit_{};
  while (!exit_)
  {
    // wait for any events
    // If timeout equals -1, block until an event occurs for one of the file
    // descriptors in the interest list for epfd or until a signal is caught.

    auto nums_ = TEMP_FAILURE_RETRY(epoll_wait(m_epollfd, events_, 32, -1));
    if (nums_ < 0)
    {
      LOG_MSG("epoll wait failed in epoll {%s}", m_name.c_str());
      continue;
    }

    // process events
    for (int i = 0; i < nums_; ++i)
    {
      // get tag and flag
      const auto tag_   = events_[i].data.u64;
      const auto flags_ = events_[i].events;

      // process eventfd
      if (tag_ == 0)
      {
        uint64_t noneed_{};
        if (TEMP_FAILURE_RETRY(
              read(m_death_eventfd, &noneed_, sizeof(noneed_))) < 0)
        {
          // NOTE: really happen? Here, end poll thread regardless.
          LOG_MSG("failed to read eventfd in epoll {%s}", m_name.c_str());
        }

        exit_ = true;
        break;
      }
      // process the rest
      else
      {
        std::lock_guard<std::recursive_mutex> lock(m_lock);

        auto it = m_handlers.find(tag_);

        // process eventfd from executeInPollloop()
        if (m_handlers.end() != it)
        {
          // convert epoll flags
          unsigned events_{};

          if (flags_ & (EPOLLRDHUP | EPOLLERR | EPOLLHUP))
            events_ |= Exception;
          if (flags_ & EPOLLIN)
            events_ |= Readable;
          if (flags_ & EPOLLOUT)
            events_ |= Writeable;

          // call the handler which is lambda in executeInPollLoop()
          const auto fd_ = it->second.fd;
          const auto call_ = it->second.handler;
          if (call_)
            call_(fd_, events_);
        }
        // process timer fds
        else
        {
          const auto it = m_timers.find(tag_);
          if (m_timers.cend() != it)
          {
            // read timerfd
            const auto fd_ = it->second.fd;

            // not needed since it's # of expirations that have occurred.
            uint64_t noneed_{};
            if (TEMP_FAILURE_RETRY(read(fd_, &noneed_, sizeof(noneed_))) < 0)
            {
              // NOTE: check if the timer read failed because it hasn't
              // expired, this can happen if we stop the timer
              // after epoll has woken us. REALLY??

              if (EAGAIN == errno)
              {
                LOG_MSG("timer woke up without expiry in epoll {%s}",
                        m_name.c_str());
                continue;
              }

              LOG_MSG("failed to read timer in epoll {%s}", m_name.c_str());
            }

            // debug
            // LOG_MSG("> tag {%d}, fd {%d}", tag_, fd_);

            // call the handler
            const auto handler_ = it->second.handler;
            if (handler_)
              handler_(tag_);

            // single or not?
            const auto single_ = it->second.singleShot;
            if (single_)
            {
              m_timers.erase(tag_);

              if (epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd_, nullptr) < 0)
              {
                LOG_MSG("failed to delete timer in epoll {%s}", m_name.c_str());
              }

              if (0 != close(fd_))
              {
                LOG_MSG("failed to close timer in epoll {%s}", m_name.c_str());
              }
            }
          }
        }
      } // else for process events
    }
  } // while
}

/* ={==========================================================================
*/
bool EPollLoop::start()
{
  if (m_thread.joinable())
  {
    LOG_MSG("epoll loop {%s} is already running so return", m_name.c_str());
    // note that return "true" than "false"
    return true;
  }

  // init value 0 and use semaphore flag
  // TODO: EFD_SEMAPHORE. is it useful?
  m_death_eventfd = eventfd(0, EFD_CLOEXEC | EFD_SEMAPHORE);
  if (m_death_eventfd < 0)
  {
    LOG_MSG("failed to create eventfd for epoll {%s}", m_name.c_str());
    return false;
  }

  // create epoll
  m_epollfd = epoll_create1(EPOLL_CLOEXEC);
  if (m_epollfd < 0)
  {
    // NOTE: may use logging with printing errno
    LOG_MSG("failed to create epoll {%s}", m_name.c_str());

    // NOTE: clean up. pollloop.cpp uses "goto" instead
    close(m_death_eventfd);
    m_death_eventfd = -1;
    return false;
  }

  // add eventfd to epoll with tag(id) num 0
  struct epoll_event event_;
  event_.data.u64 = 0;
  event_.events   = EPOLLIN;

  if (epoll_ctl(m_epollfd, EPOLL_CTL_ADD, m_death_eventfd, &event_) < 0)
  {
    LOG_MSG("failed to add evnetfd to epoll {%s}", m_name.c_str());

    close(m_death_eventfd);
    m_death_eventfd = -1;
    close(m_epollfd);
    m_epollfd = -1;

    return false;
  }

  // spawn the epoll thread
  m_thread = std::thread(&EPollLoop::run_, this);
  if (!m_thread.joinable())
  {
    LOG_MSG("failed to create epoll thread {%s}", m_name.c_str());

    close(m_death_eventfd);
    m_death_eventfd = -1;
    close(m_epollfd);
    m_epollfd = -1;

    return false;
  }

  return true;
}

/* ={==========================================================================
Like void PollLoop::stop(), stop() supports that it can be called multiple times
but here uses joinable() check first instead of using if checks.

*/
void EPollLoop::stop()
{
  // epoll loop do not run and no epoll thread to stop.
  if (!m_thread.joinable())
  {
    LOG_MSG("epoll loop {%s} is not running so return", m_name.c_str());
    return;
  }

  // NOTE: this is unlikely. necessary?
  if (m_death_eventfd < 0)
  {
    LOG_MSG("invalid eventfd in epoll {%s}. this is bad", m_name.c_str());
  }
  else
  {
    uint64_t set_{1};
    if (sizeof(set_) !=
        TEMP_FAILURE_RETRY(write(m_death_eventfd, &set_, sizeof(set_))))
    {
      LOG_MSG("failed to write death eventfd in epoll {%s}", m_name.c_str());
    }
    // no need to check .joinable() since it's done above.
    else
    {
      m_thread.join();
    }

    if (close(m_death_eventfd) < 0)
    {
      LOG_MSG("failed to close death eventfd in epoll {%s}", m_name.c_str());
    }

    m_death_eventfd = -1;
  }

  // close epoll. close() returns 0 on success as with other system calls.
  if ((m_epollfd >= 0) && (0 != close(m_epollfd)))
  {
    LOG_MSG("failed to close epoll {%s}", m_name.c_str());
  }

  m_epollfd = -1;

  // TODO: remove any installed handlers
  // m_generic_fds.clear();

  // remove any timers
  cleanupTimers_();
}

/* ={==========================================================================
start a single shot timer that will call the given handler
note that "timeout" is in nano and conversion is done when pass milliseconds.
*/

bool EPollLoop::startSingleShotTimer(const std::chrono::nanoseconds &timeout,
                                     const TimerHandler &handler)
{
  if (timeout.count() < 0)
  {
    LOG_MSG("timer duration must be positive in epoll {%s}", m_name.c_str());
    return false;
  }

  auto fd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
  if (fd < 0)
  {
    LOG_MSG("faild to create timer in epoll {%s}", m_name.c_str());
    return false;
  }

  struct itimerspec spec_{};

  // since it's single shot, set initial value only.
  spec_.it_value.tv_sec =
    std::chrono::duration_cast<std::chrono::seconds>(timeout).count();
  spec_.it_value.tv_nsec = (timeout % std::chrono::seconds(1)).count();

  // we allow the user to pass a zero timeout, however that will disable the
  // timerfd so we make the timeout have at least 1ns which is close enough
  if ((spec_.it_value.tv_sec == 0) && (spec_.it_value.tv_nsec == 0))
  {
    spec_.it_value.tv_nsec = 1;
  }

  // not explicit set spec_.it_interval since it's set using list list, {}.

  // debug
  // std::cout << "sec: " << spec_.it_value.tv_sec
  //           << ", nsec : " << spec_.it_value.tv_nsec << std::endl;

  // NOTE:
  // when use TFD_TIMER_ABSTIME, it appears that timers gets expired in the
  // order of creation regardless of timeout setting. Instead, using 0 works as
  // expected but that's not in man page. HOW?
  //
  // if (0 != timerfd_settime(fd, TFD_TIMER_ABSTIME, &spec_, nullptr))

  if (0 != timerfd_settime(fd, 0, &spec_, nullptr))
  {
    LOG_MSG("faild to set timer in epoll {%s}", m_name.c_str());
    return false;
  }

  // add a timer to epoll. not use timer id since it starts right away.
  if (installTimer_(fd, spec_, handler, true) < 0)
  {
    if (0 != close(fd))
    {
      LOG_MSG("faild to close timer in epoll {%s}", m_name.c_str());
    }
  }

  return true;
}

/* ={==========================================================================
add a periodic timer to the poll but not start it
*/

int64_t EPollLoop::addTimer(const std::chrono::nanoseconds &initial,
                         const std::chrono::nanoseconds &interval,
                         const TimerHandler &handler)
{
  if (interval.count() < 0)
  {
    LOG_MSG("timer interval must be positive in epoll {%s}", m_name.c_str());
    return false;
  }

  auto fd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
  if (fd < 0)
  {
    LOG_MSG("faild to create timer in epoll {%s}", m_name.c_str());
    return false;
  }

  // NOTE: it didn't work
  // struct itimerspec spec_
  // {
  //   {std::chrono::duration_cast<std::chrono::seconds>(initial).count(),
  //    (initial % std::chrono::seconds(1)).count()},
  //   {
  //     std::chrono::duration_cast<std::chrono::seconds>(interval).count(),
  //       (interval % std::chrono::seconds(1)).count()
  //   }
  // };

  struct itimerspec spec_{};

  spec_.it_value.tv_sec =
    std::chrono::duration_cast<std::chrono::seconds>(initial).count();
  spec_.it_value.tv_nsec = (initial % std::chrono::seconds(1)).count();

  spec_.it_interval.tv_sec =
    std::chrono::duration_cast<std::chrono::seconds>(interval).count();
  spec_.it_interval.tv_nsec = (interval % std::chrono::seconds(1)).count();

  // we allow the user to pass a zero timeout, however that will disable the
  // timerfd so we make the timeout have at least 1ns which is close enough
  if ((spec_.it_value.tv_sec == 0) && (spec_.it_value.tv_nsec == 0))
  {
    spec_.it_value.tv_nsec = 1;
  }

  // do not call timerfd_settime() as that will start a timer, instead wait that
  // startTimer() gets called.

  // add a timer to epoll and not on "false"; that is "single" is false
  auto id = installTimer_(fd, spec_, handler, false);
  if (id < 0)
  {
    if (0 != close(fd))
    {
      LOG_MSG("faild to close timer in epoll {%s}", m_name.c_str());
    }
  }

  return id;
}

/* ={==========================================================================
start a periodic timer to the poll but not start it
*/

bool EPollLoop::startTimer(int64_t id)
{
  // TODO:
  std::lock_guard<std::recursive_mutex> lock(m_lock);

  auto it = m_timers.find(id);

  if (m_timers.end() == it)
  {
    LOG_MSG("failed to find timer %d in epoll {%s}", id, m_name.c_str());
    return false;
  }

  auto fd_ = it->second.fd;

  // set timer and start it
  if (0 != timerfd_settime(fd_, 0, &it->second.timeout, nullptr))
  {
    LOG_MSG("faild to set timer in epoll {%s}", m_name.c_str());
    return false;
  }

  return true;
}

/* ={==========================================================================
stop a periodic timer
*/

bool EPollLoop::stopTimer(int64_t id)
{
  // TODO:
  std::lock_guard<std::recursive_mutex> lock(m_lock);

  auto it = m_timers.find(id);

  if (m_timers.end() == it)
  {
    LOG_MSG("failed to find timer %d in epoll {%s}", id, m_name.c_str());
    return false;
  }

  auto fd_ = it->second.fd;

  // clear the timer's values to disarm the timer
  const struct itimerspec clear{}; //  = { { 0, 0L }, { 0, 0L } };

  // set timer and start it
  if (0 != timerfd_settime(fd_, 0, &clear, nullptr))
  {
    LOG_MSG("faild to set timer in epoll {%s}", m_name.c_str());
    return false;
  }

  return true;
}

/* ={==========================================================================
run a functor epoll loop
*/
bool EPollLoop::executeInPollloop(const Executor &func)
{
  // NOTE: use 1 init value and by doing this, "func" will be run without
  // waiting for a signal since read on eventfd only waits when it's 0.

  auto eventfd_ = eventfd(1, EFD_CLOEXEC);
  if (eventfd_ < 0)
  {
    LOG_MSG("failed to create eventfd for epoll {%s}", m_name.c_str());
    return false;
  }

  bool done_{false};
  std::mutex m_{};
  std::condition_variable cond_{};

  Handler call = [this, func, &done_, &m_, &cond_](int fd, unsigned events) {
    uint64_t noneed_{};
    if (TEMP_FAILURE_RETRY(read(fd, &noneed_, sizeof(noneed_))) !=
        sizeof(noneed_))
    {
      LOG_MSG("failed to read eventfd in epoll {%s}", m_name.c_str());
    }

    func();

    std::lock_guard<std::mutex> lock(m_);
    done_ = true;
    cond_.notify_all();
  };

  auto id_ = addDescriptor_(eventfd_, EPollLoop::Readable, call);
  if ( id_ < 0)
  {
    LOG_MSG("failed to add eventfd for epoll {%s}", m_name.c_str());
    close(eventfd_);
    return false;
  }

  // wait until signalled. timeout occurs every second if not signaled and loop
  // again. what's the difference when use no timeout and use cond.wait()?
  std::unique_lock<std::mutex> wait_(m_);

  while (!done_)
  {
    if (std::cv_status::timeout == cond_.wait_for(wait_, std::chrono::seconds(1)))
    {
      LOG_MSG("timed out waiting for handler to be run by the poll {%s}", m_name.c_str());
    }
  }

  removeDescriptor_(id_);

  if (close(eventfd_) < 0)
  {
    LOG_MSG("failed to close eventfd in poll {%s}", m_name.c_str());
  }

  return true;
}

/* ={=========================================================================
returns the epoll thread id
  @returns thread id
*/
std::thread::id EPollLoop::threadId() const
{
  return m_thread.get_id();
}

bool EPollLoop::running() const
{
  return m_thread.joinable();
}

/* ={=========================================================================
install the timerfd into the epoll
  @returns tiemr id since it's used in addTimer/startTimer
*/

int64_t EPollLoop::installTimer_(int fd,
                              const struct itimerspec &timeout,
                              const TimerHandler &handler,
                              bool single)
{
  if (m_epollfd < 0)
  {
    LOG_MSG("epoll {%s} is not running", m_name.c_str());
    return -1;
  }

  // create the event
  struct epoll_event event_
  {};
  event_.events = EPOLLIN;

  std::lock_guard<std::recursive_mutex> lock(m_lock);

  // generate a tag and save it to event
  int64_t tag_    = m_tag_counter++;
  event_.data.u64 = tag_;

  if (epoll_ctl(m_epollfd, EPOLL_CTL_ADD, fd, &event_) < 0)
  {
    LOG_MSG("failed to add a timer to epoll {%s}", m_name.c_str());
    return -1;
  }

  // add a timer
  TimerEntry timer_{};
  timer_.fd         = fd;
  timer_.singleShot = single;
  timer_.handler    = handler;
  timer_.timeout    = timeout;

  // debug
  // LOG_MSG("> tag {%d}, fd {%d}, timeout {%d}", tag_, fd, timeout.it_value.tv_nsec);

  m_timers.emplace(tag_, timer_);

  return tag_;
}

/* ={=========================================================================
clean up all timers from epoll and close them
*/

void EPollLoop::cleanupTimers_()
{
  auto it = m_timers.begin();

  while (it != m_timers.end())
  {
    auto fd_ = it->second.fd;

    if ((fd_ > 0) && (close(fd_) < 0))
    {
      LOG_MSG("failed to close timer in epoll {%s}", m_name.c_str());
    }

    it = m_timers.erase(it);
  }
}

/* ={=========================================================================
adds a handler for a given file descriptor
  @return a tag(id) to be used when to remove the handler. -1 on failure.
*/

int64_t EPollLoop::addDescriptor_(int fd, unsigned events, const Handler &handler)
{
  if (m_epollfd < 0)
  {
    LOG_MSG("epoll {%s} is not running", m_name.c_str());
    return -1;
  }

  // create a event for epoll 
  struct epoll_event event_{};

  event_.events = 0;

  if (events & EPollLoop::Readable)
    event_.events |= EPOLLIN;

  if (events & EPollLoop::Writeable)
    event_.events |= EPOLLOUT;

  std::lock_guard<std::recursive_mutex> lock(m_lock);

  auto tag_ = m_tag_counter++;
  event_.data.u64 = tag_;

  if (epoll_ctl(m_epollfd, EPOLL_CTL_ADD, fd, &event_) < 0)
  {
    LOG_MSG("failed to add a handler to epoll {%s}", m_name.c_str());
    return -1;
  }

  // add a handler
  HandlerEntry handler_{};
  handler_.fd = fd;
  handler_.events = events;
  handler_.handler = handler;

  m_handlers.emplace(tag_, handler_);

  return tag_;
}

/* ={=========================================================================
removes a handler for a given tag(id)
  @return true if handler is uninstalled. -1 on failure.
*/

bool EPollLoop::removeDescriptor_(const int64_t tag)
{
  if (m_epollfd < 0)
  {
    LOG_MSG("epoll {%s} is not running", m_name.c_str());
    return -1;
  }

  std::lock_guard<std::recursive_mutex> lock(m_lock);

  auto it = m_handlers.find(tag);

  if (it == m_handlers.end())
  {
    LOG_MSG("failed to find a handler in epoll {%s}", m_name.c_str());
    return false;
  }

  const int fd_ = it->second.fd;

  m_handlers.erase(it);

  if (epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd_, nullptr) < 0)
  {
    LOG_MSG("failed to remove a handler from epoll {%s}", m_name.c_str());
    return false;
  }

  return true;
}
