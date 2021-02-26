#pragma once

// #include <atomic>
#include <chrono>
#include <functional>
// #include <map>
#include <mutex>
#include <string>
// #include <sys/timerfd.h>
#include <condition_variable>
#include <thread>

// can be inside class
enum class TimerType
{
  SingleShot,
  Periodic
};

enum class TimerThreadPriority
{
  Default,
  Low
};

class Timer
{
private:
  using Executor = std::function<void()>;

  std::thread m_thread{};
  std::mutex m_lock{};
  std::condition_variable m_cond{};
  bool m_cancel{};
  Executor m_callback{};

private:
  void start_(const std::chrono::milliseconds &timeout,
              TimerType type,
              TimerThreadPriority priority,
              const Executor &call);

  void singleShot_(TimerThreadPriority, const std::chrono::milliseconds &);
  void periodicShot_(TimerThreadPriority, const std::chrono::milliseconds &);

public:
  Timer(const std::chrono::milliseconds &timeout,
        TimerType type,
        TimerThreadPriority priority,
        Executor f);

  // copy and move ctor
  Timer(const Timer &) = delete;
  Timer(Timer &&)      = delete;

  // copy and move assign
  Timer &operator=(const Timer &) = delete;
  Timer &operator=(Timer &&) = delete;

  ~Timer();

  void cancel();
};
