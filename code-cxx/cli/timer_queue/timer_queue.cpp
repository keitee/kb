#include <fcntl.h>
#include <list>
#include <poll.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <unistd.h>

#include "slog.h"
#include "timer_queue.h"

/*

23.7 Timers That Notify via File Descriptors: the timerfd API

Starting with kernel 2.6.25, Linux provides another API for creating timers.

The Linux-specific timerfd API creates a timer whose expiration notifications
`can be read from a file descriptor.` This is useful because the file descriptor
can be monitored along with other descriptors using select(), poll(), and epoll

#include <sys/timerfd.h>

int timerfd_create(int clockid, int flags);

Returns file descriptor on success, or –1 on error

The operation of the three new system calls in this API is analogous to the
operation of the timer_create(), timer_settime(), and timer_gettime() system
calls described in Section 23.6.

The first of the new system calls is timerfd_create(), which creates a new timer

The value of clockid can be either CLOCK_REALTIME or CLOCK_MONOTONIC (see Table
23-1).


Starting with Linux 2.6.27, the following values may be bitwise ORed in flags to
change the behavior of timerfd_create():

TFD_NONBLOCK

Set the O_NONBLOCK file status flag on the new open file description. Using this
flag saves extra calls to fcntl(2) to achieve the same result.


TFD_CLOEXEC

Set the close-on-exec flag (FD_CLOEXEC) for the new file descriptor. This flag
is useful for the same reasons as the open() O_CLOEXEC flag described in Section
4.3.1.

In Linux versions up to and including 2.6.26, flags must be specified as zero.


Q: what's return from read()? From timerfd_create man:

   Operating on a timer file descriptor
       The file descriptor returned by timerfd_create() supports the following
       operations:

       read(2)
              If the timer has already expired one or more times since its
              settings were last modified using timerfd_settime(), or since the
              last successful read(2), then the buffer given to read(2) returns
              an unsigned 8-byte  inte‐ ger (uint64_t) containing the number of
              expirations that have occurred.  (The returned value is in host
              byte order—that is, the native byte order for integers on the host
              machine.)


os-eventfd

       #include <sys/eventfd.h>
       int eventfd(unsigned int initval, int flags);

Since kernel 2.6.22, Linux provides an additional, nonstandard synchronization
mechanism via the eventfd() system call. This system call creates an eventfd
object that has an associated 8-byte unsigned integer maintained by the kernel.

The system call returns a file descriptor that refers to the object. Writing an
integer to this file descriptor adds that integer to the object’s value. A
read() from the file descriptor *blocks* if the object’s value is 0. 

If the object has a non-zero value, a read() returns that value and resets it to
0. In addition, poll(), select(), or epoll can be used to test if the object has
a nonzero value; if it does, the file descriptor indicates as being readable. An
application that wishes to use an eventfd object for synchronization must first
create the object using eventfd(), and then call fork() to create related
processes that inherit file descriptors referring to the object. For further
details, see the eventfd(2) manual page.

*/

/*

Not sure where this code come from and used in btaudio code.

Utility object that can be used to register a callback function to execute in
the future

Multiple callbacks can be registered via this object, internally it runs a
thread with a "single timerfd" that wakes up at the correct time and then
calls any handlers registered.

All callbacks are processed in the same thread, so obviously one timer handler
can block all the others, clients should bear this in mind.

It is safe to called the add() or remote() APIs from within a callback
handler.  Added timers will be processed once all callbacks have been handled
in a cycle, whereas removed timers will be added to a queue and removed from
the loop once the current callback has finished execution. In addition it is
guaranteed that the callback for a particular timer will never be called once
the remove() method on the timer in question returns.

o The eventfd is used to signal to end the poll loop thread at shutdown time.

o support that "remove() can be called from the context of a timer handler"

  remove() can be called from the context of a timer handler, however if
  you want to cancel a repeating timer then the recomeneded way is to just
  return false in the handler.

  so one shot timer will run once regardless of return value from callback since
  `reschedule` will be false and it's get deleted in the expired clean up. no
  need to call remove().

  so repeat timer will repate and it will stop if remove() is called or return
  false in callback. it is simple to return false.

  for both cases, no need to call remove().

  NOTE is a good idea? if use a simple lock between remove() and thread, then
  while a thread runs all expired timers, not able to add/remove timer.

  guess that to ease that point, allows that which makes this complicated:

  removing a timer gets a bit complicated because TimerQueue::remove() is
  allowed to be called from a timer callback from beging removed or the
  another thread.

  use "callback_running_" to indicate that thread is processing callbacks.
  NOT ABLE TO MAKE SENCE from remove() and drop it off.

o use single interface, add(), for single shot and peridic timer.

o use "prioriry queue" of timers.

  why say priority queue? since it uses std::multiset and use compare
  predicate which sorts out timer entries in "expiry time order".

o use "poll()"

o the head, that is the earliest expiry of queue at that time which can be
  updated when add() is called or while processing timers in the thread.
  updateTimerfd_() is a function to set the head. 

  This head is used to set timerfd to kick off a timer. When expired, thread
  wakes up and see if there are already expired timers in the queue. Since
  queue is ordered by expiry time, add() can be called with times between now
  and time of the head. 

  When the head gets expired, it figures out expired timers in the queue and
  timers that's not removed among them, and then run their functor and
  reschedule it if it's periodic timer by updating expiry time and put it back
  to the queue. Do it for all expired timers

  Then clean up removed tiemrs and set the head of the queue again which will
  set next timerfd to expire.

  This is one cycle of timerfd use/run.

*/

TimerQueue::TimerQueue()
    : callback_running_(false)
    , callback_tag_(-1)
    , tag_counter_(1)
{
  timerfd_ = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC);
  if (timerfd_ < 0)
  {
    LOG_MSG("failed to create timerfd");
    return;
  }

  eventfd_ = eventfd(0, EFD_CLOEXEC);
  if (eventfd_ < 0)
  {
    LOG_MSG("failed to create eventfd");
    return;
  }

  t_ = std::thread(&TimerQueue::timerThread_, this);
}

TimerQueue::~TimerQueue()
{
  stop();

  // that is, only when timerfd is still valid, close it and if close is a
  // success then return 0 so the final evaluation value is 0.

  if ((timerfd_ >= 0) && (close(timerfd_)) != 0)
    LOG_MSG("failed to close timerfd");

  if ((eventfd_ >= 0) && (close(eventfd_)) != 0)
    LOG_MSG("failed to close eventfd");
}

/* 
={============================================================================
add new timer to the timer queue
*/

int64_t TimerQueue::add(std::chrono::milliseconds const &timeout,
                        bool oneshot,
                        std::function<bool()> const &func)
{
  // check the timer thread is running
  if (!t_.joinable())
  {
    LOG_MSG("timer thread is not running");
    return -1LL;
  }

  // get a new id and will fail if exhausted
  int64_t tag = tag_counter_++;

  // use realtime mono clock
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);

  // create the entry
  TimerEntry entry;
  entry.tag     = tag;
  entry.oneshot = oneshot;
  entry.func    = func;
  entry.timeout = timeout;

  // set expiry time
  entry.expiry = calcAbsTime_(now, timeout);

  // hold the lock and push the timer into the priority queue
  std::lock_guard<std::mutex> lock(m_);

  auto it = tqueue_.emplace(entry);

  // insert to multiset may change the order of elements. if the new timer was
  // added to the head of the queue then start a timerfd with that time.

  if (it == tqueue_.begin())
    updateTimerfd_();

  return tag;
}

/* 
={============================================================================
remove the given timer from the queue
  @return true if the timer was found and was removed from the queue, otherwise
  false.
*/

bool TimerQueue::remove(int64_t tag)
{
  // sanity check on tag
  if (tag <= 0)
  {
    LOG_MSG("invalid timer tag %lld", tag);
    return false;
  }

  std::unique_lock<std::mutex> lock(m_);

  if (!t_.joinable())
  {
    LOG_MSG("timer thread not running");
    return false;
  }

  if (callback_running_)
  {
    // now it's running callback so add it to the remove set so that it can be
    // removed after callback finishes.
    removed_.insert(tag);

    // the next special case is if the timer callback currently being called is
    // the one we are removing. that is "callback_tag_ == tag"
    //
    // if this is called from the timer thread then will deadlock waiting on the
    // condifitonal variable. in such situations it is safe just to add the
    // timer to the removed set.

    // this is when called from another thread and called from the callback
    // IS IT POSSIBLE CASE?
    if ((callback_tag_ == tag) && (std::this_thread::get_id() != t_.get_id()))
    {
      // wait till the callback completes since callback_tag_ is set to -1 when
      // callback finishes.
      while (callback_tag_ == tag)
      {
        callback_complete_.wait(lock);
      }
    }

    // this is when called from the same timer thread
    // ???
    // unfortunatly we don't actually know if the tag was valid or not at
    // this point, so for now just assume it was ... FIXME
    return true;
  }
  else
  {
    // not currently runs any callbacks so can just remove it
    // NOTE: call doRemove_() without lock.
    return doRemove_(tag);
  }
}

/* 
={============================================================================
stops the poll loop thread and cancels all timers
*/
void TimerQueue::stop()
{
  // if the thread is still running, terminate by triggering the eventfd that
  // write to the eventfd to wake the pool loop

  if (t_.joinable())
  {
    uint64_t doesnt_matter{1};
    if (sizeof(uint64_t) !=
        TEMP_FAILURE_RETRY(write(eventfd_, &doesnt_matter, sizeof(uint64_t))))
    {
      LOG_MSG("failed to write to eventfd");
    }

    t_.join();
  }

  if (!tqueue_.empty())
  {
    LOG_MSG("queue is not empty, %d", tqueue_.size());
  }

  tqueue_.clear();
}

/* 
-{----------------------------------------------------------------------------
thread that runs the timer pool loop
*/

void TimerQueue::timerThread_()
{
  pthread_setname_np(pthread_self(), "TimerQueue");

  if (timerfd_ < 0)
  {
    LOG_MSG("no timerfd available");
    return;
  }

  if (eventfd_ < 0)
  {
    LOG_MSG("no eventfd available");
    return;
  }

  while (true)
  {
    struct pollfd fds[2];

    fds[0].fd      = eventfd_;
    fds[0].events  = POLLIN;
    fds[0].revents = 0;

    fds[1].fd      = timerfd_;
    fds[1].events  = POLLIN;
    fds[1].revents = 0;

    // note that not handling when ret == 0 since
    // poll() returns number of ready file descriptors, 0 on timeout, or -1 on
    // error

    int ret = TEMP_FAILURE_RETRY(poll(fds, 2, -1));
    if (ret < 0)
    {
      LOG_MSG("poll failed");
    }
    else if (ret > 0)
    {
      // check if the eventfd has fired
      if (fds[0].revents != 0)
      {
        // check for any error conditions
        if (fds[0].revents & (POLLERR | POLLHUP | POLLNVAL))
        {
          LOG_MSG("received error events on eventfd (0x%04x)", fds[0].revents);
        }

        // clear value on eventfd
        uint64_t ignore;
        if (TEMP_FAILURE_RETRY(sizeof(uint64_t) !=
                               read(eventfd_, &ignore, sizeof(uint64_t))))
        {
          LOG_MSG("failed to read from eventfd");
        }

        // break out of the poll loop. END thread
        break;
      }

      if (fds[1].revents != 0)
      {
        // check for any error conditions
        if (fds[1].revents & (POLLERR | POLLHUP | POLLNVAL))
        {
          LOG_MSG("received error events on timerfd (0x%04x)", fds[1].revents);
        }

        // clear value on timerfd
        uint64_t ignore;
        if (TEMP_FAILURE_RETRY(sizeof(uint64_t) !=
                               read(timerfd_, &ignore, sizeof(uint64_t))))
        {
          LOG_MSG("failed to read from timerfd");
        }

        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);

        std::unique_lock<std::mutex> lock(m_);

        // set a marker so that anyone calls remove() then the timer to remove
        // is added to the removed set rather than looking up in the timer queue.
        callback_running_ = true;

        // move all expired timers into a separate set for processing
        // that is, separate timers that are before than the fired in time and
        // these are `expired` timers.
        //
        // also note that timers are deleted from the timer queue.
        std::list<TimerEntry> expired;
        auto it = tqueue_.begin();
        while ((it != tqueue_.end()) && (it->checkLessThanOrEqualTo(now)))
        {
          expired.emplace_back(std::move(*it));
          it = tqueue_.erase(it);
        }

        for (auto &entry : expired)
        {
          // if a timer is in removed than no need to run it.
          if (removed_.count(entry.tag) != 0)
          {
            continue;
          }

          callback_tag_ = entry.tag;

          // {
          // run callback and *do not hold the lock*.
          lock.unlock();

          // if callback returns true and not oneshot then reschedule it
          bool reschedule = entry.func && entry.func() && !entry.oneshot;

          lock.lock();
          // }

          // clear the callback we're in
          callback_tag_ = -1;

          // signal condition that remove() may be waiting on
          callback_complete_.notify_all();

          // reschedule a timer. checking if it's in removed set is already
          // done.
          // if (reschedule && (mRemovedSet.count(entry.tag) == 0))
          if (reschedule)
          {
            entry.expiry = calcAbsTime_(now, entry.timeout);
            tqueue_.emplace(std::move(entry));
          }
        } // expired for

        // finished running callbacks
        callback_running_ = false;

        // clean up the removed timers which can be before or future than now.
        for (auto tag : removed_)
        {
          doRemove_(tag);
        }

        // finally update timerfd for the next item that is on the head of queue
        // if any so taht run timer queue.
        updateTimerfd_();
      } // timerfd
    }
  } // while
}

/* 
-{----------------------------------------------------------------------------
calculate new timespec time based on now timespec and ms offset
*/

struct timespec
TimerQueue::calcAbsTime_(struct timespec const &base,
                         std::chrono::milliseconds const &offset) const
{
#define NSECS_PER_SEC 1000000000L
#define NSECS_PER_MSEC 1000000L

  struct timespec ts;

  ts.tv_sec = base.tv_sec +
              std::chrono::duration_cast<std::chrono::seconds>(offset).count();

  // (offset % std::chrono::seconds(1)).count() makes remaining ms and convert
  // it to ns.
  ts.tv_nsec = base.tv_nsec +
               ((offset % std::chrono::seconds(1)).count() * NSECS_PER_MSEC);

  if (ts.tv_nsec > NSECS_PER_SEC)
  {
    ts.tv_nsec -= NSECS_PER_SEC;
    ts.tv_nsec += 1;
  }

  return ts;
}

/* 
-{----------------------------------------------------------------------------
write the item on the head of the `expiry queue` into the timerfd for the next
wake-up time. that is set timerfd with the next timer to run.

        struct timeval {
            time_t tv_sec;         // Seconds since 00:00:00, 1 Jan 1970 UTC
            suseconds_t tv_usec;   // Additional `microseconds` (long int)
        };

        struct itimerval {
          struct timeval it_interval; // Interval for periodic timer
          struct timeval it_value;    // Current value (time until next expiration)
        };

/usr/include/x86_64-linux-gnu/bits/types/struct_timespec.h

POSIX.1b structure for a time value.  This is like a `struct timeval' but
has nanoseconds instead of microseconds.

       struct timespec {
           time_t tv_sec;                // Seconds
           long   tv_nsec;               // Nanoseconds
       };

       struct itimerspec {
           struct timespec it_interval;  // Interval for periodic timer 
           struct timespec it_value;     // Initial expiration
       };

       int timerfd_settime(int fd, int flags,
                           const struct itimerspec *new_value,
                           struct itimerspec *old_value);

       TFD_TIMER_ABSTIME
              Interpret new_value.it_value as an absolute value on the timer's
              clock.  The timer will expire when the value of the timer's
              clock reaches the value specified in new_value.it_value.

    TFD_TIMER_ABSTIME = 1 << 0,
 
*/

void TimerQueue::updateTimerfd_() const
{
  struct itimerspec its
  {};

  if (tqueue_.empty())
  {
    // will disable the timerfd
    its.it_value.tv_sec  = 0;
    its.it_value.tv_nsec = 0;
  }
  else
  {
    // set expiry time. begin() returns iterator hence use ->
    its.it_value = tqueue_.begin()->expiry;
  }

  if (0 != timerfd_settime(timerfd_, 0, &its, nullptr))
  // if (0 != timerfd_settime(timerfd_, TFD_TIMER_ABSTIME, &its, nullptr))
  {
    LOG_MSG("failed to set timerfd value");
  }
}

/* 
-{----------------------------------------------------------------------------
removes the time with tag from the queue.
note that it must be called with the lock held.
*/

bool TimerQueue::doRemove_(int64_t tag)
{
  for (auto it = tqueue_.begin(); it != tqueue_.end(); ++it)
  {
    if (it->tag == tag)
    {
      bool requires_update = (it == tqueue_.begin());

      // see the order of q handling. use begin() and erase() later but not the
      // other way round.
      tqueue_.erase(it);

      // if we remove the timer from the head then need to update the expiry of
      // the timerfd with the new head.
      if (requires_update)
        updateTimerfd_();

      return true;
    }
  }

  LOG_MSG("failed to find timer %lld to remove", tag);
  return false;
}
