#include "client3_object.h"
#include "sender_interface_extended.h"

// ClientUseProxy::ClientUseProxy(org::example::sender *)
ClientUseProxy::ClientUseProxy(org::example::sender *sender)
    : proxy_(sender)
{}

// signature of the remote signal
// void action(const QString &nickname, const QString &text);

void ClientUseProxy::onSignalReceived(const QString &name, const QString &text)
{
  qDebug() << "ClientUseProxy::onSignalReceived: name: " << name
           << ", text: " << text << " called";

  // DO NOT WORK AS it returns `invalid`
  // `If no such property exists, the returned variant is invalid.`
  qDebug() << "ClientUseProxy::onSignalReceived: get property: "
           << proxy_->powered();
  qDebug() << "ClientUseProxy::onSignalReceived: get property: "
           << proxy_->property("Powered");

  qDebug() << "ClientUseProxy::onSignalReceived: set property(false)";
  proxy_->setPowered(false);

  qDebug() << "ClientUseProxy::onSignalReceived: get property: "
           << proxy_->powered();

  qDebug() << "ClientUseProxy::onSignalReceived: set property(true)";
  proxy_->setPowered(true);

  qDebug() << "ClientUseProxy::onSignalReceived: get property: "
           << proxy_->powered();
}
