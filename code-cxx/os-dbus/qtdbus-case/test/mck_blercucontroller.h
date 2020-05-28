//
//  mck_blercucontroller.h
//  BleRcuDaemon
//
//  Copyright © 2017 Sky UK. All rights reserved.
//

#ifndef MCK_BLERCUCONTROLLER_H
#define MCK_BLERCUCONTROLLER_H

#include <QDBusContext>
#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <QtDBus>

#include "blercucontroller.h"

#include <gmock/gmock.h>

class MockBleRcuController : public BleRcuController, public QDBusContext
{
  Q_OBJECT

public:
  MockBleRcuController()
      : BleRcuController(nullptr)
  {}

  ~MockBleRcuController() {}

public:
  MOCK_CONST_METHOD1(isRegisteredToDBus, bool(const QDBusConnection &));
  MOCK_METHOD1(registerToDBus, bool(const QDBusConnection &));
  MOCK_METHOD1(unregisterFromDBus, bool(const QDBusConnection &));

public:
  MOCK_CONST_METHOD0(isValid, bool());

  // MOCK_CONST_METHOD0(lastError, BleRcuError());

  MOCK_CONST_METHOD0(isPairing, bool());
  MOCK_CONST_METHOD0(pairingCode, quint8());

  MOCK_METHOD1(startPairing, bool(quint8));
  MOCK_METHOD0(cancelPairing, void());

  MOCK_CONST_METHOD0(managedDevices, QString());

  // MOCK_CONST_METHOD0(managedDevices, QSet<BleAddress>());
  // MOCK_CONST_METHOD1(managedDevice, QSharedPointer<BleRcuDevice>(const BleAddress&));
};

#endif // !defined(MCK_BLERCUCONTROLLER_H)
