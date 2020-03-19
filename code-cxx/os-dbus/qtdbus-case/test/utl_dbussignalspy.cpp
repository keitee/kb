//
//  utl_dbussignalspy.cpp
//  BleRcuDaemon
//
//  Copyright © 2017 Sky UK. All rights reserved.
//

#include "utl_dbussignalspy.h"

#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QTimer>

#include <functional>


DBusSignalSpy::DBusSignalSpy(const QDBusConnection &dbusConn,
                             const QString &path,
                             const QString &interface,
                             const QString &name)
	: QObject(nullptr)
	, m_dbusConn(dbusConn)
	, m_count(0)
{
	EXPECT_TRUE(m_dbusConn.isConnected());
	EXPECT_TRUE(m_dbusConn.connect("", path, interface, name, this, SLOT(onDBusSignal())));
}

DBusSignalSpy::~DBusSignalSpy()
{
}

bool DBusSignalSpy::wait(int timeout)
{
	QTimer timeOutTimer;
	timeOutTimer.setSingleShot(true);
	timeOutTimer.start(timeout);

	// add a dummy function to wake the event thread
	std::function<void()> lambda = []() {
		;
	};
	QObject::connect(&timeOutTimer, &QTimer::timeout, lambda);

	while (timeOutTimer.isActive() && (m_count == 0)) {
		QCoreApplication::processEvents();
	}

	return (m_count > 0);
}

int DBusSignalSpy::count() const
{
	return m_count;
}

void DBusSignalSpy::clear()
{
	m_count = 0;
}

void DBusSignalSpy::reset()
{
	m_count = 0;
}

void DBusSignalSpy::onDBusSignal()
{
	m_count++;
}
