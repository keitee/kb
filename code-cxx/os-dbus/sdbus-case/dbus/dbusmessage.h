#ifndef DBUSMESSAGE_H
#define DBUSMESSAGE_H

#include <memory>
#include <string>

typedef struct sd_bus_message sd_bus_message;

class DBusMessagePrivate;

class DBusMessage
{
private:
  friend class DBusConnectionPrivate;

  std::shared_ptr<DBusMessagePrivate> m_private;

  DBusMessage(std::unique_ptr<DBusMessagePrivate> &&);

public:
  enum MessageType
  {
    MethodCallMessage,
    SignalMessage,
    ReplyMessage,
    IncomingSignalMessage,
    ErrorMessage,
    InvalidMessage
  };

  enum ErrorType
  {
    NoError,
    Other,
    Failed,
    NoMemory,
    ServiceUnknown,
    NoReply,
    BadAddress,
    NotSupported,
    LimitsExceeded,
    AccessDenied,
    NoServer,
    Timeout,
    NoNetwork,
    AddressInUse,
    Disconnected,
    InvalidArgs,
    UnknownMethod,
    TimedOut,
    InvalidSignature,
    UnknownInterface,
    UnknownObject,
    UnknownProperty,
    PropertyReadOnly,
    InternalError,
    InvalidObjectPath,
    InvalidService,
    InvalidMember,
    InvalidInterface,
  };

public:
  DBusMessage();
  ~DBusMessage();

  // NOTE: no copy but yes move? Yes, like std::unique_ptr
  DBusMessage(const DBusMessage &) = delete;
  DBusMessage(DBusMessage &&) noexcept;

  DBusMessage &operator=(const DBusMessage &) = delete;
  DBusMessage &operator=(DBusMessage &&) noexcept;

  bool isValid() const;
  bool isError() const;

  // getters
  MessageType type() const;
  std::string service() const;
  std::string path() const;
  std::string interface() const;
  std::string member() const;
  std::string signature() const;
  std::string errorMessage() const;
  std::string errorName() const;

  // write(push) arg from message
  template <typename T>
    DBusMessage &operator<<(const T &arg);

  // read(extract) arg from message
  template <typename T>
    DBusMessage &operator>>(T &arg);

public:
  static DBusMessage createMethodCall(
      const std::string &service,
      const std::string &path,
      const std::string &interface,
      const std::string &methond);

  static DBusMessage createIncomingSignal(sd_bus_message *reply);

};

#endif // DBUSMESSAGE_H
