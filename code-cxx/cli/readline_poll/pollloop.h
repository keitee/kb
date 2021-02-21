#pragma once

#include "ipoll.h"
#include <atomic>
#include <list>
#include <stdint.h>
#include <string>
#include <thread>

// summary from the comment:
// a mutex is overkill. It has he same API as std::mutex so can be swapped
// out since it implements the C++ BasicLockable and Lockable requirements.

class SpinLock
{
private:
  std::atomic_flag locked_;

public:
  SpinLock()
      : locked_(ATOMIC_FLAG_INIT)
  {}

  void lock()
  {
    while (locked_.test_and_set(std::memory_order_acquire))
      ;
  }

  void unlock() { locked_.clear(std::memory_order_release); }

  bool try_lock() { return locked_.test_and_set(std::memory_order_acquire); }
};

// TODO:
// note that EPOLLDEFERRED is not in man epoll_ctl. what's this?
// the same value is defined in /usr/include/sys/epoll.h
//    EPOLLWAKEUP = 1u << 29; 536870912
// is it going to be okay?

const uint32_t EPOLLDEFERRED{1u << 29};

class PollLoop : public IPollLoop, public std::enable_shared_from_this<PollLoop>
{
private:
  // the poll thread name
  const std::string m_name;

  std::thread m_thread;

  // TODO: necessary?
  std::atomic<pid_t> m_threadid;

  // epollfd which is made from epoll_create()
  int m_epollfd{-1};

  // timerfd
  int m_defer_timerfd{-1};

  const unsigned int m_max_source;

  // timerfd interval
  const struct itimerspec m_defer_timespec;

  // eventfd to end thread loop
  int m_death_eventfd{-1};

  // the number of sources that have the EPOLLDEFERRED flag
  int m_deferred_sources{};

  void run_(const std::string &name, int priority);
  void enableDeferredTimer_();
  void disableDeferredTimer_();

private:
  struct PollSourceWrapper
  {
    std::string m_name;
    std::weak_ptr<IPollSource> m_source;
    int m_fd;
    uint32_t m_events;

    PollSourceWrapper(const std::string &name,
                      const std::shared_ptr<IPollSource> &source,
                      int fd,
                      uint32_t events)
        : m_name(name)
        , m_source(source)
        , m_fd(fd)
        , m_events(events)
    {}
  };

  std::list<PollSourceWrapper> m_sources;

  // the lock to protect m_sources
  SpinLock m_lock{};

public:
  PollLoop(const std::string &name,
           unsigned int max_source     = 512,
           long deferred_time_interval = 20);
  virtual ~PollLoop();

  // IPollLoop interface
public:
  virtual bool start(int priority = -1) override;
  virtual void stop() override;

  virtual bool addSource(const std::string &name,
                         const std::shared_ptr<IPollSource> &source,
                         int fd,
                         uint32_t events) override;

  virtual bool modSource(const std::shared_ptr<IPollSource> &source,
                         uint32_t events) override;

  virtual bool delSource(const std::shared_ptr<IPollSource> &source) override;
};
