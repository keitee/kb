#ifndef DBUSCONNECTION_P_H
#define DBUSCONNECTION_P_H

class DBusConnectionPrivate
{
public:
  DBusConnectionPrivate(EventLoop const &eventLoop, sd_bus *bus);
  ~DBusConnectionPrivate();

  bool send(DBusMessage &&message) const;

  bool callWithCallback(DBusMessage &&message,
                        std::function<void(DBusMessage &&)> const &callback,
                        int msTimeout);

private:
  static int methodCallCallback(sd_bus_message *message,
                                void *data,
                                sd_bus_error *retError);

private:
  EventLoop m_eventLoop;
  sd_bus *m_bus;
  std::map<uint64_t, std::function<void(DBusMessage &&)>> m_callbacks;
};

#endif // DBUSCONNECTION_P_H
