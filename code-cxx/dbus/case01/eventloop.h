#ifndef EVENTLOOP_H
#define EVENTLOOP_H

class EventLoop
{
  public:
    EventLoop();
    EventLoop(const EventLoop &other);
    EventLoop(EventLoop &&other);
    ~EventLoop();
  private:
    std::shared_ptr<EventLoopPrivate> m_private;
};

#endif // EVENTLOOP_H

