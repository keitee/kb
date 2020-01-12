/*
https://linoxide.com/how-tos/d-bus-ipc-mechanism-linux/

Example 2: Connecting two desktop applications

send.c

*/

#include <dbus/dbus.h>
#include <stdio.h>
#include <string.h>

/*
DBUS_EXPORT
DBusMessage * dbus_message_new_signal(const char * path,
const char * iface,
const char * name
);

Constructs a new message representing a signal emission.

Returns NULL if memory can't be allocated for the message. A signal is
identified by its originating object path, interface, and the name of the
signal.

Path, interface, and signal name must all be valid (the D-Bus specification
defines the syntax of these fields).

Parameters
path    the path to the object emitting the signal
iface   the interface the signal is emitted from
name    name of the signal

Returns
a new DBusMessage, free with dbus_message_unref()
*/

static void send_config(DBusConnection *connection)
{
  DBusMessage *message;

  message =
    dbus_message_new_signal("/org/share/linux", "org.share.linux", "Config");

  /* Send the signal */
  dbus_connection_send(connection, message, NULL);
  dbus_message_unref(message);
}

static void send_customise(DBusConnection *connection)
{
  DBusMessage *message;

  message =
    dbus_message_new_signal("/org/share/linux", "org.share.linux", "Customize");

  /* Send the signal */
  dbus_connection_send(connection, message, NULL);
  dbus_message_unref(message);
}

static void send_quit(DBusConnection *connection)
{
  DBusMessage *message;

  message =
    dbus_message_new_signal("/org/share/linux", "org.share.linux", "Quit");

  /* Send the signal */
  dbus_connection_send(connection, message, NULL);
  dbus_message_unref(message);
}

int main(int argc, char **argv)
{
  DBusConnection *connection;

  DBusError error;

  dbus_error_init(&error);

  connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
  if (!connection)
  {
    printf("Failed to connect to the D-BUS daemon: %s", error.message);
    dbus_error_free(&error);
    return 1;
  }

  if (argc == 1)
  {
    return 0;
  }

  int i;

  for (i = 1; i < argc; i++)
  {
    if (!strcmp(argv[i], "-c"))
    {
      send_config(connection);
    }
    if (!strcmp(argv[i], "-u"))
    {
      send_customise(connection);
    }
    else if (!strcmp(argv[i], "-q"))
    {
      send_quit(connection);
    }
  }

  return 0;
}
