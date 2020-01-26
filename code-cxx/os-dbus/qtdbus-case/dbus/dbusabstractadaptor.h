#ifndef DBUSABSTRACTADAPTOR_H
#define DBUSABSTRACTADAPTOR_H

#include <QDBusAbstractAdaptor>
#include <QDBusConnection>
#include <QDBusContext>
#include <QDBusMessage>
#include <QDebug>

// from:
// appinfrastructure/AppInfrastructure/BluetoothRcu/daemon/source/dbus/dbusabstractadaptor.h

class DBusAbstractAdaptor : public QDBusAbstractAdaptor
{
  Q_OBJECT

private:
	QList<QDBusConnection> m_dbusConnections;
	QDBusContext *m_parentContext;

protected:
  explicit DBusAbstractAdaptor(QObject *parent);

public:
  virtual ~DBusAbstractAdaptor() = default;

  void registerConnection(const QDBusConnection &);
  void unregisterConnection(const QDBusConnection &);

protected:
  void sendErrorReply(const QDBusMessage &request,
      const QString &name,
      const QString &message) const;

  void sendReply(const QDBusMessage &request, const QVariant &result) const;

  // TODO
  // skip `connectFutureToDBusReply` family for now
  //
  // TODO
  // sendPropertyChangeNotification
};

#endif // DBUSABSTRACTADAPTOR_H
