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

namespace
{
  void w1(int &v)
  {
    v += 1;
    std::cout << "w1: " << v << std::endl;
  }

  void cause_dangling_reference(EventLoop &loop)
  {
    int value{0};
    loop.invokeMethod(std::bind(w1, std::ref(value)));
  }

  void f2(std::string &message) { message += ":called"; }

  // so pass a reference to local which will be invalid when f gets called.
  void cause_dangling_reference_2(EventLoop &loop)
  {
    std::string message("dangling");
    loop.invokeMethod(std::bind(f2, std::ref(message)));
  }
} // namespace

// posts many work but nothing is done since do not run event loop and process
// nothing.
//
// size is 10 but value is still 0.

TEST(EventLoop, check_no_work)
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

TEST(EventLoop, DISABLED_check_flush_and_no_work)
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
// it surfaces deadlock issue and works okay when have
// #define DEADLOCK_FIX

TEST(EventLoop, check_deadlock)
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

TEST(EventLoop, check_flush_with_flush)
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

TEST(EventLoop, check_flush_with_flush_and_copy)
{
  {
    int value{};

    EventLoop loop;

    auto f1 = std::async(std::launch::async, [&]() {
      // std::this_thread::sleep_for(chrono::milliseconds(300));

      // shall use std::ref
      for (int i = 0; i < 10; ++i)
        loop.invokeMethod(std::bind(w1, value));

      loop.flush();
      EXPECT_THAT(loop.size(), 0);

      loop.quit(0);
    });

    // blocks here
    loop.run();

    // since do not use std::ref and w1 changes v which is local to
    // clousure(callable) so no effect on real value
    EXPECT_THAT(value, 0);
  }

  // same as above but use the convenient form of invokeMethod()
  {
    int value{};

    EventLoop loop;

    auto f1 = std::async(std::launch::async, [&]() {
      // std::this_thread::sleep_for(chrono::milliseconds(300));

      // shall use std::ref
      for (int i = 0; i < 10; ++i)
        loop.invokeMethod(w1, value);

      loop.flush();
      EXPECT_THAT(loop.size(), 0);

      loop.quit(0);
    });

    // blocks here
    loop.run();

    // since do not use std::ref and w1 changes v which is local to
    // clousure(callable) so no effect on real value
    EXPECT_THAT(value, 0);
  }
}

// What's going to happen when f() uses reference to local.
//
// as you can see, for dangling case, it's changes randomly
//
// [ RUN      ] EventLoop.check_flush_and_dangling
// w1: 1
// w1: 2
// w1: 3
// w1: 4
// w1: 5
// w1: 6
// w1: 7
// w1: 8
// w1: 9
// w1: 10
// w1: 1
// w1: 2
// w1: 3
// w1: 4
// w1: 5
// w1: 6
// w1: 7
// w1: 8
// w1: 32559
// w1: 32560
// [       OK ] EventLoop.check_flush_and_dangling (1 ms)
// [ RUN      ] EventLoop.check_flush_and_dangling
// w1: 1
// w1: 2
// w1: 3
// w1: 4
// w1: 5
// w1: 6
// w1: 7
// w1: 8
// w1: 9
// w1: 10
// w1: 32596
// w1: 32597
// w1: 32598
// w1: 32599
// w1: 32600
// w1: 32601
// w1: 32602
// w1: 32603
// w1: 32604
// w1: 32605
// [       OK ] EventLoop.check_flush_and_dangling (1 ms)
//
// so how about using structure than int?  so see when use
// void cause_dangling_reference_2(EventLoop &loop);

TEST(EventLoop, check_flush_and_dangling_1)
{
  // no dangling case
  {
    int value{};

    EventLoop loop;

    auto f1 = std::async(std::launch::async, [&]() {
      // std::this_thread::sleep_for(chrono::milliseconds(300));

      // shall use std::ref
      for (int i = 0; i < 10; ++i)
        loop.invokeMethod(std::bind(w1, std::ref(value)));

      loop.quit(0);
    });

    // blocks here
    loop.run();

    EXPECT_THAT(value, 10);
  }

  {
    int value{};

    EventLoop loop;

    auto f1 = std::async(std::launch::async, [&]() {
      // std::this_thread::sleep_for(chrono::milliseconds(300));

      // shall use std::ref
      for (int i = 0; i < 10; ++i)
        cause_dangling_reference(loop);

      loop.quit(0);
    });

    // blocks here
    loop.run();

    EXPECT_THAT(value, 0);
  }

  // sometimes raise exception but mostly makes code dump:
  //
  // #0  __memmove_avx_unaligned_erms () at ../sysdeps/x86_64/multiarch/memmove-vec-unaligned-erms.S:308
  // #1  0x000055d9e07e2142 in std::char_traits<char>::copy (__s1=0xd5663e8d6c30 <error: Cannot access memory at address 0xd5663e8d6c30>, __s2=0x55d9e084fb12 ":called", __n=7) at /usr/include/c++/7/bits/char_traits.h:350
  // #2  0x000055d9e07eae9a in std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_S_copy (__d=0xd5663e8d6c30 <error: Cannot access memory at address 0xd5663e8d6c30>, __s=0x55d9e084fb12 ":called", __n=7)
  //     at /usr/include/c++/7/bits/basic_string.h:340
  // #3  0x000055d9e07e852f in std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_append (this=0x7f8c5c873800, __s=0x55d9e084fb12 ":called", __n=7) at /usr/include/c++/7/bits/basic_string.tcc:367
  // #4  0x000055d9e07e4de1 in std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::append (this=0x7f8c5c873800, __s=0x55d9e084fb12 ":called") at /usr/include/c++/7/bits/basic_string.h:1258
  // #5  0x000055d9e07e50f1 in std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::operator+= (this=0x7f8c5c873800, __s=0x55d9e084fb12 ":called") at /usr/include/c++/7/bits/basic_string.h:1168
  // #6  0x000055d9e07b0cd7 in (anonymous namespace)::f2 (message="") at /home/keitee/git/kb/code-cxx/os-dbus/sdbus-case/test_sdbus_case.cpp:37
  // #7  0x000055d9e07f059a in std::__invoke_impl<void, void (*&)(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >&), std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >&> (
  //     __f=@0x7f8c58000c60: 0x55d9e07b0cb8 <(anonymous namespace)::f2(std::__cxx11::string&)>) at /usr/include/c++/7/bits/invoke.h:60
  // #8  0x000055d9e07efb1a in std::__invoke<void (*&)(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >&), std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >&> (
  //     __fn=@0x7f8c58000c60: 0x55d9e07b0cb8 <(anonymous namespace)::f2(std::__cxx11::string&)>) at /usr/include/c++/7/bits/invoke.h:95
  // #9  0x000055d9e07eeeb1 in std::_Bind<void (*(std::reference_wrapper<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >))(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >&)>::__call<void, , 0ul>(std::tuple<>&&, std::_Index_tuple<0ul>) (this=0x7f8c58000c60, __args=...) at /usr/include/c++/7/functional:467
  // #10 0x000055d9e07ed86d in std::_Bind<void (*(std::reference_wrapper<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >))(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >&)>::operator()<, void>() (this=0x7f8c58000c60) at /usr/include/c++/7/functional:551
  // #11 0x000055d9e07eb51f in std::_Function_handler<void (), std::_Bind<void (*(std::reference_wrapper<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >))(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >&)> >::_M_invoke(std::_Any_data const&) (__functor=...) at /usr/include/c++/7/bits/std_function.h:316
  // #12 0x000055d9e07f4fac in std::function<void ()>::operator()() const (this=0x7ffc10cd0630) at /usr/include/c++/7/bits/std_function.h:706
  // #13 0x000055d9e07f3bb6 in EventLoopPrivate::event_handler_ (s=0x55d9e2064740, fd=6, revents=1, userdata=0x55d9e2061f40) at /home/keitee/git/kb/code-cxx/os-dbus/sdbus-case/eventloop2/eventloop.cpp:128
  // #14 0x00007f8c5e577e90 in ?? () from /lib/x86_64-linux-gnu/libsystemd.so.0
  // #15 0x00007f8c5e57924a in sd_event_dispatch () from /lib/x86_64-linux-gnu/libsystemd.so.0
  // #16 0x00007f8c5e5793d9 in sd_event_run () from /lib/x86_64-linux-gnu/libsystemd.so.0
  // #17 0x00007f8c5e57960b in sd_event_loop () from /lib/x86_64-linux-gnu/libsystemd.so.0
  // #18 0x000055d9e07f3d3a in EventLoopPrivate::run (this=0x55d9e2061f40) at /home/keitee/git/kb/code-cxx/os-dbus/sdbus-case/eventloop2/eventloop.cpp:183
  // #19 0x000055d9e07f4312 in EventLoop::run (this=0x7ffc10cd07e0) at /home/keitee/git/kb/code-cxx/os-dbus/sdbus-case/eventloop2/eventloop.cpp:386
  // #20 0x000055d9e07b2642 in EventLoop_check_flush_and_dangling_1_Test::TestBody (this=0x55d9e20634a0) at /home/keitee/git/kb/code-cxx/os-dbus/sdbus-case/test_sdbus_case.cpp:371
  // #21 0x000055d9e083e7c0 in void testing::internal::HandleSehExceptionsInMethodIfSupported<testing::Test, void>(testing::Test*, void (testing::Test::*)(), char const*) ()
  // #22 0x000055d9e0837e71 in void testing::internal::HandleExceptionsInMethodIfSupported<testing::Test, void>(testing::Test*, void (testing::Test::*)(), char const*) ()
  // #23 0x000055d9e0813dba in testing::Test::Run() ()

  {
    int value{};

    EventLoop loop;

    auto f1 = std::async(std::launch::async, [&]() {
      // std::this_thread::sleep_for(chrono::milliseconds(300));

      // shall use std::ref
      for (int i = 0; i < 10; ++i)
        cause_dangling_reference_2(loop);

      loop.quit(0);
    });

    // blocks here
    loop.run();

    EXPECT_THAT(value, 0);
  }
}

// see that it works without flush() since flush() is only necessary when
// forcefully shutdown all.

TEST(EventLoop, check_flush_without_flush)
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
// see TEST(DBusMessage, message_call)
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

// see quit() first and flush() later

TEST(EventLoop, check_flush_with_wrong_order)
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

  // from ASRequestPrivate::marshallAndSendReply
  static void marshallAndSendReply(const std::vector<std::string> &headers,
                                   const std::string &body)
  {
    std::this_thread::sleep_for(chrono::milliseconds(100));

    std::cout << "================" << std::endl;
    std::cout << "headers: size : " << headers.size() << ": ";
    for (const auto &h : headers)
    {
      std::cout << h << ", ";
    }
    std::cout << std::endl;

    std::cout << "body: " << body << std::endl;
  }
} // namespace

// invokeMethod() use copy? okay use on temporary? yes since cxx-bind use copy
// context which is usded in invokeMethod()
//
// as can see, while vector header is increasing and `reset` happens at
// different points in these runs, functor f still prints vector header
// properly. since callable from std::bind() is self-contained.

// [ RUN      ] EventLoop.event_invoke_do_copy
// reset them and quit  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// ================
// headers: size : 1: header1,
// body: body1
// ================
// headers: size : 2: header1, header2,
// body: body2
// ================
// headers: size : 3: header1, header2, header3,
// body: body3
// ================
// headers: size : 4: header1, header2, header3, header4,
// body: body4
// ================
// headers: size : 5: header1, header2, header3, header4, header5,
// body: body5
// [       OK ] EventLoop.event_invoke_do_copy (501 ms)
//
// [ RUN      ] EventLoop.event_invoke_do_copy
// ================
// headers: size : 1: header1,
// body: body1
// ================
// headers: size : 2: header1, header2,
// body: body2
// ================
// headers: size : 3: header1, header2, header3,
// body: body3
// ================
// headers: size : 4: header1, header2, header3, header4,
// body: body4
// reset them and quit  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// ================
// headers: size : 5: header1, header2, header3, header4, header5,
// body: body5
// [       OK ] EventLoop.event_invoke_do_copy (501 ms)
//
// [ RUN      ] EventLoop.event_invoke_do_copy
// ================
// headers: size : 1: header1,
// body: body1
// ================
// headers: size : 2: header1, header2,
// body: body2
// ================
// headers: size : 3: header1, header2, header3,
// body: body3
// reset them and quit  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// ================
// headers: size : 4: header1, header2, header3, header4,
// body: body4
// ================
// headers: size : 5: header1, header2, header3, header4, header5,
// body: body5
// [       OK ] EventLoop.event_invoke_do_copy (501 ms)

TEST(EventLoop, check_invoke_do_copy)
{
  {
    int value{};

    EventLoop loop;

    auto f1 = std::async(std::launch::async, [&]() {
      std::vector<std::string> header;
      std::string body;

      // header.push_back(std::string("header1"));
      header.push_back("header1");
      body = "body1";

      // Unlike void cause_dangling_reference_2(EventLoop &loop); case where
      // uses reference to local, bind() uses copies so not a problem here.

      loop.invokeMethod(marshallAndSendReply, header, body);

      // now changes header and body. post it
      header.push_back("header2");
      body = "body2";
      loop.invokeMethod(marshallAndSendReply, header, body);

      // now changes header and body. post it
      header.push_back("header3");
      body = "body3";
      loop.invokeMethod(marshallAndSendReply, header, body);

      // now changes header and body. post it
      header.push_back("header4");
      body = "body4";
      loop.invokeMethod(marshallAndSendReply, header, body);

      // now changes header and body. post it
      header.push_back("header5");
      body = "body5";
      loop.invokeMethod(marshallAndSendReply, header, body);

      // okay, reset them
      header.clear();
      body.clear();

      std::cout << "reset them and quit" << std::endl;

      EXPECT_THAT(header.size(), 0);
      EXPECT_THAT(body.size(), 0);

      loop.quit(0);
    });

    // blocks here
    loop.run();
  }
}

// use memeber function

TEST(EventLoop, check_invoke_member)
{
  {
    int value{};

    EventLoop loop;
    Work1 w;

    auto f1 = std::async(std::launch::async, [&]() {
      for (int i = 0; i < 10; ++i)
        loop.invokeMethod(&Work1::printArgs, &w, 10, 20, 30);

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

  // without `target` to call member function since it's static and it is to
  // call on different thread
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
