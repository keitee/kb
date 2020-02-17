#include "dbusconnection.h"
#include "dbusconnection_p.h"

#include "dbusmessage.h"
// NOTE: direct access to _p.h
#include "dbusmessage_p.h"

#include "rlog.h"
#include <cassert>
#include <semaphore.h>

// 25 secs
#define DBUS_DEFAULT_TIMEOUT_USEC    (25 * 1000 * 1000)

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
  // TODO: need reset()?

  std::shared_ptr<DBusMessagePrivate> messageData = message.m_private;
  message.m_private.reset();

  // TODO: mutable is necessary?
  auto sendMessageLambda = [this, messageData]() mutable {
    // calling thread and a thread running event loop are the same
    assert(m_eventloop.onEventLoopThread());

    // construct sd_bus_message and get unique_ptr<sd_bus_messsage>
    auto msg = messageData->toMessage(m_bus);
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
    // NOTE: return m_eventloop.invokeMethod(std::move(sendMessageLambda));
    return m_eventloop.invokeMethod(sendMessageLambda);
  }
}

// private and static and called back with reply
int methodCallCallback_(sd_bus_message *reply,
                                 void *userData,
                                 sd_bus_error *retError)
{
  DBusConnectionPrivate *self = reinterpret_cast<DBusConnectionPrivate *>(userData);

  // shall be true since it's called back from event loop
  assert(self->m_eventloop.onEventLoopThread());

  // reply cookie
  uint64_t cookie;
  int r = sd_bus_message_get_reply_cookie(reply, &cookie);
  if (r < 0)
  {
    logSysFatal(-r, "failed to get cookie of the reply");
    return 0;
  }

  // this means that reply cookie is the same as cookie when sent.
  auto it = self->m_callbacks.find(cookie);

  // this case can really happen?
  if (if == self->m_callbacks.end())
  {
    logFatal("failed to find callback for cookie %" PRIu64, cookie);
    return 0;
  }

  auto f = it->second;
  self->m_callbacks.erase(it);

  if (f)
    f(DBusMessage(std::make_unique<DBusMessagePrivate>(reply)));

  // done. NOTE: any documentation about return value of handler?
  return 0;
}

// 1. the first use case.
// used from ::call() when it's called on the other thread. the suppliced
// callback has code to release `sem` and to get reply message.
//
// so send the `call` to eventloop if sd_message is constructed from the input
// messge without errors. If errors occur while constructing, run the supplied
// callabck to release `sem` and to pass back the error reply.

bool DBusConnectionPrivate::callWithCallback(DBusMessage &&message,
                                             const std::function<void(DBusMessage&&)> &callback,
                                             int msTimeout)
{
  // convert ms timeout(10^3) to sd-bus timeout in micro sec(us) time(10^6)
  uint64_t timeout;
  if (msTimeout >= 0)
    timeout = msTimeout * 1000;
  else
    timeout = DBUS_DEFAULT_TIMEOUT_USEC;

  std::function<void(DBusMessage &&)> errorCallback;

  if (!m_eventloop.onEventLoopThread())
    errorCallback = callback;

  // take the message data
  // NOTE: need reset()? since DBusMessagePrivate do not have move-assign, do it
  // manually
  std::shared_ptr<DBusMessagePrivate> messageData = message.m_private;
  message.m_private.reset();

  auto call = [this, messageData, callback, errorCallback, timeout]() {
    // must be false since it's called on the other thread
    assert(m_eventloop.onEventLoopThread());

    // construct the request message
    auto msg = messageData->toMessage(m_bus);
    if (!msg)
    {
      if (errorCallback)
        errorCallback(DBusMessage(DBusMessage::ErrorType::Failed));

      return false;
    }

    // ok, the message is constructed and send it out
    //
    // int sd_bus_call_async(
    //  sd_bus *bus,
    //  sd_bus_slot **slot,
    //  sd_bus_message *m,
    //  sd_bus_message_handler_t callback,
    //  void *userdata,
    //  uint64_t usec);
    int r = sd_bus_call_async(m_bus,
                              nullptr,
                              msg.get(),
                              &DBusConnectionPrivate::methodCallCallback_,
                              this,
                              timeout);
    if (r < 0)
    {
      logSysWarning(-rc, "dbus call failed");
      if (errorCallback)
        errorCallback(DBusMessage(DBusMessage::ErrorType::Failed));

      return false;
    }

    uint64_t cookie = 0;
    r = sd_bus_message_get_cookie(msg.get(), &cookie);
    if (r < 0)
    {
      logSysWarning(-rc, "failed to get request message cookie");
      if (errorCallback)
        errorCallback(DBusMessage(DBusMessage::ErrorType::Failed));

      return false;
    }

    // save the supplied callback
    m_callbacks.emplace(cookie, callback);

    return true;
  } // call

  // NOTE: really support this case??
  // if (m_eventloop.onEventLoopThread())
  // {
  //   return call();
  // }
  // else
  {
    return m_eventloop.invokeMethod(std::move(call));
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

// sends the message over this connection and `blocks`, waiting for a reply, for
// at most msTimeout ms or default value.
//
// this function is suitable for method calls only. it returns the reply message
// as its return value, which will be either of type DBusMessage::ReplyMessage
// or DBusMessage::ErrorMessage.
//
// if no reply is received within timeout, an automatic error will be delivered
// indicating the expiration of the call. The default timeout is -1, which will
// be replaced with an implementation-defined value that is suitable for
// inter-process communications. that is 25 seconds now.
//
// it is safe to call this from any thread. If called from the event loop thread
// it will block the event loop until complete
//
// TODO: ??? otherwise it will post a message to the event loop and send the dbus
// message within that.

DBusMessage DBusConnection::call(DBusMessage &&message, int msTimeout) const
{
  // sanity check
  if (DBusMessage::MethodCallMessage != message.type())
  {
    logWarning("trying to call with non-method call message");

    // DBusMessage(DBusMessage::Failed);
    // when use `enum class`

    // TODO: NO ctor???
    return DBusMessage(DBusMessage::ErrorType::Failed);
  }

  // can access private member, m_bus, since they are in friendship.
  auto *priv = m_private.get();
  if (!priv || !priv->m_bus)
  {
    logWarning("sd_bus not connected");
    return DBusMessage(DBusMessage::ErrorType::NoNetwork);
  }

  // called from the eventloop thread
  if (priv->m_eventloop.onEventLoopThread())
  {
    logWarning("DBusConnection::call() is called on the eventloop thread");

    // construct `request` sd_bus_message from the given message and returns
    // unique_ptr

    auto msg = message.m_private->toMessage(priv->m_bus);
    if (!msg)
    {
      logWarning("failed to make sd_bus message from DBusMessage");
      return DBusMessage(DBusMessage::ErrorType::Failed);
    }

    sd_bus_error error;
    memset(&error, 0, sizeof(error));

    // convert ms timeout(10^3) to sd-bus timeout in micro sec(us) time(10^6)
    uint64_t timeout{};
    if (msTimeout >= 0)
      timeout = msTimeout * 1000;
    else
      timeout = DBUS_DEFAULT_TIMEOUT_USEC;

    // make the sync call and get reply
    sd_bus_message *reply{nullptr};
    int rc = sd_bus_call(priv->m_bus, msg.get(), timeout, &error, &reply);

    // if there is an error reply
    if ((rc < 0) || (!reply))
    {
      // TODO: construct DBusMessage from the sd_bus_error of call
      // DBusMessagePrivate is exposed and can we fix it??
      DBusMessage errorMessage(std::make_unique<DBusMessagePrivate>(&error));
      sd_bus_error_free(&error);
      return errorMessage;
    }

    // if there is no error but reply
    DBusMessage replyMessage(std::make_unique<DBusMessagePrivate>(reply));

    // release
    sd_bus_message_unref(reply);
    sd_bus_error_free(&error);

    return replyMessage;
  }

  // otherwise, not called from the dbus/eventloop thread so use async version
  // and block on the callback.
  //
  // else on "if (priv->m_eventloop.onEventLoopThread())"

  logWarning("DBusConnection::call() is called on the other thread");

  DBusMessage replyMessage;

  sem_t sem;

  sem_init(&sem, 0, 0);

  std::function<void(DBusMessage)> f = [&](DBusMessage &&reply) {
    // must be false since expects it runs on other thread
    assert(priv->m_eventloop.onEventLoopThread());

    replyMessage = std::move(reply);

    if (0 != sem_post(&sem))
      logSysFatal(errno, "failed to post semaphore");
  };

  // make the dbus call
  if (!m_private->callWithCallback(std::move(message), f, msTimeout))
  {
    replyMessage = DBusMessage(DBusMessage::ErrorType::Failed);
  }
  // wait for 'f' to be called
  else if (0 != sem_wait(&sem))
  {
    logSysFatal(errno, "failed to wait on semaphore");
    replyMessage = DBusMessage(DBusMessage::ErrorType::Failed);
  }

  sem_destroy(&sem);

  return replyMessage;
}
