#include <chrono>
// #include <condition_variable>
// #include <forward_list>
// #include <iostream>
// #include <limits>
// #include <list>
// #include <memory>
// #include <mutex>
// #include <regex>
// #include <set>
#include <thread>
// #include <vector>

#include "timer_simple.h"

#include "gmock/gmock.h"

// use lpi log
#include "slog.h"

using namespace std;
using namespace std::placeholders;
using namespace testing;

// ={=========================================================================
// exact one time and time difference?
TEST(SimpleTimerTest, set_timer_and_wait_for_action)
{
  bool complete_{false};

  // Timer t(std::chrono::milliseconds(100), [&]() { complete_ = true; });

  Timer t(std::chrono::milliseconds(100),
          TimerType::SingleShot,
          TimerThreadPriority::Default,
          [&]() 
          { 
          LOG_MSG("functor: called and set true");
          complete_ = true; 
          });

  std::this_thread::sleep_for(std::chrono::milliseconds(150));

  EXPECT_THAT(complete_, true);
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
