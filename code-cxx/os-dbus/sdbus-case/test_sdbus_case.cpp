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

// one thread which runs event loop and the other pushes task and use flush()
//
// it surfaces deadlock issue and works oken when have
// #define DEADLOCK_FIX

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

// flush() is not called in dtor. do we ever need to call flush()? As
// event_flush2 shows, there is no real need to do since quit() pushes exist
// task which will be in the last of the queue. So until that gets processed,
// event loop runs and no exit.

// [ RUN      ] EventLoop.event_flush1
// M:eventloop.cpp F:EventLoopPrivate L:60 > EventLoopPrivate::EventLoopPrivate() thread(139919476278208)
// M:eventloop.cpp F:run L:173 > eventloop runs and set the thread local to this. thread id(139919476278208), m_loopRunning(0x0x557fd4f13620)
// M:eventloop.cpp F:flush L:322 > flush called from different thread(139919440869120), m_loopRunning(0x(nil))
// M:eventloop.cpp F:operator() L:329 > sem_post
// M:eventloop.cpp F:run L:185 > eventloop stops and set the thread local to null
// M:eventloop.cpp F:~EventLoopPrivate L:80 > EventLoopPrivate::~EventLoopPrivate()
//
// M:eventloop.cpp F:EventLoopPrivate L:60 > EventLoopPrivate::EventLoopPrivate() thread(139919476278208)
// M:eventloop.cpp F:run L:173 > eventloop runs and set the thread local to this. thread id(139919476278208), m_loopRunning(0x0x557fd4f13620)
// M:eventloop.cpp F:flush L:322 > flush called from different thread(139919440869120), m_loopRunning(0x(nil))
// M:eventloop.cpp F:operator() L:329 > sem_post
// M:eventloop.cpp F:run L:185 > eventloop stops and set the thread local to null
// M:eventloop.cpp F:~EventLoopPrivate L:80 > EventLoopPrivate::~EventLoopPrivate()
// [       OK ] EventLoop.event_flush1 (1 ms)

TEST(EventLoop, event_flush1)
{
  {
    int value{};

    EventLoop loop;

    auto f1 = std::async(std::launch::async, [&]() {
      // std::this_thread::sleep_for(chrono::milliseconds(300));

      // shall use std::ref
      for (int i = 0; i < 10; ++i)
        loop.invokeMethod(std::bind(w1, std::ref(value)));

      // here, there is no guarantee that all posted work are in the queue so
      // sometimes okay or sometimes fail.
      // EXPECT_THAT(loop.size(), 10);

      loop.flush();
      EXPECT_THAT(loop.size(), 0);

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

      // here, there is no guarantee that all posted work are in the queue so
      // sometimes okay or sometimes fail.
      // EXPECT_THAT(loop.size(), 10);

      loop.flush();
      EXPECT_THAT(loop.size(), 0);

      loop.quit(0);
    });

    // blocks here
    loop.run();

    EXPECT_THAT(value, 10);
  }
}

TEST(EventLoop, event_flush2)
{
  const int count{10000000};

  {
    int value{};

    EventLoop loop;

    auto f1 = std::async(std::launch::async, [&]() {
      // std::this_thread::sleep_for(chrono::milliseconds(300));

      // shall use std::ref
      for (int i = 0; i < count; ++i)
        loop.invokeMethod(std::bind(w1, std::ref(value)));

      // here, there is no guarantee that all posted work are in the queue so
      // sometimes okay or sometimes fail.
      // EXPECT_THAT(loop.size(), 10);

      // loop.flush();
      // EXPECT_THAT(loop.size(), 0);

      loop.quit(0);
    });

    // blocks here
    loop.run();

    EXPECT_THAT(value, count);
  }

  // same as above but use the convenient form of invokeMethod()
  {
    int value{};

    EventLoop loop;

    auto f1 = std::async(std::launch::async, [&]() {
      // std::this_thread::sleep_for(chrono::milliseconds(300));

      // shall use std::ref
      for (int i = 0; i < count; ++i)
        loop.invokeMethod(w1, std::ref(value));

      // here, there is no guarantee that all posted work are in the queue so
      // sometimes okay or sometimes fail.
      // EXPECT_THAT(loop.size(), 10);

      // loop.flush();
      // EXPECT_THAT(loop.size(), 0);

      loop.quit(0);
    });

    // blocks here
    loop.run();

    EXPECT_THAT(value, count);
  }
}

// NOTE: sometimes works and sometimes not. race condition.
//
// [ RUN      ] EventLoop.event_flush3
// 0000017889.273986 WRN: < M:eventloop.cpp F:EventLoopPrivate L:60 > EventLoopPrivate::EventLoopPrivate() thread(140032610059200)
// 0000017889.274065 WRN: < M:eventloop.cpp F:run L:173 > eventloop runs and set the thread local to this. thread id(140032610059200), m_loopRunning(0x0x55dccb25c7a0)
// 0000017889.274160 WRN: < M:eventloop.cpp F:flush L:322 > flush called from different thread(140032574650112), m_loopRunning(0x(nil))
// 0000017889.274220 WRN: < M:eventloop.cpp F:operator() L:329 > sem_post
// 0000017889.274251 WRN: < M:eventloop.cpp F:run L:185 > eventloop stops and set the thread local to null
// 0000017889.274302 WRN: < M:eventloop.cpp F:~EventLoopPrivate L:80 > EventLoopPrivate::~EventLoopPrivate()
// [       OK ] EventLoop.event_flush3 (0 ms)
// [----------] 1 test from EventLoop (0 ms total)
//
// [----------] Global test environment tear-down
// [==========] 1 test from 1 test case ran. (0 ms total)
// [  PASSED  ] 1 test.
// keitee@kit-hdebi:~/git/kb/code-cxx/os-dbus/sdbus-case/build$ ./test_sdbus_case --gtest_filter=*event_flush3*
// Note: Google Test filter = *event_flush3*
// [==========] Running 1 test from 1 test case.
// [----------] Global test environment set-up.
// [----------] 1 test from EventLoop
// [ RUN      ] EventLoop.event_flush3
// 0000017889.913828 WRN: < M:eventloop.cpp F:EventLoopPrivate L:60 > EventLoopPrivate::EventLoopPrivate() thread(140079097209792)
// 0000017889.913904 WRN: < M:eventloop.cpp F:run L:173 > eventloop runs and set the thread local to this. thread id(140079097209792), m_loopRunning(0x0x559c049627a0)
// 0000017889.914018 WRN: < M:eventloop.cpp F:flush L:322 > flush called from different thread(140079061800704), m_loopRunning(0x(nil))
// 0000017889.914052 WRN: < M:eventloop.cpp F:run L:185 > eventloop stops and set the thread local to null

TEST(EventLoop, event_flush3)
{
  int value{};

  EventLoop loop;

  auto f1 = std::async(std::launch::async, [&]() {
    // shall use std::ref
    for (int i = 0; i < 10; ++i)
      loop.invokeMethod(std::bind(w1, std::ref(value)));

    loop.quit(0);
    loop.flush();
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

// M:eventloop.cpp F:EventLoopPrivate L:60 > EventLoopPrivate::EventLoopPrivate() thread(140408928236480)
// event loops blocks on here
// M:eventloop.cpp F:run L:173 > eventloop runs and set the thread local to this. thread id(140408928236480), m_loopRunning(0x0x5651c1ad9620)
// M:dbusconnection.cpp F:call L:481 > DBusConnection::call() is called on the other thread
// M:dbusconnection.cpp F:callWithCallback L:249 > callWithCallback::call() place a call on the event loop thread
// M:dbusconnection.cpp F:operator() L:188 > callWithCallback::call() called on the event loop thread
// M:dbusconnection.cpp F:methodCallCallback_ L:143 > methodCallCallback_:: calls f
// M:dbusconnection.cpp F:call L:513 > call:: return replyMessage
// name: org.freedesktop.DBus
// name: org.freedesktop.Notifications
// name: :1.7
// name: org.freedesktop.network-manager-applet
// name: :1.8
//
// NOTE: it causes `deadlock` and m_loopRunning is null when called from the
// same thread?
//
// since loop.quit() set it to null and ends event loop. Then DBusConnection()
// dtor calls loop.flush() but event loop is already gone. so blocking forever.
// so same as callig, like `event_flush3` case
//
// loop.quit();
// loop.flush();
//
// M:eventloop.cpp F:run L:185 > eventloop stops and set the thread local to null
// M:eventloop.cpp F:flush L:322 > flush called from different thread(140408928236480), m_loopRunning(0x(nil))
//
// So, to fix?
// 1. works when remove flush() from DBusConnection
// 2. case examples do not use quit()
//
// NOTE:
// 1. it is less likely to use flush() from event loop thread and if do, have to
// post a task which calls flush().
//
// 2. should consider to use quit() in event loop dtor and to have check in
// flush() to see whether event loop is still running or not.
//
// 3. case examples seems not to handle "dtor/exit" path of event loop.

TEST(DBusMessage, message_call)
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
