#ifndef DBUSMESSAGE_P_H
#define DBUSMESSAGE_P_H

class DBusMessagePrivate
{
  public:
    DBusMessagePrivate(DBusMessage::MessageType type,
        const std::string &service,
        const std::string &path,
        const std::string &interface,
        const std::string &method);

    DBusMessagePrivate(DBusMessage::ErrorType errorType,
        const char *errorMessage);

    explicit DBusMessagePrivate(sd_bus_message *reply);
    explicit DBusMessagePrivate(sd_bus_error *error);

    // TODO: different?
    // ~DBusMessagePrivate() = default;
    ~DBusMessagePrivate() {};

  public:
    const DBusMessage::MessageType m_type;
    const std::string m_service;

  private:
    std::string m_path;
    std::string m_interface;
    std::string m_method;

};

#endif // DBUSMESSAGE_P_H
