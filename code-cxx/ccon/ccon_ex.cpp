#include <iostream>
#include <thread>
#include <future>
#include <list>
#include <mutex>
#include <queue>
#include <stack>
#include <exception>

#include <boost/thread/shared_mutex.hpp>

#include "gmock/gmock.h"

using namespace std;
using namespace std::placeholders;
using namespace testing;


// ={=========================================================================
// cxx-condition

// class locked_queue {
//  void push(T &);
//  T pop();
// };

namespace cxx_condition
{
  template <typename T>
    class locked_queue {
      public:
        void push(T const& item)
        {
          std::lock_guard<std::mutex> lock(m_);
          queue_.push(item);

          // as soon as there are items in queue
          if (queue_.size())
            cond_.notify_one();
        }

        T pop() 
        {
          // to use with cxx-condition
          std::unique_lock<std::mutex> lock(m_);

          while (queue_.empty())
            cond_.wait(m_);

          T item = queue_.front();
          queue_.pop();
          return item;
        }

      private:
        std::queue<T> queue_;
        std::mutex m_;
        std::condition_variable cond_;
    };

  void producer(locked_queue<int> &q)
  {
    for (int i = 0; i < 10; ++i)
    {
      ++value;
    }
  }

  void consumer(locked_queue<int> &q)
  {
    for (int i = 0; i < 10; ++i)
    {
      ++value;
    }
  }

} // namesapce

TEST(CConCondition, ProducerAndConsumer)
{
  using namespace cxx_condition;

  locked_queue<int> q;

  std::thread c1(&consumer, &q);
  std::thread c2(&consumer, &q);
  std::thread c3(&consumer, &q);
}


// ={=========================================================================
int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
