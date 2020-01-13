#include <stdio.h>

#include <QtCore/QCoreApplication>
#include <QtDBus/QtDBus>

#include "ping-common.h"

/*
https://doc.qt.io/qt-5/qdbusinterface.html

Detailed Description

QDBusInterface is a generic accessor class that is used *to place calls* to
remote objects, connect to signals exported by remote objects and get/set the
value of remote properties. This class is useful for dynamic access to remote
objects: that is, when you do not have a generated code that represents the
remote interface.

Calls are usually placed by using the call() function, which constructs the
message, sends it over the bus, waits for the reply and decodes the reply.

Signals are connected to by using the normal QObject::connect() function.

Finally, properties are accessed using the QObject::property() and
QObject::setProperty() functions.


https://doc.qt.io/qt-5/qdbusabstractinterface.html#call

QDBusMessage QDBusAbstractInterface::call(
  const QString &method, const QVariant &arg1 = QVariant(), const QVariant &arg2
  = QVariant(), const QVariant &arg3 = QVariant(), const QVariant &arg4 =
  QVariant(), const QVariant &arg5 = QVariant(), const QVariant &arg6 =
  QVariant(), const QVariant &arg7 = QVariant(), const QVariant &arg8 =
  QVariant());

Calls the method method on this interface and passes the parameters to this
function to the method.

The parameters to call are passed on to the remote function via D-Bus as input
arguments. Output arguments are returned in the QDBusMessage reply. If the reply
is an error reply, lastError() will also be set to the contents of the error
message.


https://doc.qt.io/qt-5/qdbusreply.html

Detailed Description

A QDBusReply object is a *subset* of the QDBusMessage object that represents a
method call's reply. It contains only the first output argument or the error
code and is used by QDBusInterface-derived classes to allow returning the error
code as the function's return argument.
 
However, if it does fail under those conditions, the value returned by
QDBusReply<T>::value() is a default-constructed value. It may be
indistinguishable from a valid return value.


NOTE: 
input argument = in parameter
output argument = out parameter = return value


QDBusReply objects are used for remote calls that have no output arguments or
return values (i.e., they have a "void" return type). 

Use the isValid() function to test if the reply succeeded.


QDBusReply::Type QDBusReply::value() const

Returns the remote function's calls return value. If the remote call returned
with an error, the return value of this function is undefined and may be
undistinguishable from a valid return value.

This function is not available if the remote call returns void.

*/

// PING

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  if (!QDBusConnection::sessionBus().isConnected()) {
    fprintf(stderr, "Cannot connect to the D-Bus session bus.\n"
        "To start it, run:\n"
        "\teval `dbus-launch --auto-syntax`\n");
    return 1;
  }


  // Make a *direct* call(slot) of the remote object, instead of using
  // signal/slot/connect way.

  QDBusInterface iface(SERVICE_NAME, "/", "", QDBusConnection::sessionBus());
  if (iface.isValid()) {
    QDBusReply<QString> reply = iface.call("ping", argc > 1 ? argv[1] : "");
    if (reply.isValid()) {
      printf("Reply was: %s\n", qPrintable(reply.value()));
      return 0;
    }

    fprintf(stderr, "Call failed: %s\n", qPrintable(reply.error().message()));
    return 1;
  }

  fprintf(stderr, "%s\n",
      qPrintable(QDBusConnection::sessionBus().lastError().message()));
  return 1;
}
