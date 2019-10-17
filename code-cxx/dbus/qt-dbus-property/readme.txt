This is a DBus example to study DBus interactions and property change
notifications.


The sender

This is a process which runs DBus adaptor and CLI which allows send signal and
property changes.


The receiver

This is a process which uses DBusInterface to use proxy and will responds to
signal and property changes from the sender.


o take one method which has string in argument

$ more org.bluez.Device1.xml
<!DOCTYPE node PUBLIC "-//freedesktop//DTD D-BUS Object Introspection 1.0//EN"
"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd">
<node>
        <interface name="org.bluez.Device1">

                <method name="ConnectProfile">
                        <arg name="UUID" type="s" direction="in"/>
                </method>

        </interface>

</node>

inline QDBusPendingReply<> ConnectProfile(const QString &UUID)
{
  QList<QVariant> argumentList;
  argumentList << QVariant::fromValue(UUID);
  return asyncCallWithArgumentList(QStringLiteral("ConnectProfile"), argumentList);
}

o use one signale from char example

o take one property from org.bluez.Adapter1.xml

  <property name="Powered" type="b" access="readwrite"/>

o generate adaptor codes

/home/keitee/Qt/5.12.3/gcc_64/bin/qdbusxml2cpp -i sender_adaptor.h -a :sender_adaptor.cpp org.example.sender.xml
/home/keitee/Qt/5.12.3/gcc_64/bin/qdbusxml2cpp -a sender_adaptor.h: org.example.sender.xml


o dbus-send runs

#define SERVICE_NAME "org.example.SenderExample"

dbus-send  --session --type=method_call --print-reply --dest='org.freedesktop.DBus' / org.freedesktop.DBus.ListNames | ag "Sender"
      string "org.example.SenderExample"


dbus-send --session --type=method_call --print-reply --dest='org.example.SenderExample' / org.freedesktop.DBus.Introspectable.Introspect

method return time=1571310650.913287 sender=:1.414 -> destination=:1.418 serial=3 reply_serial=2
   string "<!DOCTYPE node PUBLIC "-//freedesktop//DTD D-BUS Object Introspection 1.0//EN"
"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd">
<node>
  <interface name="org.example.sender">
    <method name="ConnectProfile">
      <arg direction="in" type="s" name="UUID"/>
    </method>
    <signal name="action">
      <arg direction="out" type="s" name="nickname"/>
      <arg direction="out" type="s" name="text"/>
    </signal>
    <property access="readwrite" type="b" name="Powered"/>
  </interface>
  <interface name="org.freedesktop.DBus.Properties">
    <method name="Get">
      <arg name="interface_name" type="s" direction="in"/>
      <arg name="property_name" type="s" direction="in"/>
      <arg name="value" type="v" direction="out"/>
    </method>
    <method name="Set">
      <arg name="interface_name" type="s" direction="in"/>
      <arg name="property_name" type="s" direction="in"/>
      <arg name="value" type="v" direction="in"/>
    </method>
    <method name="GetAll">
      <arg name="interface_name" type="s" direction="in"/>
      <arg name="values" type="a{sv}" direction="out"/>
      <annotation name="org.qtproject.QtDBus.QtTypeName.Out0" value="QVariantMap"/>
    </method>
    <signal name="PropertiesChanged">
      <arg name="interface_name" type="s" direction="out"/>
      <arg name="changed_properties" type="a{sv}" direction="out"/>
      <annotation name="org.qtproject.QtDBus.QtTypeName.Out1" value="QVariantMap"/>
      <arg name="invalidated_properties" type="as" direction="out"/>
    </signal>
  </interface>
  <interface name="org.freedesktop.DBus.Introspectable">
    <method name="Introspect">
      <arg name="xml_data" type="s" direction="out"/>
    </method>
  </interface>
  <interface name="org.freedesktop.DBus.Peer">
    <method name="Ping"/>
    <method name="GetMachineId">
      <arg name="machine_uuid" type="s" direction="out"/>
    </method>
  </interface>
</node>
"

dbus-send --session --type=method_call --print-reply --dest='org.example.SenderExample' / org.example.sender.ConnectProfile string:'hello'
method return time=1571314615.835578 sender=:1.414 -> destination=:1.424 serial=4 reply_serial=2

