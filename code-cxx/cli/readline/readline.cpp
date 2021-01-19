
#include "readline.h"
#include "slog.h"
#include <iomanip>
#include <iostream>
#include <sstream>

#include <cassert>
#include <stdio.h>
#include <stdlib.h>

// standard readline include files
#include <readline/history.h>
#include <readline/readline.h>

#define G_UNLIKELY(x) __glibc_unlikely(x)

/* ={==========================================================================
TODO: return reference of instance than shared pointer?

*/

// ReadLinePrivate *ReadLinePrivate::instance()
std::shared_ptr<ReadLinePrivate> ReadLinePrivate::instance()
{
  static std::shared_ptr<ReadLinePrivate> instance_{nullptr};

  if (G_UNLIKELY(instance_ == nullptr))
  {
    // instance_ = std::make_shared<ReadLinePrivate>();
    instance_ = std::shared_ptr<ReadLinePrivate>(new ReadLinePrivate);
  }

  // return instance_.get();
  return instance_;
}

/* ={==========================================================================
o add "help" to list out commands registered by default in readline class.
*/

ReadLinePrivate::ReadLinePrivate()
{
  rl_attempted_completion_function = completer_;

  m_commands.emplace_back(Command("help",
                                  "shows help message",
                                  std::bind(&ReadLinePrivate::helpCommand_,
                                            this,
                                            std::placeholders::_1,
                                            std::placeholders::_2),
                                  ""));
}

ReadLinePrivate::~ReadLinePrivate() {}

/* ={==========================================================================
note that tried to avoid using "static vectors which grows dynamically" but
not find any other way than this since:

1. for ">><tab>" case, called many times with state 0 and one for other
state.

2. for ">>xxx<tab>" case, called once with state 0.
*/

char *ReadLinePrivate::completionGenerator_(const char *text, int state)
{
  static std::vector<std::string> matches{};
  static size_t match_index{0};

  // std::cout << "generator: t: " << std::string(text) << ", s: " << state
  //           << std::endl;

  // This function is called with state=0 the first time; subsequent calls are
  // with a nonzero state. state=0 can be used to perform one-time
  // initialization for this completion session.

  if (0 == state)
  {
    // During initialization, compute the actual matches for 'text' and keep
    // them in a static vector.

    matches.clear();
    match_index = 0;

    // Collect a vector of matches: vocabulary words that begin with text.
    std::string textstr{text};

    for (auto word : m_commands)
    {
      // if found match, add it to the list
      // int std::sting.compare
      // (size_type pos1, size_type count1, const CharT* s) const;

      if (word.name().size() >= textstr.size() &&
          word.name().compare(0, textstr.size(), textstr) == 0)
      {
        // std::cout << "generator: add " << word << std::endl;
        matches.push_back(word.name());
      }
    }
  }

  if (match_index >= matches.size())
  {
    // We return nullptr to notify the caller no more matches are available.
    return nullptr;
  }
  else
  {
    // std::cout << "return: " << matches[match_index] << std::endl;
    auto ret = strdup(matches[match_index].c_str());
    match_index++;
    return ret;

    // // Return a malloc'd char* for the match. The caller frees it.
    // return strdup(matches[match_index++].c_str());
  }
}

char *ReadLinePrivate::completion_generator_(const char *text, int state)
{
  // ReadLinePrivate *self = instance();
  auto self = instance();
  return self->completionGenerator_(text, state);
}

char **ReadLinePrivate::completer_(const char *text, int start, int end)
{
  // note if not set nullptr, it crahses
  char **matches{nullptr};

  // Don't do filename completion even if our generator finds no matches.
  //
  // without this, it shows the files in the current directory:
  //
  // >> add<TAB>
  // CMakeCache.txt       CMakeFiles/          CTestTestfile.cmake  Makefile             bin/                 cmake_install.cmake  gmock/               googletest/          lib/                 test_readline
  // >>
  //
  // NOTE: so with this, shows only the commands registered.
  //
  // Variable:
  // int rl_attempted_completion_over
  //
  // If an application-specific completion function assigned to
  // rl_attempted_completion_function sets this variable to a non-zero value,
  // Readline will not perform its default filename completion even if the
  // application's completion function returns no matches. It should be set only
  // by an application's completion function.

  rl_attempted_completion_over = 1;

  // ">> <TAB>" also gets "start is 0"
  if (start == 0)
  {
    // std::cout << "start: " << start << std::endl;
    matches = rl_completion_matches(text, completion_generator_);
  }

  return matches;
}

/* ={==========================================================================
add an handler context item which wraps up name, desc, function pointer and data
pointer into a vector

TODO: do not check if command is already exist or not

*/

void ReadLinePrivate::addCommand(const std::string &name,
                                 const std::string &description,
                                 handler f,
                                 const std::string &args)
{
  m_commands.emplace_back(Command(name, description, f, args));
}

/* ={==========================================================================
return false if input command is empty or cannot find input command in commands
vector.

1. Since it's single call chain from console.runCommand, command ana args are
reference.

2. args in Command is just for displaying purpose if used since not used when
run a command. args of running command comes from the input.

*/

bool ReadLinePrivate::runCommand(const std::string &command,
                                 const std::vector<std::string> &args)
{
  if (command.size())
  {
    for (const auto &e : m_commands)
    {
      // comment out since it seems it's not necessary
      // if (e.isMatch(command))
      
      if (e.name() == command)
        return e.fire(command, args);
    }
  }

  return false;
}

/* ={==========================================================================
*/
bool ReadLinePrivate::helpCommand_(const std::string &command,
                                   const std::vector<std::string> &args)
{
  std::ostringstream os{};

  os << "size : " << m_commands.size() << "\n";

  os.setf(std::ios::left);

  for (const auto &e : m_commands)
  {
    os << std::setw(20) << e.name() << " - " << e.description() << "\n";
  }

  std::cout << os.str() << std::endl;

  rl_on_new_line();
}

/* ={==========================================================================

*/

ReadLine::ReadLine()
    : m_private(ReadLinePrivate::instance())
{
  if (m_private == nullptr)
  {
    LOG_MSG("failed to get readline instance");
  }
}

ReadLine::~ReadLine() {}

void ReadLine::runCommand(const std::string &command,
                          const std::vector<std::string> &args)
{
  assert(m_private);

  m_private->runCommand(command, args);
}

void ReadLine::addCommand(const std::string &name,
                  const std::string &description,
                  handler f,
                  const std::string &args)
{
  assert(m_private);

  m_private->addCommand(name, description, f, args);
}
