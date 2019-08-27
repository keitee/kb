//
// dbustimeouts.h
//
// appinfrastructure/AppInfrastructure/IpcService/lib/source/DbusTimeouts.h
//

#ifndef K_DBUS_TIMEOUT_H
#define K_DBUS_TIMEOUT_H

/*
Object that manages the timeouts for a given dbus connection

warning:
this class is not thread safe, it is designed to only be called from one thread
which is the same thread the libdbus callbacks will be called from.


*/
class DbusTimeouts
{
  private:
    DBusConnection *conn_;

    int timer_fd_;

    bool within_event_handler_;

    struct _TimeoutEntry
    {
      struct timespec expiry_;
      DBusTimeout *timeout_;

      bool operator<(struct _TimeoutEntry const &rhs) const
      {
        return (expiry_.tv_sec < rhs.expiry_.tv_sec) ||
          ((expiry_.tv_sec == rhs.expiry_.tv_sec) && (expiry_.tv_nsec < rhs.expiry_.tv_nsec));
      }
    } TimeoutEntry;

    std::list<TimeoutEntry> timeouts_;

    std::thread::id expected_tid_;

    // static timeout callbacks
    static dbus_bool_t add_timeout_(DBusTimeout *timeout, void *data);
    static void toogle_timeout_(DBusTimeout *timeout, void *data);
    static void remove_timeout_(DBusTimeout *timeout, void *data);

  public:
    DbusTimeouts(DBusConnection *conn);
    ~DbusTimeouts();

    stop_event_dispatcher();
};

#endif // K_DBUS_TIMEOUT_H
