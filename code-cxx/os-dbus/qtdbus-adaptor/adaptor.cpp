/*
SENDER

#define SERVICE_NAME "org.example.SenderExample"

dbus-send --session --type=method_call --print-reply --dest='org.example.SenderExample' / org.freedesktop.DBus.Introspectable.Introspect

method return time=1571310650.913287 sender=:1.414 -> destination=:1.418 serial=3 reply_serial=2
   string "<!DOCTYPE node PUBLIC "-//freedesktop//DTD D-BUS Object Introspection 1.0//EN"
"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd">
<node>
  <interface name="org.example.sender">
    <method name="ConnectProfile">
      <arg direction="in" type="s" name="UUID"/>
    </method>
    <signal name="action">
      <arg direction="out" type="s" name="nickname"/>
      <arg direction="out" type="s" name="text"/>
    </signal>
    <property access="readwrite" type="b" name="Powered"/>
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

SENDER METHOD and SIGNAL

<interface name="org.bluez.Device1">
        <method name="ConnectProfile">
                <arg name="UUID" type="s" direction="in"/>
        </method>
</interface>

translated to in the proxy:

inline QDBusPendingReply<> ConnectProfile(const QString &UUID)
{
  QList<QVariant> argumentList;
  argumentList << QVariant::fromValue(UUID);
  return asyncCallWithArgumentList(QStringLiteral("ConnectProfile"), argumentList);
}


CLIENT 1
========

use remote signal connection and remore property

$ ./sender
// write property and get a signal
SenderAdaptor::setPowered( false ) called     // previous value
got powerChanged signal ( true )              // new value

org.example.sender is registered
org.example.sender registered and started

<ex>
// call sendCommand("command") from the other console
dbus-send --session --type=method_call --print-reply --dest='org.example.SenderExample' / org.example.sender.SendCommand string:'command'
method return time=1578995647.935382 sender=:1.792 -> destination=:1.794 serial=5 reply_serial=2

// sender got called
"sender::SendCommand(command) got called"
message.service:  ":1.123"
message.path   :  "/"
message.member :  "SendCommand"

SenderAdaptor::setPowered( true ) called
got powerChanged signal ( true )

// see timer get executed later and send signal to client1
timer expired and emit action signal
timer expired and change property

SenderAdaptor::setPowered( true ) called
got powerChanged signal ( false )

// client1's slot got called and read remote property which runs READ function.
SenderAdaptor::powered( false ) called


$ ./client1
org.example.client started
"Client::onSignalReceived: name(from sender), text(do you hear me?)"
"Client::onSignalReceived: powerd propertry(0)"


<ex>
// trigger ProperChanged but nothing happens to the client1
dbus-send --session --type=method_call --print-reply --dest='org.example.SenderExample' / org.example.sender.SendCommand string:'changed' 

"sender::SendCommand(\"changed\") got called"
notifyPropertyChanged called
SenderAdaptor::powered( false ) called

// can see the signal sent from `sender` to `client1`
method call time=1580002835.990687 sender=:1.126 -> destination=org.example.SenderExample serial=2 path=/; interface=org.example.sender; member=SendCommand
   string "changed"
signal time=1580002835.991221 sender=:1.121 -> destination=(null destination) serial=11 path=/; interface=org.freedesktop.DBus.Properties; member=PropertiesChanged
   string "org.example.sender"
   array [
      dict entry(
         string "Powered"
         variant             boolean false
      )
   ]
   array [
   ]
method return time=1580002835.991246 sender=:1.121 -> destination=:1.126 serial=12 reply_serial=2

<ex>
// to quit
dbus-send --session --type=method_call --print-reply --dest='org.example.SenderExample' / org.example.sender.ConnectProfile string:'hello'


CLIENT 2
========

1. Unlike client1, uses proxy instead:

class OrgExampleChatInterface: public QDBusAbstractInterface

but add changed to the generated proxy to get notified:

  Q_PROPERTY(
    bool Powered READ powered WRITE setPowered NOTIFY senderPowerChanged)

However, notification don't happen.

$ ./sender
org.example.sender is registered
org.example.sender registered and started


// to see if client 2 can access remote properties and client can do that
// call sendCommand("command") from the other console
dbus-send --session --type=method_call --print-reply --dest='org.example.SenderExample' / org.example.sender.SendCommand string:'command'


// client2's slot got called and shows various ways to access remote property.
$ ./client2
org.example.client.use.proxy started
ClientUseProxy::onSignalReceived: name:  "from sender" , text:  "do you hear me?"  called
ClientUseProxy::onSignalReceived: get property:  false
ClientUseProxy::onSignalReceived: get property:  QVariant(bool, false)
ClientUseProxy::onSignalReceived: set property(false)
ClientUseProxy::onSignalReceived: get property:  false
ClientUseProxy::onSignalReceived: set property(true)
ClientUseProxy::onSignalReceived: get property:  true


// trigger PropertyChanged but nothing happens to the client2 even if client2
// defines `NOTIFY` as:
//
// bool Powered READ powered WRITE setPowered NOTIFY senderPowerChanged)

dbus-send --session --type=method_call --print-reply --dest='org.example.SenderExample' / org.example.sender.SendCommand string:'changed' 


NOTE:
So, client access remote property, read/write but not notified from server
although server always sends PropertiesChanged signal for all client examples
since  the server is the same. Add changes to client


CLIENT 3
========

1. use sender_interface_extended.* which use `DBusAbstractInterface` to have
property changed delivered from server

class OrgExampleSenderInterface : public DBusAbstractInterface

// trigger PropertyChanged
dbus-send --session --type=method_call --print-reply --dest='org.example.SenderExample' / org.example.sender.SendCommand string:'changed' 

$ ./client3
org.example.client.use.proxy started
changed property org.example.sender.Powered     // from DBusAbstractInterface
got powerChanged signal ( true ) from the remote sender

// access property do not trigger PropertyChagned
dbus-send --session --type=method_call --print-reply --dest='org.example.SenderExample' / org.example.sender.SendCommand string:'command'


RETURN

dbus-send --session --type=method_call --print-reply --dest='org.example.SenderExample' / org.example.sender.SendCommand string:'error'

*/

#include "controller.h"

#define SERVICE_NAME "org.example.adaptor"

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  if (!QDBusConnection::sessionBus().isConnected())
  {
    fprintf(stderr,
            "Cannot connect to the D-Bus session bus.\n"
            "To start it, run:\n"
            "\teval `dbus-launch --auto-syntax`\n");
    return 1;
  }

  // as with chat example, create Adaptor and register it to dbus
  // class SenderAdaptor: public QDBusAbstractAdaptor;

  // see that:
  //
  // 1. use of adaptor
  // 2. use of parent.
  //  chat example use adaptor within CharMainWindow class so able to use:
  //  new ChatAdaptor(this);
  //
  //  ping example do not use adaptor
  //
  //  complexping uses adaptor and pass QObject as parent
  //
  // this parent is passed up to QDBusAbstractAdaptor and since not sure what
  // will happen if remove this parent connection, keep this.

  QObject obj;
  Controller1Adaptor *adaptor = new Controller1Adaptor(&obj);

  // see that register `obj` but not
  if (QDBusConnection::sessionBus().registerObject("/", &obj))
    qDebug() << "org.example.adaptor is registered";
  else
  {
    qDebug() << "org.example.adaptor is not registered";
    return 1;
  }

  if (!QDBusConnection::sessionBus().registerService(SERVICE_NAME))
  {
    fprintf(stderr,
            "%s\n",
            qPrintable(QDBusConnection::sessionBus().lastError().message()));
    printf("pong cannot register service and exit\n");
    exit(1);
  }

  qDebug() << "org.example.adaptor registered and started";

  app.exec();

  qDebug() << "org.example.adaptor ended";

  return 0;
}
