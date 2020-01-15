#include <QtCore/QCoreApplication>
#include <QtDBus/QtDBus>

#include "client2_object.h"

// proxy
#include "sender_interface.h"

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

  // PROXY
  //
  // bool QDBusAbstractInterface::isValid() const
  // Returns true if this is a valid reference to a remote object. It returns
  // false if there was an error during the creation of this interface (for
  // instance, if the remote application does not exist).
  //
  // Note: when dealing with remote objects, it is not always possible to
  // determine if it exists when creating a QDBusInterface.

  QSharedPointer<org::example::sender> proxy =
    QSharedPointer<org::example::sender>::create(SERVICE_NAME,
                                                 "/",
                                                 QDBusConnection::sessionBus());
  if (!proxy || !proxy->isValid())
  {
    qWarning() << "failed to create proxy";
    return 1;
  }

  ClientUseProxy client((org::example::sender *)proxy.data());

  // org::example::sender *sender;
  // sender = new org::example::sender(QString(), QString(),
  //     QDBusConnection::sessionBus());

  // connect the proxy and the receving end
  proxy->connect(proxy.data(),
                 SIGNAL(action(QString, QString)),
                 &client,
                 SLOT(onSignalReceived(QString, QString)));

  proxy->connect(proxy.data(),
                 SIGNAL(aboutToQuit()),
                 QCoreApplication::instance(),
                 SLOT(quit()));

  qDebug() << "org.example.client.use.proxy started";

  app.exec();

  qDebug() << "org.example.client.use.proxy ended";

  return 0;
}
