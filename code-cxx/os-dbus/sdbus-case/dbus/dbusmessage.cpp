
/* ={--------------------------------------------------------------------------
 @brief :
  DBusMessagePrivate
*/

// create a new message object

DBusMessagePrivate::DBusMessagePrivate(DBusMessage::MessageType type,
                                       std::string const &service,
                                       std::string const &path,
                                       std::string const &interface,
                                       std::string const &method)
    : m_type(type)
    , m_service(service)
    , m_path(path)
    , m_interface(interface)
    , m_method(method)
{}

DBusMessagePrivate::DBusMessagePrivate(DBusMessage::ErrorType errorType,
                                       const char *errorMessage)
    : m_type(DBusMessage::ErrorMessage)
    , m_errorName(m_errorNames.at(errorType))
    , m_errorMessage(errorMessage ? errorMessage : "")
{}

/*
https://www.freedesktop.org/software/systemd/man/sd_bus_error.html#
man sd_bus_error

note that header has more updated details:

/usr/include/systemd/sd-bus.h

#include <systemd/sd-bus.h>

typedef struct {
        const char *name;
        const char *message;
        ...
} sd_bus_error;

*/

DBusMessagePrivate::DBusMessagePrivate(sd_bus_error *error)
    : m_type(DBusMessage::ErrorMessage)
    , m_errorName((error && error->name) ? error->name : "")
    , m_errorMessage((error && error->message) ? error->message : "")
{}

DBusMessagePrivate::DBusMessagePrivate(sd_bus_message *reply)
    : m_type(getMessageType_(reply))
    , m_service(sd_bus_message_get_sender(reply))
{}

static DBusMessage::MessageType getMessageType_(sd_bus_message *reply)
{
  uint8_t type{};

  int ret = sd_bus_message_get_type(reply, &type);
  if (ret < 0)
  {
    LOG_MSG("failed to get message type");
    return DBusMessage::InvalidMessage;
  }
}

/* ={--------------------------------------------------------------------------
 @brief :
  DBusMessage
*/
