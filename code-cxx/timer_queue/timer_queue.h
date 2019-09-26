#ifndef TIMER_QUEUE
#define TIMER_QUEUE

#include <atomic>
#include <condition_variable>
#include <chrono>
#include <mutex>
#include <time.h>
#include <thread>
#include <set>
#include <functional>


/*
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
*/

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
      bool operator() (TimerEntry const &lhs, TimerEntry const &rhs)
      {
        return (lhs.expiry.tv_sec < rhs.expiry.tv_sec) ||
          ((lhs.expiry.tv_sec == rhs.expiry.tv_sec) && (lhs.expiry.tv_nsec < rhs.expiry.tv_nsec));
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
