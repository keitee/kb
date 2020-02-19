#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <functional>
#include <memory>

// NOTE: use typedef and "include <systemd/sd-event.h>" has the same typedef.
// sd-event.h do not have full structure and only have typedef. How possible?
// since client uses only pointers.

typedef struct sd_event sd_event;

class EventLoopPrivate;

class EventLoop
{
private:
  // shared_ptr since it suppots copy and move ctors
  std::shared_ptr<EventLoopPrivate> m_private;

  bool invokeMethodImpl(std::function<void()> &&f) const;

public:
  explicit EventLoop();
  EventLoop(const EventLoop &);
  EventLoop(EventLoop &&);
  ~EventLoop();

  int run();
  void quit(int exitCode);
  void flush();

  // debug
  size_t size() const;

  sd_event *handle() const;

  // return true if th calling thread is the smae running eventloop
  bool onEventLoopThread() const;

public:
  template <typename F>
  inline bool invokeMethod(F &&f) const
  {
    // org:
    // return invokeMethodImpl(std::forward<F>(f));
    return invokeMethodImpl(f);
  }

  template <typename F, typename... Args>
  inline bool invokeMethod(F &&f, Args &&... args) const
  {
    return invokeMethodImpl(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...));
  }
};

#endif // EVENTLOOP_H
