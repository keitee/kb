//
// dbusconnection.h
//
// appinfrastructure/AppInfrastructure/IpcService/lib/source/DbusConnection.cpp
// appinfrastructure/AppInfrastructure/IpcService/lib/source/DbusConnection.h
//

#ifndef K_DBUSCONNECTION_H
#define K_DBUSCONNECTION_H

// since DBusConnection is used DBus use DbusConnection

class DbusConnection
{
  public:
    DbusConnection();
    ~DbusConnection();

    // enum DBusBusType { DBUS_BUS_SESSION, DBUS_BUS_SYSTEM, DBUS_BUS_STARTER }
    // Well-known bus types. More...

    bool connect(DBusBusType type, std::string const &serviceName = std::string());

  private:
    struct
    {
      uint64_t token_;
      DBusConnection * conn_;
    } ReplyContext;

    std::atomic<uint64_t> token_counter_;
    std::map<uint64_t, DBusMessage*> replies_;

    // Q: what's the benefit of using own mutex, condition_variable?

    std::mutex m_;
    std::condition_variable cv_;

    // https://dbus.freedesktop.org/doc/api/html/group__DBusShared.html
    // enum DBusHandlerResult { DBUS_HANDLER_RESULT_HANDLED, DBUS_HANDLER_RESULT_NOT_YET_HANDLED, DBUS_HANDLER_RESULT_NEED_MEMORY }
    // Results that a message handler can return. More...

    using MessageHandler = std::function<DBusHandlerResult(DBusMessage*)>;
    MessageHandler handler_;

    COMMON::SpinLock handler_lock_;
    std::string service_name_;

    DBusConnection *conn_;
    DBusEventDispatcher disp_;
};

#endif // K_DBUSCONNECTION_H
