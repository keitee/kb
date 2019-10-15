/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -i sender_adaptor.h -a :sender_adaptor.cpp org.example.sender.xml
 *
 * qdbusxml2cpp is Copyright (C) 2019 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "sender_adaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
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
}

SenderAdaptor::~SenderAdaptor()
{
    // destructor
}

bool SenderAdaptor::powered() const
{
    // get the value of property Powered
    return qvariant_cast< bool >(parent()->property("Powered"));
}

void SenderAdaptor::setPowered(bool value)
{
    // set the value of property Powered
    parent()->setProperty("Powered", QVariant::fromValue(value));
}

void SenderAdaptor::ConnectProfile(const QString &UUID)
{
    // handle method call org.example.sender.ConnectProfile
    QMetaObject::invokeMethod(parent(), "ConnectProfile", Q_ARG(QString, UUID));
}

