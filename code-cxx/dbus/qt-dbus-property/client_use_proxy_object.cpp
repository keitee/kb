#include "client_use_proxy_object.h"
#include "sender_interface.h"

ClientUseProxy::ClientUseProxy()
{
}


// signature of the remote signal
// void action(const QString &nickname, const QString &text);

void ClientUseProxy::onSignalReceived(const QString &name, const QString &text)
{
  qDebug() << "ClientUseProxy::onSignalReceived: name: " 
    << name << ", text: " 
    << text << " called";

  emit senderPowerChanged(true);
}
