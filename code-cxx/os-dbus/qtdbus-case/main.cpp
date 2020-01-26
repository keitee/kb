/*

./qtdbus-case --session

dbus-send --session --type=method_call --print-reply --dest='com.sky.blercu' / org.freedesktop.DBus.Introspectable.Introspect

method return time=1580063005.227481 sender=:1.97 -> destination=:1.99 serial=3 reply_serial=2
   string "<!DOCTYPE node PUBLIC "-//freedesktop//DTD D-BUS Object Introspection 1.0//EN"
"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd">
<node>
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
  <node name="com"/>
</node>
"


dbus-send --session --type=method_call --print-reply --dest='com.sky.blercu' /com/sky/bleruc/controller org.freedesktop.DBus.Introspectable.Introspect

method return time=1580063123.107212 sender=:1.97 -> destination=:1.101 serial=5 reply_serial=2
   string "<!DOCTYPE node PUBLIC "-//freedesktop//DTD D-BUS Object Introspection 1.0//EN"
"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd">
<node>
  <interface name="com.sky.blercu.Controller1">
    <method name="Shutdown"/>
    <method name="StartPairing">
      <arg direction="in" type="y" name="pairing_code"/>
    </method>
    <method name="CancelPairing"/>
    <method name="GetDevices">
      <arg direction="out" type="ao" name="devices"/>
    </method>
    <method name="IsReady">
      <annotation value="true" name="org.freedesktop.DBus.Method.NoReply"/>
    </method>
    <signal name="DeviceAdded">
      <arg type="o" name="path"/>
      <arg type="s" name="address"/>
    </signal>
    <signal name="DeviceRemoved">
      <arg type="o" name="path"/>
      <arg type="s" name="address"/>
    </signal>
    <signal name="Ready"/>
    <property access="read" type="b" name="Pairing"/>
    <property access="read" type="y" name="PairingCode"/>
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

$ busctl tree com.sky.blercu
Failed to introspect object / of service com.sky.blercu: The name com.sky.blercu was not provided by any .service files
No objects discovered.


dbus-send --session --type=method_call --print-reply --dest='com.sky.blercu' /com/sky/bleruc/controller com.sky.blercu.Controller1.StartPairing byte:10
Error com.sky.Error.Failed: Not in pairing state

BleRcuController1Adaptor::IsReady()
BleRcuController1Adaptor::StartPairing: code:  10
DBusAbstractAdaptor::sendErrorReply:
QDBusMessage(type=Error, service="", error name="com.sky.Error.Failed", error message="Not in pairing state", signature="", contents=() )


dbus-send --session --type=method_call --print-reply --dest='com.sky.blercu' /com/sky/bleruc/controller com.sky.blercu.Controller1.IsReady
method return time=1580064168.679080 sender=:1.103 -> destination=:1.106 serial=6 reply_serial=2


dbus-send --session --type=method_call --print-reply --dest='com.sky.blercu' /com/sky/bleruc/controller com.sky.blercu.Controller1.Shutdown

*/

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>
#include <QSharedPointer>

#include <signal.h>

#include <blercucontroller_p.h>
#include <cmdlineoptions.h>
#include <configsettings.h>

static QSharedPointer<QDBusConnection> setupDBus_(CmdLineOptions::DBusType type,
                                                  const QString &address,
                                                  const QString &service)
{
  QSharedPointer<QDBusConnection> connection;

  if (CmdLineOptions::DBusType::SystemBus == type)
  {
    // QDBusConnection(const QDBusConnection &other);

    connection =
      QSharedPointer<QDBusConnection>::create(QDBusConnection::systemBus());
  }
  else if (CmdLineOptions::DBusType::SessionBus == type)
  {
    connection =
      QSharedPointer<QDBusConnection>::create(QDBusConnection::sessionBus());
  }
  else if (CmdLineOptions::DBusType::CustomBus == type)
  {
    // https://doc.qt.io/qt-5/qdbusconnection.html
    //
    // D-Bus also supports peer-to-peer connections, without the need for a bus
    // server daemon. Using this facility, two applications can talk to each
    // other and exchange messages. This can be achieved by passing an address
    // to connectToBus() function, which was opened by another D-Bus application
    // using QDBusServer.

    connection = QSharedPointer<QDBusConnection>::create(
      QDBusConnection::connectToBus(address, service));
  }
  else
    return QSharedPointer<QDBusConnection>();

  // check the connection
  if (!connection || !connection->isConnected())
  {
    // https://doc.qt.io/qt-5/qdbuserror.html
    // QString	name() const

    QDBusError error = connection->lastError();
    qCritical("failed to connect to dbus, due to %s'",
        qPrintable(error.name()));
    return QSharedPointer<QDBusConnection>();
  }

  qDebug() << "setupDBus_: service " << service;

  // register ourselves on the dbus
  if (!connection->registerService(service))
  {
    QDBusError error = connection->lastError();
    qCritical("failed to register service, due to %s'",
        qPrintable(error.name()));
    return QSharedPointer<QDBusConnection>();
  }

  return connection;
}

static QSharedPointer<BleRcuController> setupBleRcuController_(
    const QSharedPointer<CmdLineOptions> &option,
    const QSharedPointer<ConfigSettings> &config,
    const QSharedPointer<QDBusConnection> &connection)
{
  // create the controller object
  QSharedPointer<BleRcuController> controller =
    QSharedPointer<BleRcuControllerImpl>::create(config);

  // // QSharedPointer<BleRcuControllerImpl>::create(config, manager, options->networkNamespace());

  if (!controller)
  {
    qFatal("failed to setup the `blercu controller`");
  }

	// register on the controller on dbus
  controller->registerToDBus(*connection);

  return controller;
}

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);

  // TODO:
  // disable the SIGPIPE
  // why "it is the most annoying signal in the world"?
  signal(SIGPIPE, SIG_IGN);

  // parse the command line options
  // BleRcuDaemon --noconsole --session --netns=3 --audio-fifo-dir=/tmp/blercu/
  QSharedPointer<CmdLineOptions> options =
    QSharedPointer<CmdLineOptions>::create();

  options->process(app);

#if 0
  // error when call isSet() without process():
  // QCommandLineParser: call process() or parse() before isSet

  if (options->isSet("help"))
    qDebug() << "option help is set";

  if (options->isSet("noconsole"))
  {
    qDebug() << "option noconsole is set, value="
             << options->value("noconsole");
  }
#endif

  // create the config options
  QSharedPointer<ConfigSettings> config = ConfigSettings::defaults();

  // connect to dbus
  // NOTE: do not use `address` for system and session bus and no default for
  // that.
  //
  // DBusType::SystemBus, "com.sky.blercu"

  QSharedPointer<QDBusConnection> connection =
    setupDBus_(options->dbusType(),
               options->dbusAddress(),
               options->dbusService());
  if (!connection)
    return EXIT_FAILURE;

  // TODO:
#if 0
#if (AI_BUILD_TYPE == AI_DEBUG)
	if (options->debugDBusType() != CmdLineOptions::NoBus) {
		debugDBusConn = setupDBus(options->debugDBusType(),
		                          options->debugDBusAddress(),
		                          options->dbusServiceName());
	} else if (qEnvironmentVariableIsSet("DBUS_DEBUG_BUS_ADDRESS")) {
		debugDBusConn = setupDBus(CmdLineOptions::CustomBus,
		                          qgetenv("DBUS_DEBUG_BUS_ADDRESS"),
		                          QStringLiteral("com.sky.blercu.debug"));
	}
#endif // (AI_BUILD_TYPE == AI_DEBUG)
#endif
  
  // starts blercu controller
  QSharedPointer<BleRcuController> controller =
    setupBleRcuController_(options, config, connection);
  if (!controller)
    return EXIT_FAILURE;

  qDebug() << "qtbus-case daemon started";

  int exitCode = app.exec();

  qDebug() << "qtbus-case daemon ended";

  return exitCode;
}
