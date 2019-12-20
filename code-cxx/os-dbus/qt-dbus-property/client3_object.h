#ifndef CLIENT_USE_PROXY_H
#define CLIENT_USE_PROXY_H

#include <QtCore/QCoreApplication>
#include <QtDBus/QtDBus>

#include "sender_interface_extended.h"

class ClientUseProxy : public QObject
{
  Q_OBJECT
  public:
    ClientUseProxy(const QString &name, org::example::sender *);
    // ClientUseProxy(QDBusAbstractInterface *);

  public slots:
    void onSignalReceived(const QString &name, const QString &text);

  private:
    QString name_;
    org::example::sender *proxy_;
};

#endif // CLIENT_USE_PROXY_H
