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

namespace cxx_ex
{
} // namespace

TEST(CCon, Ex)
{
  using namespace cxx_ex;

  EXPECT_THAT(true, true);
}


// ={=========================================================================

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
