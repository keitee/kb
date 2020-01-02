#ifndef DBUSCONNECTION_P_H
#define DBUSCONNECTION_P_H

class DBusConnecitonPrivae
{
private:
  EventLoop m_eventloop;
  sd_bus *m_bus;

  std::map<uint64_t, std::function<void(DBusMessage &&)>> m_callbacks;

public:
  explicit DBusConnectionPrivate(const EventLoop &eventloop, sd_bus *bus);
  ~DBusConnectionPrivate();

  bool send(DBusMessage &&message) const;
};

#endif // DBUSCONNECTION_P_H
