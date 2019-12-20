//
// dbuseventdispatcher.h
//
// appinfrastructure/AppInfrastructure/IpcService/lib/source/DbusEventDispatcher.h
//

#ifndef K_DBUS_EVENT_DISPATCHER_H
#define K_DBUS_EVENT_DISPATCHER_H

class DbusEventDispatcher
{
  private:
    DBusConnection *conn_;

    std::thread dispatcher_;

    int stop_event_fd;
    int wakeup_event_fd;
    int dispatch_event_fd;

    uint64_t call_count_;

    std::mutex m_;
    std::condition_variable cv_;
    std::queue<std::pair<uint64_t, std::function<void()>>> q_;

    bool call_in_event_loop_impl_(std::function<void()> const &func);
    void cleanup_all_events_();

  public:
    DbusEventDispatcher();
    ~DbusEventDispatcher();

    stop_event_dispatcher();
};

#endif // K_DBUS_EVENT_DISPATCHER_H
