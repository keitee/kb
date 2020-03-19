//
//  tst_blercucontrolleradaptor.cpp
//  BleRcuDaemon
//
//  Copyright © 2017 Sky UK. All rights reserved.
//

#include <adaptors/blercucontroller1_adaptor.h>
// #include "logging.h"

#include "mck_blercucontroller.h"
// #include "mck_blercudevice.h"
// #include "mck_blercuservices.h"
// #include "mck_blercuaudioservice.h"
// #include "mck_blercubatteryservice.h"
// #include "mck_blercudeviceinfoservice.h"
// #include "mck_blercufindmeservice.h"
// #include "mck_blercuinfraredservice.h"
// #include "mck_blercutouchservice.h"

#include "utl_dbuspropertychangespy.h"
#include "utl_dbussignalspy.h"
#include "utl_eventloop.h"

#include <gtest/gtest.h>

#include <QDebug>
#include <QUuid>
#include <QDBusServer>
#include <QSharedPointer>

#include <functional>

/*
 
https://doc.qt.io/qt-5/qdbusserver.html#details

The QDBusServer class provides peer-to-peer communication between processes on
the same computer.

*/

class BleRcuControllerAdaptorTest : public ::testing::Test
{
public:
	void SetUp()
	{
		m_mockController = QSharedPointer<MockBleRcuController>::create();

		m_dbusServer = new QDBusServer();
		ASSERT_NE(m_dbusServer, nullptr);
		ASSERT_TRUE(m_dbusServer->isConnected());
		m_dbusServer->setAnonymousAuthenticationAllowed(true);

		m_newConnectionReady = false;

		QObject::connect(m_dbusServer, &QDBusServer::newConnection,
		                 std::bind(&BleRcuControllerAdaptorTest::onNewConnection, this, std::placeholders::_1));

		processEventsFor(10);

		// for some reason the name of the server has to be unique, use a UUID as conventient for now
		const QUuid name = QUuid::createUuid();
		m_dbusConn = QSharedPointer<QDBusConnection>::create(QDBusConnection::connectToPeer(m_dbusServer->address(), name.toString()));

		// if all goes to plan then onNewConnection() should be called, wait for
		// that to happen before proceeding
		std::function<bool()> lambda = [this]() {
			return m_newConnectionReady;
		};

		ASSERT_TRUE(processEventsUtil(lambda, 1000));
		ASSERT_TRUE(m_dbusConn->isConnected());
	}

	void TearDown()
	{
		m_dbusConn.reset();

		m_dbusServer->deleteLater();
		m_dbusServer = nullptr;

		m_mockController.reset();
	}

protected:
	QDBusServer *m_dbusServer;
	QSharedPointer<QDBusConnection> m_dbusConn;
	bool m_newConnectionReady;

	QSharedPointer<MockBleRcuController> m_mockController;

protected:
	const QString m_deviceObjPath = QStringLiteral("/com/sky/blercu/monkeycontroller");
	const QString m_interfaceName = QStringLiteral("com.sky.blercu.Controller1");

protected:
	void onNewConnection(const QDBusConnection &connection)
	{
		QDBusConnection newConn(connection);

		// will be owned by m_mockController and therefore automatically
		// deleted with the mock controller is destructed (no memory leak here)
		BleRcuController1Adaptor *adaptor = new BleRcuController1Adaptor(m_mockController.data(),
		                                                                 QDBusObjectPath(m_deviceObjPath));
		adaptor->registerConnection(newConn);

		if (!newConn.registerObject(m_deviceObjPath, m_mockController.data()))
			qCritical("failed to register blercucontroller adaptor object");

		// indicate the next connection is ready
		m_newConnectionReady = true;
	}

protected:
	QDBusMessage constructGetPropertyMethod(const QString &name)
	{
		QDBusMessage request = QDBusMessage::createMethodCall("", m_deviceObjPath, "org.freedesktop.DBus.Properties", "Get");
		request << m_interfaceName;
		request << name;
		return request;
	}

	QDBusVariant getPropertyFromDBus(const QString &name)
	{
		// construct the request
		QDBusMessage request = constructGetPropertyMethod(name);

		// perform the request
		QDBusMessage reply = m_dbusConn->call(request, QDBus::BlockWithGui);
		EXPECT_EQ(reply.type(), QDBusMessage::ReplyMessage);

		// check the reply
		QList<QVariant> results = reply.arguments();
		EXPECT_EQ(results.length(), 1);
		EXPECT_TRUE(results.first().canConvert<QDBusVariant>());

		return results.first().value<QDBusVariant>();
	}
};


TEST_F(BleRcuControllerAdaptorTest, testPairingProperty)
{
	const QList<bool> values = { true, false };

	for (bool value : values) {

		// expect the the device name will be queried
		EXPECT_CALL(*m_mockController, isPairing())
			.WillOnce(::testing::Return(value));

		// get the property
		const QDBusVariant pairingVar = getPropertyFromDBus("Pairing");

		// check it
		const QVariant pairing = pairingVar.variant();
		EXPECT_TRUE(pairing.canConvert<bool>());
		EXPECT_EQ(pairing.toBool(), value);
	}
}


TEST_F(BleRcuControllerAdaptorTest, testPairingPropertyChanged)
{
	const QList<bool> values = { true, false };

	for (const bool &value : values) {

		// create an object to watch for the signal
		DBusPropertyChangeSpy propSpy(*m_dbusConn, m_deviceObjPath);

		// emit a pairing state change from the controller object
		emit m_mockController->pairingStateChanged(value);

		// check the event was sent over dbus as a prop change
		ASSERT_TRUE(propSpy.wait(10000));

		ASSERT_TRUE(propSpy.signalled());
		ASSERT_EQ(propSpy.interfaceName(), m_interfaceName);

		const QVariantMap changedProps = propSpy.changedProperties();
		ASSERT_TRUE(changedProps.contains("Pairing"));

		const QVariant propValue = changedProps["Pairing"];
		ASSERT_TRUE(propValue.canConvert<bool>());
		ASSERT_EQ(propValue.value<bool>(), value);
		
	}
}

TEST_F(BleRcuControllerAdaptorTest, testPairingCodeProperty)
{
	const QList<quint8> values = { 0, 33, 255 };

	for (quint8 value : values) {

		// expect the the device name will be queried
		EXPECT_CALL(*m_mockController, pairingCode())
			.WillOnce(::testing::Return(value));

		// get the property
		const QDBusVariant codeVar = getPropertyFromDBus("PairingCode");

		// check it
		const QVariant code = codeVar.variant();
		EXPECT_TRUE(code.canConvert<quint8>());
		EXPECT_EQ(code.value<quint8>(), value);
	}
}


TEST_F(BleRcuControllerAdaptorTest, testIsReady)
{
	// IsReady is a method call that doesn't respond but instead sends back a
	// Ready signal

	// create an object to watch for the signal
	DBusSignalSpy signalSpy(*m_dbusConn, m_deviceObjPath, m_interfaceName, "Ready");

	// construct the request
	QDBusMessage request = QDBusMessage::createMethodCall("", m_deviceObjPath, m_interfaceName, "IsReady");

	// send the request
	m_dbusConn->call(request, QDBus::BlockWithGui);


	// check the event was sent over dbus as a prop change
	ASSERT_TRUE(signalSpy.wait(1000));
	ASSERT_EQ(signalSpy.count(), 1);
}

#if 0

TEST_F(BleRcuControllerAdaptorTest, testStartPairing)
{
	// check happy case
	const QList<quint8> values = { 0, 123, 255 };
	for (const quint8 &value : values) {

		// construct the request
		QDBusMessage request = QDBusMessage::createMethodCall("", m_deviceObjPath, m_interfaceName, "StartPairing");
		request << QVariant::fromValue(value);

		// expect the the controller will be called to start pairing
		// return success
		EXPECT_CALL(*m_mockController, isPairing())
			.WillOnce(::testing::Return(false));
		EXPECT_CALL(*m_mockController, startPairing(value))
			.WillOnce(::testing::Return(true));

		// check for a positive reply
		QDBusMessage reply = m_dbusConn->call(request, QDBus::BlockWithGui);
		EXPECT_EQ(reply.type(), QDBusMessage::ReplyMessage);
	}


	// check already pairing case
	{
		// construct the request
		QDBusMessage request = QDBusMessage::createMethodCall("", m_deviceObjPath, m_interfaceName, "StartPairing");
		request << QVariant::fromValue<quint8>(123);

		// expect the the controller will be called to start pairing
		// return success
		EXPECT_CALL(*m_mockController, isPairing())
			.WillOnce(::testing::Return(true));

		// check for a positive reply
		QDBusMessage reply = m_dbusConn->call(request, QDBus::BlockWithGui);
		EXPECT_EQ(reply.type(), QDBusMessage::ErrorMessage);
		EXPECT_EQ(reply.errorName(), BleRcuError::errorString(BleRcuError::Busy));
		EXPECT_EQ(reply.errorMessage(), QString("Already in pairing state"));
	}


	// check failure case
	{
		// construct the request
		QDBusMessage request = QDBusMessage::createMethodCall("", m_deviceObjPath, m_interfaceName, "StartPairing");
		request << QVariant::fromValue<quint8>(234);

		// expect the the controller will be called to start pairing, it fails
		// and then calls lastError
		EXPECT_CALL(*m_mockController, isPairing())
			.WillOnce(::testing::Return(false));
		EXPECT_CALL(*m_mockController, startPairing(234))
			.WillOnce(::testing::Return(false));
		EXPECT_CALL(*m_mockController, lastError())
			.WillOnce(::testing::Return(BleRcuError(BleRcuError::BadFormat, "Monkey gone to heaven")));

		// check for an error reply
		QDBusMessage reply = m_dbusConn->call(request, QDBus::BlockWithGui);
		EXPECT_EQ(reply.type(), QDBusMessage::ErrorMessage);
		EXPECT_EQ(reply.errorName(), BleRcuError::errorString(BleRcuError::BadFormat));
		EXPECT_EQ(reply.errorMessage(), QString("Monkey gone to heaven"));
	}
}


TEST_F(BleRcuControllerAdaptorTest, testGetDevices)
{
	BleAddress bdaddr1("11:22:33:44:55:66");
	QDBusObjectPath objpath1("/com/sky/blercu/device_11_22_33_44_55_66");
	QSharedPointer<MockBleRcuDevice> device1 = QSharedPointer<MockBleRcuDevice>::create();
	EXPECT_CALL(*device1, isValid())
		.WillOnce(::testing::Return(true));
	EXPECT_CALL(*device1, objectPath())
		.WillOnce(::testing::Return(objpath1));

	BleAddress bdaddr2("AA:BB:CC:DD:EE:FF");
	QDBusObjectPath objpath2("/com/sky/blercu/device_AA_BB_CC_DD_EE_FF");
	QSharedPointer<MockBleRcuDevice> device2 = QSharedPointer<MockBleRcuDevice>::create();
	EXPECT_CALL(*device2, isValid())
		.WillOnce(::testing::Return(true));
	EXPECT_CALL(*device2, objectPath())
		.WillOnce(::testing::Return(objpath2));

	BleAddress bdaddr3("DE:AD:BE:EF:00:01");
	QDBusObjectPath objpath3("/com/sky/blercu/device_DE_AD_BE_EF_00_01");
	QSharedPointer<MockBleRcuDevice> device3 = QSharedPointer<MockBleRcuDevice>::create();
	EXPECT_CALL(*device3, isValid())
		.WillOnce(::testing::Return(true));
	EXPECT_CALL(*device3, objectPath())
		.WillOnce(::testing::Return(objpath3));



	// construct the dbus request
	QDBusMessage request = QDBusMessage::createMethodCall("", m_deviceObjPath, m_interfaceName, "GetDevices");

	// expect the following calls to the controller
	EXPECT_CALL(*m_mockController, managedDevices())
		.WillOnce(::testing::Return(QSet<BleAddress>{bdaddr1, bdaddr2, bdaddr3}));

	EXPECT_CALL(*m_mockController, managedDevice(bdaddr1))
		.WillOnce(::testing::Return(device1));
	EXPECT_CALL(*m_mockController, managedDevice(bdaddr2))
		.WillOnce(::testing::Return(device2));
	EXPECT_CALL(*m_mockController, managedDevice(bdaddr3))
		.WillOnce(::testing::Return(device3));


	// check the dbus reply
	QDBusMessage reply = m_dbusConn->call(request, QDBus::BlockWithGui);
	EXPECT_EQ(reply.type(), QDBusMessage::ReplyMessage);

	const QList<QVariant> args = reply.arguments();
	ASSERT_EQ(args.length(), 1);
	ASSERT_TRUE(args.first().canConvert<QDBusArgument>());
	const QDBusArgument arg = args.first().value<QDBusArgument>();

	// tease out the object paths
	QList<QDBusObjectPath> paths;

	arg.beginArray();
        while (!arg.atEnd())
        {
          QDBusObjectPath path;
          arg >> path;
          paths.append(path);
        }
        arg.endArray();

	// check we got all the object paths for all 3 devices
	ASSERT_EQ(paths.length(), 3);
	EXPECT_TRUE(paths.contains(objpath1));
	EXPECT_TRUE(paths.contains(objpath2));
	EXPECT_TRUE(paths.contains(objpath3));
}

#endif

