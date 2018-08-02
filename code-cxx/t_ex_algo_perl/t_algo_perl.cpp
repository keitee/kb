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
// algo-swap: swap without a temporary

namespace pearl_produce_duplicates
{
  int bigrand()
  {
    return RAND_MAX*rand() + rand();
  }

  int randint(int l, int u)
  {
    // return l + (bigrand() % (u-l+1));
    return l + (rand() % (u-l+1));
  }
} // namespace

TEST(C12, RandWithDuplicates)
{
  using namespace pearl_produce_duplicates;

  multiset<int> coll{};
 
  for (int i = 0; i < 100; ++i)
  {
    auto value = randint(10, 100);
    coll.insert(value);
  }
 
  cout << "duplicates: ";
  for (auto e : coll)
  {
    if (1 < coll.count(e))
      cout << e << ", ";
  }
  cout << endl;
}


namespace pearl_produce_without_duplicates
{
  // 1.6.4
  // If you face Problem 3 seriously, you will face the problem of generating k integers less than n without duplicates. The simplest approach uses the first k positive integers.
  // This extreme data set won't alter the run time of the bitmap method by much, but it might skew the run time of a system sort. How could you generate a file of k unique random integers
  // between 0 and n-1 in random order? Strive for a short program that is also efficient.
  // --
  // Here, we are faced with the challenge of avoiding duplicates. Column 12 is a must-read to solve this exercise the good way. It introduces very important concepts and algorithms to efficiently
  // generating a set of m unique random integers in [0, n-1], with m <= n.
  // 
  // Because we don't want the set to be ordered, we cannot use Knuth's Algorithm S. Instead, we opt for Knuth's Algorithm P, in which we shuffle an n-element array that contains the numbers 0..n-1.
  // To implement Algorithm P, we need a randint() function. We will learn more about it in column 13; for now, it simply adds a random quantity to the lower bound that doesn't exceed the upper bound.
  // 
  // The resulting file, numbers2.in, is provided in this column's root folder.

  const int N=1000;
  const int M=100;
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

TEST(C_1_6_4, Rand)
{
  using namespace pearl_produce_without_duplicates;

  multiset<int> coll{};

	int i;
	srand(time(NULL));
	for (i = 0; i < N; i++)
		arr[i] = i;
	for (i = 0; i < M; i++)
		swap(i, randint(i, N-1));
	for (i = 0; i < M; i++)
  {
    coll.insert(arr[i]);
		// printf("%d\n", arr[i]);
  }

  cout << "duplicates: ";
  for (auto e : coll)
  {
    if (1 < coll.count(e))
      cout << e << ", ";
  }
  cout << endl;
}



int main(int argc, char** argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
