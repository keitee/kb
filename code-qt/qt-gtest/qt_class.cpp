#include "qt_class.h"

// ={=========================================================================

void Counter::setValue(int value)
{
  if (value != m_value) {
    m_value = value;
    emit valueChanged(value);
  }
}

void Counter::doSomethingLong()
{
  qCritical() << "doSomethingLong: before sleep..";
  // here is the expensive or blocking operations
  QThread::sleep(10);
  qCritical() << "doSomethingLong: after sleep..";
}


// ={=========================================================================

namespace qt_thread_2
{
  void Controller::handleResults(const QString &result)
  {
    qCritical() << "handleResults: " << result;
  }
} // namespace

// ={=========================================================================
