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

/*
D-Bus Chat Example 

Demonstrates communication among instances of an application.

Chat is a Qt D-Bus example which demonstrates a simple chat system among
instances of an application. Users connect and send message to broadcast.

  instance #1   
    |                    |
   \|/ register(expose) \|/ message(call)

      instance #2   
       | register(expose) | message(call)

          instance #3   
           | register(expose) | message(call)

  <-------------- dbus ------------->

so when only instance #1 exist, this gets input message. when there are many
instances, all gets the same input message when one of them sends signal.

$ dbus-send --session / org.example.chat.action string:'dbus:' string:'do you see?'

*/


#include <QApplication>
#include <QMessageBox>

#include "chat.h"
#include "chat_adaptor.h"
#include "chat_interface.h"

ChatMainWindow::ChatMainWindow()
    : m_nickname(QLatin1String("nickname"))
{
    setupUi(this);
    sendButton->setEnabled(false);

    connect(messageLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(textChangedSlot(QString)));
    connect(sendButton, SIGNAL(clicked(bool)), this, 
        SLOT(sendClickedSlot()));
    connect(actionChangeNickname, SIGNAL(triggered(bool)), 
        this, SLOT(changeNickname()));
    connect(actionAboutQt, SIGNAL(triggered(bool)), 
        this, SLOT(aboutQt()));
    connect(qApp, SIGNAL(lastWindowClosed()), this, 
        SLOT(exiting()));

    // add our D-Bus interface and connect to D-Bus
    new ChatAdaptor(this);
    if(QDBusConnection::sessionBus().registerObject("/", this))
      qDebug() << "org.exampe.chat is registered";
    else
      qDebug() << "org.exampe.chat is not registered";

    // Access to remote object is done via proxy, OrgExampleChatInterface.
    //
    // Make a connection between signals exposed by proxy and slot functoins in
    // this adaptor object.
    //
    // When runs this
    //
    // emit action(m_nickname, QLatin1String("joins the chat"));
    //
    // this is send a signal, which is atuclly a dbus call to proxy and this is
    // relayed to the connected slot function exposed by adaptor.
    //
    // that is `emit xxx` made a call to itself since adaptor is from the same
    // object.
    // ping do not use proxy but still able to connect to remote signal.
    // emit xxx -> proxy signal -> adaptor slot
    //                              signal:
    //                               void action(...);
    //                               void message(...);
    //
    // Interesting thing is that `emit xxx` makes a call to the proxy although
    // do not specify it?

    // OrgExampleChatInterface proxy
    org::example::chat *iface;
    iface = new org::example::chat(QString(), QString(), 
        QDBusConnection::sessionBus(), this);

    // // maps org.example.chat signal to slots
    // connect(iface, SIGNAL(message(QString,QString)), 
    //     this, SLOT(messageSlot(QString,QString)));

    // shorter than this:
    //
    // bool QDBusConnection::connect(
    //  const QString &service, 
    //  const QString &path, 
    //  const QString &interface, 
    //  const QString &name, 
    //  QObject *receiver, 
    //  const char *slot)
    //
    // Connects the signal specified by the service, path, interface and name
    // parameters to the slot `slot` in object `receiver`. 
    //
    // The arguments service and path *can be empty*, denoting a connection to any
    // signal of the (interface, name) pair, from any remote application.
    //
    // Returns true if the connection was successful.
    // 
    // Warning: The signal will only be delivered to the slot if the parameters
    // match. This verification can be done only when the signal is received,
    // not at connection time.

    if (!QDBusConnection::sessionBus().connect(QString(), QString(),
     "org.example.chat", "message", this,
     SLOT(messageSlot(QString,QString))))
    {
      qDebug() << "connect message signal failed";
    }

    // this shows error when comment out signals on the proxy code.
    connect(iface, SIGNAL(action(QString,QString)), 
        this, SLOT(actionSlot(QString,QString)));

    // shows new nickname dialog
    NicknameDialog dialog;
    dialog.cancelButton->setVisible(false);
    dialog.exec();
    m_nickname = dialog.nickname->text().trimmed();
    
    // send Qt signal but not dbus signal via ChatMainWindow's signal since
    // it comment out ChatMainWindow's signals then gets compile error and
    // comment out ChatAdaptor's signals then no compile error but nothing
    // happens.
    qDebug() << "emit action()";
    emit action(m_nickname, QLatin1String("joins the chat"));
}

ChatMainWindow::~ChatMainWindow()
{
}

void ChatMainWindow::rebuildHistory()
{
    QString history = m_messages.join( QLatin1String("\n" ) );
    chatHistory->setPlainText(history);
}

void ChatMainWindow::messageSlot(const QString &nickname, const QString &text)
{
    QString msg( QLatin1String("<%1> %2") );
    msg = msg.arg(nickname, text);
    m_messages.append(msg);

    if (m_messages.count() > 100)
        m_messages.removeFirst();
    rebuildHistory();
}

void ChatMainWindow::actionSlot(const QString &nickname, const QString &text)
{
    QString msg( QLatin1String("* %1 %2") );
    msg = msg.arg(nickname, text);
    m_messages.append(msg);

    if (m_messages.count() > 100)
        m_messages.removeFirst();
    rebuildHistory();
}

void ChatMainWindow::textChangedSlot(const QString &newText)
{
    sendButton->setEnabled(!newText.isEmpty());
}

void ChatMainWindow::sendClickedSlot()
{
  // send dbus signal
  // emit message(m_nickname, messageLineEdit->text());
  //
  // do the same as:
  //
  // QDBusMessage msg = QDBusMessage::createSignal("/", "org.example.chat", "message");
  // msg << m_nickname << messageLineEdit->text();
  // QDBusConnection::sessionBus().send(msg);

  emit message(m_nickname, messageLineEdit->text());

  messageLineEdit->setText(QString());
}

void ChatMainWindow::changeNickname()
{
    NicknameDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString old = m_nickname;
        m_nickname = dialog.nickname->text().trimmed();

        // send dbus signal
        emit action(old, QString("is now known as %1").arg(m_nickname));
    }
}

void ChatMainWindow::aboutQt()
{
    QMessageBox::aboutQt(this);
}

void ChatMainWindow::exiting()
{
    // send dbus signal
    emit action(m_nickname, QLatin1String("leaves the chat"));
}

NicknameDialog::NicknameDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    if (!QDBusConnection::sessionBus().isConnected()) {
        qWarning("Cannot connect to the D-Bus session bus.\n"
                 "Please check your system settings and try again.\n");
        return 1;
    }

    ChatMainWindow chat;
    chat.show();

    return app.exec();
}
