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
#include "complexping.h"

// COMPLEX PING
//
// D-Bus Complex Ping Pong Example
// Demonstrates usage of the Qt D-Bus typesystem.

// Complex Ping Pong Example demonstrates the use of Qt D-Bus typesystem with
// QDBusVariant and QDBusReply. The example consists of the main application
// complexping which starts the other application, complexpong. Entering
// keywords such as hello and ping is handled by complexpong and the reply is
// printed to the standard output.
//
// Running the Example
//
// To run, execute the complexping application.
//
// $ ./complexping
// Ask your question: When is the next Qt release?
// Reply was: Sorry, I don't know the answer
// Ask your question: What is the answer to life, the universe and everything?
// Reply was: 42

// slot to be called by serviceWatcher.

// #define SERVICE_NAME            "org.example.QtDBus.PingExample"

void Ping::start(const QString &name)
{
  if (name != SERVICE_NAME)
    return;

  // open stdin for reading
  qstdin.open(stdin, QIODevice::ReadOnly);

  //
  iface = new QDBusInterface(SERVICE_NAME, "/", "org.example.QtDBus.ComplexPong.Pong",
      QDBusConnection::sessionBus(), this);
  if (!iface->isValid())
  {
    fprintf(stderr, "%s\n",
        qPrintable(QDBusConnection::sessionBus().lastError().message()));
    QCoreApplication::instance()->quit();
  }

  // connect to the remote signal, pong::aboutToQuit.
  // NOTE: ping do not use proxy but still able to connect to remote signal.
  //
  // Also see that proxy inherits from QDBusAbstractInterface as QDBusInterface
  // does. So proxy is not much different from QDBusInterface use.
  //
  // class OrgExampleChatInterface: public QDBusAbstractInterface

  connect(iface, SIGNAL(aboutToQuit()),
      QCoreApplication::instance(), SLOT(quit()));


  // calls pong's property and slots
  //
  // NOTE: see the use of property and method call
  //
  // o property get:
  //    QVariant reply = iface->property("value");
  //   property set:
  //    iface->setProperty("value", line.mid(6));
  // o iface->call("query", line);

  while (true)
  {
    printf("Ask your question: ");

    QString line = QString::fromLocal8Bit(qstdin.readLine()).trimmed();
    if (line.isEmpty())
    {
      iface->call("quit");
      return;
    }
    else if(line == "value")
    {
      QVariant reply = iface->property("value");
      if (!reply.isNull())
        printf("value property = %s\n", qPrintable(reply.toString()));
    }
    else if (line.startsWith("value="))
    {
      iface->setProperty("value", line.mid(6));
    }
    else
    {
      QDBusReply<QDBusVariant> reply = iface->call("query", line);
      if (reply.isValid())
        printf("Reply was: %s\n", qPrintable(reply.value().variant().toString()));
    }

    if (iface->lastError().isValid())
    {
      fprintf(stderr, "Call failed: %s\n", 
          qPrintable(iface->lastError().message()));
    }
  } // while
}

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
  */

  // use serviceWatcher to get notified when service(pong) is registered.

  QDBusServiceWatcher serviceWatcher(SERVICE_NAME,
      QDBusConnection::sessionBus(),
      QDBusServiceWatcher::WatchForRegistration);

  Ping ping;
  QObject::connect(&serviceWatcher, &QDBusServiceWatcher::serviceRegistered,
      &ping, &Ping::start);


  printf("run ./complepong\n");

  /*
  https://doc.qt.io/qt-5/qprocess.html

  Running a Process

  To start a process, pass the name and command line arguments of the program
  you want to run as arguments to start(). Arguments are supplied as individual
  strings in a QStringList.

  */

  QProcess pong;
  pong.start("./complexpong");

  app.exec();
}