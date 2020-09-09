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

/*

class Stack
{
   bool empty() const;
   void push(T const &elem);
   void pop();
   T &top();
}

*/

namespace cxx_stack
{
  class ReadOnEmptyStack : public std::exception
  {
  public:
    virtual const char *what() const noexcept
    {
      return "connot read when statck is empty";
    }
  };

  template <typename T>
  class Stack
  {
  private:
    std::deque<T> _coll;

  public:
    bool empty() const { return _coll.empty(); }

    void push(const T &elem) { _coll.emplace_back(elem); }

    void pop()
    {
      if (_coll.empty())
        throw ReadOnEmptyStack();

      _coll.pop_back();
    }

    T &top()
    {
      if (_coll.empty())
        throw ReadOnEmptyStack();

      // note "back() but not pop_back()"
      return _coll.back();
    }
  };
} // namespace cxx_stack

TEST(StlStack, check_implemented_in_terms_of)
{
  using namespace cxx_stack;

  Stack<int> coll{};
  std::vector<int> result{};

  coll.push(10);
  coll.push(20);
  coll.push(30);

  coll.pop();

  coll.top() = 80;

  coll.push(100);
  coll.push(200);

  while (!coll.empty())
  {
    result.emplace_back(coll.top());
    coll.pop();
  }

  EXPECT_THAT(result, ElementsAre(200, 100, 80, 10));

  // stack is already empty
  EXPECT_THROW(coll.pop(), ReadOnEmptyStack);
}

// ={=========================================================================

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
