/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -a blercucontroller1_adaptor.h:blercucontroller1_adaptor.cpp com.sky.blercu.Controller1.xml
 *
 * qdbusxml2cpp is Copyright (C) 2019 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef BLERCUCONTROLLER1_ADAPTOR_H
#define BLERCUCONTROLLER1_ADAPTOR_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>

// #include "dbusabstractadaptor.h"
#include <dbusabstractadaptor.h>

// NOTE: since use it as s pointer
class BleRcuController;

// NOTE: removed
// QT_BEGIN_NAMESPACE
// class QByteArray;
// template<class T> class QList;
// template<class Key, class Value> class QMap;
// class QString;
// class QStringList;
// class QVariant;
// QT_END_NAMESPACE

/*
 * Adaptor class for interface com.sky.blercu.Controller1
 */

// NOTE:
// Although used `blercucontroller1_adaptor.h` in qdubxml2cpp, take
// `Controller1` from xml and used it as class name. So change it
// class Controller1Adaptor: public QDBusAbstractAdaptor
//
// NOTE:
// inherits from BusAbstractAdaptor

class BleRcuController1Adaptor : public DBusAbstractAdaptor
{
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "com.sky.blercu.Controller1")
  Q_CLASSINFO(
    "D-Bus Introspection",
    ""
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
    "      <annotation value=\"true\" "
    "name=\"org.freedesktop.DBus.Method.NoReply\"/>\n"
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

  // generated code:
  // public:
  //     Controller1Adaptor(QObject *parent);
  //     virtual ~Controller1Adaptor();

public:
  BleRcuController1Adaptor(BleRcuController *parent,
                           const QDBusObjectPath &objPath);
  virtual ~BleRcuController1Adaptor();

public: // PROPERTIES
  Q_PROPERTY(bool Pairing READ pairing)
  bool pairing() const;

  Q_PROPERTY(uchar PairingCode READ pairingCode)
  uchar pairingCode() const;

public Q_SLOTS: // METHODS
  // generated: void CancelPairing();
  void CancelPairing(const QDBusMessage &message);
  // generated: QList<QDBusObjectPath> GetDevices();
  QList<QDBusObjectPath> GetDevices();
  Q_NOREPLY void IsReady();
  void Shutdown();
  // generated: void StartPairing(uchar pairing_code);
  void StartPairing(quint8 pairingCode, const QDBusMessage &message);

Q_SIGNALS: // SIGNALS
  void DeviceAdded(const QDBusObjectPath &path, const QString &address);
  void DeviceRemoved(const QDBusObjectPath &path, const QString &address);
  void Ready();

  // added for property changes
private:
  template <typename T>
  void emitPropertyChanged(const QString &name, const T &value) const;

  // manually added functions
private:
  // void onDeviceAdded(const BleAddress &address);
  void onDeviceAdded(const QString &address);

  // manually added members
private:
  BleRcuController *const m_controller;
  const QDBusObjectPath m_dbusObjectPath;
};

#endif // BLERCUCONTROLLER1_ADAPTOR_H
