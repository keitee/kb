#ifndef READLINE_H
#define READLINE_H

// readline function types

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

class ReadLinePrivate : public QObject
{
  Q_OBJECT

public:
  static ReadLinePrivate *instance();

private:
  ReadLinePrivate(QObject *parent = nullptr);

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
};

class ReadLine : public QObject
{
  Q_OBJECT

  public:
    ReadLine(QObject *parent = nullptr);
    ~ReadLine();

  private:
    QPointer<ReadLinePrivate> m_private;
};

#endif /* ifndef READLINE_H
