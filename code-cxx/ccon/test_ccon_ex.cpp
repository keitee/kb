#include <condition_variable>
#include <exception>
#include <future>
#include <iostream>
#include <list>
#include <mutex>
#include <queue>
#include <stack>
#include <thread>

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
} // namespace cxx_ex

TEST(CCon, Ex)
{
  using namespace cxx_ex;

  EXPECT_THAT(true, true);
}

// ={=========================================================================

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
