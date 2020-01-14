#include "client1_object.h"

#define SERVICE_NAME "org.example.SenderExample"

Client::Client()
{
  // QDBusInterface::QDBusInterface(
  //  const QString &service,
  //  const QString &path,
  //  const QString &interface = QString(),
  //  const QDBusConnection &connection = QDBusConnection::sessionBus(),
  //  QObject *parent = nullptr)

#if 0

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

  QObject::connect(sender,
                   SIGNAL(action(QString, QString)),
                   this,
                   SLOT(onSignalReceived(QString, QString)));

  // connect sender signal to quit
  QObject::connect(sender,
                   SIGNAL(aboutToQuit()),
                   QCoreApplication::instance(),
                   SLOT(quit()));
#else

  m_sender =
    QSharedPointer<QDBusInterface>::create(SERVICE_NAME,
                                           "/",
                                           "org.example.sender",
                                           QDBusConnection::sessionBus());
  if (!m_sender || !m_sender->isValid())
  {
    fprintf(stderr,
            "%s\n",
            qPrintable(QDBusConnection::sessionBus().lastError().message()));
    QCoreApplication::instance()->quit();
  }

  // connects to `sender`'s signals
  //
  // NOTE: have to use m_sender->connect() but not QObject::connect() and if do,
  // nothing happens meaning SLOT get not called.
  m_sender->connect(m_sender.data(),
                    SIGNAL(action(QString, QString)),
                    this,
                    SLOT(onSignalReceived(QString, QString)));

  // connect sender signal to quit
  m_sender->connect(m_sender.data(),
                    SIGNAL(aboutToQuit()),
                    QCoreApplication::instance(),
                    SLOT(quit()));
#endif
}

// signature of the remote signal
// void action(const QString &nickname, const QString &text);
void Client::onSignalReceived(const QString &name, const QString &text)
{
  qDebug() << "Client::onSignalReceived: name: " << name << ", text: " << text
           << " called";

  // Asscess the remote property.

  // do not work
  // qDebug() << m_sender->call("powered").arguments().at(0);

  // work
  qDebug() << m_sender->property("Powered").value<bool>();
}
