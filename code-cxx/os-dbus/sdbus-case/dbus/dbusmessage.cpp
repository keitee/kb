/* ={--------------------------------------------------------------------------
 @brief :
  DBusMessagePrivate
*/

const std::map<DBusMessage::ErrorType, std::string>
  DBusMessagePrivate::m_errorNames = {
    {DBusMessage::NoError, ""},
    {DBusMessage::Other, ""},
    {DBusMessage::Failed, "org.freedesktop.DBus.Error.Failed"},
    {DBusMessage::NoMemory, "org.freedesktop.DBus.Error.NoMemory"},
    {DBusMessage::ServiceUnknown, "org.freedesktop.DBus.Error.ServiceUnknown"},
    {DBusMessage::NoReply, "org.freedesktop.DBus.Error.NoReply"},
    {DBusMessage::BadAddress, "org.freedesktop.DBus.Error.BadAddress"},
    {DBusMessage::NotSupported, "org.freedesktop.DBus.Error.NotSupported"},
    {DBusMessage::LimitsExceeded, "org.freedesktop.DBus.Error.LimitsExceeded"},
    {DBusMessage::AccessDenied, "org.freedesktop.DBus.Error.AccessDenied"},
    {DBusMessage::NoServer, "org.freedesktop.DBus.Error.NoServer"},
    {DBusMessage::Timeout, "org.freedesktop.DBus.Error.Timeout"},
    {DBusMessage::NoNetwork, "org.freedesktop.DBus.Error.NoNetwork"},
    {DBusMessage::AddressInUse, "org.freedesktop.DBus.Error.AddressInUse"},
    {DBusMessage::Disconnected, "org.freedesktop.DBus.Error.Disconnected"},
    {DBusMessage::InvalidArgs, "org.freedesktop.DBus.Error.InvalidArgs"},
    {DBusMessage::UnknownMethod, "org.freedesktop.DBus.Error.UnknownMethod"},
    {DBusMessage::TimedOut, "org.freedesktop.DBus.Error.TimedOut"},
    {DBusMessage::InvalidSignature, "org.freedesktop.DBus.Error.InvalidSignature"},
    {DBusMessage::UnknownInterface, "org.freedesktop.DBus.Error.UnknownInterface"},
    {DBusMessage::UnknownObject, "org.freedesktop.DBus.Error.UnknownObject"},
    {DBusMessage::UnknownProperty, "org.freedesktop.DBus.Error.UnknownProperty"},
    {DBusMessage::PropertyReadOnly, "org.freedesktop.DBus.Error.PropertyReadOnly"},
    {DBusMessage::InternalError, "org.qtproject.QtDBus.Error.InternalError"},
    {DBusMessage::InvalidObjectPath, "org.qtproject.QtDBus.Error.InvalidObjectPath"},
    {DBusMessage::InvalidService, "org.qtproject.QtDBus.Error.InvalidService"},
    {DBusMessage::InvalidMember, "org.qtproject.QtDBus.Error.InvalidMember"},
    {DBusMessage::InvalidInterface, "org.qtproject.QtDBus.Error.InvalidInterface"}
  };

DBusMessagePrivate::DBusMessagePrivate(DBusMessage::MessageType type,
                                       const std::string &service,
                                       const std::string &path,
                                       const std::string &interface,
                                       const std::string &method)
    : m_type(type)
    , m_service(service)
    , m_path(path)
    , m_interface(interface)
    , m_name(method)
{
  m_signature.reserve(8);
}

// construct an error message for cases when something went wrong outside of the
// sd-bus
DBusMessagePrivate::DBusMessagePrivate(DBusMessage::ErrorType error,
                                       const char *message)
    : m_type(DBusMessage::ErrorMessage)
    , m_errorName(m_errorNames.at(error))
    , m_errorMessage(message ? message : "")
{}

// construct an error message type from sd-bus error
// 
// https://www.freedesktop.org/software/systemd/man/sd_bus_error.html#
// typedef struct {
//         const char *name;
//         const char *message;
//         ...
// } sd_bus_error;

DBusMessagePrivate::DBusMessagePrivate(sd_bus_error *error)
    : m_type(DBusMessage::ErrorMessage)
    , m_errorName((error && error->name) ? error->name : "")
    , m_errorMessage((error && error->message) ? error->message : "")
{}

// construct either a new reply or error message from sd-bus message
DBusMessagePrivate::DBusMessagePrivate(sd_bus_message *reply)
  : m_type(getMessageType_(reply))
    , m_service(sd_bus_message_get_sender(reply))
{
  // extract name and message if it is an error message
  if (DBusMessage::ErrorMessage == m_type)
  {
    const sd_bus_error *error = sd_bus_message_get_error(reply);
    m_errorName = error->name;
    m_errorMessage = error->message;
  }

  // try and read any args if it is a reply
  if (DBusMessage::ReplyMessage == m_type)
  {
    const char *path = sd_bus_message_get_path(reply);
  }
}

// https://www.freedesktop.org/software/systemd/man/sd_bus_message_get_type.html#
// int sd_bus_message_get_type(	sd_bus_message *message,
//    uint8_t *type);
//
// sd_bus_message_get_type() returns the type of a message in the output
// parameter type, one of 
// SD_BUS_MESSAGE_METHOD_CALL,
// SD_BUS_MESSAGE_METHOD_RETURN,
// SD_BUS_MESSAGE_METHOD_ERROR,
// SD_BUS_MESSAGE_SIGNAL. 

DBusMessage::MessageType getMessageType_(sd_bus_message *reply)
{
  uint8_t type{};

  int rc = sd_bus_message_get_type(reply, &type);
  if (rc < 0)
  {
    logSysError(-rc, "failed to get message type");
    return DBusMessage::InvalidMessage;
  }

  switch (type)
  {
    case SD_BUS_MESSAGE_METHOD_RETURN:
      return DBusMessage::ReplyMessage;
    case SD_BUS_MESSAGE_METHOD_ERROR:
      return DBusMessage::ErrorMessage;
    default:
      logError("unexpected message type %hu", type);
      return DBusMessage::InvalidMessage;
  }
}
