#include <future>
#include <string>
#include <sys/signal.h>
#include <systemd/sd-bus-vtable.h>
#include <systemd/sd-daemon.h>
#include <systemd/sd-id128.h>

#include "dbusconnection.h"
#include "dbusmessage.h"
#include "eventloop.h"
#include "rlog.h"

#include "gmock/gmock.h"

using namespace std;
using namespace testing;

namespace
{
  // create the event loop
  EventLoop loop;
} // namespace

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

// ={=========================================================================
// server example which is from sdbus_server_1.cpp

#undef SD_BUS_VTABLE_START
#define SD_BUS_VTABLE_START(_flags)                                            \
  {                                                                            \
    .type = _SD_BUS_VTABLE_START, .flags = _flags,                             \
    .x = {                                                                     \
      .start = {.element_size = sizeof(sd_bus_vtable)},                        \
    },                                                                         \
  }

#undef SD_BUS_METHOD_WITH_OFFSET
#define SD_BUS_METHOD_WITH_OFFSET(_member,                                     \
                                  _signature,                                  \
                                  _result,                                     \
                                  _handler,                                    \
                                  _offset,                                     \
                                  _flags)                                      \
  {                                                                            \
    .type = _SD_BUS_VTABLE_METHOD, .flags = _flags,                            \
    .x = {                                                                     \
      .method =                                                                \
        {                                                                      \
          .member    = _member,                                                \
          .signature = _signature,                                             \
          .result    = _result,                                                \
          .handler   = _handler,                                               \
          .offset    = _offset,                                                \
        },                                                                     \
    },                                                                         \
  }

#undef SD_BUS_SIGNAL
#define SD_BUS_SIGNAL(_member, _signature, _flags)                             \
  {                                                                            \
    .type = _SD_BUS_VTABLE_SIGNAL, .flags = _flags,                            \
    .x = {                                                                     \
      .signal =                                                                \
        {                                                                      \
          .member    = _member,                                                \
          .signature = _signature,                                             \
        },                                                                     \
    },                                                                         \
  }

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

// *os-dbus-type*
// x           64-bit signed integer
// #define SD_BUS_METHOD(_member, _signature, _result, _handler, _flags)

static const sd_bus_vtable calculator_vtable[] = {
  SD_BUS_VTABLE_START(0),
  SD_BUS_METHOD(
    "Multiply", "xx", "x", method_multiply, SD_BUS_VTABLE_UNPRIVILEGED),
  SD_BUS_METHOD("Divide", "xx", "x", method_divide, SD_BUS_VTABLE_UNPRIVILEGED),
  SD_BUS_VTABLE_END};

// ={=========================================================================
class SdbusServerTest : public ::testing::Test
{
protected:
  // DBusConnection m_conn = DBusConnection::sessionBus(eventLoop);

protected:
  const std::string m_path{"/net/poettering/Calculator"};     // object path
  const std::string m_interface{"net.poettering.Calculator"}; // interface
  sd_bus *m_bus{nullptr};
  sd_id128_t m_id{};
  int m_fds[2]{};

public:
  SdbusServerTest() {}

  ~SdbusServerTest() {}

  void SetUp() { logWarning("SetUp() called"); }

  void TearDown() { logWarning("TearDown() called"); }
};

TEST_F(SdbusServerTest, check_1)
{
  EXPECT_THAT(true, true);
}

#if 0

// As with as-player which create event loop and dbus connection in main and
// hold them for its lifetime, do the same here.
//

TEST_F(SdbusServerTest, check_2)
{
  int r{};
  sd_bus_slot *slot{nullptr};

  // disable the SIGPIPE signal, the most annoying signal in the world
  signal(SIGPIPE, SIG_IGN);

  // have to use sessionBus:
  // 0000095658.544841 ERR: < M:dbusconnection.cpp F:registerName L:228 > failed
  // to acquire the service name (13-Permission denied) DBusConnection conn =
  // DBusConnection::systemBus(eventLoop);

  // note that DBusConnection only support copy ctor and means that not able
  // to create member or global var.
  DBusConnection conn = DBusConnection::sessionBus(loop);
  if (!conn.isConnected())
  {
    logError("failed to connect to session bus");
    return;
  }

  logWarning("sdbus is connected");

  // https://www.freedesktop.org/software/systemd/man/sd_bus_add_object_vtable.html#
  // install the object
  //
  // sd_bus_add_object_vtable() is used to declare attributes for the path
  // object path `path` connected to the bus connection bus under the interface
  // interface.
  //
  // The table vtable may contain property declarations using SD_BUS_PROPERTY()
  // or SD_BUS_WRITABLE_PROPERTY(), method declarations using SD_BUS_METHOD(),
  // SD_BUS_METHOD_WITH_NAMES(), SD_BUS_METHOD_WITH_OFFSET(), or
  // SD_BUS_METHOD_WITH_NAMES_OFFSET(), and signal declarations using
  // SD_BUS_SIGNAL_WITH_NAMES() or SD_BUS_SIGNAL(), see below.
  //
  // The `userdata` parameter contains a pointer that will be passed to handler
  // callback functions. It may be specified as NULL if no value is necessary.
  //
  // typedef int (*sd_bus_message_handler_t)(	sd_bus_message *m,
  //  	void *userdata,
  //  	sd_bus_error *ret_error);
  //
  // For both functions, a `match slot` is created internally. If the output
  // parameter slot is NULL, a "floating" slot object is created, see
  // sd_bus_slot_set_floating(3). Otherwise, a pointer to the slot object is
  // returned. In that case, the reference to the slot object should be dropped
  // when the vtable is not needed anymore, see sd_bus_slot_unref(3).

  r = sd_bus_add_object_vtable(conn.handle(),       // bus
                               &slot,               // slot
                               m_path.c_str(),      // object path
                               m_interface.c_str(), // interface
                               calculator_vtable,   // vtable
                               nullptr);            // user data

  if (r < 0)
  {
    logSysError(-r, "failed to add object");
    return;
  }

  logWarning("sdbus object is added");

  // server example requset name and run for loop to handle dbus call.
  // r = sd_bus_request_name(bus, "net.poettering.Calculator", 0);
  // r = sd_bus_process(bus, NULL);
  //
  // now uses event loop and what to do?

  // int sd_bus_set_server(sd_bus *bus, int b, sd_id128_t bus_id);
  //
  // sd_bus_set_server() configures the bus object as a server for direct D-Bus
  // connections. b enables/disables the server mode. If zero, the server mode is
  // disabled. Otherwise, the server mode is enabled. Configuring a bus object as
  // a server is required to allow establishing direct connections between two
  // peers without going via the D-Bus daemon.
  //
  // id must contain a 128-bit integer id for the server. If clients add a guid
  // field to their D-Bus address string, the server id must match this guid or
  // the D-Bus authentication handshake will fail. If no specific id is defined
  // for the server, sd_id128_randomize can be used to generate a random id
  // instead.

  // https://www.freedesktop.org/software/systemd/man/sd_id128_randomize.html#
  //
  // #include <systemd/sd-id128.h>
  // int sd_id128_randomize(	sd_id128_t *ret);
  //
  // sd_id128_randomize() generates a new randomized 128-bit ID and returns it
  // in ret. Every invocation returns a new randomly generated ID. This uses the
  // /dev/urandom kernel random number generator.

  sd_id128_t bus_id{};

  r = sd_id128_randomize(&bus_id);
  if (r < 0)
  {
    logSysError(-r, "failed to add id128");
    return;
  }

  // int sd_bus_set_server(sd_bus *bus, int b, sd_id128_t bus_id);
  r = sd_bus_set_server(conn.handle(), (int)1, bus_id);
  if (r < 0)
  {
    std::cout << "r : " << r << std::endl;
    logSysError(-r, "failed to set server");
    return;
  }

  // int sd_bus_is_server(sd_bus *bus);
  r = sd_bus_is_server(conn.handle());
  if (r > 0)
  {
    logWarning("server mode is set");
  }
}
#endif

// https://www.freedesktop.org/software/systemd/man/sd_bus_add_object_vtable.html#
// install the object
//
// sd_bus_add_object_vtable() is used to declare attributes for the path
// object path `path` connected to the bus connection bus under the interface
// interface.
//
// The table vtable may contain property declarations using SD_BUS_PROPERTY()
// or SD_BUS_WRITABLE_PROPERTY(), method declarations using SD_BUS_METHOD(),
// SD_BUS_METHOD_WITH_NAMES(), SD_BUS_METHOD_WITH_OFFSET(), or
// SD_BUS_METHOD_WITH_NAMES_OFFSET(), and signal declarations using
// SD_BUS_SIGNAL_WITH_NAMES() or SD_BUS_SIGNAL(), see below.
//
// The `userdata` parameter contains a pointer that will be passed to handler
// callback functions. It may be specified as NULL if no value is necessary.
//
// typedef int (*sd_bus_message_handler_t)(	sd_bus_message *m,
//  	void *userdata,
//  	sd_bus_error *ret_error);
//
// For both functions, a `match slot` is created internally. If the output
// parameter slot is NULL, a "floating" slot object is created, see
// sd_bus_slot_set_floating(3). Otherwise, a pointer to the slot object is
// returned. In that case, the reference to the slot object should be dropped
// when the vtable is not needed anymore, see sd_bus_slot_unref(3).

// ={=========================================================================
// uses the same server/client code from
// libsystemd/sd-bus/test-bus-server.c

#define _cleanup_(x) __attribute__((__cleanup__(x)))

static inline const char *strna(const char *s)
{
  return s ?: "n/a";
}

#if 0
TEST_F(SdbusServerTest, check_2_1)
{
  int r{};
  sd_bus_slot *slot{nullptr};

  assert(socketpair(AF_UNIX, SOCK_STREAM, 0, m_fds) >= 0);

  assert(sd_id128_randomize(&m_id) >= 0);

  assert(sd_bus_new(&m_bus) >= 0);
  assert(sd_bus_set_fd(m_bus, m_fds[0], m_fds[0]) >= 0);
  assert(sd_bus_set_server(m_bus, 1, m_id) >= 0);
  assert(sd_bus_set_anonymous(m_bus, true) >= 0);
  assert(sd_bus_negotiate_fds(m_bus, true) >= 0);
  assert(sd_bus_start(m_bus) >= 0);

  // int sd_bus_is_server(sd_bus *bus);
  r = sd_bus_is_server(m_bus);
  if (r > 0)
  {
    logWarning("server mode is set");
  }

  auto t = std::async([this] {
    int r{};

    _cleanup_(sd_bus_message_unrefp) sd_bus_message *m = NULL, *reply = NULL;
    _cleanup_(sd_bus_unrefp) sd_bus *bus{nullptr};
    _cleanup_(sd_bus_error_free) sd_bus_error error = SD_BUS_ERROR_NULL;

    assert(sd_bus_new(&bus) >= 0);
    assert(sd_bus_set_fd(bus, m_fds[1], m_fds[1]) >= 0);
    assert(sd_bus_negotiate_fds(bus, true) >= 0);
    assert(sd_bus_set_anonymous(bus, true) >= 0);
    assert(sd_bus_start(bus) >= 0);

    logWarning("client bus is connected");

    // int sd_bus_message_new_method_call(	sd_bus *bus,
    //  	sd_bus_message **m,
    //  	const char *destination,
    //  	const char *path,
    //  	const char *interface,
    //  	const char *member);

    r = sd_bus_message_new_method_call(bus,
                                       &m,
                                       "org.freedesktop.systemd.test",
                                       "/",
                                       "org.freedesktop.systemd.test",
                                       "Exit");
    if (r < 0)
    {
      logSysError(-r, "Failed to allocate method call: %m");
      return;
    }

    logWarning("client call");

    r = sd_bus_call(bus, m, 0, &error, &reply);
    if (r < 0)
    {
      logSysError(-r, "Failed to issue method call:");

      // logSysError(-r,
      //     "Failed to issue method call: %s",
      //     bus_error_message(&error, -r));
      return;
    }
  });

  {
    const sd_bus_error error{SD_BUS_ERROR_UNKNOWN_METHOD, "Unknown method."};
    bool quit = false;

    while (!quit)
    {
      _cleanup_(sd_bus_message_unrefp) sd_bus_message *m = NULL, *reply = NULL;

      r = sd_bus_process(m_bus, &m);
      if (r < 0)
      {
        // log_error_errno(r, "Failed to process requests: %m");
        logSysError(-r, "failed to process requests");
        break;
        // goto fail;
      }

      if (r == 0)
      {
        r = sd_bus_wait(m_bus, (uint64_t)-1);
        if (r < 0)
        {
          // log_error_errno(r, "Failed to wait: %m");
          logSysError(-r, "Failed to wait: %m");
          break;
          // goto fail;
        }

        continue;
      }

      if (!m)
        continue;

      // log_info("Got message! member=%s", strna(sd_bus_message_get_member(m)));
      logWarning("Got message! member=%s", strna(sd_bus_message_get_member(m)));

      // sd_bus_message_is_method_call() checks if message m is a method call
      // message. If interface is non-null, it also checks if the message has the
      // same interface set. If member is non-null, it also checks if the message
      // has the same member set. Also see sd_bus_set_message_new_method_call(3).
      // It returns true when all checks pass.

      if (sd_bus_message_is_method_call(m,
                                        "org.freedesktop.systemd.test",
                                        "Exit"))
      {
        assert((sd_bus_can_send(m_bus, 'h') >= 1) == 1);

        r = sd_bus_message_new_method_return(m, &reply);
        if (r < 0)
        {
          // log_error_errno(r, "Failed to allocate return: %m");
          logSysError(-r, "Failed to allocate return: %m");
          // goto fail;
          break;
        }

        quit = true;
      }
      else if (sd_bus_message_is_method_call(m, NULL, NULL))
      {
        // int sd_bus_message_new_method_error(sd_bus_message *call, sd_bus_message **m, const sd_bus_error *e);
        // r = sd_bus_message_new_method_error(
        //                 m,
        //                 &reply,
        //                 &SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_UNKNOWN_METHOD, "Unknown method."));
        r = sd_bus_message_new_method_error(m, &reply, &error);
        if (r < 0)
        {
          // log_error_errno(r, "Failed to allocate return: %m");
          logSysError(-r, "Failed to allocate return: %m");
          // goto fail;
          break;
        }
      }

      if (reply)
      {
        r = sd_bus_send(m_bus, reply, NULL);
        if (r < 0)
        {
          // log_error_errno(r, "Failed to send reply: %m");
          logSysError(-r, "Failed to send reply: %m");
          // goto fail;
          break;
        }
      }
    }
  }

  t.get();

  sd_bus_unref(m_bus);
}
#endif

// ={=========================================================================
// uses net.poettering.Calculator from sdbus_server_1.cpp
// however, it didn't call the expected

// [ RUN      ] SdbusServerTest.check_2_2
// 0000073942.107396 WRN: < M:test_sdbus_server.cpp F:SetUp L:255 > SetUp() called
// 0000073942.107512 WRN: < M:test_sdbus_server.cpp F:TestBody L:618 > sdbus is connected
// 0000073942.107563 WRN: < M:test_sdbus_server.cpp F:TestBody L:637 > server mode is set
// 0000073942.107877 WRN: < M:test_sdbus_server.cpp F:operator() L:653 > client bus is connected
// 0000073942.108023 WRN: < M:test_sdbus_server.cpp F:method_multiply L:175 > method multiply is called
//
// 0000073942.108243 WRN: < M:test_sdbus_server.cpp F:TestBody L:708 > Got message! member=Disconnected
//
// 0000073942.108277 ERR: < M:test_sdbus_server.cpp F:TestBody L:685 > failed to process requests (104-Connection reset by peer)
// 0000073942.108309 WRN: < M:test_sdbus_server.cpp F:TearDown L:257 > TearDown() called
// [       OK ] SdbusServerTest.check_2_2 (1 ms)

TEST_F(SdbusServerTest, check_2_2)
{
  int r{};
  sd_bus_slot *slot{nullptr};

  assert(socketpair(AF_UNIX, SOCK_STREAM, 0, m_fds) >= 0);

  assert(sd_id128_randomize(&m_id) >= 0);

  assert(sd_bus_new(&m_bus) >= 0);
  assert(sd_bus_set_fd(m_bus, m_fds[0], m_fds[0]) >= 0);
  assert(sd_bus_set_server(m_bus, 1, m_id) >= 0);
  assert(sd_bus_set_anonymous(m_bus, true) >= 0);
  assert(sd_bus_negotiate_fds(m_bus, true) >= 0);
  assert(sd_bus_start(m_bus) >= 0);

  logWarning("sdbus is connected");

  r = sd_bus_add_object_vtable(m_bus,               // bus
                               &slot,               // slot
                               m_path.c_str(),      // object path
                               m_interface.c_str(), // interface
                               calculator_vtable,   // vtable
                               nullptr);            // user data

  if (r < 0)
  {
    logSysError(-r, "failed to add object");
    return;
  }

  // int sd_bus_is_server(sd_bus *bus);
  r = sd_bus_is_server(m_bus);
  if (r > 0)
  {
    logWarning("server mode is set");
  }

  auto t = std::async([this] {
    int r{};

    _cleanup_(sd_bus_message_unrefp) sd_bus_message *m = NULL, *reply = NULL;
    _cleanup_(sd_bus_unrefp) sd_bus *bus{nullptr};
    _cleanup_(sd_bus_error_free) sd_bus_error error = SD_BUS_ERROR_NULL;

    assert(sd_bus_new(&bus) >= 0);
    assert(sd_bus_set_fd(bus, m_fds[1], m_fds[1]) >= 0);
    assert(sd_bus_negotiate_fds(bus, true) >= 0);
    assert(sd_bus_set_anonymous(bus, true) >= 0);
    assert(sd_bus_start(bus) >= 0);

    logWarning("client bus is connected");

    // issue the method call and store the response message in m
    r = sd_bus_call_method(bus,
                           "net.poettering.Calculator",  // service
                           "/net/poettering/Calculator", // object path
                           "net.poettering.Calculator",  // interface
                           "Multiply",                   // method
                           &error, // object to return error in
                           &m,     // return message on success
                           "xx",   // input signature
                           5,      // first argument
                           7       // second argument
    );
    if (r < 0)
    {
      fprintf(stderr, "failed to issue method call: %s\n", error.message);
    }
  });

  {
    const sd_bus_error error{SD_BUS_ERROR_UNKNOWN_METHOD, "Unknown method."};
    bool quit = false;

    while (!quit)
    {
      _cleanup_(sd_bus_message_unrefp) sd_bus_message *m = NULL, *reply = NULL;

      r = sd_bus_process(m_bus, &m);
      if (r < 0)
      {
        // log_error_errno(r, "Failed to process requests: %m");
        logSysError(-r, "failed to process requests");
        break;
        // goto fail;
      }

      if (r == 0)
      {
        r = sd_bus_wait(m_bus, (uint64_t)-1);
        if (r < 0)
        {
          // log_error_errno(r, "Failed to wait: %m");
          logSysError(-r, "Failed to wait: %m");
          break;
          // goto fail;
        }

        continue;
      }

      if (!m)
        continue;

      // log_info("Got message! member=%s", strna(sd_bus_message_get_member(m)));
      logWarning("Got message! member=%s", strna(sd_bus_message_get_member(m)));

      // sd_bus_message_is_method_call() checks if message m is a method call
      // message. If interface is non-null, it also checks if the message has the
      // same interface set. If member is non-null, it also checks if the message
      // has the same member set. Also see sd_bus_set_message_new_method_call(3).
      // It returns true when all checks pass.

      if (sd_bus_message_is_method_call(m,
                                        "net.poettering.Calculator",
                                        "Multiply"))
      {

        assert((sd_bus_can_send(m_bus, 'h') >= 1) == 1);

        r = sd_bus_message_new_method_return(m, &reply);
        if (r < 0)
        {
          // log_error_errno(r, "Failed to allocate return: %m");
          logSysError(-r, "Failed to allocate return: %m");
          // goto fail;
          break;
        }

        quit = true;
      }
      else if (sd_bus_message_is_method_call(m, NULL, NULL))
      {

        // int sd_bus_message_new_method_error(sd_bus_message *call, sd_bus_message **m, const sd_bus_error *e);
        // r = sd_bus_message_new_method_error(
        //                 m,
        //                 &reply,
        //                 &SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_UNKNOWN_METHOD, "Unknown method."));
        r = sd_bus_message_new_method_error(m, &reply, &error);
        if (r < 0)
        {
          // log_error_errno(r, "Failed to allocate return: %m");
          logSysError(-r, "Failed to allocate return: %m");
          // goto fail;
          break;
        }
      }

      if (reply)
      {
        r = sd_bus_send(m_bus, reply, NULL);
        if (r < 0)
        {
          // log_error_errno(r, "Failed to send reply: %m");
          logSysError(-r, "Failed to send reply: %m");
          // goto fail;
          break;
        }
      }
    }
  }

  t.get();

  sd_bus_unref(m_bus);
}

// ={=========================================================================

#if 0
This code is to show how gtest init done under Qt env.

static void GoogleTestRunner(int argc, char **argv)
{
  // Since Google Mock depends on Google Test, InitGoogleMock() is
  // also responsible for initializing Google Test. Therefore there's
  // no need for calling testing::InitGoogleTest() separately.
  testing::InitGoogleMock(&argc, argv);
  int result = RUN_ALL_TESTS();

  QCoreApplication::exit(result);
}

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  // Disable sigpipe as we run a few tests that deliberately close remote
  // ends of pipes / sockets.
  signal(SIGPIPE, SIG_IGN);

  // This will run the functor from the application event loop.
  QTimer::singleShot(0, &app, std::bind(&GoogleTestRunner, argc, argv));

  return app.exec();
}

so follows the same approach.

#endif

static void GoogleTestRunner(int argc, char **argv)
{
  // Since Google Mock depends on Google Test, InitGoogleMock() is
  // also responsible for initializing Google Test. Therefore there's
  // no need for calling testing::InitGoogleTest() separately.
  //
  // NOTE: what's the benefit?
  // testing::InitGoogleTest(&argc, argv);

  testing::InitGoogleMock(&argc, argv);
  // to supress comple warning since not use return value
  int result = RUN_ALL_TESTS();

  // exit event loop
  loop.quit(0);
}

// Q:
//
// 1. How to pass eventloop to each test since dbus connection need this? make
// it global? yes, it works for now.

int main(int argc, char **argv)
{
  // EventLoop is created at global

  // post function to run on the event loop
  loop.invokeMethod(std::bind(&GoogleTestRunner, argc, argv));

  // run event loop and blocks
  loop.run();
}
