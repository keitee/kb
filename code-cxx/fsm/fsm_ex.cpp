#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <queue>
#include <future>

#include "gmock/gmock.h"

// to exercise cxx fsm code

using namespace testing;

namespace cxx_fsm
{
  template<typename T>
    class queue
    {
      public:
        void push(T const &message)
        {
          std::lock_guard<std::mutex> lock(m_);

          queue.push(message);
          cv_.notify_all();
        }

        T wait_and_pop()
        {
          std::unique_lock<std::mutex> lock(m_);

          // wait until q it not empty
          cv_.wait(lock, [&]{return !q_.empty();});

          auto message = q_.front();
          m_.pop();
          return message;
        }

      private:
        std::condition_variable cv_;
        std::mutex m_;
        std::queue<T> q_;
    };
} // namespce


// ={=========================================================================

TEST(FsmEx, Exercise)
{
  EXPECT_TRUE(1);
}


// ={=========================================================================

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
