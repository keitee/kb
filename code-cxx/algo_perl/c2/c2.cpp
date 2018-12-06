#include <iostream>
#include <cstring>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <bitset>

#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;


// ={=========================================================================
template <typename T>
void PRINT_ELEMENTS( T& coll, const string optstr="" )
{
    size_t count{};
    cout << optstr;

    for( const auto &elem : coll )
    {
        cout << elem << " ";
        ++count;
    }

    cout << "(" << count << ")" << endl;
}

template <typename T>
void PRINT_M_ELEMENTS( T& coll, const string optstr="" )
{
    size_t count{};
    cout << optstr;

    for( const auto &elem : coll )
    {
        cout << "(" << elem.first << ", " << elem.second << ") ";
        ++count;
    }

    cout << "(" << count << ")" << endl;
}


// ={=========================================================================
// rotate

TEST(AlgoRotate, Rotate)
{
  vector<int> coll{1,2,3,4,5,6,7,8};

  // rotate one to the left
  // GCC 4.9.2, void rotate() so comment out 
  // auto pos = rotate(
  rotate(
    coll.begin(),     // begin  
    coll.begin()+1,   // new begin
    coll.end()        // end
  );
  EXPECT_THAT(coll, ElementsAre(2,3,4,5,6,7,8,1));

  // return the new position of the (pervious) first element.
  // EXPECT_THAT(*pos, 1);

  // rotate two to the right or think that rotate to the left since `no direction` 
  // in the call definition.
  // 
  // from stl
  //  *  This effectively swaps the ranges @p [__first,__middle) and
  //  *  @p [__middle,__last).

  // pos = rotate(
  rotate(
    coll.begin(),
    coll.end()-2,
    coll.end()
  );
  EXPECT_THAT(coll, ElementsAre(8,1,2,3,4,5,6,7));
  // EXPECT_THAT(*pos, 2);

  // rotate so that 4 is the beginning
  // pos = rotate(
  rotate(
    coll.begin(),
    find(coll.begin(), coll.end(), 4),
    coll.end()
  );
  EXPECT_THAT(coll, ElementsAre(4,5,6,7,8,1,2,3));
  // EXPECT_THAT(*pos, 8);
}

using ROTATE_ITER = vector<int>::iterator;

void my_rotate(ROTATE_ITER begin, ROTATE_ITER new_begin, ROTATE_ITER end)
{
  auto num_swap = distance(new_begin, end);
  for (; new_begin != begin; --new_begin)
  {
    auto start = new_begin;
    for (int i = 0; i < num_swap; ++i)
    {
      swap(*start, *(start-1));
      ++start;
    }
  }
}

// /usr/include/c++/4.9.2/bits/stl_algo.h
//
// /// This is a helper function for the rotate algorithm.
// template<typename _ForwardIterator>
//   _ForwardIterator
//   __rotate(_ForwardIterator __first,
//      _ForwardIterator __middle,
//      _ForwardIterator __last,
//      forward_iterator_tag)
// {}
//
// /// This is a helper function for the rotate algorithm.
// template<typename _RandomAccessIterator>
//   _RandomAccessIterator
//   __rotate(_RandomAccessIterator __first,
//      _RandomAccessIterator __middle,
//      _RandomAccessIterator __last,
//      random_access_iterator_tag)
// {}

TEST(AlgoRotate, OwnRotate)
{
  vector<int> coll{1,2,3,4,5,6,7,8};

  // rotate one to the left
  my_rotate(
    coll.begin(),     // begin  
    coll.begin()+1,   // new begin
    coll.end()        // end
  );
  EXPECT_THAT(coll, ElementsAre(2,3,4,5,6,7,8,1));

  my_rotate(
    coll.begin(),
    coll.end()-2,
    coll.end()
  );
  EXPECT_THAT(coll, ElementsAre(8,1,2,3,4,5,6,7));

  my_rotate(
    coll.begin(),
    find(coll.begin(), coll.end(), 4),
    coll.end()
  );
  EXPECT_THAT(coll, ElementsAre(4,5,6,7,8,1,2,3));
}


// Programming Pearl, p14, reverse version
// 
// The reversal code is time and space efficient, and is so short adn simple
// that it's pretty hard to get wrong. 
//
// abcdefgh, i=3, n=8, rotate to defgh|abc
//
// reverse(0, i-1)  cba|defgh
// reverse(i, n-1)  cba|hgfed
// reverse(0, n-1)  defgh|abc

void reverse_rotate(ROTATE_ITER begin, ROTATE_ITER new_begin, ROTATE_ITER end)
{
  // reverse(begin, end) reverse [begin, end)
  reverse(begin, new_begin);
  reverse(new_begin, end);
  reverse(begin, end);
}

TEST(AlgoRotate, ReverseRotate)
{
  vector<int> coll{1,2,3,4,5,6,7,8};

  // rotate one to the left
  reverse_rotate(
    coll.begin(),     // begin  
    coll.begin()+1,   // new begin
    coll.end()        // end
  );
  EXPECT_THAT(coll, ElementsAre(2,3,4,5,6,7,8,1));

  reverse_rotate(
    coll.begin(),
    coll.end()-2,
    coll.end()
  );
  EXPECT_THAT(coll, ElementsAre(8,1,2,3,4,5,6,7));

  reverse_rotate(
    coll.begin(),
    find(coll.begin(), coll.end(), 4),
    coll.end()
  );
  EXPECT_THAT(coll, ElementsAre(4,5,6,7,8,1,2,3));
}

// ={=========================================================================
// binary search

/* 
2.1.A

Given a sequential file that contains at most four billion 32 bit integers in
random order, find a 32 bit integer that isn't in the file (and there must be at
least one missing -why?). How would you solve this problem with ample quantities
of main memory? How would you solve it if you could use several external scratch
files but only a few hundred bytes of main memory?


p13. The insight is that we can probe a range by `counting` the elements above
and below its midpoint: either the upper or the lower range has at most half the
elements in the total range. Because the total range has a missing element, the
smaller half must also have a missing element.

9/2 = 4, when 8 is mssing

|1|2|3|4|5|6|7|9|10|
         ^|

9/2 = 4, when 2 is mssing

|1|3|4|5|6|7|8|9|10|
         ^|

How this work? This is not what it meant and  it means:

p208. Ed Reingold method. 

In the first pass, read the (at most) four billion input integers and write
those with leading zero bit to one sequential file and those with a leading one
bit to another file. One of those two files contains at most two billion
integers, so next use that file as the current input and repeat the probe
process, but this time on the second bit.

(1 bit shift means halving the range as shown below)

2^32 = 4 Billion 

  000   2^0, 0  { 4 elements
  001
  010   2^1, 2
  011           }
  100   2^2, 4  { 4 elements
  101
  110
  111           }
01000   2^3, 8
01001
01010
01011
01100
01101
01110
01111
10000   2^4, 16

*/


// ={=========================================================================

int main(int argc, char** argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
