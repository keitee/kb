#include <iostream>
#include <set>
#include <vector>

#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;


// ={=========================================================================

/* Copyright (C) 1999 Lucent Technologies
From 'Programming Pearls' by Jon Bentley

14 Heap
*/

// #define MAXN  100000

// using DataType = int;

// DataType x[MAXN];

// // note:
// // n is global which binarysearch uses and also tests uses. n is range that
// // binarysearch works on. The good thing is that runs on various input easily.

// int n = MAXN-1;

#define USE_MINE

#ifdef USE_MINE

template<typename T>
class priqueue {
  public:
    priqueue(int m)
    {
      cout << "use USE_MINE" << endl;

      maxsize = m;
      x = new T[maxsize+1];
      n = 0;  // size
    }

    ~priqueue()
    {
      if (x)
        delete[] x;
    }

    void insert(T t)
    {
      // if (n >= maxsize)
      //  report error

      x[++n] = t;

      // heap(1, n-1)
      siftup();
      // heap(1, n)
    }

    // void pop()
    T extractmin()
    {
      T t{};

      // if (n < 1)
      //  report error

      t = x[1];
      x[1] = x[n--];

      // heap(2, n)
      siftdown();
      // heap(1, n)

      return t;
    }


  private:
    int n;
    int maxsize;
    T *x;

    // void siftup(n)
    //   pre n > 0 && heap(1, n-1)
    //   post heap(1, n)
    // 
    //   i = n;
    // 
    //   loop
    //     // invariant: heap(1, n) except perhaps between i and its parent
    //     if i == 1
    //       break;
    //
    //     p = i / 2;
    // 
    //     if x[p] <= x[i]
    //       break;
    // 
    //     swap(p, i);
    // 
    //     i = p;

    void siftup()
    {
      int i{}, p{};

      i = n;

      // p = i / 2;

      for (i = n; i > 1 && x[p = i/2] > x[i]; i = p)
      {
        // p = i / 2;

        swap(x[p], x[i]);
      }
    }

    // void siftdown(n)
    //   pre heap(2, n) && n >= 0
    //   post heap(1, n)
    // 
    //   i = 1;
    // 
    //   loop
    //   // invariant: heap(1, n) except perhaps between i and its(0,1, or 2) children
    //   
    //   c = 2 * i;
    // 
    //   if c > n
    //     break;  // no childrens and terminate.
    // 
    //   // c is the left child of i
    //   if c+1 <= n
    //     // c+1 is the right child of i
    //     if x[c+1] < x[c]
    //       c++;
    // 
    //   // c is the lesser child of i
    //   if x[i] <= x[c]   // since it already meets heap condition so terminate.
    //     break;    
    // 
    //   swap(i, c);
    // 
    //   i = c;

    void siftdown()
    {
      int c{}, i{};

      i = 1;

      // c = 2 * i;

      for (i = 1; (c = 2*i) <= n; i = c)
      {
        // c = 2 * i;

        if (c+1 <= n && x[c+1] < x[c])
          c++;

        if (x[i] <= x[c])
          break;

        swap(x[c], x[i]);
      }
    }
};

#else // USE_MINE

template<class T>
class priqueue {

  private:
    int n, maxsize;
    T *x;
    void swap(int i, int j)
    { T t = x[i]; x[i] = x[j]; x[j] = t; }

  public:
    priqueue(int m)
    { 
      maxsize = m;
      x = new T[maxsize+1];
      n = 0;
    }
    void insert(T t)
    { 
      int i, p;
      x[++n] = t;
      for (i = n; i > 1 && x[p=i/2] > x[i]; i = p)
        swap(p, i);
    }
    T extractmin()
    { 
      int i, c;
      T t = x[1];
      x[1] = x[n--];
      for (i = 1; (c=2*i) <= n; i = c) {
        if (c+1<=n && x[c+1]<x[c])
          c++;
        if (x[i] <= x[c])
          break;
        swap(c, i);
      }
      return t;
    }
};

#endif // USE_MINE


// ={=========================================================================

// void timedriver(const std::vector<std::tuple<int, int, int>> &tests)
// {
//   int i, algnum, numtests, start, clicks;
//   float result{};
// 
//   // while (cin >> algnum >> n >> numtests)
//   for (const auto & e: tests)
//   {
// 
//     algnum    = get<0>(e);
//     n         = get<1>(e);
//     numtests  = get<2>(e);
//     result    = 0;
// 
//     cout << "algnum: " << algnum << ", n: " << n << ", numtests: " << numtests << endl;
// 
//     start = clock();
// 
//     for (int test = 0; test < numtests; ++test)
//     {
//       for (i = 0; i < n; ++i)
//       {
//         switch (algnum)
//         {
//           case 1: 
//             run_set_stl(100, 100);
//             break;
//           case 2: 
//             break;
//           case 3: 
//             break;
//         }
//       }
//     }
// 
//     clicks = clock() - start;
//     cout << "algnum: " << algnum 
//       << ", n: " << n
//       << ", numtests: " << numtests
//       << ", result: " << result
//       << ", clicks: " << clicks
//       // total
//       << ", " << (clicks/((float)CLOCKS_PER_SEC))
//       // average
//       << ", " << (clicks/((float)CLOCKS_PER_SEC*n*numtests))
//       // average in ns
//       << ", " << (clicks/(1e9*(float)CLOCKS_PER_SEC*n*numtests)) << endl;
//   }
// }


TEST(c14, PriQueue)
{
  {
    vector<int> coll;

    int n = 10;
    priqueue<int> pq(n);

    for (int i = 0; i < n; ++i)
      pq.insert(i);

    for (int i = 0; i < n; ++i)
      coll.push_back(pq.extractmin());

    EXPECT_THAT(coll, ElementsAreArray({0,1,2,3,4,5,6,7,8,9}));
  }

  {
    vector<int> coll{4,5,6,7,8,9,1,2,3,0};
    vector<int> collo;

    int n = 10;
    priqueue<int> pq(n);

    for (int i = 0; i < n; ++i)
      pq.insert(coll[i]);

    for (int i = 0; i < n; ++i)
      collo.push_back(pq.extractmin());

    EXPECT_THAT(collo, ElementsAreArray({0,1,2,3,4,5,6,7,8,9}));
  }
}


// ={=========================================================================

int main(int argc, char** argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
