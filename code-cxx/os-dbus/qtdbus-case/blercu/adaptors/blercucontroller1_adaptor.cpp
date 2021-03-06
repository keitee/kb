/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -a blercucontroller1_adaptor.h:blercucontroller1_adaptor.cpp com.sky.blercu.Controller1.xml
 *
 * qdbusxml2cpp is Copyright (C) 2019 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "blercucontroller1_adaptor.h"
#include "blercucontroller.h"
#include <QDebug>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QMetaObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class Controller1Adaptor
 */

BleRcuController1Adaptor::BleRcuController1Adaptor(
  BleRcuController *parent, const QDBusObjectPath &objPath)
    : DBusAbstractAdaptor(parent)
    , m_controller(parent)
    , m_dbusObjectPath(objPath)
{
  // NOTE:
  // https://doc.qt.io/qt-5/qdbusabstractadaptor.html
  // QDBusAbstractAdaptor uses the standard QObject mechanism of signals, slots
  // and properties to determine what signals, methods and properties to export
  // to the bus. Any signal emitted by QDBusAbstractAdaptor-derived classes will
  // be automatically be relayed through any D-Bus connections the object is
  // registered on.

  // void QDBusAbstractAdaptor::setAutoRelaySignals(bool enable)
  //
  // Toggles automatic signal relaying from the real object (see object()).
  //
  // Automatic signal relaying consists of signal-to-signal connection of the
  // signals on the parent that have the exact same method signatue in both
  // classes.
  //
  // If enable is set to true, connect the signals; if set to false, disconnect
  // all signals.

  // don't use "auto replay signals, do this manually using connect()
  // setAutoRelaySignals(true);
  setAutoRelaySignals(false);

  // connect to the device added and removed signals from parent
  QObject::connect(m_controller,
                   &BleRcuController::managedDeviceAdded,
                   this,
                   &BleRcuController1Adaptor::onDeviceAdded);

  // SKIP other connect() calls to be simple
}

BleRcuController1Adaptor::~BleRcuController1Adaptor()
{
  // destructor
}

// not generated but manually added
// slot called by `BleRcuController` when a new managed device is added.
// We hook this signal to emit another deviceAdded signal over dbus.
void BleRcuController1Adaptor::onDeviceAdded(const QString &address)
{
  // const QSharedPointer<const BleRcuDevice> device
  //  = m_controller->managedDevice(address);
  // if (!device || !device->isValid()) {
  //   qError() << "odd, failed to find" << address << "in added slot";
  //   return;
  // }

  // That is, when parent, BleRcuController, signals and it's called. This gets
  // device from parent and if it's valid, then sends out
  // `com.sky.blercu.Controller1.DeviceAdded` signal

  qDebug("BleRcuController1Adaptor::onDeviceAdded: emit DeviceAdded");

  // emit DeviceAdded(device->objectPath(), address.toString());
  // emit DeviceAdded(device->objectPath(), address.toString());
}

// not generated but manually added
// to emit `org.freedesktop.DBus.Properties.PropertiesChanged` signal over dbus.
template <typename T>
void BleRcuController1Adaptor::emitPropertyChanged(const QString &name,
                                                   const T &value) const
{
  qDebug() << "BleRcuController1Adaptor::emitPropertyChanged: name: " << name
           << ", value: " << value;

  // TODO
  // sendPropertyChangeNotification<T>(m_dbusObjPath.path(), name, value);
}

// call for com.sky.blercu.Controller1.Pairing
bool BleRcuController1Adaptor::pairing() const
{
  // generated code:
  // // get the value of property Pairing
  // return qvariant_cast< bool >(parent()->property("Pairing"));

  // calls parent's one
  qDebug("BleRcuController1Adaptor::pairing: calls m_controller->isPairing()");
  return m_controller->isPairing();
}

// call for com.sky.blercu.Controller1.PairingCode
quint8 BleRcuController1Adaptor::pairingCode() const
{
  // generated code:
  // // get the value of property PairingCode
  // return qvariant_cast< uchar >(parent()->property("PairingCode"));

  auto ret = m_controller->pairingCode();

  // calls parent's one
  qDebug("BleRcuController1Adaptor::pairingCode: calls m_controller->pairingCode()");
  qDebug() << "ret: " << ret;

  return ret;
}

// call for com.sky.blercu.Controller1.StartPairing
void BleRcuController1Adaptor::StartPairing(uchar pairing_code,
                                      const QDBusMessage &message)
{
  // generated code:
  // // handle method call com.sky.blercu.Controller1.StartPairing
  // QMetaObject::invokeMethod(parent(),
  //                           "StartPairing",
  //                           Q_ARG(uchar, pairing_code));

  // calls parent's one
  m_controller->startPairing(pairing_code);

  // TODO: reply
  // BleRcuError::errorString(BleRcuError::General) do:
  // return QStringLiteral("com.sky.Error.Failed");

  qDebug() << "BleRcuController1Adaptor::StartPairing: code: " << pairing_code;

  sendErrorReply(message,
                 // BleRcuError::errorString(BleRcuError::General),
                 QStringLiteral("com.sky.Error.Failed"),
                 QStringLiteral("Not in pairing state"));
  return;
}

// call for com.sky.blercu.Controller1.CancelPairing
void BleRcuController1Adaptor::CancelPairing(const QDBusMessage &message)
{
  // generated code:
  // // handle method call com.sky.blercu.Controller1.CancelPairing
  // QMetaObject::invokeMethod(parent(), "CancelPairing");

  // TODO: reply
  qDebug("BleRcuController1Adaptor::CancelPairing: return reply");
  sendErrorReply(message,
                 QStringLiteral("com.sky.Error.Failed"),
                 QStringLiteral("Not in pairing state"));
  return;

  // calls parent's one
  m_controller->cancelPairing();
}

// call for com.sky.blercu.Controller1.GetDevices
QList<QDBusObjectPath> BleRcuController1Adaptor::GetDevices()
{
  // generated code:
  // // handle method call com.sky.blercu.Controller1.GetDevices
  // QList<QDBusObjectPath> devices;
  // QMetaObject::invokeMethod(parent(),
  //                           "GetDevices",
  //                           Q_RETURN_ARG(QList<QDBusObjectPath>, devices));
  // return devices;

  qDebug("BleRcuController1Adaptor::GetDevices()");

  QList<QDBusObjectPath> devices;
  // TODO: fill `devices` with sample data
  return devices;
}

// call for com.sky.blercu.Controller1.IsReady
void BleRcuController1Adaptor::IsReady()
{
  // generated code:
  // // handle method call com.sky.blercu.Controller1.IsReady
  // QMetaObject::invokeMethod(parent(), "IsReady");

  qDebug("BleRcuController1Adaptor::IsReady()");

  // NOTE: simply replay it to a signal
  emit Ready();
}

// call for com.sky.blercu.Shutdown
void BleRcuController1Adaptor::Shutdown()
{
  // generated code:
  // // handle method call com.sky.blercu.Controller1.Shutdown
  // QMetaObject::invokeMethod(parent(), "Shutdown");

  qDebug("BleRcuController1Adaptor::Shutdown()");

  // TODO: qApp?? where doet it come from?
  qApp->quit();
}
