#ifndef TIMER_QUEUE
#define TIMER_QUEUE

#include <atomic>
#include <condition_variable>
#include <chrono>
#include <mutex>
#include <time.h>
#include <thread>
#include <set>

class TimerQueue
{
  public:
    TimerQueue();
    ~TimerQueue();

    int64_t add(std::chrono::milliseconds const &timeout, bool oneshot,
        std::function<bool()> const &func);

    bool remove(int64_t tag);

    void stop();

  private:
    void timerThread_();

    void doRemove_(int64_t tag);

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

    struct TimeEntryCompare
    {
      bool oparator() (TimeEntry const &lhs, TimeEntry const &rhs)
      {
        return (lhs.tv_sec < rhs.tv_sec) ||
          ((lhs.tv_sec == rhs.tv_sec) && (lhs.tv_nsec < rhs.tv_nsec));
      }
    };

    std::multiset<TimerEntry, TimerEntryCompre> tqueue_;

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
