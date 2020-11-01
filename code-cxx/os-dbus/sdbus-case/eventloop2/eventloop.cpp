#include <eventloop.h>
#include <eventloop_p.h>

#include <cassert> // assert
#include <rlog.h>
#include <semaphore.h>
#include <sys/eventfd.h>
#include <systemd/sd-event.h>
#include <unistd.h>

#include <thread>

/*
Shows client thread actually runs loop and sd_event do dispatch work

#6  0x000055555557b64d in EventLoopPrivate::eventHandler_ (source=0x55555580ac20, fd=4, revents=1, data=0x55555580a4a0) at /home/keitee/git/kb/code-cxx/os-dbus/sdbus-case/eventloop1/eventloop.cpp:115
#7  0x00007ffff7f65b03 in ?? () from /lib/x86_64-linux-gnu/libsystemd.so.0
#8  0x00007ffff7f667e4 in sd_event_dispatch () from /lib/x86_64-linux-gnu/libsystemd.so.0
#9  0x00007ffff7f67a87 in sd_event_run () from /lib/x86_64-linux-gnu/libsystemd.so.0
#10 0x00007ffff7f99abb in sd_event_loop () from /lib/x86_64-linux-gnu/libsystemd.so.0
#11 0x000055555557b4ad in EventLoopPrivate::run (this=0x55555580a4a0) at /home/keitee/git/kb/code-cxx/os-dbus/sdbus-case/eventloop1/eventloop.cpp:81
#12 0x000055555557bc92 in EventLoop::run (this=0x7fffffffd7d0) at /home/keitee/git/kb/code-cxx/os-dbus/sdbus-case/eventloop1/eventloop.cpp:250

*/

// #define EVENTLOOP_DEBUG
#define DEADLOCK_FIX

/* ={--------------------------------------------------------------------------
 @brief :
  EventLoopPrivate
*/

thread_local EventLoopPrivate *EventLoopPrivate::m_loopRunning{nullptr};

// since it is static
// EventLoopPrivate *EventLoopPrivate::m_loopRunning{nullptr};

// get eventloop and eventfd
EventLoopPrivate::EventLoopPrivate()
    : m_eventloop{nullptr}
    , m_eventfd{-1}
{
  // int sd_event_new(sd_event **event);
  int ret = sd_event_new(&m_eventloop);
  if (ret < 0)
  {
    logSysError(-ret, "failed to create new eventloop");
    return;
  }

  // int eventfd(unsigned int initval, int flags);
  m_eventfd = eventfd(0, EFD_CLOEXEC);
  if (m_eventfd < 0)
  {
    logSysError(errno, "failed to create new eventfd");
  }

#ifdef EVENTLOOP_DEBUG
  logWarning("EventLoopPrivate::EventLoopPrivate() thread(%ld)",
             std::this_thread::get_id());
#endif
}

// release eventfd and eventloop
EventLoopPrivate::~EventLoopPrivate()
{
  if ((m_eventfd >= 0) && close(m_eventfd) != 0)
  {
    logSysError(errno, "failed to close eventfd");
  }

  m_eventfd = -1;

  if (m_eventloop)
  {
    // sd_event_unref returns null
    m_eventloop = sd_event_unref(m_eventloop);

#ifdef EVENTLOOP_DEBUG
    logWarning("EventLoopPrivate::~EventLoopPrivate()");
#endif
  }
}

// event handler
// typedef int (*sd_event_io_handler_t)(
//    sd_event_source *s,
//  	int fd,
//  	uint32_t revents,
//  	void *userdata);

// TODO:
// 1. read(m_eventfd) really fails and if so, does it matter since it's already
// signaled?
//
// 3. return value from event handler? no mention in the document.

int EventLoopPrivate::event_handler_(sd_event_source *s,
                                     int fd,
                                     uint32_t revents,
                                     void *userdata)
{
  EventLoopPrivate *self = static_cast<EventLoopPrivate *>(userdata);

  // sanity
  assert(self != nullptr);
  assert(self->m_eventfd == fd);

  uint64_t ignore{};

  if (sizeof(ignore) != TEMP_FAILURE_RETRY(read(fd, &ignore, sizeof(ignore))))
  {
    logSysError(errno, "failed to read eventfd");
  }

  // debug
  // logWarning(
  //   "event_handler_: dispatched so process all tasks in the q. thread(%ld)",
  //   std::this_thread::get_id());

  {
    std::lock_guard<std::recursive_mutex> lock(self->m_rm);

    while (!self->m_q.empty())
    {
      const auto f = self->m_q.front();
      if (f)
        f();

      self->m_q.pop();
    }
  }

  return 0;
}

int EventLoopPrivate::run()
{
  if (!m_eventloop)
  {
    logWarning("no eventloop to use");
    return EXIT_FAILURE;
  }

  sd_event_source *event_source_quit{nullptr};

  // int sd_event_add_io(
  //    sd_event *event,
  //  	sd_event_source **source,
  //  	int fd,
  //  	uint32_t events,
  //  	sd_event_io_handler_t handler,
  //  	void *userdata);

  int ret = sd_event_add_io(m_eventloop,
                            &event_source_quit,
                            m_eventfd,
                            EPOLLIN,
                            event_handler_,
                            this);
  if (ret < 0)
  {
    logSysError(-ret, "failed to add io to eventloop");
    return EXIT_FAILURE;
  }

  m_loopRunning = this;

#ifdef EVENTLOOP_DEBUG
  logWarning("eventloop runs and set the thread local to this. thread id(%ld), "
             "m_loopRunning(0x%p)",
             std::this_thread::get_id(),
             m_loopRunning);
#endif

  // run eventloop which `dispatch` sources
  //
  // uses exit code from quit() since sd_event_exit() requests the event loop
  // specified in the event event loop object to exit. The code parameter may be
  // any integer value and is returned as-is by sd_event_loop(3) after the last
  // event loop iteration.

  int exit_code = sd_event_loop(m_eventloop);

#ifdef EVENTLOOP_DEBUG
  logWarning("eventloop stops and set the thread local to null");
  m_loopRunning = nullptr;
#endif

  return exit_code;
}

// post a task
bool EventLoopPrivate::invokeMethod(std::function<void()> &&f)
{
  // use `block` to release lock
  {
    std::lock_guard<std::recursive_mutex> lock(m_rm);
    m_q.emplace(f);
  }

  uint64_t wakeup{1};

  if ((sizeof(wakeup)) !=
      TEMP_FAILURE_RETRY(write(m_eventfd, &wakeup, sizeof(wakeup))))
  {
    logSysError(errno, "failed to write eventfd");
    return false;
  }

  return true;
}

void EventLoopPrivate::quit(int exitCode)
{
  auto quit_signal = [this, exitCode]() {
    sd_event_exit(m_eventloop, exitCode);
  };

  invokeMethod(quit_signal);
}

/*
NOTE: thread local

`m_loopRunning` is set in run().

Use of `m_loopRunning` and sem(semaphore) means flush() can be called from 
a different thread than a thread running event loop.


1. EventLoopPrivate *m_loopRunning;
member variable and single entity. when two threads use the same EventLoop,
m_loopRunning has the same single value since use the same EventLoopPrivate.

2. static EventLoopPrivate *m_loopRunning;
same as 1 since `static` only make it available without making instance.

3. static thread_local EventLoopPrivate *m_loopRunning;

So how can differentiate threads? 

By using `thread_local`, a thread which runs EventLoop will have a copy of
m_loopRunning since run() is where it's used first time and gets create/set but
other threads which do not call run() and only uses public interface will not
have this variable set and it'll be null.


So deadlock will happen since when flush() gets called from different thread,
m_loopRunning is null and take path when calling thread is different from
running thread. call event_handler_() which locks on the same recursive mutex so
blocks.

On gdb, confirms that and this can be the other way around; t2 can be blocked.

Thread 1 (Thread 0x7ffff7f3d280 (LWP 6137)):
#0  __lll_lock_wait () at ../sysdeps/unix/sysv/linux/x86_64/lowlevellock.S:135
#1  0x00007ffff7328c26 in __GI___pthread_mutex_lock (mutex=0x55555580a4b0) at ../nptl/pthread_mutex_lock.c:115
#2  0x000055555557afb5 in __gthread_mutex_lock (__mutex=0x55555580a4b0) at /usr/include/x86_64-linux-gnu/c++/6/bits/gthr-default.h:748
#3  0x000055555557b005 in __gthread_recursive_mutex_lock (__mutex=0x55555580a4b0) at /usr/include/x86_64-linux-gnu/c++/6/bits/gthr-default.h:810
#4  0x000055555557c4ea in std::recursive_mutex::lock (this=0x55555580a4b0) at /usr/include/c++/6/mutex:105
#5  0x000055555557cab6 in std::lock_guard<std::recursive_mutex>::lock_guard (this=0x7fffffffd640, __m=...) at /usr/include/c++/6/bits/std_mutex.h:162
#6  0x000055555557b64d in EventLoopPrivate::eventHandler_ (source=0x55555580ac20, fd=4, revents=1, data=0x55555580a4a0) at /home/keitee/git/kb/code-cxx/os-dbus/sdbus-case/eventloop1/eventloop.cpp:115
#7  0x00007ffff7f65b03 in ?? () from /lib/x86_64-linux-gnu/libsystemd.so.0
#8  0x00007ffff7f667e4 in sd_event_dispatch () from /lib/x86_64-linux-gnu/libsystemd.so.0
#9  0x00007ffff7f67a87 in sd_event_run () from /lib/x86_64-linux-gnu/libsystemd.so.0
#10 0x00007ffff7f99abb in sd_event_loop () from /lib/x86_64-linux-gnu/libsystemd.so.0
#11 0x000055555557b4ad in EventLoopPrivate::run (this=0x55555580a4a0) at /home/keitee/git/kb/code-cxx/os-dbus/sdbus-case/eventloop1/eventloop.cpp:81

Thread 2 (Thread 0x7ffff5d7c700 (LWP 6144)):
#0  0x00007ffff732e556 in futex_abstimed_wait_cancelable (private=0, abstime=0x0, expected=0, futex_word=0x7ffff5d7b7e0) at ../sysdeps/unix/sysv/linux/futex-internal.h:205
#1  do_futex_wait (sem=sem@entry=0x7ffff5d7b7e0, abstime=0x0) at sem_waitcommon.c:111
#2  0x00007ffff732e604 in __new_sem_wait_slow (sem=0x7ffff5d7b7e0, abstime=0x0) at sem_waitcommon.c:181
#3  0x000055555557bb0c in EventLoopPrivate::flush (this=0x55555580a4a0) at /home/keitee/git/kb/code-cxx/os-dbus/sdbus-case/eventloop1/eventloop.cpp:213
#4  0x000055555557bce2 in EventLoop::flush (this=0x7fffffffd7d0) at /home/keitee/git/kb/code-cxx/os-dbus/sdbus-case/eventloop1/eventloop.cpp:260


NOTE: 
recursive lock? means that:

1. it is possible to be called (dispatched) while handling the one? Is there
really a case where running thread flush itself?

2. while serving or dispatching functor, it is possible to call invokeMethond()
which pushes item to queue. (from the same thread)

this is used in client interfaces as well and recursive lock only works for
the same thread. Uses recursive lock for this case which seems to be rare.

*/

void EventLoopPrivate::flush()
{
#ifndef DEADLOCK_FIX
  // orginal code
  std::lock_guard<std::recursive_mutex> lock(m_rm);
#endif

  // if calling thread is the same which runs eventloop, the process all
  if (this == m_loopRunning)
  {

#ifdef DEADLOCK_FIX
    std::lock_guard<std::recursive_mutex> lock(m_rm);
#endif

#ifdef EVENTLOOP_DEBUG
    logWarning("flush called from smae thread(%ld)"
               "m_loopRunning(0x%p)",
               std::this_thread::get_id(),
               m_loopRunning);
#endif

    while (!m_q.empty())
    {
      const auto f = m_q.front();
      if (f)
        f();

      m_q.pop();
    }

    return;
  }

  // otherwise post a f to the event loop on different thread/process and wait
  // until it runs

#ifdef EVENTLOOP_DEBUG
  logWarning("flush called from different thread(%ld), "
             "m_loopRunning(0x%p)",
             std::this_thread::get_id(),
             m_loopRunning);
#endif

  sem_t sem;
  sem_init(&sem, 0, 0);

  // NOTE: this lambda is "self-contained" since sem is local and lambda will
  // run on different thread?

  auto flushLambda = [&]() {
    logWarning("sem_post");

    if (sem_post(&sem) != 0)
      logSysError(errno, "failed to post sem in flush");
  };

  // unnecessary.
  // if (!invokeMethod(std::move(flushLambda)))

  if (!invokeMethod(flushLambda))
    logError("failed to post flush");
  else if (sem_wait(&sem) != 0)
    logSysError(errno, "failed to wait sem in flush");

  sem_destroy(&sem);
}

size_t EventLoopPrivate::size() const
{
  return m_q.size();
}

/* ={--------------------------------------------------------------------------
 @brief :
  EventLoop
*/

EventLoop::EventLoop()
    : m_private(std::make_shared<EventLoopPrivate>())
{}

EventLoop::EventLoop(const EventLoop &other)
    : m_private(other.m_private)
{}

EventLoop::EventLoop(EventLoop &&other)
    : m_private(std::move(other.m_private))
{}

EventLoop::~EventLoop()
{
  m_private.reset();
}

// NOTE:
// no need to check on m_private since it's constructed when EventLoop is
// created.

int EventLoop::run()
{
  return m_private->run();
}

void EventLoop::quit(int exitCode)
{
  m_private->quit(exitCode);
}

void EventLoop::flush()
{
  m_private->flush();
}

size_t EventLoop::size() const
{
  return m_private->size();
}

// NOTE:
// access private member so make it friend or add public member functions
// to it.

sd_event *EventLoop::handle() const
{
  return m_private->m_eventloop;
}

bool EventLoop::onEventLoopThread() const
{
  return (EventLoopPrivate::m_loopRunning == m_private.get());
}

bool EventLoop::invokeMethodImpl(std::function<void()> &&f) const
{
  // when use:
  //
  // template <typename F>
  // inline bool invokeMethod(F &&f) const
  // {
  //   return invokeMethodImpl(f);
  //   or
  //   return invokeMethodImpl(std::forward<F>(f));
  // }
  //
  // : error: cannot bind ‘std::function<void()>’ lvalue
  // to ‘std::function<void()>&&’
  //
  // return m_private->invokeMethod(f);

  return m_private->invokeMethod(std::move(f));
}
