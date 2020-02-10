#ifndef READLINE_H
#define READLINE_H

#include <QDebug>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QPointer>
#include <QSocketNotifier>
#include <QString>

// libreadline function types

typedef char *rl_compentry_func_t(const char *, int);
typedef int rl_command_func_t(int, int);
typedef void rl_vcpfunc_t(char *);

typedef char **rl_completion_func_t(const char *text, int start, int end);

typedef int (*rl_message_t)(const char *, ...);
typedef int (*rl_crlf_t)(void);
typedef int (*rl_on_new_line_t)(void);
typedef int (*rl_forced_update_display_t)(void);
typedef char **(*rl_completion_matches_t)(const char *, rl_compentry_func_t *);
typedef int (*rl_bind_key_t)(int, rl_command_func_t *);
typedef void (*rl_callback_handler_install_t)(const char *, rl_vcpfunc_t *);
typedef void (*rl_callback_read_char_t)(void);
typedef void (*rl_callback_handler_remove_t)(void);

typedef void (*add_history_t)(const char *);

// ReadLinePrivate

class ReadLinePrivate : public QObject
{
  Q_OBJECT

private:
  ReadLinePrivate(QObject *parent = nullptr);

public:
  static ReadLinePrivate *instance();
  ~ReadLinePrivate();
  bool addCommand(const QString &name,
                  const QStringList &args,
                  const QString &description,
                  const QObject *receiver,
                  QtPrivate::QSlotObjectBase *slotObj);
  void runCommand(const QString &command, const QStringList &arguments);

  void start(const QString &prompt = QStringLiteral(">"));
  void stop();

  bool isValid() const;
  bool isRunning() const;

private slots:
  void onStdinActivated(int fd);
  void onQuitCommand(const QStringList &args);
  void onHelpCommand(const QStringList &args);

private:
  // cxx-error: cannot convert ‘ReadLinePrivate::commandLineHandler’
  //  from type ‘void (ReadLinePrivate::)(char*)’ to type ‘void (*)(char*)’
  // void commandLineHandler(char *line);

  static void commandLineHandler(char *line);

  void commandLineHandler_(const QString &line);
  void commandExecute(const QString &command, const QStringList &arguments);

  static char **completionCallback(const char *text, int start, int end);
  static char *commandGenerator(const char *text, int state);
  char *commandGenerator_(const char *text, int state);
  static void qtMessageHandler(QtMsgType type,
                               const QMessageLogContext &context,
                               const QString &message);

private:
  void *m_libHandle;

  // readline functions
  rl_crlf_t m_rl_crlf;
  rl_message_t m_rl_message;
  rl_on_new_line_t m_rl_on_new_line;
  rl_forced_update_display_t m_rl_forced_update_display;
  rl_completion_matches_t m_rl_completion_matches;
  rl_bind_key_t m_rl_bind_key;
  rl_callback_handler_install_t m_rl_callback_handler_install;
  rl_callback_read_char_t m_rl_callback_read_char;
  rl_callback_handler_remove_t m_rl_callback_handler_remove;
  add_history_t m_add_history;

  QMutex m_commandsLock;

  struct Command
  {
    QStringList arguments;
    QString description;
    bool hasValidReceiver;
    QPointer<const QObject> receiver;
    QtPrivate::QSlotObjectBase *slotObj;
  };

  QMap<QString, Command> m_commands;

  QSocketNotifier *m_stdinListener;
  bool m_running;
  int m_maxCommandHelpWidth;
};

// ReadLine

class ReadLine : public QObject
{
  Q_OBJECT

public:
  ReadLine(QObject *parent = nullptr);
  ~ReadLine();

public:
  bool isValid() const;
  void setPrompt(const QString &prompt);
  QString prompt() const;
  void start();
  void stop();
  void runCommand(const QString &command, const QStringList &arguments);

  // addCommand to a QObject slot
  template <typename Func1>
  inline bool
  addCommand(const QString &name,
             const QStringList &args,
             const QString &description,
             const typename QtPrivate::FunctionPointer<Func1>::Object *receiver,
             Func1 slot)
  {
    typedef QtPrivate::FunctionPointer<Func1> SlotType;

    // compilation error if the slot has arguments.
    Q_STATIC_ASSERT_X(int(SlotType::ArgumentCount) == 1,
                      "The slot must have one argument.");

    Q_STATIC_ASSERT_X((QtPrivate::AreArgumentsCompatible<
                        typename SlotType::Arguments,
                        typename QtPrivate::List<const QStringList &>>::value),
                      "The one and only argument to the slot must be 'const "
                      "QCommandLineParser&'");

    return addCommandImpl(
      name,
      args,
      description,
      receiver,
      new QtPrivate::QSlotObject<Func1, typename SlotType::Arguments, void>(
        slot));
  }

private:
  QPointer<ReadLinePrivate> m_private;
  QString m_prompt;

  bool addCommandImpl(const QString &name,
                      const QStringList &args,
                      const QString &desc,
                      const QObject *receiver,
                      QtPrivate::QSlotObjectBase *slotObj);
};

#endif // ifndef READLINE_H
