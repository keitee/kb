#include "client1_object.h"

#define SERVICE_NAME "org.example.SenderExample"

Client::Client()
{
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

#ifdef THIS_WORKS_AS_WELL
  m_sender->connect(m_sender.data(),
                    SIGNAL(action(QString, QString)),
                    this,
                    SLOT(onSignalReceived(QString, QString)));
  connect sender signal to quit m_sender->connect(m_sender.data(),
                                                  SIGNAL(aboutToQuit()),
                                                  QCoreApplication::instance(),
                                                  SLOT(quit()));
#else
  connect(m_sender.data(),
          SIGNAL(action(QString, QString)),
          this,
          SLOT(onSignalReceived(QString, QString)));

  connect(m_sender.data(),
          SIGNAL(aboutToQuit()),
          QCoreApplication::instance(),
          SLOT(quit()));
#endif
}

void Client::onSignalReceived(const QString &name, const QString &text)
{
  qDebug()
    << QString("Client::onSignalReceived: name(%1), text(%2)").arg(name, text);

  // Asscess the remote property.

  // NOT able to call property read methond as it's not work
  // qDebug() << m_sender->call("powered").arguments().at(0);
  // use instead:
  // QVariant reply = iface->property("powered");

  qDebug() << QString("Client::onSignalReceived: powerd propertry(%1)")
                .arg(m_sender->property("Powered").value<bool>());
}
