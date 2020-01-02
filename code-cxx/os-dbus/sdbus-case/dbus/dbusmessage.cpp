/* ={--------------------------------------------------------------------------
 @brief :
 DBusMessagePrivate::Argument
*/

DBusMessagePrivate::Argument::operator bool() const
{
  if (type_ != Boolean)
  {
    logWarning("argument type is not boolean");
    return false;
  }

  return basic_.bool_;
}

DBusMessagePrivate::Argument::operator int() const
{
  if (type_ != Integer)
  {
    logWarning("argument type is not boolean");
    // TODO
    return INT32_MAX;
  }

  return basic_.i_;
}

DBusMessagePrivate::Argument::operator unsigned() const
{
  if (type_ != UnsignedInteger)
  {
    logWarning("argument type is not boolean");
    // TODO
    return UINT32_MAX;
  }

  return basic_.ui_;
}

DBusMessagePrivate::Argument::operator double() const
{
  if (type_ != Double)
  {
    logWarning("argument type is not boolean");
    // TODO
    // https://en.cppreference.com/w/cpp/numeric/math/nan
    return std::nan("");
  }

  return basic_.real_;
}

DBusMessagePrivate::Argument::operator std::string() const
{
  if (type_ != String)
  {
    logWarning("argument type is not boolean");
    return std::string();
  }

  return basic_.str_;
}

DBusMessagePrivate::Argument::operator DBusFileDescriptor() const
{
  if (type_ != FileDescriptor)
  {
    logWarning("argument type is not boolean");
    return DBusFileDescriptor();
  }

  return fd_;
}

// return the dbus type character that matches the stored type
// TODO: no defaults
char DBusMessagePrivate::Argument::dbusType() const
{
  switch (type_)
  {
    // explicit way
    // case Argument::Boolean: return 'b';
    case Boolean: return 'b';
    case Integer: return 'i';
    case UnsignedInteger: return 'u';
    case Double: return 'd';
    case String: return 's';
    case FileDescriptor: return 'h';
  }
}

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
    if (path)
      m_path.assign(path);

    const char *interface = sd_bus_message_get_interface(reply);
    if (interface)
      m_interface.assign(interface);

    // https://www.freedesktop.org/software/systemd/man/sd_bus_message_get_signature.html#
    //
    // const char* sd_bus_message_get_signature(
    //  sd_bus_message *message,
    //  int complete);
    //
    // sd_bus_message_get_signature() returns the signature of message message.
    // If complete is true, the signature of the whole message is returned, and
    // just the signature of the currently open container otherwise.

    const char *signature = sd_bus_message_get_signature(reply, 1);
    if (signature)
      m_signature.assign(signature);

    demarshallArgs(reply);
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

DBusMessage::MessageType DBusMessagePrivate::getMessageType_(sd_bus_message *reply)
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

// attempts to read the arguments from the message
//
// /usr/include/systemd/sd-bus-protocol.h
// Primitive types
// 
// enum {
//         _SD_BUS_TYPE_INVALID         = 0,
//         SD_BUS_TYPE_BYTE             = 'y',
//         SD_BUS_TYPE_BOOLEAN          = 'b',
//         SD_BUS_TYPE_INT16            = 'n',
//         SD_BUS_TYPE_UINT16           = 'q',
//         SD_BUS_TYPE_INT32            = 'i',
//         SD_BUS_TYPE_UINT32           = 'u',
//         SD_BUS_TYPE_INT64            = 'x',
//         SD_BUS_TYPE_UINT64           = 't',
//         SD_BUS_TYPE_DOUBLE           = 'd',
//         SD_BUS_TYPE_STRING           = 's',
//         SD_BUS_TYPE_OBJECT_PATH      = 'o',
//         SD_BUS_TYPE_SIGNATURE        = 'g',
//         SD_BUS_TYPE_UNIX_FD          = 'h',
//         SD_BUS_TYPE_ARRAY            = 'a',
//         SD_BUS_TYPE_VARIANT          = 'v',
//         SD_BUS_TYPE_STRUCT           = 'r', /* not actually used in signatures */
//         SD_BUS_TYPE_STRUCT_BEGIN     = '(',
//         SD_BUS_TYPE_STRUCT_END       = ')',
//         SD_BUS_TYPE_DICT_ENTRY       = 'e', /* not actually used in signatures */
//         SD_BUS_TYPE_DICT_ENTRY_BEGIN = '{',
//         SD_BUS_TYPE_DICT_ENTRY_END   = '}'
// };

bool DBusMessagePrivate::demarshallArgs_(sd_bus_message *message)
{
  // NOTE:
  // No document on sd_bus_message_at_end() and need to look into the code?
  while (!sd_bus_message_at_end(message, false))
  {
    char type = '\0';
    const char *content{nullptr};
    int rc = sd_bus_message_peek_type(message, &type, &content);
    if (rc < 0)
    {
      logSysWarning(-rc, "failed to get message arg");
      return false;
    }
    
    // process the argument type
    switch (type)
    {
      case SD_BUS_TYPE_BOOLEAN:
        {
          int32_t value;
          rc = sd_bus_message_read_basic(message, type, &value);
          if (rc > 0)
            m_args.emplace_back(Argument(static_cast<bool>(value)));
          break;
        }

      case SD_BUS_TYPE_INT32:
        {
          int32_t value;
          rc = sd_bus_message_read_basic(message, type, &value);
          if (rc > 0)
            m_args.emplace_back(Argument(static_cast<int>(value)));
          break;
        }

      case SD_BUS_TYPE_UINT32:
        {
          int32_t value;
          rc = sd_bus_message_read_basic(message, type, &value);
          if (rc > 0)
            m_args.emplace_back(Argument(static_cast<unsigned>(value)));
          break;
        }

      case SD_BUS_TYPE_DOUBLE:
        {
          double value;
          rc = sd_bus_message_read_basic(message, type, &value);
          if (rc > 0)
            m_args.emplace_back(Argument(value));
          break;
        }

      case SD_BUS_TYPE_STRING:
        {
          const char *value{nullptr};
          rc = sd_bus_message_read_basic(message, type, &value);
          if (rc > 0)
            m_args.emplace_back(Argument(value));
          break;
        }

      case SD_BUS_TYPE_UNIX_FD:
        {
          int value{-1};
          rc = sd_bus_message_read_basic(message, type, &value);
          if (rc > 0)
            m_args.emplace_back(Argument(DBusFileDescriptor(value)));
          break;
        }

      case SD_BUS_TYPE_ARRAY:
      case SD_BUS_TYPE_STRUCT:
        {
          std::string types;

          // is used to skip below
          types += type;
          if (content)
            types += content;

          logWarning("received message with unsupported array or struct so skip");
          rc = sd_bus_message_skip(message, types.c_str());
          break;
        }

      default:
        {
          std::string types;

          // is used to skip below
          types += type;

          logWarning("received message with unsupported array or struct so skip");
          rc = sd_bus_message_skip(message, types.c_str());
          break;
        }
    } // switch

    // NOTE: single place to handle failure for all cases
    if (rc < 0)
    {
      logSysWarning(-rc, "failed to read or skip message arguments");
      return false;
    }
  } // while
}

// construct a sd_bus_message
toMessage(sd_bus *bus) const
{
  int rc{};

  sd_bus_message *message{nullptr};
  if (DBusMessage::MethodCallMessage == m_type)
  {
    rc = sd_bus_message_new_method_call(
        bus, &msg,
        m_service.c_str(),
        m_path.c_str(),
        m_interface.c_str(),
        m_name.c_str());
  }
}
