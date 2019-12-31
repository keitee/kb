// #include "eventloop_p.h"
#include "eventloop.h"
// #include <systemd/sd-event.h>
// use lpi log
#include "rlog.h"

#include <cassert>
#include <semaphore.h>
#include <sys/eventfd.h>
#include <unistd.h>

/* ={--------------------------------------------------------------------------
 @brief :
  EventLoopPrivate
*/

// since it is static
thread_local EventLoopPrivate *EventLoopPrivate::m_loopRunning{nullptr};

EventLoopPrivate::EventLoopPrivate()
    : m_eventloop{nullptr}
    , m_eventfd{-1}
{
  int rc = sd_event_new(&m_eventloop);
  if (rc < 0)
  {
    // use rlog
    logSysError(-rc, "failed to create new event loop");
    return;
  }

  m_eventfd = eventfd(0, EFD_CLOEXEC);
  if (m_eventfd < 0)
  {
    logSysError(-rc, "failed to create new eventfd");
  }
}

EventLoopPrivate::~EventLoopPrivate()
{
  if ((m_eventfd >= 0) && (close(m_eventfd) != 0))
  {
    logSysError(errno, "failed to close eventfd");
  }

  if (m_eventloop)
  {
    // sd_event_unref() always returns NULL.
    m_eventloop = sd_event_unref(m_eventloop);
  }
}

// int EventLoopPrivate::exec()
int EventLoopPrivate::run()
{
  if (!m_eventloop)
  {
    logWarning("no event loop to start");
    return EXIT_FAILURE;
  }

  sd_event_source *quitEventSource{nullptr};

  int rc = sd_event_add_io(m_eventloop,
                           &quitEventSource,
                           m_eventfd,
                           EPOLLIN,
                           eventHandler_,
                           this);
  if (rc < 0)
  {
    logSysError(-rc, "failed to attach quit eventfd source");
    return EXIT_FAILURE;
  }

  // TODO: set the thread local pointer back to us. ???
  m_loopRunning = this;

  // not use rc to say explicit that it is exit code from event loop
  int exitCode = sd_event_loop(m_eventloop);

  m_loopRunning = nullptr;

  sd_event_source_unref(quitEventSource);

  return exitCode;
}

// TODO: cannot find the detail of return value in the document
int EventLoopPrivate::eventHandler_(sd_event_source *source,
                                    int fd,
                                    uint32_t revents,
                                    void *data)
{
  EventLoopPrivate *self = static_cast<EventLoopPrivate *>(data);

  assert(nullptr != self);
  assert(fd == self->m_eventfd);

  uint64_t ignore;

  if (sizeof(ignore) !=
      TEMP_FAILURE_RETRY(read(self->m_eventfd, &ignore, sizeof(ignore))))
  {
    logSysError(errno, "failed to read from eventfd");
  }

  // take the lock and process *ALL* in the queue
  {
    // TODO: recursive_mutex???
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

// quits the event loop which unblocks run() function.
void EventLoopPrivate::quit(int exitCode)
{
  auto quitLambda = [this, exitCode]() {
    sd_event_exit(m_eventloop, exitCode);
  };

  invokeMethod(quitLambda);
}

// post()
bool EventLoopPrivate::invokeMethod(std::function<void()> &&f)
{
  // NOTE: use block to release lock
  {
    std::lock_guard<std::recursive_mutex> lock(m_rm);
    m_q.emplace(std::move(f));
  }

  uint64_t wakeup{1};

  if (sizeof(wakeup) !=
      TEMP_FAILURE_RETRY(write(m_eventfd, &wakeup, sizeof(wakeup))))
  {
    logSysError(errno, "failed to write to eventfd");
    return false;
  }

  return true;
}

void EventLoopPrivate::flush()
{
  std::lock_guard<std::recursive_mutex> lock(m_rm);

  // if called on the event loop thread
  if (this == m_loopRunning)
  {
    while (!m_q.empty())
    {
      const auto f = m_q.front();
      if (f)
        f();

      m_q.pop();
    }

    return;
  }

  // TODO:
  // otherwise post a f to the event loop on different thread/process and wait
  // until it runs

  logWarning("flush runs on different thread/process");

  sem_t sem;
  sem_init(&sem, 0, 0);

  auto flushLambda = [&]() {
    if (sem_post(&sem) != 0)
      logSysError(errno, "failed to post sem in flush");
  };

  if (invokeMethod(std::move(flushLambda)))
    logError("failed to post flush");
  else if (sem_wait(&sem) != 0)
    logSysError(errno, "failed to wait sem in flush");

  sem_destroy(&sem);
}

/* ={--------------------------------------------------------------------------
 @brief :
  EventLoop
*/

EventLoop::EventLoop()
  : m_private(std::make_shared<EventLoopPrivate>())
{}

EventLoop::~EventLoop()
{
  m_private.reset();
}

EventLoop::EventLoop(const EventLoop &rhs)
  : m_private(rhs.m_private)
{}

EventLoop::EventLoop(EventLoop &&rhs)
  : m_private(rhs.m_private)
{}

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

bool EventLoop::invokeMethodImpl(std::function<void()> &&f) const
{
  // return m_private->invokeMethod(f);
  return m_private->invokeMethod(std::move(f));
}

// access private member of EventLoopPrivate
sd_event *EventLoop::handle() const
{
  return m_private->m_eventloop;
}

// access private member of EventLoopPrivate
bool EventLoop::onEventLoopThread() const
{
  return (EventLoopPrivate::m_loopRunning == m_private.get());
}
