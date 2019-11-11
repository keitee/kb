#include "eventloop.h"

/* ={--------------------------------------------------------------------------
 @brief :
  EventLoopPrivate
*/

EventLoopPrivate::EventLoopPrivate()
    : m_loop(nullptr)
    , m_eventfd(-1)
{
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
