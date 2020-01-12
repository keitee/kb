/*
https://linoxide.com/how-tos/d-bus-ipc-mechanism-linux/

Example 3: DBus services

The message bus can start applications (services) on behalf of other
applications, the application asks DBus to start a service by its name, usually
the name should be known such as org.freedesktop.TextEditor.

In order for DBus to find the executable corresponding to a particular name, the
bus daemon looks for service description files which usually are installed in
/usr/share/dbus-1/services and they have .service in their extension name (all
linux distros that i know they use this prefix to install dbus services files),
as an example of a service file.

DBus service file example:

[D-BUS Service]
Name=org.share.linux
Exec=path to the executable.

keitee@kit-hdebi:/usr/share/dbus-1/services$ more org.bluez.obex.service
[D-BUS Service]
Name=org.bluez.obex
Exec=/usr/lib/bluetooth/obexd
SystemdService=obex.service

We will write two programs, one is the service that we want to start the other
is the application that activates this service

share-linux-serivce-example.c

$ gcc `pkg-config --libs --cflags dbus-1` dbus-03.c -o dbus-03-service
$ gcc `pkg-config --libs --cflags dbus-1` dbus-03-start.c -o dbus-03-start

*/

#include <dbus/dbus.h>
#include <stdio.h>

int main()
{
  DBusConnection *connection;

  DBusError error;

  dbus_error_init(&error);

  /* DBUS_BUS_STARTER is the bus that started us */
  connection = dbus_bus_get(DBUS_BUS_STARTER, &error);

  // Do something here to make sure that the application was successfully
  // started by DBus
  // Example could be something like

  printf("share-linux-serivce-example.c is launched...\n");
  FILE *tmp;
  tmp = fopen("/tmp/share-linux-service.result", "w");
  fprintf(tmp, "share-linux service was started successfully");
  fclose(tmp);

  // After that you have the service up, so you can do whetever you like

  dbus_connection_unref(connection);

  return 0;
}
