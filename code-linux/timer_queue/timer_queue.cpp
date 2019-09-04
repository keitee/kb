
#include <sys/timerfd.h>

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


#define LOG_SYS_ERROR(err, fmt) \
  std::cout << "(" << err << ")" << fmt << std::endl;

#define LOG_ERROR(fmt) \
  std::cout << fmt << std::endl;

TimerQueue::TimerQueue()
{
  timer_fd_ = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC);
  if (timer_fd_ < 0)
  {
    LOG_SYS_ERROR(errno, "failed to create timerfd");
    return;
  }

  event_fd_ = eventfd(0, EFD_CLOEXEC);
  if (event_fd_ < 0)
  {
    LOG_SYS_ERROR(errno, "failed to create eventfd");
    return;
  }

  thread_ = std::thread(&TimerQueue::timerThread, this);
}

void TimerQueue::timerThread()
{
  pthread_setname_np(pthread_self(), "TimerQThread");
  
  if (timer_fd_ < 0)
  {
    LOG_ERROR("no timerfd available");
    return;
  }

  if (event_fd_ < 0)
  {
    LOG_ERROR("no eventfd available");
    return;
  }

  while (true)
  {
    struct pollfd fds[2];

    fds[0].fd = event_fd_;
    fds[0].events = POLLIN;
    fds[0].revents = 0;

    fds[1].fd = timer_fd_;
    fds[1].events = POLLIN;
    fds[1].revents = 0;

    // note that not handling when ret == 0 since 
    // poll() returns number of ready file descriptors, 0 on timeout, or -1 on
    // error 

    int ret = TEMP_FAILURE_RETRY(poll(fds, 2, -1));
    if (ret < 0)
    {
      LOG_SYS_ERROR(errno, "poll failed");
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

        // 
      }
    }
  } // while
}


