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

gcc `pkg-config --libs --cflags dbus-1 glib-2.0 dbus-glib-1` listen.c -o listen
    -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -ldbus-glib-1 -ldbus-1 -lgobject-2.0 -lglib-2.0
gcc -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -ldbus-glib-1 -ldbus-1 -lgobject-2.0 -lglib-2.0 dbus-02-listen.c -o dbus-02-listen

// the above do not work and have to use below for gcc (Ubuntu 7.4.0-1ubuntu1~18.04) 7.4.0
gcc `pkg-config --cflags dbus-1 glib-2.0 dbus-glib-1` dbus-02-listen.c `pkg-config --libs dbus-1 glib-2.0 dbus-glib-1` -o dbus-02-listen
$ echo gcc `pkg-config --cflags dbus-1 glib-2.0 dbus-glib-1` dbus-02-listen.c `pkg-config --libs dbus-1 glib-2.0 dbus-glib-1` -o dbus-02-listen
gcc -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include dbus-02-listen.c -ldbus-glib-1 -ldbus-1 -lgobject-2.0 -lglib-2.0 -o dbus-02-listen


// works on gcc (Debian 6.3.0-18+deb9u1) 6.3.0 20170516
// gcc `pkg-config --libs --cflags dbus-1` send.c -o send
gcc -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include dbus-02-send.c -ldbus-1 -o dbus-02-send


// log from send and listen. As you see, filter gets called whenever message is
// sent.

$ ./dbus-02-send -c

enter: dbus_filter gets called...
leave: dbus_filter gets called...

$ ./dbus-02-send -u

enter: dbus_filter gets called...
Message cutomize received

$ ./dbus-02-send -q

enter: dbus_filter gets called...
Message quit received


// monitor log when run listen

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

// to aviod this warning:
// dbus-02-listen.c: In function ‘main’:
// dbus-02-listen.c:255:3: warning: implicit declaration of function ‘dbus_connection_setup_with_g_main’; did you mean ‘dbus_connection_send_with_reply’? [-Wimplicit-function-declaration]
//    dbus_connection_setup_with_g_main(connection,NULL);
//    ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//    dbus_connection_send_with_reply
//
// https://dbus.freedesktop.org/doc/dbus-glib/index.html
#include <dbus/dbus-glib-lowlevel.h>

static DBusHandlerResult

dbus_filter (DBusConnection *connection, DBusMessage *message, void *user_data)
{
  printf("enter: dbus_filter gets called...\n");

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
    printf("Message cutomize received\n"); 
    return DBUS_HANDLER_RESULT_HANDLED; 
  }

  if ( dbus_message_is_signal(message,"org.share.linux", "Quit" ) )
  {
    printf("Message quit received\n");

    GMainLoop *loop = (GMainLoop*) user_data;

    g_main_loop_quit(loop);

    return DBUS_HANDLER_RESULT_HANDLED;
  }

  printf("leave: dbus_filter gets called...\n");
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
  https://dbus.freedesktop.org/doc/api/html/group__DBusBus.html#ga4eb6401ba014da3dbe3dc4e2a8e5b3ef

  DBUS_EXPORT 
  void dbus_bus_add_match(DBusConnection * connection,
    const char * rule,
    DBusError * error 
  );

  Adds a match rule to match messages going through the message bus.

  The "rule" argument is the string form of a match rule.
  
  If you pass NULL for the error, this function will not block; the match thus
  won't be added until you flush the connection, and if there's an error adding
  the match you won't find out about it. This is generally acceptable, since the
  possible errors (including a lack of resources in the bus, the connection
  having exceeded its quota of active match rules, or the match rule being
  unparseable) are generally unrecoverable.
  
  If you pass non-NULL for the error this function will block until it gets a
  reply. This may be useful when using match rule keys introduced in recent
  versions of D-Bus, like 'arg0namespace', to allow the application to fall back
  to less efficient match rules supported by older versions of the daemon if the
  running version is not new enough; or when using user-supplied rules rather
  than rules hard-coded at compile time.
  
  Normal API conventions would have the function return a boolean value
  indicating whether the error was set, but that would require blocking always
  to determine the return value.
  
  The AddMatch method is fully documented in the D-Bus specification. For quick
  reference, the format of the match rules is discussed here, but the
  specification is the canonical version of this information.
  
  Rules are specified as a string of comma separated key/value pairs. An example
  is 

  "type='signal',sender='org.freedesktop.DBus', interface='org.freedesktop.DBus',member='Foo', path='/bar/foo',destination=':452345.34'"

  Possible keys you can match on are type, sender, interface, member, path,
  destination and numbered keys to match message args (keys are 'arg0', 'arg1',
  etc.). Omitting a key from the rule indicates a wildcard match. For instance
  omitting the member from a match rule but adding a sender would let all
  messages from that sender through regardless of the member.
  
  Matches are inclusive not exclusive so as long as one rule matches the message
  will get through. It is important to note this because every time a message is
  received the application will be paged into memory to process it. This can
  cause performance problems such as draining batteries on embedded platforms.
  
  If you match message args ('arg0', 'arg1', and so forth) only string arguments
  will match. That is, arg0='5' means match the string "5" not the integer 5.
  
  Currently there is no way to match against non-string arguments.
  
  A specialised form of wildcard matching on arguments is supported for
  path-like namespaces. If your argument match has a 'path' suffix (eg:
  "arg0path='/some/path/'") then it is considered a match if the argument
  exactly matches the given string or if one of them ends in a '/' and is a
  prefix of the other.
  
  Matching on interface is tricky because method call messages only optionally
  specify the interface. If a message omits the interface, then it will NOT
  match if the rule specifies an interface name. This means match rules on
  method calls should not usually give an interface.
  
  However, signal messages are required to include the interface so when
  matching signals usually you should specify the interface in the match rule.
  
  For security reasons, you can match arguments only up to
  DBUS_MAXIMUM_MATCH_RULE_ARG_NUMBER.
  
  Match rules have a maximum length of DBUS_MAXIMUM_MATCH_RULE_LENGTH bytes.
  
  Both of these maximums are much higher than you're likely to need, they only
  exist because the D-Bus bus daemon has fixed limits on all resource usage.

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
