/*

http://0pointer.net/blog/introducing-sd-event.html


These APIs are implemented as a shared library, which can be compiled and linked
to with the libsystemd pkg-config(1) file.


$ `pkg-config --cflags --libs libsystemd`
Package libsystemd was not found in the pkg-config search path.
Perhaps you should add the directory containing `libsystemd.pc'
to the PKG_CONFIG_PATH environment variable
No package 'libsystemd' found

$ sudo apt-get install libsystemd-dev
[sudo] password for keitee:
Reading package lists... Done

$ pkg-config --cflags --libs libsystemd
-lsystemd


g++ sdevent.cpp -o event-example -lsystemd


Return Value
On failure, they return a negative errno-style error code.


https://www.freedesktop.org/software/systemd/man/sd_event_new.html#

sd_event_new, sd_event_default, sd_event_ref, sd_event_unref, sd_event_unrefp,
sd_event_get_tid, sd_event

— Acquire and release an event loop object

sd_event_default() acquires a reference to the default event loop object of the
calling thread, possibly allocating a new object if no default event loop object
has been allocated yet for the thread. After use, drop the returned reference
with sd_event_unref(). When the last reference is dropped, the event loop is
freed. If this function is called while the object returned from a previous call
from the same thread is still referenced, the same object is returned again, but
the reference is increased by one. 


sd_event_default() and ad_event_new()?

It is recommended to use this call instead of sd_event_new() in order to share
event loop objects between various components that are dispatched in the same
thread. 

All threads have exactly either zero or
one default event loop objects associated, but never more.


https://www.freedesktop.org/software/systemd/man/sd_event_add_signal.html#

sd_event_add_signal, sd_event_source_get_signal, sd_event_signal_handler_t

— Add a UNIX process signal event source to an event loop

int sd_event_add_signal(sd_event *event,
  sd_event_source **source,
  int signal,
  sd_event_signal_handler_t handler,
  void *userdata);


https://www.freedesktop.org/software/systemd/man/sd_event_set_watchdog.html#

sd_event_set_watchdog, sd_event_get_watchdog

— Enable event loop watchdog support

int sd_event_set_watchdog(sd_event *event, int b);


https://www.freedesktop.org/software/systemd/man/sd_event_add_io.html#

typedef int (*sd_event_io_handler_t)(
    sd_event_source *s,
    int fd,
    uint32_t revents,
    void *userdata);


— Add an I/O event source to an event loop

int sd_event_add_io(
  sd_event *event,
  sd_event_source **source,
  int fd,
  uint32_t events,
  sd_event_io_handler_t handler,
  void *userdata);

sd_event_add_io() adds a new I/O event source to an event loop. The event loop
object is specified in the `event parameter`, the event source object is returned
in the `source parameter`. 

The fd parameter takes the UNIX file descriptor to watch, which may refer to a
socket, a FIFO, a message queue, a serial connection, a character device, or any
other file descriptor compatible with Linux epoll(7). 

The events parameter takes a bit mask of events to watch for, a combination of
the following event flags: EPOLLIN, EPOLLOUT, EPOLLRDHUP, EPOLLPRI, and EPOLLET,
    see epoll_ctl(2) for details. 

The handler shall reference a function to call when the event source is
triggered. The userdata pointer will be passed to the handler function, and may
be chosen freely by the caller. The handler will also be passed the file
descriptor the event was seen on, as well as the actual event flags. It's
generally a subset of the events watched, however may additionally include
EPOLLERR and EPOLLHUP.


https://www.freedesktop.org/software/systemd/man/sd_notifyf.html#

sd_notify, sd_notifyf, sd_pid_notify, sd_pid_notifyf, sd_pid_notify_with_fds

— Notify service manager about start-up completion and other service status
changes

sd_notify() may be called by a service to notify the *service manager* about
state changes. It can be used to send arbitrary information, encoded in an
environment-block-like string. Most importantly, it can be used for start-up
completion notification.


int sd_notifyf(int unset_environment,
  const char *format,
  ...);

READY=1

Tells the service manager that service startup is finished, or the service
finished loading its configuration. This is only used by systemd if the service
definition file has Type=notify set. Since there is little value in signaling
non-readiness, the only value services should send is "READY=1" (i.e. "READY=0"
is not defined).

STATUS=...
Passes a single-line UTF-8 status string back to the service manager that
describes the service state. This is free-form and can be used for various
purposes: general state feedback, fsck-like programs could pass completion
percentages and failing programs could pass a human-readable error message.
Example: "STATUS=Completed 66% of file system check…"


https://www.freedesktop.org/software/systemd/man/sd_event_loop.html#

sd_event_run, sd_event_loop
— Run an event loop

sd_event_run() may be used to run a single iteration of the event loop specified
in the event parameter. The function waits until an event to process is
available, and dispatches the registered handler for it. The usec parameter
specifies the maximum time (in microseconds) to wait for an event. Use
(uint64_t) -1 to specify an infinite timeout.

sd_event_loop() invokes sd_event_run() in a loop, thus implementing the actual
event loop. The call returns as soon as exiting was requested using
sd_event_exit(3).

The event loop object event is created with sd_event_new(3). Events sources to
wait for and their handlers may be registered with sd_event_add_io(3),
     sd_event_add_time(3), sd_event_add_signal(3), sd_event_add_child(3),
     sd_event_add_defer(3), sd_event_add_post(3) and sd_event_add_exit(3).


https://www.freedesktop.org/software/systemd/man/sd_event_exit.html

sd_event_exit, sd_event_get_exit_code — Ask the event loop to exit

Synopsis

#include <systemd/sd-event.h>

int sd_event_exit(
  sd_event *event,
  int code);
 
Description

sd_event_exit() requests the event loop specified in the event event loop object
to exit. The code parameter may be any integer value and is returned as-is by
sd_event_loop(3) after the last event loop iteration. It may also be queried
using sd_event_get_exit_code(), see below.


*/

#include <errno.h>
#include <netinet/in.h> // sockaddr_in
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <unistd.h>

#include <systemd/sd-daemon.h>
#include <systemd/sd-event.h>

static int
io_handler(sd_event_source *source, int fd, uint32_t revents, void *user_data)
{
  void *buffer{};
  ssize_t n{};
  int sz{};

  printf("io_handler: revents(0x%x)\n", revents);

  // UDP enforces a somewhat resonalble max datagram size of 64K, we can just
  // allocate the buffer on the stack
  if (ioctl(fd, FIONREAD, &sz) < 0)
    return -errno;

  // alloca - allocate memory that is automatically freed
  // The alloca() function allocates size bytes of space in the stack frame of
  // the caller. This temporary space is automatically freed when the function
  // that called alloca() returns to its caller.
  buffer = alloca(sz);

  n = recv(fd, buffer, sz, 0);
  if (n < 0)
  {
    if (errno == EAGAIN)
      return 0;

    return -errno;
  }

  if (n == 5 && memcmp(buffer, "EXIT\n", 5) == 0)
  {
    sd_event_exit(sd_event_source_get_event(source), 0);
    return 0;
  }

  fwrite(buffer, 1, n, stdout);
  fflush(stdout);
  return 0;
}

/*

The example above shows how to write a minimal UDP/IP server, that listens on
port 7777. Whenever a datagram is received it outputs its contents to STDOUT,
unless it is precisely the string EXIT\n in which case the service exits. The
service will react to SIGTERM and SIGINT and do a clean exit then. It also
notifies the service manager about its completed startup, if it runs under a
service manager. Finally, it sends watchdog keep-alive messages to the service
manager if it asked for that, and if it runs under a service manager.


When run as systemd service this service's STDOUT will be connected to the
logging framework of course, which means the service can act as a minimal
UDP-based remote logging service.

To compile and link this example, save it as event-example.c, then run:

$ gcc event-example.c -o event-example `pkg-config --cflags --libs libsystemd`


For a first test, simply run the resulting binary from the command line, and
test it against the following netcat command line:

$ nc -u localhost 7777

For the sake of brevity error checking is minimal, and in a real-world
application should, of course, be more comprehensive. However, it hopefully gets
the idea across how to write a daemon that reacts to external events with
sd-event.


$ nc -u localhost 7777
opens a udp connection and send what you type to the server. type EXIT to quit.

*/

int main(int argc, char *argvp[])
{
  union
  {
    struct sockaddr_in in;
    struct sockaddr sa;
  } sa;

  sd_event_source *event_source =  nullptr;
  sd_event *event = nullptr;
  int fd{-1};
  int r{};
  sigset_t ss;

  r = sd_event_default(&event);
  if (r < 0)
    goto finish;

  if (sigemptyset(&ss) < 0 ||
      sigaddset(&ss, SIGTERM) < 0 ||
      sigaddset(&ss, SIGINT) < 0)
  {
    // to use the same way to handle errors since sd_event_xxx() return negative
    // errno values.

    r = -errno;
    goto finish;
  }

  // block SIGTERM first, so that the event loop can handle it.
  if (sigprocmask(SIG_BLOCK, &ss, nullptr) < 0)
  {
    r = -errno;
    goto finish;
  }

  // make use of the default handler and "floating" reference features of
  // sd_event_add_signal()
  // NOTE: floatring reference feature??

  r = sd_event_add_signal(event, nullptr, SIGTERM, nullptr, nullptr);
  if (r < 0)
    goto finish;

  r = sd_event_add_signal(event, nullptr, SIGINT, nullptr, nullptr);
  if (r < 0)
    goto finish;

  // enable automatic service watchdog support
  r = sd_event_set_watchdog(event, true);
  if (r < 0)
    goto finish;

  fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0);
  if (fd < 0)
  {
    r = -errno;
    goto finish;
  }

  sa.in = (struct sockaddr_in){
    .sin_family = AF_INET,
    .sin_port   = htobe16(7777),
  };

  if (bind(fd, &sa.sa, sizeof(sa)) < 0)
  {
    r = -errno;
    goto finish;
  }

  r = sd_event_add_io(event,  // event
                      &event_source,  // source
                      fd,   // fd
                      EPOLLIN, // events
                      io_handler,   // handler
                      nullptr);     // user data
  if (r < 0)
    goto finish;

  (void)sd_notify(false,
                  "READY=1\n"
                  "STATUS=Daemon startup completed, processing events.");

  printf("sdevent daemon started\n");

  r = sd_event_loop(event);

  printf("sdevent daemon finished\n");

finish:
  event_source = sd_event_source_unref(event_source);
  event        = sd_event_unref(event);

  if (fd >= 0)
    (void)close(fd);

  if (r < 0)
    fprintf(stderr, "Failure: %s\n", strerror(-r));

  return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

