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

#include <stdio.h>

#include <QtCore/QCoreApplication>
#include <QtDBus/QtDBus>

#include "ping-common.h"

/*

class ChatAdaptor: public QDBusAbstractAdaptor;

https://doc.qt.io/qt-5/qdbusinterface.html

Detailed Description

QDBusInterface is a generic accessor class that is used *to place calls* to
remote objects, connect to signals exported by remote objects and get/set the
value of remote properties. This class is useful for dynamic access to remote
objects: that is, when you do not have a generated code that represents the
remote interface.

Calls are usually placed by using the call() function, which constructs the
message, sends it over the bus, waits for the reply and decodes the reply.

Signals are connected to by using the normal QObject::connect() function.

Finally, properties are accessed using the QObject::property() and
QObject::setProperty() functions.


https://doc.qt.io/qt-5/qdbusabstractinterface.html#call

QDBusMessage QDBusAbstractInterface::call(
  const QString &method, const QVariant &arg1 = QVariant(), const QVariant &arg2
  = QVariant(), const QVariant &arg3 = QVariant(), const QVariant &arg4 =
  QVariant(), const QVariant &arg5 = QVariant(), const QVariant &arg6 =
  QVariant(), const QVariant &arg7 = QVariant(), const QVariant &arg8 =
  QVariant());

Calls the method method on this interface and passes the parameters to this
function to the method.

The parameters to call are passed on to the remote function via D-Bus as input
arguments. Output arguments are returned in the QDBusMessage reply. If the reply
is an error reply, lastError() will also be set to the contents of the error
message.


https://doc.qt.io/qt-5/qdbusreply.html

Detailed Description

A QDBusReply object is a *subset* of the QDBusMessage object that represents a
method call's reply. It contains only the first output argument or the error
code and is used by QDBusInterface-derived classes to allow returning the error
code as the function's return argument.
 
However, if it does fail under those conditions, the value returned by
QDBusReply<T>::value() is a default-constructed value. It may be
indistinguishable from a valid return value.


NOTE: 
input argument = in parameter
output argument = out parameter = return value


QDBusReply objects are used for remote calls that have no output arguments or
return values (i.e., they have a "void" return type). 

Use the isValid() function to test if the reply succeeded.


QDBusReply::Type QDBusReply::value() const

Returns the remote function's calls return value. If the remote call returned
with an error, the return value of this function is undefined and may be
undistinguishable from a valid return value.

This function is not available if the remote call returns void.

*/

// PING


int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  if (!QDBusConnection::sessionBus().isConnected()) {
    fprintf(stderr, "Cannot connect to the D-Bus session bus.\n"
        "To start it, run:\n"
        "\teval `dbus-launch --auto-syntax`\n");
    return 1;
  }


  // Make a *direct* call(slot) of the remote object, instead of using
  // signal/slot/connect way.

  QDBusInterface iface(SERVICE_NAME, "/", "", QDBusConnection::sessionBus());
  if (iface.isValid()) {
    QDBusReply<QString> reply = iface.call("ping", argc > 1 ? argv[1] : "");
    if (reply.isValid()) {
      printf("Reply was: %s\n", qPrintable(reply.value()));
      return 0;
    }

    fprintf(stderr, "Call failed: %s\n", qPrintable(reply.error().message()));
    return 1;
  }

  fprintf(stderr, "%s\n",
      qPrintable(QDBusConnection::sessionBus().lastError().message()));
  return 1;
}
