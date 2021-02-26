#include <chrono>
#include <condition_variable>
#include <forward_list>
#include <iostream>
#include <limits>
#include <list>
#include <memory>
#include <mutex>
#include <regex>
#include <set>
#include <thread>
#include <vector>

#include "epollloop.h"
#include "gmock/gmock.h"

// use lpi log
#include "slog.h"

using namespace std;
using namespace std::placeholders;
using namespace testing;

class EPollLoopTest : public ::testing::Test
{
public:
  void SetUp() { LOG_MSG("EpollLoopTest: SetUp called..."); }
  void TearDown() { LOG_MSG("EpollLoopTest: TearDown called..."); }

public:
  using TimePoint = std::chrono::steady_clock::time_point;

  std::vector<std::pair<int, TimePoint>> m_timer_events;

  void onTimerCallback(int eventid, int64_t tag)
  {
    // LOG_MSG("have got eventid %d", eventid);

    m_timer_events.emplace_back(eventid, std::chrono::steady_clock::now());
  }

  // diff in ms
  long long getTimeDiff(const TimePoint &from, const TimePoint &to)
  {
    auto diff =
      std::chrono::duration_cast<std::chrono::milliseconds>(from - to);
    return diff.count();
  }
};

// ={=========================================================================
// exact one time and time difference?
TEST_F(EPollLoopTest, singleshot_timer_1)
{
  EPollLoop eloop("test");
  ASSERT_TRUE(eloop.start());

  auto start = std::chrono::steady_clock::now();

  ASSERT_TRUE(
    eloop.startSingleShotTimer(std::chrono::milliseconds(300),
                               std::bind(&EPollLoopTest::onTimerCallback,
                                         this,
                                         100, // eventid
                                         std::placeholders::_1)));

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  // expect one time
  ASSERT_THAT(m_timer_events.size(), 1);

  // time difference
  EXPECT_THAT(m_timer_events[0].first, 100);
  EXPECT_THAT(getTimeDiff(m_timer_events[0].second, start), 300);
}

// ={=========================================================================
TEST_F(EPollLoopTest, singleshot_timer_2)
{
  EPollLoop eloop("test");
  ASSERT_TRUE(eloop.start());

  auto start = std::chrono::steady_clock::now();

  ASSERT_TRUE(
    eloop.startSingleShotTimer(std::chrono::milliseconds(300),
                               std::bind(&EPollLoopTest::onTimerCallback,
                                         this,
                                         300, // eventid
                                         std::placeholders::_1)));

  ASSERT_TRUE(
    eloop.startSingleShotTimer(std::chrono::milliseconds(200),
                               std::bind(&EPollLoopTest::onTimerCallback,
                                         this,
                                         200, // eventid
                                         std::placeholders::_1)));

  ASSERT_TRUE(
    eloop.startSingleShotTimer(std::chrono::milliseconds(100),
                               std::bind(&EPollLoopTest::onTimerCallback,
                                         this,
                                         100, // eventid
                                         std::placeholders::_1)));

  ASSERT_TRUE(
    eloop.startSingleShotTimer(std::chrono::milliseconds(0),
                               std::bind(&EPollLoopTest::onTimerCallback,
                                         this,
                                         0, // eventid
                                         std::placeholders::_1)));

  std::this_thread::sleep_for(std::chrono::milliseconds(1500));

  ASSERT_THAT(m_timer_events.size(), 4);

  // since m_timer_event is vector and is inserted in the order of callback and
  // which is when timer is expired.

  EXPECT_THAT(m_timer_events[0].first, 0);
  EXPECT_THAT(getTimeDiff(m_timer_events[0].second, start), 0);

  // ASSERT_THAT(m_timer_events[0].first, 0);
  // ASSERT_THAT(m_timer_events[1].first, 100);
  // ASSERT_THAT(m_timer_events[2].first, 200);
  // ASSERT_THAT(m_timer_events[3].first, 300);

  EXPECT_THAT(m_timer_events[1].first, 100);
  EXPECT_THAT(getTimeDiff(m_timer_events[1].second, start), 100);

  EXPECT_THAT(m_timer_events[2].first, 200);
  EXPECT_THAT(getTimeDiff(m_timer_events[2].second, start), 200);

  EXPECT_THAT(m_timer_events[3].first, 300);
  EXPECT_THAT(getTimeDiff(m_timer_events[3].second, start), 300);
}

// ={=========================================================================
TEST_F(EPollLoopTest, periodic_timer_run)
{
  EPollLoop eloop("test");
  ASSERT_TRUE(eloop.start());

  const std::chrono::milliseconds period{20};
  const std::chrono::milliseconds testPeriod{300};

  auto id = eloop.addTimer(period,
                           period,
                           std::bind(&EPollLoopTest::onTimerCallback,
                                     this,
                                     3301, // eventid
                                     std::placeholders::_1));

  // id should not be < 0 since reutrn -1 when fails
  ASSERT_GT(id, 0);
  ASSERT_THAT(eloop.startTimer(id), true);

  // wait some time
  std::this_thread::sleep_for(testPeriod);

  // expects 15 times run of timer? sometimes fails so has to use the reference
  // approach
  //
  // ASSERT_THAT(m_timer_events.size(), 15);

  // the reference
  // ASSERT_NEAR(static_cast<long long>(m_timer_events.size()),
  //     (testPeriod.count() / period.count()), 5LL);

  ASSERT_NEAR(static_cast<long long>(m_timer_events.size()),
              (testPeriod.count() / period.count()),
              1LL);

  // start from the second event
  auto event = m_timer_events.cbegin() + 1;

  for (; event != m_timer_events.cend(); ++event)
  {
    // eventid
    ASSERT_THAT(event->first, 3301);

    auto previousTime = (event - 1)->second;
    auto diff         = getTimeDiff(event->second, previousTime);

    // expects that the difference between events is near to the timer interval,
    // 20.
    //
    // the reference
    // ASSERT_NEAR(diff, period.count(), 10);

    ASSERT_NEAR(diff, period.count(), 1);
  }
}

// ={=========================================================================
TEST_F(EPollLoopTest, periodic_timer_cancel)
{
  EPollLoop eloop("test");
  ASSERT_TRUE(eloop.start());

  const std::chrono::milliseconds period{50};

  auto id = eloop.addTimer(period,
                           period,
                           std::bind(&EPollLoopTest::onTimerCallback,
                                     this,
                                     3302, // eventid
                                     std::placeholders::_1));

  // id should not be < 0 since reutrn -1 when fails
  ASSERT_GT(id, 0);

  // wait some time
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  // get start time
  auto start = std::chrono::steady_clock::now();

  // start timer
  ASSERT_THAT(eloop.startTimer(id), true);

  // wait some time
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  // stop timer
  ASSERT_THAT(eloop.stopTimer(id), true);

  // how many times did it run?
  auto nums = m_timer_events.size();

  // fails sometimes so comment out so use GE/LE as the reference
  // EXPECT_THAT(nums, 4);

  ASSERT_GE(nums, 3);
  // val1 <= val2
  ASSERT_LE(nums, 4);

  // check eventid
  ASSERT_THAT(m_timer_events[0].first, 3302);

  // check time diff at 50 space
  EXPECT_THAT(getTimeDiff(m_timer_events[0].second, start), 50);
  EXPECT_THAT(getTimeDiff(m_timer_events[1].second, start), 100);
  EXPECT_THAT(getTimeDiff(m_timer_events[2].second, start), 150);
}

/*
// ={=========================================================================
In above tests, initial time is the same as interval time so effectively don't
have the initial time. Here set and test it.
*/

TEST_F(EPollLoopTest, periodic_timer_with_initial_time)
{
  EPollLoop eloop("test");
  ASSERT_TRUE(eloop.start());

  const std::chrono::milliseconds initial{200};
  const std::chrono::milliseconds period{33};
  const std::chrono::milliseconds testPeriod{300};

  auto id = eloop.addTimer(initial, // initial
                           period,
                           std::bind(&EPollLoopTest::onTimerCallback,
                                     this,
                                     3303, // eventid
                                     std::placeholders::_1));

  // id should not be < 0 since reutrn -1 when fails
  ASSERT_GT(id, 0);

  // start timer
  ASSERT_THAT(eloop.startTimer(id), true);

  // get start time
  auto start = std::chrono::steady_clock::now();

  // wait some time
  std::this_thread::sleep_for(testPeriod);

  // stop the loop which cleans up all timers
  eloop.stop();

  // how many times did it run?
  auto nums = m_timer_events.size();

  EXPECT_THAT(nums, 4);

  // the reference
  // EXPECT_NEAR(static_cast<long long>(nums), 6LL, 2LL);

  // check eventid
  ASSERT_THAT(m_timer_events[0].first, 3303);

  // check time diff at 50 space
  EXPECT_THAT(getTimeDiff(m_timer_events[0].second, start), 200);

  // start from the second event
  auto event = m_timer_events.cbegin() + 1;

  for (; event != m_timer_events.cend(); ++event)
  {
    auto previousTime = (event - 1)->second;
    auto diff         = getTimeDiff(event->second, previousTime);

    // expects that the difference between events is near to the timer interval,
    // NEAR 33.

    EXPECT_NEAR(diff, 33, 5LL);
  }
}

// ={=========================================================================
// run a functor in the epoll loop
TEST_F(EPollLoopTest, execute_in_pollloop)
{
  EPollLoop eloop("test");
  ASSERT_THAT(eloop.start(), true);

  ASSERT_THAT(eloop.running(), true);
  // expects on different thread
  ASSERT_THAT(eloop.threadId(), Ne(std::this_thread::get_id()));

  bool called_{false};

  EPollLoop::Executor call_ = [&]() {
    // expects on the same epoll thread since this functor runs on it now.
    ASSERT_THAT(eloop.threadId(), std::this_thread::get_id());
    called_ = true;
  };

  // blocking call
  eloop.executeInPollloop(call_);

  EXPECT_THAT(called_, true);
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
