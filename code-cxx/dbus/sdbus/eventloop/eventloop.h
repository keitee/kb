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

    // TODO
    // return boolean if the calling thread is the same thread that runs the
    // event loop. WHY is onXXX()?
    bool onEventLoopThread() const;

    template <typename F>
      bool invokeMethod(F &&func) const
      {
        return this->invokeMethodImpl(
            std::forward<F>(func)
            );
      }

    template <typename F, typename... Args>
      bool invokeMethod(F &&func, Args &&... args) const
      {
        return this->invokeMethodImpl(
            std::bind(std::forward<F>(func),
              std::forward<Args>(args)...)
            );
      }

  private:
    void invokeMethodImpl(std::function<void()> &&func) const;

  private:
    std::shared_ptr<EventLoopPrivate> m_private;
};

#endif // EVENTLOOP_H

