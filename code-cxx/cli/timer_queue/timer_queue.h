#ifndef TIMER_QUEUE
#define TIMER_QUEUE

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <set>
#include <thread>
#include <time.h>

class TimerQueue
{
public:
  TimerQueue();
  ~TimerQueue();

  int64_t add(std::chrono::milliseconds const &timeout,
              bool oneshot,
              std::function<bool()> const &func);

  bool remove(int64_t tag);

  void stop();

private:
  void timerThread_();

  bool doRemove_(int64_t tag);

  void updateTimerfd_() const;

  struct timespec calcAbsTime_(struct timespec const &now,
                               std::chrono::milliseconds const &timeout) const;

private:
  struct TimerEntry
  {
    int64_t tag;
    bool oneshot;
    struct timespec expiry;
    std::function<bool()> func;
    std::chrono::milliseconds timeout;

    // check if time of entry is <= than rhs.
    // *cxx-shortcut-evaluation*
    // have to be less in seconds and then check on nano seconds.

    bool checkLessThanOrEqualTo(struct timespec const &rhs) const
    {
      return (expiry.tv_sec < rhs.tv_sec) ||
             ((expiry.tv_sec == rhs.tv_sec) && (expiry.tv_nsec <= rhs.tv_nsec));
    }
  };

  // check if lhs < rhs in time and this is `compare` for multiset which sort
  // timer from near to further in time.

  struct TimerEntryCompare
  {
    bool operator()(TimerEntry const &lhs, TimerEntry const &rhs)
    {
      return (lhs.expiry.tv_sec < rhs.expiry.tv_sec) ||
             ((lhs.expiry.tv_sec == rhs.expiry.tv_sec) &&
              (lhs.expiry.tv_nsec < rhs.expiry.tv_nsec));
    }
  };

  std::multiset<TimerEntry, TimerEntryCompare> tqueue_;

private:
  std::mutex m_;
  std::thread t_;

  int timerfd_;
  int eventfd_;

  bool callback_running_;
  int64_t callback_tag_;
  std::condition_variable callback_complete_;

  std::set<int64_t> removed_;
  std::atomic<int64_t> tag_counter_;
};

#endif // TIMER_QUEUE
