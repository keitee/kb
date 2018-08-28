#include <iostream>
#include <set>
#include <vector>

#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp

using namespace std;


// ={=========================================================================

/* Copyright (C) 1999 Lucent Technologies
From 'Programming Pearls' by Jon Bentley

13 Searching. Appendix 5
*/

#define MAXN  100000

using DataType = int;

DataType x[MAXN];

// note:
// n is global which binarysearch uses and also tests uses. n is range that
// binarysearch works on. The good thing is that runs on various input easily.

int n = MAXN-1;

// set set version

class IntSetStl
{
  private:
    set<int> S;
  public:
    IntSetStl(int maxelms, int maxval) { (void)maxelms; (void)maxval; }
    int size() { return S.size(); }
    void insert(int t) { S.insert(t); }
    void report(int *v)
    {
      int i{0}, n{};
      for (auto it = S.begin(); it != S.end(); ++it)
      {
        v[i++] = *it;
        ++n;
      }
      cout << setw(20) << "IntSetStl: n : " << n << endl;
    }
};


void run_set_stl(int m, int maxval)
{
  int *v = new int[m];
  IntSetStl S(m, maxval);
  while (S.size() < m)
    S.insert(rand() % maxval);

  S.report(v);
}


// set array version

class IntSetArray
{
  private:
    int n, *x;

  public:
    IntSetArray(int maxelms, int maxval)
    {
      x = new int[1 + maxelms];
      n = 0;
      x[0] = maxval;
    }
    
    int size() { return n; }

    void insert(int t)
    {
      int i{}, j{};
      //
      for (i = 0; x[i] < t; ++i)
        ;

      for (j = n; j >= i; --j)
        x[j+1] = x[j];

      x[i] = t;
      ++n;
    }

    void report(int *v)
    {
      for (int i = 0; i < n; ++i)
      {
        v[i] = x[i];
      }
      cout << setw(20) << "IntSetArray: n : " << n << endl;
    }
};

void run_set_array(int m, int maxval)
{
  int *v = new int[m];
  IntSetArray S(m, maxval);
  while (S.size() < m)
    S.insert(rand() % maxval);

  S.report(v);
}


// ={=========================================================================

void timedriver(const std::vector<std::tuple<int, int, int>> &tests)
{
  int i, algnum, numtests, start, clicks;
  float result{};

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
            run_set_stl(100, 100);
            break;
          case 2: 
            break;
          case 3: 
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

TEST(c13, StlSet)
{
  // // tuple{algnum, n, numtests}
  // std::vector<std::tuple<int, int, int>> tests{
  //   make_tuple(1, n, 10),
  //   make_tuple(2, n, 10),
  //   make_tuple(3, n, 10)
  // };

  // timedriver(tests);

  // #define MAXN  100000
  run_set_stl(MAXN, MAXN*10);
}

TEST(c13, Array)
{
  run_set_array(MAXN, MAXN*10);
}


// ={=========================================================================

int main(int argc, char** argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
