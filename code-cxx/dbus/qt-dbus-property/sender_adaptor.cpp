/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -i sender_adaptor.h -a :sender_adaptor.cpp
 * org.example.sender.xml
 *
 * qdbusxml2cpp is Copyright (C) 2019 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "sender_adaptor.h"
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QMetaObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class SenderAdaptor
 */

SenderAdaptor::SenderAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
  // constructor
  setAutoRelaySignals(true);

  connect(this, SIGNAL(powerChanged(bool)), this, SLOT(onPowerChanged(bool)));
}

SenderAdaptor::~SenderAdaptor()
{
  // destructor
}

bool SenderAdaptor::powered() const
{
  // get the value of property Powered
  qDebug() << "SenderAdaptor::powered(" << m_powered << ") called";
  return qvariant_cast<bool>(parent()->property("Powered"));
  // return m_powered;
}

/*

bool QObject::setProperty(const char *name, const QVariant &value)

Sets the value of the object's name property to value.

If the property is defined in the class using Q_PROPERTY then true is returned
on success and false otherwise. If the property is not defined using Q_PROPERTY,
and therefore not listed in the meta-object, it is added as a dynamic property
and false is returned.

Information about all available properties is provided through the metaObject()
and dynamicPropertyNames().

Dynamic properties can be queried again using property() and can be removed by
setting the property value to an invalid QVariant. Changing the value of a
dynamic property causes a QDynamicPropertyChangeEvent to be sent to the object.

Note: Dynamic properties starting with "_q_" are reserved for internal purposes.

See also property(), metaObject(), dynamicPropertyNames(), and
QMetaProperty::write().

*/

void SenderAdaptor::setPowered(bool value)
{
  // set the value of property Powered
  parent()->setProperty("Powered", QVariant::fromValue(value));

  m_powered = value;
  qDebug() << "SenderAdaptor::setPowered(" << m_powered << ") called";
}

// As with ping example, QString Pong::ping(const QString &arg), use this slot
// to ends the sender
void SenderAdaptor::ConnectProfile(const QString &UUID)
{
  emit aboutToQuit();

  // // handle method call org.example.sender.ConnectProfile
  // QMetaObject::invokeMethod(parent(), "ConnectProfile", Q_ARG(QString,
  // UUID));

  QMetaObject::invokeMethod(QCoreApplication::instance(), "quit");
  qDebug() << QString(
                "sender::ConnectProfile(\"%1\") got called and ask to quit")
                .arg(UUID);
}

void SenderAdaptor::SendCommand(const QString &command)
{
  qDebug() << QString("sender::SendCommand(\"%1\") got called").arg(command);
  QTimer::singleShot(2000, this, SLOT(onTimerExpired()));

  // set property value and `powerChanged` is emitted
  qDebug() << "sender::setProperty(false)";
  setProperty("Powered", false);
}

void SenderAdaptor::onTimerExpired()
{
  qDebug() << "timer expired and emit signal";
  emit action("from sender", "do you hear me?");
}

void SenderAdaptor::onPowerChanged(bool power)
{
  qDebug() << "got powerChanged signal (" << power << ")";
}
