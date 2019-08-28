//
// dbusconnection.cpp
//

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
*message handlers*

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

  dbus_connection_dispatch()
  
  DBUS_EXPORT DBusDispatchStatus 
  dbus_connection_dispatch(DBusConnection *connection)
  
  Processes any incoming data.
  
  If there's incoming raw data that has not yet been parsed, it is parsed, which
  may or may not result in adding messages to the incoming queue.
  
  The incoming data buffer is filled when the connection reads from its
  underlying transport (such as a socket). Reading usually happens in
  dbus_watch_handle() or dbus_connection_read_write().
  
  If there are complete messages in the incoming queue,
  dbus_connection_dispatch() removes one message from the queue and processes
  it. 

  Processing has three steps.
  
  First, any method replies are passed to DBusPendingCall or
  dbus_connection_send_with_reply_and_block() in order to complete the pending
  method call.
  
  Second, any filters registered with dbus_connection_add_filter() are run. If
  any filter returns DBUS_HANDLER_RESULT_HANDLED then processing stops after
  that filter.
  
  Third, if the message is a method call it is forwarded to any registered
  object path handlers added with dbus_connection_register_object_path() or
  dbus_connection_register_fallback().
  
  A single call to dbus_connection_dispatch() will process at most one message;
  it will not clear the entire message queue.
  
  Be careful about calling dbus_connection_dispatch() from inside a message
  handler, i.e. calling dbus_connection_dispatch() recursively. If threads have
  been initialized with a recursive mutex function, then this will not deadlock;
  however, it can certainly confuse your application.
  
  Returns dispatch status, see dbus_connection_get_dispatch_status()


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

#include "dbusconnection.h"

DbusConnection::DbusConnection()
  : conn_(nullptr)
    , handler_(nullptr)
    , token_counter_(1)
{
  // https://dbus.freedesktop.org/doc/api/html/group__DBusThreads.html#gac7b8a7001befc3eaa8c6b043151008dc
  //
  // Initializes threads.
  // 
  // If this function is not called, the D-Bus library will not lock any data
  // structures. If it is called, D-Bus will do locking, at some cost in
  // efficiency.
  // 
  // Since D-Bus 1.7 it is safe to call this function from any thread, any
  // number of times (but it must be called before any other libdbus API is
  // used).

  if (!dbus_thread_init_default())
    std::cout << "fatal: dbus_thread_init_default failed" << std::endl;
}

DbusConnection::~DbusConnection()
{
  if (conn_)
  {
    std::cout << "error: destructed while connected and force disconnect" << std::endl;

    disconnect();
  }
}


// registers a handler to be called when a message (method or signal) is
// received.

void DbusConnection::register_message_handler(
    std::function<DBusHandlerResult(DBusMessage *)> handler)
{
  std::unique_lock<COMMON::SpinLock> lock(self->handler_lock_);
  self->handler_ = handler;
  lock.unlock();

  flush_connection_();
}

bool DbusConnection::flush_connection_()
{
  std::function<void()> worker = [&]()
  {
    // Blocks until the outgoing message queue is empty.
    dbus_connection_flush(conn_);
  }
}


// callback from the libdbus in the context of the event/dispatcher thread
// this callback is installed right after we've connected, we hook this point

DBusHandlerResult DbusConnection::handle_dbus_message_cb_(DBusConnection *conn,
    DBusMessage *mesg, void *userData)
{
  // see that DbusConnection, not DBusConnection
  DbusConnection *self = std::reinterpret_cast<DbusConnection*>(userData);
  if (!self || (self->conn_ != conn))
  {
    std::cout << "fatal: invalid filter callback data" << std::endl;
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  }

  std::unique_lock<COMMON::SpinLock> lock(self->handler_lock_);
  const MessageHandler handler = self->handler_;
  lock.unlock();

  // see that handler_ is set via registerMessageHandler()
  if (nullptr == handler)
  {
    std::cout << "fatal: no handler installed for dbus message" << std::endl;
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  }

  return handler(mesg);
}


// attempts to reserve a service name on dbus
// service names must be unique (per dbus) so if the name is already owned by
// the other then this function will fail.

bool DbusConnection::reserve_service_name_(DBusConnection *conn, 
    std::string const &name) const
{
  DBusError error;
  dbus_error_init(&error);

  dbus_bool_t ret = dbus_bus_name_has_owner(conn, name.c_str(), &error);
  if (dbus_error_is_set(&error))
  {
    std::cout << "error: check if there is an owner for " <<
      name << " - " << error.message << std::endl;
    dbus_error_free(&error);
    return false;
  }

  if (ret != FALSE)
  {
    std::cout << "error: bus name " << name << " is already revserved" 
      << std::endl;
    return false;
  }
  else
  {
    // bus name doesn't have an owner and reserve it

    int result = dbus_bus_request_name(conn, name.c_str(),
        DBUS_NAME_FLAG_DO_NOT_QUEUE, &error);

    if (dbus_error_is_set(&error))
    {
      std::cout << "error: request bus name, " <<
        name << " - " << error.message << std::endl;
      dbus_error_free(&error);
      return false;
    }
    else if (result != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
    {
      std::cout << "info: primary ownership is not granted for bus name , " <<
        name << std::endl;
    }
    else
    {
      std::cout << "info: bus name , " << name << " is granted" << std::endl;
    }
  }

  return true;
}


bool DbusConnection::complete_connect_(DBusConnection *conn, std::string const &serviceName)
{
  DBusError error;
  dbus_error_init(&error);

  // Set whether _exit() should be called when the connection receives a
  // disconnect signal.  By default, exit_on_disconnect is FALSE; but for
  // message bus connections returned from dbus_bus_get() it will be toggled on
  // by default.

  // (comment) we never want to call exit() on disconnect, this should be the
  // default, but just in case, force it to false here
  dbus_connection_set_exit_on_disconnect(conn, FALSE);

  // https://dbus.freedesktop.org/doc/api/html/group__DBusBus.html#ga0c21cf74d05c0bd8003846b56a588a4b
  // Registers a connection with the bus.
  dbus_bool_t ret = dbus_bus_register(conn, &error);
  if ((ret == FALSE) || dbus_error_is_set(&error))
  {
    std::cout << "error: dbus register failed - %s",
      error.message ? error.message : "unknown error" << std::endl;
    dbus_error_free(&error);

    // since dbus_bus_get_private() creates a new connection
    dbus_connection_close(conn);
    dbus_connection_unref(conn);

    return false;
  }

  // if the caller supplies a name then try and claim it
  if (!serviceName.empty())
  {
    if (reserve_service_name_(conn, serviceName))
    {
      // save the reserved name
      service_name_ = serviceName;
    }
    else
    {
      dbus_connection_close(conn);
      dbus_connection_unref(conn);
      return false;
    }
  }

  // save the conn before installing the filter and starting the dispatcher
  conn_ = conn;

  // DBUS_EXPORT dbus_bool_t dbus_connection_add_filter(
  //  DBusConnection * connection,
  //  DBusHandleMessageFunction function,
  //  void * user_data,
  //  DBusFreeFunction free_data_function 
  // );
  //
  // free_data_function   function to use for freeing user data
  //
  // Adds a message filter.
  // 
  // Filters are handlers that are run on all incoming messages, prior to the
  // objects registered with dbus_connection_register_object_path(). Filters are
  // run in the order that they were added. The same handler can be added as a
  // filter more than once, in which case it will be run more than once. Filters
  // added during a filter callback won't be run on the message being processed.

  if (TRUE != dbus_connection_add_filter(conn_, handle_dbus_message_cb_, this, NULL))
  {
    std::cout << "error: fail to install dbus message filter" 
      << std::endl;
  }
}


// attempts to connect to one of the known buses and optionally reserve the
// given service name.
//
// [in] type
// [in] serviceName   Optional service name to use for the connection.

bool DBusConnection::connect(DBusBusType type, std::string const &serviceName /* = std::string() */)
{
  DBusError error;
  dbus_error_init(&error);

  // https://dbus.freedesktop.org/doc/api/html/group__DBusBus.html#ga9c62186f19cf3bd3c7c604bdcefb4e09
  //
  // Connects to a bus daemon and registers the client with it as with
  // dbus_bus_register().
  // 
  // Unlike dbus_bus_get(), *always creates a new connection.* This connection
  // will not be saved or recycled by libdbus. Caller owns a reference to the
  // bus and must either close it or know it to be closed prior to releasing
  // this reference.
  // 
  // See dbus_connection_open_private() for more details on when to close and
  // unref this connection.
  // 
  // dbus_bus_get_private() calls dbus_bus_register() for you.
  // 
  // This function will block until authentication and bus registration are
  // complete.
  // 
  // Definition at line 590 of file dbus-bus.c.

  DBusConnection *conn = dbus_bus_get_private(type, &error);
  if ((conn == nullptr) || dbus_error_is_set(&error))
  {
    // https://dbus.freedesktop.org/doc/api/html/group__DBusErrors.html

    std::cout << "error: error connecting to the bus - %s",
      error.message ? error.message : "unknown error" << std::endl;
    dbus_error_free(&error);
    return false;
  }

  return complete_connect_(conn, serviceName);
}
