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

  // NOTE: 
  // DBusConnection(std::shared_ptr<DBusConnectionPrivate> &&);
  // did not hide shared pointer arg inside and this means user have to specify
  // it. WHY?
  //
  // systemBus() call returns either invalid connection or valid one. To
  // distinguish two case, have two seperate ctors. 

private:
  DBusConnection();
  DBusConnection(std::shared_ptr<DBusConnectionPrivate> &&);

public:
  DBusConnection(const DBusConnection &);
  DBusConnection(DBusConnection &&);
  ~DBusConnection() = default;

  static DBusConnection systemBus(const EventLoop &);
  static DBusConnection sessionBus(const EventLoop &);

  // getters
  // get sd_bus handle
  sd_bus *handle() const;

  // get eventloop
  EventLoop eventLoop() const;

  // true if sd_bus* is set
  bool isConnected() const;

  // register the given name
  bool registerName(const std::string &name);

  DBusMessage call(DBusMessage &&message, int msTimeout = -1) const;
  bool callWithCallback(DBusMessage &&message,
                        const std::function<void(DBusMessage &&)> &callback,
                        int msTimeout = -1) const;
};

#endif // DBUSCONNECTION_H
