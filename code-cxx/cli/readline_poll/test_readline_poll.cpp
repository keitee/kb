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
  }
} // namespace

/*
={=============================================================================
os-io-file

*/

TEST(TestReadLinePoll, run_single_command)
{
  // add commands
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
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
