#include <boost/lexical_cast.hpp>
#include <chrono>
#include <cstdarg>
#include <forward_list>
#include <iostream>
#include <limits>
#include <list>
#include <memory>
#include <regex>
#include <set>
#include <thread>
#include <vector>

#include "gmock/gmock.h"

using namespace std;
using namespace std::placeholders;
using namespace testing;

// ={=========================================================================

TEST(Cxx_1, Test1)
{
  EXPECT_THAT(true, true);
}


// ={=========================================================================

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
