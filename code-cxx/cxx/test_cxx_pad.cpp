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
  // While std::string do not support cxx-bool-conversion, std::optional does.
  // Have used "!!" to use that in EXPECT_.
  {
    std::optional<std::string> option{};

    EXPECT_THAT(!!option, false);
  }
}

// ={=========================================================================

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
