#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>
#include <QSharedPointer>

#include <signal.h>

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
    QSharedPointer<BleRcuController>::create(config, manager, options->networkNamespace());
  if (!controller || !controller->isValid())
  {
    qFatal("failed to setup the `blercu controller`");
  }
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

  // qDebug() << "qtbus-case daemon started";

  // int exitCode = app.exec();

  // qDebug() << "qtbus-case daemon ended";

  // return exitCode;
}
