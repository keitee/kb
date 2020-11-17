
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


       tree [SERVICE...]
           Shows an object tree of one or more services. If SERVICE is
           specified, show object tree of the specified services only.
           Otherwise, show all object trees of all services on the bus that
           acquired at least one well-known name.

$ busctl --user tree net.poettering.Calculator
└─/net
  └─/net/poettering
    └─/net/poettering/Calculator

As we can see, there's only a single object on the service, which is not
surprising, given that our code above only registered one. Let's see the
interfaces and the members this object exposes:


dbus-send --session --type=method_call --print-reply --dest='net.poettering.Calculator' / org.freedesktop.DBus.Introspectable.Introspect

method return time=1603105453.441741 sender=:1.4291 -> destination=:1.4294 serial=5 reply_serial=2
   string "<!DOCTYPE node PUBLIC "-//freedesktop//DTD D-BUS Object Introspection 1.0//EN"
"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd">
<node>
 <interface name="org.freedesktop.DBus.Peer">
  <method name="Ping"/>
  <method name="GetMachineId">
   <arg type="s" name="machine_uuid" direction="out"/>
  </method>
 </interface>
 <interface name="org.freedesktop.DBus.Introspectable">
  <method name="Introspect">
   <arg name="data" type="s" direction="out"/>
  </method>
 </interface>
 <interface name="org.freedesktop.DBus.Properties">
  <method name="Get">
   <arg name="interface" direction="in" type="s"/>
   <arg name="property" direction="in" type="s"/>
   <arg name="value" direction="out" type="v"/>
  </method>
  <method name="GetAll">
   <arg name="interface" direction="in" type="s"/>
   <arg name="properties" direction="out" type="a{sv}"/>
  </method>
  <method name="Set">
   <arg name="interface" direction="in" type="s"/>
   <arg name="property" direction="in" type="s"/>
   <arg name="value" direction="in" type="v"/>
  </method>
  <signal name="PropertiesChanged">
   <arg type="s" name="interface"/>
   <arg type="a{sv}" name="changed_properties"/>
   <arg type="as" name="invalidated_properties"/>
  </signal>
 </interface>
 <node name="net"/>
</node>
"

man busctl

       introspect SERVICE OBJECT [INTERFACE]
           Show interfaces, methods, properties and signals of the specified
           object (identified by its path) on the specified service. If the
           interface argument is passed, the output is limited to members of the
           specified interface.


busctl --user introspect net.poettering.Calculator /net/poettering/Calculator

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


busctl --user introspect net.poettering.Calculator /
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


The sd-bus library automatically added a couple of generic interfaces, as
mentioned above. But the first interface we see is actually the one we added! It
shows our two methods, and both take "xx" (two 64bit signed integers) as input
parameters, and return one "x". 

dbus-send --session --type=method_call --print-reply --dest='net.poettering.Calculator' \
/net/poettering/Calculator net.poettering.Calculator.Multiply \
int64:5 int64:7

method return time=1590799398.142437 sender=:1.418 -> destination=:1.420 serial=4 reply_serial=2
   int64 35

busctl --user call net.poettering.Calculator /net/poettering/Calculator net.poettering.Calculator Multiply xx 5 7
x 35

busctl --user call net.poettering.Calculator /net/poettering/Calculator net.poettering.Calculator Divide xx 99 17
x 5

busctl --user call net.poettering.Calculator /net/poettering/Calculator net.poettering.Calculator Divide xx 99 0
sorry, can't do that

dbus-monitor --session "interface=net.poettering.Calculator"

method call time=1603110729.667975 sender=:1.4306 -> destination=net.poettering.Calculator serial=2 path=/net/poettering/Calculator; interface=net.poettering.Calculator; member=Multiply
   int64 5
   int64 7



<case>

NOTE "introspect" on /

# dbus-send --system --type=method_call --print-reply --dest='com.sky.as.player' / org.freedesktop.DBus.Introspectable.Introspect

method return time=1600287980.030992 sender=:1.89 -> destination=:1.180 serial=1815 reply_serial=2
   string "<!DOCTYPE node PUBLIC "-//freedesktop//DTD D-BUS Object Introspection 1.0//EN"
"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd">
<node>
 <interface name="org.freedesktop.DBus.Peer">
  <method name="Ping"/>
  <method name="GetMachineId">
   <arg type="s" name="machine_uuid" direction="out"/>
  </method>
 </interface>
 <interface name="org.freedesktop.DBus.Introspectable">
  <method name="Introspect">
   <arg name="data" type="s" direction="out"/>
  </method>
 </interface>
 <interface name="org.freedesktop.DBus.Properties">
  <method name="Get">
   <arg name="interface" direction="in" type="s"/>
   <arg name="property" direction="in" type="s"/>
   <arg name="value" direction="out" type="v"/>
  </method>
  <method name="GetAll">
   <arg name="interface" direction="in" type="s"/>
   <arg name="properties" direction="out" type="a{sv}"/>
  </method>
  <method name="Set">
   <arg name="interface" direction="in" type="s"/>
   <arg name="property" direction="in" type="s"/>
   <arg name="value" direction="in" type="v"/>
  </method>
  <signal name="PropertiesChanged">
   <arg type="s" name="interface"/>
   <arg type="a{sv}" name="changed_properties"/>
   <arg type="as" name="invalidated_properties"/>
  </signal>
 </interface>
 <node name="com"/>
</node>
"

#define SD_BUS_METHOD(_member, _signature, _result, _handler, _flags)   \

so "nullptr" on _signature means that no input parameters

static const sd_bus_vtable g_asServiceVTable[] =
{
    SD_BUS_VTABLE_START(0),

    SD_BUS_METHOD("Config",                      nullptr,         "s",         &ASServicePrivate::config,                SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("Request",                     "usa{ss}a{ss}s", "(ua{ss}s)", &ASServicePrivate::request,               SD_BUS_VTABLE_UNPRIVILEGED),

    SD_BUS_METHOD("GetDiagContexts",             nullptr,         "s",         &ASServicePrivate::GetDiagContexts,       SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("SetDiagContexts",             "s",             nullptr,     &ASServicePrivate::SetDiagContexts,       SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("GetEuid",                     nullptr,         "us",        &ASServicePrivate::getEuid,               SD_BUS_VTABLE_UNPRIVILEGED),

    SD_BUS_METHOD("RegisterWebSocketListener",   "s",             nullptr,     &ASServicePrivate::registerWsListener,    SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("UnregisterWebSocketListener", "s",             nullptr,     &ASServicePrivate::unregisterWsListener,  SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_SIGNAL("WebSocketUpdate",             "ss",                                                                   0),

    SD_BUS_METHOD("RegisterUpdatesListener",     "s",             nullptr,     &ASServicePrivate::registerHttpListener,  SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("UnregisterUpdatesListener",   "s",             nullptr,     &ASServicePrivate::unregisterHttpListener,SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_SIGNAL("HttpUpdate",                  "sx",                                                                   0),

    SD_BUS_VTABLE_END
};

    m_objectPath("/com/sky/as/service")
    m_interface("com.sky.as.Service1")

    // installs the object and handlers for the methods
    int rc = sd_bus_add_object_vtable(m_dbusConn.handle(),
                                      &m_slot,
                                      m_objectPath.c_str(),
                                      m_interface.c_str(),
                                      g_asServiceVTable,
                                      this);


NOTE "introspect" on /com/sky/as/service 

# dbus-send --system --type=method_call --print-reply --dest='com.sky.as.player' /com/sky/as/service org.freedesktop.DBus.Introspectable.Introspect

method return time=1600288276.608564 sender=:1.89 -> destination=:1.184 serial=2112 reply_serial=2
   string "<!DOCTYPE node PUBLIC "-//freedesktop//DTD D-BUS Object Introspection 1.0//EN"
"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd">
<node>
 <interface name="org.freedesktop.DBus.Peer">
  <method name="Ping"/>
  <method name="GetMachineId">
   <arg type="s" name="machine_uuid" direction="out"/>
  </method>
 </interface>
 <interface name="org.freedesktop.DBus.Introspectable">
  <method name="Introspect">
   <arg name="data" type="s" direction="out"/>
  </method>
 </interface>
 <interface name="org.freedesktop.DBus.Properties">
  <method name="Get">
   <arg name="interface" direction="in" type="s"/>
   <arg name="property" direction="in" type="s"/>
   <arg name="value" direction="out" type="v"/>
  </method>
  <method name="GetAll">
   <arg name="interface" direction="in" type="s"/>
   <arg name="properties" direction="out" type="a{sv}"/>
  </method>
  <method name="Set">
   <arg name="interface" direction="in" type="s"/>
   <arg name="property" direction="in" type="s"/>
   <arg name="value" direction="in" type="v"/>
  </method>
  <signal name="PropertiesChanged">
   <arg type="s" name="interface"/>
   <arg type="a{sv}" name="changed_properties"/>
   <arg type="as" name="invalidated_properties"/>
  </signal>
 </interface>
 <interface name="com.sky.as.Service1">
  <method name="Config">
   <arg type="s" direction="out"/>
  </method>
  <method name="Request">
   <arg type="u" direction="in"/>
   <arg type="s" direction="in"/>
   <arg type="a{ss}" direction="in"/>
   <arg type="a{ss}" direction="in"/>
   <arg type="s" direction="in"/>
   <arg type="(ua{ss}s)" direction="out"/>
  </method>
  <method name="GetDiagContexts">
   <arg type="s" direction="out"/>
  </method>
  <method name="SetDiagContexts">
   <arg type="s" direction="in"/>
  </method>
  <method name="GetEuid">
   <arg type="u" direction="out"/>
   <arg type="s" direction="out"/>
  </method>
  <method name="RegisterWebSocketListener">
   <arg type="s" direction="in"/>
  </method>
  <method name="UnregisterWebSocketListener">
   <arg type="s" direction="in"/>
  </method>
  <signal name="WebSocketUpdate">
   <arg type="s"/>
   <arg type="s"/>
  </signal>
  <method name="RegisterUpdatesListener">
   <arg type="s" direction="in"/>
  </method>
  <method name="UnregisterUpdatesListener">
   <arg type="s" direction="in"/>
  </method>
  <signal name="HttpUpdate">
   <arg type="s"/>
   <arg type="x"/>
  </signal>
 </interface>
</node>
"

<ex>

static const sd_bus_vtable g_asServiceVTable[] =
{
    SD_BUS_VTABLE_START(0),
    SD_BUS_METHOD("DBusProxyMethodCall",       "s",         "us",         &ASProxyService::proxyMethodCall,                SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_VTABLE_END
};

    mPath("/com/sky/as/service")
    mInterface("com.sky.as.Service1")

    // installs the object and handlers for the methods
    int rc = sd_bus_add_object_vtable(mSDBus->handle(),
                                      &m_slot,
                                      mPath.c_str(),
                                      mInterface.c_str(),
                                      g_asServiceVTable,
                                      this);

dbus-send --system --type=method_call --print-reply --dest='com.sky.as.proxy' /com/sky/as/service org.freedesktop.DBus.Introspectable.Introspect

method return time=1600288780.823462 sender=:1.4 -> destination=:1.185 serial=688 reply_serial=2
   string "<!DOCTYPE node PUBLIC "-//freedesktop//DTD D-BUS Object Introspection 1.0//EN"
"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd">
<node>
 <interface name="org.freedesktop.DBus.Peer">
  <method name="Ping"/>
  <method name="GetMachineId">
   <arg type="s" name="machine_uuid" direction="out"/>
  </method>
 </interface>
 <interface name="org.freedesktop.DBus.Introspectable">
  <method name="Introspect">
   <arg name="data" type="s" direction="out"/>
  </method>
 </interface>
 <interface name="org.freedesktop.DBus.Properties">
  <method name="Get">
   <arg name="interface" direction="in" type="s"/>
   <arg name="property" direction="in" type="s"/>
   <arg name="value" direction="out" type="v"/>
  </method>
  <method name="GetAll">
   <arg name="interface" direction="in" type="s"/>
   <arg name="properties" direction="out" type="a{sv}"/>
  </method>
  <method name="Set">
   <arg name="interface" direction="in" type="s"/>
   <arg name="property" direction="in" type="s"/>
   <arg name="value" direction="in" type="v"/>
  </method>
  <signal name="PropertiesChanged">
   <arg type="s" name="interface"/>
   <arg type="a{sv}" name="changed_properties"/>
   <arg type="as" name="invalidated_properties"/>
  </signal>
 </interface>
 `<interface name="com.sky.as.Service1">`
  <method name="DBusProxyMethodCall">
   <arg type="s" direction="in"/>
   <arg type="u" direction="out"/>
   <arg type="s" direction="out"/>
  </method>
 </interface>
</node>
"

*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <systemd/sd-bus-vtable.h>
#include <systemd/sd-bus.h>

// #include <systemd/sd-bus.h>
//
// typedef int (*sd_bus_message_handler_t)(	sd_bus_message *m,
//  	void *userdata,
//  	sd_bus_error *ret_error);
//
// int sd_bus_add_match(	sd_bus *bus,
//  	sd_bus_slot **slot,
//  	const char *match,
//  	sd_bus_message_handler_t callback,
//  	void *userdata);

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

  // int x;
  // int y;
  int r;

  // reads the parameters
  r = sd_bus_message_read(m, "xx", &x, &y);
  // r = sd_bus_message_read(m, "ii", &x, &y);
  if (r < 0)
  {
    fprintf(stderr, "failed to parse parameters: %s\n", strerror(-r));
    return r;
  }

  printf("server: multiply {%d, %d} and sleep\n", x, y);

  // sleep(30);

  // reply with the result
  // return sd_bus_reply_method_return(m, "i", x * y);
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
// ii  systemd                                               237-3ubuntu10.33
//
// struct sd_bus_vtable {
//         /* Please do not initialize this structure directly, use the
//          * macros below instead */
//
//         uint8_t type:8;
//         uint64_t flags:56;
//         union {
//                 struct {
//                         size_t element_size;
//                 } start;
//                 struct {
//                         const char *member;
//                         const char *signature;
//                         const char *result;
//                         sd_bus_message_handler_t handler;
//                         size_t offset;
//                 } method;
//                 struct {
//                         const char *member;
//                         const char *signature;
//                 } signal;
//                 struct {
//                         const char *member;
//                         const char *signature;
//                         sd_bus_property_get_t get;
//                         sd_bus_property_set_t set;
//                         size_t offset;
//                 } property;
//         } x;
// };
//
// #define SD_BUS_VTABLE_START(_flags)                                     \
//         {                                                               \
//                 .type = _SD_BUS_VTABLE_START,                           \
//                 .flags = _flags,                                        \
//                 .x.start.element_size = sizeof(sd_bus_vtable),          \
//         }
//
// To build it under C++ which is already in the latest systemd:

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

// *os-dbus-type*
// x           64-bit signed integer
// #define SD_BUS_METHOD(_member, _signature, _result, _handler, _flags)

static const sd_bus_vtable calculator_vtable[] = {
  SD_BUS_VTABLE_START(0),
  SD_BUS_METHOD(
    "Multiply", "xx", "x", method_multiply, SD_BUS_VTABLE_UNPRIVILEGED),
  // SD_BUS_METHOD(
  //   "Multiply", "ii", "i", method_multiply, SD_BUS_VTABLE_UNPRIVILEGED),
  SD_BUS_METHOD("Divide", "xx", "x", method_divide, SD_BUS_VTABLE_UNPRIVILEGED),
  SD_BUS_VTABLE_END};

int main(int argc, char *argvp[])
{
  sd_bus_slot *slot = NULL;
  sd_bus *bus       = NULL;

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
  // object path `path` connected to the bus connection bus under the interface
  // interface.
  //
  // The table vtable may contain property declarations using SD_BUS_PROPERTY()
  // or SD_BUS_WRITABLE_PROPERTY(), method declarations using SD_BUS_METHOD(),
  // SD_BUS_METHOD_WITH_NAMES(), SD_BUS_METHOD_WITH_OFFSET(), or
  // SD_BUS_METHOD_WITH_NAMES_OFFSET(), and signal declarations using
  // SD_BUS_SIGNAL_WITH_NAMES() or SD_BUS_SIGNAL(), see below.
  //
  // NOTE: The `userdata` parameter contains a pointer that will be passed to
  // handler callback functions. It may be specified as NULL
  // if no value is necessary.
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

  r = sd_bus_add_object_vtable(bus,                          // bus
                               &slot,                        // slot
                               "/net/poettering/Calculator", // object path
                               "net.poettering.Calculator",  // interface
                               calculator_vtable,            // vtable
                               NULL);                        // user data

  if (r < 0)
  {
    fprintf(stderr, "failed to add object: %s\n", strerror(-r));
    goto finish;
  }

  printf("server: succuess on adding object\n");

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
  // messages *are dispatched to registered callbacks.* Each time it is invoked a
  // single operation is executed.
  //
  // return positive if a message was processed. It returns zero when no
  // operations were pending.
  //
  // When zero is returned the caller should synchronously poll for I/O events
  // before calling into sd_bus_process() again. For that either use the
  // simple, synchronous sd_bus_wait(3) call, or hook up the bus connection
  // object to an external or manual event loop using sd_bus_get_fd(3).
  //
  // https://www.freedesktop.org/software/systemd/man/sd_bus_wait.html#
  //
  // sd_bus_wait() synchronously waits for I/O on the specified bus connection
  // object. This function is supposed to be called whenever sd_bus_process(3)
  // returns zero, indicating that no work is pending on the connection.
  //
  // Internally, this call invokes ppoll(3), to wait for I/O on the bus
  // connection. If the timeout_sec parameter is specified, the call will block
  // at most for the specified amount of time in µs. Pass UINT64_MAX to permit
  // it to sleep *indefinitely*.

  for (;;)
  {
    // process requests
    r = sd_bus_process(bus, NULL);
    if (r < 0)
    {
      fprintf(stderr, "failed to process bus: %s\n", strerror(-r));
      goto finish;
    }

    printf("server: sd_bus_process returns {%d}\n", r);

    // return positive. processed a request and try to process
    // another one right away

    if (r > 0)
    {
      printf("processed a request and try another\n");
      continue;
    }

    printf("server: calls sd_bus_wait()\n");

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
