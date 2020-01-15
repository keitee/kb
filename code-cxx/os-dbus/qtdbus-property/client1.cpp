#include "client1_object.h"

#define SERVICE_NAME "org.example.SenderExample"

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  if (!QDBusConnection::sessionBus().isConnected())
  {
    fprintf(stderr,
            "Cannot connect to the D-Bus session bus.\n"
            "To start it, run:\n"
            "\teval `dbus-launch --auto-syntax`\n");
    return 1;
  }

  // connect to the remote signal
  Client client;

#if APPROACH_1
  // QDBusInterface::QDBusInterface(
  //  const QString &service,
  //  const QString &path,
  //  const QString &interface = QString(),
  //  const QDBusConnection &connection = QDBusConnection::sessionBus(),
  //  QObject *parent = nullptr)

  auto sender = new QDBusInterface(SERVICE_NAME,
                                   "/",
                                   "org.example.sender",
                                   QDBusConnection::sessionBus());
  if (!sender->isValid())
  {
    fprintf(stderr,
            "%s\n",
            qPrintable(QDBusConnection::sessionBus().lastError().message()));
    QCoreApplication::instance()->quit();
  }

  sender->connect(sender,
                  SIGNAL(action(QString, QString)),
                  &client,
                  SLOT(onSignalReceived(QString, QString)));

  // connect sender signal to quit
  sender->connect(sender,
                  SIGNAL(aboutToQuit()),
                  QCoreApplication::instance(),
                  SLOT(quit()));
#endif

  qDebug() << "org.example.client started";

  app.exec();

  qDebug() << "org.example.client ended";

  return 0;
}
