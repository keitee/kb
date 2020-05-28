/*
D-Bus Chat Example

copied from ~/Qt/Examples/Qt-5.12.3/dbus/chat

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

not able to do this since it has no name:
dbus-send --session --type=method_call --print-reply --dest='org.example.QtDBus.PingExample' / org.freedesktop.DBus.Introspectable.Introspect

dbus-send --session / org.example.chat.action string:'dbus:' string:'do you see?' 

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

  connect(messageLineEdit,
          SIGNAL(textChanged(QString)),
          this,
          SLOT(textChangedSlot(QString)));

  connect(sendButton, SIGNAL(clicked(bool)), this, SLOT(sendClickedSlot()));

  // for menu->file->change nick name
  connect(actionChangeNickname,
          SIGNAL(triggered(bool)),
          this,
          SLOT(changeNickname()));

  // for memu->help->about  qt
  connect(actionAboutQt, SIGNAL(triggered(bool)), this, SLOT(aboutQt()));

  connect(qApp, SIGNAL(lastWindowClosed()), this, SLOT(exiting()));

  // ADAPTOR
  // add our D-Bus interface and connect to D-Bus
  new ChatAdaptor(this);

  if (QDBusConnection::sessionBus().registerObject("/", this))
    qDebug() << "org.exampe.chat is registered";
  else
    qDebug() << "org.exampe.chat is not registered";

  // Access to remote object is done via proxy, OrgExampleChatInterface.
  //
  // Make a connection between signals exposed by proxy and slot functions in
  // this adaptor object.
  //
  // When runs this
  //
  // emit action(m_nickname, QLatin1String("joins the chat"));
  //
  // emit xxx -> adaptor signal -> dbus message -> proxy signal -> slot
  //
  // ChatMainWindow defines signals
  //    if omit this, comple error
  //
  // Proxy          defines signals
  //    `connect()` on this
  //    if omit this, compile error
  //
  // Adaptor        defines signals
  //    expose signals to dbus
  //    if omit this, no compile error but nothing happens

  // PROXY
  // OrgExampleChatInterface proxy
  org::example::chat *iface;
  iface = new org::example::chat(QString(),
                                 QString(),
                                 QDBusConnection::sessionBus(),
                                 this);

  // connect (local) slot to (remote) org.example.chat signal
  //
  // connect(iface, SIGNAL(message(QString,QString)),
  //     this, SLOT(messageSlot(QString,QString)));
  //
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
  // The arguments `service` and `path` *can be empty*, denoting a connection to
  // any signal of the (interface, name) pair, from any remote application.
  //
  // Returns true if the connection was successful.
  //
  // Warning: The signal will only be delivered to the slot if the parameters
  // match. This verification can be done only when the signal is received,
  // not at connection time.

  if (!QDBusConnection::sessionBus().connect(
        QString(), // service
        QString(), // path
        "org.example.chat",
        "message",
        this,
        SLOT(messageSlot(QString, QString))))
  {
    qDebug() << "connect message signal failed";
  }

  // Q: who's going to free `iface`? no one since done when app ends.
  connect(iface,
          SIGNAL(action(QString, QString)),
          this,
          SLOT(actionSlot(QString, QString)));

  // shows new nickname dialog
  NicknameDialog dialog;
  dialog.cancelButton->setVisible(false);
  dialog.exec();
  m_nickname = dialog.nickname->text().trimmed();

  qDebug() << "emit action()";
  emit action(m_nickname, QLatin1String("joins the chat"));
}

ChatMainWindow::~ChatMainWindow() {}

void ChatMainWindow::rebuildHistory()
{
  QString history = m_messages.join(QLatin1String("\n"));
  chatHistory->setPlainText(history);
}

// for `message` signal
void ChatMainWindow::messageSlot(const QString &nickname, const QString &text)
{
  qDebug() << QString("nickname(%1): called messageSlot()").arg(nickname);

  QString msg(QLatin1String("<%1> %2"));
  msg = msg.arg(nickname, text);
  m_messages.append(msg);

  if (m_messages.count() > 100)
    m_messages.removeFirst();
  rebuildHistory();
}

// for `action` signal
void ChatMainWindow::actionSlot(const QString &nickname, const QString &text)
{
  qDebug() << QString("nickname(%1): called actionSlot()").arg(nickname);

  QString msg(QLatin1String("* %1 %2"));
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

#define WHEN_USE_MESSAGE

void ChatMainWindow::sendClickedSlot()
{
  // NOTE:
  // shows that `emit` is equals to `sending dbus signal`

#ifdef WHEN_USE_MESSAGE

  QDBusMessage msg =
    QDBusMessage::createSignal("/", "org.example.chat", "message");

  msg << m_nickname;
  msg << messageLineEdit->text();

  // // if change arg order
  // msg << messageLineEdit->text();
  // msg << m_nickname;

  qDebug() << QString("nickname(%1): send message").arg(m_nickname);

  QDBusConnection::sessionBus().send(msg);

#else
  qDebug() QString("nickname(%1): emit message").arg(m_nickname);
  emit message(m_nickname, messageLineEdit->text());
#endif

  messageLineEdit->setText(QString());
}

void ChatMainWindow::changeNickname()
{
  NicknameDialog dialog(this);
  if (dialog.exec() == QDialog::Accepted)
  {
    QString old = m_nickname;
    m_nickname  = dialog.nickname->text().trimmed();

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

  if (!QDBusConnection::sessionBus().isConnected())
  {
    qWarning("Cannot connect to the D-Bus session bus.\n"
             "Please check your system settings and try again.\n");
    return 1;
  }

  ChatMainWindow chat;
  chat.show();

  return app.exec();
}
