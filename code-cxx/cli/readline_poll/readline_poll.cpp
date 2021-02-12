#include "readline_poll.h"
#include "slog.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <dlfcn.h>
#include <iterator>
#include <signal.h>
#include <sstream>
#include <sys/epoll.h>
#include <unistd.h> // STDIN_FILENO

#ifndef STDIN_FILENO
#error "STDIN_FILENO is not defined"
#endif

namespace
{
  // The function pointer points a particular type, function type.
  //
  // bool (const string &, const string &);        // function's type
  // bool (*pf)(const string &, const string &);   // pointer to function's type
  //
  // The () around *pf are necessary. Otherwise, declare pf as a function that
  // returns a pointer to bool.

  typedef char *rl_compentry_func_t(const char *, int);
  typedef int rl_command_func_t(int, int);
  typedef void rl_vcpfunc_t(char *);

  typedef int (*rl_on_new_line_t)(void);
  typedef char **(*rl_completion_matches_t)(const char *,
                                            rl_compentry_func_t *);
  typedef int (*rl_bind_key_t)(int, rl_command_func_t *);
  typedef void (*rl_callback_handler_install_t)(const char *, rl_vcpfunc_t *);
  typedef void (*rl_callback_read_char_t)(void);
  typedef void (*rl_callback_handler_remove_t)(void);
  typedef void (*add_history_t)(const char *);

  // The libreadline function pointers, extracted by dlsym
  // note that can make then member variables
  rl_on_new_line_t _rl_on_new_line                           = nullptr;
  rl_completion_matches_t _rl_completion_matches             = nullptr;
  rl_bind_key_t _rl_bind_key                                 = nullptr;
  rl_callback_handler_install_t _rl_callback_handler_install = nullptr;
  rl_callback_read_char_t _rl_callback_read_char             = nullptr;
  rl_callback_handler_remove_t _rl_callback_handler_remove   = nullptr;
  add_history_t _add_history                                 = nullptr;

} // namespace

/*
1. use simpler singleton code than the reference:

std::shared_ptr<ReadLine> ReadLine::instance()
{
  std::lock_guard<std::mutex> locker(mInstanceLock);

  if (!mInstance)
  {
    class make_shared_enabler : public ReadLine {};
    mInstance = std::make_shared<make_shared_enabler>();
  }

  return mInstance;
}

2. the reference uses IReadLine and 

class ReadLine : public IReadLine,

this causes error on "static ReadLine instance_" since it's ABC. so has to
remove that to use simple singleton

*/

ReadLine &ReadLine::instance()
{
  // std::lock_guard<std::mutex> lock(m_instance_lock);

  static ReadLine instance_;

  return instance_;
}

ReadLine::ReadLine()
    : m_pollloop(std::make_shared<PollLoop>("ReadLine"))
{
  initialise_();

  // register built-in commands. use public interface directly.
  addCommand("quit",                           // name
             "quit this interactive terminal", // desc
             "quit",                           // help
             std::bind(&ReadLine::quitCommand_,
                       this,
                       std::placeholders::_1,
                       std::placeholders::_2), // handler
             "");                              // args

  addCommand("help",                               // name
             "show commmands available and helps", // desc
             "help [command]",                     // help
             std::bind(&ReadLine::helpCommand_,
                       this,
                       std::placeholders::_1,
                       std::placeholders::_2), // handler
             "");                              // args

  LOG_MSG("readline is created");
}

ReadLine::~ReadLine()
{
  m_pollloop->stop();
}

/* ={==========================================================================
completion functions for libreadline
*/

char *ReadLine::completionGenerator_(const char *text, int state)
{
  static size_t len = 0, index = 0;

  // std::lock_guard<std::mutex> lock(m_lock);

  // this is new word to complete so initialize now; saves the length of text
  // and set index 0.
  if (0 == state)
  {
    len   = strlen(text);
    index = 0;
  }

  char *match{nullptr};

  for (; index < m_commands.size(); ++index)
  {
    // if partial match is found. since index is static, it is kept while this
    // function gets called multiple times with state > 0 and can discern
    // between what's searched and what's not.
    if (m_commands[index].m_name.compare(0, len, text) == 0)
    {
      match = strdup(m_commands[index].m_name.c_str());
      ++index;
      break;
    }
  }

  // if no match is found, return null.
  return match;
}

char *ReadLine::completion_generator_(const char *text, int state)
{
  // std::lock_guard<std::mutex> lock(m_instance_lock);
  return instance().completionGenerator_(text, state);
}

char **ReadLine::completer_(const char *text, int start, int end)
{
  char **matches{nullptr};

  // If this word is at the start of the line, then it is a command to complete.
  if (start == 0)
    matches = _rl_completion_matches(text, completion_generator_);

  return matches;
}

void ReadLine::commandLineHandler_(const char *text)
{
  // std::lock_guard<std::mutex> lock(m_lock);

  if (!text)
  {
    // use public interface
    quit();
    return;
  }

  std::string line_{text};

  // do not need this since it's done when use stringstream
  //
  // // trim leading spaces, (pos, end]
  // auto startpos = line_.find_first_not_of(" \t");
  // if (startpos != std::string::npos)
  //   line_ = line_.substr(startpos);
  //
  // // trim trailing spaces, (0, pos+1)
  // auto endpos = line_.find_last_not_of(" \t");
  // if (endpos != std::string::npos)
  //   line_ = line_.substr(0, endpos + 1);

  // ok, "the line" is ready
  if (line_.size())
  {
    std::stringstream ss{line_};

    std::istream_iterator<std::string> it{ss};
    std::istream_iterator<std::string> end{};
    std::vector<std::string> args{it, end};

    // no checks on the size of args since it always wiil not be empty. right?
    std::string command{args[0]};
    args.erase(args.begin());

    runCommand_(command, args);

    // add it to readline
    _add_history(line_.c_str());
  }
}

void ReadLine::commandline_handler_(char *text)
{
  // std::lock_guard<std::mutex> lock(m_instance_lock);
  instance().commandLineHandler_(text);
}

/* ={==========================================================================
Unlike "readline" which use global variables for functions exposed by
libreadline, here uses dlsym instead. Why is that? The comment is:

  Because we don't have access to the readline headers of library in the build
  system, we use dlsym to try and get the symbols.

*/
void ReadLine::initialise_()
{
  // clear off error messages
  (void)dlerror();

  // void *handle = dlopen("/usr/lib/libreadline.so", RTLD_NOW);
  void *handle = dlopen("libreadline.so", RTLD_NOW);
  if (nullptr == handle)
  {
    LOG_MSG("failed to open libreadline.so {%s}", dlerror());
    goto failed;
  }

#define GET_RL_FUNC(f)                                                         \
  do                                                                           \
  {                                                                            \
    _##f = reinterpret_cast<f##_t>(dlsym(handle, "" #f ""));                   \
    if (!_##f)                                                                 \
    {                                                                          \
      LOG_MSG("failed to get symbol '" #f "' (%s)", dlerror());                \
      goto failed;                                                             \
    }                                                                          \
  } while (0)

  // get pointers and save them
  GET_RL_FUNC(rl_on_new_line);
  GET_RL_FUNC(rl_completion_matches);
  GET_RL_FUNC(rl_bind_key);
  GET_RL_FUNC(rl_callback_handler_install);
  GET_RL_FUNC(rl_callback_read_char);
  GET_RL_FUNC(rl_callback_handler_remove);
  GET_RL_FUNC(add_history);

#undef GET_RL_FUNC

  // void *dlsym(void *handle, char *symbol);
  //
  // rl_on_new_line_t f_on_new_line =
  //   (rl_on_new_line_t)dlsym(handle, "rl_on_new_line");
  //
  // can call like:
  //
  // f_on_new_line();
  //
  // so why do like this?

  {
    void **rl_attempted_completion_function = reinterpret_cast<void **>(
      dlsym(handle, "rl_attempted_completion_function"));
    if (rl_attempted_completion_function)
    {
      *rl_attempted_completion_function = reinterpret_cast<void *>(completer_);
    }

    rl_command_func_t *rl_complete =
      reinterpret_cast<rl_command_func_t *>(dlsym(handle, "rl_complete"));
    if (rl_complete)
    {
      _rl_bind_key('\t', rl_complete);
    }
  }

  // all goes well. that is not close handle as long as it is live.
  return;

failed:
  if (handle)
    dlclose(handle);

  return;
}

/* ={==========================================================================
run a command. the reference code supports partial match:

o if there is one partial match then run it since will be "cmdRef = it"
o if there are multiple partial match, errStr will be set and shows error
  message.

{
    std::string errStr;

    std::vector<ReadLine::ReadLineCommand>::const_iterator cmdRef = mCommands.end();
    std::vector<ReadLine::ReadLineCommand>::const_iterator it = mCommands.begin();

    for (; it != mCommands.end(); ++it)
    {
        // partial match
        if (it->name.compare(0, cmdStr.length(), cmdStr) == 0)
        {
            // exact matches always work
            if (it->name.length() == cmdStr.length())
            {
                cmdRef = it;
                break;
            }

            // check if we don't already have a match, if we do then we have
            // multiple matches and have to report an error.
            if (cmdRef == mCommands.end())
            {
                cmdRef = it;
            }
            else
            {
                errStr += it->name + " ";
            }
        }
    }

    //
    if (cmdRef == mCommands.end())
    {
        fprintf(stderr, "%s: No such command.\n", cmdStr.c_str());
    }
    else if (!errStr.empty())
    {
        fprintf(stderr, "Ambiguous command '\%s', possible commands: %s %s\n",
                cmdStr.c_str(), cmdRef->name.c_str(), errStr.c_str());
    }
    else if (cmdRef->handler != nullptr)
    {
        // Call the command handler, if it returns false we stop the poll loop
        // which effectively quits the app
        cmdRef->handler(shared_from_this(), args);
    }
}

Here only support exact match

*/

void ReadLine::runCommand_(const std::string &command,
                           const std::vector<std::string> &args)
{
  if (command.size())
  {
    for (const auto &e : m_commands)
    {
      if (e.m_name == command)
        e.m_handler(shared_from_this(), args);
    }
  }
}

/* ={==========================================================================
return true if readline class is in valid state. uses m_pollloop shared pointer
for this purpose since instance() do not return to check this out.

bool ReadLine::isValid() const
{
  return m_pollloop;
}
*/

/* ={==========================================================================
 add a new command to the readline loop. use a lock.
*/
void ReadLine::addCommand(const std::string &name,
                          const std::string &description,
                          const std::string &help,
                          handler f,
                          const std::string &args)
{
  // std::lock_guard<std::mutex> lock(m_lock);

  m_commands.emplace_back(Command{name, description, help, f, args});
}

/* ={==========================================================================
run single command once from argc/argv.
*/
void ReadLine::runCommand(int argc, const char **argv)
{
  std::vector<std::string> args{};

  for (int i = 0; i < argc; ++i)
  {
    args.emplace_back(argv[i]);
  }

  if (args.size())
  {
    // std::lock_guard<std::mutex> lock(m_lock);

    std::string command{args[0]};
    args.erase(args.begin());
    runCommand_(command, args);
  }
}

/* ={==========================================================================
run loop

={=========================================================================
The GNU Readline Library

Documentation

The documentation for the Readline and History libraries appears in the `doc'
subdirectory. There are three texinfo files and two Unix-style manual pages
describing the facilities available in the Readline and History libraries. The
texinfo files include both user and programmer's manuals. The current manuals
are:

The GNU Readline Library

https://tiswww.case.edu/php/chet/readline/readline.html

={=========================================================================
2.4 Readline Convenience Functions

2.4.12 Alternate Interface

An alternate interface is available to plain readline(). Some applications need
to interleave keyboard I/O with file, device, or window system I/O, typically by
using a main loop to select() on various file descriptors. To accommodate this
need, readline can also be invoked as a `callback' function "from an event loop."
There are functions available to make this easy.


Function: void rl_callback_handler_install (const char *prompt, rl_vcpfunc_t *lhandler)

Set up the terminal for readline I/O and display the initial expanded value of
prompt. Save the value of lhandler to use as a handler function to call when a
complete line of input has been entered. The handler function receives the text
of the line as an argument. As with readline(), the handler function should free
the line when it it finished with it.

Function: void rl_callback_read_char (void)

Whenever an application determines that keyboard input is available, it should
call rl_callback_read_char(), which will read the next character from the
current input source. If that character completes the line,
rl_callback_read_char will invoke the lhandler function installed by
"rl_callback_handler_install" to process the line. Before calling the lhandler
function, the terminal settings are reset to the values they had before calling
rl_callback_handler_install. If the lhandler function returns, and the line
handler remains installed, the terminal settings are modified for Readline's use
again. EOF is indicated by calling lhandler with a NULL line.


2.5 Readline Signal Handling

When an application is using the callback interface (see section 2.4.12
Alternate Interface), Readline installs signal handlers only for the duration of
the call to rl_callback_read_char. Applications using the callback interface
should be prepared to clean up Readline's state if they wish to handle the
signal before the line handler completes and restores the terminal state.

If an application using the callback interface wishes to have Readline install
its signal handlers at the time the application calls
rl_callback_handler_install and remove them only when a complete line of input
has been read, it should set the rl_persistent_signal_handlers variable to a
non-zero value. This allows an application to defer all of the handling of the
signals Readline catches to Readline. Applications should use this variable with
care; it can result in Readline catching signals and not acting on them (or
allowing the application to react to them) until the application calls
rl_callback_read_char. This can result in an application becoming less
responsive to keyboard signals like SIGINT. If an application does not want or
need to perform any signal handling, or does not need to do any processing
between calls to rl_callback_read_char, setting this variable may be desirable.

*/

/* ={==========================================================================
handler for capturing ctrl-c and restoring the terminal state
*/
void ReadLine::signalHandler(int)
{
  // std::lock_guard<std::mutex> lock(m_instance_lock);

  // needs to be "instance().quit()" since signal context don't know about
  // instance otherwise gets:
  // : error: ‘instance_’ was not declared in this scope
  instance().quit();
}

void ReadLine::run()
{
  std::unique_lock<std::mutex> quit_lock(m_quit_lock);

  // *sig-int*
  // When the user types the terminal interrupt character (usually Control-C),
  // the terminal driver sends this signal to the foreground process group. The
  // default action for this signal is to terminate the process.

  signal(SIGINT, signalHandler);

  // install the handler
  _rl_callback_handler_install("> ", commandline_handler_);

  if (false == m_pollloop->addSource(shared_from_this(), STDIN_FILENO, EPOLLIN))
  {
    LOG_MSG("failed to add stdin source to poll loop");
    return;
  }

  // start pollloop thread
  m_pollloop->start();

  while (false == m_quit)
  {
    m_cond.wait(quit_lock);
  }

  m_pollloop->delSource(shared_from_this());
  m_pollloop->stop();

  // uninstall the handler
  _rl_callback_handler_remove();
}

/* ={==========================================================================
IPollSouce::process(), callbacks from the pollloop
*/

void ReadLine::process(const std::shared_ptr<IPollLoop> &poll, uint32_t events)
{
  if (events & EPOLLIN)
  {
    // input is ready and call commandline_handler_
    _rl_callback_read_char();
  }
}

/* ={==========================================================================
trigger an exit from the realine loop
*/
void ReadLine::quit() const
{
  std::unique_lock<std::mutex> quit_lock(m_quit_lock);

  m_quit = true;
  m_cond.notify_all();
}

/* ={==========================================================================
default command handlers

On hander signatures. "readline" uses:

using handler =
  std::function<bool(const std::string &, const std::vector<std::string> &)>;

and each command handler gets the command name as a first param. Here in
"readline_pollloop" uses different handler signature:

using handler = 
  std::function<void(const std::shared_ptr<IReadLine> &source,
                                   const std::vector<std::string> &args)>;

"the command name" seems not necessary and how about std::shared_ptr<IReadLine>?

o no need to use m_lock in handlers since it runs in runCommand_() context which
has a lock already if used.

*/

void ReadLine::quitCommand_(const std::shared_ptr<ReadLine> &readline,
                            const std::vector<std::string> &args)
{
  // how about "quit();" simply?
  readline->quit();
}

void ReadLine::helpCommand_(const std::shared_ptr<ReadLine> &readline,
                            const std::vector<std::string> &args)
{
  // no args, prints out name and decs only
  if (args.empty())
  {
    for (const auto &e : m_commands)
    {
      readline->printLine("%-16s %s",
                          e.m_name.c_str(),
                          e.m_description.c_str());
    }
  }
  // if arg(command), prints out help and args as well for that command
  else
  {
    for (const auto &e : m_commands)
    {
      // TODO: going to work since the input args don't have command at 0?
      if (e.m_name == args[0])
      {
        readline->printLine("%-16s %s\n",
                            e.m_name.c_str(),
                            e.m_description.c_str());

        if (e.m_help.size())
          readline->printLine("%s\n", e.m_help.c_str());

        if (e.m_args.size())
          readline->printLine("%s\n", e.m_args.c_str());
      }
    }
  }
}

/* ={==========================================================================
print out from the handler

https://tiswww.case.edu/php/chet/readline/readline.html
2.4.6 Redisplay

Function: int rl_on_new_line (void)
Tell the update functions that we have moved onto a new (empty) line, usually
after outputting a newline.

*/

void ReadLine::printLine(const char *fmt, ...) const
{
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
  printf("\n");

  _rl_on_new_line();
}
