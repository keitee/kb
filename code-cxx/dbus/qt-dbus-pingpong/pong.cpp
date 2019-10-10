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
#include <stdlib.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtDBus/QtDBus>

#include "ping-common.h"
#include "pong.h"


// https://doc.qt.io/qt-5/qmetaobject.html
//
// Detailed Description
//
// The Qt Meta-Object System in Qt is responsible for the signals and slots
// inter-object communication mechanism, runtime type information, and the Qt
// property system. 
//
// A single QMetaObject instance is created for each QObject subclass that is
// used in an application, and this instance stores all the meta-information for
// the QObject subclass. This object is available as QObject::metaObject().
//
// bool QMetaObject::invokeMethod(QObject *obj, const char *member,
// Qt::ConnectionType type, QGenericReturnArgument ret, QGenericArgument val0 =
// QGenericArgument(nullptr), QGenericArgument val1 = QGenericArgument(),
// QGenericArgument val2 = QGenericArgument(), QGenericArgument val3 =
// QGenericArgument(), QGenericArgument val4 = QGenericArgument(),
// QGenericArgument val5 = QGenericArgument(), QGenericArgument val6 =
// QGenericArgument(), QGenericArgument val7 = QGenericArgument(),
// QGenericArgument val8 = QGenericArgument(), QGenericArgument val9 =
// QGenericArgument())
//
// Invokes the member (a signal or a slot name) on the object obj. Returns true
// if the member could be invoked. Returns false if there is no such member or
// the parameters did not match.
//
// The invocation can be either synchronous or asynchronous, depending on type:
//
// If type is Qt::DirectConnection, the member will be invoked immediately.
//
// If type is Qt::QueuedConnection, a QEvent will be sent and the member is
// invoked as soon as the application enters the main event loop.
//
// If type is Qt::BlockingQueuedConnection, the method will be invoked in the
// same way as for Qt::QueuedConnection, except that the current thread will
// block until the event is delivered. Using this connection type to communicate
// between objects in the same thread will lead to deadlocks.

// If type is Qt::AutoConnection, the member is invoked synchronously if obj
// lives in the same thread as the caller; otherwise it will invoke the member
// asynchronously.
//
// With asynchronous method invocations, the parameters must be of types that
// are known to Qt's meta-object system, because Qt needs to copy the arguments
// to store them in an event behind the scenes. If you try to use a queued
// connection and get the error message


QString Pong::ping(const QString &arg)
{
  QMetaObject::invokeMethod(QCoreApplication::instance(), "quit");
  return QString("ping(\"%1\") got called").arg(arg);
}


// https://doc.qt.io/qt-5/qtdbus-pingpong-example.html#
//
// Demonstrates a simple message system using D-Bus.
//
// Ping Pong is a command-line example that demonstrates the basics of Qt D-Bus.
// A message is sent to another application and there is a confirmation of the
// message.
//
// PONG
// that is ping send a call and pong quit its running when it gets a call from
// ping. 
//
// ping can specify what to send from command line arg
//
// $ QDBUS_DEBUG=1 ./pong
// (lost of debug messages)
//
// $ QDBUS_DEBUG=1 ./ping "hello"
//
// Reply was: ping("hello") got called


int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  if (!QDBusConnection::sessionBus().isConnected()) {
    fprintf(stderr, "Cannot connect to the D-Bus session bus.\n"
        "To start it, run:\n"
        "\teval `dbus-launch --auto-syntax`\n");
    return 1;
  }

  /*
  bool QDBusConnection::registerService(const QString &serviceName)

  Attempts to register the serviceName on the D-Bus server and returns true if
  the registration succeeded. The registration will fail if the name is already
  registered by another application.

  #define SERVICE_NAME            "org.example.QtDBus.PingExample"

  NOTE that unlike `chat` case, this do not allow running the same `pong`, that
  is, not allow multiple registered with the same service name.

  */

  if (!QDBusConnection::sessionBus().registerService(SERVICE_NAME)) {
    fprintf(stderr, "%s\n",
        qPrintable(QDBusConnection::sessionBus().lastError().message()));
    printf("pong cannot register service and exit\n");
    exit(1);
  }

  /*
  enum QDBusConnection::RegisterOption
  flags QDBusConnection::RegisterOptions
  Specifies the options for registering objects with the connection. The
  possible values are:

  QDBusConnection::ExportAllSlots
  ExportScriptableSlots|ExportNonScriptableSlots
  export all of this object's slots

  */

  // see that can expose object without using adapter interface

  Pong pong;
  QDBusConnection::sessionBus().registerObject("/", &pong, 
      QDBusConnection::ExportAllSlots);

  printf("pong is ready to accept a call\n");
  printf("pong is ready to accept a call\n");
  // fflush();

  app.exec();
  return 0;
}
