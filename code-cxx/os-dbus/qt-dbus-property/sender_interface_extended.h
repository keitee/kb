/*
 * copied from sender_interface.h
 */

#ifndef SENDER_INTERFACE_EXTENDED_H
#define SENDER_INTERFACE_EXTENDED_H

#include "dbusabstractinterface.h"

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface org.example.sender
 */
// class OrgExampleSenderInterface: public QDBusAbstractInterface
class OrgExampleSenderInterface : public DBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.example.sender"; }

public:
  OrgExampleSenderInterface(const QString &service,
                            const QString &path,
                            const QDBusConnection &connection,
                            QObject *parent = nullptr);

  ~OrgExampleSenderInterface();

  Q_PROPERTY(
    bool Powered READ powered WRITE setPowered NOTIFY senderPowerChanged)
  inline bool powered() const
  {
    return qvariant_cast<bool>(property("Powered")); }
    inline void setPowered(bool value)
    { setProperty("Powered", QVariant::fromValue(value)); }

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<> ConnectProfile(const QString &UUID)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(UUID);
        return asyncCallWithArgumentList(QStringLiteral("ConnectProfile"), argumentList);
    }

    void onPowerChanged(bool power);

Q_SIGNALS: // SIGNALS
    void aboutToQuit();
    void action(const QString &nickname, const QString &text);

    // need to define signal as well
    void senderPowerChanged(bool powered);
};

namespace org {
  namespace example {
    typedef ::OrgExampleSenderInterface sender;
  }
}

#endif // SENDER_INTERFACE_EXTENDED_H
