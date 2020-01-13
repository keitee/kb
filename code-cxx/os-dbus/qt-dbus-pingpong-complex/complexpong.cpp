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


#include <stdio.h>
#include <stdlib.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtDBus/QtDBus>

#include "ping-common.h"
#include "complexpong.h"


// property functions

QString Pong::value() const
{
  qDebug() << "pong::value returns" << m_value;
  return m_value;
}

void Pong::setValue(QString const &newValue)
{
  qDebug() << "pong::set value " << newValue;
  m_value = newValue;
}

void Pong::quit()
{
  // As a special case, a QTimer with a timeout of 0 will time out as soon as
  // all the events in the window system's event queue have been processed. This
  // can be used to do heavy work while providing a snappy user interface:

  // QTimer::singleShot(0, QCoreApplication::instance(),
  //     &QCoreApplication::quit);

  QTimer::singleShot(0, this, &Pong::really_quit);
}

void Pong::really_quit()
{
  qDebug() << "really_quit is called";

  emit aboutToQuit();

  QTimer::singleShot(0, QCoreApplication::instance(),
      &QCoreApplication::quit);
}

QDBusVariant Pong::query(const QString &query)
{
  qDebug() << "Pong::query called";

  QString q = query.toLower();
  if (q == "hello")
    return QDBusVariant("World");
  if (q == "ping")
    return QDBusVariant("Pong");
  if (q.indexOf("the answer to life, the universe and everything") != -1)
    return QDBusVariant(42);
  if (q.indexOf("unladen swallow") != -1) {
    if (q.indexOf("european") != -1)
      return QDBusVariant(11.0);
    return QDBusVariant(QByteArray("african or european?"));
  }

  return QDBusVariant("Sorry, I don't know the answer");
}

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  // see that use of adaptor
  QObject obj;
  Pong *pong = new Pong(&obj);

  // NOTE
  // QObject::connect(&app, &QCoreApplication::aboutToQuit,
  //     pong, &Pong::aboutToQuit);
  //
  // this is signal-to-signal connection. when ping calls remote pong's quit(),
  // it will cause appliction exit which trigger QCoreApplication::aboutToQuit.
  // this is mapped and it leads to raises Pong::aboutToQuit.
  //
  // `ping` connect to this Pong::aboutToQuit and its quit() runs.
  //
  // Adaptors are intended to be lightweight classes
  // `whose main purpose is to relay calls to and from the real object`,
  //
  // that is Adaptor maps incoming messages for signal/method calls to Qt slot,
  // signal, and property.
  //
  // To see this connect() effectively do "emit aboutToQuit", disable it and use
  // emit instead above. works well.
  //
  // that is, "send a signal", "emit a signal" means a "send a signal message on
  // dbus" after all.

  // use of property "value"
  pong->setProperty("value", "initial value");

  /*
  enum QDBusConnection::RegisterOption
  flags QDBusConnection::RegisterOptions
  Specifies the options for registering objects with the connection. The
  possible values are:

  QDBusConnection::ExportAllSlots
  ExportScriptableSlots|ExportNonScriptableSlots
  export all of this object's slots

  */

  QDBusConnection::sessionBus().registerObject("/", &obj);


  // NOTE
  // this uses adaptor but use registerService() which is not used `chat`
  // example

  /*
  bool QDBusConnection::registerService(const QString &serviceName)

  Attempts to register the serviceName on the D-Bus server and returns true if
  the registration succeeded. The registration will fail if the name is already
  registered by another application.

  #define SERVICE_NAME            "org.example.QtDBus.PingExample"

  NOTE that unlike `chat` case, this do not allow running the same `pong`, that
  is, not allow multiple registered with the same service name.

  */

  // #define SERVICE_NAME            "org.example.QtDBus.PingExample"
  if (!QDBusConnection::sessionBus().registerService(SERVICE_NAME)) {
    fprintf(stderr, "%s\n",
        qPrintable(QDBusConnection::sessionBus().lastError().message()));
    printf("pong cannot register service and exit\n");
    exit(1);
  }


  printf("complexpong is ready to accept a call\n");
  printf("complexpong is ready to accept a call\n");
  // fflush();

  app.exec();
  return 0;
}
