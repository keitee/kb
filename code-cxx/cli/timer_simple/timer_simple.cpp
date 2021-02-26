#include "timer_simple.h"
#include "slog.h"

// #include <iostream>
#include <mutex>
// #include <sys/epoll.h>
// #include <sys/eventfd.h>
// #include <sys/timerfd.h>
#include <unistd.h>

/*

1.
  public:
    using Executor = std::function<void()>;

  bool executeInPollloop(const Executor &);

2. use a thread for each timer request

*/

/* ={=========================================================================
*/

Timer::Timer(const std::chrono::milliseconds &timeout,
             TimerType type,
             TimerThreadPriority priority,
             Executor f)
{
  start_(timeout, type, priority, f);
}

Timer::~Timer()
{
  cancel();
}

/* ={=========================================================================
*/
void Timer::cancel()
{
  {
    std::lock_guard<std::mutex> lock(m_lock);
    m_cancel = true;
  }

  // prevent when it gets called from callback
  if (std::this_thread::get_id() != m_thread.get_id())
  {
    // notify to cancel pending timer and wait for it to end.
    if (m_thread.joinable())
    {
      m_cond.notify_all();
      m_thread.join();
    }
  }
}

/* ={=========================================================================
*/

void Timer::start_(const std::chrono::milliseconds &timeout,
                   TimerType type,
                   TimerThreadPriority priority,
                   const Executor &call)
{
  m_callback = call;
  m_cancel   = false;

  if (TimerType::SingleShot == type)
  {
    m_thread = std::thread(&Timer::singleShot_, this, priority, timeout);
  }
  else
  {
    m_thread = std::thread(&Timer::periodicShot_, this, priority, timeout);
  }
}

/* ={=========================================================================
o can be canceled before timeout expires
*/

void Timer::singleShot_(const TimerThreadPriority priority,
                        const std::chrono::milliseconds &timeout)
{
  pthread_setname_np(pthread_self(), "REPEAT_TIMER");

  // use 3 for low priority
  if (TimerThreadPriority::Low == priority)
  {
    // struct sched_param {
    //     int sched_priority;     // Scheduling priority
    // };

    struct sched_param param_
    {
      3
    };

    pthread_setschedparam(pthread_self(), SCHED_RR, &param_);
  }

  std::unique_lock<std::mutex> lock(m_lock);

  // get timepoint from timeout since wait_until() uses timepoint
  auto timeout_ = std::chrono::steady_clock::now() + timeout;

  // use wait_until() so it's out either when cond notified from cancel() or
  // timeout happens
  while (!m_cancel && (std::chrono::steady_clock::now() < timeout_))
  {
    m_cond.wait_until(lock, timeout_);
  }

  // ok, not canceled and expired so run call
  if (!m_cancel && m_callback)
  {
    lock.unlock();
    m_callback();
  }
}

/* ={=========================================================================
*/

void Timer::periodicShot_(const TimerThreadPriority priority,
                          const std::chrono::milliseconds &interval)
{
  pthread_setname_np(pthread_self(), "REPEAT_TIMER");

  // use 3 for low priority
  if (TimerThreadPriority::Low == priority)
  {
    // struct sched_param {
    //     int sched_priority;     // Scheduling priority
    // };

    struct sched_param param_
    {
      3
    };

    pthread_setschedparam(pthread_self(), SCHED_RR, &param_);
  }

  std::unique_lock<std::mutex> lock(m_lock);

  // get timepoint from timeout since wait_until() uses timepoint
  auto timeout_ = std::chrono::steady_clock::now() + interval;

  while (!m_cancel)
  {
    // use wait_until() so it's out either when cond notified from cancel() or
    // timeout happens
    while (!m_cancel && (std::chrono::steady_clock::now() < timeout_))
    {
      m_cond.wait_until(lock, timeout_);
    }

    // not canceled
    if (!m_cancel && m_callback)
    {
      timeout_ += interval;

      lock.unlock();
      m_callback();

      // need to lock again since it's on while and wait_until() again.
      lock.lock();
    }
  } // while
}
