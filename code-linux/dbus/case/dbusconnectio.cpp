//
// dbusconnection.cpp
//

#include "dbusconnection.h"

DBusConnection::DBusConnection()
  : conn_(nullptr)
    , handler_(nullptr)
    , token_counter_(1)
{
  // https://dbus.freedesktop.org/doc/api/html/group__DBusThreads.html#gac7b8a7001befc3eaa8c6b043151008dc
  //
  // Initializes threads.
  // 
  // If this function is not called, the D-Bus library will not lock any data
  // structures. If it is called, D-Bus will do locking, at some cost in
  // efficiency.
  // 
  // Since D-Bus 1.7 it is safe to call this function from any thread, any
  // number of times (but it must be called before any other libdbus API is
  // used).

  if (!dbus_thread_init_default())
    std::cout << "fatal: dbus_thread_init_default failed" << std::endl;
}

DBusConnection::~DBusConnection()
{
}

