#ifndef EVENTLOOP_P_H
#define EVENTLOOP_P_H

#include <systemd/sd-event.h>

class EventLoopPrivate
{
  public:
    EventLoopPrivate();
    ~EventLoopPrivate();

    int exec();

  private:
    sd_event *m_loop{nullptr};
    int m_eventfd{-1};
    thread_local EventLoopPrivate *m_loopRunning{nullptr};
};

#endif // EVENTLOOP_P_H
