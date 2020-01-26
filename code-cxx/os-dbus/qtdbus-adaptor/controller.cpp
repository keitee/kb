/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -a controller.h:controller.cpp com.sky.blercu.Controller1.xml
 *
 * qdbusxml2cpp is Copyright (C) 2019 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "controller.h"
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

Controller1Adaptor::Controller1Adaptor(QObject *parent)
    //    : QDBusAbstractAdaptor(parent)
    : DBusAbstractAdaptor(parent)
{
  // constructor
  setAutoRelaySignals(true);
}

Controller1Adaptor::~Controller1Adaptor()
{
  // destructor
}

bool Controller1Adaptor::pairing() const
{
  // // get the value of property Pairing
  // return qvariant_cast< bool >(parent()->property("Pairing"));
  qDebug("Controller1Adaptor::pairing: return true");
  return true;
}

uchar Controller1Adaptor::pairingCode() const
{
  // // get the value of property PairingCode
  // return qvariant_cast< uchar >(parent()->property("PairingCode"));
  qDebug("Controller1Adaptor::pairingCode: return 10");
  return 10;
}

void Controller1Adaptor::CancelPairing(const QDBusMessage &message)
{
  // // handle method call com.sky.blercu.Controller1.CancelPairing
  // QMetaObject::invokeMethod(parent(), "CancelPairing");

  // generated code:
  // // handle method call com.sky.blercu.Controller1.CancelPairing
  // QMetaObject::invokeMethod(parent(), "CancelPairing");

  // TODO: reply
  // BleRcuError::errorString(BleRcuError::General) do:
  // return QStringLiteral("com.sky.Error.Failed");

  qDebug("Controller1Adaptor::CancelPairing: return reply");
  sendErrorReply(message,
                 QStringLiteral("com.sky.Error.Failed"),
                 QStringLiteral("Not in pairing state"));
  return;
}

QList<QDBusObjectPath> Controller1Adaptor::GetDevices()
{
  // // handle method call com.sky.blercu.Controller1.GetDevices
  // QList<QDBusObjectPath> devices;
  // QMetaObject::invokeMethod(parent(), "GetDevices", Q_RETURN_ARG(QList<QDBusObjectPath>, devices));
  // return devices;

  qDebug("Controller1Adaptor::GetDevices:");
  QList<QDBusObjectPath> devices;
  // TODO: fill `devices` with sample data
  return devices;
}

void Controller1Adaptor::IsReady()
{
  // // handle method call com.sky.blercu.Controller1.IsReady
  // QMetaObject::invokeMethod(parent(), "IsReady");

  qDebug("Controller1Adaptor::IsReady:");
}

void Controller1Adaptor::Shutdown()
{
  // // handle method call com.sky.blercu.Controller1.Shutdown
  // QMetaObject::invokeMethod(parent(), "Shutdown");

  qDebug("Controller1Adaptor::Shutdown:");
}

void Controller1Adaptor::StartPairing(uchar pairing_code,
                                      const QDBusMessage &message)
{
  // // handle method call com.sky.blercu.Controller1.StartPairing
  // QMetaObject::invokeMethod(parent(), "StartPairing", Q_ARG(uchar, pairing_code));

  // TODO: reply
  qDebug("Controller1Adaptor::StartPairing: return reply");
  sendErrorReply(message,
                 QStringLiteral("com.sky.Error.Failed"),
                 QStringLiteral("Not in pairing state"));
  return;
}
