#include <chrono>
#include <condition_variable>
#include <forward_list>
#include <iostream>
#include <limits>
#include <list>
#include <memory>
#include <mutex>
#include <regex>
#include <set>
#include <thread>
#include <vector>

#include "readline_poll.h"
#include "gmock/gmock.h"

// use lpi log
#include "slog.h"

using namespace std;
using namespace std::placeholders;
using namespace testing;

namespace
{
  void testCommand1(const std::shared_ptr<ReadLine> &source,
                    const std::vector<std::string> &args)
  {
    LOG_MSG("testCommand1 runs");
  }

  void testCommand2(const std::shared_ptr<ReadLine> &source,
                    const std::vector<std::string> &args)
  {
    LOG_MSG("testCommand2 runs");

    for (auto const &e : args)
    {
      LOG_MSG("testCommand2 runs: args: %s", e.c_str());
    }
  }
} // namespace

/*
={=============================================================================
run a single command than running a loop

cannot run both tests. need to disable one or the other:
TEST(TestReadLinePoll, DISABLED_run_command_loop)

[ RUN      ] TestReadLinePoll.run_single_command
LOG| F:pollloop.cpp C:PollLoop::PollLoop(const string&, unsigned int, long int) L:00056  check on TFD_TIMER_ABSTIME {1}
LOG| F:pollloop.cpp C:PollLoop::PollLoop(const string&, unsigned int, long int) L:00057  check on EPOLLWAKEUP {536870912}
LOG| F:pollloop.cpp C:PollLoop::PollLoop(const string&, unsigned int, long int) L:00058  check on EPOLLDEFERRED {536870912}
LOG| F:pollloop.cpp C:PollLoop::PollLoop(const string&, unsigned int, long int) L:00059  pollloop is ctored
LOG| F:readline_poll.cpp C:ReadLine::ReadLine() L:00179  readline is created
LOG| F:test_readline_poll.cpp C:void {anonymous}::testCommand1(const std::shared_ptr<ReadLine>&, const std::vector<std::__cxx11::basic_string<char> >&) L:00029  testCommand1 runs
LOG| F:test_readline_poll.cpp C:void {anonymous}::testCommand2(const std::shared_ptr<ReadLine>&, const std::vector<std::__cxx11::basic_string<char> >&) L:00035  testCommand2 runs
LOG| F:test_readline_poll.cpp C:void {anonymous}::testCommand2(const std::shared_ptr<ReadLine>&, const std::vector<std::__cxx11::basic_string<char> >&) L:00039  testCommand2 runs: args: -x
LOG| F:test_readline_poll.cpp C:void {anonymous}::testCommand2(const std::shared_ptr<ReadLine>&, const std::vector<std::__cxx11::basic_string<char> >&) L:00039  testCommand2 runs: args: -p
LOG| F:test_readline_poll.cpp C:void {anonymous}::testCommand2(const std::shared_ptr<ReadLine>&, const std::vector<std::__cxx11::basic_string<char> >&) L:00039  testCommand2 runs: args: hello

*/

// TEST(TestReadLinePoll, run_single_command)
TEST(TestReadLinePoll, DISABLED_run_single_command)
{
  // add commands
#ifdef USE_SIMPLE_SINGLETON
  ReadLine::instance().addCommand(
    "test1",                                        // name
    "test command 1",                               // desc
    "test command which uses golbal free function", // help
    testCommand1,
    ""); // args

  ReadLine::instance().addCommand(
    "test2",                                        // name
    "test command 2",                               // desc
    "test command which uses golbal free function", // help
    testCommand2,
    ""); // args

  // const char *argv[] = {"test1", "-x", "-p", "hello"};
  const char *argv[] = {"test1"};
  int argc{1};

  // ReadLine::instance().runCommand(argc, const_cast<char **>(argv));
  ReadLine::instance().runCommand(argc, argv);

#else
  std::shared_ptr<ReadLine> readline = ReadLine::instance();

  ReadLine::instance()->addCommand(
    "test1",                                        // name
    "test command 1",                               // desc
    "test command which uses golbal free function", // help
    testCommand1,
    ""); // args

  readline->addCommand("test2",                                        // name
                       "test command 2",                               // desc
                       "test command which uses golbal free function", // help
                       testCommand2,
                       ""); // args

  // without args
  {
    const char *argv[] = {"test1"};
    int argc{1};

    readline->runCommand(argc, argv);
  }

  // with args
  {
    const char *argv[] = {"test2", "-x", "-p", "hello"};
    int argc{4};

    readline->runCommand(argc, argv);
  }

#endif
}

/*
={=============================================================================
use command loop

[ RUN      ] TestReadLinePoll.run_command_loop
>
help   quit   test1  test2

> help
quit             quit this interactive terminal
help             show commmands available and helps
test1            test command 1
test2            test command 2
> test1
LOG| F:test_readline_poll.cpp C:void {anonymous}::testCommand1(const std::shared_ptr<ReadLine>&, const std::vector<std::__cxx11::basic_string<char> >&) L:00029  testCommand1 runs
> test1 one two
LOG| F:test_readline_poll.cpp C:void {anonymous}::testCommand1(const std::shared_ptr<ReadLine>&, const std::vector<std::__cxx11::basic_string<char> >&) L:00029  testCommand1 runs
> test2 one two
LOG| F:test_readline_poll.cpp C:void {anonymous}::testCommand2(const std::shared_ptr<ReadLine>&, const std::vector<std::__cxx11::basic_string<char> >&) L:00035  testCommand2 runs
LOG| F:test_readline_poll.cpp C:void {anonymous}::testCommand2(const std::shared_ptr<ReadLine>&, const std::vector<std::__cxx11::basic_string<char> >&) L:00039  testCommand2 runs: args: one
LOG| F:test_readline_poll.cpp C:void {anonymous}::testCommand2(const std::shared_ptr<ReadLine>&, const std::vector<std::__cxx11::basic_string<char> >&) L:00039  testCommand2 runs: args: two

> quit
> [       OK ] TestReadLinePoll.run_command_loop (19256 ms)
[----------] 2 tests from TestReadLinePoll (19256 ms total)

*/

#ifndef USE_SIMPLE_SINGLETON

TEST(TestReadLinePoll, run_command_loop)
// TEST(TestReadLinePoll, DISABLED_run_command_loop)
{
  std::shared_ptr<ReadLine> readline = ReadLine::instance();

  ReadLine::instance()->addCommand(
    "test1",                                        // name
    "test command 1",                               // desc
    "test command which uses golbal free function", // help
    testCommand1,
    ""); // args

  readline->addCommand("test2",                                        // name
                       "test command 2",                               // desc
                       "test command which uses golbal free function", // help
                       testCommand2,
                       "-x agr1"); // args

  readline->run();
}

#endif

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
