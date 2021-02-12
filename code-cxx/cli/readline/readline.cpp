
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

o C++ gurantees that static instance_ gets properly initialised when there are
multiple threads that calls this function. However, accessing members via this
is different matter and if necessary, need to use lock to protect calls to
member functions.

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

libreadline function pointers used:

rl_attempted_completion_function;
rl_completion_matches
rl_on_new_line

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
not find any other way than this. Well, see more compact version.

*/

char *ReadLinePrivate::completionGenerator_(const char *text, int state)
{
  static size_t len = 0, index = 0;

  // this is new word to complete so initialize now; saves the length of text
  // and set index 0.
  if (0 == state)
  {
    len = strlen(text);
    index = 0;
  }

  char *match{nullptr};

  for (; index < m_commands.size(); ++index)
  {
    // if partial match is found. since index is static, it is kept while this
    // function gets called multiple times with state > 0 and can discern
    // between what's searched and what's not.
    if (m_commands[index].name().compare(0, len, text) == 0)
    {
      match = strdup(m_commands[index].name().c_str());
      // to exclude it from next search
      ++index;
      break;
    }
  }

  // if no match is found, return null.
  return match;
}

/* ={==========================================================================
note that tried to avoid using "static vectors which grows dynamically" but
not find any other way than this since:

1. for ">><tab>" case

>> generator: t: , s: 0
generator: add help
generator: add test2
generator: add test1
return: help
generator: t: , s: 1
return: test2
generator: t: , s: 2
return: test1
generator: t: , s: 3
return null

help   test1  test2

2. for ">>xxx<tab>" case. 
"test" completed even when returned "test1, test2". and then "test<TAB>"

>> t<TAB>generator: t: t, s: 0
generator: add test2
generator: add test1
return: test2
generator: t: t, s: 1
return: test1
generator: t: t, s: 2
return null

>> test<TAB>generator: t: test, s: 0
generator: add test2
generator: add test1
return: test2 (match index 0)
generator: t: test, s: 1
return: test1 (match index 1)
generator: t: test, s: 2
return null (match index 2)

test1  test2

that is "match_index" is to say "no more match to return"

*/

#if 0
// #define DEBUG_COMPLETION_ON

char *ReadLinePrivate::completionGenerator_(const char *text, int state)
{
  static std::vector<std::string> matches{};
  static size_t match_index{0};

#ifdef DEBUG_COMPLETION_ON
  std::cout << "generator: t: " << std::string(text) << ", s: " << state
            << std::endl;
#endif

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
#ifdef DEBUG_COMPLETION_ON
        std::cout << "generator: add " << word.name() << std::endl;
#endif
        matches.push_back(word.name());
      }
    }
  }

  if (match_index >= matches.size())
  {
#ifdef DEBUG_COMPLETION_ON
    std::cout << "return null\n";
#endif 
    // We return nullptr to notify the caller no more matches are available.
    return nullptr;
  }
  else
  {
#ifdef DEBUG_COMPLETION_ON
    std::cout << "return: " << matches[match_index] << std::endl;
#endif 
    auto ret = strdup(matches[match_index].c_str());
    match_index++;
    return ret;

    // // Return a malloc'd char* for the match. The caller frees it.
    // return strdup(matches[match_index++].c_str());
  }
}
#endif

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

    // do not work
    //if (matches)
    //{
    //  size_t index{0};
    //  char *entry = matches[index];
    //  for (; index < 3; index++)
    //  {
    //    std::cout << "match: " << entry << std::endl;
    //  }
    //}
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

  // os << "size : " << m_commands.size() << "\n";

  os.setf(std::ios::left);

  for (const auto &e : m_commands)
  {
    os << std::setw(20) << e.name() << " : " << e.description() << "\n";
    os << std::setw(20) << " "
       << " : " << e.args() << "\n";
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
