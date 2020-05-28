//
//  utl_dbuspropertychangespy.cpp
//  BleRcuDaemon
//
//  Copyright © 2017 Sky UK. All rights reserved.
//

#include "utl_dbuspropertychangespy.h"

#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QTimer>

#include <functional>

DBusPropertyChangeSpy::DBusPropertyChangeSpy(const QDBusConnection &dbusConn,
                                             const QString &objPath)
    : QObject(nullptr)
    , m_dbusConn(dbusConn)
    , m_signalled(false)
{
  EXPECT_TRUE(m_dbusConn.isConnected());
  EXPECT_TRUE(
    m_dbusConn.connect("",
                       objPath,
                       "org.freedesktop.DBus.Properties",
                       "PropertiesChanged",
                       "sa{sv}as",
                       this,
                       SLOT(onPropertyChangeSignal(const QString &,
                                                   const QVariantMap &,
                                                   const QStringList &))));
}

DBusPropertyChangeSpy::~DBusPropertyChangeSpy() {}

bool DBusPropertyChangeSpy::wait(int timeout)
{
  QTimer timeOutTimer;
  timeOutTimer.setSingleShot(true);
  timeOutTimer.start(timeout);

  // add a dummy function to wake the event thread
  std::function<void()> lambda = []() { ; };
  QObject::connect(&timeOutTimer, &QTimer::timeout, lambda);

  while (timeOutTimer.isActive() && !m_signalled)
  {
    QCoreApplication::processEvents();
  }

  return m_signalled;
}

bool DBusPropertyChangeSpy::signalled() const
{
  return m_signalled;
}

QString DBusPropertyChangeSpy::interfaceName() const
{
  EXPECT_TRUE(m_signalled);
  return m_interfaceName;
}

QVariantMap DBusPropertyChangeSpy::changedProperties() const
{
  EXPECT_TRUE(m_signalled);
  return m_changedProperties;
}

QStringList DBusPropertyChangeSpy::invalidatedProperties() const
{
  EXPECT_TRUE(m_signalled);
  return m_invalidatedProperties;
}

void DBusPropertyChangeSpy::onPropertyChangeSignal(
  const QString &interfaceName,
  const QVariantMap &changedProperties,
  const QStringList &invalidatedProperties)
{
  EXPECT_FALSE(m_signalled);

  m_interfaceName         = interfaceName;
  m_changedProperties     = changedProperties;
  m_invalidatedProperties = invalidatedProperties;

  m_signalled = true;
}
