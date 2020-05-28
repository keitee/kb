//
//  utl_dbuspropertychangespy.h
//  BleRcuDaemon
//
//  Copyright © 2017 Sky UK. All rights reserved.
//

#ifndef UTL_DBUSPROPERTYCHANGESPY_H
#define UTL_DBUSPROPERTYCHANGESPY_H

#include <QDBusConnection>
#include <QObject>
#include <QString>
#include <QVariantMap>

class DBusPropertyChangeSpy : public QObject
{
  Q_OBJECT

public:
  DBusPropertyChangeSpy(const QDBusConnection &dbusConn,
                        const QString &objPath);
  ~DBusPropertyChangeSpy();

  bool wait(int timeout = 5000);

  bool signalled() const;
  QString interfaceName() const;
  QVariantMap changedProperties() const;
  QStringList invalidatedProperties() const;

private slots:
  void onPropertyChangeSignal(const QString &interfaceName,
                              const QVariantMap &changedProperties,
                              const QStringList &invalidatedProperties);

private:
  QDBusConnection m_dbusConn;

  bool m_signalled;
  QString m_interfaceName;
  QVariantMap m_changedProperties;
  QStringList m_invalidatedProperties;
};

#endif // !defined(UTL_DBUSPROPERTYCHANGESPY_H)
