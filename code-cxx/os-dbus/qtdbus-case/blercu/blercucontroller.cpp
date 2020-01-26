#include <adaptors/blercucontroller1_adaptor.h>
#include <blercucontroller_p.h>

BleRcuControllerImpl::BleRcuControllerImpl(
  const QSharedPointer<ConfigSettings> &config, QObject *parent)
    : BleRcuController(parent)
    , m_config(config) // pass shared pointer
    , m_objectPath(QStringLiteral("/com/sky/bleruc/controller"))
{
  // there were connect() calls to connect state machine signals and bluez
  // manager signals to its slots like:
  //
  // QObject::connect(&m_pairingStateMachine, &BleRcuPairingStateMachine::finished,
  //                  this, &BleRcuControllerImpl::onFinishedPairing,
  //                  Qt::QueuedConnection);

  // create and attach the dbus adaptor for the controller interface
  m_adaptors.append(new BleRcuController1Adaptor(this, m_objectPath));
}

BleRcuControllerImpl::~BleRcuControllerImpl() {}

// returns true if this object has been registered on dbus

bool BleRcuControllerImpl::isRegisteredToDBus(const QDBusConnection &dbus) const
{
  // QObject *QDBusConnection::objectRegisteredAt(const QString &path) const
  // Return the object that was registered with the registerObject() at the object path given by path.

  return (dbus.objectRegisteredAt(m_objectPath.path()) == this);
}

bool BleRcuControllerImpl::registerToDBus(const QDBusConnection &dbus)
{
  if (isRegisteredToDBus(dbus))
  {
    qWarning("it's already registered on dbus");
    return false;
  }

  // strip the const'ness off (bit of a hack). HOW? since it's copy of const
  // object.
  //
  // https://doc.qt.io/qt-5/qdbusconnection.html
  // Further attempts to connect using the same name will return the same connection.

  QDBusConnection connection(dbus);

  if (!connection.registerObject(m_objectPath.path(), this))
  {
    qWarning("failed to register blercu controller object");
    return false;
  }

  qDebug() << "BleRcuControllerImpl::registerToDBus: registered path "
           << m_objectPath.path();

  // tell all adaptors that they're now registered on the dbus. this is used for
  // property change notification

  for (const auto &adaptor : m_adaptors)
  {
    // DBusAbstractAdaptor::registerConnection()
    adaptor->registerConnection(connection);
  }

  return true;
}

bool BleRcuControllerImpl::unregisterFromDBus(const QDBusConnection &) {}

QString BleRcuControllerImpl::managedDevices() const
{
  qWarning("BleRcuControllerImpl::managedDevices() called");
  return "blercudevice1";
}

bool BleRcuControllerImpl::isPairing() const
{
  qWarning("BleRcuControllerImpl::isPairing() called");
  return true;
}

quint8 BleRcuControllerImpl::pairingCode() const
{
  qWarning("BleRcuControllerImpl::pairingCode() called");
  return 33;
}

bool BleRcuControllerImpl::startPairing(quint8 pairingCode)
{
  qWarning("BleRcuControllerImpl::startPairing() called");
  return true;
}

void BleRcuControllerImpl::cancelPairing()
{
  qWarning("BleRcuControllerImpl::cancelPairing() called");
}
