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
    int
    eventHandler(sd_event_source *source, int fd, uint32_t revents, void *data);

  private:
    sd_event *m_loop{nullptr};
    int m_eventfd{-1};

    // TODO static?
    thread_local EventLoopPrivate *m_loopRunning{nullptr};

    // TODO mutable?
    mutable std::recursive_mutex m_rm;

    std::queue<std::function<void()>> m_methods;
};

#endif // EVENTLOOP_P_H
