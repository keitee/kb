#ifndef DBUSMESSAGE_P_H
#define DBUSMESSAGE_P_H

#include <list>
#include <map>
#include <memory>
#include <string>

// NOTE: can be removed by using forward declaration
#include <systemd/sd-bus.h>

#include "dbusfiledescriptor.h"

/*
this class is to encapsulate creating sd_bus_message; sd_bus_error or
sd_bus_message which are all dbus message after all.

it provides conversion from or to them but limited to support types that are
in Arguments. For example, array type is not supported (added)

creates DBusMessage whenever needs a message

not easy to test DBusMessage itself. for example, it requires sd_bus* to use
sd_bus_message_dump but no way to pass via public interfaces. DBusConnection can
do since it's friend to access privates calls.

*/

// NOTE: can use forward declaration for friendship.
// #include "dbusconnection_p.h"
// #include "dbusmessage.h"
class DBusConnectionPrivate;
class DBusConnection;
class DBusMessage;

class DBusMessagePrivate
{
private:
  friend DBusMessage;
  friend DBusConnectionPrivate;
  friend DBusConnection;

  std::string m_path;
  std::string m_interface;
  std::string m_name;
  std::string m_errorName;
  std::string m_errorMessage;

  // NOTE: was public. make it private and use friend instead
  const DBusMessage::MessageType m_type;
  const std::string m_service;

  // is map necessary than vector?
  static const std::map<DBusMessage::ErrorType, std::string> m_errorNames;

private:
  DBusMessage::MessageType getMessageType_(sd_bus_message *message);

  // it was bool demarshallArgs(sd_bus_message *reply);
  bool fromMessage_(sd_bus_message *message);

  // to use unique_ptr with deleter
  // sd_bus_message *sd_bus_message_ref(sd_bus_message *m);

  using sd_bus_message_ptr =
    std::unique_ptr<sd_bus_message, sd_bus_message *(*)(sd_bus_message *)>;

  // NOTE: was public but make it private and is used by others directrly. shall
  // be used through DBusMessage?
  sd_bus_message_ptr toMessage_(sd_bus *bus) const;

private:
  struct Argument
  {
    enum Type
    {
      Boolean,
      Integer,
      UnsignedInteger,
      Double,
      FileDescriptor,
      String
    } type_;

    union BasicType
    {
      // *cxx-error* since it's union
      // bool bool_{};
      // int i_{};
      // unsigned ui_{};
      // double real_{0.0f};

      bool bool_;
      int i_;
      unsigned ui_;
      double real_;

      BasicType()
          : real_(0.0f)
      {}
      explicit BasicType(bool b)
          : bool_(b)
      {}
      explicit BasicType(int i)
          : i_(i)
      {}
      explicit BasicType(unsigned u)
          : ui_(u)
      {}
      explicit BasicType(double d)
          : real_(d)
      {}
    } basic_;

    std::string str_;

    // from `dusfiledescriptor.h`
    DBusFileDescriptor fd_;

    explicit Argument(bool b)
        : type_(Boolean)
        , basic_(b)
    {}
    explicit Argument(int i)
        : type_(Integer)
        , basic_(i)
    {}
    explicit Argument(unsigned u)
        : type_(UnsignedInteger)
        , basic_(u)
    {}
    explicit Argument(double d)
        : type_(Double)
        , basic_(d)
    {}
    explicit Argument(const char *str)
        : type_(String)
        , str_(str)
    {}
    explicit Argument(const std::string &str)
        : type_(String)
        , str_(str)
    {}
    explicit Argument(std::string &&str)
        : type_(String)
        , str_(str)
    {}
    explicit Argument(DBusFileDescriptor fd)
        : type_(FileDescriptor)
        , fd_(fd)
    {}

    explicit operator bool() const;
    explicit operator int() const;
    explicit operator unsigned() const;
    explicit operator double() const;
    explicit operator std::string() const;
    explicit operator DBusFileDescriptor() const;

    char dbusType() const;
  };

private:
  std::string m_signature;
  std::list<Argument> m_args;

public:
  // used by DBusMessage
  explicit DBusMessagePrivate(DBusMessage::MessageType type,
                              const std::string &service,
                              const std::string &path,
                              const std::string &interface,
                              const std::string &method);

  // the rest are used by DBusConection
  explicit DBusMessagePrivate(DBusMessage::ErrorType error,
                              const char *message);

  explicit DBusMessagePrivate(sd_bus_error *error);

  explicit DBusMessagePrivate(sd_bus_message *reply);
};

#endif // DBUSMESSAGE_P_H
