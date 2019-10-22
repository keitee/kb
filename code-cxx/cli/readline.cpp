
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

ReadLinePrivate::ReadLinePrivate(QObject *parent)
    : QObject(parent)
    , m_stdinListener(nullptr)
    , m_running(false)
    , m_maxCommandHelpWidth(30)
    , m_libHandle(nullptr)
    , m_rl_forced_update_display(nullptr)
    , m_rl_completion_matches(nullptr)
    , m_rl_bind_key(nullptr)
    , m_rl_callback_handler_install(nullptr)
    , m_rl_callback_read_char(nullptr)
    , m_rl_callback_handler_remove(nullptr)
    , m_add_history(nullptr)
{
  dlerror();

  m_libHandle = dlopen("libreadline.so", RTLD_NOW);
  if (!m_libHandle)
  {
    LOG_MSG("failed to open 'libreadline.so' (%s)", dlerror());
    return;
  }

  /*
  assume that for `rl_on_new_line` symbol, there is symbol_t typedef and
  m_symbol member variable to set.

  */

#define GET_RL_FUNC(f)                                                         \
  do                                                                           \
  {                                                                            \
    m_##f = reinterpret_cast<f##_t>(dlsym(handle, "" #f ""));                  \
    if (!m_##f)                                                                \
    {                                                                          \
      LOG_MSG("failed to get symbol '" #f "' (%s)", dlerror());                \
      dlclose(handle);                                                         \
      handle = nullptr;                                                        \
      return;                                                                  \
    }                                                                          \
  } while (0)
}

ReadLine::ReadLine(QObject *parent)
    : QObject(parent)
    , m_private(ReadLinePrivate::instance())
    , m_prompt("> ")
{}
