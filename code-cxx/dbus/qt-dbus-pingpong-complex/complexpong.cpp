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
#include "complexpong.h"


// property functions

QString Pong::value() const
{
  return m_value;
}

void Pong::setValue(QString const &newValue)
{
  m_value = newValue;
}

void Pong::quit()
{
  // QTimer::singleShot(0, QCoreApplication::instance(),
  //     &QCoreApplication::quit);

  QTimer::singleShot(0, this,
      &Pong::really_quit);
}

void Pong::really_quit()
{
  qDebug() << "really_quit is called";

  emit aboutToQuit();

  QTimer::singleShot(0, QCoreApplication::instance(),
      &QCoreApplication::quit);
}

QDBusVariant Pong::query(const QString &query)
{
  qDebug() << "Pong::query called";

  QString q = query.toLower();
  if (q == "hello")
    return QDBusVariant("World");
  if (q == "ping")
    return QDBusVariant("Pong");
  if (q.indexOf("the answer to life, the universe and everything") != -1)
    return QDBusVariant(42);
  if (q.indexOf("unladen swallow") != -1) {
    if (q.indexOf("european") != -1)
      return QDBusVariant(11.0);
    return QDBusVariant(QByteArray("african or european?"));
  }

  return QDBusVariant("Sorry, I don't know the answer");
}

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  // use of adaptor
  QObject obj;
  Pong *pong = new Pong(&obj);

  // NOTE
  // this is signal-to-signal connection. when ping calls remote pong's quit(),
  // it will cause appliction exit which trigger QCoreApplication::aboutToQuit.
  // this raises Pong::aboutToQuit.
  //
  // ping connect to Pong::aboutToQuit and its quit() runs.
  //
  // Adaptors are intended to be lightweight classes 
  // `whose main purpose is to relay calls to and from the real object`, 
  //
  // that is Adaptor maps incoming messages for signal/method calls to Qt slot,
  // signal, and property.
  // 

  // NOTE
  // To see "this call effectively "emit aboutToQuit", disable it and use emit
  // instead above. works well.
  //
  // QObject::connect(&app, &QCoreApplication::aboutToQuit,
  //     pong, &Pong::aboutToQuit);

  // use of property "value"
  pong->setProperty("value", "initial value");

  /*
  enum QDBusConnection::RegisterOption
  flags QDBusConnection::RegisterOptions
  Specifies the options for registering objects with the connection. The
  possible values are:

  QDBusConnection::ExportAllSlots
  ExportScriptableSlots|ExportNonScriptableSlots
  export all of this object's slots

  */

  QDBusConnection::sessionBus().registerObject("/", &obj);


  // NOTE
  // this uses adaptor but still use registerService() which is not used `chat`
  // case.

  /*
  bool QDBusConnection::registerService(const QString &serviceName)

  Attempts to register the serviceName on the D-Bus server and returns true if
  the registration succeeded. The registration will fail if the name is already
  registered by another application.

  #define SERVICE_NAME            "org.example.QtDBus.PingExample"

  NOTE that unlike `chat` case, this do not allow running the same `pong`, that
  is, not allow multiple registered with the same service name.

  */

  // #define SERVICE_NAME            "org.example.QtDBus.PingExample"
  if (!QDBusConnection::sessionBus().registerService(SERVICE_NAME)) {
    fprintf(stderr, "%s\n",
        qPrintable(QDBusConnection::sessionBus().lastError().message()));
    printf("pong cannot register service and exit\n");
    exit(1);
  }


  printf("complexpong is ready to accept a call\n");
  printf("complexpong is ready to accept a call\n");
  // fflush();

  app.exec();
  return 0;
}
