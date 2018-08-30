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

*algo-sort-heap* O(nlogn) for n

Heap has two properties; order and shape

root = 1
value(i) = x[i]
leftchild(i) = 2i
rightchild(i) = 2i + 1
parent(i) = i/2
null(i) = i < 1 or i > n (n is current size of heap)

*/

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

    // add it to the end
    //
    // void siftup(n)
    //   pre  : n > 0 && heap(1, n-1)
    //   post : heap(1, n)
    // 
    //   i = n;
    // 
    //   loop:
    //
    //     // invariant: heap(1, n) except perhaps between i and its parent
    //     // note: "perhaps" since they are i and parent when progressing
    //     // towards the root. 
    //
    //     // reaches the root 
    //     if i == 1
    //       break;
    //
    //     p = i / 2;
    // 
    //     // meets parent condition
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

// #define PROBLEM_FOR
#ifdef PROBLEMATIC_FOR
      p = i / 2;

      for (i = n; i > 1 && x[p] > x[i]; i = p)
      {
        p = i / 2;
        swap(x[p], x[i]);
      }
#else        
      // this is the original
      for (i = n; i > 1 && x[p = i/2] > x[i]; i = p)
      {
        swap(x[p], x[i]);
      }
#endif
    }


    // add it to the root
    //
    // void siftdown(n)
    //   pre  : heap(2, n) && n >= 0 (note on =)
    //   post : heap(1, n)
    // 
    //   i = 1;
    // 
    //   loop:
    //
    //   // invariant: heap(1, n) except perhaps between i 
    //   // and its(0,1, or 2) children
    //   
    //   c = 2 * i;
    // 
    //   if c > n
    //     break;  // no childrens and terminate.
    // 
    //   // c is the left child of i
    //   if c+1 <= n
    //     // c+1 is the right child of i, the right is the lesser
    //     if x[c+1] < x[c]
    //       c++;
    // 
    //   // c is the lesser child of i with is either the left or right
    //   if x[i] <= x[c]   // since it already meets parent condition so terminate.
    //     break;    
    // 
    //   swap(i, c);
    // 
    //   i = c;

    void siftdown()
    {
      int c{}, i{};

      i = 1;

#ifdef PROBLEMATIC_FOR
      c = 2 * i;

      for (i = 1; c <= n; i = c)
      {
        c = 2 * i;
#else        
      for (i = 1; (c = 2*i) <= n; i = c)
      {
#endif
        if (c+1 <= n && x[c+1] < x[c])
          c++;

        if (x[i] <= x[c])
          break;

        swap(x[c], x[i]);
      }
    }
};


template<typename T>
class priqueuemax {
  public:
    priqueuemax(int m)
    {
      cout << "use USE_MINE_MAX" << endl;

      maxsize = m;
      x = new T[maxsize+1];
      n = 0;  // size
    }

    ~priqueuemax()
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

    // add it to the end
    //
    // void siftup(n)
    //   pre  : n > 0 && heap(1, n-1)
    //   post : heap(1, n)
    // 
    //   i = n;
    // 
    //   loop:
    //
    //     // invariant: heap(1, n) except perhaps between i and its parent
    //     // note: "perhaps" since they are i and parent when progressing
    //     // towards the root. 
    //
    //     // reaches the root 
    //     if i == 1
    //       break;
    //
    //     p = i / 2;
    // 
    //     // meets parent condition for max
    //     // note: change op for max
    //     if x[p] >= x[i]
    //       break;
    // 
    //     swap(p, i);
    // 
    //     i = p;

    void siftup()
    {
      int i{}, p{};

      i = n;

      // this is the original
      for (i = n; i > 1 && x[p = i/2] < x[i]; i = p)
      {
        swap(x[p], x[i]);
      }
    }


    // add it to the root
    //
    // void siftdown(n)
    //   pre  : heap(2, n) && n >= 0 (note on =)
    //   post : heap(1, n)
    // 
    //   i = 1;
    // 
    //   loop:
    //
    //   // invariant: heap(1, n) except perhaps between i 
    //   // and its(0,1, or 2) children
    //   
    //   c = 2 * i;
    // 
    //   if c > n
    //     break;  // no childrens and terminate.
    // 
    //   // c is the left child of i
    //   if c+1 <= n
    //     // c+1 is the right child of i, the right is the bigger
    //     // note: change op for max
    //     if x[c+1] > x[c]
    //       c++;
    // 
    //   // c is the lesser child of i with is either the left or right
    //   // note: change op for max
    //   if x[i] >= x[c]   // since it already meets parent condition so terminate.
    //     break;    
    // 
    //   swap(i, c);
    // 
    //   i = c;
    //
    // note:
    // WHY it should be like below?
    //
    //     // c+1 is the right child of i, the right is the bigger
    //     // note: change op for max
    //     if x[c+1] > x[c]
    //

    void siftdown()
    {
      int c{}, i{};

      i = 1;

      for (i = 1; (c = 2*i) <= n; i = c)
      {
        if (c+1 <= n && x[c+1] > x[c])
          c++;

        if (x[i] >= x[c])
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


TEST(c14, PriQueueSort)
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

TEST(c14, PriQueueMaxSort)
{
  {
    vector<int> coll;

    int n = 10;
    priqueuemax<int> pq(n);

    for (int i = 0; i < n; ++i)
      pq.insert(i);

    for (int i = 0; i < n; ++i)
      coll.push_back(pq.extractmin());

    EXPECT_THAT(coll, ElementsAreArray({9,8,7,6,5,4,3,2,1,0}));
  }

  {
    vector<int> coll{4,5,6,7,8,9,1,2,3,0};
    vector<int> collo;

    int n = 10;
    priqueuemax<int> pq(n);

    for (int i = 0; i < n; ++i)
      pq.insert(coll[i]);

    for (int i = 0; i < n; ++i)
      collo.push_back(pq.extractmin());

    EXPECT_THAT(collo, ElementsAreArray({9,8,7,6,5,4,3,2,1,0}));
  }
}

namespace heap_sort {

  void siftup_min(int n, vector<int> &coll)
  {
    int i{}, p{};

    i = n;

    // this is the original
    for (i = n; i > 1 && coll[p = i/2] > coll[i]; i = p)
    {
      swap(coll[p], coll[i]);
    }
  }

  void siftdown_min(int n, vector<int> &coll)
  {
    int c{}, i{};

    i = 1;

    for (i = 1; (c = 2*i) <= n; i = c)
    {
      if (c+1 <= n && coll[c+1] < coll[c])
        c++;

      if (coll[i] <= coll[c])
        break;

      swap(coll[c], coll[i]);
    }
  }

  void siftup_max(int n, vector<int> &coll)
  {
    int i{}, p{};

    i = n;

    // this is the original
    for (i = n; i > 1 && coll[p = i/2] < coll[i]; i = p)
    {
      swap(coll[p], coll[i]);
    }
  }

  void siftdown_max(int n, vector<int> &coll)
  {
    int c{}, i{};

    i = 1;

    for (i = 1; (c = 2*i) <= n; i = c)
    {
      if (c+1 <= n && coll[c+1] > coll[c])
        c++;

      if (coll[i] >= coll[c])
        break;

      swap(coll[c], coll[i]);
    }
  }
}


/*
The simple one uses two arrays, one for the priority queue and one for the
element to be sorted.

Heapsort saves space by using just one. Two stage process:

1. build input into a heap
establish heap(1, n), by sifting elements to move up front in the array, the one
single array.

  | heap | ?  |
  1      i    n

[0] is not used and [1] so put from [2] into heap. 

2. the next n step extract the elements in `decreasing` order and build the
sorted

  | heap | sorted  |
  1      i         n

*/


void sort_heap_01(vector<int> &coll)
{
  using namespace heap_sort;

  // heap is one-based array; the easiest approach is to declare x[n+1] and
  // waste x[0].

  int n = coll.size()-1;

  for(int i = 2; i <= n; ++i)
  {
    // invariant: heap(1, i-1)
    siftup_min(i, coll);
    // invariant: heap(1, i)
  }

  // since [1] is the minimum, the sorted is {9,8,7,6,5,4,3,2,1} for example.
  for(int i = n; i >= 2; --i)
  {
    // heap(1, i) && sorted(i+1, n) && x[1..i] >= x[i+1..n]
    swap(coll[1], coll[i]);
    // heap(2, i-1) && sorted(i, n)
    siftdown_min(i-1, coll);
    // heap (1, i-1) && sorted(i, n)
  }
}

// max version
void sort_heap_02(vector<int> &coll)
{
  using namespace heap_sort;

  int n = coll.size()-1;

  for(int i = 2; i <= n; ++i)
    siftup_max(i, coll);

  for(int i = n; i >= 2; --i)
  {
    swap(coll[1], coll[i]);
    siftdown_max(i-1, coll);
  }
}

TEST(c14, HeapSort)
{
  {
    // add the dummpy at 0 to make heap
    vector<int> coll{0,4,5,6,7,8,9,1,2,3,0};

    sort_heap_01(coll);

    EXPECT_THAT(coll, ElementsAreArray({0,9,8,7,6,5,4,3,2,1,0}));
  }

  {
    // add the dummpy at 0 to make heap
    vector<int> coll{0,4,5,6,7,8,9,1,2,3,0};

    sort_heap_02(coll);

    EXPECT_THAT(coll, ElementsAreArray({0,0,1,2,3,4,5,6,7,8,9}));
  }

  {
    vector<int> coll{ 0, 33, 2, 31, 5, 30, 6, 12, 10, 13, 15, 17, 29, 3 };
    sort_heap_02(coll); 
    EXPECT_THAT(coll, 
        ElementsAreArray({0, 2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33 }));
  }
}


// ={=========================================================================

int main(int argc, char** argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
