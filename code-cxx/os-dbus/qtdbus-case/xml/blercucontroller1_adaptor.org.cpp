//
//  blercucontroller1_adaptor.cpp
//  BleRcuDaemon
//
//  Copyright © 2017 Sky UK. All rights reserved.
//
#include "blercucontroller1_adaptor.h"
#include "blercucontroller.h"
#include "blercudevice.h"

#include "logging.h"

#include <QCoreApplication>


BleRcuController1Adaptor::BleRcuController1Adaptor(BleRcuController *parent,
                                                   const QDBusObjectPath &objPath)
	: DBusAbstractAdaptor(parent)
	, m_controller(parent)
	, m_dbusObjPath(objPath)
{
	// don't auto relay signals, we do this manually
	setAutoRelaySignals(false);

	// connect to the device added and removed signals
	QObject::connect(m_controller, &BleRcuController::managedDeviceAdded,
	                 this, &BleRcuController1Adaptor::onDeviceAdded);
	QObject::connect(m_controller, &BleRcuController::managedDeviceRemoved,
	                 this, &BleRcuController1Adaptor::onDeviceRemoved);

	// connect to the pairing state change signals
	QObject::connect(m_controller, &BleRcuController::pairingStateChanged,
	                 this, &BleRcuController1Adaptor::onPairingStateChanged);

}

BleRcuController1Adaptor::~BleRcuController1Adaptor()
{
	// destructor
}

// -----------------------------------------------------------------------------
/*!
	\internal

	Slot called by the \l{BleRcuController} when a new managed device is added.
	We hook this signal to emit another deviceAdded signal over dbus.

 */
void BleRcuController1Adaptor::onDeviceAdded(const BleAddress &address)
{
	// for the device added signal we also want to broadcast the dbus object path
	const QSharedPointer<const BleRcuDevice> device = m_controller->managedDevice(address);
	if (!device || !device->isValid()) {
		qError() << "odd, failed to find" << address << "in added slot";
		return;
	}

	// send out the com.sky.BleRcuController1.DeviceAdded dbus signal
	emit DeviceAdded(device->objectPath(), address.toString());
}

// -----------------------------------------------------------------------------
/*!
	\internal

	Slot called by the \l{BleRcuController} when a new managed device is added.
	We hook this signal to emit another deviceRemoved signal over dbus.

 */
void BleRcuController1Adaptor::onDeviceRemoved(const BleAddress &address)
{
	// FIXME: for now I've hard-coded the object paths for the devices, however
	// we probably should get these from the BleRcuController object
	const QVarLengthArray<quint8, 6> bdaddrArray = address.toArray();
	const QString objPath = QString::asprintf("/com/sky/blercu/device_%02hhX_%02hhX_%02hhX_%02hhX_%02hhX_%02hhX",
	                                          bdaddrArray[0], bdaddrArray[1], bdaddrArray[2],
	                                          bdaddrArray[3], bdaddrArray[4], bdaddrArray[5]);

	// send out the com.sky.BleRcuController1.DeviceRemoved dbus signal
	emit DeviceRemoved(QDBusObjectPath(objPath), address.toString());
}

// -----------------------------------------------------------------------------
/*!
	\internal

	Utility function to emit a org.freedesktop.DBus.Properties.PropertiesChanged
	signal over the dbus interface.

 */
template <typename T>
void BleRcuController1Adaptor::emitPropertyChanged(const QString &propName,
                                                   const T &propValue) const
{
	sendPropertyChangeNotification<T>(m_dbusObjPath.path(), propName, propValue);
}

// -----------------------------------------------------------------------------
/*!
	\internal

	Slot called by the \l{BleRcuController} when the pairing state changes. We
	hook this point so we can send out a property changed signal for the
	'Pairing' property.

 */
void BleRcuController1Adaptor::onPairingStateChanged(bool pairing)
{
	emitPropertyChanged(QStringLiteral("Pairing"), pairing);
}

// -----------------------------------------------------------------------------
/*!
	DBus get property call for com.sky.BleRcuController1.Pairing

 */
bool BleRcuController1Adaptor::pairing() const
{
	return m_controller->isPairing();
}

// -----------------------------------------------------------------------------
/*!
	DBus get property call for com.sky.BleRcuController1.PairingCode

 */
quint8 BleRcuController1Adaptor::pairingCode() const
{
	return m_controller->pairingCode();
}

// -----------------------------------------------------------------------------
/*!
	DBus method call handler for com.sky.BleRcuController1.StartPairing

 */
void BleRcuController1Adaptor::StartPairing(quint8 pairingCode,
                                            const QDBusMessage &message)
{
	// sanity check we're not already in the pairing state
	if (m_controller->isPairing()) {
		sendErrorReply(message, BleRcuError::errorString(BleRcuError::Busy),
		               QStringLiteral("Already in pairing state"));
		return;
	}

	// attempt to start pairing using the supplied code
	if (!m_controller->startPairing(pairingCode)) {

		const BleRcuError error = m_controller->lastError();
		sendErrorReply(message, error.name(), error.message());

	}
}

// -----------------------------------------------------------------------------
/*!
	DBus method call handler for com.sky.BleRcuController1.CancelPairing

 */
void BleRcuController1Adaptor::CancelPairing(const QDBusMessage &message)
{
	// sanity check we're actually in the pairing state
	if (!m_controller->isPairing()) {
		sendErrorReply(message, BleRcuError::errorString(BleRcuError::General),
		               QStringLiteral("Not in pairing state"));
		return;
	}

	// cancel the pairing
	m_controller->cancelPairing();
}

// -----------------------------------------------------------------------------
/*!
	DBus method call handler for com.sky.BleRcuController1.GetDevices

 */
QList<QDBusObjectPath> BleRcuController1Adaptor::GetDevices(const QDBusMessage &message)
{
	Q_UNUSED(message);

	QList<QDBusObjectPath> devicePaths;

	const QSet<BleAddress> addresses = m_controller->managedDevices();
	for (const BleAddress &address : addresses) {

		const QSharedPointer<const BleRcuDevice> device = m_controller->managedDevice(address);
		if (device && device->isValid()) {
			devicePaths.append(device->objectPath());
		}
	}

	return devicePaths;
}

// -----------------------------------------------------------------------------
/*!
	DBus method call handler for com.sky.BleRcuController1.IsReady

 */
void BleRcuController1Adaptor::IsReady()
{
	// send out a com.sky.BleRcuController1.Ready signal
	emit Ready();
}

// -----------------------------------------------------------------------------
/*!
	DBus method call handler for com.sky.BleRcuController1.Shutdown

 */
void BleRcuController1Adaptor::Shutdown()
{
	// the following will post a quit message to the main event loop
	qApp->quit();
}


