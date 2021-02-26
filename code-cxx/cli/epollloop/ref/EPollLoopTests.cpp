#include "EPollLoop.h"

#include <Logging.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

// CDI key codes
extern "C" {
#include <common.h>
}



class EPollLoopTest : public ::testing::Test
{
public:
    void SetUp()
    {
        AICommon::initLogging();
    }

    void TearDown()
    {
    }

protected:


public:
    typedef std::chrono::time_point<std::chrono::steady_clock> TimePoint;

    std::vector< std::pair<int, TimePoint> > mTimerEvents;

    void onTimerEvent(int eventId, int64_t tag)
    {
        (void)tag;

        mTimerEvents.emplace_back(eventId, std::chrono::steady_clock::now());
    }

    long long timeDiff(const TimePoint& a, const TimePoint& b)
    {
        std::chrono::milliseconds diff = std::chrono::duration_cast<std::chrono::milliseconds>(a - b);
        return diff.count();
    }

};


TEST_F(EPollLoopTest, testSimpleTimeout)
{
    EPollLoop loop("test");
    ASSERT_TRUE(loop.start());

    TimePoint start = std::chrono::steady_clock::now();

    // set 4 timers 100ms apart and ensure they all fire within sensible ranges
    ASSERT_TRUE(loop.singleShotTimer(std::chrono::milliseconds(300), std::bind(&EPollLoopTest::onTimerEvent, this, 300, std::placeholders::_1)));

    ASSERT_TRUE(loop.singleShotTimer(std::chrono::milliseconds(200), std::bind(&EPollLoopTest::onTimerEvent, this, 200, std::placeholders::_1)));

    ASSERT_TRUE(loop.singleShotTimer(std::chrono::milliseconds(100), std::bind(&EPollLoopTest::onTimerEvent, this, 100, std::placeholders::_1)));

    ASSERT_TRUE(loop.singleShotTimer(std::chrono::milliseconds(0), std::bind(&EPollLoopTest::onTimerEvent, this, 0, std::placeholders::_1)));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    ASSERT_EQ(mTimerEvents.size(), 4U);

    ASSERT_EQ(mTimerEvents[0].first, 0);
    ASSERT_NEAR(timeDiff(mTimerEvents[0].second, start), 0, 50);

    ASSERT_EQ(mTimerEvents[1].first, 100);
    ASSERT_NEAR(timeDiff(mTimerEvents[1].second, start), 100, 50);

    ASSERT_EQ(mTimerEvents[2].first, 200);
    ASSERT_NEAR(timeDiff(mTimerEvents[2].second, start), 200, 50);

    ASSERT_EQ(mTimerEvents[3].first, 300);
    ASSERT_NEAR(timeDiff(mTimerEvents[3].second, start), 300, 50);
}

TEST_F(EPollLoopTest, testSimplePeriodicTimer)
{
    EPollLoop loop("test");
    ASSERT_TRUE(loop.start());

    const std::chrono::milliseconds period(20);
    const std::chrono::milliseconds testPeriod(300);

    int64_t id = loop.addTimer(period, period, std::bind(&EPollLoopTest::onTimerEvent, this, 123, std::placeholders::_1));
    ASSERT_GT(id, 0);
    ASSERT_TRUE(loop.startTimer(id));

    std::this_thread::sleep_for(testPeriod);


    ASSERT_NEAR(static_cast<long long>(mTimerEvents.size()),
                (testPeriod.count() / period.count()), 5LL);

    std::vector< std::pair<int, TimePoint> >::const_iterator event = mTimerEvents.begin() + 1;
    for (; event != mTimerEvents.end(); ++event)
    {
        ASSERT_EQ(event->first, 123);

        TimePoint prevTime = (event - 1)->second;
        long long diff = timeDiff(event->second, prevTime);

        ASSERT_NEAR(diff, period.count(), 10);
    }
}

TEST_F(EPollLoopTest, testCancelPeriodicTimer)
{
    EPollLoop loop("test");
    ASSERT_TRUE(loop.start());


    int64_t id = loop.addTimer(std::chrono::milliseconds(50), std::chrono::milliseconds(50),
                               std::bind(&EPollLoopTest::onTimerEvent, this, 246, std::placeholders::_1));
    ASSERT_GT(id, 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    TimePoint start = std::chrono::steady_clock::now();
    ASSERT_TRUE(loop.startTimer(id));

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    ASSERT_TRUE(loop.stopTimer(id));

    size_t nEvents = mTimerEvents.size();
    ASSERT_GE(nEvents, 3U);
    ASSERT_LE(nEvents, 4U);

    ASSERT_EQ(mTimerEvents[0].first, 246);
    EXPECT_NEAR(timeDiff(mTimerEvents[0].second, start), 50,  20);
    EXPECT_NEAR(timeDiff(mTimerEvents[1].second, start), 100, 20);
    EXPECT_NEAR(timeDiff(mTimerEvents[2].second, start), 150, 20);


    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    ASSERT_EQ(nEvents, mTimerEvents.size());
}

TEST_F(EPollLoopTest, testPeriodicTimerWithInitialtime)
{
    EPollLoop loop("test");
    ASSERT_TRUE(loop.start());

    const std::chrono::milliseconds firstPeriod(200);
    const std::chrono::milliseconds secondPeriod(33);
    const std::chrono::milliseconds testPeriod(300);

    int64_t id = loop.addTimer(firstPeriod, secondPeriod, std::bind(&EPollLoopTest::onTimerEvent,
                                                                    this, 5678, std::placeholders::_1));
    ASSERT_GT(id, 0);
    ASSERT_TRUE(loop.startTimer(id));
    TimePoint startTime = std::chrono::steady_clock::now();

    std::this_thread::sleep_for(testPeriod);

    loop.stop();


    ASSERT_NEAR(static_cast<long long>(mTimerEvents.size()), 6LL, 2LL);

    // the first delay should be close to 200ms
    std::vector< std::pair<int, TimePoint> >::const_iterator event = mTimerEvents.begin();
    {
        ASSERT_EQ(event->first, 5678);

        long long diff = timeDiff(event->second, startTime);

        ASSERT_NEAR(diff, firstPeriod.count(), 10);
    }

    // the following delays should be close to 33ms
    event++;
    for (; event != mTimerEvents.end(); ++event)
    {
        ASSERT_EQ(event->first, 5678);

        TimePoint prevTime = (event - 1)->second;
        long long diff = timeDiff(event->second, prevTime);

        ASSERT_NEAR(diff, secondPeriod.count(), 10);
    }
}

TEST_F(EPollLoopTest, testInThreadExecutor)
{
    EPollLoop loop("test");
    ASSERT_TRUE(loop.start());
    ASSERT_TRUE(loop.isRunning());

    const std::thread::id pollThreadId = loop.threadId();
    ASSERT_NE(pollThreadId, std::this_thread::get_id());

    volatile bool wasCalled = false;
    EPollLoop::Executor executor = [&]()
    {
        EXPECT_EQ(pollThreadId, std::this_thread::get_id());
        wasCalled = true;
    };

    loop.executeInPollloop(executor);

    ASSERT_TRUE(wasCalled);
}

