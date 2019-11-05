/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/


// @brief
// shows various ways to call methods on remote dbus server(deamon) in Qt.


#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtDBus/QtDBus>

void method1()
{
  qDebug() << "==============================================================";
  qDebug() << "Method 1:";

  // https://doc.qt.io/qt-5/qdbusconnection.html
  //
  // The QDBusConnection class represents a connection to the D-Bus bus daemon. 
  //
  // QDBusConnectionInterface *QDBusConnection::interface() const
  //
  // Returns a QDBusConnectionInterface object that represents the D-Bus server
  // interface on this connection.
  // 
  //
  // https://doc.qt.io/qt-5/qdbusconnectioninterface.html
  //
  // Detailed Description
  //
  // Inherits:	QDBusAbstractInterface
  //
  // The D-Bus bus server daemon provides one special interface
  // org.freedesktop.DBus that allows clients to access certain properties of
  // the bus, such as the current list of clients connected. The
  // QDBusConnectionInterface class provides access to that interface.
  // 
  // The most common uses of this class are to register and unregister service
  // names on the bus using the registerService() and unregisterService()
  // functions, query about existing names using the isServiceRegistered(),
  // registeredServiceNames() and serviceOwner() functions, and to receive
  // notification that a client has registered or de-registered through the
  // serviceRegistered(), serviceUnregistered() and serviceOwnerChanged()
  // signals.

  QDBusReply<QStringList> reply = QDBusConnection::sessionBus().interface()->registeredServiceNames();
  if (!reply.isValid()) {
    qDebug() << "Error:" << reply.error().message();
    exit(1);
  }

  // https://doc.qt.io/qt-5/qdbusreply.html

  // foreach(variable, container)
  // This macro is used to implement Qt's foreach loop. The variable parameter
  // is a variable name or variable definition; the container parameter is a Qt
  // container whose value type corresponds to the type of the variable. See The
  // foreach Keyword for details.

  foreach (QString name, reply.value())
    qDebug() << name;
}


//    <method name="ListNames">
//      <arg direction="out" type="as"/>
//    </method>

void method2()
{
  qDebug() << "==============================================================";
  qDebug() << "Method 2:";

  QDBusConnection bus = QDBusConnection::sessionBus();

  // https://doc.qt.io/qt-5/qdbusinterface.html
  //
  // Inherits: QDBusAbstractInterface
  //
  // Detailed Description
  //
  // QDBusInterface is a *generic accessor class* that is used to place calls to
  // remote objects, connect to signals exported by remote objects and get/set
  // the value of remote properties.
  //
  // This class is useful for dynamic access to remote objects: that is, when
  // you do not have a generated code that represents the remote interface.
  //
  // NOTE:
  // Like proxy/adaptor, inherits QDBusAbstractInterface and understand Qt
  // property system and Qt.
  //
  // Calls are usually placed by using the call() function, which constructs the
  // message, sends it over the bus, waits for the reply and decodes the reply.
  //
  // Signals are connected to by using the normal QObject::connect() function.
  //
  // Finally, properties are accessed using the QObject::property() and
  // QObject::setProperty() functions.

  // QDBusInterface::QDBusInterface(
  //  const QString &service, 
  //  const QString &path,
  //  const QString &interface = QString(), 
  //  const QDBusConnection &connection = QDBusConnection::sessionBus(), 
  //  QObject *parent = nullptr)

  QDBusInterface dbus_iface("org.freedesktop.DBus", "/org/freedesktop/DBus",
      "org.freedesktop.DBus", bus);

  // https://doc.qt.io/qt-5/qdbusabstractinterface.html#call-1
  //
  // QDBusMessage QDBusAbstractInterface::call(
  //  const QString &method, 
  //  const QVariant &arg1 = QVariant(), const QVariant &arg2 = QVariant(), 
  //  const QVariant &arg3 = QVariant(), const QVariant &arg4 = QVariant(),
  //  const QVariant &arg5 = QVariant(), const QVariant &arg6 = QVariant(),
  //  const QVariant &arg7 = QVariant(), const QVariant &arg8 = QVariant())
  //
  //
  // Calls the method method on this interface and passes the parameters to this
  // function to the method.
  //
  // The parameters to call are passed on to the remote function via D-Bus as
  // input arguments. Output arguments are returned in the `QDBusMessage reply`.
  // If the reply is an error reply, lastError() will also be set to the
  // contents of the error message.
  //
  // This function can be used with up to 8 parameters, passed in arguments
  // arg1, arg2, arg3, arg4, arg5, arg6, arg7 and arg8. If you need more than 8
  // parameters or if you have a variable number of parameters to be passed, use
  // callWithArgumentList().

  // Sync or Async?
  //
  // * Since there is QDBusPendingCall QDBusAbstractInterface::asyncCall(),
  // call() is sync call
  //
  // * QDBusMessage reads:
  //
  // https://doc.qt.io/qt-5/qdbusmessage.html
  //
  // QDBusMessage QDBusMessage::createMethodCall(
  //  const QString &service, const QString &path, const QString &interface,
  //  const QString &method)
  //
  // Constructs a new DBus message representing a method call. A method call
  // always informs its destination address (service, path, interface and
  // method).
  //
  // The DBus bus allows calling a method on a given remote object without
  // specifying the destination interface, if the method name is unique.
  // However, if two interfaces on the remote object export the same method
  // name, the result is undefined (one of the two may be called or an error may
  // be returned).
  //
  // When using DBus in a peer-to-peer context (i.e., not on a bus), the service
  // parameter is optional.
  //
  // NOTE: The QDBusInterface class provides a simpler abstraction to
  // synchronous method calling.
  //
  // This function returns a QDBusMessage object that can be sent with
  // QDBusConnection::call().
  //
  //
  // SO, QDBusInterface::call() is sync call.

  // QList<QVariant> QDBusMessage::arguments() const
  //
  // Returns the list of arguments that are going to be sent or were received
  // from D-Bus.

  qDebug() << dbus_iface.call("ListNames").arguments().at(0);
}

void method2_1()
{
  qDebug() << "==============================================================";
  qDebug() << "Method 2_1:";

  // QDBusMessage 
  //  QDBusMessage::createMethodCall(const QString &service, 
  //  const QString &path, const QString &interface, const QString &method)
  //
  // Constructs a new DBus message representing a method call. A method call
  // always informs its destination address (service, path, interface and
  // method).
  // 
  // The DBus bus allows calling a method on a given remote object without
  // specifying the destination interface, if the method name is unique.
  //
  // However, if two interfaces on the remote object export the same method
  // name, the result is undefined (one of the two may be called or an error may
  // be returned).
  // 
  // When using DBus in a peer-to-peer context (i.e., not on a bus), the service
  // parameter is optional.
  // 
  // The QDBusInterface class provides a *simpler abstraction to synchronous*
  // method calling.


  // This function returns a QDBusMessage object that can be sent with
  // QDBusConnection::call().
  //
  // QDBusInterface dbus_iface("org.freedesktop.DBus", "/org/freedesktop/DBus",
  //    "org.freedesktop.DBus", bus);

  QDBusMessage request = QDBusMessage::createMethodCall("org.freedesktop.DBus", 
      "/org/freedesktop/DBus", "org.freedesktop.DBus", "ListNames");

  // QDBusConnection sessionBus();
  //
  // QDBusMessage 
  // QDBusConnection::call(const QDBusMessage &message, 
  //  QDBus::CallMode mode = QDBus::Block, int timeout = -1) const
  //
  // Sends the message over this connection and blocks, waiting for a reply, for
  // at most timeout milliseconds. 
  //
  // *This function is suitable for method calls only.* 
  //
  // It returns the reply message as its return value, which will be either of 
  // type QDBusMessage::ReplyMessage or QDBusMessage::ErrorMessage.
  // 
  // If no reply is received within timeout milliseconds, an automatic error
  // will be delivered indicating the expiration of the call. The default
  // timeout is -1, which will be replaced with an implementation-defined value
  // that is suitable for inter-process communications (generally, 25 seconds).
  // 
  // See the QDBusInterface::call() function for a more friendly way of placing
  // calls.
  // 
  // Warning: If mode is QDBus::BlockWithGui, this function will reenter the Qt
  // event loop in order to wait for the reply. During the wait, it may deliver
  // signals and other method calls to your application. Therefore, it must be
  // prepared to handle a reentrancy whenever a call is placed with call().

  // send out message
  //
  // QDBusPendingCall QDBusConnection::asyncCall(
  //  const QDBusMessage &message,
  //  int timeout = -1) const
  //
  // Sends the message over this connection and returns immediately. This
  // function is suitable for method calls only. It returns an object of type
  // QDBusPendingCall which can be used to track the status of the reply.
  //
  // If no reply is received within timeout milliseconds, an automatic error
  // will be delivered indicating the expiration of the call. The default
  // timeout is -1, which will be replaced with an implementation-defined value
  // that is suitable for inter-process communications (generally, 25 seconds).
  // This timeout is also the upper limit for waiting in
  // QDBusPendingCall::waitForFinished().
  //
  // See the QDBusInterface::asyncCall() function for a more friendly way of
  // placing calls.
  //
  // This function was introduced in Qt 4.5.

  QDBusMessage reply = QDBusConnection::sessionBus().call(request);

  // qDebug() << dbus_iface.call("ListNames").arguments().at(0);
  qDebug() << reply.arguments().at(0);
}

void method2_2()
{
  qDebug() << "==============================================================";
  qDebug() << "Method 2_2:";

  QDBusConnection bus = QDBusConnection::sessionBus();
  QDBusInterface dbus_iface("org.bluez.obex", "/",
      "org.freedesktop.DBus.Introspectable", bus);

  // qDebug() << dbus_iface.call("Introspect").arguments().at(0);
  //
  // Method 2_2:
  // QVariant(QString, "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\"\n\"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n<node><interface name=\"org.freedesktop.DBus.Introspectable\"><method name=\"Introspect\"><arg name=\"xml\" type=\"s\" direction=\"out\"/>\n</method></interface><interface name=\"org.freedesktop.DBus.ObjectManager\"><method name=\"GetManagedObjects\"><arg name=\"objects\" type=\"a{oa{sa{sv}}}\" direction=\"out\"/>\n</method><signal name=\"InterfacesAdded\"><arg name=\"object\" type=\"o\"/>\n<arg name=\"interfaces\" type=\"a{sa{sv}}\"/>\n</signal>\n<signal name=\"InterfacesRemoved\"><arg name=\"object\" type=\"o\"/>\n<arg name=\"interfaces\" type=\"as\"/>\n</signal>\n</interface><node name=\"org\"/></node>")

  // Method 2_2:
  // "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\"\n\"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n<node><interface name=\"org.freedesktop.DBus.Introspectable\"><method name=\"Introspect\"><arg name=\"xml\" type=\"s\" direction=\"out\"/>\n</method></interface><interface name=\"org.freedesktop.DBus.ObjectManager\"><method name=\"GetManagedObjects\"><arg name=\"objects\" type=\"a{oa{sa{sv}}}\" direction=\"out\"/>\n</method><signal name=\"InterfacesAdded\"><arg name=\"object\" type=\"o\"/>\n<arg name=\"interfaces\" type=\"a{sa{sv}}\"/>\n</signal>\n<signal name=\"InterfacesRemoved\"><arg name=\"object\" type=\"o\"/>\n<arg name=\"interfaces\" type=\"as\"/>\n</signal>\n</interface><node name=\"org\"/></node>"

  // "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\"\n\"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n<node><interface name=\"org.freedesktop.DBus.Introspectable\"><method name=\"Introspect\"><arg name=\"xml\" type=\"s\" direction=\"out\"/>\n</method></interface><interface name=\"org.freedesktop.DBus.ObjectManager\"><method name=\"GetManagedObjects\"><arg name=\"objects\" type=\"a{oa{sa{sv}}}\" direction=\"out\"/>\n</method><signal name=\"InterfacesAdded\"><arg name=\"object\" type=\"o\"/>\n<arg name=\"interfaces\" type=\"a{sa{sv}}\"/>\n</signal>\n<signal name=\"InterfacesRemoved\"><arg name=\"object\" type=\"o\"/>\n<arg name=\"interfaces\" type=\"as\"/>\n</signal>\n</interface><node name=\"org\"/></node>"

  // <!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\"\n\"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">
  //   \n
  // <node>
  // <interface name=\"org.freedesktop.DBus.Introspectable\">
  //   <method name=\"Introspect\">
  //     <arg name=\"xml\" type=\"s\" direction=\"out\"/>
  //     \n</method>
  //   </interface>
  //   <interface name=\"org.freedesktop.DBus.ObjectManager\">
  //     <method name=\"GetManagedObjects\">
  //       <arg name=\"objects\" type=\"a{oa{sa{sv}}}\" direction=\"out\"/>
  //       \n</method>
  //       <signal name=\"InterfacesAdded\">
  //         <arg name=\"object\" type=\"o\"/>
  //         \n<arg name=\"interfaces\" type=\"a{sa{sv}}\"/>
  //         \n</signal>
  //           \n
  //         <signal name=\"InterfacesRemoved\">
  //         <arg name=\"object\" type=\"o\"/>
  //         \n<arg name=\"interfaces\" type=\"as\"/>
  //         \n</signal>
  //           \n
  //         </interface>
  //         <node name=\"org\"/>
  //       </node>
  //        "

  QDBusReply<QString> reply = dbus_iface.call("Introspect");
  qDebug() << reply.value();
}

void method3()
{
  qDebug() << "==============================================================";
  qDebug() << "Method 3:";
  qDebug() << QDBusConnection::sessionBus().interface()->registeredServiceNames().value();
}

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  // https://doc.qt.io/qt-5/qdbusconnection.html
  //
  // As a convenience for the two most common connection types, the sessionBus()
  // and systemBus() functions return open connections to the session server
  // daemon and the system server daemon, respectively. Those connections are
  // opened when first used and are closed when the QCoreApplication destructor
  // is run.

  if (!QDBusConnection::sessionBus().isConnected()) {
    fprintf(stderr, "Cannot connect to the D-Bus session bus.\n"
        "To start it, run:\n"
        "\teval `dbus-launch --auto-syntax`\n");
    return 1;
  }

  method1();
  method2();
  method2_1();
  method3();
  method2_2();

  return 0;
}

/*
outputs:

==============================================================
Method 1:
"org.freedesktop.DBus"
":1.326"
":1.106"
"org.freedesktop.Notifications"
":1.7"
":1.327"
"org.freedesktop.network-manager-applet"
":1.8"
":1.9"
"org.Nemo"
"org.freedesktop.systemd1"
"org.fedoraproject.Config.Printing"
"org.gtk.vfs.Daemon"
"org.pulseaudio.Server"
"org.gnome.SessionManager"
"org.cinnamon.SettingsDaemon.KeybindingHandler"
":1.191"
":1.192"
"org.mpris.MediaPlayer2.rhythmbox"
"org.gnome.GConf"
":1.40"
":1.270"
"org.gnome.Terminal"
":1.63"
":1.41"
"org.a11y.Bus"
"org.gtk.vfs.UDisks2VolumeMonitor"
":1.271"
":1.194"
":1.86"
":1.42"
":1.20"
":1.195"
":1.65"
":1.43"
":1.21"
"org.gnome.Identity"
":1.44"
"org.gnome.keyring"
":1.22"
":1.197"
":1.45"
":1.23"
":1.68"
":1.46"
"org.NemoDesktop"
":1.24"
":1.69"
":1.47"
"org.gnome.Caribou.Keyboard"
":1.25"
":1.48"
"org.Cinnamon"
":1.26"
":1.157"
":1.49"
"org.cinnamon.SettingsDaemon.XRANDR_2"
":1.27"
":1.334"
":1.158"
":1.28"
"org.bluez.obex"
"ca.desrt.dconf"
":1.29"
"org.gtk.vfs.GPhoto2VolumeMonitor"
":1.116"
"org.gnome.SettingsDaemon"
":1.117"
"org.freedesktop.ScreenSaver"
"org.gtk.vfs.mountpoint_2069"
"org.cinnamon.SettingsDaemon.Power.Keyboard"
"org.gnome.Magnifier"
"org.gtk.vfs.AfcVolumeMonitor"
"org.freedesktop.FileManager1"
"org.cinnamon.SettingsDaemon.Power.Screen"
"org.PulseAudio1"
"org.gnome.UPnP.MediaServer2.Rhythmbox"
"org.gtk.vfs.mountpoint_http"
"org.Cinnamon.LookingGlass"
"org.gnome.Panel"
"org.gtk.vfs.GoaVolumeMonitor"
"org.x.reader.Daemon"
":1.91"
":1.70"
"org.cinnamon.SettingsDaemon.Power"
"org.gnome.OnlineAccounts"
":1.72"
":1.50"
"org.gtk.vfs.mountpoint_10489"
":1.73"
":1.51"
":1.96"
"org.gtk.vfs.Metadata"
":1.30"
":1.53"
":1.31"
"org.gnome.Rhythmbox3"
"org.cinnamon.ScreenSaver"
":1.54"
":1.10"
":1.32"
"org.gtk.vfs.mountpoint_dnssd"
":1.55"
":1.11"
":1.33"
":1.12"
":1.34"
":1.0"
":1.13"
":1.35"
":1.58"
":1.36"
":1.101"
"org.cinnamon.SettingsDaemon.Sound"
":1.37"
"org.freedesktop.secrets"
":1.15"
":1.102"
":1.38"
":1.16"
"org.gtk.vfs.MTPVolumeMonitor"
":1.147"
":1.39"
":1.17"
":1.324"
":1.148"
":1.5"
":1.18"
":1.325"
":1.6"
":1.19"
==============================================================
Method 2:
QVariant(QStringList, ("org.freedesktop.DBus", ":1.326", ":1.106", "org.freedesktop.Notifications", ":1.7", ":1.327", "org.freedesktop.network-manager-applet", ":1.8", ":1.9", "org.Nemo", "org.freedesktop.systemd1", "org.fedoraproject.Config.Printing", "org.gtk.vfs.Daemon", "org.pulseaudio.Server", "org.gnome.SessionManager", "org.cinnamon.SettingsDaemon.KeybindingHandler", ":1.191", ":1.192", "org.mpris.MediaPlayer2.rhythmbox", "org.gnome.GConf", ":1.40", ":1.270", "org.gnome.Terminal", ":1.63", ":1.41", "org.a11y.Bus", "org.gtk.vfs.UDisks2VolumeMonitor", ":1.271", ":1.194", ":1.86", ":1.42", ":1.20", ":1.195", ":1.65", ":1.43", ":1.21", "org.gnome.Identity", ":1.44", "org.gnome.keyring", ":1.22", ":1.197", ":1.45", ":1.23", ":1.68", ":1.46", "org.NemoDesktop", ":1.24", ":1.69", ":1.47", "org.gnome.Caribou.Keyboard", ":1.25", ":1.48", "org.Cinnamon", ":1.26", ":1.157", ":1.49", "org.cinnamon.SettingsDaemon.XRANDR_2", ":1.27", ":1.334", ":1.158", ":1.28", "org.bluez.obex", "ca.desrt.dconf", ":1.29", "org.gtk.vfs.GPhoto2VolumeMonitor", ":1.116", "org.gnome.SettingsDaemon", ":1.117", "org.freedesktop.ScreenSaver", "org.gtk.vfs.mountpoint_2069", "org.cinnamon.SettingsDaemon.Power.Keyboard", "org.gnome.Magnifier", "org.gtk.vfs.AfcVolumeMonitor", "org.freedesktop.FileManager1", "org.cinnamon.SettingsDaemon.Power.Screen", "org.PulseAudio1", "org.gnome.UPnP.MediaServer2.Rhythmbox", "org.gtk.vfs.mountpoint_http", "org.Cinnamon.LookingGlass", "org.gnome.Panel", "org.gtk.vfs.GoaVolumeMonitor", "org.x.reader.Daemon", ":1.91", ":1.70", "org.cinnamon.SettingsDaemon.Power", "org.gnome.OnlineAccounts", ":1.72", ":1.50", "org.gtk.vfs.mountpoint_10489", ":1.73", ":1.51", ":1.96", "org.gtk.vfs.Metadata", ":1.30", ":1.53", ":1.31", "org.gnome.Rhythmbox3", "org.cinnamon.ScreenSaver", ":1.54", ":1.10", ":1.32", "org.gtk.vfs.mountpoint_dnssd", ":1.55", ":1.11", ":1.33", ":1.12", ":1.34", ":1.0", ":1.13", ":1.35", ":1.58", ":1.36", ":1.101", "org.cinnamon.SettingsDaemon.Sound", ":1.37", "org.freedesktop.secrets", ":1.15", ":1.102", ":1.38", ":1.16", "org.gtk.vfs.MTPVolumeMonitor", ":1.147", ":1.39", ":1.17", ":1.324", ":1.148", ":1.5", ":1.18", ":1.325", ":1.6", ":1.19"))
==============================================================
Method 3:
("org.freedesktop.DBus", ":1.326", ":1.106", "org.freedesktop.Notifications", ":1.7", ":1.327", "org.freedesktop.network-manager-applet", ":1.8", ":1.9", "org.Nemo", "org.freedesktop.systemd1", "org.fedoraproject.Config.Printing", "org.gtk.vfs.Daemon", "org.pulseaudio.Server", "org.gnome.SessionManager", "org.cinnamon.SettingsDaemon.KeybindingHandler", ":1.191", ":1.192", "org.mpris.MediaPlayer2.rhythmbox", "org.gnome.GConf", ":1.40", ":1.270", "org.gnome.Terminal", ":1.63", ":1.41", "org.a11y.Bus", "org.gtk.vfs.UDisks2VolumeMonitor", ":1.271", ":1.194", ":1.86", ":1.42", ":1.20", ":1.195", ":1.65", ":1.43", ":1.21", "org.gnome.Identity", ":1.44", "org.gnome.keyring", ":1.22", ":1.197", ":1.45", ":1.23", ":1.68", ":1.46", "org.NemoDesktop", ":1.24", ":1.69", ":1.47", "org.gnome.Caribou.Keyboard", ":1.25", ":1.48", "org.Cinnamon", ":1.26", ":1.157", ":1.49", "org.cinnamon.SettingsDaemon.XRANDR_2", ":1.27", ":1.158", ":1.28", ":1.335", "org.bluez.obex", "ca.desrt.dconf", ":1.29", "org.gtk.vfs.GPhoto2VolumeMonitor", ":1.336", ":1.116", "org.gnome.SettingsDaemon", ":1.117", "org.freedesktop.ScreenSaver", ":1.339", "org.gtk.vfs.mountpoint_2069", "org.cinnamon.SettingsDaemon.Power.Keyboard", "org.gnome.Magnifier", "org.gtk.vfs.AfcVolumeMonitor", "org.freedesktop.FileManager1", "org.cinnamon.SettingsDaemon.Power.Screen", "org.PulseAudio1", "org.gnome.UPnP.MediaServer2.Rhythmbox", "org.gtk.vfs.mountpoint_http", "org.Cinnamon.LookingGlass", "org.gnome.Panel", "org.gtk.vfs.GoaVolumeMonitor", "org.x.reader.Daemon", ":1.91", ":1.70", "org.cinnamon.SettingsDaemon.Power", "org.gnome.OnlineAccounts", ":1.72", ":1.50", "org.gtk.vfs.mountpoint_10489", ":1.73", ":1.51", ":1.96", "org.gtk.vfs.Metadata", ":1.30", ":1.53", ":1.31", "org.gnome.Rhythmbox3", "org.cinnamon.ScreenSaver", ":1.54", ":1.10", ":1.32", "org.gtk.vfs.mountpoint_dnssd", ":1.55", ":1.11", ":1.33", ":1.12", ":1.34", ":1.0", ":1.13", ":1.35", ":1.58", ":1.36", ":1.101", "org.cinnamon.SettingsDaemon.Sound", ":1.37", "org.freedesktop.secrets", ":1.15", ":1.102", ":1.38", ":1.16", "org.gtk.vfs.MTPVolumeMonitor", ":1.147", ":1.39", ":1.17", ":1.324", ":1.148", ":1.5", ":1.18", ":1.325", ":1.6", ":1.19")

*/
