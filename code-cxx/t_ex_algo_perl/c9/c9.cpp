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

8.1
The problem arose in one-dimensional pattern recognition.

The input is a vector x of n floating-point numbers; the output is the maximum
sum found in any `contiguous subvector` of the input.

The problem is easy when all the numbers are positive; the maximum subvector is
the entire input vector. The difficulty comes when some of the numbers are
negative. To complete the problem definition, will say that when all inputs are
negative the maximum-sum subvector is the empty vector, which has sum zero.
*/

#define MAXN  10000

using DataType = float;

DataType x[MAXN];

// note:
// n is global which binarysearch uses and also tests uses. n is range that
// binarysearch works on. The good thing is that runs on various input easily.

int n;

/*
algo1: O(n3) and cubic algorithm.
iterate over all pairs of integers i and j satisfying 0 <= i <= j < n.

note: when sum of subvector is negative, sum should be zero. covered? yes since
maxsofar's initial value is 0 and do the same as max(sum, 0);

algo1()
{
  maxsofar = 0;

  for i = [0, n)
    for j = [i, n)
    {
      sum = 0;

      for s = [i, j]
        sum += x[s];

      // sum is sum of x[i..j]

      maxsofar = max(sum, maxsofar);
    }
}

*/

float algo1()
{
  int i{}, j{}, s{};
  float sum{}, maxsofar{0.0};

  for (i =0; i < n; ++i)
  {
    for (j = i; j < n; ++j)
    {
      sum = 0.0;
      for (s = i; s <= j; ++s)
        sum += x[s];

      if (maxsofar < sum)
        maxsofar = sum;
    }
  }
}


/*
The both removes the third inner loop with constant sum operation.

algo2-1: O(n2) and quadratic algorithm. 
use observation that the sum of x[i..j] is related to the sum previously
computed that of x[i..j-1].

algo21()
{
  maxsofar = 0;

  for i = [0, n)
    sum = 0;
    for j = [i, n)
      sum += x[j];

      // sum is sum of x[i..j]

      maxsofar = max(maxsofar, sum);
}


algo2-2: O(n2) and quadratic algorithm. 
use prefix-sum

algo22()
{
  maxsofar = 0;

  // build prefix sum array
  // ith element of cumarr contains the cumulative sum of x[0..i] and which is
  // different from *algo-prefix-sum* which is sum of x[0..i-1].

  cummarr[-1] = 0;
  for i = [0, n)
    cumarr[i] = cumarr[i-1] + x[i];

  for i = [0, n)
    for j = [i, n)
    {
      sum = 0;

      sum = cumarr[j] - cumarr[i-1];

      // for s = [i, j]
      //   sum += x[s];

      // sum is sum of x[i..j]

      maxsofar = max(sum, maxsofar);
    }
}
*/


/*

maxsum.c -- time algs for maximum-sum subsequence
Input: algnum, n
Output: algnum, n, answer, ticks, secs
See main for algnum codes
*/

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
// 00, -0.680375
// 01, 0.211234
// 02, -0.566198
// 03, -0.59688
// 04, -0.823295
// 05, 0.604897
// 06, 0.329554
// 07, -0.536459
// 08, 0.444451
// 09, -0.10794
// 10, 0.0452059
// 11, -0.257742
// 12, 0.270431
// 13, -0.0268018
// 14, -0.904459
// 15, -0.83239
// 16, -0.271423
// 17, -0.434594
// 18, 0.716795
// 19, -0.213938


// recmax(0, 19)
// l: 0, u: 19, m: 9, lmax: 0.734503
// l: 0, u: 19, m: 9, rmax: 0.0578951
// recmax(10, 19)
// l: 10, u: 19, m: 14, lmax: 0
// l: 10, u: 19, m: 14, rmax: 0
//
// recmax(15, 19)
// l: 15, u: 19, m: 17, lmax: 0
// l: 15, u: 19, m: 17, rmax: 0.716795
//
// recmax(18, 19)
// l: 18, u: 19, m: 18, lmax: 0.716795
// l: 18, u: 19, m: 18, rmax: 0
//
// recmax(19, 19)   // negative so return 0
// recmax(18, 18)
//
// lmax is sum of left side that calcs itself and lowmax is sum of recmax(low
// part). return max(rmax+lmax, lowermax, uppermax)
//
// l: 18, u: 19, m: 18, lmax: 0.716795, rmax: 0, lowmax: 0.716795, uppermax: 0
// l: 18, u: 19, m: 18, return: 0.716795
//
// recmax(15, 17)
// l: 15, u: 17, m: 16, lmax: 0
// l: 15, u: 17, m: 16, rmax: 0
// recmax(17, 17)   // negative
// recmax(15, 16)
// l: 15, u: 16, m: 15, lmax: 0
// l: 15, u: 16, m: 15, rmax: 0
// recmax(16, 16)   // negative
// recmax(15, 15)   // negative
//
// l: 15, u: 16, m: 15, lmax: 0, rmax: 0, lowmax: 0, uppermax: 0
// l: 15, u: 16, m: 15, return: 0
// l: 15, u: 17, m: 16, lmax: 0, rmax: 0, lowmax: 0, uppermax: 0
// l: 15, u: 17, m: 16, return: 0
// l: 15, u: 19, m: 17, lmax: 0, rmax: 0.716795, lowmax: 0, uppermax: 0.716795
// l: 15, u: 19, m: 17, return: 0.716795
//
// recmax(10, 14)
// l: 10, u: 14, m: 12, lmax: 0.270431
// l: 10, u: 14, m: 12, rmax: 0
// recmax(13, 14)
// l: 13, u: 14, m: 13, lmax: 0
// l: 13, u: 14, m: 13, rmax: 0
// recmax(14, 14)
// recmax(13, 13)
// l: 13, u: 14, m: 13, lmax: 0, rmax: 0, lowmax: 0, uppermax: 0
// l: 13, u: 14, m: 13, return: 0
//
// recmax(10, 12)
// l: 10, u: 12, m: 11, lmax: 0
// l: 10, u: 12, m: 11, rmax: 0.270431
// recmax(12, 12)
// recmax(10, 11)
// l: 10, u: 11, m: 10, lmax: 0.0452059
// l: 10, u: 11, m: 10, rmax: 0
// recmax(11, 11)
// recmax(10, 10)
//
// l: 10, u: 11, m: 10, lmax: 0.0452059, rmax: 0, lowmax: 0.0452059, uppermax: 0
// l: 10, u: 11, m: 10, return: 0.0452059
//
// (see that lmax and lowmax becomes different since lmax do sum on range and
// lowmax discards negatives.
// l: 10, u: 12, m: 11, lmax: 0, rmax: 0.270431, lowmax: 0.0452059, uppermax: 0.270431
// l: 10, u: 12, m: 11, return: 0.270431
// l: 10, u: 14, m: 12, lmax: 0.270431, rmax: 0, lowmax: 0.270431, uppermax: 0
// l: 10, u: 14, m: 12, return: 0.270431
//
// l: 10, u: 19, m: 14, lmax: 0, rmax: 0, lowmax: 0.270431, uppermax: 0.716795
// l: 10, u: 19, m: 14, return: 0.716795
//
//
// recmax(0, 9)
// l: 0, u: 9, m: 4, lmax: 0
// l: 0, u: 9, m: 4, rmax: 0.934452
// recmax(5, 9)
// l: 5, u: 9, m: 7, lmax: 0.397993
// l: 5, u: 9, m: 7, rmax: 0.444451
// recmax(8, 9)
// l: 8, u: 9, m: 8, lmax: 0.444451
// l: 8, u: 9, m: 8, rmax: 0
// recmax(9, 9)
// recmax(8, 8)
// l: 8, u: 9, m: 8, lmax: 0.444451, rmax: 0, lowmax: 0.444451, uppermax: 0
// l: 8, u: 9, m: 8, return: 0.444451
//
// recmax(5, 7)
// l: 5, u: 7, m: 6, lmax: 0.934452
// l: 5, u: 7, m: 6, rmax: 0
// recmax(7, 7)
// recmax(5, 6)
// l: 5, u: 6, m: 5, lmax: 0.604897
// l: 5, u: 6, m: 5, rmax: 0.329554
// recmax(6, 6)
// recmax(5, 5)
//
// (see that return lmax+rmax which is bigger)
// l: 5, u: 6, m: 5, lmax: 0.604897, rmax: 0.329554, lowmax: 0.604897, uppermax: 0.329554
// l: 5, u: 6, m: 5, return: 0.934452
//
// l: 5, u: 7, m: 6, lmax: 0.934452, rmax: 0, lowmax: 0.934452, uppermax: 0
// l: 5, u: 7, m: 6, return: 0.934452
//
// (see that lowmax is bigger than lmax)
// l: 5, u: 9, m: 7, lmax: 0.397993, rmax: 0.444451, lowmax: 0.934452, uppermax: 0.444451
// l: 5, u: 9, m: 7, return: 0.934452
//
// recmax(0, 4)
// l: 0, u: 4, m: 2, lmax: 0
// l: 0, u: 4, m: 2, rmax: 0
// recmax(3, 4)
// l: 3, u: 4, m: 3, lmax: 0
// l: 3, u: 4, m: 3, rmax: 0
// recmax(4, 4)
// recmax(3, 3)
// l: 3, u: 4, m: 3, lmax: 0, rmax: 0, lowmax: 0, uppermax: 0
// l: 3, u: 4, m: 3, return: 0
// recmax(0, 2)
// l: 0, u: 2, m: 1, lmax: 0.211234
// l: 0, u: 2, m: 1, rmax: 0
// recmax(2, 2)
// recmax(0, 1)
// l: 0, u: 1, m: 0, lmax: 0
// l: 0, u: 1, m: 0, rmax: 0.211234
// recmax(1, 1)
// recmax(0, 0)
// l: 0, u: 1, m: 0, lmax: 0, rmax: 0.211234, lowmax: 0, uppermax: 0.211234
// l: 0, u: 1, m: 0, return: 0.211234
// l: 0, u: 2, m: 1, lmax: 0.211234, rmax: 0, lowmax: 0.211234, uppermax: 0
// l: 0, u: 2, m: 1, return: 0.211234
// l: 0, u: 4, m: 2, lmax: 0, rmax: 0, lowmax: 0.211234, uppermax: 0
// l: 0, u: 4, m: 2, return: 0.211234
// l: 0, u: 9, m: 4, lmax: 0, rmax: 0.934452, lowmax: 0.211234, uppermax: 0.934452
// l: 0, u: 9, m: 4, return: 0.934452
//
// l: 0, u: 19, m: 9, lmax: 0.734503, rmax: 0.0578951, lowmax: 0.934452, uppermax: 0.716795
// l: 0, u: 19, m: 9, return: 0.934452
//
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

  // find max crossing to left, sum[l, m] and minimum is 0.
  lmax = sum = 0;

  for (i = m; l <= i; --i)
  {
    sum += x[i];

    // if (sum > lmax)
    //   lmax = sum;

    lmax = max(sum, lmax);
  }
  cout << "l: " << l << ", u: " << u << ", m: " << m  << ", lmax: " << lmax << endl;

  // find max crossing to right, sum[m+1, u] and minimum is 0.
  rmax = sum = 0;

  for (i = m + 1; i <= u; ++i)
  {
    sum += x[i];
    rmax = max(sum, rmax);
  }
  cout << "l: " << l << ", u: " << u << ", m: " << m  << ", rmax: " << rmax << endl;

  //
  auto uppermax = recmax(m+1, u);
  auto lowmax = recmax(l, m);

  cout << "l: " << l << ", u: " << u << ", m: " << m 
    << ", lmax: " << lmax << ", rmax: " << rmax << ", lowmax: " << lowmax << ", uppermax: " << uppermax << endl;

  cout << "l: " << l << ", u: " << u << ", m: " << m 
    << ", return: " << max(lmax + rmax, max(lowmax, uppermax)) << endl;

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
