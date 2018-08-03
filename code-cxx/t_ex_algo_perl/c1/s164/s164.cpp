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
// algo-pearl: Column 1. 1.6 Problems. 4.

// Problem 1.6.4
//
// If you face Problem 3 seriously, you will face the problem of generating k
// integers less than n `without duplicates.` The simplest approach uses the
// first k positive integers. This extreme data set won't alter the run time of
// the bitmap method by much, but it might skew the run time of a system sort.
// How could you generate a file of k unique random integers between 0 and n-1
// in random order? Strive for a short program that is also efficient.
//
// Solution 4.
//
// See Column 12. This code assumes that randint(1, u) returns a random integer
// 1..u.
//
// // select k unique integers out of n integers
//
// for i = [0, n)
//  x[i] = i
//
// for i = [0, k)
//  swap(i, randint(i, n-1))
//  print x[i]
//
// The swap function exchanges the two elements in x. The randint() is discussed
// in detail in Section 12.1.
//
// Problem 12.5.1
//
// The C library rand() typically returns about 15 random bits. Use that
// function to implement a function bigrand() to return at least 30 random bits,
// and a function randint(l, u) to return integer in the range [l, u].
//
// Solution 12.5.1
// 
// These functions return a large(usually 30 bit) random number and a random
// number in the specified input range, respectively.

namespace pearl_12_5_1
{
  int bigrand()
  {
    return RAND_MAX*rand() + rand();
  }

  int randint(int l, int u)
  {
    // makes negative numbers due to overflow. see *ex-itoa*
    // return l + (bigrand() % (u-l+1));
    
    return l + (rand() % (u-l+1));
  }
} // namespace

TEST(ProgrammingPearl, C12_5_1)
{
  using namespace pearl_12_5_1;

  multiset<int> coll{};
  bool duplicates{false};

  for (int i = 0; i < 100; ++i)
  {
    auto value = randint(10, 100);
    coll.insert(value);
  }
 
  for (auto e : coll)
  {
    if (1 < coll.count(e))
      duplicates = true;
  }

  EXPECT_THAT(duplicates, true);
}


namespace pearl_1_6_4
{
  // How to slect `unique random numbers`?
  //
  // https://github.com/filipegoncalves/codinghighway/tree/master/ProgrammingPearls/Column1/1.6.4
  //
  // Here, we are faced with the challenge of avoiding duplicates. Column 12 is
  // a must-read to solve this exercise the good way. It introduces very
  // important concepts and algorithms to efficiently generating a set of m
  // unique random integers in [0, n-1], with m <= n.
  // 
  // Because we don't want the set to be ordered, we cannot use Knuth's
  // Algorithm S. Instead, we opt for Knuth's Algorithm P, in which we shuffle
  // an n-element array that contains the numbers 0..n-1.  To implement
  // Algorithm P, we need a randint() function. We will learn more about it in
  // column 13; for now, it simply adds a random quantity to the lower bound
  // that doesn't exceed the upper bound.
  // 
  // The resulting file, numbers2.in, is provided in this column's root folder.

  const int N=10000000;
  const int M=1000000;
  int arr[N];

  int randint(int from, int to) {
    return from + (rand()%(to-from+1));
  }

  void swap(int i, int j) {
    int t = arr[i];
    arr[i] = arr[j];
    arr[j] = t;
  }
}

// so not sorted but makes unique random numbers

TEST(ProgrammingPearl, C1_6_4)
{
  using namespace pearl_1_6_4;

  multiset<int> coll{};
  bool duplicates{false};

	int i;

	srand(time(NULL));

  // makes unique numbers
	for (i = 0; i < N; i++)
		arr[i] = i;

  // shuffle
	for (i = 0; i < M; i++)
		swap(i, randint(i, N-1));

  // output
	for (i = 0; i < M; i++)
  {
    coll.insert(arr[i]);
		// printf("%d\n", arr[i]);
  }

  for (auto e : coll)
  {
    if (1 < coll.count(e))
      duplicates = true;
  }

  EXPECT_THAT(duplicates, false);
}


int main(int argc, char** argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
