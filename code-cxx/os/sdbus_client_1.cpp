
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

/*
//
// $ busctl introspect org.freedesktop.systemd1 /org/freedesktop/systemd1
//
// NAME                                TYPE      SIGNATURE        RESULT/VALUE
FLAGS
// org.freedesktop.DBus.Introspectable interface -                - -
// .Introspect                         method    -                s -
// org.freedesktop.DBus.Peer           interface -                - -
// .GetMachineId                       method    -                s -
// .Ping                               method    -                - -
// org.freedesktop.DBus.Properties     interface -                - -
// .Get                                method    ss               v -
// .GetAll                             method    s                a{sv} -
// .Set                                method    ssv              - -
// .PropertiesChanged                  signal    sa{sv}as         - -
// org.freedesktop.systemd1.Manager    interface -                - -
// .StartUnit                          method    ss               o -


keitee@kit-hdebi:~/git/kb/code-cxx/os/build$ ./sdbus_client
succuess on methond call
queued service job as /org/freedesktop/systemd1/job/2208


*/

int main(int argc, char *argvp[])
{
  sd_bus_error error{SD_BUS_ERROR_NULL};
  sd_bus_message *m{nullptr};
  sd_bus *bus{nullptr};

  const char *path{nullptr};
  int r;

  // connect to the system bus
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
  r = sd_bus_message_read(m, "o", &path);
  if (r < 0)
  {
    fprintf(stderr, "failed to parse response: %s\n", strerror(-r));
    goto finish;
  }

  printf("queued service job as %s\n", path);

finish:
  sd_bus_error_free(&error);
  sd_bus_message_unref(m);
  sd_bus_unref(bus);

  return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

