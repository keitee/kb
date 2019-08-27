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


/*
https://dbus.freedesktop.org/doc/api/html/group__DBusConnection.html#gae00f581e5487408cb294bf71826aff86

Detailed Description

Connection to another application.

A DBusConnection represents a connection to another application. Messages can be
sent and received via this connection. The other application may be a message
bus; for convenience, the function dbus_bus_get() is provided to automatically
open a connection to the well-known message buses.

In brief a DBusConnection is a message queue associated with some message
transport mechanism such as a socket. The connection maintains a queue of
incoming messages and a queue of outgoing messages.

Several functions use the following terms:

`read` means to fill the incoming message queue by reading from the socket

`write` means to drain the outgoing queue by writing to the socket

`dispatch` means to drain the incoming queue by invoking application-provided
message handlers

The function dbus_connection_read_write_dispatch() for example does all three of
these things, offering a simple alternative to a main loop.

In an application with a main loop, the read/write/dispatch operations are
usually separate.

The connection provides DBusWatch and DBusTimeout objects to the main loop.
These are used to know when reading, writing, or dispatching should be
performed.

Incoming messages are processed by calling dbus_connection_dispatch().
dbus_connection_dispatch() runs any handlers registered for the topmost message
in the message queue, then discards the message, then returns.

dbus_connection_get_dispatch_status() indicates whether messages are currently
in the queue that need dispatching.

dbus_connection_set_dispatch_status_function() allows you to set a function to
be used to monitor the dispatch status.


If you're using GLib or Qt add-on libraries for D-Bus, there are special
convenience APIs in those libraries that hide all the details of dispatch and
watch/timeout monitoring. For example, dbus_connection_setup_with_g_main().

(this is what this class does)

If you aren't using these add-on libraries, but want to process messages
asynchronously, you must manually call

dbus_connection_set_dispatch_status_function(),
dbus_connection_set_watch_functions(), 
dbus_connection_set_timeout_functions()

providing appropriate functions to integrate the connection with your
application's main loop. This can be tricky to get right; main loops are not
simple.

If you don't need to be asynchronous, you can ignore DBusWatch, DBusTimeout, and
dbus_connection_dispatch(). Instead, dbus_connection_read_write_dispatch() can
be used.

Or, in very simple applications, dbus_connection_pop_message() may be all you
need, allowing you to avoid setting up any handler functions (see
dbus_connection_add_filter(), dbus_connection_register_object_path() for more on
handlers).

When you use dbus_connection_send() or one of its variants to send a message,
the message is added to the outgoing queue. It's actually written to the network
later; either in dbus_watch_handle() invoked by your main loop, or in
dbus_connection_flush() which blocks until it can write out the entire outgoing
queue. The GLib/Qt add-on libraries again handle the details here for you by
setting up watch functions.

When a connection is disconnected, you are guaranteed to get a signal
"Disconnected" from the interface DBUS_INTERFACE_LOCAL, path DBUS_PATH_LOCAL.

You may not drop the last reference to a DBusConnection until that connection
has been disconnected.

You may dispatch the unprocessed incoming message queue even if the connection
is disconnected. However, "Disconnected" will always be the last message in the
queue (obviously no messages are received after disconnection).

After calling dbus_threads_init(), DBusConnection has thread locks and drops
them when invoking user callbacks, so in general is transparently threadsafe.
However, DBusMessage does NOT have thread locks; you must not send the same
message to multiple DBusConnection if those connections will be used from
different threads, for example.

Also, if you dispatch or pop messages from multiple threads, it may work in the
sense that it won't crash, but it's tough to imagine sane results; it will be
completely unpredictable which messages go to which threads.

It's recommended to dispatch from a single thread.

The most useful function to call from multiple threads at once is
dbus_connection_send_with_reply_and_block(). That is, multiple threads can make
method calls at the same time.

If you aren't using threads, you can use a main loop and
dbus_pending_call_set_notify() to achieve a similar result.

*/

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
