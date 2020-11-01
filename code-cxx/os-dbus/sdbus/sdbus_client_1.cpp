
/*
http://0pointer.net/blog/the-new-sd-bus-api-of-systemd.html

Invoking a Method, from C, with sd-bus

So much about the library in general. Here's an example for connecting to the
bus and issuing a method call:

Return Value
On failure, they return a negative errno-style error code.

https://www.freedesktop.org/software/systemd/man/sd_bus_open_system.html#

*/

#include <stdio.h>
#include <stdlib.h>

#include <systemd/sd-bus.h>

#include "gmock/gmock.h"

using namespace std;
using namespace testing;

/*

Let's see which `objects` the org.freedesktop.login1 service actually offers:

busctl tree org.freedesktop.login1 
└─/org
  └─/org/freedesktop
    └─/org/freedesktop/login1
      ├─/org/freedesktop/login1/seat
      │ ├─/org/freedesktop/login1/seat/seat0
      │ └─/org/freedesktop/login1/seat/self
      ├─/org/freedesktop/login1/session
      │ ├─/org/freedesktop/login1/session/_32
      │ ├─/org/freedesktop/login1/session/c1
      │ └─/org/freedesktop/login1/session/self
      └─/org/freedesktop/login1/user
        ├─/org/freedesktop/login1/user/_1001
        ├─/org/freedesktop/login1/user/_119
        └─/org/freedesktop/login1/user/self

Let's see what interfaces, methods, signals and properties one of these objects
actually exposes:

busctl introspect org.freedesktop.login1 /org/freedesktop/login1/session
NAME                                TYPE      SIGNATURE RESULT/VALUE FLAGS
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


https://www.freedesktop.org/software/systemd/man/busctl.html

Of course, some method call parameters can get a ton more complex, but with
busctl it's relatively easy to encode them all. See the man page for details.

busctl knows a number of other operations. For example, you can use it to
monitor D-Bus traffic as it happens (including generating a .cap file for use
with Wireshark!) or you can set or get specific properties. However, this blog
story was supposed to be about sd-bus, not busctl, hence let's cut this short
here, and let me direct you to the man page in case you want to know more about
the tool.

busctl (like the rest of system) is implemented using the sd-bus API. Thus it
exposes many of the features of sd-bus itself. For example, you can use to
connect to remote or container buses. It understands both kdbus and classic
D-Bus, and more!


$ busctl call org.freedesktop.systemd1 /org/freedesktop/systemd1 org.freedesktop.systemd1.Manager StartUnit ss "cups.service" "replace"
o "/org/freedesktop/systemd1/job/2593"

The specified signature string hence indicates what comes next. systemd's
StartUnit method call takes the unit name to start as first parameter, and the
mode in which to start it as second. The call returned a single object path
value. It is encoded the same way as the input parameter: a signature (just o
for the object path) followed by the actual value.


$ ./sdbus_client
succuess on methond call
queued service job as /org/freedesktop/systemd1/job/2208

*/

// Invoking a Method, from C, with sd-bus
// client side

TEST(SdbusClient, check_1)
{
  sd_bus_error error{SD_BUS_ERROR_NULL};
  sd_bus_message *m{nullptr};
  sd_bus *bus{nullptr};

  const char *path{nullptr};
  int r;

  // connect to the system bus and note that on `strerror(-r)`
  r = sd_bus_open_system(&bus);
  if (r < 0)
  {
    fprintf(stderr, "failed to connect to system bus: %s\n", strerror(-r));
    goto finish;
  }

  // issue the method call and store the response message in m
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
    fprintf(stderr, "failed to issue method call: %s\n", error.message);
    goto finish;
  }

  printf("succuess on methond call\n");

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
  //"o", SD_BUS_TYPE_OBJECT_PATH, object path, const char **

  r = sd_bus_message_read(m, "o", &path);
  if (r < 0)
  {
    fprintf(stderr, "failed to parse reply message: %s\n", strerror(-r));
    goto finish;
  }

  printf("queued service job as %s\n", path);

finish:
  sd_bus_error_free(&error);
  sd_bus_message_unref(m);
  sd_bus_unref(bus);
}

/*
to call server from os-dbus/sdbus/sdbus_server_1.cpp

when have "sleep(30)" in the server

$ dbus-send --session --type=method_call --print-reply --dest='net.poettering.Calculator' /net/poettering/Calculator net.poettering.Calculator.Multiply int64:5 int64:7

Error org.freedesktop.DBus.Error.NoReply: Did not receive a reply. Possible causes include: the remote application did not send a reply, the message bus security policy blocked the reply, the reply timeout expired, or the network connection was broken.


[ RUN      ] SdbusClient.check_2
failed to issue method call: Connection timed out
[       OK ] SdbusClient.check_2 (25003 ms)


https://freedesktop.org/software/systemd/man/sd_bus_call_method_async.html

sd_bus_call_method() is a convenience function for initializing a bus message
object and calling the corresponding D-Bus method. It combines the
sd_bus_message_new_method_call(3), sd_bus_message_append(3) and sd_bus_call(3)
functions into a single function call.

sd_bus_call_method_async() is a convenience function for initializing a bus
message object and calling the corresponding D-Bus method asynchronously. It
combines the sd_bus_message_new_method_call(3), sd_bus_message_append(3) and
sd_bus_call_async(3) functions into a single function call.

*/

TEST(SdbusClient, check_2)
{
  sd_bus_error error{SD_BUS_ERROR_NULL};
  sd_bus_message *m{nullptr};
  sd_bus_message *reply{nullptr};
  sd_bus *bus{nullptr};

  int value{};
  int r{};

  // connect to the system bus and note that on `strerror(-r)`
  r = sd_bus_open_user(&bus);
  if (r < 0)
  {
    fprintf(stderr, "failed to connect to system bus: %s\n", strerror(-r));
    goto finish;
  }

#if 1
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
    goto finish;
  }

#else
  // TODO: get error: failed to issue method call: Invalid argument
  //
  // int sd_bus_message_new_method_call(	sd_bus *bus,
  //  	sd_bus_message **m,
  //  	const char *destination,
  //  	const char *path,
  //  	const char *interface,
  //  	const char *member);

  r = sd_bus_message_new_method_call(bus,
                                     &m,
                                     "net.poettering.Calculator",
                                     "/net/poettering/Calculator",
                                     "net.poettering.Calculator",
                                     "Multiply");
  if (r < 0)
  {
    fprintf(stderr, "failed to allocate method call: %s\n", strerror(-r));
    goto finish;
  }

  // int sd_bus_call(sd_bus *bus, sd_bus_message *m, uint64_t usec, sd_bus_error
  // *ret_error, sd_bus_message **reply);

  r = sd_bus_call(bus, m, 0, &error, &reply);
  if (r < 0)
  {
    fprintf(stderr, "failed to issue method call: %s\n", strerror(-r));
    goto finish;
  }
#endif

  printf("succuess on methond call\n");

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
  //"o", SD_BUS_TYPE_OBJECT_PATH, object path, const char **

  r = sd_bus_message_read(m, "x", &value);
  if (r < 0)
  {
    fprintf(stderr, "failed to parse reply message: %s\n", strerror(-r));
    goto finish;
  }

  printf("returned value %d\n", value);

finish:
  sd_bus_error_free(&error);
  sd_bus_message_unref(m);
  sd_bus_unref(bus);
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
