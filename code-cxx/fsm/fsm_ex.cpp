#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <queue>
#include <future>

#include "gmock/gmock.h"


using namespace testing;

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
