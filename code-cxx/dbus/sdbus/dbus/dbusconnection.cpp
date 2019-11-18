"dbusconnection.h"

/* ={--------------------------------------------------------------------------
 @brief :
  DBusConnectionPrivate
*/

/* ={--------------------------------------------------------------------------
 @brief :
  DBusConnection
*/
DBusConnection::DBusConnection(std::shared_ptr<DBusConnectionPrivate> &&priv)
{
}
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
}

