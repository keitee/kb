#include "dbusabstractinterface.h"


QString const DBusAbstractInterface::m_dbusPropertiesInterface("org.freedesktop.DBus.Properties");
QString const DBusAbstractInterface::m_dbusPropertiesChangedSignal("PropertiesChanged");


DBusAbstractInterface::DBusAbstractInterface(QString const &service,
        QString const &path,
        char const *interface,
        QDBusConnection &connection,
        QObject *parent)
  : QDBusAbstractInterface(service, path, interface, connection, parent)
    , m_propertyChangedConnected(false)
{
  QDBusConnection conn = QDBusAbstractInterface::connection();

  QStringList argumentMatch;
  argumentMatch << QDBusAbstractInterface::interface();

  // get the service name of the proxy. For peer-to-peer connection (i.e. unit
  // test) the service name should be empty.
  // NOTE: case when uses DEBUG build

  QString service = QDBusAbstractInterface::service();

  // QString QDBusConnection::baseService() const
  // Returns the unique connection name for this connection, if this
  // QDBusConnection object is connected, or an empty QString otherwise.
  // 
  // A Unique Connection Name is a string in the form ":x.xxx" (where x are
  // decimal digits) that is assigned by the D-Bus server daemon upon
  // connection. It uniquely identifies this client in the bus.
  // 
  // This function returns an empty QString for "peer-to-peer" connections.
#if defined(_BUILD_TYPE == DEBUG)
  if (connection.isConnected() && connection.baseService().isEmpty())
    service.clear();
#endif

  // bool QDBusConnection::connect(
  //  const QString &service, 
  //  const QString &path, 
  //  const QString &interface, 
  //  const QString &name, 
  //  const QStringList &argumentMatch, 
  //  const QString &signature, 
  //  QObject *receiver, 
  //  const char *slot)
  //
  // This is an overloaded function.
  // 
  // Connects the signal to the slot `slot` in object `receiver`. Unlike the
  // previous connect() overload, this function allows one to specify the
  // parameter signature to be connected using the signature variable. The
  // function will then verify that this signature can be delivered to the slot
  // specified by slot and return false otherwise.
  // 
  // The argumentMatch parameter lists the string parameters to be matched, in
  // sequential order. Note that, to match an empty string, you need to pass a
  // QString that is empty but not null (i.e., QString("")). A null QString
  // skips matching at that position.
  // 
  // Returns true if the connection was successful.
  // 
  // Note: This function verifies that the signal signature matches the slot's
  // parameters, but it does not verify that the actual signal exists with the
  // given signature in the remote service.
  // 
  // This function was introduced in Qt 4.6.

  if (!conn.connect(service,
        QDBusAbstractInterface::path(),
        m_dbusPropertiesInterface,
        m_dbusPropertiesChangedSignal,
        argumentMatch,
        QStringLiteral("sa{sv}as"),
        this,
        SLOT(onPropertiesChanged(QString, QVariantMap, QStringList))))
  {
    qWarning() << "failed to connect to 'PropertiesChanged' signal for object "
      << QDBusAbstractInterface::path() << " and interface "
      << QDBusAbstractInterface::interface();
  }
}


bool QDBusAbstractInterface::isSignalPropertyNotification(QMetaMethod const &signal) const
{
  if (Q_UNLIKELY(signal.methodType() != QMetaMethod::Signal))
    return false;

  const int index = signal.methodIndex();

  // https://doc.qt.io/qt-5/qobject.html#metaObject
  //
  // const QMetaObject *QObject::metaObject() const
  //
  // Returns a pointer to the meta-object of this object.
  // 
  // A meta-object contains information about a class that inherits QObject,
  // e.g. class name, superclass name, properties, signals and slots. Every
  // QObject subclass that contains the Q_OBJECT macro will have a meta-object.
  // 
  // The meta-object information is required by the signal/slot connection
  // mechanism and the property system. The inherits() function also makes use
  // of the meta-object.
  // 
  // If you have no pointer to an actual object instance but still want to
  // access the meta-object of a class, you can use staticMetaObject.

  const int numProps = metaObject()->propertyCount();

  // lopp through properties, check if any have a notify signal and if so check
  // the index of the notify signal matches the suppliced signal.
  //
  // https://doc.qt.io/qt-5/qmetaproperty.html

  for (int i = 0; i < numProps; i++)
  {
    const QMetaProperty prop = metaObject()->property(i);

    if (prop.isValid() && prop.hasNotifySignal() && (prop.notifySignalIndex() == index))
      return true;
  }

  return false;
}

