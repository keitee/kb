
/*

http://0pointer.net/blog/the-new-sd-bus-api-of-systemd.html

Implementing a Service, in C, with sd-bus

Return Value
On failure, they return a negative errno-style error code.

https://www.freedesktop.org/software/systemd/man/sd_bus_open_system.html#

$lsdbus

18132 :1.89                    ./a.out
18132 net.poettering.Calculator./a.out
18233 :1.91                    gvim lsdbus.sh

$ busctl --user tree net.poettering.Calculator
└─/net
  └─/net/poettering
    └─/net/poettering/Calculator

As we can see, there's only a single object on the service, which is not
surprising, given that our code above only registered one. Let's see the
interfaces and the members this object exposes:


$ busctl --user introspect net.poettering.Calculator /net/poettering/Calculator

NAME                                TYPE      SIGNATURE RESULT/VALUE FLAGS
net.poettering.Calculator           interface -         -            -
.Divide                             method    xx        x            -
.Multiply                           method    xx        x            -
org.freedesktop.DBus.Introspectable interface -         -            -
.Introspect                         method    -         s            -
org.freedesktop.DBus.Peer           interface -         -            -
.GetMachineId                       method    -         s            -
.Ping                               method    -         -            -
org.freedesktop.DBus.Properties     interface -         -            -
.Get                                method    ss        v            -
.GetAll                             method    s         a{sv}        -
.Set                                method    ssv       -            -
.PropertiesChanged                  signal    sa{sv}as  -            -


The sd-bus library automatically added a couple of generic interfaces, as
mentioned above. But the first interface we see is actually the one we added! It
shows our two methods, and both take "xx" (two 64bit signed integers) as input
parameters, and return one "x". 

busctl --user call net.poettering.Calculator /net/poettering/Calculator net.poettering.Calculator Multiply xx 5 7
x 35

busctl --user call net.poettering.Calculator /net/poettering/Calculator net.poettering.Calculator Divide xx 99 17
x 5

busctl --user call net.poettering.Calculator /net/poettering/Calculator net.poettering.Calculator Divide xx 99 0
sorry, can't do that

*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <systemd/sd-bus.h>
#include <systemd/sd-bus-vtable.h>


// https://www.freedesktop.org/software/systemd/man/sd_bus_message_read.html#
//
// int sd_bus_message_read(
//  sd_bus_message *m,
//  char char *types,
//  ...);

static int method_multiply(sd_bus_message *m, void *data, sd_bus_error *error)
{
  int64_t x;
  int64_t y;
  int r;

  // reads the parameters
  r = sd_bus_message_read(m, "xx", &x, &y);
  if (r < 0)
  {
    fprintf(stderr, "failed to parse parameters: %s\n", strerror(-r));
    return r;
  }

  // reply with the result
  return sd_bus_reply_method_return(m, "x", x * y);
}

static int method_divide(sd_bus_message *m, void *data, sd_bus_error *error)
{
  int64_t lhs;
  int64_t rhs;
  int r;

  // reads the parameters
  r = sd_bus_message_read(m, "xx", &lhs, &rhs);
  if (r < 0)
  {
    fprintf(stderr, "failed to parse parameters: %s\n", strerror(-r));
    return r;
  }

  // return an error on division by zero
  if (rhs == 0)
  {
    sd_bus_error_set_const(error,
                           "net.poettering.DivisionByZero",
                           "sorry, can't do that");
    return -EINVAL;
  }

  // reply with the result
  return sd_bus_reply_method_return(m, "x", lhs / rhs);
}

// The vtable of our little object, implements the net.poettering.Calculator
// interface
// https://www.freedesktop.org/software/systemd/man/SD_BUS_METHOD.html#
//
// SD_BUS_METHOD( member, signature, result, handler, flags)
//
// x           64-bit signed integer

// NOTE:
// In ISO C99 you can give the elements `in any order`, specifying the array
// indices or structure field names they apply to, and GNU C allows this as an
// extension in C90 mode as well. `This extension is not implemented in GNU C++`
//
// SO have to use `.c` but not `.cpp` to build.

static const sd_bus_vtable calculator_vtable[] = {
  SD_BUS_VTABLE_START(0),
  SD_BUS_METHOD("Multiply", "xx", "x", method_multiply, SD_BUS_VTABLE_UNPRIVILEGED),
  SD_BUS_METHOD("Divide", "xx", "x", method_divide, SD_BUS_VTABLE_UNPRIVILEGED),
  SD_BUS_VTABLE_END
};

int main(int argc, char *argvp[])
{
  sd_bus_slot *slot = NULL;
  sd_bus *bus = NULL;

  const char *path = NULL;
  int r;

  // // connect to the system bus
  // r = sd_bus_open_system(&bus);
  // if (r < 0)
  // {
  //   fprintf(stderr, "failed to connect to system bus: %s\n", strerror(-r));
  //   goto finish;
  // }

  // connect to the user bus
  r = sd_bus_open_user(&bus);
  if (r < 0)
  {
    fprintf(stderr, "failed to connect to user bus: %s\n", strerror(-r));
    goto finish;
  }

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

  r = sd_bus_add_object_vtable(bus,                           // bus
                               &slot,                         // slot
                               "/net/poettering/Calculator",  // object path
                               "net.poettering.Calculator",   // interface
                               calculator_vtable,             // vtable
                               NULL);                         // user data

  if (r < 0)
  {
    fprintf(stderr, "failed to add object: %s\n", strerror(-r));
    goto finish;
  }

  printf("succuess on adding object\n");

  // take a well-known service name so that clients can find us
  r = sd_bus_request_name(bus, "net.poettering.Calculator", 0);
  if (r < 0)
  {
    fprintf(stderr, "failed to acquire service name: %s\n", strerror(-r));
    goto finish;
  }

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

finish:
  printf("server finishes\n");

  sd_bus_slot_unref(slot);
  sd_bus_unref(bus);

  return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

