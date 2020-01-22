#include "class.h"

#include <QDebug>
#include <QThread>

void ThreadFoo::progress()
{
  std::unique_lock<std::mutex> lock(m_);

  qCritical() << "Worker::progress: starts";

  // here is the expensive or blocking operations
  QThread::sleep(5);

  qCritical() << "Worker::progress: finishes";

  cv_.notify_one();
}
