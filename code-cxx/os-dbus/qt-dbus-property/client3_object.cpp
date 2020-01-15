#include "client3_object.h"
#include "sender_interface_extended.h"

// ClientUseProxy::ClientUseProxy(org::example::sender *)
ClientUseProxy::ClientUseProxy(const QString &name,
                               org::example::sender *sender)
    : name_(name)
    , proxy_(sender)
{}

// signature of the remote signal
// void action(const QString &nickname, const QString &text);

void ClientUseProxy::onSignalReceived(const QString &name, const QString &text)
{
  qDebug() << name_ << "::onSignalReceived: name: " << name
           << ", text: " << text << " called";

  qDebug() << name_
           << "::onSignalReceived: get property: " << proxy_->powered();

  qDebug() << name_ << "::onSignalReceived: get property: "
           << proxy_->property("Powered");

  qDebug() << name_ << "::onSignalReceived: set property(false)";
  proxy_->setPowered(false);

  qDebug() << name_
           << "::onSignalReceived: get property: " << proxy_->powered();

  qDebug() << name_ << "::onSignalReceived: set property(true)";
  proxy_->setPowered(true);

  qDebug() << name_
           << "::onSignalReceived: get property: " << proxy_->powered();
}
