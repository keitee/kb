#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <queue>
#include <future>

#include "gmock/gmock.h"

#include "timer_queue.h"
#include "slog.h"

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
  static int counter{};

  bool callback_return(bool value)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    counter++;
    LOG_MSG("counter value is %d", counter);
    return value;
  }

  bool callback_simple(std::chrono::milliseconds &delay)
  {
    std::this_thread::sleep_for(delay);
    counter++;
    LOG_MSG("counter value is %d", counter);
    return true;
  }

  bool callback_single(std::mutex &m, std::condition_variable &cv)
  {
    std::lock_guard<std::mutex> lock(m);
    cv.notify_one();

    return true;
  }

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

  class Callback
  {
    public:
      Callback(std::condition_variable &cv)
        : cv_(cv)
      {}

      bool CallbackSingle()
      {
        std::lock_guard<std::mutex> lock(m_);
        cv_.notify_one();

        return true;
      }

    private:
      std::mutex m_;
      std::condition_variable &cv_;
  };
}

// regardless of return value of handler, oneshot runs only one time.

TEST(TimerQueue, runOneshotTimerWithTrue)
{
  TimerQueue tq;

  // have to use std::chrono and otherwise error.
  tq.add(std::chrono::milliseconds(1000), true,
      std::bind(callback_return, true));

  std::this_thread::sleep_for(std::chrono::milliseconds(4000));
}

TEST(TimerQueue, runOneshotTimerWithFalse)
{
  TimerQueue tq;

  // have to use std::chrono and otherwise error.
  tq.add(std::chrono::milliseconds(1000), true,
      std::bind(callback_return, false));

  std::this_thread::sleep_for(std::chrono::milliseconds(4000));
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

TEST(TimerQueue, runSingleTimerOnMemberFunction)
{
  TimerQueue tq;
  std::mutex m;
  std::condition_variable cv;
  std::unique_lock<std::mutex> lock(m);

  Callback co(cv);

  // have to use std::chrono and otherwise error.
  tq.add(std::chrono::milliseconds(2000), true,
      std::bind(&Callback::CallbackSingle, &co));

  cv.wait(lock);
}

TEST(TimerQueue, runSingleTimerButRemoveBeforeRunning)
{
  TimerQueue tq;

  std::chrono::milliseconds delay(200);

  counter = 0;

  // have to use std::chrono and otherwise error.
  auto tag = tq.add(std::chrono::milliseconds(20000), true,
      std::bind(callback_simple, std::ref(delay)));

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  EXPECT_THAT(tq.remove(tag), true);

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  EXPECT_THAT(counter, 0);
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

// creates many but ends without running them.
TEST(TimerQueue, makesManyTimersAndExit)
{
  TimerQueue tq;

  std::chrono::milliseconds delay(200);

  for (int i = 0; i < 20; ++i)
  {
    // have to use std::chrono and otherwise error. oneshot
    tq.add(std::chrono::milliseconds(20000), true,
        std::bind(callback_simple, std::ref(delay)));
  }

  // do not wait for them to end
}

int main(int argc, char **argv)
{
  // Since Google Mock depends on Google Test, InitGoogleMock() is
  // also responsible for initializing Google Test.  Therefore there's
  // no need for calling testing::InitGoogleTest() separately.
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
