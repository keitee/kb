
#include "sender_adaptor.h"

#define SERVICE_NAME "org.example.SenderExample"

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  if (!QDBusConnection::sessionBus().isConnected())
  {
    fprintf(stderr, "Cannot connect to the D-Bus session bus.\n"
        "To start it, run:\n"
        "\teval `dbus-launch --auto-syntax`\n");
    return 1;
  }

  // as with chat example, create Adaptor and register it to dbus
  // class SenderAdaptor: public QDBusAbstractAdaptor;

  // see that:
  //
  // 1. use of adaptor
  // 2. use of parent.
  //  chat example use adaptor within CharMainWindow class so able to use:
  //  new ChatAdaptor(this);
  //
  //  ping example do not use adaptor
  //
  //  complexping uses adaptor and pass QObject as parent
  //
  // this parent is passed up to QDBusAbstractAdaptor and since not sure what
  // will happen if remove this parent connection, keep this.

  QObject obj;
  SenderAdaptor *sender = new SenderAdaptor(&obj);

  // set initial property value but no slot called since event loop has not
  // yet started
  // sender->setProperty("Powered", false);

  // see that register `obj` but not
  if (QDBusConnection::sessionBus().registerObject("/", &obj))
    qDebug() << "org.example.sender is registered";
  else
  {
    qDebug() << "org.example.sender is not registered";
    return 1;
  }

  if (!QDBusConnection::sessionBus().registerService(SERVICE_NAME))
  {
    fprintf(stderr, "%s\n",
            qPrintable(QDBusConnection::sessionBus().lastError().message()));
    printf("pong cannot register service and exit\n");
    exit(1);
  }

  qDebug() << "org.example.sender registered and started";

  app.exec();

  qDebug() << "org.example.server ended";

  return 0;
}
