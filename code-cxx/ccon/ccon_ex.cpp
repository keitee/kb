#include <iostream>
#include <thread>
#include <future>
#include <list>
#include <mutex>
#include <queue>
#include <stack>
#include <exception>
#include <condition_variable>

// #include <string.h>
// #include <pthread.h>
// #include <sys/syscall.h>
// #include <unistd.h>

#include "gmock/gmock.h"

using namespace std;
using namespace std::placeholders;
using namespace testing;

namespace cxx_condition
{
  template <typename T>
    class threadsafe_queue 
    {
      public:
        explicit threadsafe_queue() noexcept {}
        threadsafe_queue(threadsafe_queue const&) = delete;
        threadsafe_queue& operator=(threadsafe_queue const&) = delete;

        void push(T const& item)
        {
          std::lock_guard<std::mutex> lock(m_);

          queue_.push(item);
          cond_.notify_one();
        }

        void wait_and_pop(T& value) 
        {
          // to use with cxx-condition
          std::unique_lock<std::mutex> lock(m_);

          while (queue_.empty())
            cond_.wait(lock, [&]{return !queue_.empty();});

          value = queue_.front();
          queue_.pop();
        }

        bool empty() const
        {
          std::lock_guard<std::mutex> lock(m_);
          return queue_.empty();
        }

      private:
        std::queue<T> queue_;
        mutable std::mutex m_;
        std::condition_variable cond_;
    };

  std::mutex consumed_mtx;
  int consumed_total{};

  void producer(threadsafe_queue<int>& q)
  {
    for (int i = 0; i < 20; ++i)
    {
      q.push(i);
    }
  }

  void consumer(threadsafe_queue<int>& q)
  {
    int value{};

    for (int i = 0; i < 20; ++i)
    {
      q.wait_and_pop(value);
      ++consumed_total;
    }
  }

  void consumer_error(threadsafe_queue<int>& q)
  {
    std::unique_lock<std::mutex> lock(consumed_mtx, std::defer_lock);
  
    int value{};
  
    while (true)
    {
      if (consumed_total >= 20*3)
        break;
  
      q.wait_and_pop(value);
  
      lock.lock();
      ++consumed_total;
      lock.unlock();
    }
  }

} // namesapce

// similar to linux-sync-cond-lpi-example

TEST(CConCondition, ProducerAndConsumer)
{
  using namespace cxx_condition;

  threadsafe_queue<int> q;

  std::vector<std::thread> consumers;
  std::vector<std::thread> producers;
  
  for (int i = 0; i < 3; ++i)
  {
    consumers.emplace_back(consumer, std::ref(q));
  }

  this_thread::sleep_for(chrono::seconds(2));

  for (int i = 0; i < 3; ++i)
  {
    producers.emplace_back(producer, std::ref(q));
  }

  for (auto &c : consumers)
    c.join();

  for (auto &p : producers)
    p.join();

  EXPECT_THAT(consumed_total, 60);
}

namespace xxx
{
  std::queue<int> queue;
  std::mutex queueMutex;
  std::condition_variable queueCondVar;

  void provider (int val)
  {
    // push different values (val til val+5 with timeouts of val milliseconds into the queue
    for (int i=0; i<6; ++i) {
      {
        std::lock_guard<std::mutex> lg(queueMutex);
        queue.push(val+i);
      } // release lock
      queueCondVar.notify_one();
      std::this_thread::sleep_for(std::chrono::milliseconds(val));
    }
  }
  void consumer (int num)
  {
    // pop values if available (num identifies the consumer)
    while (true) {
      int val;
      {
        std::unique_lock<std::mutex> ul(queueMutex);
        queueCondVar.wait(ul,[]{ return !queue.empty(); });
        val = queue.front();
        queue.pop();
      } // release lock
      std::cout << "consumer " << num << ": " << val << std::endl;
    }
  }
} // namespace

TEST(X,XX)
{
  using namespace xxx;

  // start three providers for values 100+, 300+, and 500+
  auto p1 = std::async(std::launch::async,provider,100);
  auto p2 = std::async(std::launch::async,provider,300);
  auto p3 = std::async(std::launch::async,provider,500);
  // start two consumers printing the values
  auto c1 = std::async(std::launch::async,consumer,1);
  auto c2 = std::async(std::launch::async,consumer,2);
}


// ={=========================================================================

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
