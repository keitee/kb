#ifndef EVENTLOOP_P_H
#define EVENTLOOP_P_H

#include <functional>
#include <mutex>
#include <queue>

class EventLoopPrivate
{
  private:
    friend class EventLoop;

  private:
    sd_event *m_eventloop;
    int m_eventfd;

    mutable std::recursive_mutex m_rm;
    std::queue<std::function<void()>> m_q;

    static thread_local EventLoopPrivate *m_loopRunning;

    // NOTE: static
    static int eventHandler_(sd_event_source *source,
                      int fd,
                      uint32_t revents,
                      void *data);

  public:
    explicit EventLoopPrivate();
    ~EventLoopPrivate();

    int run();
    void quit(int exitCode);
    bool invokeMethod(std::function<void()> &&f);
    void flush();
};

#endif // EVENTLOOP_P_H
