#ifndef DBUSCONNECTION_P_H
#define DBUSCONNECTION_P_H

#include "eventloop.h"
#include "dbusmessage.h"

#include <systemd/sd-bus.h>
#include <systemd/sd-event.h>

class DBusConnectionPrivate
{
private:
  // NOTE
  friend class DBusConnection;

  EventLoop m_eventloop;
  sd_bus *m_bus;

  // TODO: callback supports
  // std::map<uint64_t, std::function<void(DBusMessage &&)>> m_callbacks;

public:
  DBusConnectionPrivate(const EventLoop &eventloop, sd_bus *bus);
  ~DBusConnectionPrivate();

  // send a message to dbus and uses corresponding sd_bus_send* call depending
  // on message type.
  //
  // runs either on calling thread or event loop thread

  bool send(DBusMessage &&message) const;
};

#endif // DBUSCONNECTION_P_H
