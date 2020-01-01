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
  // take the message data
  std::shared_ptr<DBusMessagePrivate> messageData = message.m_private;
  message.m_private.reset();

  auto sendMessageLambda = [this, messageData]() mutable {
    // calling thread and a thread running event loop are the same
    assert(m_eventloop.onEventLoopThread());

    auto msg = messageData->toMessage(m_dbus);

    // TODO
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
