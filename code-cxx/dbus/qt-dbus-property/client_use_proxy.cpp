#include <QtCore/QCoreApplication>
#include <QtDBus/QtDBus>

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

  // proxy
  org::example::sender *sender;
  sender = new org::example::sender(QString(), QString(), 
      QDBusConnection::sessionBus());

  qDebug() << "org.example.client.use.proxy started";

  app.exec();

  qDebug() << "org.example.client.use.proxy ended";

  return 0;
}
