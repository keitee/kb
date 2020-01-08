#include <sys/signal.h>
#include <systemd/sd-bus-vtable.h>
#include <systemd/sd-daemon.h>

#include "dbusconnection.h"
#include "eventloop.h"
#include "rlog.h"

/*
https://www.freedesktop.org/software/systemd/man/sd_bus_default.html

Normally, slot objects (as created by sd_bus_add_match(3) and similar calls)
keep a reference to their bus connection object, too. Thus, as long as a bus
slot object remains referenced its bus object will remain allocated too.
Optionally, bus slot objects may be placed in "floating" mode. When in floating
mode the life cycle of the bus slot object is bound to the bus object, i.e. when
the bus object is freed the bus slot object is automatically unreferenced too.
The floating state of a slot object may be controlled explicitly with
sd_bus_slot_set_floating(3), though usually floating bus slot objects are
created by passing NULL as the slot parameter of sd_bus_add_match() and related
calls, thus indicating that the caller is not directly interested in referencing
and managing the bus slot object.


https://www.freedesktop.org/software/systemd/man/sd_bus_add_match.html#

sd_bus_add_match, sd_bus_add_match_async, sd_bus_match_signal,
sd_bus_match_signal_async — Add a match rule for incoming message dispatching

typedef int (*sd_bus_message_handler_t)(sd_bus_message *m, void *userdata,
sd_bus_error *ret_error);

int sd_bus_add_match(	sd_bus *bus,
  sd_bus_slot **slot,
  const char *match,
  sd_bus_message_handler_t callback,
  void *userdata);

Description

sd_bus_add_match() installs a match rule for messages received on the specified
bus connection object bus. The syntax of the match rule expression passed in
match is described in the D-Bus Specification.

The specified handler function callback is called for eaching incoming message
matching the specified expression, the userdata parameter is passed as-is to the
callback function.

The match is installed synchronously when connected to a bus broker, i.e. the
call sends a control message requested the match to be added to the broker and
waits until the broker confirms the match has been installed successfully.

sd_bus_add_match_async() operates very similar to sd_bus_match_signal(), however
it installs the match asynchronously, in a non-blocking fashion: a request is
sent to the broker, but the call does not wait for a response. The
install_callback function is called when the response is later received, with
the response message from the broker as parameter. If this function is specified
as NULL a default implementation is used that terminates the bus connection
should installing the match fail.

sd_bus_match_signal() is very similar to sd_bus_add_match(), but only matches
signals, and instead of a match expression accepts four parameters: sender (the
service name of the sender), path (the object path of the emitting object),
interface (the interface the signal belongs to), member (the signal name), from
which the match string is internally generated. Optionally, these parameters may
be specified as NULL in which case the relevant field of incoming signals is not
tested.

sd_bus_match_signal_async() combines the signal matching logic of
sd_bus_match_signal() with the asynchronous behaviour of
sd_bus_add_match_async().

On success, and if non-NULL, `the slot return parameter` will be set to a slot
object that may be used as a reference to the installed match, and may be
utilized to remove it again at a later time with sd_bus_slot_unref(3). If
specified as NULL the lifetime of the match is bound to the lifetime of the bus
object itself, and the match is generally not removed independently. See
sd_bus_slot_set_floating(3) for details.


https://dbus.freedesktop.org/doc/dbus-specification.html

Messages that list a client as their DESTINATION do not need to match the
client's match rules, and are sent to that client regardless. As a result, match
rules are mainly used to receive a subset of broadcast signals.

Match rules are added using the AddMatch bus method (see the section called
    “org.freedesktop.DBus.AddMatch”). Rules are specified as a string of comma
separated key/value pairs.

Excluding a key from the rule indicates a wildcard match. For instance excluding
the the member from a match rule but adding a sender would let all messages from
that sender through. An example of a complete rule would be

"type='signal',sender='org.freedesktop.DBus',interface='org.freedesktop.DBus',member='Foo',path='/bar/foo',destination=':452345.34',arg2='bar'"

*/

// typedef int (*sd_bus_message_handler_t)(sd_bus_message *m, void *userdata,
// sd_bus_error *ret_error);

int rule_matched(sd_bus_message *msg, void *userData, void *retError)
{
  std::string message;

  // simply print out message details
  // https://www.freedesktop.org/software/systemd/man/sd_bus_message_get_sender.html#
  // const char* sd_bus_message_get_sender(	sd_bus_message *message);
  // const char* sd_bus_message_get_destination(	sd_bus_message
  // *message); const char* sd_bus_message_get_path(	sd_bus_message
  // *message); const char* sd_bus_message_get_interface(	sd_bus_message
  // *message); const char* sd_bus_message_get_member(	sd_bus_message
  // *message);

  message += "sender=" + std::string(sd_bus_message_get_sender(msg));
  message += ", destination=" + std::string(sd_bus_message_get_destination(msg));
  message += ", path=" + std::string(sd_bus_message_get_path(msg));
  message += ", interface=" + std::string(sd_bus_message_get_interface(msg));
  message += ", member=" + std::string(sd_bus_message_get_member(msg));

  logWarning("rules matched: %s", message.c_str());
}

void install_match(DBusConnection &conn, sd_bus_slot &slot)
{
  std::string matchRule = "type='signal'";

  // comment them out to accept all
  //
  // if (!service.empty())
  // {
  //   matchRule += ",sender='" + service + "'";
  // }
  // if (!path.empty())
  // {
  //   matchRule += ",path='" + path + "'";
  // }
  // if (!interface.empty())
  // {
  //   matchRule += ",interface='" + interface + "'";
  // }
  // if (!signalName.empty())
  // {
  //   matchRule += ",member='" + signalName + "'";
  // }

  int rc = sd_bus_add_match(conn.handle(),
                            &slot,
                            matchRule.c_str(),
                            sd_bus_message_handler_t(rule_matched),
                            nullptr);
  if (rc < 0)
  {
    logSysError(-rc, "failed to add match");
  }
}

// ={=========================================================================

namespace
{
  // cannot do this
  // DBusConnection dbusConn;

  std::string objectPath("/net/poettering/Calculator"); // object path
  std::string interface("net.poettering.Calculator");   // interface
  std::string service;                                  // service
} // namespace

static int method_multiply(sd_bus_message *m, void *data, sd_bus_error *error)
{
  int64_t x{};
  int64_t y{};
  int r{};

  // read the parameters
  r = sd_bus_message_read(m, "xx", &x, &y);
  if (r < 0)
  {
    logSysError(-r, "failed to parse/read parameters");
    return r;
  }

  logWarning("method multiply is called");

  // reply with the result
  return sd_bus_reply_method_return(m, "x", x * y);
}

static int method_divide(sd_bus_message *m, void *data, sd_bus_error *error)
{
  int64_t x{};
  int64_t rhs{};
  int r{};

  // read the parameters
  r = sd_bus_message_read(m, "xx", &x, &rhs);
  if (r < 0)
  {
    logSysError(-r, "failed to parse/read parameters");
    return r;
  }

  // return an error on division by zero
  //
  // sd_bus_error_set() sets an error structure to the specified name and
  // message strings.
  //
  // int sd_bus_error_set_const(sd_bus_error *e,
  //  const char *name,
  //  const char *message);
  //
  // sd_bus_error_set_const() is similar to sd_bus_error_set(), but the string
  // parameters are not copied internally, and must hence remain constant and
  // valid for the lifetime of e. Use this call to avoid memory allocations when
  // setting error structures. Since this call does not allocate memory, it will
  // not fail with an out-of-memory condition as sd_bus_error_set() can, as
  // described above. Alternatively, the SD_BUS_ERROR_MAKE_CONST() macro may be
  // used to generate a literal, constant bus error structure on-the-fly.

  if (rhs == 0)
  {
    sd_bus_error_set_const(error,
                           "net.poettering.DivisionByZero",
                           "sorry, cannot do that");
    return -EINVAL;
  }

  logWarning("method divide is called");

  // reply with the result
  return sd_bus_reply_method_return(m, "x", x / rhs);
}

static const sd_bus_vtable calculator_vtable[] = {
  SD_BUS_VTABLE_START(0),
  SD_BUS_METHOD(
    "Multiply", "xx", "x", method_multiply, SD_BUS_VTABLE_UNPRIVILEGED),
  SD_BUS_METHOD("Divide", "xx", "x", method_divide, SD_BUS_VTABLE_UNPRIVILEGED),
  SD_BUS_VTABLE_END};

// tracker callback
//
// Called by the sd-bus library when there is a tracked service drops off the
// bus.
// Annoyingly the callback doesn't tell us which service has dropped off the
// bus, so we have to iterate through all the callbacks and check if still being
// tracked, if not it means they've gone bye bye.

static int tracker_handler(sd_bus_track *track, void *data)
{
  // print out message for now
  logWarning("tracker handler is called");
}

static void send_signal(DBusConnection &conn)
{
  sd_bus_message *msg{nullptr};
  std::string arg1;
  std::string arg2;

  // https://www.freedesktop.org/software/systemd/man/sd_bus_message_new_signal.html#
  //
  // int sd_bus_message_new_signal(sd_bus *bus,
  //  sd_bus_message **m,                         // out
  //  const char *path,
  //  const char *interface,
  //  const char *member);

  // The sd_bus_message_new_signal() function creates a new bus message object
  // that encapsulates a D-Bus signal, and returns it in the m output parameter.
  // The signal will be sent to path path, on the interface interface, member
  // member. When this message is sent, no reply is expected. See
  // sd_bus_message_new_call(1) for a short description of the meaning of the
  // path, interface, and member parameters.

  int rc = sd_bus_message_new_signal(conn.handle(),
                                     &msg,
                                     objectPath.c_str(),
                                     interface.c_str(),
                                     "WebSocketUpdate");
  if (rc < 0)
  {
    logSysError(-rc, "failed to create new signal message");
    return;
  }

  // add the arguments

  // https://www.freedesktop.org/software/systemd/man/sd_bus_message_append.html#
  //
  // int sd_bus_message_append(
  //  sd_bus_message *m,
  //  const char *types,
  //  ...)
  //
  // The sd_bus_message_append() function appends a sequence of fields to the
  // D-Bus message object m. The type string types describes the types of the
  // field arguments that follow. For each type specified in the type string,
  // one or more arguments need to be specified, in the same order as declared
  // in the type string.

  rc = sd_bus_message_append(msg, "ss", arg1.c_str(), arg2.c_str());
  if (rc < 0)
  {
    logSysError(-rc, "failed to append args to the signal");
  }
  else
  {
    // send the signal
    rc = sd_bus_send_to(conn.handle(), msg, service.c_str(), nullptr);
    if (rc < 0)
    {
      logSysError(-rc, "failed to send the signal");
    }
  }

  sd_bus_message_unref(msg);
}

// ={=========================================================================
int main(int argc, char **argv)
{
  int rc{};
  sd_bus_slot *slot{nullptr};
  sd_bus_track *tracker{nullptr};

  // disable the SIGPIPE signal, the most annoying signal in the world
  signal(SIGPIPE, SIG_IGN);

  // create the event loop
  EventLoop eventLoop;

  // connect to the system bus
  // copy ctor

  // 0000095658.544841 ERR: < M:dbusconnection.cpp F:registerName L:228 > failed
  // to acquire the service name (13-Permission denied) DBusConnection conn =
  // DBusConnection::systemBus(eventLoop);
  DBusConnection conn = DBusConnection::sessionBus(eventLoop);
  if (!conn.isConnected())
  {
    logError("failed to connect to system bus");
    return EXIT_FAILURE;
  }

  // done in ASServicePrivate::ASServicePrivate for example

  // https://www.freedesktop.org/software/systemd/man/sd_bus_add_object_vtable.html#
  // install the object
  //
  // sd_bus_add_object_vtable() is used to declare attributes for the path
  // object path path connected to the bus connection bus under the interface
  // interface. The table vtable may contain property declarations using
  // SD_BUS_PROPERTY() or SD_BUS_WRITABLE_PROPERTY(), method declarations using
  // SD_BUS_METHOD(), SD_BUS_METHOD_WITH_NAMES(), SD_BUS_METHOD_WITH_OFFSET(),
  // or SD_BUS_METHOD_WITH_NAMES_OFFSET(), and signal declarations using
  // SD_BUS_SIGNAL_WITH_NAMES() or SD_BUS_SIGNAL(), see below. The userdata
  // parameter contains a pointer that will be passed to various callback
  // functions. It may be specified as NULL if no value is necessary.
  //
  // For both functions, a match slot is created internally. If the output
  // parameter slot is NULL, a "floating" slot object is created, see
  // sd_bus_slot_set_floating(3). Otherwise, a pointer to the slot object is
  // returned. In that case, the reference to the slot object should be dropped
  // when the vtable is not needed anymore, see sd_bus_slot_unref(3).

  rc = sd_bus_add_object_vtable(conn.handle(),      // bus
                                &slot,              // slot
                                objectPath.c_str(), // object path
                                interface.c_str(),  // interface
                                calculator_vtable,  // vtable
                                NULL);              // user data
  if (rc < 0)
  {
    logSysError(-rc, "failed to add object");
    return EXIT_FAILURE;
  }

#if 0
  // when use this, keep getting tracker_handler() called.

  // TODO what's tracker?
  // create a tracker object to keep track of registered clients, if they
  // disappear then we automatically remove their listeners
  rc = sd_bus_track_new(conn.handle(),
      &tracker,
      &tracker_handler,
      nullptr);
  if (rc < 0)
  {
    logSysError(-rc, "failed to create bus tracker");
  }
#endif

  // register name and it will end up calling like:
  // r = sd_bus_request_name(bus, "net.poettering.Calculator", 0);

  if (!conn.registerName("net.poettering.Calculator"))
  {
    logFatal("failed to acquire service name");
    return EXIT_FAILURE;
  }

  logWarning("sdbus server is running");

  // Notify that the service is now ready.
  sd_notify(0, "READY=1");

#if 0
  // parse the response message
  // https://www.freedesktop.org/software/systemd/man/sd_bus_process.html#
  //
  // sd_bus_process() drives the connection between the client and the message
  // bus. That is, it handles connecting, authentication, and message
  // processing. When invoked pending I/O work is executed, and queued incoming
  // messages are dispatched to registered callbacks. Each time it is invoked a
  // single operation is executed.
  //
  // return positive if a message was processed. It returns zero when no
  // operations were pending.
  //
  // When zero is returned the caller should synchronously poll for I/O events
  // before calling into sd_bus_process() again. For that either user the
  // simple, synchronous sd_bus_wait(3) call, or hook up the bus connection
  // object to an external or manual event loop using sd_bus_get_fd(3).
  //
  // https://www.freedesktop.org/software/systemd/man/sd_bus_wait.html#
  //
  // sd_bus_wait() synchronously waits for I/O on the specified bus connection
  // object. This function is supposed to be called whenever sd_bus_process(3)
  // returns zero, indicating that no work is pending on the connection.
  // Internally, this call invokes ppoll(3), to wait for I/O on the bus
  // connection. If the timeout_sec parameter is specified, the call will block
  // at most for the specified amount of time in µs. Pass UINT64_MAX to permit
  // it to sleep indefinitely.

  for (;;)
  {
    // process requests
    r = sd_bus_process(bus, NULL);
    if (r < 0)
    {
      fprintf(stderr, "failed to process bus: %s\n", strerror(-r));
      goto finish;
    }

    // processed a request and try to process another one right away
    if (r > 0)
    {
      printf("processed a request and try another\n");
      continue;
    }

    r = sd_bus_wait(bus, (uint64_t)-1);
    if (r < 0)
    {
      fprintf(stderr, "failed to wait on bus: %s\n", strerror(-r));
      goto finish;
    }
  }
#endif

  return eventLoop.run();
}
