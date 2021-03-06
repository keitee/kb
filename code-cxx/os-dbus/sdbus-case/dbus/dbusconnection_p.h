#ifndef DBUSCONNECTION_P_H
#define DBUSCONNECTION_P_H

#include "dbusmessage.h"
#include "eventloop.h"

#include <map>
#include <systemd/sd-bus.h>
#include <systemd/sd-event.h>

class DBusConnectionPrivate
{
private:
  // NOTE
  friend class DBusConnection;

  EventLoop m_eventloop;
  sd_bus *m_bus;

  // pair<message cookie, callback>
  std::map<uint64_t, std::function<void(DBusMessage &&)>> m_callbacks;

private:
  static int methodCallCallback_(sd_bus_message *msg,
                                 void *userData,
                                 sd_bus_error *retError);

public:
  DBusConnectionPrivate(const EventLoop &eventloop, sd_bus *bus);
  ~DBusConnectionPrivate();

  // to prevent copies
  DBusConnectionPrivate(const DBusConnectionPrivate &) = delete;
  DBusConnectionPrivate &operator=(const DBusConnectionPrivate &) = delete;

  bool callWithCallback(DBusMessage &&message,
                        const std::function<void(DBusMessage &&)> &callback,
                        int msTimeout);

  // send a message,signal or call, to dbus and uses corresponding sd_bus_send*
  // call depending on message type.
  //
  // runs either on calling thread or event loop thread

  bool send(DBusMessage &&message) const;
};

#endif // DBUSCONNECTION_P_H
