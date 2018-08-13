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

/* Copyright (C) 1999 Lucent Technologies
From 'Programming Pearls' by Jon Bentley

search.c -- test and time binary and sequential search

Select one of three modes by editing main() below.

1.) Probe one function
2.) Test one function extensively
3.) Time all functions
  Input lines: algnum n numtests
  Output lines: algnum n numtests clicks nanosecs_per_elem
  See timedriver for algnum codes
*/

#define MAXN  10000

using DataType = int;

DataType x[MAXN];

// note:
// n is global which binarysearch uses and also tests uses. n is range that
// binarysearch works on. The good thing is that runs on various input easily.

int n;

// scaffolding

// algo1: from PP, Column 4, raw transliteration
int binarysearch1(DataType t)
{
  int l, u, m;
  l = 0;
  u = n-1;

  for (;;)
  {
    if (l > u)
      return -1;

    m = (l + u)/2;

    if (x[m] < t)
      l = m+1;
    else if (x[m] == t)
      return m;
    else /* x[m] > t */
      u = m-1;
  }
}


// torture one algorithm

#define s binarysearch1
void test(int maxn)
{
  int i;
  
  // n is global which binarysearch uses; n is range of binarysearch
  
  for (n = 0; n < maxn; ++n)
  {
    // cout << "n = " << n << endl;
    cout << "\r" << "n = " << n;

    {
      // fill up distinct elements (plus one at top)
      // 0, 10, 20, ...
      for (i = 0; i <= n; ++i)
        x[i] = 10*i;

      for (i = 0; i < n; ++i)
      {
        ASSERT_EQ(s(10*i), i);
        ASSERT_EQ(s(10*i - 5), -1);

        // assert(s(10*i) == i);
        // assert(s(10*i - 5) == -1);
      }

      // search will use [0, n)
      ASSERT_EQ(s(10*n-5), -1);
      ASSERT_EQ(s(10*n), -1);
    }

    {
      // fill up equal elements
      // 10, 10, 10, ...
      for (i = 0; i < n; ++i)
        x[i] = 10;

      if (n == 0)
      {
        ASSERT_EQ(s(10), -1);
      } 
      else
      {
        assert(0 <= s(10) && s(10) < n);
      }

      ASSERT_EQ(s(5), -1);
      ASSERT_EQ(s(15), -1);
    }
  }
  cout << endl;
}


// algnum: 1, n: 1000, numtests: 10000
// algnum: 1, n: 1000, numtests: 10000, clicks: 608031, 60.8031
// algnum: 1, n: 10000, numtests: 1000
// algnum: 1, n: 10000, numtests: 1000, clicks: 776978, 77.6978
// algnum: 1, n: 10000, numtests: 100
// algnum: 1, n: 10000, numtests: 100, clicks: 79751, 79.751
// algnum: 1, n: 100000, numtests: 10
// algnum: 1, n: 10000, numtests: 10, clicks: 7871, 78.71

void timedriver()
{
  int i, algnum, numtests, test, start, clicks;

  while (cin >> algnum >> n >> numtests)
  {
    cout << "algnum: " << algnum << ", n: " << n << ", numtests: " << numtests << endl;

    for (i = 0; i < n; ++i)
      x[i] = i;

    start = clock();

    for (test = 0; test < numtests; ++test)
    {
      for (i = 0; i < n; ++i)
      {
        switch (algnum)
        {
          case 1: 
            assert(binarysearch1(i)==i); break;
        }
      }
    }

    clicks = clock() - start;
    cout << "algnum: " << algnum 
      << ", n: " << n
      << ", numtests: " << numtests
      << ", clicks: " << clicks
      << ", " << (1e9*clicks/((float)CLOCKS_PER_SEC*n*numtests)) << endl;
  }
}


TEST(Scafollding, Run)
{
  // test(200);

  timedriver();
}

// ={=========================================================================

int main(int argc, char** argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
