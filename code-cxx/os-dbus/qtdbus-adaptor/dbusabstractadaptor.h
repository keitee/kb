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

    void registerConnection(const QDBusConnection &);
    void unregisterConnection(const QDBusConnection &);

  protected:
    void sendErrorReply(const QDBusMessage &request,
        const QString &name, const QString &message) const;

    void sendReply(const QDBusMessage &request, const QVariant &result) const;

  protected:
    template <typename T>
      void connectFutureToBusReply(const QDBusMessage &request,
          const Future<T> &result) const
      {
        if (Q_UNLIKELY(nullptr == m_parentContext))
        {
          qWarning("missing parent dbus context");
          return;
        }

        // copy connection
        const QDBusConnection connection(m_parentContext->connection());

        // lambda to create an error message and send it
        const auto errorFunction =
          [connection, request](const QString &name, const QString &message) {
            QDBusMessage error = request.createErrorReply(name, message);
            if (!connection.send(error))
            {
              qWarning() << "failed to send error reply to request, " << error;
            }

            // TODO
            qDebug() << "connectFutureToBusReply::errorFunction: error, " << error;
          };

        // lambda to create the result into dbus message and send it
        const auto finishedFunction =
          [connection, request] (const T &value)
          {
            QDBusMessage reply = request.createReply(QVariant::fromValue<T>(value));
            if (!connection.send(reply))
            {
              qWarning() << "failed to send reply to request, " << reply;
            }

            // TODO
            qDebug() << "connectFutureToBusReply::finishedFunction: error, " << reply;
          };

        request.setDelayedReply(true);
      }
};

#endif // DBUSABSTRACTADAPTOR_H
