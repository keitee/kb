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

private:
  // not used and removed
  // DBusConnection();

  // NOTE:
  // DBusConnection ctor is private since it's used from static function
  // systemBus/sessionBus and user is expected to use this to get dbus.
  //
  // these static function creates dbus by calling ctor
  //
  // DBusConnection(std::shared_ptr<DBusConnectionPrivate> &&);
  //
  // did not hide shared pointer arg inside and this means user have to specify
  // it than DBusConnection ctor do it. WHY? because:
  //
  // systemBus() call returns either invalid connection or valid one. To
  // distinguish two case.

  DBusConnection(std::shared_ptr<DBusConnectionPrivate> &&);

public:
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

  bool send(DBusMessage &&message) const;

  // note: is not used outside of this class so why it's public?
  bool callWithCallback(DBusMessage &&message,
                        const std::function<void(DBusMessage &&)> &callback,
                        int msTimeout = -1) const;
};

#endif // DBUSCONNECTION_H
