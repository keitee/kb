#ifndef DBUSCONNECTION_H
#define DBUSCONNECTION_H

#include "eventloop/eventloop.h"

class DBusConnection
{
  public:
    DBusConnection(DBusConnection const &other);

    static DBusConnection systemBus(Eventloop const &eventLoop);
    static DBusConnection sessionBus(Eventloop const &eventLoop);
    static DBusConnection connectToBus(Eventloop const &eventLoop,
        std::string const &address);

  private:
    DBusConnection(std::shared_ptr<DBusConnectionPrivate> &&priv);
    std::shared_ptr<DBusConnectionPrivate> m_private;
};

#endif // DBUSCONNECTION_H
