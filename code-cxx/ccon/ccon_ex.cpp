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

namespace cxx_atomic
{
  struct Counter {

    Counter() : value(0) {}

    int value;

    void increment(){
      ++value;
    }

    void decrement(){
      --value;
    }

    int get(){
      return value;
    }
  };

  void increment_counter(Counter& counter)
  {
    for (int i = 0; i < 30; ++i)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      counter.increment();
    }
  }

  struct AtomicCounter {

    AtomicCounter() : value(0) {}

    std::atomic<int> value;

    void increment(){
      ++value;
    }

    void decrement(){
      --value;
    }

    int get(){
      return value.load();
    }
  };

  void increment_atomic_counter(AtomicCounter& counter)
  {
    for (int i = 0; i < 30; ++i)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      counter.increment();
    }
  }

} // namespace

TEST(DISABLED_CConAtomic, NoAtomicCounter)
{
  using namespace cxx_atomic;

  Counter counter;

  std::thread a(increment_counter, std::ref(counter));
  std::thread b(increment_counter, std::ref(counter));
  std::thread c(increment_counter, std::ref(counter));
  std::thread d(increment_counter, std::ref(counter));

  a.join(); b.join(); c.join(); d.join();
  
  cout << "counter value: " << counter.get() << endl;
  ASSERT_THAT(counter.get(), 120);
}

TEST(CConAtomic, AtomicCounter)
{
  using namespace cxx_atomic;

  AtomicCounter counter;

  std::thread a(increment_atomic_counter, std::ref(counter));
  std::thread b(increment_atomic_counter, std::ref(counter));
  std::thread c(increment_atomic_counter, std::ref(counter));
  std::thread d(increment_atomic_counter, std::ref(counter));

  a.join(); b.join(); c.join(); d.join();
  
  cout << "counter value: " << counter.get() << endl;
  ASSERT_THAT(counter.get(), 120);
}


// ={=========================================================================

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
