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

        | 0 | 1 | 2 | 3 |

i = 0   j = 0   0           -> sum
        j = 1   0, 1        -> sum
        j = 2   0, 1, 2     -> sum
        j = 3   0, 1, 2, 3  -> sum

i = 1   j = 1   1           -> sum
        j = 2   1, 2        -> sum
        j = 3   1, 2, 3     -> sum
        ...

Depending on how to get sum:

1. use loop and cubic algorithm, algo1
2. use prefix sum, algo22
3. use the previous sum, algo21

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

  return maxsofar;
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

float algo21()
{
  int i{}, j{};
  float sum{}, maxsofar{0.0};

  for (i =0; i < n; ++i)
  {
    sum = 0.0;

    for (j = i; j < n; ++j)
    {
      // for (s = i; s <= j; ++s)
      //   sum += x[s];
      
      sum += x[j];

      if (maxsofar < sum)
        maxsofar = sum;
    }
  }

  return maxsofar;
}


float cumvec[MAXN+1];

float algo22()
{
  int i{}, j{};
  float sum{}, maxsofar{0.0};
  float *cumarr;

  // build prefix sum array
  
  cumarr = cumvec + 1;  // to access cumarr[-1]
  cumarr[-1] = 0;
  for (i = 0; i < n; ++i)
    cumarr[i] = cumarr[i-1] + x[i];


  for (i = 0; i < n; ++i)
  {
    sum = 0.0;

    for (j = i; j < n; ++j)
    {
      // for (s = i; s <= j; ++s)
      //   sum += x[s];
      
      sum = cumarr[j] - cumarr[i-1];

      if (maxsofar < sum)
        maxsofar = sum;
    }
  }

  return maxsofar;
}


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

#ifdef ALGO3_DEBUG
  cout << "recmax(" << l << ", " << u << ")" << endl;
#endif

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

#ifdef ALGO3_DEBUG
  cout << "l: " << l << ", u: " << u << ", m: " << m  << ", lmax: " << lmax << endl;
#endif

  // find max crossing to right, sum[m+1, u] and minimum is 0.
  rmax = sum = 0;

  for (i = m + 1; i <= u; ++i)
  {
    sum += x[i];
    rmax = max(sum, rmax);
  }

#ifdef ALGO3_DEBUG
  cout << "l: " << l << ", u: " << u << ", m: " << m  << ", rmax: " << rmax << endl;
#endif

  //
  auto uppermax = recmax(m+1, u);
  auto lowmax = recmax(l, m);

#ifdef ALGO3_DEBUG
  cout << "l: " << l << ", u: " << u << ", m: " << m 
    << ", lmax: " << lmax << ", rmax: " << rmax << ", lowmax: " << lowmax << ", uppermax: " << uppermax << endl;

  cout << "l: " << l << ", u: " << u << ", m: " << m 
    << ", return: " << max(lmax + rmax, max(lowmax, uppermax)) << endl;
#endif

  return max(lmax + rmax, max(lowmax, uppermax));

  // return max(lmax + rmax, max(recmax(l, m), recmax(m+1, u)));
}


float algo3()
{
  return recmax(0, n-1);
}


// use inline 
// the key point is that when it goes negative, it is reset to zero and means
// that i is now the empty vector. That is when sum is zero, discard elements to
// i as if they are not there. Tried to find input array that proves this wrong
// but not found.
//
// How about when the first, 2, is out of sum range?
//
// | 2 | 1 | -5 | 3 | 2 | 1 |
//     <------------------->
//     <--------------->
//     ...
//              <----------> this is max from all combinations.

float algo4()
{
  int i{};

  // sum is maxendingshere in the book
  float sum{0.0}, maxsofar{0.0};

  for (i =0; i < n; ++i)
  {
    sum += x[i];

    sum = std::max(sum, (float)0);
    maxsofar = std::max(maxsofar, sum);
  }

  return maxsofar;
}

// use if
float algo42()
{
  int i{};

  // sum is maxendingshere in the book
  float sum{0.0}, maxsofar{0.0};

  for (i =0; i < n; ++i)
  {
    sum += x[i];

    if (sum < 0)
      sum = 0;
    if (maxsofar < sum)
      maxsofar = sum;
  }

  return maxsofar;
}

// use macro
#define maxmac(a, b) ((a) > (b) ? (a) : (b))

float algo43()
{
  int i{};

  // sum is maxendingshere in the book
  float sum{0.0}, maxsofar{0.0};

  for (i =0; i < n; ++i)
  {
    sum += x[i];

    sum = maxmac(sum, (float)0);
    maxsofar = maxmac(maxsofar, sum);
  }

  return maxsofar;
}

// use function
float maxfunc(float a, float b)
{
  return a > b ? a : b;
}

float algo44()
{
  int i{};

  // sum is maxendingshere in the book
  float sum{0.0}, maxsofar{0.0};

  for (i =0; i < n; ++i)
  {
    sum += x[i];

    sum = maxfunc(sum, (float)0);
    maxsofar = maxfunc(maxsofar, sum);
  }

  return maxsofar;
}


// algnum: 1, n: 100, numtests: 10
// algnum: 1, n: 100, numtests: 10, result: 4.50922, clicks: 480934, 480934
// algnum: 2, n: 100, numtests: 10
// algnum: 2, n: 100, numtests: 10, result: 4.50922, clicks: 16614, 16614
// algnum: 3, n: 100, numtests: 10
// algnum: 3, n: 100, numtests: 10, result: 4.50922, clicks: 13468, 13468
// algnum: 4, n: 100, numtests: 10
// algnum: 4, n: 100, numtests: 10, result: 4.50922, clicks: 6396, 6396
//
// algo 4, use inline
// algnum: 5, n: 100, numtests: 10
// algnum: 5, n: 100, numtests: 10, result: 4.50922, clicks: 1048, 1048
//
// algo42,  use if
// algnum: 6, n: 100, numtests: 10
// algnum: 6, n: 100, numtests: 10, result: 4.50922, clicks: 468, 468
//
// algo43, use macro
// algnum: 7, n: 100, numtests: 10
// algnum: 7, n: 100, numtests: 10, result: 4.50922, clicks: 376, 376
//
// algo44, use function
// algnum: 8, n: 100, numtests: 10
// algnum: 8, n: 100, numtests: 10, result: 4.50922, clicks: 1594, 1594

// when n=1000
// algnum: 1, n: 1000, numtests: 10
// algnum: 1, n: 1000, numtests: 10, result: 15.6175, clicks: 819738644, 8.19739e+07
// algnum: 2, n: 1000, numtests: 10
// algnum: 2, n: 1000, numtests: 10, result: 15.6175, clicks: 17779096, 1.77791e+06
// algnum: 3, n: 1000, numtests: 10
// algnum: 3, n: 1000, numtests: 10, result: 15.6175, clicks: 21752900, 2.17529e+06


void timedriver(const std::vector<std::tuple<int, int, int>> &tests)
{
  int i, algnum, numtests, start, clicks;
  float result{};

  // to use the same input vector for all algos.
  n = 100;
  sprinkle();

  // while (cin >> algnum >> n >> numtests)
  for (const auto & e: tests)
  {
    algnum    = get<0>(e);
    // n         = get<1>(e);
    n         = 100;
    numtests  = get<2>(e);
    result    = 0;

    cout << "algnum: " << algnum << ", n: " << n << ", numtests: " << numtests << endl;

    // sprinkle();

    start = clock();

    for (int test = 0; test < numtests; ++test)
    {
      for (i = 0; i < n; ++i)
      {
        switch (algnum)
        {
          case 1: 
            result = algo1();
            break;

          case 2: 
            result = algo21();
            break;

          case 3: 
            result = algo22();
            break;

          case 4: 
            result = algo3();
            break;

          case 5: 
            result = algo4();
            break;

          case 6: 
            result = algo42();
            break;

          case 7: 
            result = algo43();
            break;

          case 8: 
            result = algo44();
            break;
        }
      }
    }

    clicks = clock() - start;
    cout << "algnum: " << algnum 
      << ", n: " << n
      << ", numtests: " << numtests
      << ", result: " << result
      << ", clicks: " << clicks
      // total
      << ", " << (clicks/((float)CLOCKS_PER_SEC))
      // average
      << ", " << (clicks/((float)CLOCKS_PER_SEC*n*numtests))
      // average in ns
      << ", " << (clicks/(1e9*(float)CLOCKS_PER_SEC*n*numtests)) << endl;
  }
}


TEST(C8, MaxSubVector)
{
  // tuple{algnum, n, numtests}
  std::vector<std::tuple<int, int, int>> tests{
    make_tuple(1, 20, 10),
    make_tuple(2, 20, 10),
    make_tuple(3, 20, 10),
    make_tuple(4, 20, 10),
    make_tuple(5, 20, 10),
    make_tuple(6, 20, 10),
    make_tuple(7, 20, 10),
    make_tuple(8, 20, 10)
  };

  timedriver(tests);
}


// ={=========================================================================

int main(int argc, char** argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
