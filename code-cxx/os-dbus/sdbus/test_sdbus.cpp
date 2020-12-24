// #include <boost/lexical_cast.hpp>
// #include <chrono>
// #include <cstdarg>
// #include <forward_list>
// #include <future>
// #include <iostream>
// #include <limits>
// #include <list>
// #include <memory>
// #include <regex>
// #include <set>
#include <condition_variable>
#include <mutex>
#include <thread>

// #include <vector>

#include <systemd/sd-bus.h>

#include "rlog.h"
#include "gmock/gmock.h"

using namespace std;
using namespace std::placeholders;
using namespace testing;

// ={=========================================================================
// copy of sdbus_client_1.cpp and refer to it for more details
TEST(Sdbus, check_call_and_read_1)
{
  sd_bus_error error{SD_BUS_ERROR_NULL};
  sd_bus_message *m{nullptr};
  sd_bus *bus{nullptr};

  const char *path{nullptr};
  int r{};

  // connect to the system bus
  r = sd_bus_open_system(&bus);
  if (r < 0)
  {
    LOG_SERR(-r, "failed to connect to system bus");
    goto finish;
  }

  // issue the method call and store the reply message in m
  r = sd_bus_call_method(bus,
                         "org.freedesktop.systemd1",         // service
                         "/org/freedesktop/systemd1",        // object path
                         "org.freedesktop.systemd1.Manager", // interface
                         "StartUnit",                        // method
                         &error,         // object to return error in
                         &m,             // return message on success
                         "ss",           // input signature
                         "cups.service", // first argument
                         "replace"       // second argument
  );
  if (r < 0)
  {
    LOG_SERR(-r, "failed to issue method call");
    goto finish;
  }

  // success on method call
  //
  // Return Value
  // On success, sd_bus_new() returns 0 or a positive integer. On failure, it
  // returns a negative errno-style error code.

  EXPECT_THAT(r, 1);

  // parse the response message
  //
  // https://www.freedesktop.org/software/systemd/man/sd_bus_message_read.html#
  //
  // sd_bus_message_read() reads a sequence of fields from the D-Bus message
  // object m and advances the read position in the message. The type string
  // types describes the types of items expected in the message and the field
  // arguments that follow. The type string may be NULL or empty, in which case
  // nothing is read.
  //
  // "o", SD_BUS_TYPE_OBJECT_PATH, object path, const char **

  r = sd_bus_message_read(m, "o", &path);
  if (r < 0)
  {
    LOG_SERR(-r, "failed to parse reply message");
    goto finish;
  }

  std::cout << "queued service job as " << path << std::endl;

finish:
  sd_bus_error_free(&error);
  sd_bus_message_unref(m);
  sd_bus_unref(bus);
}

// ={=========================================================================
// refer to listnames.cpp and show to parse(demarshall) message using sd-bus

TEST(Sdbus, check_call_and_read_2)
{
  sd_bus_error error{SD_BUS_ERROR_NULL};
  sd_bus_message *m{nullptr};
  sd_bus *bus{nullptr};

  const char *path{nullptr};
  int r{};

  std::vector<std::string> names;

  // connect to the user bus
  r = sd_bus_open_user(&bus);
  if (r < 0)
  {
    LOG_SERR(-r, "failed to connect to system bus");
    goto finish;
  }

  // issue the method call and store the reply message in m

  // int sd_bus_call_method(
  //  sd_bus *bus, const char *destination, const char *path, const char *interface,
  //  const char *member, sd_bus_error *ret_error, sd_bus_message **reply,
  //  const char *types, ...);
  //
  // int sd_bus_call_method_async(
  //  sd_bus *bus, sd_bus_slot **slot, const char *destination, const char *path, const char *interface,
  //  const char *member, sd_bus_message_handler_t callback, void *userdata,
  //  const char *types, ...);

  r = sd_bus_call_method(bus,
                         "org.freedesktop.DBus",  // service
                         "/org/freedesktop/DBus", // object path
                         "org.freedesktop.DBus",  // interface
                         "ListNames",             // method
                         &error,                  // object to return error in
                         &m,                      // return message on success
                         nullptr);
  if (r < 0)
  {
    LOG_SERR(-r, "failed to issue method call");
    goto finish;
  }

  // success on method call
  //
  // Return Value
  // On success, sd_bus_new() returns 0 or a positive integer. On failure, it
  // returns a negative errno-style error code.

  EXPECT_THAT(r, 1);

  // "LisNames" returns "as"

  // _public_ int sd_bus_message_has_signature(sd_bus_message *m, const char *signature) {
  //         assert_return(m, -EINVAL);
  //
  //         return streq(strempty(m->root_container.signature), strempty(signature));
  // }
  r = sd_bus_message_has_signature(m, "as");
  if (r < 0)
  {
    LOG_SERR(-r, "failed to match signature");
    goto finish;
  }

  EXPECT_THAT(r, Not(0));

  // _public_ int sd_bus_message_enter_container(sd_bus_message *m,
  //                                             char type,
  //                                             const char *contents);
  //
  r = sd_bus_message_enter_container(m, SD_BUS_TYPE_ARRAY, nullptr);
  if (r < 0)
  {
    LOG_SERR(-r, "failed to enter container");
    goto finish;
  }

  // returns true if we're at the end of the current container.
  // _public_ int sd_bus_message_at_end(sd_bus_message *m, int complete);
  //
  //  bool atEnd()
  //  { return (sd_bus_message_at_end(mMessage, false) != 0); }
  //
  // so sd_bus_message_at_end() returns 1(true?) when reaches the end

  while (0 == sd_bus_message_at_end(m, false))
  {
    const char *str{nullptr};

    // _public_ int sd_bus_message_read_basic(sd_bus_message *m, char type, void *p);
    r = sd_bus_message_read_basic(m, SD_BUS_TYPE_STRING, &str);
    if ((r < 0) || !str)
    {
      LOG_SERR(-r, "failed to read message");
      goto finish;
    }

    names.emplace_back(std::string(str));
  } // while

  // ok, reading is done and close it
  // _public_ int sd_bus_message_close_container(sd_bus_message *m);
  r = sd_bus_message_exit_container(m);
  if (r < 0)
  {
    LOG_SERR(-r, "failed to exit container");
    goto finish;
  }

  // ok, let see what we've got back from the call
  for (const auto e : names)
    std::cout << "names: " << e << std::endl;

finish:
  sd_bus_error_free(&error);
  sd_bus_message_unref(m);
  sd_bus_unref(bus);
}

// ={=========================================================================
// same as client_2 but use sd_dbus_call()
//
// This means that any synchronous remote operation (such as sd_bus_call(3),
// sd_bus_add_match(3) or sd_bus_request_name(3)),
//
// int sd_bus_call(
//  sd_bus *bus,
//  sd_bus_message *m,
//  uint64_t usec,
//  sd_bus_error *ret_error,
//  sd_bus_message **reply);

TEST(Sdbus, check_call_and_read_3)
{
  sd_bus_error error{SD_BUS_ERROR_NULL};
  sd_bus_message *m{nullptr};
  sd_bus *bus{nullptr};

  const char *path{nullptr};
  int r{};

  // timeout in us (micro sec)
  // #define DBUS_DEFAULT_TIMEOUT_USEC    (25 * 1000 * 1000)
  uint64_t timeout{25 * 1000 * 1000};
  sd_bus_message *reply{nullptr};

  std::vector<std::string> names;

  uint64_t cookie = 0;

  // connect to the user bus
  r = sd_bus_open_user(&bus);
  if (r < 0)
  {
    LOG_SERR(-r, "failed to connect to system bus");
    goto finish;
  }

  // issue the method call and store the reply message in m

  // int sd_bus_call_method(
  //  sd_bus *bus, const char *destination, const char *path, const char *interface,
  //  const char *member, sd_bus_error *ret_error, sd_bus_message **reply,
  //  const char *types, ...);
  //
  // int sd_bus_call_method_async(
  //  sd_bus *bus, sd_bus_slot **slot, const char *destination, const char *path, const char *interface,
  //  const char *member, sd_bus_message_handler_t callback, void *userdata,
  //  const char *types, ...);
  //
  // r = sd_bus_call_method(bus,
  //                        "org.freedesktop.DBus",  // service
  //                        "/org/freedesktop/DBus", // object path
  //                        "org.freedesktop.DBus",  // interface
  //                        "ListNames",             // method
  //                        &error,                  // object to return error in
  //                        &m,                      // return message on success
  //                        nullptr);
  // if (r < 0)
  // {
  //   LOG_SERR(-r, "failed to issue method call");
  //   goto finish;
  // }

  r = sd_bus_message_new_method_call(bus,
                                     &m,
                                     "org.freedesktop.DBus",  // service
                                     "/org/freedesktop/DBus", // object path
                                     "org.freedesktop.DBus",  // interface
                                     "ListNames");            // method
  if (r < 0)
  {
    LOG_SERR(-r, "failed to issue method call");
    goto finish;
  }

  // make the call

  r = sd_bus_call(bus, m, timeout, &error, &reply);

  // success on method call
  //
  // Return Value
  // On success, sd_bus_new() returns 0 or a positive integer. On failure, it
  // returns a negative errno-style error code.

  EXPECT_THAT(r, 1);

  // sd_bus_message_get_cookie() returns the transaction cookie of a message.
  // The cookie uniquely identifies a message within each bus peer, but is not
  // globally unique. It is assigned when a message is sent.
  r = sd_bus_message_get_cookie(m, &cookie);
  if (r < 0)
  {
    LOG_SERR(-r, "failed to get request message cookie");
  }

  std::cout << "cookie: " << cookie << std::endl;

  // "LisNames" returns "as"

  // _public_ int sd_bus_message_has_signature(sd_bus_message *m, const char *signature) {
  //         assert_return(m, -EINVAL);
  //
  //         return streq(strempty(m->root_container.signature), strempty(signature));
  // }
  r = sd_bus_message_has_signature(reply, "as");
  if (r < 0)
  {
    LOG_SERR(-r, "failed to match signature");
    goto finish;
  }

  EXPECT_THAT(r, Not(0));

  // _public_ int sd_bus_message_enter_container(sd_bus_message *m,
  //                                             char type,
  //                                             const char *contents);
  //
  r = sd_bus_message_enter_container(reply, SD_BUS_TYPE_ARRAY, nullptr);
  if (r < 0)
  {
    LOG_SERR(-r, "failed to enter container");
    goto finish;
  }

  // returns true if we're at the end of the current container.
  // _public_ int sd_bus_message_at_end(sd_bus_message *m, int complete);
  //
  //  bool atEnd()
  //  { return (sd_bus_message_at_end(mMessage, false) != 0); }
  //
  // so sd_bus_message_at_end() returns 1(true?) when reaches the end

  while (0 == sd_bus_message_at_end(reply, false))
  {
    const char *str{nullptr};

    // _public_ int sd_bus_message_read_basic(sd_bus_message *m, char type, void *p);
    r = sd_bus_message_read_basic(reply, SD_BUS_TYPE_STRING, &str);
    if ((r < 0) || !str)
    {
      LOG_SERR(-r, "failed to read message");
      goto finish;
    }

    names.emplace_back(std::string(str));
  } // while

  // ok, reading is done and close it
  // _public_ int sd_bus_message_close_container(sd_bus_message *m);
  r = sd_bus_message_exit_container(reply);
  if (r < 0)
  {
    LOG_SERR(-r, "failed to exit container");
    goto finish;
  }

  // ok, let see what we've got back from the call
  for (const auto e : names)
    std::cout << "names: " << e << std::endl;

finish:
  sd_bus_error_free(&error);
  sd_bus_message_unref(m);
  sd_bus_message_unref(reply);
  sd_bus_unref(bus);
}

// ={=========================================================================
// same as client_3 but use sd_dbus_call_async()
//
// int sd_bus_call_async(
//  sd_bus *bus,
//  sd_bus_slot **slot,
//  sd_bus_message *m,
//  sd_bus_message_handler_t callback,
//  void *userdata,
//  uint64_t usec);

namespace
{
  // to have m and cv in a single object
  struct message_pack
  {
    std::mutex m;
    std::condition_variable cv;
  };

  // typedef int (*sd_bus_message_handler_t)(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
  int message_handler(sd_bus_message *reply,
                      void *userdata,
                      sd_bus_error *ret_error)
  {
    struct message_pack *pack =
      reinterpret_cast<struct message_pack *>(userdata);

    std::vector<std::string> names;

    uint64_t cookie{};

    int r = sd_bus_message_get_reply_cookie(reply, &cookie);
    if (r < 0)
    {
      LOG_SERR(-r, "failed to get request message cookie");
    }

    std::cout << "reply cookie: " << cookie << std::endl;

    // "LisNames" returns "as"

    // _public_ int sd_bus_message_has_signature(sd_bus_message *m, const char *signature) {
    //         assert_return(m, -EINVAL);
    //
    //         return streq(strempty(m->root_container.signature), strempty(signature));
    // }
    r = sd_bus_message_has_signature(reply, "as");
    if (r < 0)
    {
      LOG_SERR(-r, "failed to match signature");
    }

    EXPECT_THAT(r, Not(0));

    // _public_ int sd_bus_message_enter_container(sd_bus_message *m,
    //                                             char type,
    //                                             const char *contents);
    //
    r = sd_bus_message_enter_container(reply, SD_BUS_TYPE_ARRAY, nullptr);
    if (r < 0)
    {
      LOG_SERR(-r, "failed to enter container");
    }

    // returns true if we're at the end of the current container.
    // _public_ int sd_bus_message_at_end(sd_bus_message *m, int complete);
    //
    //  bool atEnd()
    //  { return (sd_bus_message_at_end(mMessage, false) != 0); }
    //
    // so sd_bus_message_at_end() returns 1(true?) when reaches the end

    while (0 == sd_bus_message_at_end(reply, false))
    {
      const char *str{nullptr};

      // _public_ int sd_bus_message_read_basic(sd_bus_message *m, char type, void *p);
      r = sd_bus_message_read_basic(reply, SD_BUS_TYPE_STRING, &str);
      if ((r < 0) || !str)
      {
        LOG_SERR(-r, "failed to read message");
      }

      names.emplace_back(std::string(str));
    } // while

    // ok, reading is done and close it
    // _public_ int sd_bus_message_close_container(sd_bus_message *m);
    r = sd_bus_message_exit_container(reply);
    if (r < 0)
    {
      LOG_SERR(-r, "failed to exit container");
    }

    // ok, let see what we've got back from the call
    for (const auto e : names)
      std::cout << "names: " << e << std::endl;

    {
      std::lock_guard<std::mutex> lock(pack->m);
      pack->cv.notify_one();
    }
  }
} // namespace

// ={=========================================================================
// NOTE: do not work as expected since there is no event loop running to call
// back `handler`
TEST(DISABLED_Sdbus, sdbus_client_4)
{
  sd_bus_error error{SD_BUS_ERROR_NULL};
  sd_bus_message *m{nullptr};
  sd_bus *bus{nullptr};

  const char *path{nullptr};
  int r{};

  // timeout in us (micro sec)
  // #define DBUS_DEFAULT_TIMEOUT_USEC    (25 * 1000 * 1000)
  uint64_t timeout{25 * 1000 * 1000};

  uint64_t cookie = 0;

  // condition variable, mutex in the pack
  message_pack pack;

  // connect to the user bus
  r = sd_bus_open_user(&bus);
  if (r < 0)
  {
    LOG_SERR(-r, "failed to connect to system bus");
    goto finish;
  }

  r = sd_bus_message_new_method_call(bus,
                                     &m,
                                     "org.freedesktop.DBus",  // service
                                     "/org/freedesktop/DBus", // object path
                                     "org.freedesktop.DBus",  // interface
                                     "ListNames");            // method
  if (r < 0)
  {
    LOG_SERR(-r, "failed to issue method call");
    goto finish;
  }

  // make the call

  r = sd_bus_call_async(bus,
                        nullptr,
                        m,
                        message_handler, // handler
                        &pack,           // pass pack
                        timeout);

  // success on method call
  //
  // Return Value
  // On success, sd_bus_new() returns 0 or a positive integer. On failure, it
  // returns a negative errno-style error code.

  EXPECT_THAT(r, 1);

  // sd_bus_message_get_cookie() returns the transaction cookie of a message.
  // The cookie uniquely identifies a message within each bus peer, but is not
  // globally unique. It is assigned when a message is sent.
  r = sd_bus_message_get_cookie(m, &cookie);
  if (r < 0)
  {
    LOG_SERR(-r, "failed to get request message cookie");
  }

  std::cout << "call cookie: " << cookie << std::endl;

  // move the code to read the reply to the handler

  // wait for cv to be signaled within timeout value
  {
    std::unique_lock<std::mutex> lock(pack.m);
    EXPECT_THAT(pack.cv.wait_for(lock, std::chrono::seconds(5)),
                std::cv_status::no_timeout);
  }

finish:
  sd_bus_error_free(&error);
  sd_bus_message_unref(m);
  sd_bus_unref(bus);
}

// systemd/sd-bus.h
// int sd_bus_send(sd_bus *bus, sd_bus_message *m, uint64_t *cookie);
// int sd_bus_send_to(sd_bus *bus, sd_bus_message *m, const char *destination, uint64_t *cookie);
//
// sd_bus_send.xml
// sd_bus_send() queues the bus message object m for transfer. If bus is NULL,
// the bus that m is attached to is used. bus only needs to be set when the
// message is sent to a different bus than the one it's attached to, for example
// when forwarding messages.
//
// If the output parameter cookie is not NULL, it is set to the message
// identifier. This value can later be used to match incoming replies to their
// corresponding messages. If cookie is set to NULL and the message is not
// sealed, sd_bus_send() assumes the message m doesn't expect a reply and adds
// the necessary headers to indicate this.

// Note that in most scenarios, sd_bus_send() should not be called directly.
// Instead, use higher level functions such as sd_bus_call_method and
// sd_bus_reply_method_return which call sd_bus_send() internally.

// ={=========================================================================
TEST(Sdbus, check_send) {}

// int sd_bus_is_server(sd_bus *bus);
//
// sd_bus_is_server() returns whether the server mode is enabled for the given
// bus object.

// sd_bus_get_bus_id() stores the D-Bus server id configured using
// sd_bus_set_server() (for server bus objects) or received during D-Bus
// authentication (for client bus objects) in id.

// int sd_bus_set_bus_client(sd_bus *bus, int b);
//
// sd_bus_set_bus_client() configures the bus object as a D-Bus daemon client.
// b enables/disables the client mode. If zero, the client mode is disabled and
// the bus object should connect directly to a D-Bus server. Otherwise, the
// client mode is enabled and the bus object should connect to a D-Bus daemon.
//
// When connecting to an existing bus using any of the functions in the
// sd_bus_open family of functions or any of the functions in the sd_bus_default
// family of functions, the bus object is automatically configured as a bus
// client. However, when connecting to a D-Bus daemon by calling
// sd_bus_set_address followed by sd_bus_start, the bus object should be
// manually configured as a bus client using sd_bus_set_bus_client(). By
// default, a bus object is not configured as a D-Bus daemon client.

// int sd_bus_is_bus_client(sd_bus *bus);
//
// sd_bus_is_bus_client() returns whether the client mode is enabled/disabled
// for the given bus object.
//
// sd_bus_is_server(), sd_bus_is_bus_client() and sd_bus_is_monitor() return a
// positive integer when the server or client mode is enabled, respectively.
// Otherwise, they return zero.

// sd_bus_set_monitor() configures the bus object as a D-Bus monitor object.  b
// enables/disables the monitor mode. If zero, the monitor mode is disabled. If
// non-zero, the monitor mode is enabled. When the monitor mode is enabled, no
// messages may be sent via the bus object and it may not expose any objects on
// the bus. To start monitoring messages, call the
// org.freedesktop.DBus.Monitoring.BecomeMonitor method of the D-Bus daemon and
// pass a list of matches indicating which messages to intercept. See <ulink
// url="https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-become-monitor">
// The D-Bus specification</ulink> for more information.

// sd_bus_is_monitor() returns whether the monitor mode is enabled/disabled for
// the given bus object.

// same as TEST(Sdbus, check_call_and_read_1) and add client/server mode check.
//
// [ RUN      ] Sdbus.check_client_and_server_1
// client mode is set
// queued service job as /org/freedesktop/systemd1/job/8447
// [       OK ] Sdbus.check_client_and_server_1 (6023 ms)

// ={=========================================================================
TEST(Sdbus, check_client_and_server_1)
{
  sd_bus_error error{SD_BUS_ERROR_NULL};
  sd_bus_message *m{nullptr};
  sd_bus *bus{nullptr};

  const char *path{nullptr};
  int r{};

  // connect to the system bus
  r = sd_bus_open_system(&bus);
  if (r < 0)
  {
    LOG_SERR(-r, "failed to connect to system bus");
    goto finish;
  }

  // int sd_bus_is_server(sd_bus *bus);
  r = sd_bus_is_server(bus);
  if (r > 0)
  {
    std::cout << "server mode is set" << std::endl;
  }

  // int sd_bus_is_bus_client(sd_bus *bus);
  r = sd_bus_is_bus_client(bus);
  if (r > 0)
  {
    std::cout << "client mode is set, r:" << r << std::endl;
  }

  // issue the method call and store the reply message in m
  r = sd_bus_call_method(bus,
                         "org.freedesktop.systemd1",         // service
                         "/org/freedesktop/systemd1",        // object path
                         "org.freedesktop.systemd1.Manager", // interface
                         "StartUnit",                        // method
                         &error,         // object to return error in
                         &m,             // return message on success
                         "ss",           // input signature
                         "cups.service", // first argument
                         "replace"       // second argument
  );
  if (r < 0)
  {
    LOG_SERR(-r, "failed to issue method call");
    goto finish;
  }

  // success on method call
  //
  // Return Value
  // On success, sd_bus_new() returns 0 or a positive integer. On failure, it
  // returns a negative errno-style error code.

  EXPECT_THAT(r, 1);

  // parse the response message
  //
  // https://www.freedesktop.org/software/systemd/man/sd_bus_message_read.html#
  //
  // sd_bus_message_read() reads a sequence of fields from the D-Bus message
  // object m and advances the read position in the message. The type string
  // types describes the types of items expected in the message and the field
  // arguments that follow. The type string may be NULL or empty, in which case
  // nothing is read.
  //
  // "o", SD_BUS_TYPE_OBJECT_PATH, object path, const char **

  r = sd_bus_message_read(m, "o", &path);
  if (r < 0)
  {
    LOG_SERR(-r, "failed to parse reply message");
    goto finish;
  }

  std::cout << "queued service job as " << path << std::endl;

finish:
  sd_bus_error_free(&error);
  sd_bus_message_unref(m);
  sd_bus_unref(bus);
}

// ={=========================================================================

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
