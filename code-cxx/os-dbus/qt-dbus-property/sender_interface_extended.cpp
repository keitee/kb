#include "sender_interface_extended.h"

/*
 * Implementation of interface class OrgExampleSenderInterface
 */

OrgExampleSenderInterface::OrgExampleSenderInterface(
  const QString &service,
  const QString &path,
  const QDBusConnection &connection,
  QObject *parent)
    : DBusAbstractInterface(
        service, path, staticInterfaceName(), connection, parent)
{
  connect(this,
          SIGNAL(senderPowerChanged(bool)),
          this,
          SLOT(onPowerChanged(bool)));
}

OrgExampleSenderInterface::~OrgExampleSenderInterface() {}

void OrgExampleSenderInterface::onPowerChanged(bool power)
{
  qDebug() << "got powerChanged signal (" << power
           << ") from the remote sender";
}
