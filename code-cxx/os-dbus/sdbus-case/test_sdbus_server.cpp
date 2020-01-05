#include <sys/signal.h>
#include <systemd/sd-daemon.h>
#include "eventloop.h"
#include "dbusconnection.h"
#include "rlog.h"

// ={=========================================================================
int main(int argc, char **argv)
{
  // disable the SIGPIPE signal, the most annoying signal in the world
  signal(SIGPIPE, SIG_IGN);

  // create the event loop
  EventLoop eventLoop;

  // connect to the system bus
  // DBusConnection dbusConn = DBusConnection::systemBus(eventLoop);
  DBusConnection::systemBus(eventLoop);
  // if (!dbusConn.isConnected())
  // {
  //   logError("failed to connect to system bus");
  //   return EXIT_FAILURE;
  // }

  logWarning("sdbus server is running");

  // Notify that the service is now ready.
  sd_notify(0, "READY=1");

  return eventLoop.run();
}
