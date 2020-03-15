#include "threadpool.h"
#include <condition_variable>
#include <gmock/gmock.h>
#include <mutex>

// Modern C++ Programming with Test-Driven Development
// c9/1/GeoServerTest.cpp

using namespace testing;

// test ThreadPool without createing a thread and which means it's testing queue
// handling only

class ThreadPoolTest : public Test
{
public:
  ThreadPool pool_;
};

TEST_F(ThreadPoolTest, ShouldHaveNoWorkOnCreation)
{
  EXPECT_THAT(pool_.hasWork(), false);
}

TEST_F(ThreadPoolTest, ShouldHaveWorkAfterAdd)
{
  pool_.add(Work{});
  EXPECT_THAT(pool_.hasWork(), true);
}

TEST_F(ThreadPoolTest, ShouldSeeSameWork)
{
  pool_.add(Work{10});
  auto work = pool_.pullWork();
  EXPECT_THAT(work.id(), 10);
}

TEST_F(ThreadPoolTest, ShouldSeeTheOrder)
{
  pool_.add(Work{10});
  pool_.add(Work{20});
  auto work = pool_.pullWork();
  EXPECT_THAT(work.id(), 10);
}

TEST_F(ThreadPoolTest, ShouldHaveNoWorkAfterPulled)
{
  pool_.add(Work{10});
  auto work = pool_.pullWork();
  EXPECT_THAT(pool_.hasWork(), false);
}

TEST_F(ThreadPoolTest, ShouldHaveWorkRemainedAfterPulled)
{
  pool_.add(Work{10});
  pool_.add(Work{20});
  auto work = pool_.pullWork();
  EXPECT_THAT(pool_.hasWork(), true);
}

// c9/9/ThreadPoolTest.cpp

// Now it creates a thread which runs on a pool by calling start().

class ThreadPoolAddRequestTest : public Test
{
  public:
    ThreadPool pool;
    std::mutex m;
    std::condition_variable cv;
    unsigned int count{0};

    std::vector<std::shared_ptr<std::thread>> threads;

  public:
    void SetUp() override { pool.start(); }

    void TearDown() override
    {
      for (auto &t : threads)
        t->join();
    }

    void incrementCountAndNotify()
    {
      std::lock_guard<std::mutex> lock(m);
      ++count;
      cv.notify_all();
    }

    bool waitForCountAndFailOnTimeout(unsigned int expectedCount)
    {
      std::unique_lock<std::mutex> lock(m);
      return cv.wait_for(lock, std::chrono::milliseconds(100), [&] {
        return count == expectedCount;
      });
    }
};

// the added work should be done by the pool thread
TEST_F(ThreadPoolAddRequestTest, ShouldBeRunOnPoolThread)
{
  Work work{[this] { incrementCountAndNotify(); }};
  unsigned int NumberOfWorkItems{1};

  pool.add(work);
  EXPECT_THAT(waitForCountAndFailOnTimeout(NumberOfWorkItems), true);
}

TEST_F(ThreadPoolAddRequestTest, ShouldBeRunOnPoolThreadMultipleTimes)
{
  Work work{[this] { incrementCountAndNotify(); }};
  unsigned int NumberOfWorkItems{10};

  for (unsigned int i{0}; i < NumberOfWorkItems; ++i)
    pool.add(work);

  EXPECT_THAT(waitForCountAndFailOnTimeout(NumberOfWorkItems), true);
}
