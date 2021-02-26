#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <sys/timerfd.h>
#include <thread>

class EPollLoop
{
private:
  const std::string m_name;
  const int m_priority;

  int m_epollfd{};
  int m_death_eventfd{};

  std::atomic<int64_t> m_tag_counter{};
  std::thread m_thread{};

  // std::recursive_mutex m_lock;
  std::mutex m_lock;

public:
  explicit EPollLoop(const std::string &name, int priority = -1);
  ~EPollLoop();

public:
  enum Events
  {
    Readable  = 0x01,
    Writeable = 0x02,
    Exception = 0x04
  };

  using Executor = std::function<void()>;

private:
  using TimerHandler = std::function<void(int64_t)>;
  using Handler      = std::function<void(int, unsigned)>;

  // use by both single and periodic timer
  struct TimerEntry
  {
    int fd{};
    bool singleShot{};
    TimerHandler handler{};
    struct itimerspec timeout
    {};
  };

  std::map<int64_t, TimerEntry> m_timers;

  // add and remove a timer
  int64_t installTimer_(int fd,
                        const struct itimerspec &timeout,
                        const TimerHandler &handler,
                        bool single);

  void cleanupTimers_();

  void run_();

  struct HandlerEntry
  {
    int fd{};
    unsigned events{};
    Handler handler{};
  };

  std::map<int64_t, HandlerEntry> m_handlers;

public:
  bool start();
  void stop();

  // single shot timer
  bool startSingleShotTimer(const std::chrono::nanoseconds &timeout,
                            const TimerHandler &handler);

  // periodic timer
  int64_t addTimer(const std::chrono::nanoseconds &initial,
                   const std::chrono::nanoseconds &interval,
                   const TimerHandler &handler);

  bool startTimer(int64_t id);
  bool stopTimer(int64_t id);
  bool removeTimer(int64_t id);

  bool executeInPollloop(const Executor &);

  // add and remove a handler. note that these are public and also used in
  // executeInPollloop()
  int64_t addDescriptor(int fd, unsigned events, const Handler &handler);
  bool removeDescriptor(const int64_t tag);

  std::thread::id threadId() const;
  bool running() const;
};
