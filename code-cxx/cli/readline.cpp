
#include "readline.h"
#include "slog.h"

#include <QCoreApplication>
#include <QAbstractEventDispatcher>
#include <QRegularExpression>
#include <QReadWriteLock>
// #include <QTextStream>
#include <QDebug>

#include <stdio.h>
#include <stdlib.h>
// #include <stdarg.h>
#include <string.h>
#include <dlfcn.h>

#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif


/* ={--------------------------------------------------------------------------
 @brief :
  helper TODO

  Helper utility for returning a QtPrivate::QSlotObjectBase object
 pointing to the given method.

 \warning This function does no argument validation, DO NOT use this
 unless you're 100% sure the slot args will match the signal.
*/

template <typename Func1>
static QtPrivate::QSlotObjectBase *slotToObject(Func1 slot)
{
  typedef QtPrivate::FunctionPointer<Func1> SlotType;
  return new QtPrivate::QSlotObject<Func1, typename SlotType::Arguments, void>(
    slot);
}


/* ={==========================================================================
 @brief :
*/

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

    if (instance_.isNull())
      instance_ = new ReadLinePrivate(QAbstractEventDispatcher::instance());
  }

  lock.unlock();

  // T *QPointer::data() const
  // Returns the pointer to the object being guarded.

  return instance_.data();
}

/* ={==========================================================================
 @brief :
*/

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
    m_##f = reinterpret_cast<f##_t>(dlsym(m_libHandle, "" #f ""));                  \
    if (!m_##f)                                                                \
    {                                                                          \
      LOG_MSG("failed to get symbol '" #f "' (%s)", dlerror());                \
      dlclose(m_libHandle);                                                         \
      m_libHandle = nullptr;                                                        \
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
    {
      *rl_attempted_completion_function =
        reinterpret_cast<void *>(completionCallback);
    }

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

  // TODO: install a notifier on stdin to feed readline
  m_stdinListener =
    new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read, this);
  m_stdinListener->setEnabled(false);

  QObject::connect(m_stdinListener,
                   &QSocketNotifier::activated,
                   this,
                   &ReadLinePrivate::onStdinActivated);
}

ReadLinePrivate::~ReadLinePrivate()
{
  // remove the listener on stdin
  if (m_stdinListener) {
    m_stdinListener->setEnabled(false);
    delete m_stdinListener;
  }

  // uninstall the handler
  if (m_rl_callback_handler_remove)
    m_rl_callback_handler_remove();

  // close the handle to the readline library
  if (m_libHandle) {
    dlclose(m_libHandle);
    m_libHandle = nullptr;
  }

  // clean up all the command handlers
  QMap<QString, Command>::iterator it = m_commands.begin();
  while (it != m_commands.end()) {

    const Command &command = it.value();
    if (command.slotObj)
      command.slotObj->destroyIfLastRef();

    it = m_commands.erase(it);
  }
}

/* ={==========================================================================
 @brief :
  it adds a new command and maps it to the \a slotObj for the given \a receiver.
*/

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

/* ={==========================================================================
 @brief :

*/
void ReadLinePrivate::runCommand(const QString &command,
                                 const QStringList &arguments)
{
  commandExecute(command, arguments);
}

/* ={==========================================================================
 @brief :
  ...

*/

void ReadLinePrivate::start(const QString &prompt)
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
  // Save the value of lhandler to use as a handler function to call **when a
  // complete line of input has been entered.** The handler function receives
  // the text of the line as an argument.
  //
  // As with readline(), the handler function should free the line when it it
  // finished with it.

  m_rl_callback_handler_install(prompt.toLatin1().constData(),
                                commandLineHandler);

  m_stdinListener->setEnabled(true);

  // https://doc.qt.io/qt-5/qtglobal.html#qInstallMessageHandler
  //
  // Installs a Qt message handler which has been defined previously. Returns a
  // pointer to the previous message handler.
  //
  // The message handler is a function that prints out debug messages, warnings,
  // critical and fatal error messages. The Qt library (debug mode) contains
  // hundreds of warning messages that are printed when internal errors (usually
  // invalid function arguments) occur. Qt built in release mode also contains
  // such warnings unless QT_NO_WARNING_OUTPUT and/or QT_NO_DEBUG_OUTPUT have
  // been set during compilation. If you implement your own message handler, you
  // get total control of these messages.
  //
  // The default message handler prints the message to the standard output under
  // X11 or to the debugger under Windows. If it is a fatal message, the
  // application aborts immediately.
  //
  // Only one message handler can be defined, since this is usually done on an
  // application-wide basis to control debug output.
  //
  // To restore the message handler, call qInstallMessageHandler(0).

  qInstallMessageHandler(qtMessageHandler);

  m_running = true;
}

/* ={==========================================================================
 @brief :
  ...

*/

void ReadLinePrivate::stop()
{
  Q_ASSERT(m_libHandle != nullptr);
  if (!m_libHandle)
    return;

  Q_ASSERT(m_stdinListener != nullptr);
  if (!m_stdinListener)
    return;

  m_running = false;

  qInstallMessageHandler(0);

  m_rl_callback_handler_remove();
}

bool ReadLinePrivate::isValid() const { return m_libHandle != nullptr; }

bool ReadLinePrivate::isRunning() const { return m_running; }

/* ={--------------------------------------------------------------------------
 @brief :
  callback from libreadline
*/

void ReadLinePrivate::commandLineHandler(char *line)
{
  // necessary since this is member function? yes because it's called back
  // without `this` pointer.

  ReadLinePrivate *self = instance();

  if (line == nullptr)
    QCoreApplication::quit();
  else
    self->commandLineHandler_(QString(line));
}

void ReadLinePrivate::commandLineHandler_(const QString &line)
{
  // TODO:
  // split the string up using regext to group by whitespace (this takes into
  // account quoted strings)
  QRegularExpression regex(R"regex([^\"\'\s]\S*|\".*?\"|\'.*?\')regex");
  QRegularExpressionMatchIterator it = regex.globalMatch(line);

  QStringList args;

  while (it.hasNext())
  {

    // get the match and strip any leading and trailing quotes
    QString match = it.next().captured(0);

    int matchLen = match.length();
    if (matchLen <= 0)
      continue;

    if ((match[0] == QChar('\"')) && (match[matchLen - 1] == QChar('\"')))
    {
      match.remove(matchLen - 1, 1);
      match.remove(0, 1);
    }
    if ((match[0] == QChar('\'')) && (match[matchLen - 1] == QChar('\'')))
    {
      match.remove(matchLen - 1, 1);
      match.remove(0, 1);
    }

    args += match;
  }

  if (!args.empty())
  {

    // the first argument should be the command
    const QString command = args.takeFirst();

    // try and excute the command
    commandExecute(command, args);

    // add the command to the history
    m_add_history(line.toLatin1().constData());
  }
}

/* ={--------------------------------------------------------------------------
 @brief :
  slot called when input arrives on stdin. use this to trigger libreadline
*/

void ReadLinePrivate::commandExecute(const QString &command,
                                     const QStringList &arguments)
{
  QStringList possibleCommands;

  QMutexLocker locker(&m_commandsLock);

  QMap<QString, Command>::const_iterator cmdRef = m_commands.end();
  QMap<QString, Command>::const_iterator it     = m_commands.begin();

  for (; it != m_commands.end(); ++it)
  {

    const QString &name = it.key();
    if (name.startsWith(command))
    {

      // exact matches always work
      if (command.length() == name.length())
      {
        cmdRef = it;
        break;
      }

      // check if we don't already have a match, if we do then we have
      // multiple matches and have to report an error.
      if (cmdRef == m_commands.end())
        cmdRef = it;
      else
        possibleCommands << name;
    }
  }

  if (cmdRef == m_commands.end())
  {
    qWarning("%s: No such command.", command.toLatin1().constData());
  }
  else if (!possibleCommands.isEmpty())
  {
    qWarning() << "Ambiguous command" << command
               << "possible commands:" << cmdRef.key() << possibleCommands;
  }
  else
  {

    // copy the command info
    const Command handler = cmdRef.value();

    // unlock before calling the slot
    locker.unlock();

    if (handler.slotObj)
    {

      // if the receiver was destroyed, skip this part
      if (Q_LIKELY(!handler.receiver.isNull() || !handler.hasValidReceiver))
      {

        // we've already checked the arguments when the command was
        // added, so can safely do the following void* casts
        void *args[2] = {
          0,
          reinterpret_cast<void *>(const_cast<QStringList *>(&arguments))};
        handler.slotObj->call(const_cast<QObject *>(handler.receiver.data()),
                              args);
      }
    }
  }
}

/* ={--------------------------------------------------------------------------
 @brief :
  slot called when input arrives on stdin. use this to trigger libreadline
*/

void ReadLinePrivate::onStdinActivated(int fd)
{
  Q_ASSERT(STDIN_FILENO == fd);

  if (m_rl_callback_read_char)
    m_rl_callback_read_char();
}

void ReadLinePrivate::onQuitCommand(const QStringList &args)
{
  Q_UNUSED(args);

  QCoreApplication::quit();
}

void ReadLinePrivate::onHelpCommand(const QStringList &args)
{
  Q_UNUSED(args);

  QString text("Available commands:\n");

  // TODO:
  QMutexLocker lock(&m_commandsLock);

  for (auto it = m_commands.cbegin(); it != m_commands.cend(); ++it)
  {
    const QString &name    = it.key();
    const Command &details = it.value();

    // build up a line with command and its args
    QString command(name);
    for (const QString &arg : details.arguments)
    {
      command += ' ';
      command += arg;
    }

    QString line = QString("  %1 %2\n")
                     .arg(command, -m_maxCommandHelpWidth)
                     .arg(details.description);

    text += line;
  }

  // TODO
  qInfo().noquote() << text;
}

/* ={--------------------------------------------------------------------------
 @brief :
 callback
*/

char **ReadLinePrivate::completionCallback(const char *text, int start, int end)
{
  Q_UNUSED(end);

  ReadLinePrivate *self = instance();
  char **matches{nullptr};

  if (start == 0)
    matches = self->m_rl_completion_matches(text, commandGenerator);

  return matches;
}

char *ReadLinePrivate::commandGenerator(const char *text, int state)
{
  ReadLinePrivate *self = instance();
  return self->commandGenerator_(text, state);
}

char *ReadLinePrivate::commandGenerator_(const char *text, int state)
{
  Q_UNUSED(state);

  QMutexLocker lock(&m_commandsLock);

  static QList<QByteArray> matches;

  // if this is a new word to complete, initialise now.
  if (state == 0)
  {
    matches.clear();

    for (auto it = m_commands.cbegin();
        it != m_commands.cend(); ++it)
    {
      const QString &command = it.key();

      if (command.startsWith(text))
        matches.append(command.toLatin1());
    }
  }

  // if no names matched, return null
  if (matches.empty())
    return nullptr;

  // otherwise dup the match then remove it from the front of the list
  char *match = qstrdup(matches.first().constData());
  matches.removeFirst();

  return match;
}

void ReadLinePrivate::qtMessageHandler(QtMsgType type,
                                       const QMessageLogContext &context,
                                       const QString &message)
{
  Q_UNUSED(type);
  Q_UNUSED(context);

  fputs(message.toLatin1().constData(), stdout);
  fputc('\n', stdout);
  fflush(stdout);
  //printf("%s\n", message.toLatin1().constData());
  //printf("\n");

  ReadLinePrivate *readline = instance();
  if (readline && readline->m_rl_on_new_line && readline->isRunning()) {
    readline->m_rl_on_new_line();
  }
}


/* ={==========================================================================
 @brief :
  ...

 @note  :

*/

ReadLine::ReadLine(QObject *parent)
    : QObject(parent), m_private(ReadLinePrivate::instance()), m_prompt("> ") 
{
  if (m_private.isNull() || !m_private->isValid()) {
    qCritical("failed to get readline instance");
  }
}

ReadLine::~ReadLine()
{
  if (!m_private.isNull())
    m_private->stop();
}


bool ReadLine::isValid() const
{
  return !m_private.isNull() && m_private->isValid();
}

void ReadLine::setPrompt(const QString &prompt)
{
  m_prompt = prompt;
}

QString ReadLine::prompt() const
{
  return m_prompt;
}

void ReadLine::start()
{
  Q_ASSERT(!m_private.isNull());
  m_private->start(m_prompt);
}

void ReadLine::stop()
{
  Q_ASSERT(!m_private.isNull());
  m_private->stop();
}

void ReadLine::runCommand(const QString &command, const QStringList &arguments)
{
  Q_ASSERT(!m_private.isNull());
  m_private->runCommand(command, arguments);
}

bool ReadLine::addCommandImpl(const QString &name, const QStringList &args,
    const QString &desc, const QObject *receiver,
    QtPrivate::QSlotObjectBase *slotObj)
{
  Q_ASSERT(!m_private.isNull());
  return m_private->addCommand(name, args, desc, receiver, slotObj);
}
