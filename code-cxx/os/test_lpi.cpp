#include <boost/lexical_cast.hpp>
#include <boost/variant.hpp>
#include <chrono>
#include <condition_variable>
#include <forward_list>
#include <iostream>
#include <limits>
#include <list>
#include <memory>
#include <mutex>
#include <regex>
#include <set>
#include <thread>
#include <vector>

#include <sys/prctl.h>

#include "gmock/gmock.h"

using namespace std;
using namespace std::placeholders;
using namespace testing;

// (gdb) b
// PatternObserver_sendNotificationWithCallerDispatcherButRaiseException_Test::TestBody()

/*
={=============================================================================
lpi-socket-unix

*/

TEST(LpiSocket, UnixDomain)
{
  EXPECT_THAT(true, true);
}


// ={=========================================================================

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
