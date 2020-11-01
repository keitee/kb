#include "dbusmessage.h"
#include "dbusmessage_p.h"
#include "rlog.h"

// debug
#include <iostream>
#include <vector>

#include <cmath> // std::nan
#include <systemd/sd-bus.h>

#include <fcntl.h>

/* ={--------------------------------------------------------------------------
 @brief : implement variant type
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
    return INT32_MAX;
  }

  return basic_.i_;
}

DBusMessagePrivate::Argument::operator unsigned() const
{
  if (type_ != UnsignedInteger)
  {
    logWarning("argument type is not boolean");
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

  return str_;
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

// return the corresponding dbus type character that matches the stored type
// int the variant. os-dbus-type
char DBusMessagePrivate::Argument::dbusType() const
{
  switch (type_)
  {
    // explicit way
    // case Argument::Boolean: return 'b';
    case Boolean:
      return 'b';
    case Integer:
      return 'i';
    case UnsignedInteger:
      return 'u';
    case Double:
      return 'd';
    case String:
      return 's';
    case FileDescriptor:
      return 'h';
  }
}

/* ={--------------------------------------------------------------------------
 @brief :
  DBusMessagePrivate
*/

// clang-format off
// when use `enum class`
// reply error: org.freedesktop.DBus.Error.NoReply Method call timed out
//
// src/libsystemd/sd-bus/bus-error.c
// 126:                return SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_ACCESS_DENIED, "Access denied");
// 129:                return SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_INVALID_ARGS, "Invalid argument");
// 132:                return SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_UNIX_PROCESS_ID_UNKNOWN, "No such process");
// 135:                return SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_FILE_NOT_FOUND, "File not found");
// 138:                return SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_FILE_EXISTS, "File exists");
// 142:                return SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_TIMEOUT, "Timed out");
// 145:                return SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_IO_ERROR, "Input/output error");
// 150:                return SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_DISCONNECTED, "Disconnected");
// 153:                return SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_NOT_SUPPORTED, "Not supported");
// 156:                return SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_BAD_ADDRESS, "Address not available");
// 159:                return SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_LIMITS_EXCEEDED, "Limits exceeded");
// 162:                return SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_ADDRESS_IN_USE, "Address in use");
// 165:                return SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_INCONSISTENT_MESSAGE, "Inconsistent message");
// 338:        *e = SD_BUS_ERROR_MAKE_CONST(name, message);
// src/libsystemd/sd-bus/sd-bus.c
// 2436:                        &SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_NO_REPLY, "Method call timed out"),

const std::map<DBusMessage::ErrorType, std::string>
  DBusMessagePrivate::m_errorNames = {
    {DBusMessage::ErrorType::NoError, ""},
    {DBusMessage::ErrorType::Other, ""},
    {DBusMessage::ErrorType::Failed, "org.freedesktop.DBus.Error.Failed"},
    {DBusMessage::ErrorType::NoMemory, "org.freedesktop.DBus.Error.NoMemory"},
    {DBusMessage::ErrorType::ServiceUnknown, "org.freedesktop.DBus.Error.ServiceUnknown"},
    {DBusMessage::ErrorType::NoReply, "org.freedesktop.DBus.Error.NoReply"},
    {DBusMessage::ErrorType::BadAddress, "org.freedesktop.DBus.Error.BadAddress"},
    {DBusMessage::ErrorType::NotSupported, "org.freedesktop.DBus.Error.NotSupported"},
    {DBusMessage::ErrorType::LimitsExceeded, "org.freedesktop.DBus.Error.LimitsExceeded"},
    {DBusMessage::ErrorType::AccessDenied, "org.freedesktop.DBus.Error.AccessDenied"},
    {DBusMessage::ErrorType::NoServer, "org.freedesktop.DBus.Error.NoServer"},
    {DBusMessage::ErrorType::Timeout, "org.freedesktop.DBus.Error.Timeout"},
    {DBusMessage::ErrorType::NoNetwork, "org.freedesktop.DBus.Error.NoNetwork"},
    {DBusMessage::ErrorType::AddressInUse, "org.freedesktop.DBus.Error.AddressInUse"},
    {DBusMessage::ErrorType::Disconnected, "org.freedesktop.DBus.Error.Disconnected"},
    {DBusMessage::ErrorType::InvalidArgs, "org.freedesktop.DBus.Error.InvalidArgs"},
    {DBusMessage::ErrorType::UnknownMethod, "org.freedesktop.DBus.Error.UnknownMethod"},
    {DBusMessage::ErrorType::TimedOut, "org.freedesktop.DBus.Error.TimedOut"},
    {DBusMessage::ErrorType::InvalidSignature, "org.freedesktop.DBus.Error.InvalidSignature"},
    {DBusMessage::ErrorType::UnknownInterface, "org.freedesktop.DBus.Error.UnknownInterface"},
    {DBusMessage::ErrorType::UnknownObject, "org.freedesktop.DBus.Error.UnknownObject"},
    {DBusMessage::ErrorType::UnknownProperty, "org.freedesktop.DBus.Error.UnknownProperty"},
    {DBusMessage::ErrorType::PropertyReadOnly, "org.freedesktop.DBus.Error.PropertyReadOnly"},
    {DBusMessage::ErrorType::InternalError, "org.qtproject.QtDBus.Error.InternalError"},
    {DBusMessage::ErrorType::InvalidObjectPath, "org.qtproject.QtDBus.Error.InvalidObjectPath"},
    {DBusMessage::ErrorType::InvalidService, "org.qtproject.QtDBus.Error.InvalidService"},
    {DBusMessage::ErrorType::InvalidMember, "org.qtproject.QtDBus.Error.InvalidMember"},
    {DBusMessage::ErrorType::InvalidInterface, "org.qtproject.QtDBus.Error.InvalidInterface"}
};
// clang-format on

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
  // the signature is dynamically updated as args are marshalled into the
  // message object ... anyway reserve some space for the arg signature
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
    m_errorName               = error->name;
    m_errorMessage            = error->message;
  }

  // try and read any args if it is a reply
  if ((DBusMessage::ReplyMessage == m_type) ||
      (DBusMessage::IncomingSignalMessage == m_type))
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

    fromMessage_(reply);
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

DBusMessage::MessageType
DBusMessagePrivate::getMessageType_(sd_bus_message *reply)
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
    case SD_BUS_MESSAGE_SIGNAL:
      return DBusMessage::IncomingSignalMessage;
    case SD_BUS_MESSAGE_METHOD_ERROR:
      return DBusMessage::ErrorMessage;
    default:
      logError("unexpected message type %hu", type);
      return DBusMessage::InvalidMessage;
  }
}

/*
attempts to read the arguments from the message and convert sd_bus_message to
Arguments

https://freedesktop.org/software/systemd/man/sd_bus_message_read_basic.html#

/usr/include/systemd/sd-bus-protocol.h
Primitive types

enum {
       _SD_BUS_TYPE_INVALID         = 0,
       SD_BUS_TYPE_BYTE             = 'y',
       SD_BUS_TYPE_BOOLEAN          = 'b',
       SD_BUS_TYPE_INT16            = 'n',
       SD_BUS_TYPE_UINT16           = 'q',
       SD_BUS_TYPE_INT32            = 'i',
       SD_BUS_TYPE_UINT32           = 'u',
       SD_BUS_TYPE_INT64            = 'x',
       SD_BUS_TYPE_UINT64           = 't',
       SD_BUS_TYPE_DOUBLE           = 'd',
       SD_BUS_TYPE_STRING           = 's',
       SD_BUS_TYPE_OBJECT_PATH      = 'o',
       SD_BUS_TYPE_SIGNATURE        = 'g',
       SD_BUS_TYPE_UNIX_FD          = 'h',
       SD_BUS_TYPE_ARRAY            = 'a',
       SD_BUS_TYPE_VARIANT          = 'v',
       SD_BUS_TYPE_STRUCT           = 'r', // not actually used in signatures
       SD_BUS_TYPE_STRUCT_BEGIN     = '(',
       SD_BUS_TYPE_STRUCT_END       = ')',
       SD_BUS_TYPE_DICT_ENTRY       = 'e', // not actually used in signatures
       SD_BUS_TYPE_DICT_ENTRY_BEGIN = '{',
       SD_BUS_TYPE_DICT_ENTRY_END   = '}'
};

int sd_bus_message_read_basic(sd_bus_message *m,
  char type,
  void *p);

sd_bus_message_read_basic() reads a basic type from a message and advances the
read position in the message. The set of basic types and their ascii codes
passed in type are described in the D-Bus Specification.

int sd_bus_message_skip(sd_bus_message *m,
  const char* types);


https://freedesktop.org/software/systemd/man/sd_bus_message_at_end.html#

int sd_bus_message_at_end(sd_bus_message *m,
  int complete);

sd_bus_message_at_end() returns whether all data from the currently opened
container in m or all data from all containers in m has been read. If complete
is zero, this function returns whether all data from the currently opened
container has been read. If complete is non-zero, this function returns whether
all data from all containers in m has been read.

https://freedesktop.org/software/systemd/man/sd_bus_message_peek_type.html#

sd_bus_message_peek_type() determines the type of the next element in m to be
read by sd_bus_message_read() or similar functions. On success, the type is
stored in type, if it is not NULL. If the type is a container type, the type of
its elements is stored in contents, if it is not NULL. If this function
successfully determines the type of the next element in m, it returns a positive
integer. If there are no more elements to be read, it returns zero.

*/

bool DBusMessagePrivate::fromMessage_(sd_bus_message *message)
{
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

      // original code which don't support array type. added support here and
      // where get this from?
      case SD_BUS_TYPE_ARRAY:
      case SD_BUS_TYPE_STRUCT:
#if 0
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
#endif

      {
        std::vector<std::string> names;

        // _public_ int sd_bus_message_enter_container(sd_bus_message *m,
        //                                             char type,
        //                                             const char *contents);
        //
        int r =
          sd_bus_message_enter_container(message, SD_BUS_TYPE_ARRAY, nullptr);
        if (r < 0)
        {
          logSysError(-r, "failed to enter container");
        }

        // returns true if we're at the end of the current container.
        // _public_ int sd_bus_message_at_end(sd_bus_message *m, int complete);
        //
        //  bool atEnd()
        //  { return (sd_bus_message_at_end(mMessage, false) != 0); }
        //
        // so sd_bus_message_at_end() returns 1(true?) when reaches the end

        while (0 == sd_bus_message_at_end(message, false))
        {
          const char *str{nullptr};

          // _public_ int sd_bus_message_read_basic(sd_bus_message *m, char type, void *p);
          r = sd_bus_message_read_basic(message, SD_BUS_TYPE_STRING, &str);
          if ((r < 0) || !str)
          {
            logSysError(-r, "failed to read message");
          }

          m_args.emplace_back(Argument(std::string(str)));

          names.emplace_back(std::string(str));
        } // while

        // ok, reading is done and close it
        // _public_ int sd_bus_message_close_container(sd_bus_message *m);
        r = sd_bus_message_exit_container(message);
        if (r < 0)
        {
          logSysError(-r, "failed to exit container");
        }

        // // ok, let see what we've got back from the call
        // for (const auto e : names)
        //   std::cout << "names: " << e << std::endl;

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

    // single place to handle failure for all cases
    if (rc < 0)
    {
      logSysWarning(-rc, "failed to read or skip message arguments");
      return false;
    }
  } // while
}

/*
construct a sd_bus_message for either a method call or signal
that is, from args to sd_bus_message

https://freedesktop.org/software/systemd/man/sd_bus_message_append_basic.html#

sd_bus_message_append_basic() appends a single field to the message m. The
parameter type determines how the pointer p is interpreted. type must be one of
the basic types as defined by the Basic Types section of the D-Bus
specification, and listed in the table below.

*/

DBusMessagePrivate::sd_bus_message_ptr
DBusMessagePrivate::toMessage_(sd_bus *bus) const
{
  int rc{};

  sd_bus_message *message{nullptr};

  if (DBusMessage::MethodCallMessage == m_type)
  {
    rc = sd_bus_message_new_method_call(bus,
                                        &message,
                                        m_service.c_str(),
                                        m_path.c_str(),
                                        m_interface.c_str(),
                                        m_name.c_str());
  }
  else if (DBusMessage::SignalMessage)
  {
    rc = sd_bus_message_new_signal(bus,
                                   &message,
                                   m_path.c_str(),
                                   m_interface.c_str(),
                                   m_name.c_str());
  }
  else
  {
    logError("invalid message type");
    // same as T() and sd_bus_message_ptr is unique_ptr with deleter
    return sd_bus_message_ptr(nullptr, sd_bus_message_unref);
  }

  // message should be created and check if message is created
  if ((rc < 0) || (message == nullptr))
  {
    logSysError(-rc, "failed to create message");
    return sd_bus_message_ptr(nullptr, sd_bus_message_unref);
  }

  // populate it with all the args
  for (const Argument &arg : m_args)
  {
    switch (arg.type_)
    {
      case Argument::Boolean:
        rc = sd_bus_message_append_basic(message,
                                         SD_BUS_TYPE_BOOLEAN,
                                         &arg.basic_.bool_);
        break;

      case Argument::Integer:
        rc = sd_bus_message_append_basic(message,
                                         SD_BUS_TYPE_INT32,
                                         &arg.basic_.i_);
        break;

      case Argument::UnsignedInteger:
        rc = sd_bus_message_append_basic(message,
                                         SD_BUS_TYPE_UINT32,
                                         &arg.basic_.ui_);
        break;

      case Argument::Double:
        rc = sd_bus_message_append_basic(message,
                                         SD_BUS_TYPE_DOUBLE,
                                         &arg.basic_.real_);
        break;

      case Argument::String:
        rc = sd_bus_message_append_basic(message,
                                         SD_BUS_TYPE_STRING,
                                         arg.str_.c_str());
        break;

      case Argument::FileDescriptor:
        int desc = arg.fd_.fd();
        rc = sd_bus_message_append_basic(message, SD_BUS_TYPE_UNIX_FD, &desc);
        break;
    } // switch

    // check for any errors
    if (rc < 0)
    {
      logSysWarning(-rc, "failed to marshall the method call arguments");
      sd_bus_message_unref(message);
      return sd_bus_message_ptr(nullptr, sd_bus_message_unref);
    }
  } // for

  // sd_bus_message_dump() is not found. which version?
  //
  // debug purpose
  // https://freedesktop.org/software/systemd/man/sd_bus_message_dump.html#
  // int sd_bus_message_dump( sd_bus_message *m,
  //   FILE *f,
  //   uint64_t flags);
  //
  // On inclusion of the <cstdio> header file, three objects of this type are
  // automatically created, and pointers to them are declared: stdin, stdout and
  // stderr, associated with the standard input stream, standard output stream
  // and standard error stream, respectively.
  //
  // sd_bus_message_dump(message, stdout, O_WRONLY);

  // now return the constructed message
  return sd_bus_message_ptr(message, sd_bus_message_unref);
}

/* ={--------------------------------------------------------------------------
 @brief :
 DBusMessage
*/

// construct an invalid DBusMessage object. NOTE: shall make it private or
// deleted?
DBusMessage::DBusMessage()
    : m_private(nullptr)
{}

DBusMessage::~DBusMessage()
{
  m_private.reset();
}

// move supports
DBusMessage::DBusMessage(DBusMessage &&rhs) noexcept
    : m_private(std::move(rhs.m_private))
{}

DBusMessage &DBusMessage::operator=(DBusMessage &&rhs) noexcept
{
  m_private = std::move(rhs.m_private);
  return *this;
}

// have to use std::move() since uses rvalue from unique to shared
// pointer.
//
//      template<typename _Tp1, typename _Del>
//        shared_ptr(std::unique_ptr<_Tp1, _Del>&& __r)
//        : __shared_ptr<_Tp>(std::move(__r)) { }

DBusMessage::DBusMessage(std::unique_ptr<DBusMessagePrivate> &&rhs)
    : m_private(std::move(rhs))
{}

DBusMessage::DBusMessage(ErrorType error, const char *message)
    : m_private(std::make_unique<DBusMessagePrivate>(error, message))
{}

// true if the message is valid
bool DBusMessage::isValid() const
{
  // return !!m_private;
  return m_private != nullptr;
}

// true if message type is error
bool DBusMessage::isError() const
{
  return m_private->m_type == MessageType::ErrorMessage;
}

// return message type
DBusMessage::MessageType DBusMessage::type() const
{
  return m_private->m_type;
}

std::string DBusMessage::service() const
{
  return m_private->m_service;
}

std::string DBusMessage::path() const
{
  return m_private->m_path;
}

std::string DBusMessage::interface() const
{
  return m_private->m_interface;
}

std::string DBusMessage::member() const
{
  return m_private->m_name;
}

std::string DBusMessage::signature() const
{
  return m_private->m_signature;
}

std::string DBusMessage::errorMessage() const
{
  return m_private->m_errorMessage;
}

std::string DBusMessage::errorName() const
{
  return m_private->m_errorName;
}

template <typename T>
DBusMessage &DBusMessage::operator>>(T &arg)
{
  if ((MessageType::ReplyMessage == m_private->m_type) ||
      (MessageType::IncomingSignalMessage == m_private->m_type))
  {
    if (m_private->m_args.empty())
    {
      logWarning("no more args in the dbus message");
    }
    else
    {
      const auto &value = m_private->m_args.front();
      arg               = static_cast<T>(value);
      // since it's std::list
      m_private->m_args.pop_front();
    }
  }
  else
  {
    logWarning(
      "message is not a method reply or signal so cannot read arguments");
  }

  return *this;
}

template DBusMessage &DBusMessage::operator>><bool>(bool &);
template DBusMessage &DBusMessage::operator>><std::string>(std::string &);
template DBusMessage &DBusMessage::operator>><int>(int &);
template DBusMessage &DBusMessage::operator>><unsigned>(unsigned &);
template DBusMessage &DBusMessage::operator>><double>(double &);
template DBusMessage &DBusMessage::operator>>
  <DBusFileDescriptor>(DBusFileDescriptor &);

template <typename T>
DBusMessage &DBusMessage::operator<<(const T &arg)
{
  if ((MessageType::MethodCallMessage == m_private->m_type) ||
      (MessageType::SignalMessage == m_private->m_type))
  {
    DBusMessagePrivate::Argument variant(arg);

    m_private->m_signature.push_back(variant.dbusType());
    // since Argument do not have 'move support', it will not have an effect.
    m_private->m_args.emplace_back(std::move(variant));
  }
  else
  {
    logWarning(
      "message is not a method call or signal so cannot add arguments");
  }

  return *this;
}

template DBusMessage &DBusMessage::operator<<<bool>(const bool &);
template DBusMessage &DBusMessage::operator<<<int>(const int &);
template DBusMessage &DBusMessage::operator<<<unsigned>(const unsigned &);
template DBusMessage &DBusMessage::operator<<<double>(const double &);
template DBusMessage &DBusMessage::operator<<<std::string>(const std::string &);
template DBusMessage &
  DBusMessage::operator<<<DBusFileDescriptor>(const DBusFileDescriptor &);

DBusMessage DBusMessage::createMethodCall(const std::string &service,
                                          const std::string &path,
                                          const std::string &interface,
                                          const std::string &method)
{
  // uses private ctor
  return DBusMessage(
    std::make_unique<DBusMessagePrivate>(DBusMessage::MethodCallMessage,
                                         service,
                                         path,
                                         interface,
                                         method));
}

DBusMessage DBusMessage::createIncomingSignal(sd_bus_message *reply)
{
  return DBusMessage(std::make_unique<DBusMessagePrivate>(reply));
}
