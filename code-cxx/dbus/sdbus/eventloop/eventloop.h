#ifndef EVENTLOOP_H
#define EVENTLOOP_H

class EventLoopPrivate;

class EventLoop
{
  public:
    EventLoop();
    EventLoop(const EventLoop &other);
    EventLoop(EventLoop &&other);
    ~EventLoop();

    sd_event *handle() const;
    int exec();
    void quit(int exitCode);
    void flush();

  private:
    void invokeMethodImpl(std::function<void()> &&func) const;

  private:
    std::shared_ptr<EventLoopPrivate> m_private;
};

#endif // EVENTLOOP_H

