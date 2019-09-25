#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <queue>
#include <future>

#include "gmock/gmock.h"

#include "timer_queue.h"

using namespace testing;

// void StateMachineTest_handleSuperStateNotSameParent_Test::TestBody();
// (gdb) b StateMachineTest_handleSuperStateNotSameParent_Test::TestBody()
// void StateMachineTest_handleSuperStateSameParent_Test::TestBody();

// ={=========================================================================

TEST(TimerQueue, Create)
{
  TimerQueue tq;
}

namespace
{
  bool callback_single(std::mutex &m, std::condition_variable &cv)
  {
    std::lock_guard<std::mutex> lock(m);
    cv.notify_one();

    return true;
  }

  static int counter{};

  bool callback_repeat(std::mutex &m, std::condition_variable &cv, int repeat)
  {
    std::lock_guard<std::mutex> lock(m);

    counter++;

    // debug
    // std::cout << "counter : " << counter << std::endl;

    if (counter >= repeat)
    {
      cv.notify_one();
      // to cancel timer
      return false;
    }

    return true;
  }
}

TEST(TimerQueue, runSingleTimer)
{
  TimerQueue tq;
  std::mutex m;
  std::condition_variable cv;
  std::unique_lock<std::mutex> lock(m);

  // have to use std::chrono and otherwise error.
  tq.add(std::chrono::milliseconds(2000), true,
      std::bind(callback_single, std::ref(m), std::ref(cv)));

  cv.wait(lock);
}

TEST(TimerQueue, runRepeatTimer)
{
  TimerQueue tq;
  std::mutex m;
  std::condition_variable cv;
  std::unique_lock<std::mutex> lock(m);

  counter = 0;

  // have to use std::chrono and otherwise error.
  tq.add(std::chrono::milliseconds(2000), false,
      std::bind(callback_repeat, std::ref(m), std::ref(cv), 10));

  cv.wait(lock);

  // to make sure there's no more tiemrs fired
  std::this_thread::sleep_for(std::chrono::seconds(4));

  EXPECT_THAT(counter, 10);
}


int main(int argc, char **argv)
{
  // Since Google Mock depends on Google Test, InitGoogleMock() is
  // also responsible for initializing Google Test.  Therefore there's
  // no need for calling testing::InitGoogleTest() separately.
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
