/*

NOTE: DO NOT USE PROXY/ADAPTOR

D-Bus Complex Ping Pong Example

Demonstrates usage of the Qt D-Bus typesystem.

Complex Ping Pong Example demonstrates the use of Qt D-Bus typesystem with
QDBusVariant and QDBusReply. The example consists of the main application
complexping which starts the other application, complexpong. Entering
keywords such as hello and ping is handled by complexpong and the reply is
printed to the standard output.

Running the Example

To run, execute the complexping application which will lauch complexpong server.
Entering empty line will call `quit` on remote server.

$ ./complexping
Ask your question: When is the next Qt release?
Reply was: Sorry, I don't know the answer
Ask your question: What is the answer to life, the universe and everything?
Reply was: 42

same service name as "ping/pong" example

#define SERVICE_NAME "org.example.QtDBus.PingExample"

dbus-send --session --type=method_call --print-reply --dest='org.example.QtDBus.PingExample' / org.freedesktop.DBus.Introspectable.Introspect

method return time=1579041212.526071 sender=:1.447 -> destination=:1.449 serial=4 reply_serial=2
   string "<!DOCTYPE node PUBLIC "-//freedesktop//DTD D-BUS Object Introspection 1.0//EN"
"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd">
<node>
  <interface name="org.example.QtDBus.ComplexPong.Pong">
    <property name="value" type="s" access="readwrite"/>
    <signal name="aboutToQuit">
    </signal>
    <method name="query">
      <arg type="v" direction="out"/>
      <arg name="query" type="s" direction="in"/>
    </method>
    <method name="quit">
      <annotation name="org.freedesktop.DBus.Method.NoReply" value="true"/>
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

As you can see, signal/slot/property maps to dbus signal/method/property

class Pong : public QDBusAbstractAdaptor
{
  NOTE: qtmoc still scans "Q_OBJECT" even if it's in the comment block and which
  causes compile error so removed from here this line.

  Q_CLASSINFO("D-Bus Interface", "org.example.QtDBus.ComplexPong.Pong")
  Q_PROPERTY(QString value READ value WRITE setValue)

  ...

signals:
  void aboutToQuit();

public slots:
  QDBusVariant query(const QString &query);
  Q_NOREPLY void quit();
};

  <interface name="org.example.QtDBus.ComplexPong.Pong">
    <property name="value" type="s" access="readwrite"/>
    <signal name="aboutToQuit">
    </signal>
    <method name="query">
      <arg type="v" direction="out"/>
      <arg name="query" type="s" direction="in"/>
    </method>
    <method name="quit">
      <annotation name="org.freedesktop.DBus.Method.NoReply" value="true"/>
    </method>
  </interface>


call "ping":

dbus-send --session --type=method_call --print-reply --dest='org.example.QtDBus.PingExample' / org.example.QtDBus.ComplexPong.Pong.quit

call "query":

dbus-send --session --type=method_call --print-reply --dest='org.example.QtDBus.PingExample' / org.example.QtDBus.ComplexPong.Pong.query string:"hello"
method return time=1590681924.560269 sender=:1.184 -> destination=:1.185 serial=3 reply_serial=2
   variant       string "World"


NOTE
call "aboutToQuit" signal? do not work and why?

dbus-send --session --type=signal --dest='org.example.QtDBus.PingExample' / org.example.QtDBus.ComplexPong.Pong.aboutToQuit

so "ping" expose signal and "pong" connect to it. Later, ping emits that signal.
Under dbus, guess that this translates to that:
  "ping" register its interest to signal.
  "pong" send a signal which is a broadcast.

Unlike call which "pong" receives and "pong" send a signal. 


call property get:

interface=org.freedesktop.DBus.Properties; member=Get
string "org.example.QtDBus.ComplexPong.Pong"
string "value"

dbus-send --session --type=method_call --print-reply --dest='org.example.QtDBus.PingExample' / org.freedesktop.DBus.Properties.Get \
string:"org.example.QtDBus.ComplexPong.Pong" \
string:"value"

method return time=1590682295.430013 sender=:1.184 -> destination=:1.187 serial=5 reply_serial=2
   variant       string "initial value"


call property set:

interface=org.freedesktop.DBus.Properties; member=Set
string "org.example.QtDBus.ComplexPong.Pong"
string "value"
variant       string "this is message from ping"

dbus-send --session --type=method_call --print-reply --dest='org.example.QtDBus.PingExample' / org.freedesktop.DBus.Properties.Set \
string:"org.example.QtDBus.ComplexPong.Pong" \
string:"value" \
variant:string:"this is message from ping"

check if "value" property is changed

$ dbus-send --session --type=method_call --print-reply --dest='org.example.QtDBus.PingExample' / org.freedesktop.DBus.Properties.Get string:"org.example.QtDBus.ComplexPong.Pong" string:"value"
method return time=1590682798.629954 sender=:1.184 -> destination=:1.191 serial=8 reply_serial=2
   variant       string "this is message from ping"

*/

#include <stdio.h>
#include <stdlib.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtDBus/QtDBus>

#include "complexpong.h"
#include "ping-common.h"

// property functions

QString Pong::value() const
{
  qDebug() << "pong::value returns" << m_value;
  return m_value;
}

void Pong::setValue(QString const &newValue)
{
  qDebug() << "pong::set value " << newValue;
  m_value = newValue;
}

/*
the original code was:

int main(int argc, char **argv)
{
  QObject::connect(&app, &QCoreApplication::aboutToQuit,
    pong, &Pong::aboutToQuit);
}

void Pong::quit()
{
  QTimer::singleShot(0, QCoreApplication::instance(),
    &QCoreApplication::quit);
}

this is signal-to-signal connection. when ping calls remote pong's quit(),
it will cause appliction exit which trigger QCoreApplication::aboutToQuit.
this is mapped and it leads to raises Pong::aboutToQuit signal.

`ping` connect to this Pong::aboutToQuit and runs its quit() so ping also exit.

*/

void Pong::quit()
{
  // As a special case, a QTimer with a timeout of 0 will time out as soon as
  // all the events in the window system's event queue have been processed. This
  // can be used to do heavy work while providing a snappy user interface:

  // QTimer::singleShot(0, QCoreApplication::instance(),
  //     &QCoreApplication::quit);

  QTimer::singleShot(0, this, &Pong::really_quit);
}

void Pong::really_quit()
{
  qDebug() << "really_quit is called";

  // send a signal
  emit aboutToQuit();

  QTimer::singleShot(0, QCoreApplication::instance(), &QCoreApplication::quit);
}

QDBusVariant Pong::query(const QString &query)
{
  qDebug() << "Pong::query called";

  QString q = query.toLower();
  if (q == "hello")
    return QDBusVariant("World");
  if (q == "ping")
    return QDBusVariant("Pong");
  if (q.indexOf("the answer to life, the universe and everything") != -1)
    return QDBusVariant(42);
  if (q.indexOf("unladen swallow") != -1)
  {
    if (q.indexOf("european") != -1)
      return QDBusVariant(11.0);
    return QDBusVariant(QByteArray("african or european?"));
  }

  return QDBusVariant("Sorry, I don't know the answer");
}

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  QObject obj;
  Pong *pong = new Pong(&obj);

  // set property "value"
  pong->setProperty("value", "initial value");

  /*
  enum QDBusConnection::RegisterOption
  flags QDBusConnection::RegisterOptions
  Specifies the options for registering objects with the connection. The
  possible values are:

  QDBusConnection::ExportAllSlots
  ExportScriptableSlots|ExportNonScriptableSlots
  export all of this object's slots

  */

  QDBusConnection::sessionBus().registerObject("/", &obj);

  // NOTE that unlike `chat` case, use registerService() and do not allow running
  // the same `pong`, that is, not allow multiple registered with the same service
  // name.

  // #define SERVICE_NAME            "org.example.QtDBus.PingExample"

  if (!QDBusConnection::sessionBus().registerService(SERVICE_NAME))
  {
    fprintf(stderr,
            "%s\n",
            qPrintable(QDBusConnection::sessionBus().lastError().message()));
    printf("pong cannot register service and exit\n");
    exit(1);
  }

  printf("complexpong is ready to accept a call\n");

  app.exec();
  return 0;
}
