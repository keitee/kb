#include <algorithm>
#include <deque>
#include <future>
#include <iostream>
#include <memory>
#include <vector>

#include "gmock/gmock.h"

#include "eventloop.h"

#include "scli.h" // scli

using namespace std;
using namespace testing;

// ={=========================================================================

void w1(int &v)
{
  v += 1;
  // std::cout << "w1: " << v << std::endl;
}

// posts many work but nothing is done since do not run event loop and process
// nothing.
//
// size is 10 but value is still 0.

TEST(EventLoop, NoWorkDone)
{
  int value{};

  EventLoop loop;

  // auto w1 = [&]()
  // {
  //   value += 1;
  // };

  for (int i = 0; i < 10; ++i)
    loop.invokeMethod(std::bind(w1, value));

  EXPECT_THAT(loop.size(), 10);
  EXPECT_THAT(value, 0);
}

// will flush() make difference? No since flush() simply post a wait task and
// wait to finish it but do not call run() so blocks there.

TEST(DISABLED_EventLoop, FlushNoWorkDone)
{
  int value{};

  EventLoop loop;

  for (int i = 0; i < 10; ++i)
    loop.invokeMethod(std::bind(w1, value));

  EXPECT_THAT(loop.size(), 10);

  loop.flush();

  EXPECT_THAT(value, 0);
}

// running on a different thread should make difference? nope see the comment on
// flush()

TEST(DISABLED_EventLoop, ThreadButDeadlock)
// TEST(EventLoop, ThreadButDeadlock)
{
  int value{};

  EventLoop loop;

  auto f1 = std::async(std::launch::async, [&]() {
    std::this_thread::sleep_for(chrono::milliseconds(1300));

    std::cout << "post works to event loop" << std::endl;
    std::cout << "value: " << value << std::endl;

    for (int i = 0; i < 10; ++i)
      loop.invokeMethod(std::bind(w1, std::ref(value)));

    loop.flush();

    std::cout << "value: " << value << std::endl;

    loop.quit(0);
  });

  loop.run();

  EXPECT_THAT(value, 10);
}

// so do not use flush()

TEST(EventLoop, ThreadWorkDone1)
{
  int value{};

  EventLoop loop;

  auto f1 = std::async(std::launch::async, [&]() {
    // std::this_thread::sleep_for(chrono::milliseconds(300));

    // shall use std::ref
    for (int i = 0; i < 10; ++i)
      loop.invokeMethod(std::bind(w1, std::ref(value)));

    EXPECT_THAT(loop.size(), 10);

    loop.quit(0);
  });

  // blocks here
  loop.run();

  EXPECT_THAT(value, 10);
}

// same as above but use the convenient form of invokeMethod()

TEST(EventLoop, ThreadWorkDone2)
{
  int value{};

  EventLoop loop;

  auto f1 = std::async(std::launch::async, [&]() {
    // std::this_thread::sleep_for(chrono::milliseconds(300));

    // shall use std::ref
    for (int i = 0; i < 10; ++i)
      loop.invokeMethod(w1, std::ref(value));

    EXPECT_THAT(loop.size(), 10);

    loop.quit(0);
  });

  // blocks here
  loop.run();

  EXPECT_THAT(value, 10);
}

namespace
{
  class Work1
  {
    public:
      void printArgs(int v1, int v2, int v3)
      {
        std::cout << "v1: " << v1 
          << ", v2: " << v2 << ", v3: " << v3 << std::endl;
      }
  };

#if 0
  // TODO: HOW possible without using target??
  //
  // bool ASRequestPrivate::doSendReply(unsigned int code)
  // {
  // 
  //     // otherwise running on a different thread so need to invoke from the
  //     // event loop thread
  //     return m_eventLoop.invokeMethod(&ASRequestPrivate::marshallAndSendReply,
  //                                     reply, code, headers, body);
  // }

  class Work2
  {
    private:
      const EventLoop m_eventloop;

    public:
      Work2(const EventLoop &eventloop)
        : m_eventloop(eventloop)
      {}

      void pushWorks();

      void printArgs(int v1, int v2, int v3)
      {
        std::cout << "v1: " << v1 
          << ", v2: " << v2 << ", v3: " << v3 << std::endl;
      }
  };

  // cause compile error
  void Work2::pushWorks()
  {
    for (int i = 0; i < 10; ++i)
      m_eventloop.invokeMethod(&Work2::printArgs, 10, 20, 30);
  }
#endif
}

// may use target with invokeMethod()

TEST(EventLoop, InvokeWithThis)
{
  int value{};

  EventLoop loop;
  Work1 w;

  auto f1 = std::async(std::launch::async, [&]() {

    for (int i = 0; i < 10; ++i)
      loop.invokeMethod(&Work1::printArgs, &w, 10, 20, 30);

    EXPECT_THAT(loop.size(), 10);

    loop.quit(0);
  });

  // blocks here
  loop.run();
}

#if 0
TEST(EventLoop, InvokeWithoutThis)
{
  int value{};

  EventLoop loop;

  auto f1 = std::async(std::launch::async, [&]() {

      Work2 w(loop);
      w.pushWorks();

      EXPECT_THAT(loop.size(), 10);

      loop.quit(0);
      });

  // blocks here
  loop.run();
}
#endif

namespace
{
  static bool helpCommand(std::string const &command, void *data)
  {
    CommandHandler *handler = static_cast<CommandHandler *>(data);
    handler->showCommands();
    return true;
  }

  static bool quitCommand(std::string const &command, void *data)
  {
    bool *running = static_cast<bool *>(data);
    *running      = false;
    std::cout << "quit command : " << command << std::endl;
    return true;
  }

  void counter1()
  {
    static int count{};
    count++;
    std::cout << "count: " << count << std::endl;
  }

  static bool work1(std::string const &command, void *data)
  {
    EventLoop *loop = static_cast<EventLoop *>(data);

    for (int i = 0; i < 10; ++i)
      loop->invokeMethod(counter1);

    EXPECT_THAT(loop->size(), 10);
    return true;
  }
} // namespace

TEST(EventLoop, CliWorkDone)
{
  CommandHandler handler;
  bool running{true};
  int value{};

  EventLoop loop;

  handler.addCommand("q", "quit", quitCommand, &running);
  handler.addCommand("h", "help", helpCommand, &handler);
  handler.addCommand("w1", "work1", work1, &loop);

  auto f1 = std::async(std::launch::async, [&]() {
    // blocks here
    loop.run();
  });

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

  // exit event loop
  loop.quit(0);

  // wait for async thread to finish
  f1.get();
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
