#pragma once

#include <chrono>
#include <functional>
#include <mutex>
#include <string>
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

  Timer(const std::chrono::milliseconds &timeout, Executor f);

  template <typename F, typename... Args>
    Timer(const std::chrono::milliseconds &timeout, TimerType type, TimerThreadPriority priority, F f, Args&&... args)
    {
      start_(timeout, type, priority, std::bind(f, args...));
    }

  template <typename F, typename... Args>
    Timer(const std::chrono::milliseconds &timeout, F f, Args&&... args)
    {
      start_(timeout, TimerType::SingleShot, TimerThreadPriority::Default, std::bind(f, args...));
    }

  // copy and move ctor
  Timer(const Timer &) = delete;
  Timer(Timer &&)      = delete;

  // copy and move assign
  Timer &operator=(const Timer &) = delete;
  Timer &operator=(Timer &&) = delete;

  ~Timer();

  void cancel();
};
