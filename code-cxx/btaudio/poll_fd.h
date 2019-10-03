#ifndef POLL_FD_H
#define POLL_FD_H

#include <inttypes.h>
#include <map>
#include <poll.h>
#include <pthread.h>
#include <vector>

typedef uint64_t PollFDTag;
#define PollFDTagInvalid 0

/**
 * @class PollFDEventListener
 * Abstract listener class for notification of file descriptor IO events.
 */
class PollFDEventListener
{
public:
  virtual ~PollFDEventListener(){};
  virtual void onEvent(int fd, int event, void *dptr) = 0;
};

/**
 * @class Poller
 * Poller provides file-descriptor polling with per-fd listeners.
 */
class Poller : public PollFDEventListener
{
public:
  Poller();
  ~Poller();

  PollFDTag addFD(int fd, int flags, bool enabled,
                  PollFDEventListener *listener, void *dptr);
  bool enableFD(PollFDTag tag, bool isEnabled);
  bool removeFD(PollFDTag tag);

  void doPoll(int timeout_ms);
  void interruptPoll();

  void onEvent(int fd, int event, void *dptr);

private:
  class PollFD
  {
  public:
    PollFD()
        : m_fd(-1), m_flags(0), m_enabled(false), m_removed(false),
          m_listener(NULL), m_dptr(NULL){};
    PollFD(int fd, int flags, bool enabled, PollFDEventListener *listener,
           void *dptr)
        : m_fd(fd), m_flags(flags), m_enabled(enabled), m_removed(false),
          m_listener(listener), m_dptr(dptr){};
    virtual ~PollFD(){};

    int getFD() { return m_fd; };
    int getFlags() { return m_flags; };
    bool isEnabled() { return m_enabled; };
    bool isRemoved() { return m_removed; };

    void setEnabled(bool is_enabled) { m_enabled = is_enabled; };
    void setRemoved() { m_removed = true; };
    void fire(int event)
    {
      if (m_listener)
        m_listener->onEvent(m_fd, event, m_dptr);
    };

  private:
    int m_fd;
    int m_flags;
    bool m_enabled;
    bool m_removed;
    PollFDEventListener *m_listener;
    void *m_dptr;
  };

  void clearInterrupt();
  bool populatePollArray(nfds_t *nfds);
  void firePollCallbacks();
  void removeStalePollFDs();

  PollFDTag m_tag_seed;
  int m_pipe_fds[2];
  nfds_t m_poll_array_size;
  struct pollfd *m_poll_array;
  std::map<PollFDTag, PollFD> m_poll_fd_map;
  std::map<int, std::vector<PollFD *>> m_poll_fd_reverse_map;
  pthread_mutex_t m_lock;
};
#endif // POLL_FD_H