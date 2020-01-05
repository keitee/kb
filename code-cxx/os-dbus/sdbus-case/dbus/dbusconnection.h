#ifndef DBUSCONNECTION_H
#define DBUSCONNECTION_H

#include <memory>
// #include "eventloop/eventloop.h"
#include "eventloop.h"

typedef struct sd_bus sd_bus;
class DBusMessage;
class DBusConnectionPrivate;

class DBusConnection
{
  private:
    std::shared_ptr<DBusConnectionPrivate> m_private;
    DBusConnection();
    DBusConnection(std::shared_ptr<DBusConnectionPrivate> &&);

  public:
    // explicit DBusConnection(const DBusConnection &);
    // explicit DBusConnection(DBusConnection &&);

    DBusConnection(const DBusConnection &);
    DBusConnection(DBusConnection &&);

    // ~DBusConnection();

    static DBusConnection systemBus(const EventLoop &);
    static DBusConnection sessionBus(const EventLoop &);

    // getters
    sd_bus *handle() const;
    EventLoop eventLoop() const;

    // true if sd_bus* is set
    bool isConnected() const;

    // register the given name
    bool registerName(const std::string &name);

    DBusMessage call(DBusMessage &&message, int msTimeout = -1) const;
};

#endif // DBUSCONNECTION_H
