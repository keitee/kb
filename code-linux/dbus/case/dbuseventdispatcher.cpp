#include "dbuseventdispatcher.h"

DbusEventDispatcher::DbusEventDispatcher()
  : conn_(nullptr)
    , stop_event_fd_(-1)
    , wakeup_event_fd_(-1)
    , dispatch_event_fd(-1)
    , call_count_(0)
{
}


DbusEventDispatcher::~DbusEventDispatcher()
{
  if (dispatcher_.joinable())
  {
    stop_event_dispatcher();
  }

  cleanup_all_events_();
}


void DbusEventDispatcher::start_event_dispatcher(DBusConnection *conn)
{
  // sanity check if dispatch thread is *already* running.
  if (dispatcher_.joinable())
  {
    std::cout << "fatal: dispatch thread already running" << std::endl;
    return;
  }

  stop_event_fd = eventfd(0, EFD_CLOEXEC);

  conn_ = conn;

  dbus_connection_set_dispatch_status_function(conn_,
      DbusEventDispatcher::status_cb_, this, NULL);

  // Sets the mainloop wakeup function for the connection.
  // Called when the main loop's thread should be notified that there's now work
  // to do. 
  //
  // This function is responsible for waking up the main loop (if its sleeping
  // in another thread) when some some change has happened to the connection
  // that the mainloop needs to reconsider (e.g. a message has been queued for
  // writing). 
  //
  // When using Qt, this typically results in a call to QEventLoop::wakeUp().
  // When using GLib, it would call g_main_context_wakeup().
  dbus_connection_set_wakeup_main_function(conn_,
      DbusEventDispatcher::wakeup_cb_, this, NULL);

  dispatcher_ = std::thread(&DbusEventDispatcher::dispatcher_function_);
}


// the MAIN poll thread that processes all dbus events

void DbusEventDispatcher::dispatcher_function_()
{
  // set thread name
  pthread_setname_np(pthread_self(), "DBUS_EVENTS");

  // create the timeout objects to manage the dbus timeouts
}
