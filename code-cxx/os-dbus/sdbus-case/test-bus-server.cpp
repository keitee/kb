/* SPDX-License-Identifier: LGPL-2.1+ */

#include <pthread.h>
#include <stdlib.h>

// #include "sd-bus.h"
#include <systemd/sd-bus.h>

// #include "bus-internal.h"
// #include "bus-util.h"
// #include "log.h"

// #include "macro.h"
// #define assert_se(expr) assert_message_se(expr, #expr)
#include <cassert>
#define assert_se(expr) assert(expr)
#define _cleanup_(x) __attribute__((__cleanup__(x)))
#define PTR_TO_INT(p) ((int)((intptr_t)(p)))
#define INT_TO_PTR(u) ((void *)((intptr_t)(u)))

#define memzero(x, l)                                                          \
  ({                                                                           \
    size_t _l_ = (l);                                                          \
    if (_l_ > 0)                                                               \
      memset(x, 0, _l_);                                                       \
  })

#define zero(x) (memzero(&(x), sizeof(x)))

static inline const char *strna(const char *s)
{
  return s ?: "n/a";
}

static inline const char *strerror_safe(int error)
{
  /* 'safe' here does NOT mean thread safety. */
  return strerror(abs(error));
}

const char *bus_error_message(const sd_bus_error *e, int error)
{

  if (e)
  {
    /* Sometimes, the D-Bus server is a little bit too verbose with
                 * its error messages, so let's override them here */
    if (sd_bus_error_has_name(e, SD_BUS_ERROR_ACCESS_DENIED))
      return "Access denied";

    if (e->message)
      return e->message;
  }

  if (error < 0)
    error = -error;

  return strerror_safe(error);
}

#include <cerrno>
#include <sys/socket.h>
#include <sys/un.h> // sockaddr_un

#include "rlog.h"

struct context
{
  int fds[2];

  bool client_negotiate_unix_fds;
  bool server_negotiate_unix_fds;

  bool client_anonymous_auth;
  bool server_anonymous_auth;
};

static void *server(void *p)
{
  struct context *c = (struct context *)p;
  sd_bus *bus       = NULL;
  sd_id128_t id;
  bool quit = false;
  int r;

  // #define SD_BUS_ERROR_MAKE_CONST(name, message) ((const sd_bus_error) {(name), (message), 0})
  const sd_bus_error error{SD_BUS_ERROR_UNKNOWN_METHOD, "Unknown method."};

  assert_se(sd_id128_randomize(&id) >= 0);

  assert_se(sd_bus_new(&bus) >= 0);
  assert_se(sd_bus_set_fd(bus, c->fds[0], c->fds[0]) >= 0);
  assert_se(sd_bus_set_server(bus, 1, id) >= 0);
  assert_se(sd_bus_set_anonymous(bus, c->server_anonymous_auth) >= 0);
  assert_se(sd_bus_negotiate_fds(bus, c->server_negotiate_unix_fds) >= 0);
  assert_se(sd_bus_start(bus) >= 0);

  while (!quit)
  {
    _cleanup_(sd_bus_message_unrefp) sd_bus_message *m = NULL, *reply = NULL;

    r = sd_bus_process(bus, &m);
    if (r < 0)
    {
      // log_error_errno(r, "Failed to process requests: %m");
      logSysError(-r, "failed to process requests");
      goto fail;
    }

    if (r == 0)
    {
      r = sd_bus_wait(bus, (uint64_t)-1);
      if (r < 0)
      {
        // log_error_errno(r, "Failed to wait: %m");
        logSysError(-r, "Failed to wait: %m");
        goto fail;
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

      assert_se((sd_bus_can_send(bus, 'h') >= 1) ==
                (c->server_negotiate_unix_fds && c->client_negotiate_unix_fds));

      r = sd_bus_message_new_method_return(m, &reply);
      if (r < 0)
      {
        // log_error_errno(r, "Failed to allocate return: %m");
        logSysError(-r, "Failed to allocate return: %m");
        goto fail;
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
        goto fail;
      }
    }

    if (reply)
    {
      r = sd_bus_send(bus, reply, NULL);
      if (r < 0)
      {
        // log_error_errno(r, "Failed to send reply: %m");
        logSysError(-r, "Failed to send reply: %m");
        goto fail;
      }
    }
  }

  r = 0;

fail:
  if (bus)
  {
    sd_bus_flush(bus);
    sd_bus_unref(bus);
  }

  return INT_TO_PTR(r);
}

static int client(struct context *c)
{
  _cleanup_(sd_bus_message_unrefp) sd_bus_message *m = NULL, *reply = NULL;
  _cleanup_(sd_bus_unrefp) sd_bus *bus            = NULL;
  _cleanup_(sd_bus_error_free) sd_bus_error error = SD_BUS_ERROR_NULL;
  int r;

  assert_se(sd_bus_new(&bus) >= 0);
  assert_se(sd_bus_set_fd(bus, c->fds[1], c->fds[1]) >= 0);
  assert_se(sd_bus_negotiate_fds(bus, c->client_negotiate_unix_fds) >= 0);
  assert_se(sd_bus_set_anonymous(bus, c->client_anonymous_auth) >= 0);
  assert_se(sd_bus_start(bus) >= 0);

  r = sd_bus_message_new_method_call(bus,
                                     &m,
                                     "org.freedesktop.systemd.test",
                                     "/",
                                     "org.freedesktop.systemd.test",
                                     "Exit");
  if (r < 0)
  {
    // return log_error_errno(r, "Failed to allocate method call: %m");
    logSysError(-r, "Failed to allocate method call: %m");
    return 0;
  }

  r = sd_bus_call(bus, m, 0, &error, &reply);
  if (r < 0)
  {
    // return log_error_errno(r, "Failed to issue method call: %s", bus_error_message(&error, -r));
    logSysError(-r,
                "Failed to issue method call: %s",
                bus_error_message(&error, -r));
    return 0;
  }

  return 0;
}

static int test_one(bool client_negotiate_unix_fds,
                    bool server_negotiate_unix_fds,
                    bool client_anonymous_auth,
                    bool server_anonymous_auth)
{

  struct context c;
  pthread_t s;
  void *p;
  int r, q;

  zero(c);

  assert_se(socketpair(AF_UNIX, SOCK_STREAM, 0, c.fds) >= 0);

  c.client_negotiate_unix_fds = client_negotiate_unix_fds;
  c.server_negotiate_unix_fds = server_negotiate_unix_fds;
  c.client_anonymous_auth     = client_anonymous_auth;
  c.server_anonymous_auth     = server_anonymous_auth;

  r = pthread_create(&s, NULL, server, &c);
  if (r != 0)
    return -r;

  r = client(&c);

  q = pthread_join(s, &p);
  if (q != 0)
    return -q;

  if (r < 0)
    return r;

  if (PTR_TO_INT(p) < 0)
    return PTR_TO_INT(p);

  return 0;
}

int main(int argc, char *argv[])
{
  int r;

  r = test_one(true, true, false, false);
  assert_se(r >= 0);

  r = test_one(true, false, false, false);
  assert_se(r >= 0);

  r = test_one(false, true, false, false);
  assert_se(r >= 0);

  r = test_one(false, false, false, false);
  assert_se(r >= 0);

  r = test_one(true, true, true, true);
  assert_se(r >= 0);

  r = test_one(true, true, false, true);
  assert_se(r >= 0);

  // keitee@kit-ubuntu:~/git/kb/code-cxx/os-dbus/sdbus-case/build$ ./test_bus_server
  // test_bus_server: /home/keitee/git/kb/code-cxx/os-dbus/sdbus-case/test-bus-server.cpp:227: int main(int, char**): Assertion `r == -1' failed.
  // Aborted (core dumped)
  //
  // r = test_one(true, true, true, false);
  // assert_se(r == -EPERM);

  return EXIT_SUCCESS;
}