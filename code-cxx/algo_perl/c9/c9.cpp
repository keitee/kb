#include <iostream>
#include <cstring>
#include <vector>

#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp

using namespace std;


// ={=========================================================================

/* Copyright (C) 1999 Lucent Technologies
From 'Programming Pearls' by Jon Bentley

9.2 Problem Three, Sequential Search
*/

#define MAXN  100000

using DataType = int;

DataType x[MAXN];

// note:
// n is global which binarysearch uses and also tests uses. n is range that
// binarysearch works on. The good thing is that runs on various input easily.

int n = MAXN-1;

void fillx()
{
  int i;
  for (i = 0; i < n; ++i)
    x[i] = (int)rand() % MAXN;
}

int ssearch1(int t)
{
  for (int i = 0; i < n; ++i)
    if (x[i] == t)
      return i;

  return -1;
}

int ssearch2(int t)
{
  int i;
  // int hold = x[n];
  x[n] = t;

  for (i = 0; ; ++i)
    if (x[i] == t)
      break;

  // x[n] = hold;

  if (i == n)
    return -1;
  else
    return i;
}

int ssearch3(int t)
{
  int i;
  x[n] = t;

  for (i = 0; ; i += 8)
  {
    if (x[i] == t) {break;}
    if (x[i+1] == t) { i += 1; break;}
    if (x[i+2] == t) { i += 2; break;}
    if (x[i+3] == t) { i += 3; break;}
    if (x[i+4] == t) { i += 4; break;}
    if (x[i+5] == t) { i += 5; break;}
    if (x[i+6] == t) { i += 6; break;}
    if (x[i+7] == t) { i += 7; break;}
  } 

  if (i == n)
    return -1;
  else
    return i;
}


void timedriver(const std::vector<std::tuple<int, int, int>> &tests)
{
  int i, algnum, numtests, start, clicks;
  float result{};

  // to use the same input vector for all algos.
  fillx();

  // while (cin >> algnum >> n >> numtests)
  for (const auto & e: tests)
  {

    algnum    = get<0>(e);
    n         = get<1>(e);
    numtests  = get<2>(e);
    result    = 0;

    cout << "algnum: " << algnum << ", n: " << n << ", numtests: " << numtests << endl;

    start = clock();

    for (int test = 0; test < numtests; ++test)
    {
      for (i = 0; i < n; ++i)
      {
        switch (algnum)
        {
          case 1: 
            result = ssearch1(100);
            break;
          case 2: 
            result = ssearch2(100);
            break;
          case 3: 
            result = ssearch3(100);
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


// algnum: 1, n: 99999, numtests: 10
// algnum: 1, n: 99999, numtests: 10, result: -1, clicks: 217437429, 217.437, 0.00021744, 2.1744e-13
// algnum: 2, n: 99999, numtests: 10
// algnum: 2, n: 99999, numtests: 10, result: -1, clicks: 157730780, 157.731, 0.000157732, 1.57732e-13
// algnum: 3, n: 99999, numtests: 10
// algnum: 3, n: 99999, numtests: 10, result: -1, clicks: 56838075, 56.8381, 5.68386e-05, 5.68386e-14

TEST(c9, SequentialSearch)
{
  // tuple{algnum, n, numtests}
  std::vector<std::tuple<int, int, int>> tests{
    make_tuple(1, n, 10),
    make_tuple(2, n, 10),
    make_tuple(3, n, 10)
  };

  timedriver(tests);
}


// ={=========================================================================

int main(int argc, char** argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
