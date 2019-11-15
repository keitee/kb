#ifndef EVENTLOOP_P_H
#define EVENTLOOP_P_H

#include <systemd/sd-event.h>

class EventLoopPrivate
{
  public:
    EventLoopPrivate();
    ~EventLoopPrivate();

    int exec();
    void quit(int exitCode);
    bool invokeMethod(std::function<void()> &&f);
    void flush();

  private:
    sd_event *m_loop{nullptr};
    int m_eventfd{-1};
    thread_local EventLoopPrivate *m_loopRunning{nullptr};

    mutable std::recursive_mutex m_rm;
    std::queue<std::function<void()>> m_methods;
};

#endif // EVENTLOOP_P_H
