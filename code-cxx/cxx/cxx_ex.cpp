#include <iostream>
#include <set>
#include <vector>
#include <memory>
#include <chrono>
#include <limits>
#include <thread>
#include <list>
#include <forward_list>
#include <regex>
#include <boost/lexical_cast.hpp>

#include "gmock/gmock.h"

using namespace std;
using namespace std::placeholders;
using namespace testing;

class foo
{
  public:

    explicit foo(int &value) : value_(value) 
    {
      cout << "foo(int)" << endl;
    }

    foo(int &value, int) : value_(value) 
    {
      cout << "foo(int, int)" << endl;
    }

  private:
    int value_;
};


TEST(Cxx, Ex)
{ 
  int value{10};

  // *cxx-error*
  //
  // cxx_ex.cpp: In member function ‘virtual void Cxx_Ex_Test::TestBody()’:
  // cxx_ex.cpp:42:12: error: no matching function for call to ‘foo::foo(int)’
  //    foo f1(10);
  //             ^
  // cxx_ex.cpp:42:12: note: candidates are:
  // (1)
  // cxx_ex.cpp:27:5: note: foo::foo(int&, int)
  //      foo(int &value, int) : value_(value)
  //      ^
  // cxx_ex.cpp:27:5: note:   candidate expects 2 arguments, 1 provided
  // (2)
  // cxx_ex.cpp:23:14: note: foo::foo(int&)
  //      explicit foo(int &value) : value_(value)
  //               ^
  // cxx_ex.cpp:23:14: note:   no known conversion for argument 1 from ‘int’ to ‘int&’
  // (3)
  // cxx_ex.cpp:19:7: note: constexpr foo::foo(const foo&)
  //  class foo
  //        ^
  // cxx_ex.cpp:19:7: note:   no known conversion for argument 1 from ‘int’ to ‘const foo&’
  // (4)
  // cxx_ex.cpp:19:7: note: constexpr foo::foo(foo&&)
  // cxx_ex.cpp:19:7: note:   no known conversion for argument 1 from ‘int’ to ‘foo&&’
  //
  // foo f1(10);

  foo f1(value);
  foo f2(value, 30);
} 


// ={=========================================================================

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

