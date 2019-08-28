#include "dbustimeouts.h"

DbusTimeouts::DbusTimeouts(DBusConnection *conn)
  : conn_(conn)
    , timer_fd_(-1)
    , within_event_handler_(false)
    , expected_tid_(std::this_thread::get_id())
{
  // create a timerfd for adding to the poll loop and for using timeout wake-ups
  timer_fd_ = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
  if (timer_fd_ < 0)
  {
    std::cout << "fatal: failed to create timerfd" << std::endl;
    return;
  }

  // set the timeout functions that are responsible for making the dispatcher
  // thread aware of timeouts

  /*
  Sets the timeout functions for the connection.

  These functions are responsible for making the application's main loop aware
  of timeouts. When using Qt, typically the DBusAddTimeoutFunction would create
  a QTimer. When using GLib, the DBusAddTimeoutFunction would call
  g_timeout_add.

  The DBusTimeoutToggledFunction notifies the application that the timeout has
  been enabled or disabled. Call dbus_timeout_get_enabled() to check this. A
  disabled timeout should have no effect, and enabled timeout should be added to
  the main loop. This feature is used instead of simply adding/removing the
  timeout because enabling/disabling can be done without memory allocation. 
  
  With Qt, QTimer::start() and QTimer::stop() can be used to enable and disable.
  The toggled function may be NULL if a main loop re-queries
  dbus_timeout_get_enabled() every time anyway. Whenever a timeout is toggled,
  its interval may change.

  The DBusTimeout can be queried for the timer interval using
  dbus_timeout_get_interval(). dbus_timeout_handle() should be called
  repeatedly, each time the interval elapses, starting after it has elapsed
  once. The timeout stops firing when it is removed with the given
  remove_function. The timer interval may change whenever the timeout is added,
  removed, or toggled.

  Note
  The thread lock on DBusConnection is held while timeout functions are invoked,
  so inside these functions you may not invoke any methods on DBusConnection or
  it will deadlock. See the comments in the code or
  http://lists.freedesktop.org/archives/dbus/2007-July/thread.html#8144 if you
  encounter this issue and want to attempt writing a patch.

  Parameters
  connection          the connection.
  add_function        function to add a timeout.
  remove_function     function to remove a timeout.
  toggled_function    function to notify of enable/disable
  data                data to pass to add_function and remove_function.
  free_data_function  function to be called to free the data.

  Returns
  FALSE on failure (no memory)

  Definition at line 5013 of file dbus-connection.c.

  */

  dbus_bool_t status = dbus_connection_set_timeout_function(conn,
     add_timeout_cb_,
     remove_timeout_cb_,
     toggle_timeout_cb_,
     this, NULL);
}


DbusTimeouts::~DbusTimeouts()
{
  // debugging check to ensure it's being called from the correct thread
  if (expected_tid_ != std::this_thread::get_id())
  {
    std::cout << "fatal: called from the wrong thread" << std::endl;
  }

  // clear all callbacks
  dbus_connection_set_timeout_function(conn,
      NULL, NULL, NULL, NULL, NULL);
}

// (private)
dbus_bool_t DbusTimeouts::add_timeout_cb_(DBusTimeout *timeout, void *data)
{
  DbusTimeouts *self = std::reinterpret_cast<DbusTimeouts*>(data);
  return self->add_timeout_(timeout);
}

// (private)
void DbusTimeouts::toggle_timeout_cb_(DBusTimeout *timeout, void *data)
{
  DbusTimeouts *self = std::reinterpret_cast<DbusTimeouts*>(data);
  return self->toggle_timeout_(timeout);
}

// (private)
void DbusTimeouts::remove_timeout_cb_(DBusTimeout *timeout, void *data)
{
  DbusTimeouts *self = std::reinterpret_cast<DbusTimeouts*>(data);
  return self->remove_timeout_(timeout);
}

// (private)
// calculate a new timespec based on now and the supplied offset in ms
struct timespec DbusTimeouts::calculate_abs_time_(struct timespec const &base,
    int milliseconds) const
{
#define NSECS_PER_MSEC  1000000L
#define NSECS_PER_SEC   1000000000L

  // timespec supports nanoseconds
  struct timespec ts;

  if (milliseconds <= 0)
  {
    std::cout << "warning: timeout milliseconds is <= 0" << std::endl;
    ts = base;
  }
  else
  {
    ts.tv_sec = base.tv_sec + (milliseconds / 1000u);
    ts.tv_nsec = base.tv_nsec + ((milliseconds % 1000u) * NSECS_PER_MSEC);

    if (ts.tv_nsec > NSECS_PER_SEC)
    {
      ts.tv_nsec -= NSECS_PER_SEC;
      ts.tv_sec += 1;
    }
  }

  return ts;
}


// (private)
// write the item at the head of timeout queue into timerfd for the next
// wake-up. If the queue is empty then 0 is written which disable timer.
void DbusTimeouts::update_timer_() const
{
  struct itimerspec its;
  its.it_interval.tv_sec = 0;
  its.it_interval.tv_nsec = 0;

  // by default disable the timerfd
  its.it_value.tv_sec = 0;
  its.it_value.tv_nsec = 0;

  // check we have any timeouts
  if (!timeouts_.empty())
  {
    // if so use the head of the queue for the timeout
    its.it_value = timeouts_.front().expiry;
  }

  // finally update the timerfd to either; disable it or set to the next
  // expiry value
  if (timerfd_settime(timer_fd_, TFD_TIMER_ABSTIME, &its, NULL) != 0)
  {
    std::cout << "error: failed to set timerfd" << std::endl;
  }
}


// (private)
// add the given timeout to the timerfd poll
//
// return TRUE if the timeout is added, otherwise FALSE
dbus_bool_t DbusTimeouts::add_timeout_(DBusTimeout *timeout)
{
  // debugging check to ensure it's being called from the correct thread
  //
  // Q: should it be libdbus thread?

  if (expected_tid_ != std::this_thread::get_id())
  {
    std::cout << "fatal: called from the wrong thread" << std::endl;
  }

  // the timeout is enabled? if not, ignore
  if (!dbus_timeout_get_enabled(timeout))
  {
    std::cout << "error: libdbus trying to add disabled timeout" << std::endl;
    return FALSE;
  }

  int interval = dbus_timeout_get_interval(timeout);
  if (interval <= 0)
  {
    std::cout << "error: libdbus trying to add timeout with invalid interval " 
      interval << std::endl;
    return FALSE;
  }

  TimeoutEntry entry;
  entry.timeout_ = timeout;

  // get the expiry time of the given timeout
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  entry.expiry_ = calculate_abs_time_(now, interval);

  // Q: move() has an effect?
  timeouts_.emplace_front(std::move(entry));

  // if not currently processing the timeouts then we may need to update the
  // timerfd since this element might now be the next item to expire
  if (!within_event_handler)
  {
    // to have the next to expire(the earliest) at the head
    timeouts_.sort();

    // update timer if the new head of the list is the one we just added.
    // Q: why this check?

    if (timeouts_.front().timeout_ == timeout)
      update_timer_();
  }

  return TRUE;
}


// toggles the enable/disable state of A timeout
void DbusTimeouts::toggle_timeout_(DBusTimeout *timeout)
{
  // debugging check to ensure it's being called from the correct thread
  if (expected_tid_ != std::this_thread::get_id())
  {
    std::cout << "fatal: called from the wrong thread" << std::endl;
  }

  // ???
  // if in the middle of processing the timeouts then we don't need to do
  // anything as the code checks if the timeout is enabled at the end of
  // the processing loop

  if (within_event_handler_)
    return;

  // find the timeout in the list
  auto it = timeouts_.begin();
  for (; it != timeouts_.end(); ++it)
  {
    if (it->timeout_ == timeout)
      break;
  }

  if (timeouts_.end() == it)
  {
    std::cout << "error: failed to find timeout to remove" << std::endl;
    return;
  }

  auto interval = dbus_timeout_get_interval(timeout);

  // timeout is *now* enabled and set the expiry
  if (dbus_timeout_get_enabled(timeout) && (interval > 0))
  {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    it->expiry_ = calculate_abs_time(timeout, interval);

    // since the order of timeouts may have changed.
    timeouts_.sort();
  }
  // ???
  // remove?
  else
  {
    timeouts_.erase(it);
  }

  // do since it could have toggled the first entry in the queue.
  update_timer_();
}


// remove A timeout
void DbusTimeouts::remove_timeout_(DBusTimeout *timeout)
{
  // debugging check to ensure it's being called from the correct thread
  if (expected_tid_ != std::this_thread::get_id())
  {
    std::cout << "fatal: called from the wrong thread" << std::endl;
  }

  // find the timeout in the list
  auto it = timeouts_.begin();
  for (; it != timeouts_.end(); ++it)
  {
    if (it->timeout_ == timeout)
      break;
  }

  if (timeouts_.end() == it)
  {
    std::cout << "error: failed to find timeout to remove" << std::endl;
    return;
  }

  // we're in the middle of processing the timeout list. don't remove A timeout
  // and instead set the timeout to nullptr then the processing loop will clean
  // it up.
  if (within_event_handler_)
  {
    it->timeout_ = nullptr;
    it->expiry_.tv_sec = INT_MAX;
  }
  // not in the processing and just remove the entry. In theory, should also
  // update the timer but we only need to do this if we're removing the next
  // expiry. 
  //
  // it's probably just more efficient to let the timer expire and handle it in
  // the processing.
  else
  {
    timeouts_.erase(it);
  }
}
