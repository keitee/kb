/*

*/
#include <algorithm>
#include <deque>
#include <iostream>
#include <memory>
#include <vector>

#include <condition_variable>
#include <mutex>
#include <thread>
#include <variant>

#include <sys/prctl.h>

#include "gmock/gmock.h"

using namespace std;
using namespace testing;

// ={=========================================================================
TEST(CxxTypeVariant, custom_variant)
{
  // specify types to hold
  std::variant<int, std::string, double> v = 1;
}

// ={=========================================================================

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
