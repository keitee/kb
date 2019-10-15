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
