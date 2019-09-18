#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <queue>
#include <future>

#include "gmock/gmock.h"

#include "scli.h"
#include "statemachine.h"
#include "bleaudiostreamer.h"

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

TEST(BleAudioStreamer, runStateMachine)
{
  enum {
    initialState
  };

  StateMachine machine;
  machine.setObjectName("machine");

  EXPECT_TRUE(machine.addState(initialState));
  EXPECT_TRUE(machine.setInitialState(initialState));

  std::map<int, unsigned> enters;
  std::map<int, unsigned> exits;

  auto entered = [&](int state)
  { 
    enters[state]++;
    // std::cout << "entered state: " << state << std::endl;
  };

  auto exited = [&](int state)
  {
    exits[state]++;
    // std::cout << "exited state: " << state << std::endl;
  };

  EXPECT_TRUE(machine.connect(entered, exited));

  EXPECT_TRUE(machine.start());

  EXPECT_TRUE(machine.isRunning());

  // return current state
  EXPECT_EQ(machine.state(), int(initialState));

  machine.stop();

  EXPECT_FALSE(machine.isRunning());

  EXPECT_THAT(enters[initialState], 1);
  EXPECT_THAT(exits.size(), 0);
}

TEST(DISABLED_BleAudioStreamer, runLoopToTestCli)
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

TEST(BleAudioStreamer, runBleAudioStreamer)
{
  BleAudioStreamer streamer;

  for (int i = 0; i < 5; i++)
  {
    streamer.postMessage(i*100);
  }

  std::this_thread::sleep_for(std::chrono::seconds(5));
}

namespace
{
  static bool sendMeddage(std::string const &command, void *data)
  {
    BleAudioStreamer *streamer = static_cast<BleAudioStreamer*>(data);
    streamer->postMessage(BleAudioStreamer::DeviceConnectedEvent);
    std::cout << "send message : " << 100  << std::endl;
    return true;
  }
} // namesapce

TEST(BleAudioStreamer, runLoopToTestStreamer)
{
  CommandHandler handler;
  BleAudioStreamer streamer;
  bool running{true};

  handler.addCommand("q", "quit", quitCommand, &running);
  handler.addCommand("h", "help", helpCommand, &handler);
  handler.addCommand("send", "send message", sendMeddage, &streamer);

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


TEST(BleAudioStreamer, runDefinedFsm)
{
  BleAudioStreamer streamer;
  std::string path{"/org/bluez/hci0/dev_D4_B8_FF_66_D6_25"};

  // device connected
  streamer.onDevicePropertyChange(path, "Connected", "true");
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  // transport added
  path.append("/fd");
  streamer.onTransportAdded(path);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  // player added
  path.append("/player");
  streamer.onPlayerAdded(path);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  // transport pending
  streamer.onTransportPropertyChange(path, "State", "pending");
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  // transport active
  streamer.onTransportPropertyChange(path, "State", "active");
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  // player playing
  streamer.onPlayerPropertyChange(path, "Status", "playing");
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  // player stopped
  streamer.onPlayerPropertyChange(path, "Status", "stopped");
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  // transport idle
  streamer.onTransportPropertyChange(path, "State", "idle");
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  // player removed
  streamer.onPlayerRemoved(path);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  // transport removed
  streamer.onTransportRemoved(path);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  // device disconnected
  streamer.onDevicePropertyChange(path, "Connected", "false");
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
}


// ={=========================================================================

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
