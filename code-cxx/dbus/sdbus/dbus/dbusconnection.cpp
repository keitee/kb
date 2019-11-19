"dbusconnection.h"

/* ={--------------------------------------------------------------------------
 @brief :
  DBusConnectionPrivate
*/

DBusConnectionPrivate::DBusConnectionPrivate(EventLoop const &eventLoop, sd_bus *bus)
  : m_eventLoop(eventLoop)
    , m_bus(bus)
{} 


/* ={--------------------------------------------------------------------------
 @brief :
  DBusConnection
*/
DBusConnection::DBusConnection(std::shared_ptr<DBusConnectionPrivate> &&priv)
  : m_private(std::move(priv))
{
}

// https://www.freedesktop.org/software/systemd/man/sd_bus_attach_event.html#
//
// sd_bus_attach_event, sd_bus_detach_event, sd_bus_get_event 
// — Attach a bus connection object to an event loop
//
// sd_bus_attach_event() attaches the specified bus connection object to an
// sd-event(3) event loop object at the specified priority (see
// sd_event_source_set_priority(3) for details on event loop priorities). 
//
// When a bus connection object is attached to an event loop incoming messages
// will be automatically read and processed, and outgoing messages written,
// whenever the event loop is run. When the event loop is about to terminate,
// the bus connection is automatically flushed and closed (see
// sd_bus_set_close_on_exit(3) for details on this). 
//
// By default bus connection objects are not attached to any event loop. When a
// bus connection object is attached to one it is not necessary to invoke
// sd_bus_wait(3) or sd_bus_process(3) as this functionality is handled
// automatically by the event loop.
//
// see sdbus_server_1.c example


DBusConnection DBusConnection::systemBus(EventLoop const &eventLoop)
{
  sd_bus *bus{nullptr};

  int ret = sd_bus_open_system(&bus);
  if (ret < 0)
  {
    LOG_MSG("failed to connect to system bus");
    return DBusConnection(nullptr);
  }

  ret = sd_bus_attach_event(bus, eventLoop.handle(),
      SD_EVENT_PRIORITY_NORMAL);
  if (ret < 0)
  {
    LOG_MSG("failed to attach bus to event loop");
    sd_bus_unref(bus);
    return DBusConnection(nullptr);
  }

  return DBusConnection(std::make_shared<DBusConnectionPrivate>(eventLoop, bus));
}

