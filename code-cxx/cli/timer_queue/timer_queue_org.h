/*
 * File:   timer_queue.h
 */
#ifndef AS_BLUETOOTH_TIMER_QUEUE_H
#define AS_BLUETOOTH_TIMER_QUEUE_H

#include <time.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <set>
#include <thread>

// -----------------------------------------------------------------------------
/**
 *  @class TimerQueue
 *  @brief Utility object that can be used to register a callback function to
 *  execute in the future
 *
 *  Multiple callbacks can be registered via this object, internally it runs a
 *  thread with a single timerfd that wakes up at the correct time and then
 *  calls any handlers registered.
 *
 *  All callbacks are processed in the same thread, so obviously one timer
 *  handler can block all the others, clients should bear this in mind.
 *
 *  It is safe to called the add() or remote() APIs from within a callback
 *  handler.  Added timers will be processed once all callbacks have been
 *  handled in a cycle, whereas removed timers will be added to a queue and
 *  removed from the loop once the current callback has finished execution. In
 *  addition it is guaranteed that the callback for a particular timer will
 *  never be called once the remove() method on the timer in question returns.
 *
 */
class TimerQueue
{
public:
  TimerQueue();
  ~TimerQueue();

public:
  void stop();

public:
  int64_t add(const std::chrono::milliseconds &timeout,
              bool oneShot,
              const std::function<bool()> &func);
  bool remove(int64_t tag);

private:
  void timerThread();

  bool doRemove(int64_t tag);

  void updateTimerFd() const;
  struct timespec calcAbsTime(const struct timespec &now,
                              const std::chrono::milliseconds &timeout) const;

private:
  struct TimerEntry
  {
    int64_t tag;
    bool oneshot;
    struct timespec expiry;
    std::function<bool()> func;
    std::chrono::milliseconds timeout;

    bool isLessThanOrEqualTo(const struct timespec &rhs) const
    {
      return (expiry.tv_sec < rhs.tv_sec) ||
             ((expiry.tv_sec == rhs.tv_sec) && (expiry.tv_nsec <= rhs.tv_nsec));
    }
  };

  class TimerEntryCompare
  {
  public:
    bool operator()(const TimerEntry &a, const TimerEntry &b)
    {
      return (a.expiry.tv_sec < b.expiry.tv_sec) ||
             ((a.expiry.tv_sec == b.expiry.tv_sec) &&
              (a.expiry.tv_nsec < b.expiry.tv_nsec));
    }
  };

  std::multiset<TimerEntry, TimerEntryCompare> mTimersQueue;

private:
  std::mutex mLock;
  std::thread mThread;
  int mTimerFd;
  int mEventFd;

  bool mExecCallbacks;
  std::set<int64_t> mRemovedSet;

  int64_t mInCallbackTag;
  std::condition_variable mCallbackComplete;

  std::atomic<int64_t> mTagCounter;
};

#endif // !defined(AS_BLUETOOTH_TIMER_QUEUE_H)
