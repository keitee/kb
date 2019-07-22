/* 
https://linoxide.com/how-tos/d-bus-ipc-mechanism-linux/

Example 2: Connecting two desktop applications

In this example, we will use DBus to connect two desktop applications, one
listen to DBus messages and the other send DBus messages, but before starting,
the listener program should not just start and exit, it has to wait for events,
so we have to find a way to organize the events sent to our program, a simple
solution for this is to use the main event loop from glib, when using it we can
keep our program in sleep mode until receiving events

another problem occurs is the fact that how we can integrate our bus connection
with the glib main event loop, here comes dbus-glib, so our tiny program will
depend also on dbus-glib for just one call, dbus_connection_setup_with_g_main,
this call integrates the glib main loop and the DBus bus events.

A question raises here, if we want to use just DBus, how we can avoid the usage
of its glib binding, the answer is not simple, first we have to write our own
loop events, and integrate it with the bus events, a good start is to look at
the DBus source as they have a helpful code in dbus/dbus-mainloop, but to
simplify our job we will use dbus-glib.

listen.c

In this program we will use 

dbus_bus_add_match(DbusConnection *,const char *rule, DBusError *) 

to add a match for the messages that we want to receive, the rule string has a
specific format, see DBus match rule for full details.

// gcc `pkg-config --libs --cflags dbus-1 glib-2.0 dbus-glib-1` listen.c -o listen
gcc -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -ldbus-glib-1 -ldbus-1 -lgobject-2.0 -lglib-2.0 dbus-02-listen.c -o dbus-02-listen


// works on gcc (Debian 6.3.0-18+deb9u1) 6.3.0 20170516
// gcc `pkg-config --libs --cflags dbus-1` send.c -o send
gcc -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include dbus-02-send.c -ldbus-1 -o dbus-02-send


// when run listen

method call time=1562107336.225789 sender=:1.93 -> destination=org.freedesktop.DBus serial=1 path=/org/freedesktop/DBus; interface=org.freedesktop.DBus; member=Hello
method return time=1562107336.225808 sender=org.freedesktop.DBus -> destination=:1.93 serial=1 reply_serial=1
   string ":1.93"
signal time=1562107336.225819 sender=org.freedesktop.DBus -> destination=(null destination) serial=911 path=/org/freedesktop/DBus; interface=org.freedesktop.DBus; member=NameOwnerChanged
   string ":1.93"
   string ""
   string ":1.93"
signal time=1562107336.225838 sender=org.freedesktop.DBus -> destination=:1.93 serial=2 path=/org/freedesktop/DBus; interface=org.freedesktop.DBus; member=NameAcquired
   string ":1.93"
method call time=1562107336.226229 sender=:1.93 -> destination=org.freedesktop.DBus serial=2 path=/org/freedesktop/DBus; interface=org.freedesktop.DBus; member=AddMatch
   string "type='signal', interface='org.share.linux'"


// when run send

method call time=1562107489.942815 sender=:1.94 -> destination=org.freedesktop.DBus serial=1 path=/org/freedesktop/DBus; interface=org.freedesktop.DBus; member=Hello
method return time=1562107489.942833 sender=org.freedesktop.DBus -> destination=:1.94 serial=1 reply_serial=1
   string ":1.94"
signal time=1562107489.942843 sender=org.freedesktop.DBus -> destination=(null destination) serial=912 path=/org/freedesktop/DBus; interface=org.freedesktop.DBus; member=NameOwnerChanged
   string ":1.94"
   string ""
   string ":1.94"
signal time=1562107489.942855 sender=org.freedesktop.DBus -> destination=:1.94 serial=2 path=/org/freedesktop/DBus; interface=org.freedesktop.DBus; member=NameAcquired
   string ":1.94"

signal time=1562107489.943722 sender=:1.94 -> destination=(null destination) serial=2 path=/org/share/linux; interface=org.share.linux; member=Config

signal time=1562107489.943747 sender=org.freedesktop.DBus -> destination=:1.94 serial=5 path=/org/freedesktop/DBus; interface=org.freedesktop.DBus; member=NameLost
   string ":1.94"
signal time=1562107489.943757 sender=org.freedesktop.DBus -> destination=(null destination) serial=913 path=/org/freedesktop/DBus; interface=org.freedesktop.DBus; member=NameOwnerChanged
   string ":1.94"
   string ":1.94"
   string ""

*/

#include <stdio.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include <glib.h>

static DBusHandlerResult

dbus_filter (DBusConnection *connection, DBusMessage *message, void *user_data)
{
  printf("dbus_filter gets called...\n");

  /*

  DBUS_EXPORT 
  dbus_bool_t dbus_message_is_signal(DBusMessage *message,
    const char * iface,
    const char * signal_name 
  );

  Checks whether the message is a signal with the given interface and member
  fields.
  
  If the message is not DBUS_MESSAGE_TYPE_SIGNAL, or has a different interface
  or member field, returns FALSE.
  
  Parameters message	the message iface	the name to check (must not be NULL)
  signal_name	the name to check (must not be NULL) Returns TRUE if the message
  is the specified signal Definition at line 3865 of file dbus-message.c.
  
  References FALSE, and NULL.
  */
  if ( dbus_message_is_signal(message, "org.share.linux", "Customize")) 
  {
    printf("Message cutomize received\n"); return DBUS_HANDLER_RESULT_HANDLED; 
  }

  if ( dbus_message_is_signal(message,"org.share.linux","Quit" ) )
  {
    printf("Message quit received\n");

    GMainLoop *loop = (GMainLoop*) user_data;

    g_main_loop_quit(loop);

    return DBUS_HANDLER_RESULT_HANDLED;
  }

  return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

int main()
{
  DBusConnection *connection;

  DBusError error;

  /* glib main loop */
  GMainLoop *loop;

  loop = g_main_loop_new(NULL,FALSE);

  dbus_error_init(&error);

  connection = dbus_bus_get(DBUS_BUS_SESSION, &error);

  if ( dbus_error_is_set(&error) )
  {
    printf("Error connecting to the daemon bus: %s",error.message);
    dbus_error_free(&error);
    return 1;
  }

  /*
  DBUS_EXPORT 
  void dbus_bus_add_match(DBusConnection * connection,
    const char * rule,
    DBusError * error 
  );

  Adds a match rule to match messages going through the message bus.
  */
  dbus_bus_add_match (connection,
      "type='signal', interface='org.share.linux'",NULL);

  /* 
  DBUS_EXPORT 
  dbus_bool_t dbus_connection_add_filter(DBusConnection * connection,
    DBusHandleMessageFunction function,
    void * user_data,
    DBusFreeFunction free_data_function 
  );

  Adds a message filter.

  Filters are handlers that are run on all incoming messages, prior to the
  objects registered with dbus_connection_register_object_path(). Filters are
  run in the order that they were added. The same handler can be added as a
  filter more than once, in which case it will be run more than once. Filters
  added during a filter callback won't be run on the message being processed.
  */
  dbus_connection_add_filter (connection, dbus_filter, loop, NULL);

  /* dbus-glib call */
  dbus_connection_setup_with_g_main(connection,NULL);

  /* run glib main loop */
  g_main_loop_run(loop);

  return 0;
}
