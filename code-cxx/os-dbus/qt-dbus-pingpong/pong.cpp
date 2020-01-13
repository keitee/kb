#include <stdio.h>
#include <stdlib.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtDBus/QtDBus>

#include "ping-common.h"
#include "pong.h"

/*
from /home/keitee/Qt/Examples/Qt-5.12.3/dbus/pingpong

#define SERVICE_NAME            "org.example.QtDBus.PingExample"


dbus-send  --session --type=method_call --print-reply --dest='org.freedesktop.DBus' / org.freedesktop.DBus.ListNames | ag "Ping"
      string "org.example.QtDBus.PingExample"


o see that Pong do not set `inerface name` and it's automatically(?) assigned:

  <interface name="local.pong.Pong">

dbus-send --session --type=method_call --print-reply --dest='org.example.QtDBus.PingExample' / org.freedesktop.DBus.Introspectable.Introspect

method return time=1571302308.302756 sender=:1.378 -> destination=:1.381 serial=3 reply_serial=2
   string "<!DOCTYPE node PUBLIC "-//freedesktop//DTD D-BUS Object Introspection 1.0//EN"
"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd">
<node>
  <interface name="local.pong.Pong">
    <method name="ping">
      <arg type="s" direction="out"/>
      <arg name="arg" type="s" direction="in"/>
    </method>
  </interface>
  <interface name="org.freedesktop.DBus.Properties">
    <method name="Get">
      <arg name="interface_name" type="s" direction="in"/>
      <arg name="property_name" type="s" direction="in"/>
      <arg name="value" type="v" direction="out"/>
    </method>
    <method name="Set">
      <arg name="interface_name" type="s" direction="in"/>
      <arg name="property_name" type="s" direction="in"/>
      <arg name="value" type="v" direction="in"/>
    </method>
    <method name="GetAll">
      <arg name="interface_name" type="s" direction="in"/>
      <arg name="values" type="a{sv}" direction="out"/>
      <annotation name="org.qtproject.QtDBus.QtTypeName.Out0" value="QVariantMap"/>
    </method>
    <signal name="PropertiesChanged">
      <arg name="interface_name" type="s" direction="out"/>
      <arg name="changed_properties" type="a{sv}" direction="out"/>
      <annotation name="org.qtproject.QtDBus.QtTypeName.Out1" value="QVariantMap"/>
      <arg name="invalidated_properties" type="as" direction="out"/>
    </signal>
  </interface>
  <interface name="org.freedesktop.DBus.Introspectable">
    <method name="Introspect">
      <arg name="xml_data" type="s" direction="out"/>
    </method>
  </interface>
  <interface name="org.freedesktop.DBus.Peer">
    <method name="Ping"/>
    <method name="GetMachineId">
      <arg name="machine_uuid" type="s" direction="out"/>
    </method>
  </interface>
</node>
"

dbus-send --session --type=method_call --print-reply --dest='org.example.QtDBus.PingExample' / local.pong.Pong.ping string:'hello'
method return time=1571302741.367224 sender=:1.383 -> destination=:1.385 serial=4 reply_serial=2
   string "ping("hello") got called"

*/


// https://doc.qt.io/qt-5/qmetaobject.html
//
// Detailed Description
//
// The Qt Meta-Object System in Qt is responsible for the signals and slots
// inter-object communication mechanism, runtime type information, and the Qt
// property system. 
//
// A single QMetaObject instance is created for each QObject subclass that is
// used in an application, and this instance stores all the meta-information for
// the QObject subclass. This object is available as QObject::metaObject().
//
// bool QMetaObject::invokeMethod(QObject *obj, const char *member,
// Qt::ConnectionType type, QGenericReturnArgument ret, QGenericArgument val0 =
// QGenericArgument(nullptr), QGenericArgument val1 = QGenericArgument(),
// QGenericArgument val2 = QGenericArgument(), QGenericArgument val3 =
// QGenericArgument(), QGenericArgument val4 = QGenericArgument(),
// QGenericArgument val5 = QGenericArgument(), QGenericArgument val6 =
// QGenericArgument(), QGenericArgument val7 = QGenericArgument(),
// QGenericArgument val8 = QGenericArgument(), QGenericArgument val9 =
// QGenericArgument())
//
// Invokes the member (a signal or a slot name) on the object obj. Returns true
// if the member could be invoked. Returns false if there is no such member or
// the parameters did not match.
//
// The invocation can be either synchronous or asynchronous, depending on type:
//
// If type is Qt::DirectConnection, the member will be invoked immediately.
//
// If type is Qt::QueuedConnection, a QEvent will be sent and the member is
// invoked as soon as the application enters the main event loop.
//
// If type is Qt::BlockingQueuedConnection, the method will be invoked in the
// same way as for Qt::QueuedConnection, except that the current thread will
// block until the event is delivered. Using this connection type to communicate
// between objects in the same thread will lead to deadlocks.

// If type is Qt::AutoConnection, the member is invoked synchronously if obj
// lives in the same thread as the caller; otherwise it will invoke the member
// asynchronously.
//
// With asynchronous method invocations, the parameters must be of types that
// are known to Qt's meta-object system, because Qt needs to copy the arguments
// to store them in an event behind the scenes. If you try to use a queued
// connection and get the error message


QString Pong::ping(const QString &arg)
{
  QMetaObject::invokeMethod(QCoreApplication::instance(), "quit");
  return QString("ping(\"%1\") got called").arg(arg);
}


// https://doc.qt.io/qt-5/qtdbus-pingpong-example.html#
//
// Demonstrates a simple message system using D-Bus.
//
// Ping Pong is a command-line example:
//
// o do not use adaptor
//
// o ping make a remote call and print reply and pong quit its running when it
// gets a call from ping. 
//
// $ QDBUS_DEBUG=1 ./pong (lost of debug messages)
//
// ping can specify what to send from command line arg
//
// $ QDBUS_DEBUG=1 ./ping "hello"
// Reply was: ping("hello") got called

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  if (!QDBusConnection::sessionBus().isConnected()) {
    fprintf(stderr, "Cannot connect to the D-Bus session bus.\n"
        "To start it, run:\n"
        "\teval `dbus-launch --auto-syntax`\n");
    return 1;
  }

  /*

  https://doc.qt.io/qt-5/qdbusconnection.html

  As a convenience for the two most common connection types, the sessionBus()
  and systemBus() functions return open connections to the session server
  *daemon* and the system server daemon, respectively. Those connections are
  opened when first used and are closed when the QCoreApplication destructor is
  run.

  bool QDBusConnection::registerService(const QString &serviceName)

  Attempts to register the serviceName on *the D-Bus server* and returns true if
  the registration succeeded. The registration will fail if the name is already
  registered by another application.

  #define SERVICE_NAME            "org.example.QtDBus.PingExample"

  NOTE that unlike `chat` case, this do not allow running the same `pong`, that
  is, not allow multiple registered with the same service name.

  */

  if (!QDBusConnection::sessionBus().registerService(SERVICE_NAME)) {
    fprintf(stderr, "%s\n",
        qPrintable(QDBusConnection::sessionBus().lastError().message()));
    printf("pong cannot register service and exit\n");
    exit(1);
  }

  /*
  enum QDBusConnection::RegisterOption
  flags QDBusConnection::RegisterOptions
  Specifies the options for registering objects with the connection. The
  possible values are:

  QDBusConnection::ExportAllSlots
  ExportScriptableSlots|ExportNonScriptableSlots
  export all of this object's slots

  */

  // see that can expose object without using adapter interface

  Pong pong;
  QDBusConnection::sessionBus().registerObject("/", &pong, 
      QDBusConnection::ExportAllSlots);

  printf("pong is ready to accept a call\n");
  printf("pong is ready to accept a call\n");
  // fflush();

  app.exec();
  return 0;
}
