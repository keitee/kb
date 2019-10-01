#if !defined(__STDC_FORMAT_MACROS)
#define __STDC_FORMAT_MACROS
#endif
#include "poll_fd.h"
#include "locker.h"
#include <algorithm>
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>

#include "AS_Diag.h"
extern AS_DIAG::Context *dbusitf_logging_ctx;
#undef AS_DIAG_CONTEXT_DEFAULT
#define AS_DIAG_CONTEXT_DEFAULT (dbusitf_logging_ctx)

/**
 * @file poll_fd.cpp
 *
 * Poller's internal implementation is somewhat subtle and worth briefly
 * describing here.  The way file descriptors are tracked has been driven by the
 * following design goals for Poller.
 *
 * Firstly, it needs to support the requirements of libdbus's "watches".  These
 * are file descriptors which can be watched for events, and which libdbus can
 * enable and disable at any point.  For reasons of its own, libdbus may add
 * multiple watches for the same file descriptor.  Each watch may be
 * independently enabled.
 *
 * Secondly, Poller needs to avoid edge-cases around descriptor addition and
 * removal in a threaded environment.  In particular, it shouldn't be possible
 * to leave NULL pointers floating about if a descriptor is removed while a poll
 * is in process, or if a descriptor is removed as a part of a callback on an IO
 * event.
 *
 * Finally, Poller needs to be reasonably performant in the face of a reasonable
 * number of file descriptors.  It needs to avoid doing high-cost translations
 * for mapping descriptors back to listener callbacks, for example.
 *
 * Of these goals, the second informs the majority of Poller's design.  The key
 * decision here is to avoid exposing user code to any dynamically allocated
 * structure which could be freed without proper synchronisation.  Instead, when
 * a descriptor is added to Poller, a PollFD object is created internally, and a
 * uint64_t tag is created corresponding to that PollFD instance.  The client
 * code gets the tag back, and can use that tag to (indirectly) refer back to
 * the PollFD instance later on.  To store the mapping between the tag and the
 * PollFD, Poller uses std::map (m_poll_fd_map). Hence, when client code wants
 * to e.g. disable a descriptor, Poller uses the tag to reference into
 * m_poll_fd_map and retrieve the associated PollFD object.
 *
 * While m_poll_fd_map is the key data structure in Poller, containing all the
 * important state, at the lower levels poll(2) doesn't know or care about
 * m_poll_fd_map, it expects to see an array of struct pollfd.  To
 * impedance-match between the two, Poller maintains a dynamically allocated
 * array of struct pollfd (m_poll_array) which it updates before each poll(2)
 * call.  m_poll_array is populated based on the enabled PollFD descriptors and
 * their corresponding event flags.
 *
 * On return from poll, descriptors with events pending are flagged in
 * m_poll_array in the struct pollfd revents member.  Poller now needs to map
 * back from the file descriptor value to a PollFD instance in order to pass the
 * IO event to the client listener code. A naive approach might iterate
 * m_poll_fd_map to find the PollFD corresponding to each file descriptor with
 * events pending.  But recall design goals one and three!  Libdbus might add
 * multiple watches for a single file descriptor, so there's not a one-to-one
 * mapping between PollFD and a given file descriptor.  Furthermore, iterating
 * m_poll_fd_map is going to incur a costly overhead if a lot of file
 * descriptors have fired.  To avoid such problems, Poller maintains another
 * std::map which provides a reverse mapping (m_poll_fd_reverse_map) between a
 * file descriptor value and the corresponding PollFD structure which was
 * created at addFD() time.  This map stores a pointer to the PollFD structure
 * in m_poll_fd_map, keyed by the file descriptor.  Finding the PollFD matching
 * a file descriptor with pending events is then a trivial matter of indexing
 * m_poll_fd_reverse_map: quick, and efficient.
 *
 * The last subtle part of Poller's implementation revolves around the
 * relationship between m_poll_fd_map and m_poll_fd_reverse_map.  Poller needs
 * to take care to keep these two maps in sync as m_poll_fd_reverse_map holds
 * pointers to objects stored in m_poll_fd_map. Removing a PollFD instance from
 * m_poll_fd_map without updating m_poll_fd_reverse_map would potentially be
 * disasterous.  The decision to keep PollFD entirely internal to Poller makes
 * this easy to manage, however.  During normal use, any client code actions on
 * Poller are reflected in m_poll_fd_map using state variables in PollFD.
 * Crucially, they don't remove entries from m_poll_fd_map, nor do they alter
 * m_poll_fd_reverse_map.  These latter activities are both carried out by
 * removeStalePollFDs a part of the doPoll method after poll(2) returns.  At
 * this point, any PollFD instances flagged as having been removed are erased
 * from m_poll_fd_map and m_poll_fd_reverse_map.
 */
Poller::Poller() : m_tag_seed(128000), m_poll_array_size(1), m_poll_array(NULL)
{
  pthread_mutex_init(&m_lock, NULL);
  if (0 != pipe(m_pipe_fds))
  {
    AS_LOG_ERROR("pipe() failed: %s", strerror(errno));
  }
  else
  {
    addFD(m_pipe_fds[0], POLLIN | POLLPRI, true, this, NULL);
  }
}

Poller::~Poller()
{
  close(m_pipe_fds[0]);
  close(m_pipe_fds[1]);
  pthread_mutex_destroy(&m_lock);
  delete[] m_poll_array;
}

/**
 * @brief Add a file descriptor to the set of descriptors to be polled.
 * File descriptors can optionally be flagged as enabled or disabled.  Disabled
 * descriptors are held internally by Poller but won't be added to the poll
 * array before calling poll(2).
 * @param fd the file descriptor to add
 * @param flags I/O event flags you want to be notified about (this corresponds
 * to struct pollfd's events member)
 * @param enabled true if the fd should be enabled, false otherwise
 * @param listener pointer to a PollFDEventListener to receive notifications of
 * fd events
 * @param dptr optional opaque pointer which will be passed to the listener's
 * onEvent callback
 * @return PollFDTagInvalid on failure, otherwise a valid tag which can be used
 * in further Poller calls
 */
PollFDTag Poller::addFD(int fd, int flags, bool enabled,
                        PollFDEventListener *listener, void *dptr)
{
  Locker lock(&m_lock);
  PollFD pfd(fd, flags, enabled, listener, dptr);
  m_poll_fd_map[m_tag_seed] = pfd;
  m_poll_fd_reverse_map[fd].push_back(&m_poll_fd_map[m_tag_seed]);
  AS_LOG_INFO("fd %d : %s : tag %" PRIu64, fd, enabled ? "enabled" : "disabled",
              m_tag_seed);
  return m_tag_seed++;
}

/**
 * @brief Enable/disable a file descriptor held by Poller
 * @param tag valid descriptor tag identifying the descriptor to enable/disable
 * @param isEnabled true if descriptor should be enabled, false otherwise
 * @return true on success, false otherwise
 */
bool Poller::enableFD(PollFDTag tag, bool isEnabled)
{
  AS_LOG_INFO("tag %" PRIu64 " : %s", tag, isEnabled ? "enabled" : "disabled");
  Locker lock(&m_lock);
  if (m_poll_fd_map.count(tag))
  {
    if (!m_poll_fd_map[tag].isRemoved())
    {
      m_poll_fd_map[tag].setEnabled(isEnabled);
      return true;
    }
    else
    {
      AS_LOG_ERROR("tag %" PRIu64 " has been removed", tag);
      return false;
    }
  }
  AS_LOG_ERROR("no such tag %" PRIu64, tag);
  return false;
}

/**
 * @brief Remove a file descriptor from Poller
 * @param tag valid descriptor tag identifying the descriptor to remove
 * @return true on success, false otherwise
 */
bool Poller::removeFD(PollFDTag tag)
{
  AS_LOG_INFO("tag %" PRIu64, tag);
  Locker lock(&m_lock);
  if (m_poll_fd_map.count(tag))
  {
    m_poll_fd_map[tag].setRemoved();
    return true;
  }
  AS_LOG_ERROR("no such tag %" PRIu64, tag);
  return false;
}

/**
 * @brief Call poll(2) on the currently enabled descriptors held by Poller
 * @param timeout_ms the poll timeout in milliseconds as per poll(2)
 */
void Poller::doPoll(int timeout_ms)
{
  nfds_t nfds = 0;
  int ret;

  if (populatePollArray(&nfds))
  {
    if (nfds > 0)
    {
      ret = poll(m_poll_array, nfds, timeout_ms);
      if (ret < 0)
      {
        AS_LOG_ERROR("poll error: %s", strerror(errno));
      }
      else if (ret > 0)
      {
        // some fds have events pending
        firePollCallbacks();
      }
      removeStalePollFDs();
    }
    else
    {
      AS_LOG_ERROR("poll called with no fds");
    }
  }
  else
  {
    AS_LOG_ERROR("failed to populate poll array");
  }
}

/**
 * @brief Interrupt a thread blocking in doPoll()
 */
void Poller::interruptPoll()
{
  char buffer  = 'a';
  bool written = false;
  while (!written)
  {
    ssize_t nb = write(m_pipe_fds[1], &buffer, 1);
    if (nb == 1)
      written = true;
    else if (nb < 0)
    {
      AS_LOG_ERROR("write() to pipe failed: %s", strerror(errno));
    }
  }
}

void Poller::clearInterrupt()
{
  char buffer;
  bool reddit = false;
  while (!reddit)
  {
    ssize_t nb = read(m_pipe_fds[0], &buffer, 1);
    if (nb == 1)
      reddit = true;
    else if (nb < 0)
    {
      AS_LOG_ERROR("read() from pipe failed: %s", strerror(errno));
    }
  }
}

/**
 * @brief PollFDEventListener callback
 * For internal use only
 */
void Poller::onEvent(int fd, int event, void *dptr)
{
  if (event & POLLIN)
    clearInterrupt();
}

bool Poller::populatePollArray(nfds_t *nfds)
{
  Locker lock(&m_lock);

  // resize poll array?
  if (m_poll_fd_map.size() >= m_poll_array_size)
  {
    delete[] m_poll_array;
    while (m_poll_fd_map.size() >= m_poll_array_size)
      m_poll_array_size *= 2;
    m_poll_array = new pollfd[m_poll_array_size];
    if (!m_poll_array)
      return false;
  }

  // populate with fds and event masks from the PollFD map
  std::map<PollFDTag, PollFD>::iterator it;
  nfds_t i = 0;
  memset(m_poll_array, 0, m_poll_array_size * sizeof(struct pollfd));
  for (it = m_poll_fd_map.begin(); it != m_poll_fd_map.end(); ++it)
  {
    if (it->second.isEnabled() && !it->second.isRemoved())
    {
      m_poll_array[i].fd     = it->second.getFD();
      m_poll_array[i].events = it->second.getFlags();
      i++;
    }
  }

  if (nfds)
    *nfds = i;
  return true;
}

void Poller::firePollCallbacks()
{
  for (size_t i = 0; i < m_poll_array_size; i++)
  {
    if (m_poll_array[i].revents)
    {
      // no lock around map/vector access since the only thing another
      // thread can alter is enabled or removed state at this point
      if (m_poll_fd_reverse_map.count(m_poll_array[i].fd))
      {
        std::vector<PollFD *> pollfds =
            m_poll_fd_reverse_map[m_poll_array[i].fd];
        std::vector<PollFD *>::iterator it;
        for (it = pollfds.begin(); it != pollfds.end(); ++it)
        {
          bool fire;
          {
            Locker lock(&m_lock);
            fire = ((*it)->isEnabled() && !(*it)->isRemoved());
          }
          if (fire)
            (*it)->fire(m_poll_array[i].revents);
        }
      }
      else
      {
        AS_LOG_ERROR("no reverse lookup for fd %d", m_poll_array[i].fd);
      }
    }
  }
}

void Poller::removeStalePollFDs()
{
  Locker lock(&m_lock);

  std::map<PollFDTag, PollFD>::iterator it = m_poll_fd_map.begin();

  while (it != m_poll_fd_map.end())
  {
    if (it->second.isRemoved())
    {

      // remove from reverse map
      int fd = it->second.getFD();
      std::vector<PollFD *>::iterator iit =
          std::find(m_poll_fd_reverse_map[fd].begin(),
                    m_poll_fd_reverse_map[fd].end(), &(it->second));
      if (iit != m_poll_fd_reverse_map[fd].end())
      {
        m_poll_fd_reverse_map[fd].erase(iit);
      }
      else
      {
        AS_LOG_ERROR("failed to locate reverse map lookup for fd %d", fd);
      }

      // remove from forward map
      {
        std::map<PollFDTag, PollFD>::iterator todel = it;
        ++it;
        m_poll_fd_map.erase(todel);
      }
    }
    else
      ++it;
  }
}
