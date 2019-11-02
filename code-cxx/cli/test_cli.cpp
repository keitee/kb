#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <queue>
#include <future>

#include "gmock/gmock.h"

#include "scli.h"
#include "console.h"

using namespace testing;

// (gdb) b AlgoList_Divide_Test::TestBody()


// ={=========================================================================
// cxx-cli-simple

static bool helpCommand(std::string const &command, void *data)
{
  CommandHandler *handler = static_cast<CommandHandler*>(data);
  handler->showCommands();
  return true;
}

static bool quitCommand(std::string const &command, void *data)
{
  bool *running = static_cast<bool*>(data);
  *running = false;
  return true;
}

static bool returnFalse(std::string const &command, void *data)
{
  return false;
}

static bool echoCommand(std::string const &command, void *data)
{
  std::cout << "the entered command : " << command << std::endl;
  return true;
}

TEST(Cli, unsupportedCommands)
{
  CommandHandler handler;

  handler.addCommand("h", "help", helpCommand, &handler);

  // run/process input command
  auto ret = handler.handle("q");

  EXPECT_THAT(ret, false);
}

TEST(Cli, showCommands)
{
  CommandHandler handler;
  bool running{true};

  handler.addCommand("h", "help", helpCommand, &handler);
  handler.addCommand("q", "quit", quitCommand, &running);

  // run/process input command
  auto ret = handler.handle("h");

  EXPECT_THAT(ret, true);
}

TEST(Cli, doCommand)
{
  CommandHandler handler;
  bool running{true};

  handler.addCommand("h", "help", helpCommand, &handler);
  handler.addCommand("q", "quit", quitCommand, &running);

  auto ret = handler.handle("q");

  EXPECT_THAT(ret, true);
  EXPECT_THAT(running, false);
}

TEST(Cli, checkReturnValueFromCommand)
{
  CommandHandler handler;
  bool running{true};

  handler.addCommand("q", "quit", quitCommand, &running);
  auto ret1 = handler.handle("q");

  handler.addCommand("return", "check return value", returnFalse, nullptr);
  auto ret2 = handler.handle("return");

  EXPECT_THAT(ret1, true);
  EXPECT_THAT(ret2, false);
}

TEST(Cli, runLoop)
{
  CommandHandler handler;
  bool running{true};

  handler.addCommand("q", "quit", quitCommand, &running);
  handler.addCommand("h", "help", helpCommand, &handler);
  handler.addCommand("return", "check return value", returnFalse, nullptr);
  handler.addCommand("echo", "echo command entered", echoCommand, nullptr);

  while (running)
  {
    // prompt
    std::cout << ":> " << std::flush;

    std::string line;
    if (std::getline(std::cin, line))
    {
      auto ret = handler.handle(line);
      if (!ret)
        std::cout << line << " is not handled" << std::endl;
    }
    else if (std::cin.bad())
    {
      std::cout << "std io error" << std::endl;
      break;
    }
    else if (std::cin.eof())
    {
      std::cout << "std eof" << std::endl;
      break;
    }
  } // while
}

// ={=========================================================================
// cxx-cli-readline
// 
// DO NOT RUN:
//
// [ RUN      ] Cli.useReadLine
// QSocketNotifier: Can only be used with threads started with QThread
// console >[       OK ] Cli.useReadLine (92 ms)
// [----------] 1 test from Cli (92 ms total)


TEST(Cli, useReadLine)
{
  // create the command handler and start it
  // Console *console = new Console(&app);
  Console *console = new Console();
  console->start();
}

// ={=========================================================================

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
