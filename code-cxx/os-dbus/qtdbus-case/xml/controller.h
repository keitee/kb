/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -a controller.h:controller.cpp com.sky.blercu.Controller1.xml
 *
 * qdbusxml2cpp is Copyright (C) 2019 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
QT_BEGIN_NAMESPACE
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
QT_END_NAMESPACE

/*
 * Adaptor class for interface com.sky.blercu.Controller1
 */
class Controller1Adaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.sky.blercu.Controller1")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"com.sky.blercu.Controller1\">\n"
"    <method name=\"Shutdown\"/>\n"
"    <method name=\"StartPairing\">\n"
"      <arg direction=\"in\" type=\"y\" name=\"pairing_code\"/>\n"
"    </method>\n"
"    <method name=\"CancelPairing\"/>\n"
"    <method name=\"GetDevices\">\n"
"      <arg direction=\"out\" type=\"ao\" name=\"devices\"/>\n"
"    </method>\n"
"    <method name=\"IsReady\">\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\"/>\n"
"    </method>\n"
"    <signal name=\"DeviceAdded\">\n"
"      <arg type=\"o\" name=\"path\"/>\n"
"      <arg type=\"s\" name=\"address\"/>\n"
"    </signal>\n"
"    <signal name=\"DeviceRemoved\">\n"
"      <arg type=\"o\" name=\"path\"/>\n"
"      <arg type=\"s\" name=\"address\"/>\n"
"    </signal>\n"
"    <signal name=\"Ready\"/>\n"
"    <property access=\"read\" type=\"b\" name=\"Pairing\"/>\n"
"    <property access=\"read\" type=\"y\" name=\"PairingCode\"/>\n"
"  </interface>\n"
        "")
public:
    Controller1Adaptor(QObject *parent);
    virtual ~Controller1Adaptor();

public: // PROPERTIES
    Q_PROPERTY(bool Pairing READ pairing)
    bool pairing() const;

    Q_PROPERTY(uchar PairingCode READ pairingCode)
    uchar pairingCode() const;

public Q_SLOTS: // METHODS
    void CancelPairing();
    QList<QDBusObjectPath> GetDevices();
    Q_NOREPLY void IsReady();
    void Shutdown();
    void StartPairing(uchar pairing_code);
Q_SIGNALS: // SIGNALS
    void DeviceAdded(const QDBusObjectPath &path, const QString &address);
    void DeviceRemoved(const QDBusObjectPath &path, const QString &address);
    void Ready();
};

#endif
