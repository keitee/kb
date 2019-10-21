
#include "readline.h"

// https://doc.qt.io/qt-5/qreadwritelock.html
// https://doc.qt.io/qt-5/qpointer.html

ReadLinePrivate *ReadLinePrivate::instance()
{
  static QReadWriteLock lock;
  static QPointer<ReadLinePrivate> instance_ = nullptr;

  // void QReadWriteLock::lockForRead()
  // Locks the lock for reading. This function will block the current thread if
  // another thread has locked for writing.
  //
  // It is not possible to lock for read if the thread already has locked for
  // write.

  lock.lockForRead();

  // bool QPointer::isNull() const
  // Returns true if the referenced object has been destroyed or if there is no
  // referenced object; otherwise returns false.

  if (Q_UNLIKELY(instance_.isNull()))
  {
    lock.unlock();
    lock.lockForWrite();

    if (instance.isNull())
      instance_ = new ReadLinePrivate(QAbstractEventDispatcher::instance());
  }

  lock.unlock();

  // T *QPointer::data() const
  // Returns the pointer to the object being guarded.

  return instance_.data();
}

ReadLine::ReadLine(QObject *parent)
  : QObject(parent),
  m_private(ReadLinePrivate::instance()),
  m_prompt("> ")
{

}
