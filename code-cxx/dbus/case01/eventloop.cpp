#include "eventloop.h"

/* ={--------------------------------------------------------------------------
 @brief :
  EventLoopPrivate
*/

EventLoopPrivate::EventLoopPrivate()
{
  // create a event loop
  int ret = sd_event_new(&m_loop);
  if (ret < 0)
  {
    LOG_MSG("failed to create new event loop. (%d)", ret);
    return;
  }

  m_eventfd = eventfd(0, EFD_CLOEXEC);
  if (m_eventfd < 0)
  {
    LOG_MSG("failed to create eventfd");
  }
}

EventLoopPrivate::~EventLoopPrivate()
{
  if ((m_eventfd >= 0) && (close(m_eventfd) != 0))
  {
    LOG_MSG("failed to close eventfd");
  }

  if (m_loop)
    sd_event_unref(m_loop);
}

int EventLoopPrivate::eventHandler(sd_event_source *source,
                                   int fd,
                                   uint32_t revents,
                                   void *data)
{
  // EventLoopPrivate *self = reinterpret_cast<EventLoopPrivate*>(data);
  EventLoopPrivate *self = static_cast<EventLoopPrivate*>(data);

  assert(self != nullptr);
  assert(fd == self->m_eventfd);

  // read the eventfd to clear the event
  uint64_t ignore;
  if (TEMP_FAILURE_RETRY(read(self->m_eventfd, &ignore, sizeof(ignore)))
      != sizeof(ignore))
  {
    LOG_MSG("failed to read from eventfd");
  }

  // take the lock and process all in the queue
  {
    std::lock_guard<std::recursive_mutex> lock(self->m_rm);
    while (!self->m_methods.empty())
    {
      // const std::function<void()> &fn = self->m_methods.front();
      auto const &fn = self->m_methods.front();
      if (fn)
        fn();

      self->m_methods.pop();
    }
  }
}

// start the event loop and this is a blocking call that won't return until the
// loop ends.
int EventLoopPrivate::exec()
{
  // sanity check
  if (!m_loop)
  {
    LOG_MSG("no event loop to start");
    return EXIT_FAILURE;
  }

  // install a handler for the quit eventfd. so expect eventHandler gets called
  // when eventfd can be read or is set.
  sd_event_source *source{nullptr};
  int ret = sd_event_add_io(
      m_loop,
      &source,
      m_eventfd,
      EPOLLIN,
      eventHandler,
      this);
  if (ret < 0)
  {
    LOG_MSG("failed to add quit eventfd");
    return EXIT_FAILURE;
  }

  // TODO set the thread local pointer back to us?
  m_loopRunning = this;

  // run the event loop
  int exitCode = sd_event_loop(m_loop);

  // clear as the event loop stopped
  m_loopRunning = nullptr;

  sd_event_source_unref(source);

  return exitCode;
}

// quit the event loop and exits ::exec(). It is safe to call this function from
// within the event loop thread or externally.
void EventLoopPrivate::quit(int exitCode)
{
  auto quitFunction = [this]() { sd_event_exit(m_loop, exitCode); };

  invokeMethod(quitFunction);
}

bool EventLoopPrivate::invokeMethod(std::function<void()> &&f) 
{
  // push the function onto the queue
  {
    std::lock_guard<std::recursive_mutex> lock(m_rm);
    m_methods.emplace(std::move(f));
  }

  // wake the event loop
  uint64_t wakeup = 1;
  if (TEMP_FAILURE_RETRY(write(m_eventfd, &wakeup, sizeof(wakeup)))
      != sizeof(wakeup));
  {
    LOG_MSG("failed to write to eventfd");
    return false;
  }
}

void EventLoopPrivate::flush()
{
  std::lock_guard<std::recursive_mutex> lock(m_rm);

  // if calls on the event loop thread, then execute all until the queue is
  // empty.

  if (EventLoopPrivate::m_loopRunning == this)
  {
    while (!m_methods.empty())
    {
      auto const &fn = m_methods.front();
      if (fn)
        fn();

      m_methonds.pop();
    }
  }
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

sd_event *EventLoop::handle() const
{
  return m_private->m_loop;
}

int EventLoop::exec()
{
  return m_private->exec();
}
