#ifndef EVENTLOOP_P_H
#define EVENTLOOP_P_H

// NOTE: use typedef and "include <systemd/sd-event.h>" has the same typedef.
// sd-event.h do not have full structure and only have typedef. How possible?
// since client uses only pointers.

typedef struct sd_event sd_event;
typedef struct sd_event_source sd_event_source;

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

  std::recursive_mutex m_rm;

  std::queue<std::function<void()>> m_q;

  // see flush()
  // 1. EventLoopPrivate *m_loopRunning;
  // 2. static EventLoopPrivate *m_loopRunning;

  static thread_local EventLoopPrivate *m_loopRunning;

  // NOTE: static do not honor access control
  static int
  event_handler_(sd_event_source *s, int fd, uint32_t revents, void *userdata);

public:
  explicit EventLoopPrivate();
  ~EventLoopPrivate();

  int run();
  void quit(int exitCode);
  bool invokeMethod(std::function<void()> &&f);
  void flush();

  // debug
  size_t size() const;
};

#endif // EVENTLOOP_P_H
