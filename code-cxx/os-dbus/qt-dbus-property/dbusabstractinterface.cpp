#include "dbusabstractinterface.h"

#include <QDebug>
#include <QMetaProperty>

#include <QDBusArgument>
#include <QDBusMetaType>

// -----------------------------------------------------------------------------
/*

  \class DBusAbstractInterface
  \brief Wrapper around the QDBusAbstractInterface class to provide notify
  signals for property changes.

  The dbus specficiation defines the org.freedesktop.DBus.Properties interface
  for getting / setting properties, Qt already implements this, however it
  doesn't implement a handler for the
  org.freedesktop.DBus.Properties.PropertiesChanged signal.

  This is a problem for us as bluetoothd uses this to notify us of all sorts
  of things; i.e. scan on/off, powered on/off, etc

  \see
  https://randomguy3.wordpress.com/2010/09/07/the-magic-of-qtdbus-and-the-propertychanged-signal/

  \see https://github.com/nemomobile/qtdbusextended


  So how do you use this class ?
  1. Generate an interface class using the Qt qdbusxml2cpp tool
  2. Change the generated class so that it inherits from the
  \a DBusAbstractInterface class rather than \a QDBusAbstractInterface
  3. Add the \c NOTIFY option to the properties of the generated class and
  matching signals (just like you would for an ordinary notify property)

*/

QString const DBusAbstractInterface::m_dbusPropertiesInterface(
  "org.freedesktop.DBus.Properties");

QString const
  DBusAbstractInterface::m_dbusPropertiesChangedSignal("PropertiesChanged");

DBusAbstractInterface::DBusAbstractInterface(QString const &service,
                                             QString const &path,
                                             char const *interface,
                                             const QDBusConnection &connection,
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

  QString serviceName = QDBusAbstractInterface::service();

  // QString QDBusConnection::baseService() const
  // Returns the unique connection name for this connection, if this
  // QDBusConnection object is connected, or an empty QString otherwise.
  //
  // A Unique Connection Name is a string in the form ":x.xxx" (where x are
  // decimal digits) that is assigned by the D-Bus server daemon upon
  // connection. It uniquely identifies this client in the bus.
  //
  // This function returns an empty QString for "peer-to-peer" connections.
#if defined(_BUILD_TYPE) && (_BUILD_TYPE == DEBUG)
  if (connection.isConnected() && connection.baseService().isEmpty())
    serviceName.clear();
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

  if (!conn.connect(
        serviceName,
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

/* ={--------------------------------------------------------------------------
 @brief :
  checks whether the supplied method matches a notification signal and if it is
  true is returned.

  returns true if the supplied method is a signal that is attached to a property
  as a change notification; the signal that is supplied after the NOTIFY keyword
  in the Q_PROPERTY declaration.
*/

bool DBusAbstractInterface::isSignalPropertyNotification(
  QMetaMethod const &signal) const
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

    if (prop.isValid() && prop.hasNotifySignal() &&
        (prop.notifySignalIndex() == index))
      return true;
  }

  return false;
}

/* ={--------------------------------------------------------------------------
 @brief :
  connectNotify/disconnectNotify are called by QT when someone
  connects/disconnects signals. Override this as normally QDBusAbstractInterface
  would handle this and use this point to install a dbus match filter with the
  signal name. WHY? because any signals you put in class are ASSUMED to be
  signals sent by the remote object over dbus.

  However, this is NOT true of property change notify signals, rather these are
  signals used to notify of a change to a property as sent by the
  org.freedesktop.DBus.Properties.ProertiesChanged signal.

  So want to stop the base class, QDBusAbstractInterface, from installing dbus
  match filters for these signals.

  NOTE:
  use base class's connectNotify/disconnectNotify only when signal is not from
  property signal.
*/

void DBusAbstractInterface::connectNotify(QMetaMethod const &signal)
{
  // void QObject::connectNotify(const QMetaMethod &signal)
  // This virtual function is called when something has been connected to signal
  // in this object.

  if (!isSignalPropertyNotification(signal))
    QDBusAbstractInterface::connectNotify(signal);
}

void DBusAbstractInterface::disconnectNotify(QMetaMethod const &signal)
{
  if (!isSignalPropertyNotification(signal))
    QDBusAbstractInterface::disconnectNotify(signal);
}

/* ={--------------------------------------------------------------------------
 @brief :
  dbus callback when PropertiesChanged signal is received

  This will go through the list of properties that have changed and searches to
  see if this objcet has a Q_PROPERTY that matches, if it does and that that
  property has a NOTIFY signal we call the signal.

  NOTE more than one property change could be signalled by this method

*/
void DBusAbstractInterface::onPropertiesChanged(
  QString const &interface,
  QVariantMap const &changedProperties,
  QStringList const &invalidatedProperties)
{
  Q_UNUSED(invalidatedProperties);

  if (Q_UNLIKELY(interface != QDBusAbstractInterface::interface()))
  {
    qInfo() << "odd, received PropertiesChanged signal from wrong interface?";
    return;
  }

  auto it = changedProperties.cbegin();

  for (; it != changedProperties.cend(); ++it)
  {

    // const T QMap::value(const Key &key, const T &defaultValue = T()) const
    // Returns the value associated with the key key.
    //
    // If the map contains no item with key key, the function returns
    // defaultValue. If no defaultValue is specified, the function returns a
    // default-constructed value. If there are multiple items for key in the
    // map, the value of the most recently inserted one is returned.

    // see *cxx-return-temporary-cxx-temporary*
    // use reference of return temporary is allowed
    auto &name  = it.key();
    auto &value = it.value();

    // try to find the meta index of the named property
    // int index = metaObject()->indexOfProperty(name.toLatin1().constData());
    auto index = metaObject()->indexOfProperty(name.toLatin1().constData());
    if (index < 0)
    {
      qDebug("odd, no meta property for %s.%s",
             qPrintable(interface),
             qPrintable(name));
      continue;
    }

    // check if the property has the notify flag and if not, ignore the change.
    // NOTE: are there property changed without notify flag set???
    // const QMetaProperty prop = metaObject()->property(index);
    auto prop = metaObject()->property(index);
    if (!prop.hasNotifySignal())
    {
      qDebug("skipping property change notification for %s.%s",
             qPrintable(interface),
             qPrintable(name));
      continue;
    }

    // const QMetaMethod method = prop.notifySignal();
    auto method = prop.notifySignal();
    if (Q_UNLIKELY(!method.isValid()))
    {
      qWarning("odd, invalid property notify signal for %s.%s",
               qPrintable(interface),
               qPrintable(name));
      continue;
    }

    // As with isSignalPropertyNotification(), only notify if it's from property
    // notify.
    qDebug("changed property %s.%s", qPrintable(interface), qPrintable(name));
    invokeNotifySignal(method, name, value, prop.userType());
  } // for
}

/* ={--------------------------------------------------------------------------
 @brief :
  invoke the supplied signal with the variant value and it's type

enum QMetaMethod::MethodType
QMetaMethod::Method       0   The function is a plain member function.
QMetaMethod::Signal       1   The function is a signal.
QMetaMethod::Slot         2   The function is a slot.
QMetaMethod::Constructor  3   The function is a constructor.


int QMetaMethod::parameterType(int index) const
Returns the type of the parameter at the given index.

The return value is one of the types that are registered with QMetaType, or
QMetaType::UnknownType if the type is not registered.

This function was introduced in Qt 5.0.

*/
void DBusAbstractInterface::invokeNotifySignal(QMetaMethod const &method,
                                               QString const &name,
                                               QVariant const &value,
                                               int propertyType)
{
  // TODO???
  // check if the notify signal has any parameters, if not then no point
  // demarshalling the dbus parameters and instead just emit the signal
  // and return
  if (method.parameterCount() == 0)
  {
    method.invoke(this);
    return;
  }

  // TODO???
  // it has argument(s), it should only have one that matches the property
  // type and therefore should already support native demarshalling
  if (Q_UNLIKELY(method.parameterCount() > 1))
  {
    qWarning("too many parameters for '%s' property notify signal",
             qPrintable(name));
    return;
  }

  // TODO???
  // sanity check if the notify signal takes the same type as the actual
  // property.
  // if you see the following error then check that the signal
  // in the Q_PROPERTY(... NOTIFY <signal>) takes a matching argument type
  if (Q_UNLIKELY(propertyType != method.parameterType(0)))
  {
    qWarning("'%s' property notify signal takes inconsistent parameter type",
             qPrintable(name));
    return;
  }

  // if the received property type matches then
  if (value.userType() == propertyType)
  {
    QGenericArgument argument(QMetaType::typeName(propertyType),
                              const_cast<void *>(value.constData()));

    // https://doc.qt.io/qt-5/qt.html#ConnectionType-enum
    method.invoke(this, Qt::DirectConnection, argument);
  }
  // TODO???
  // if the received property type is a bit more complex ...
  else if (value.userType() == qMetaTypeId<QDBusArgument>())
  {
    const QDBusArgument dbusValue = value.value<QDBusArgument>();

    // get the expected signature from the actual qt properties ...
    const char *expectedSignature =
      QDBusMetaType::typeToSignature(propertyType);

    // ... then check against what dbus sent us
    if (dbusValue.currentSignature() != expectedSignature)
    {
      qWarning("mismatch signature on property '%s' notify signal, "
               "expected %s actual %s",
               qPrintable(name),
               expectedSignature,
               qPrintable(dbusValue.currentSignature()));
    }
    else
    {

      QVariant result = QVariant(propertyType, nullptr);
      if (!QDBusMetaType::demarshall(dbusValue, propertyType, result.data()) ||
          !result.isValid())
      {
        qWarning("failed to demarshall property '%s' value", qPrintable(name));
      }
      else
      {

        QGenericArgument argument(QMetaType::typeName(result.userType()),
                                  const_cast<void *>(result.constData()));
        method.invoke(this, Qt::DirectConnection, argument);
      }
    }
  }
}

/* ={--------------------------------------------------------------------------
 @brief :

Performs a org.freedesktop.DBus.Properties.Get method call requesting the given
property, however it returns the pending reply rather than blocking.

  <method name="Get">
    <arg name="interface" type="s" direction="in"/>
    <arg name="name" type="s" direction="in"/>
    <arg name="value" type="v" direction="out"/>
  </method>

https://doc.qt.io/qt-5/qdbusmessage.html

The QDBusMessage class represents one message sent or received over the D-Bus
bus.

Detailed Description

This object can represent any of the four different types of messages
(MessageType) that can occur on the bus:

Method calls
Method return values
Signal emissions
Error codes

Objects of this type are created with the static createError(),
createMethodCall() and createSignal() functions. Use the QDBusConnection::send()
function to send the messages.


QDBusMessage QDBusMessage::createMethodCall(
  const QString &service,
  const QString &path,
  const QString &interface,
  const QString &method)

Constructs a new DBus message representing a method call. A method call always
informs its destination address (service, path, interface and method).

The DBus bus allows calling a method on a given remote object without specifying
the destination interface, if the method name is unique. However, if two
interfaces on the remote object export the same method name, the result is
undefined (one of the two may be called or an error may be returned).

When using DBus in a peer-to-peer context (i.e., not on a bus), the service
parameter is optional.

The QDBusInterface class provides a simpler abstraction to synchronous method
calling.

This function returns a QDBusMessage object that can be sent with
QDBusConnection::call().

*/

QDBusPendingReply<QDBusVariant>
DBusAbstractInterface::asyncProperty(char const *name) const
{
  QDBusMessage msg =
    QDBusMessage::createMethodCall(QDBusAbstractInterface::service(),
                                   QDBusAbstractInterface::path(),
                                   m_dbusPropertiesInterface,
                                   QStringLiteral("Get"));

  // KT
  qDebug("are they different? '%s.%s'",
         m_dbusPropertiesInterface,
         QDBusAbstractInterface::interface());

  // args
  msg << QDBusAbstractInterface::interface();
  msg << QString::fromUtf8(name);

  // send out message
  //
  // QDBusPendingCall QDBusConnection::asyncCall(
  //  const QDBusMessage &message,
  //  int timeout = -1) const
  //
  // Sends the message over this connection and returns immediately. This
  // function is suitable for method calls only. It returns an object of type
  // QDBusPendingCall which can be used to track the status of the reply.
  //
  // If no reply is received within timeout milliseconds, an automatic error
  // will be delivered indicating the expiration of the call. The default
  // timeout is -1, which will be replaced with an implementation-defined value
  // that is suitable for inter-process communications (generally, 25 seconds).
  // This timeout is also the upper limit for waiting in
  // QDBusPendingCall::waitForFinished().
  //
  // See the QDBusInterface::asyncCall() function for a more friendly way of
  // placing calls.
  //
  // This function was introduced in Qt 4.5.

  return QDBusAbstractInterface::connection().asyncCall(msg);
}

/* ={--------------------------------------------------------------------------
 @brief :

 Performs a org.freedesktop.DBus.Properties.Set method call requesting
 a set on the given property, however it returns the pending reply rather
 than blocking.

  <method name="Set">
    <arg name="interface" type="s" direction="in"/>
    <arg name="name" type="s" direction="in"/>
    <arg name="value" type="v" direction="in"/>
  </method>

*/

QDBusPendingReply<>
DBusAbstractInterface::asyncSetProperty(char const *name,
                                        const QVariant &value) const
{
  QDBusMessage msg =
    QDBusMessage::createMethodCall(QDBusAbstractInterface::service(),
                                   QDBusAbstractInterface::path(),
                                   m_dbusPropertiesInterface,
                                   QStringLiteral("Set"));

  // KT
  qDebug("are they different? '%s.%s'",
         m_dbusPropertiesInterface,
         QDBusAbstractInterface::interface());

  // args
  msg << QDBusAbstractInterface::interface();
  msg << QString::fromUtf8(name);
  msg << QVariant::fromValue(QDBusVariant(value));

  return QDBusAbstractInterface::connection().asyncCall(msg);
}

/* ={--------------------------------------------------------------------------
 @brief :

  Performs a org.freedesktop.DBus.Properties.GetAll method call requesting
  the all properties of a given interface.  This is a non-block async call.

  <method name="GetAll">
    <arg name="interface" type="s" direction="in"/>
    <arg name="properties" type="a{sv}" direction="out"/>
    <annotation name="org.qtproject.QtDBus.QtTypeName.Out0"
 value="QVariantMap"/>
  </method>
*/

QDBusPendingReply<DBusPropertyMap>
DBusAbstractInterface::asyncGetAllProperties() const
{
  QDBusMessage msg =
    QDBusMessage::createMethodCall(QDBusAbstractInterface::service(),
                                   QDBusAbstractInterface::path(),
                                   m_dbusPropertiesInterface,
                                   QStringLiteral("GetAll"));

  // KT
  qDebug("are they different? '%s.%s'",
         m_dbusPropertiesInterface,
         QDBusAbstractInterface::interface());

  // args
  msg << QDBusAbstractInterface::interface();

  return QDBusAbstractInterface::connection().asyncCall(msg);
}
