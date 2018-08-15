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

/* Copyright (C) 1999 Lucent Technologies
From 'Programming Pearls' by Jon Bentley

maxsum.c -- time algs for maximum-sum subsequence
Input: algnum, n
Output: algnum, n, answer, ticks, secs
See main for algnum codes
*/

#define MAXN  10000

using DataType = float;

DataType x[MAXN];

// note:
// n is global which binarysearch uses and also tests uses. n is range that
// binarysearch works on. The good thing is that runs on various input easily.

int n;

// fill x[n] with reals uniform on [-1, 1]
// ((float)rand()/RAND_MAX) ranges on [0, 1] so "1 - 2*((float)rand()/RAND_MAX)"
// ranges on [-1, 1].

void sprinkle()
{
  int i;
  for (i = 0; i < n; ++i)
    x[i] = 1 - 2*((float)rand()/RAND_MAX);
}

// algo3: from PP, Column 8, p80
//
// n : 20
//  0, -0.680375
//   , 0.211234
//   , -0.566198
//   , -0.59688
//   , -0.823295
//   , 0.604897
//   , 0.329554
//   , -0.536459
//   , 0.444451
//   , -0.10794
//   , 0.0452059
//   , -0.257742
//   , 0.270431
//   , -0.0268018
//   , -0.904459
// 15, -0.83239
// 16, -0.271423
// 17, -0.434594
// 18, 0.716795
// 19, -0.213938

// recmax(0, 19)
// recmax(10, 19)
// recmax(15, 19)
// recmax(18, 19)
// recmax(19, 19)
// recmax(18, 18)
// l: 18, u: 19, m: 18, lmax: 0.716795, rmax: 0, lowmax: 0.716795, uppermax: 0
// recmax(15, 17)
// recmax(17, 17)
// recmax(15, 16)
// recmax(16, 16)
// recmax(15, 15)
// l: 15, u: 16, m: 15, lmax: 0, rmax: 0, lowmax: 0, uppermax: 0
// l: 15, u: 17, m: 16, lmax: 0, rmax: 0, lowmax: 0, uppermax: 0
// l: 15, u: 19, m: 17, lmax: 0, rmax: 0.716795, lowmax: 0, uppermax: 0.716795
// recmax(10, 14)
// recmax(13, 14)
// recmax(14, 14)
// recmax(13, 13)
// l: 13, u: 14, m: 13, lmax: 0, rmax: 0, lowmax: 0, uppermax: 0
// recmax(10, 12)
// recmax(12, 12)
// recmax(10, 11)
// recmax(11, 11)
// recmax(10, 10)
// l: 10, u: 11, m: 10, lmax: 0.0452059, rmax: 0, lowmax: 0.0452059, uppermax: 0
// l: 10, u: 12, m: 11, lmax: 0, rmax: 0.270431, lowmax: 0.0452059, uppermax: 0.270431
// l: 10, u: 14, m: 12, lmax: 0.270431, rmax: 0, lowmax: 0.270431, uppermax: 0
// l: 10, u: 19, m: 14, lmax: 0, rmax: 0, lowmax: 0.270431, uppermax: 0.716795
// recmax(0, 9)
// recmax(5, 9)
// recmax(8, 9)
// recmax(9, 9)
// recmax(8, 8)
// l: 8, u: 9, m: 8, lmax: 0.444451, rmax: 0, lowmax: 0.444451, uppermax: 0
// recmax(5, 7)
// recmax(7, 7)
// recmax(5, 6)
// recmax(6, 6)
// recmax(5, 5)
// l: 5, u: 6, m: 5, lmax: 0.604897, rmax: 0.329554, lowmax: 0.604897, uppermax: 0.329554
// l: 5, u: 7, m: 6, lmax: 0.934452, rmax: 0, lowmax: 0.934452, uppermax: 0
// l: 5, u: 9, m: 7, lmax: 0.397993, rmax: 0.444451, lowmax: 0.934452, uppermax: 0.444451
// recmax(0, 4)
// recmax(3, 4)
// recmax(4, 4)
// recmax(3, 3)
// l: 3, u: 4, m: 3, lmax: 0, rmax: 0, lowmax: 0, uppermax: 0
// recmax(0, 2)
// recmax(2, 2)
// recmax(0, 1)
// recmax(1, 1)
// recmax(0, 0)
// l: 0, u: 1, m: 0, lmax: 0, rmax: 0.211234, lowmax: 0, uppermax: 0.211234
// l: 0, u: 2, m: 1, lmax: 0.211234, rmax: 0, lowmax: 0.211234, uppermax: 0
// l: 0, u: 4, m: 2, lmax: 0, rmax: 0, lowmax: 0.211234, uppermax: 0
// l: 0, u: 9, m: 4, lmax: 0, rmax: 0.934452, lowmax: 0.211234, uppermax: 0.934452
// l: 0, u: 19, m: 9, lmax: 0.734503, rmax: 0.0578951, lowmax: 0.934452, uppermax: 0.716795
// result: 0.934452


float recmax(int l, int u)
{
  int i, m;
  float lmax, rmax, sum;

  cout << "recmax(" << l << ", " << u << ")" << endl;

  // handle small vectors:

  // l(low), u(upper)
  // the max of a zero-element vector is defined to be zero.
  if (l > u)
    return 0;

  // the max of a one-element vector is the only value in the vector 
  // (or zero if that number is negative) so negatives are filtered out.
  
  if (l == u)
    return max((float)0, x[l]);

  m = (l + u)/2;

  // find max crossing to left
  lmax = sum = 0;

  for (i = m; l <= i; --i)
  {
    sum += x[i];

    // if (sum > lmax)
    //   lmax = sum;

    lmax = max(sum, lmax);
  }

  // find max crossing to right
  rmax = sum = 0;

  for (i = m + 1; i <= u; ++i)
  {
    sum += x[i];
    rmax = max(sum, rmax);
  }

  //
  auto uppermax = recmax(m+1, u);
  auto lowmax = recmax(l, m);

  cout << "l: " << l << ", u: " << u << ", m: " << m 
    << ", lmax: " << lmax << ", rmax: " << rmax << ", lowmax: " << lowmax << ", uppermax: " << uppermax << endl;

  return max(lmax + rmax, max(lowmax, uppermax));

  // return max(lmax + rmax, max(recmax(l, m), recmax(m+1, u)));
}


float algo3()
{
  return recmax(0, n-1);
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
            // assert(binarysearch1(i)==i); break;
            break;
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


TEST(MaxSubArray, Run)
{
  cin >> n;
  cout << "n : " << n << endl;

  sprinkle();

  int i;
  for (i = 0; i < n; ++i)
    cout << ", " << x[i] << endl;

  auto result = recmax(0, n-1);

  cout << "result: " << result << endl;
}


// ={=========================================================================

int main(int argc, char** argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
