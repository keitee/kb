
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

  GET_RL_FUNC(rl_on_new_line);
  GET_RL_FUNC(rl_forced_update_display);
  GET_RL_FUNC(rl_completion_matches);
  GET_RL_FUNC(rl_bind_key);
  GET_RL_FUNC(rl_callback_handler_install);
  GET_RL_FUNC(rl_callback_read_char);
  GET_RL_FUNC(rl_callback_handler_remove);
  GET_RL_FUNC(add_history);

#undef GET_RL_FUNC

  // Variable: rl_completion_func_t * rl_attempted_completion_function
  //
  // A pointer to an alternative function to create matches. The function is
  // called with text, start, and end. start and end are indices in
  // rl_line_buffer defining the boundaries of text, which is a character
  // string. If this function exists and returns NULL, or if this variable is
  // set to NULL, then rl_complete() will call the value of
  // rl_completion_entry_function to generate matches, otherwise the array of
  // strings returned will be used. If this function sets the
  // rl_attempted_completion_over variable to a non-zero value, Readline will
  // not perform its default completion even if this function returns no
  // matches.

  {
    // replace the completion function with ours
    void **rl_attempted_completion_function = reinterpret_cast<void **>(
      dlsym(m_libHandle, "rl_attempted_completion_function"));
    if (rl_attempted_completion_function)
      *rl_attempted_completion_function =
        reinterpret_cast<void *>(completionCallback_);

    // set the tab key to be the completion trigger
    rl_command_func_t *rl_complete =
      reinterpret_cast<rl_command_func_t *>(dlsym(m_libHandle, "rl_complete"));
    if (rl_complete)
      m_rl_bind_key('\t', rl_complete);
  }

  // install commands
  addCommand("quit", {}, "quit program", this,
             slotToObject(&ReadLinePrivate::onQuitCommand));

  addCommand("exit", {}, "Quit program (same as quit)", this,
             slotToObject(&ReadLinePrivate::onQuitCommand));

  addCommand("help", {}, "Display this text", this,
             slotToObject(&ReadLinePrivate::onHelpCommand));
}

// Internal method intended to only be called from ReadLine, it adds a new
// command and maps it to the \a slotObj for the given \a receiver.

bool ReadLinePrivate::addCommand(const QString &name, const QStringList &args,
                                 const QString &desc, const QObject *receiver,
                                 QtPrivate::QSlotObjectBase *slotObj)
{
  QMutexLocker lock(&m_commandsLock);

  if (m_commands.contains(name))
  {
    qWarning() << "already have command" << name;
    return false;
  }

  int helpWidth = name.length();
  for (const QString &arg : args)
    helpWidth += 1 + arg.length();

  // calculate the width of the command plus arg strings for the help text
  if (helpWidth >= m_maxCommandHelpWidth)
    m_maxCommandHelpWidth = qMin<int>(helpWidth, 50);

  Command command = {args, desc, (receiver != nullptr), receiver, slotObj};
  m_commands.insert(name, std::move(command));
}

void ReadLinePrivate::commandLineHandler(char *line)
{
  // TODO: necessary since this is member function?
  ReadLinePrivate *self = instance();

  if (line == nullptr)
    QCoreApplication::quit();
  else
    self->commandLineHandler_(QString(line));
}

void ReadLinePrivate : start(const QString &promt)
{
  // https://doc.qt.io/qt-5/qtglobal.html#Q_ASSERT
  // It does nothing if QT_NO_DEBUG was defined during compilation.

  Q_ASSERT(m_libHandle != nullptr);
  if (!m_libHandle)
    return;

  Q_ASSERT(m_stdinListener != nullptr);
  if (!m_stdinListener)
    return;

  // typedef void rl_vcpfunc_t (char *);
  //
  // Function:
  // void rl_callback_handler_install (const char *prompt, rl_vcpfunc_t
  // *lhandler)
  //
  // Set up the terminal for readline I/O and display the initial expanded value
  // of prompt.
  //
  // Save the value of lhandler to use as a handler function to call when a
  // complete line of input has been entered. The handler function receives the
  // text of the line as an argument.
  //
  // As with readline(), the handler function should free the line when it it
  // finished with it.

  m_rl_callback_handler_install(prompt.toLatin1().constData(),
                                commandLineHandler);

  m_stdinListener->setEnabled(true);

  qInstallMessageHandler(qtMessageHandler);

  m_running = true;
}

/* ={==========================================================================
 @brief :
  ...

 @note  :

*/
ReadLine::ReadLine(QObject *parent)
    : QObject(parent)
    , m_private(ReadLinePrivate::instance())
    , m_prompt("> ")
{}
