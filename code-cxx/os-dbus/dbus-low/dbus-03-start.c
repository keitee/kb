/*
https://linoxide.com/how-tos/d-bus-ipc-mechanism-linux/

Example 3: DBus services

Compile this example with dbus-1 argument for pkg-config, you need to install
the service file in /usr/share/dbus-1/service, name it org.share.linux and edit
the

Exec path to where you have the service example binary.

start-service.c

*/

#include <stdio.h>

#include <dbus/dbus.h>

int main()
{
  DBusConnection *connection;
  DBusError error;
  DBusMessage *message;

  const char *service_name = "org.share.linux";

  /* Currently this is not used by DBus, they say it is for futur expansion*/
  dbus_uint32_t flag;
  dbus_bool_t result;

  dbus_error_init(&error);

  connection = dbus_bus_get(DBUS_BUS_SESSION, &error);

  if (dbus_error_is_set(&error))
  {
    printf("Error getting dbus connection: %s\n", error.message);
    dbus_error_free(&error);
    dbus_connection_unref(connection);
    return 0;
  }

  message = dbus_message_new_method_call("org.freedesktop.DBus",
                                         "/org/freedesktop/DBus",
                                         "org.freedesktop.DBus",
                                         "StartServiceByName");

  if (!message)
  {
    printf("Error creating DBus message\n");
    dbus_connection_unref(connection);
    return 0;
  }

  /* We don't want to receive a reply */
  dbus_message_set_no_reply(message, TRUE);

  /* Append the argument to the message, must ends with DBUS_TYPE_UINT32 */
  dbus_message_append_args(message,
                           DBUS_TYPE_STRING,
                           &service_name,
                           DBUS_TYPE_UINT32,
                           &flag,
                           DBUS_TYPE_INVALID);

  result = dbus_connection_send(connection, message, NULL);

  if (result == TRUE)
  {
    printf("Successfully activating the %s service\n", service_name);
  }
  else
  {
    printf("Failed to activate the %s service\n", service_name);
  }

  dbus_message_unref(message);

  dbus_connection_unref(connection);

  return 0;
}
