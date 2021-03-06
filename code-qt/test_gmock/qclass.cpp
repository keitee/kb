#include "qclass.h"

/*
={=============================================================================
CounterFoo
*/

void CounterFoo::setValue(int value)
{
  if (value != m_value)
  {
    m_value = value;
    emit valueChanged(value);
  }
}

void CounterFoo::doSomethingLong()
{
  qCritical() << "Counter::doSomethingLong() enters";
  // here is the expensive or blocking operations
  // void QThread::sleep(unsigned long secs);
  QThread::sleep(5);
  m_value = 5;
}

#if 0

// ={=========================================================================

CustomEventReceiver::CustomEventReceiver(std::mutex &m, std::condition_variable &cv)
  : m_(m), cv_(cv)
{
}

// void QObject::customEvent(QEvent *event)

void CustomEventReceiver::customEvent(QEvent *event)
{
  QEvent::Type eventType = event->type();
  qDebug() << "received %d" << eventType;
  received_++;

  // when has all events then notify it.
  if (received_ == 5)
    cv_.notify_one();
}

int CustomEventReceiver::getReceived()
{
  return received_;
}


// ={=========================================================================

namespace qt_thread_2
{
  void Controller::handleResults(const QString &result)
  {
    qCritical() << "handleResults: " << result;
  }
} // namespace

#endif

// ={=========================================================================
