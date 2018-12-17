#include "gmock/gmock.h"

#include <iostream>
#include <list>
#include <deque>
#include <fstream>

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;


// ={=========================================================================

/*

Algorithms and Data Structures

45. Priority queue

Write a data structure that represents a priority queue that provides constant
time lookup for the largest element, but has logarithmic time complexity for
adding and removing elements. A queue inserts new elements at the end and
removes elements from the top. By default, the queue should use operator< to
compare elements, but it should be possible for the user to provide a comparison
function object that returns true if the first argument is less than the second.
The implementation must provide at least the following operations:

push() to add a new element
pop() to remove the top element
top() to provide access to the top element
size() to indicate the number of elements in the queue
empty() to indicate whether the queue is empty

A priority queue is an abstract data type whose elements have a priority
attached to them. Instead of working as a first-in-first-out container, a
priority queue makes elements available in the order of their priority. This
data structure is used in algorithms such as Dijkstra's shortest path, Prim's
algorithm, heap sort, the A* search algorithm, in Huffman codes used for data
compression, and others.

A very simple approach to implement a priority queue would be to use an
std::vector as the underlying container of elements and always maintain it
sorted. That means the maximum and minimum elements are always at the two ends.
However, this approach does not provide the most efficient operations.

The most suitable data structure that can be used to implement a priority queue
is a heap. This is a tree-based data structure that satisfies the following
property: if P is a parent node of C, then the key (the value) of P is either
greater than or equal to (in a max heap) or less than or equal to (in a min
heap) the key of C.

The standard library provides several operations for working with heaps:

std::make_heap(): This creates a max heap for the given range, using either
operator< or a user-provided comparison function to order the elements

std::push_heap(): This inserts a new element at the end of the max heap

std::pop_heap(): This removes the first element of the heap (by swapping the
values in the first and last position and making the sub-range [first, last-1) a
max heap)

A priority queue implementation, that uses std::vector to hold data and the
standard functions for heaps, can look as follows:

*/

namespace U45_text
{
  template <typename T, typename Compare = std::less<typename std::vector<T>::value_type>>
    class priority_queue
    {
      using size_type = typename std::vector<T>::size_type;
      using value_type = typename std::vector<T>::value_type;
      using const_reference = typename std::vector<T>::const_reference;
      using reference = typename std::vector<T>::reference;

      public:
        bool empty() const noexcept
        { return coll_.empty(); }

        size_type size() const noexcept 
        { return coll_.size(); }

        void push(value_type const& value)
        {
          coll_.push_back(value);
          std::push_heap(coll_.begin(), coll_.end(), f_);
        }

        void pop()
        {
          std::pop_heap(coll_.begin(), coll_.end(), f_);
          coll_.pop_back();
        }

        const_reference top() const 
        { return coll_.front(); }

      private:
        std::vector<T> coll_;
        Compare f_;
    };
} // namespace


TEST(U45, Text)
{
  using namespace U45_text;

  priority_queue<int> q;
  for (int i : {1, 5, 3, 1, 13, 21, 8})
  {
    q.push(i);
  }

  EXPECT_TRUE(!q.empty());
  EXPECT_TRUE(q.size() == 7);

  vector<int> coll{};

  while (!q.empty())
  {
    coll.push_back(q.top());
    q.pop();
  }

  // the default make max version which makes descending order.
  EXPECT_THAT(coll, ElementsAre(21, 13, 8, 5, 3, 1, 1));
}


/*
 
46. Circular buffer

Create a data structure that represents a circular buffer of a fixed size. A
circular buffer overwrites existing elements when the buffer is being filled
beyond its fixed size. The class you must write should:

Prohibit default construction

Support the creation of objects with a specified size

Allow checking of the buffer capacity and status (empty(), full(), size(),
capacity())

Add a new element, an operation that could potentially overwrite the oldest
element in the buffer

Remove the oldest element from the buffer

Support iteration through its elements

*/

namespace U46_text
{
  // done in cxx-algo
}


/*

47. Double buffer

Write a class that represents a buffer that could be written and read at the
same time without the two operations colliding. A read operation must provide
access to the old data while a write operation is in progress. Newly written
data must be available for reading upon completion of the write operation.

*/


// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
