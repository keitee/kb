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
  connect(this, SIGNAL(powerChanged(bool)), this, SLOT(onPowerChanged(bool)));
}

SenderAdaptor::~SenderAdaptor()
{
  // destructor
}

/*

Q_PROPERTY(bool Powered READ powered WRITE setPowered NOTIFY powerChanged)

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

*/

bool SenderAdaptor::powered() const
{
  // get the value of property Powered
  qDebug() << "SenderAdaptor::powered(" << m_powered << ") called";
  // return qvariant_cast<bool>(parent()->property("Powered"));
  return m_powered;
}

void SenderAdaptor::setPowered(bool value)
{
  qDebug() << "SenderAdaptor::setPowered(" << m_powered << ") called";
  m_powered = value;
  emit powerChanged(value);
}

// A NOTIFY signal is optional. If defined, it should specify one existing
// signal in that class that is emitted whenever the value of the property
// changes.

void SenderAdaptor::onPowerChanged(bool power)
{
  qDebug() << "got powerChanged signal (" << power << ")";
}

/*
WHY two works fine??

void SenderAdaptor::ConnectProfile(const QString &UUID);
is from qdbusxml2cpp

void SenderAdaptor::ConnectProfile(const QString &UUID, const QDBusMessage
&message); is from the case.

https://doc.qt.io/qt-5/qdbusdeclaringslots.html

Declaring Slots in D-Bus Adaptors

Slots in D-Bus adaptors are declared just like normal, public slots, but their
parameters must follow certain rules (see The Qt D-Bus Type System for more
information). Slots whose parameters do not follow those rules or that are not
public will not be accessible via D-Bus.

Slots can have one parameter of type const QDBusMessage &, which must appear at
`the end` of the input parameter list, before any output parameters. This
parameter, if present, will be initialized `with a copy of the current message`
being processed, which allows the callee to obtain information about the caller,
such as its connection name.

Slots can be of three kinds:

Asynchronous
Input-only
Input-and-output


Asynchronous Slots

Asynchronous slots are those that do not normally return any reply to the
caller. For that reason, they cannot take any output parameters. In most cases,
by the time the first line of the slot is run, the caller function has already
resumed working.

However, slots must not rely on that behavior. Scheduling and
message-dispatching issues could change the order in which the slot is run. Code
intending to synchronize with the caller should provide its own method of
synchronization.

Asynchronous slots are marked by the keyword Q_NOREPLY in the method signature,
before the void return type and the slot name. The quit() slot in the D-Bus
Complex Ping Pong Example is an example of this.

Input-Only Slots

Input-only slots are normal slots that take parameters passed by value or by
constant reference. However, unlike asynchronous slots, the caller is usually
waiting for completion of the callee before resuming operation. Therefore,
non-asynchronous slots should not block or should state it its documentation
that they may do so.

Input-only slots have no special marking in their signature, except that they
take only parameters passed by value or by constant reference. Optionally, slots
can take a QDBusMessage parameter as a last parameter, which can be used to
perform additional analysis of the method call message.

Input and Output Slots

Like input-only slots, input-and-output slots are those that the caller is
waiting for a reply. Unlike input-only ones, though, this reply will contain
data. Slots that output data may contain non-constant references and may return
a value as well. However, the output parameters must all appear at the end of
the argument list and may not have input arguments interleaved. Optionally, a
QDBusMessage argument may appear between the input and the output arguments.

Automatic Replies

`Method replies are generated automatically` with the contents of the output
parameters (if there were any) by the Qt D-Bus implementation. Slots need not
worry about constructing proper QDBusMessage objects and sending them over the
connection.

However, the possibility of doing so remains there. Should the slot find out it
needs to send a special reply or even an error, it can do so by using
QDBusMessage::createReply() or QDBusMessage::createErrorReply() on the
QDBusMessage parameter and send it with QDBusConnection::send(). The Qt D-Bus
implementation will not generate any reply if the slot did so.

Warning: When a caller places a method call and waits for a reply, it will only
wait for a limited amount of time. Slots intending to take a long time to
complete should make that fact clear in documentation so that callers properly
set higher timeouts.

Delayed Replies

In some circumstances, the called slot may not be able to process the request
immediately. This is frequently the case when the request involves an I/O or
networking operation which may block.

If this is the case, the slot should return control to the application's main
loop to avoid freezing the user interface, and resume the process later. To
accomplish this, it should make use of the extra QDBusMessage parameter at the
end of the input parameter list and request a delayed reply.

We do this by writing a slot that stores the request data in a persistent
structure, `indicating to the caller` using QDBusMessage::setDelayedReply(true)
  that the response will be sent later.


bool QDBusConnection::send(const QDBusMessage &message) const

Sends the message over this connection, `without waiting for a reply.` This is
suitable for errors, signals, and return values as well as calls whose return
values are not necessary.

Returns true if the message was queued successfully, false otherwise.


struct RequestData
{
    QString request;
    QString processedData;
    QDBusMessage reply;
};

QString processRequest(const QString &request, const QDBusMessage &message)
{
    RequestData *data = new RequestData;
    data->request = request;

    message.setDelayedReply(true);
    data->reply = message.createReply();
    QDBusConnection::sessionBus().send(data->reply);

    appendRequest(data);
    return QString();
}

The use of QDBusConnection::sessionBus().send(data->reply) is needed to
explicitly inform the caller that the response will be delayed.

In this case, the return value is unimportant; we return an arbitrary value to
satisfy the compiler.

"When the request is processed and a reply is available", it should be sent
using the QDBusMessage object that was obtained. In our example, the reply code
could be something as follows:

void sendReply(RequestData *data)
{
    // data->processedData has been initialized with the request's reply
    QDBusMessage &reply = &data->reply;

    // send the reply over D-Bus:
    reply << data->processedData;
    QDBusConnection::sessionBus().send(reply);

    // dispose of the transaction data
    delete data;
}

As can be seen in the example, when a delayed reply is in place, the return
value(s) from the slot `will be ignored by Qt D-Bus.` They are used only to
determine the slot's signature when communicating the adaptor's description to
remote applications, or in case the code in the slot decides not to use a
delayed reply.

The delayed reply itself is requested from Qt D-Bus by calling
QDBusMessage::reply() on the original message. It then becomes the resposibility
of the called code to eventually send a reply to the caller.

Warning: When a caller places a method call and waits for a reply, it will only
wait for a limited amount of time. Slots intending to take a long time to
complete should make that fact clear in documentation so that callers properly
set higher timeouts.

See also Using Qt D-Bus Adaptors, Declaring Signals in D-Bus Adaptors, The Qt
D-Bus Type System, QDBusConnection, and QDBusMessage.

*/

// As with ping example, QString Pong::ping(const QString &arg), use this slot
// to ends the sender

// to quit
void SenderAdaptor::ConnectProfile(const QString &UUID,
                                   const QDBusMessage &message)
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

// void SenderAdaptor::SendCommand(const QString &command)
void SenderAdaptor::SendCommand(const QString &command,
                                const QDBusMessage &message)
{
  qDebug() << QString("sender::SendCommand(%1) got called").arg(command);
  qDebug() << "message.service: " << message.service();
  qDebug() << "message.path   : " << message.path();
  qDebug() << "message.member : " << message.member();

  if (command == "command")
  {
    QTimer::singleShot(5000, this, SLOT(onTimerExpired()));

    // set property value and `powerChanged` is emitted
    setProperty("Powered", true);
  }
  else if (command == "error")
  {
    message.setDelayedReply(true);

    // QDBusMessage QDBusMessage::createErrorReply(const QString name, const
    // QString &msg) const
    //
    // Constructs a new DBus message representing an error reply message, with
    // the given name and msg.
    QDBusMessage error = message.createErrorReply(
      QString("SenderError"),
      QString("this is the error message from sender"));

    qDebug() << error;

    // bool QDBusConnection::send(const QDBusMessage &message) const
    //
    // Sends the message over this connection, without waiting for a reply. This
    // is suitable for errors, signals, and return values as well as calls whose
    // return values are not necessary.
    // Returns true if the message was queued successfully, false otherwise.

    if (!QDBusConnection::sessionBus().send(error))
      qDebug() << "failed to send error reply";
    else
      qDebug() << "succeeded to send error reply";
  }
  else
  {
    qDebug() << "calls setProperty and notifyPropertyChanged()";
    notifyPropertyChanged();
  }
}

void SenderAdaptor::onTimerExpired()
{
  qDebug() << "timer expired and emit action signal";
  emit action("from sender", "do you hear me?");

  qDebug() << "timer expired and change property";
  setProperty("Powered", false);
}

/*
https://doc.qt.io/qt-5/qdbusmessage.html#createSignal

QDBusMessage QDBusMessage::createSignal(
 const QString &path,
 const QString &interface,
 const QString &name)

QDBusMessage &QDBusMessage::operator<<(const QVariant &arg)

Appends the argument arg to the list of arguments to be sent over D-Bus in a
method call or signal emission.

*/

void SenderAdaptor::notifyPropertyChanged()
{
  QDBusMessage signal =
    QDBusMessage::createSignal("/",
                               "org.freedesktop.DBus.Properties",
                               "PropertiesChanged");

  signal << "org.example.sender";
  QVariantMap changedProps;

  // `QVariant`` QObject::property(const char *name) const
  // Returns the value of the object's name property.
  changedProps.insert("Powered", property("Powered"));

  signal << changedProps;
  signal << QStringList();

  QDBusConnection::sessionBus().send(signal);
}