#include <algorithm>
#include <deque>
#include <future>
#include <iostream>
#include <memory>
#include <vector>

#include "gmock/gmock.h"

#include "dbusconnection.h"
#include "dbusmessage.h"
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

TEST(EventLoop, event_no_work)
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

// will flush() make difference? blocked since didn't call run() so
// EventLoopPrivate::m_loopRuunig is null and runs path when "this !=
// m_loopRunning". that use sem and wait for that and blocked there.

TEST(DISABLED_EventLoop, event_flush_no_work)
{
  int value{};

  EventLoop loop;

  for (int i = 0; i < 10; ++i)
    loop.invokeMethod(std::bind(w1, value));

  EXPECT_THAT(loop.size(), 10);

  loop.flush();

  EXPECT_THAT(value, 0);
}

// running on a different thread should make difference?
// two threas and one event loop.

TEST(EventLoop, event_deadlock)
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

// two threas and one event loop. so do not use flush().
// NOTE: can share event loop between threads

TEST(EventLoop, event_share)
{
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
}

namespace
{
  class Work1
  {
  public:
    void printArgs(int v1, int v2, int v3)
    {
      std::cout << "v1: " << v1 << ", v2: " << v2 << ", v3: " << v3
                << std::endl;
    }
  };

  // TODO: HOW possible without using target?? the trick is that that's staic
  // function.
  //
  // bool ASRequestPrivate::doSendReply(unsigned int code)
  // {
  //
  //     // otherwise running on a different thread so need to invoke from the
  //     // event loop thread
  //     return
  //     m_eventLoop.invokeMethod(&ASRequestPrivate::marshallAndSendReply,
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

    static void printArgs(int v1, int v2, int v3)
    {
      std::cout << "v1: " << v1 << ", v2: " << v2 << ", v3: " << v3
                << std::endl;
    }
  };

  void Work2::pushWorks()
  {
    for (int i = 0; i < 10; ++i)
      m_eventloop.invokeMethod(&Work2::printArgs, 10, 20, 30);
  }
} // namespace

// may memeber function

TEST(EventLoop, event_invoke_member)
{
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

  // without `target` to call member function since it's static
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
}

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

TEST(EventLoop, event_cli)
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

/*
{
    Json::Value root;

    uint32_t returnCode = 500;
    std::string ipAddress;
    DBusMessage message = DBusMessage::createMethodCall("com.sky.as.proxy",
                                                        "/com/sky/as/service",
                                                        "com.sky.as.Service1",
                                                        "DBusProxyMethodCall"
                                                        );

    message << getIpMethodArgs;

    DBusMessage reply = GetDbusConnection().call( std::move(message) );
    if ( reply.isError() )
    {
        AS_LOG_ERROR("Get IP failed: %s, %s", reply.errorName().c_str(), reply.errorMessage().c_str() );

        returnCode = 500;
        root["errorCode"] = 103;
        root["userMessage"] = "Generic failure";
    }
    else
    {
        reply >> returnCode;

        if ( returnCode == 200 )
        {
            reply >> ipAddress;
            AS_LOG_MIL("GetIp succeeded, ipAddress: %s, return code: %u", ipAddress.c_str(), returnCode);

            Json::Value network;
            network["ipaddr"] = ipAddress;

            root["version"] = 1;
            root["network"] = network;
        }
        else
        {
            AS_LOG_ERROR("GetIp failed, return code: %u", returnCode);

            returnCode = 500;
            root["errorCode"] = 103;
            root["userMessage"] = "Generic failure";
        }
    }

    jsonResponse = Json::StyledWriter().write(root);

    return returnCode;
}
*/

TEST(DBusMessage, message_create)
{
  // create event loop
  EventLoop loop;

  // connect to dbus
  DBusConnection conn = DBusConnection::sessionBus(loop);
  EXPECT_THAT(conn.isConnected(), true);

  // without `target` to call member function since it's static
  auto f1 = std::async(std::launch::async, [&]() {

      std::string name;

    // wait for some time
    std::this_thread::sleep_for(std::chrono::seconds(5));

    DBusMessage message =
      DBusMessage::createMethodCall("org.freedesktop.DBus",  // service
                                    "/org/freedesktop/DBus", // path
                                    "org.freedesktop.DBus",  // interface
                                    "ListNames");            // method

    DBusMessage reply = conn.call(std::move(message));
    EXPECT_THAT(reply.isError(), false);

    for (int i = 0; i < 5; i++)
    {
      reply >> name;
      std::cout << "name: " << name << std::endl;
    }

    loop.quit(0);
  });

  // blocks here
  std::cout << "event loops blocks on here" << std::endl;
  loop.run();
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
