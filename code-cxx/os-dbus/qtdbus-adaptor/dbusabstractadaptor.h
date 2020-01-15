#ifndef DBUSABSTRACTADAPTOR_H
#define DBUSABSTRACTADAPTOR_H

/* ={--------------------------------------------------------------------------
  from BluetoothRcu/daemon/source/dbus/dbusabstractadaptor.h
*/

class DBusAbstractAdaptor : public QDBusAbstractAdaptor
{
  Q_OBJECT

  private:
    QList<QDBusConnection> m_dbusConnections;
    QDBusContext *m_parentContext;

  public:
    virtual ~DBusAbstractAdaptor(Object *parent) = default;
};

#endif // DBUSABSTRACTADAPTOR_H
