#include "blercucontroller_impl.h"

BleRcuControllerImpl::BleRcuControllerImpl(
  const QSharedPointer<ConfigSettings> &config, QObject *parent)
    : BleRcuController(parant)
    , m_config(config) // pass shared pointer
    , m_objectPath(QStringLiteral("/com/sky/bleruc/controller"))
    , m_lastError(BleRcuError::NoError)
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

// returns true if this object has been registered on dbus

bool BleRcuControllerImpl::isRegisteredToDBus(const QDBusConnection &dbus) const
{
  // QObject *QDBusConnection::objectRegisteredAt(const QString &path) const
  // Return the object that was registered with the registerObject() at the object path given by path.

  return (dbus.objectRegisteredAt(m_objectPath.path()) == this);
}

bool BleRcuControllerImpl::registerToDBus(const QDBusConnection &dbus) const
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
    qError("failed to register blercu controller object");
    return false;
  }

  // tell all adaptors that they're now registered on the dbus. this is used for
  // property change notification

  for (const auto &adaptor : m_adaptors)
  {
    adaptor.registerConnection(connection);
  }
}
