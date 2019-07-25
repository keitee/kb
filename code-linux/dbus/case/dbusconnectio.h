//
// dbusconnection.h
//

#ifndef K_DBUSCONNECTION_H
#define K_DBUSCONNECTION_H

class DbusConnection
{
  public:
    DbusConnection();
    ~DbusConnection();

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

    using MessageHandler = std::function<DBusHandlerResult(DBusMessage*)>;
    MessageHandler handler_;

    SpinLock spin_lock_;
    std::string service_name_;

    DBusConnection *conn_;
    DBusEventDispatcher disp_;
};

#endif // K_DBUSCONNECTION_H
