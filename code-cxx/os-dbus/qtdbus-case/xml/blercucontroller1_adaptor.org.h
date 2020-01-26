//
//  blercucontroller1_adaptor.h
//  BleRcuDaemon
//
//  Copyright © 2017 Sky UK. All rights reserved.
//

#ifndef BLERCUCONTROLLER1_ADAPTOR_H
#define BLERCUCONTROLLER1_ADAPTOR_H

#include "dbusabstractadaptor.h"

#include <QObject>
#include <QtDBus>
#include <QSharedPointer>

class BleRcuController;
class BleAddress;

class BleRcuController1Adaptor : public DBusAbstractAdaptor
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
	Q_PROPERTY(bool Pairing READ pairing)
	Q_PROPERTY(quint8 PairingCode READ pairingCode)

public:
	BleRcuController1Adaptor(BleRcuController *parent,
	                         const QDBusObjectPath &objPath);
	virtual ~BleRcuController1Adaptor();

public:
	bool pairing() const;
	quint8 pairingCode() const;

public slots:
	void StartPairing(quint8 pairingCode, const QDBusMessage &message);
	void CancelPairing(const QDBusMessage &message);

	QList<QDBusObjectPath> GetDevices(const QDBusMessage &message);

	Q_NOREPLY void IsReady();
	void Shutdown();

signals:
	void DeviceAdded(const QDBusObjectPath &path, const QString &address);
	void DeviceRemoved(const QDBusObjectPath &path, const QString &address);
	void Ready();

private:
	template <typename T>
	void emitPropertyChanged(const QString &propName, const T &propValue) const;

private:
	void onDeviceAdded(const BleAddress &address);
	void onDeviceRemoved(const BleAddress &address);
	void onPairingStateChanged(bool pairing);

private:
	BleRcuController * const m_controller;
	const QDBusObjectPath m_dbusObjPath;
};

#endif // BLERCUCONTROLLER1_ADAPTOR_H
