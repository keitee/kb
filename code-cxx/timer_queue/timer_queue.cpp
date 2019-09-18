
#include <sys/timerfd.h>

#include "timer_queue.h"
#include "slog.h"

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

TFD_CLOEXEC

Set the close-on-exec flag (FD_CLOEXEC) for the new file descriptor. This flag
is useful for the same reasons as the open() O_CLOEXEC flag described in Section
4.3.1.


eventfd

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


TimerQueue::TimerQueue()
  : callback_exec_(false)
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

  thread_ = std::thread(&TimerQueue::timerThread_, this);
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


/* add new timer to the timer queue
 */
int64_t TimerQueue::add(std::chrono::milliseconds const &timeout, bool oneshot
    , std::function<bool()> const &func)
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
  entry.tag = tag;
  entry.oneshot = oneshot;
  entry.func = func;
  entry.timeout = timeout;
  entry.expiry = calcAbsTime(now, timeout);

  // hold the lock and push the timer into the queue
  std::lock_guard<std::mutex> lock(m_);

  // if the new timer was added to the head of the queue then update the
  // timerfd.
  // TODO: why say priority queue?
  auto it = tqueue_.emplace(entry);

  if (it == tqueue_.begin())
    updateTimerfd();

  return tag;
}


/* stops the poll loop thread and cancels all timers
 */
void TimerQueue::stop()
{
  // if the thread is still running, terminate by triggering the eventfd that
  // write to the eventfd to wake the pool loop

  if (t_.joinable())
  {
    uint64_t doesnt_matter{1};
    if (sizeof(uint64_t) != TEMP_FAILURE_RETRY(write(eventfd_, &doesnt_matter, sizeof(uint64_t))))
    {
      LOG_MSG("failed to write to eventfd");
    }

    t_.join();
  }

  tqueue_.clear();
}


void TimerQueue::timerThread_()
{
  pthread_setname_np(pthread_self(), "TimerQueue");
  
  if (timerfd_ < 0)
  {
    LOG_ERROR("no timerfd available");
    return;
  }

  if (eventfd_ < 0)
  {
    LOG_ERROR("no eventfd available");
    return;
  }

  while (true)
  {
    struct pollfd fds[2];

    fds[0].fd = eventfd_;
    fds[0].events = POLLIN;
    fds[0].revents = 0;

    fds[1].fd = timerfd_;
    fds[1].events = POLLIN;
    fds[1].revents = 0;

    // note that not handling when ret == 0 since 
    // poll() returns number of ready file descriptors, 0 on timeout, or -1 on
    // error 

    int ret = TEMP_FAILURE_RETRY(poll(fds, 2, -1));
    if (ret < 0)
    {
      LOG_MSG(errno, "poll failed");
    }
    else if (ret > 0)
    {
      // check if the eventfd has fired
      if (fds[0].revents != 0)
      {
        // check for any error conditions
        if (fds[0].revents & (POLLERR | POLLHUP | POLLNVAL))
        {
          LOG_ERROR("received error events on eventfd");
          // LOG_ERROR("received error events on eventfd (0x%04x)",
          //     fd[0].revents);
        }

        // clear counter on eventfd
        uint64_t ignore;
        if (TEMP_FAILURE_RETRY(sizeof(uint64_t) != read(eventfd, &ignore, sizeof(uint64_t))))
        {
          LOG_MSG(errno, "failed to read from eventfd");
        }

        // break out of the poll loop. ???
      }
    }
  } // while
}


