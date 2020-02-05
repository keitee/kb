#include "dbusconnection.h"
#include "dbusconnection_p.h"

#include "dbusmessage.h"
// NOTE: direct access to _p.h
#include "dbusmessage_p.h"

#include "rlog.h"
#include <cassert>

/* ={--------------------------------------------------------------------------
 @brief :
  DBusConnectionPrivate
*/

DBusConnectionPrivate::DBusConnectionPrivate(const EventLoop &eventloop,
                                             sd_bus *bus)
    : m_eventloop(eventloop)
    , m_bus(bus)
{}

DBusConnectionPrivate::~DBusConnectionPrivate()
{
  m_eventloop.flush();

  // free the bus
  // sd_bus_unref() and sd_bus_flush_close_unref() always return NULL.
  m_bus = sd_bus_flush_close_unref(m_bus);
}

bool DBusConnectionPrivate::send(DBusMessage &&message) const
{
  // take the message data. 
  // NOTE: access private member since both are in friendship

  std::shared_ptr<DBusMessagePrivate> messageData = message.m_private;
  message.m_private.reset();

  // TODO: mutable is necessary?
  auto sendMessageLambda = [this, messageData]() mutable {
    // calling thread and a thread running event loop are the same
    assert(m_eventloop.onEventLoopThread());

    // construct sd_bus_message and get unique_ptr<sd_bus_messsage>
    auto msg = messageData->toMessage_(m_bus);
    if (!msg)
      return false;

    // make the call

    int rc{};
    if (DBusMessage::SignalMessage == messageData->m_type)
    {
      if (messageData->m_service.empty())
      {
        // int sd_bus_send(sd_bus *bus, sd_bus_message *m,
        //  uint64_t *cookie);
        rc = sd_bus_send(m_bus, msg.get(), nullptr);
      }
      else
      {
        // int sd_bus_send_to(sd_bus *bus, sd_bus_message *m,
        //  const char *destination, uint64_t *cookie);
        rc = sd_bus_send_to(m_bus,
                            msg.get(),
                            messageData->m_service.c_str(),
                            nullptr);
      }
    }
    else if (DBusMessage::MethodCallMessage == messageData->m_type)
    {
      // so do not expect reply of a call
      // https://www.freedesktop.org/software/systemd/man/sd_bus_message_set_expect_reply.html#
      sd_bus_message_set_expect_reply(msg.get(), false);
      rc = sd_bus_send(m_bus, msg.get(), nullptr);
    }
    else
    {
      return false;
    }

    if (rc < 0)
    {
      logSysWarning(-rc, "sd_dus_send() failed");
      return false;
    }

    return true;
  };

  // calling thread and a thread running event loop are the same
  if (m_eventloop.onEventLoopThread())
  {
    return sendMessageLambda();
  }
  else
  {
    // otherwise queue it on the event loop thread
    // return m_eventloop.invokeMethod(std::move(sendMessageLambda));
    return m_eventloop.invokeMethod(sendMessageLambda);
  }
}

/* ={--------------------------------------------------------------------------
 @brief :
  DBusConnection
*/

DBusConnection::DBusConnection() {}

DBusConnection::DBusConnection(std::shared_ptr<DBusConnectionPrivate> &&priv)
    : m_private(std::move(priv))
//  : m_private(priv)
{}

DBusConnection::DBusConnection(const DBusConnection &rhs)
    : m_private(rhs.m_private)
{}

DBusConnection::DBusConnection(DBusConnection &&rhs)
    : m_private(std::move(rhs.m_private))
{}

DBusConnection DBusConnection::systemBus(const EventLoop &eventloop)
{
  sd_bus *bus{nullptr};

  int rc = sd_bus_open_system(&bus);
  if (rc < 0)
  {
    logSysError(-rc, "failed to open system bus");

    return DBusConnection(nullptr);
  }

  // bind bus with event loop
  //
  // https://www.freedesktop.org/software/systemd/man/sd_bus_attach_event.html#
  //
  // sd_bus_attach_event() attaches the specified bus connection object to an
  // sd-event(3) event loop object at the specified priority (see
  // sd_event_source_set_priority(3) for details on event loop priorities).
  //
  // When a bus connection object is attached to an event loop incoming messages
  // will be automatically read and processed, and outgoing messages written,
  // whenever the event loop is run.
  //
  // When the event loop is about to terminate, the bus connection is
  // automatically flushed and closed (see sd_bus_set_close_on_exit(3) for
  // details on this).
  //
  // By default bus connection objects are not attached to any event loop.
  //
  // When a bus connection object is attached to one it is not necessary to
  // invoke sd_bus_wait(3) or sd_bus_process(3) as this functionality is handled
  // automatically by the event loop.

  rc = sd_bus_attach_event(bus, eventloop.handle(), SD_EVENT_PRIORITY_NORMAL);
  if (rc < 0)
  {
    logSysError(-rc, "failed to attach bus to event loop");
    sd_bus_unref(bus);
    return DBusConnection(nullptr);
  }

  // pass `eventloop` and `bus` to DBusConnectionPrivate
  return DBusConnection(
    std::make_shared<DBusConnectionPrivate>(eventloop, bus));
}

DBusConnection DBusConnection::sessionBus(const EventLoop &eventloop)
{
  sd_bus *bus{nullptr};

  int rc = sd_bus_open_user(&bus);
  if (rc < 0)
  {
    logSysError(-rc, "failed to open session bus");
    return DBusConnection(nullptr);
  }

  // bind bus with event loop
  rc = sd_bus_attach_event(bus, eventloop.handle(), SD_EVENT_PRIORITY_NORMAL);
  if (rc < 0)
  {
    logSysError(-rc, "failed to attach bus to event loop");
    sd_bus_unref(bus);
    return DBusConnection(nullptr);
  }

  return DBusConnection(
    std::make_shared<DBusConnectionPrivate>(eventloop, bus));
}

// assess `private` of DBusConnecitonPrivae
sd_bus *DBusConnection::handle() const
{
  if (!m_private)
    return nullptr;
  else
    return m_private->m_bus;
}

// assess `private` of DBusConnecitonPrivae
EventLoop DBusConnection::eventLoop() const
{
  if (!m_private)
    return EventLoop();
  else
    return m_private->m_eventloop;
}

// assess `private` of DBusConnecitonPrivae
bool DBusConnection::isConnected() const
{
  return m_private && m_private->m_bus;
}

bool DBusConnection::registerName(const std::string &name)
{
  if (!m_private || !m_private->m_bus)
  {
    return false;
  }

  // request the given name
  int rc = sd_bus_request_name(m_private->m_bus, name.c_str(), 0);
  if (rc < 0)
  {
    logSysError(-rc, "failed to acquire the service name");
    return false;
  }

  return true;
}

// DBusMessage call(DBusMessage &&message, int msTimeout) const
// {
// }
