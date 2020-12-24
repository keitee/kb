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

#include <sys/prctl.h>

#include "gmock/gmock.h"

using namespace std;
using namespace testing;

// ={=========================================================================
TEST(StlStack, check_implemented_in_terms_of)
{
  int value1{100};
  size_t value2{200};

  printf("%s: value is {%d, %d}\n", __func__, value1, value2);
  printf("%s: value is {%u, %u}\n", __func__, value1, value2);
}

// ={=========================================================================

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
