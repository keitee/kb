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

namespace cxx_rvo
{
  // Note: All methods have side effects
  struct Snitch
  {
    Snitch(int value)
        : value_(value)
    {
      cout << "c'tor" << endl;
    }
    ~Snitch() { cout << "d'tor" << endl; }

    Snitch(const Snitch &) { cout << "copy c'tor" << endl; }

    Snitch &operator=(const Snitch &)
    {
      cout << "copy assignment" << endl;
      return *this;
    }

    /*
    Snitch(Snitch &&) { cout << "move c'tor" << endl; }

    Snitch &operator=(Snitch &&)
    {
      cout << "move assignment" << endl;
      return *this;
    }
    */

    int getValue() const { return value_; }

  private:
    int value_{0};
  };

  Snitch ExampleRVO()
  {
    std::cout << "in ExampleRVO: " << std::endl;
    return Snitch(100);
  }

  std::vector<Snitch> ReturnVector()
  {
    // vector<Snitch> ivec(1000000000, 1);
    // vector(n, elem); creates n elements
    std::vector<Snitch> coll(10, Snitch(200));
    cout << "in ReturnVector: size of vector: " << coll.size() << endl;
    return coll;
  }

  Snitch createSnitch() { return Snitch(200); }

  void foo(Snitch s) { cout << "snitch value is: " << s.getValue() << endl; }

} // namespace cxx_rvo

TEST(CxxRVO, single_construction)
{
  using namespace cxx_rvo;

  Snitch snitch = ExampleRVO();
}

// ={=========================================================================

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
