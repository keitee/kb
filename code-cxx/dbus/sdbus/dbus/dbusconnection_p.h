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
    EventLoop m_eventLoop;
    sd_bus *m_bus;

};

#endif // DBUSCONNECTION_P_H
