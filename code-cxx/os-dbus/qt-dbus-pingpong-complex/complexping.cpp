#include <stdio.h>

#include <QtCore/QCoreApplication>
#include <QtDBus/QtDBus>

#include "ping-common.h"
#include "complexping.h"

// #define SERVICE_NAME            "org.example.QtDBus.PingExample"

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

  // QObject::connect()
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

      // method call time=1571906439.566168 sender=:1.888 ->
      // destination=org.example.QtDBus.PingExample serial=9 path=/;
      // interface=org.freedesktop.DBus.Properties; member=Get
      //    string "org.example.QtDBus.ComplexPong.Pong"
      //    string "value"
      // method return time=1571906439.566688 sender=:1.889 ->
      // destination=:1.888 serial=4 reply_serial=9
      //    variant       string "initial value"

      QVariant reply = iface->property("value");
      if (!reply.isNull())
        printf("value property = %s\n", qPrintable(reply.toString()));
    }
    else if (line.startsWith("value="))
    {

      // method call time=1571906585.509050 sender=:1.892 ->
      // destination=org.example.QtDBus.PingExample serial=10 path=/;
      // interface=org.freedesktop.DBus.Properties; member=Set
      //    string "org.example.QtDBus.ComplexPong.Pong"
      //    string "value"
      //    variant       string "this is message from ping"
      // method return time=1571906585.509573 sender=:1.893 ->
      // destination=:1.892 serial=5 reply_serial=10

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

  // use serviceWatcher to get notified when service(pong) is registered. And
  // runs Ping::start()

  QDBusServiceWatcher serviceWatcher(SERVICE_NAME,
      QDBusConnection::sessionBus(),
      QDBusServiceWatcher::WatchForRegistration);

  Ping ping;
  QObject::connect(&serviceWatcher, &QDBusServiceWatcher::serviceRegistered,
      &ping, &Ping::start);

  printf("run ./compleping\n");

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
