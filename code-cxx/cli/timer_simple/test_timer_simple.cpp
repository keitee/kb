#include <chrono>
#include <thread>

#include "timer_simple.h"

#include "gmock/gmock.h"

// use lpi log
#include "slog.h"

using namespace std;
using namespace std::placeholders;
using namespace testing;

// ={=========================================================================
// singleshot
TEST(SimpleTimerTest, single_timer_and_wait_for_action)
{
  bool complete_{false};

  Timer t(std::chrono::milliseconds(100),
          TimerType::SingleShot,
          TimerThreadPriority::Default,
          [&]() {
            LOG_MSG("single: called and set true");
            complete_ = true;
          });

  std::this_thread::sleep_for(std::chrono::milliseconds(150));

  EXPECT_THAT(complete_, true);
}

// ={=========================================================================
// periodic
TEST(SimpleTimerTest, periodic_timer_and_wait_for_action)
{
  int counter_{};

  Timer t(std::chrono::milliseconds(100),
          TimerType::Periodic,
          TimerThreadPriority::Default,
          [&]() {
            LOG_MSG("periodic: called and set true");
            counter_++;
          });

  std::this_thread::sleep_for(std::chrono::milliseconds(250));

  t.cancel();

  EXPECT_THAT(counter_, 2);
}

// ={=========================================================================
// single and cancel it before it expires
TEST(SimpleTimerTest, single_timer_and_cancel)
{
  bool complete_{false};

  Timer t(std::chrono::milliseconds(100), [&]() {
    LOG_MSG("single: called and set true");
    complete_ = true;
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  t.cancel();

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  EXPECT_THAT(complete_, false);
}

// ={=========================================================================
// periodic and cancel it before it expires
TEST(SimpleTimerTest, periodic_timer_and_cancel)
{
  int counter_{};

  Timer t(std::chrono::milliseconds(100),
          TimerType::Periodic,
          TimerThreadPriority::Default,
          [&]() {
            LOG_MSG("periodic: called and set true");
            counter_++;
          });

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  t.cancel();

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  EXPECT_THAT(counter_, 0);
}

// ={=========================================================================
// shows how to use std::bind() to have functors with args
TEST(SimpleTimerTest, ctors_and_binds)
{
  bool complete_{false};

  auto func = [&complete_](int magic) {
    if (magic == 42)
    {
      LOG_MSG("magic is true");
      complete_ = true;
    }
  };

  // t1 is single and no need to call cancel()
  Timer t1(std::chrono::milliseconds(100), std::bind(func, 5));

  std::this_thread::sleep_for(std::chrono::milliseconds(150));

  EXPECT_THAT(complete_, false);

  // t2 is single and no need to call cancel()
  Timer t2(std::chrono::milliseconds(100), std::bind(func, 42));

  std::this_thread::sleep_for(std::chrono::milliseconds(150));

  EXPECT_THAT(complete_, true);
}

/*
// ={=========================================================================
shows how to use template ctors to have functors with args. so the user don't
have to use std::bind for themself.

NOTE:
To use this:

o have to have both templated ctors. otherwise, compile errors. not sure why
this happens since there is normal ctors which seems to be a match. normal
ctors should have a priority in function matching?

o when removing normal ctors, it also compiles and works.

*/

TEST(SimpleTimerTest, ctors_and_templated)
{
  bool complete_{false};

  auto func = [&complete_](int magic) {
    if (magic == 42)
    {
      LOG_MSG("magic is true");
      complete_ = true;
    }
  };

  // t1 is single and no need to call cancel()
  Timer t1(std::chrono::milliseconds(100), func, 5);

  std::this_thread::sleep_for(std::chrono::milliseconds(150));

  EXPECT_THAT(complete_, false);

  // t2 is single and no need to call cancel()
  Timer t2(std::chrono::milliseconds(100), func, 42);

  std::this_thread::sleep_for(std::chrono::milliseconds(150));

  EXPECT_THAT(complete_, true);
}

// ={=========================================================================
// cancel multiple times
TEST(SimpleTimerTest, single_timer_and_double_cancel)
{
  bool complete_{false};

  Timer t(std::chrono::milliseconds(5000), [&]() {
    LOG_MSG("single: called and set true");
    complete_ = true;
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  t.cancel();
  t.cancel();

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  EXPECT_THAT(complete_, false);
}

// ={=========================================================================
// use member function

class Foo
{
  public:
    void square(int x)
    {
      std::cout << "square : " << x*x << std::endl;
    }
};

TEST(SimpleTimerTest, single_timer_and_member_function)
{
  Foo f;

  Timer t(std::chrono::milliseconds(200), &Foo::square, &f, 3);

  std::this_thread::sleep_for(std::chrono::milliseconds(400));
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
