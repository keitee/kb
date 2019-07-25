#include <QThread>
#include "worker.h"

void Worker::progress()
{
  qCritical() << "Worker::progress: starts";

  // here is the expensive or blocking operations
  QThread::sleep(10);

  qCritical() << "Worker::progress: finishes";

  emit finished();
}
