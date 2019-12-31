#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <systemd/sd-event.h>
#include "eventloop_p.h"
#include <memory>

// class EventLoopPrivate;

class EventLoop
{
private:
  std::shared_ptr<EventLoopPrivate> m_private;

  bool invokeMethodImpl(std::function<void()> &&f) const;

public:
  explicit EventLoop();
  explicit EventLoop(const EventLoop &);
  explicit EventLoop(EventLoop &&);
  ~EventLoop();

  int run();
  void quit(int exitCode);
  void flush();

  // access private member of EventLoopPrivate
  // get sd_event*
  sd_event *handle() const;

  // access private member of EventLoopPrivate
  // check if EventLoopPrivate of calling thread is the same as one of running.
  // that is if the calling thead is the same of running event loop
  bool onEventLoopThread() const;

public:
  template <typename F>
    inline bool invokeMethod(F &&f) const
    {
      // NOTE: unnecessary?
      // return invokeMethodImpl(std::forward<F>(f));
      return invokeMethodImpl(f);
    }

  template <typename F, typename... Args>
    inline bool invokeMethod(F &&f, Args&&... args) const
    {
      // NOTE: unnecessary?
      // return invokeMethodImpl(std::forward<F>(f));
      return invokeMethodImpl(f, std::forward<Args>(args)...);
    }
};

#endif // EVENTLOOP_H
