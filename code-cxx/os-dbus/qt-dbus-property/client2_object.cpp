#include "client2_object.h"
#include "sender_interface.h"

// ClientUseProxy::ClientUseProxy(org::example::sender *)
ClientUseProxy::ClientUseProxy(org::example::sender *sender)
    : proxy_(sender)
{}

// have get the remote signal and access remote property

void ClientUseProxy::onSignalReceived(const QString &name, const QString &text)
{
  qDebug() << "ClientUseProxy::onSignalReceived: name: " << name
           << ", text: " << text << " called";

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
