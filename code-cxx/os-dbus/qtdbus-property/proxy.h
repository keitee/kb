/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -p proxy.h:proxy.cpp org.example.sender.xml
 *
 * qdbusxml2cpp is Copyright (C) 2019 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef PROXY_H
#define PROXY_H

#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface org.example.sender
 */
class OrgExampleSenderInterface : public QDBusAbstractInterface
{
  Q_OBJECT
public:
  static inline const char *staticInterfaceName()
  {
    return "org.example.sender";
  }

public:
  OrgExampleSenderInterface(const QString &service,
                            const QString &path,
                            const QDBusConnection &connection,
                            QObject *parent = nullptr);

  ~OrgExampleSenderInterface();

  Q_PROPERTY(bool Powered READ powered WRITE setPowered)
  inline bool powered() const
  {
    return qvariant_cast<bool>(property("Powered"));
  }
  inline void setPowered(bool value)
  {
    setProperty("Powered", QVariant::fromValue(value));
  }

public Q_SLOTS: // METHODS
  inline QDBusPendingReply<> ConnectProfile(const QString &UUID)
  {
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(UUID);
    return asyncCallWithArgumentList(QStringLiteral("ConnectProfile"),
                                     argumentList);
  }

Q_SIGNALS: // SIGNALS
  void aboutToQuit();
  void action(const QString &nickname, const QString &text);
};

namespace org
{
  namespace example
  {
    typedef ::OrgExampleSenderInterface sender;
  }
} // namespace org
#endif