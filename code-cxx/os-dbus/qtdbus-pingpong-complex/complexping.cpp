#include <stdio.h>

#include <QtCore/QCoreApplication>
#include <QtDBus/QtDBus>

#include "complexping.h"
#include "ping-common.h"

void Ping::quit()
{
  // To print message when gets `aboutToQuit` signal from `pong`, use this slot

  printf("Ping::quit(): call quit() directly\n");
  QCoreApplication::instance()->quit();
}

// slot to be called by serviceWatcher.

void Ping::start(const QString &name)
{
  if (name != SERVICE_NAME)
    return;

  // open stdin for reading
  qstdin.open(stdin, QIODevice::ReadOnly);

  // #define SERVICE_NAME            "org.example.QtDBus.PingExample"
  iface = new QDBusInterface(SERVICE_NAME,                          // service
                             "/",                                   // path
                             "org.example.QtDBus.ComplexPong.Pong", // interface
                             QDBusConnection::sessionBus(), // connection
                             this);                         // parent
  if (!iface->isValid())
  {
    fprintf(stderr,
            "%s\n",
            qPrintable(QDBusConnection::sessionBus().lastError().message()));
    QCoreApplication::instance()->quit();
  }

  // NOTE: ping do not use proxy but still able to "connect to remote signal."
  //
  // Also see that proxy inherits from QDBusAbstractInterface as QDBusInterface
  // does. So proxy is not much different from QDBusInterface use.

  // original code:
  // connect(iface, SIGNAL(aboutToQuit()),
  //     QCoreApplication::instance(), SLOT(quit()));

  // QObject::connect()
  connect(iface, SIGNAL(aboutToQuit()), this, SLOT(quit()));

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
    else if (line == "value")
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
        printf("Reply was: %s\n",
               qPrintable(reply.value().variant().toString()));
    }

    if (iface->lastError().isValid())
    {
      fprintf(stderr,
              "Call failed: %s\n",
              qPrintable(iface->lastError().message()));
    }
  } // while
}

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

  // use serviceWatcher to get notified when service(complexpong) is registered.

  QDBusServiceWatcher serviceWatcher(SERVICE_NAME,
                                     QDBusConnection::sessionBus(),
                                     QDBusServiceWatcher::WatchForRegistration);

  // connect ping and watcher and runs Ping::start() when serviceRegistered is
  // raised.
  Ping ping;
  QObject::connect(&serviceWatcher,
                   &QDBusServiceWatcher::serviceRegistered,
                   &ping,
                   &Ping::start);

  printf("launch ./complexping\n");

  /*
  https://doc.qt.io/qt-5/qprocess.html

  Running a Process

  To start a process, pass the name and command line arguments of the program
  you want to run as arguments to start(). Arguments are supplied as individual
  strings in a QStringList.

  */

  QProcess pong;
  pong.start("./complexpong");

  printf("runs event loop\n");

  app.exec();

  printf("./complexping ends\n");
}
