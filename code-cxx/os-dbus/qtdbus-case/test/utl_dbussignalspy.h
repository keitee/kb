//
//  utl_dbussignalspy.h
//  BleRcuDaemon
//
//  Copyright © 2017 Sky UK. All rights reserved.
//

#ifndef UTL_DBUSSIGNALSPY_H
#define UTL_DBUSSIGNALSPY_H

#include <QDBusConnection>
#include <QObject>
#include <QString>
#include <QVariantMap>

class DBusSignalSpy : public QObject
{
  Q_OBJECT

public:
  DBusSignalSpy(const QDBusConnection &dbusConn,
                const QString &path,
                const QString &interface,
                const QString &name);
  ~DBusSignalSpy();

  bool wait(int timeout = 5000);

  int count() const;

  void clear();
  void reset();

private slots:
  void onDBusSignal();

private:
  QDBusConnection m_dbusConn;
  int m_count;
};

#endif // !defined(UTL_DBUSSIGNALSPY_H)
