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

/*
*/

TEST(CxxPad, pad_1)
{
  std::vector<int> coll2{0, 1, 2, 3, 4, 5, 6, 7, 8};

  // in every iteration, update it which is invalidated after insert/erase.
  for (auto it = coll2.begin(); it != coll2.end(); /* no */)
  {
    // if see even values, remove it
    if (!(*it % 2))
    {
      cout << "it : " << *it << endl;
      it = coll2.erase(it);
    }
    else
      ++it;
  }

  EXPECT_THAT(coll2, ElementsAre(1, 3, 5, 7));
}

// ={=========================================================================

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
