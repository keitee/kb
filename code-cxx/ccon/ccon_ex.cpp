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
// cxx-ex

// class locked_queue {
//  void push(T &);
//  T pop();
// };

template <typename T>
class locked_queue {
  public:
    void push(T const& item)
    {
    }

  private:
    std::queue<T> queue_;
    std::mutex lock_;
    std::condition_variable cond_;
};

// ={=========================================================================
int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
