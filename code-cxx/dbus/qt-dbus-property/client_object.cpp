#include "client_object.h"

#define SERVICE_NAME "org.example.SenderExample"

 Client::Client()
 {
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
   if(!sender->isValid())
   {
     fprintf(stderr, "%s\n",
         qPrintable(QDBusConnection::sessionBus().lastError().message()));
     QCoreApplication::instance()->quit();
   }
 
   connect(sender, SIGNAL(action(QString, QString)),
       this, SLOT(onSignalReceived(QString, QString)));

   // connect sender signal to quit
   connect(sender, SIGNAL(aboutToQuit()),
       QCoreApplication::instance(), SLOT(quit()));
 }

// signature of the remote signal
// void action(const QString &nickname, const QString &text);
void Client::onSignalReceived(const QString &name, const QString &text)
{
  qDebug() << "Client::onSignalReceived: name: " << name << ", text: " << text
    << " called";
}
