#include <QtCore/QCoreApplication>
#include <QtDBus/QtDBus>

#include "client_use_proxy_object.h"

// proxy
#include "sender_interface.h"

// #define SERVICE_NAME "org.example.SenderExample"

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  if (!QDBusConnection::sessionBus().isConnected()) {
    fprintf(stderr, "Cannot connect to the D-Bus session bus.\n"
        "To start it, run:\n"
        "\teval `dbus-launch --auto-syntax`\n");
    return 1;
  }

  ClientUseProxy client;

  // proxy
  org::example::sender *sender;
  sender = new org::example::sender(QString(), QString(), 
      QDBusConnection::sessionBus());

  // QDBusInterface::connect()
  sender->connect(sender, SIGNAL(action(QString, QString)),
      &client, SLOT(onSignalReceived(QString, QString)));

  // connect sender signal to quit
  sender->connect(sender, SIGNAL(aboutToQuit()),
      QCoreApplication::instance(), SLOT(quit()));

  qDebug() << "org.example.client.use.proxy started";

  app.exec();

  qDebug() << "org.example.client.use.proxy ended";

  return 0;
}
