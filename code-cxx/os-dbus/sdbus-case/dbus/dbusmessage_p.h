#ifndef DBUSMESSAGE_P_H
#define DBUSMESSAGE_P_H

class DBusMessagePrivate
{
private:
  std::string m_path;
  std::string m_interface;
  std::string m_name;
  std::string m_signature;
  std::string m_errorName;
  std::string m_errorMessage;

  static const std::map<DBusMessage::ErrorType, std::string> m_errorNames;

public:
  const DBusMessage::MessageType m_type;

  explicit DBusMessagePrivate(DBusMessage::MessageType type,
                              const std::string &service,
                              const std::string &path,
                              const std::string &interface,
                              const std::string &method);

  explicit DBusMessagePrivate(DBusMessage::ErrorType error,
                              const char *message);

  explicit DBusMessagePrivate(sd_bus_error *error);

  explicit DBusMessagePrivate(sd_bus_message *reply);
};

#endif // DBUSMESSAGE_P_H
