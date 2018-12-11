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


#define SORT_QUICK_DEBUG

int build_partition(vector<int> &coll, int first, int last)
{
  int pivot_pos = (first + last) / 2;
  int pivot_value = coll[pivot_pos];

#ifdef SORT_QUICK_DEBUG
    cout << "pivot_pos(" << pivot_pos << ") build before(" << first << ", " << last << "): ";

    for (int i = first; i <= last; ++i)
      cout << coll[i] << ", ";

    cout << endl;
#endif // SORT_QUICK_DEBUG

  // move the pivot to the first pos
  swap(coll[first], coll[pivot_pos]);

  int last_small = first;

  for (int inspect = first + 1; inspect <= last; ++inspect)
  {
    if (coll[inspect] < pivot_value)
    {
      ++last_small;

      // last_small == inspect case does happens and it is enhancement from the
      // previous code

      if (last_small != inspect)
        swap(coll[last_small], coll[inspect]);
    }
  }

  // move the pivot back
  swap(coll[first], coll[last_small]);

#ifdef SORT_QUICK_DEBUG
    cout << "pivot_pos(" << pivot_pos << ") build after (" << first << ", " << last << "): ";

    for (int i = first; i <= last; ++i)
      cout << coll[i] << ", ";

    cout << endl;
#endif // SORT_QUICK_DEBUG

  return last_small;
}

void sort_quick_01(vector<int> &coll, int first, int last)
{
  int last_small{};

  if (first < last)
  {
    last_small = build_partition(coll, first, last); 
    sort_quick_01(coll, first, last_small-1);
    sort_quick_01(coll, last_small+1, last);
  }
}

// from ansic, p87. exactly same way.
void sort_quick_02(vector<int> &v, int left, int right)
{
  int i, last;

  // do nothing if array contains fewer than two elements
  if( left >= right )
    return;

  // move partition elem
  // swap( v, left, (left+right)/2 );
  swap( v[left], v[(left+right)/2]);

  last = left;  // to v[0]

  // partition
  for(i = left+1; i <= right; i++)
    if( v[i] < v[left] )
    {
      // swap( v, ++last, i );   // shall ++last
      swap(v[++last], v[i]);   // shall ++last
    }

  // restore partition elem
  // swap(v, left, last);
  swap(v[left], v[last]);

  sort_quick_02( v, left, last-1 );
  sort_quick_02( v, last+1, right );
}

TEST(Algo, Ex)
{
  {
    vector<int> coll{29, 33, 35, 26, 19, 12, 22};

    sort_quick_01(coll, 0, coll.size()-1);

    EXPECT_THAT(coll, 
        ElementsAre(12, 19, 22, 26, 29, 33, 35));
  }

  {
    vector<int> coll{29, 33, 35, 26, 19, 12, 22};

    sort_quick_02(coll, 0, coll.size()-1);

    EXPECT_THAT(coll, 
        ElementsAre(12, 19, 22, 26, 29, 33, 35));
  }
}

/*
 */

// pre  : heap(1, n-1)
// post : heap(1, n)
// 
// void siftup(n)
// {
//   int i = n;
// 
// loop:
//   // invariant: heap(1, n) except between i and its parent
//   
//   // terminates when reaches the root
//   if (i == 1)
//     break;
//
//   p = i / 2;
//
//   // terminates when it meets heap property
//   if (x[i] >= x[p])
//     break;
// 
//   swap(i, p);
// 
//   i = p;
// }


// while version
void siftup_01(vector<int> &coll, int n)
{
  int i = n;
  int p = i / 2;

  for (; (i != 1) && (coll[i] <= coll[p]);)
  {
    swap(coll[i], coll[p]);
    i = p;
    p = i / 2;
  }
}

void siftup_02(vector<int> &coll, int n)
{
  int i = n;
  int p = i / 2;

  while (i != 1 && coll[i] <= coll[p])
  {
    swap(coll[i], coll[p]);
    i = p;
    p = i / 2;
  }
}

// // for version, (cxx-error) 
// void siftup_01(vector<int> &coll, int n)
// {
//   int i = n;
//   int p = i / 2;
// 
//   for (; (i != 1) && (coll[i] <= coll[p]); i = p)
//   {
//     swap(coll[i], coll[p]);
//     p = i / 2;
//   }
// 
//   // effectively this and should update p after i so this is an error
//   //
//   // for (; (i != 1) && (coll[i] <= coll[p]);)
//   // {
//   //   swap(coll[i], coll[p]);
//   //   p = i / 2;
//   //   i = p;
//   // }
// }

// // for version, (cxx-error) 
// void siftup_01_error(vector<int> &coll, int n)
// {
//   int i = n;
//   int p = i / 2;
// 
//   // for (; (i != 1) && (coll[i] <= coll[p]); i = p)
//   // {
//   //   p = i / 2;
//   //   swap(coll[i], coll[p]);
//   // }
// 
//   // effectively this and cause error since has different pair of i and p in
//   // check of the lopp.
//   for (; (i != 1) && (coll[i] <= coll[p]);)
//   {
//     p = i / 2;
//     swap(coll[i], coll[p]);
//     i = p;
//   }
// }

// for version from text
void siftup_min_text(vector<int> &coll, int n)
{
  int i, p;
  for (i = n; i > 1 && coll[p = i/2] > coll[i]; i = p)
    swap(coll[i], coll[p]);
}


// pre  : heap(2, n)
// post : heap(1, n)
// 
// void siftdown(n)
// {
//   int i = 1;
// 
// loop:
//   // invariant: heap(1, n) except between i and its children(0, 1, or 2)
//   
//   // c is index of the left child
//   c = i * 2;
//
//   // terminates when no children
//   if (c > n)
//     break;
// 
//   // when the right child is lesser
//   if (c+1 <= n)
//    if (x[c] > x[c+1])
//      c++;
//
//   // terminates when it meets heap property
//   if (x[i] <= x[c])
//     break;
// 
//   swap(i, c);
// 
//   i = c;
// }

void siftdown_01(vector<int> &coll, int n)
{
  int i = 1;
  int c = i * 2;

  if (c+1 <= n)
    if (coll[c] > coll[c+1])
      c++;

  while ((c < n) && (coll[i] > coll[c]))
  {
    swap(coll[i], coll[c]);

    i = c;

    c = i * 2;

    if (c+1 <= n)
      if (coll[c] > coll[c+1])
        c++;
  }
}

// void siftdown_01(vector<int> &coll, int n)
// {
//   int i = 1;
//   int c = i * 2;
// 
//   if (c+1 <= n)
//     if (coll[c] > coll[c+1])
//       c++;
// 
//   for (; (c < n) && (coll[i] > coll[c]); i = c)
//   {
//     swap(coll[i], coll[c]);
// 
//     if (c+1 <= n)
//       if (coll[c] > coll[c+1])
//         c++;
// 
//     c = i * 2;
//   }
// }


void siftdown_min_text(vector<int> &coll, int n)
{
  int c;

  for (int i = 1; (c = i*2) <= n; i = c)
  {
    if (c+1 <= n && coll[c] > coll[c+1])
      c++;

    if (coll[i] <= coll[c])
      break;

    swap(coll[i], coll[c]);
  }
}


TEST(Heap, SiftUp)
{
  {
    // not use coll[0], 13 elements 
    vector<int> coll{0, 12, 20, 15, 29, 23, 17, 22, 35, 40, 26, 51, 19};
    // +1, 14 elements
    coll.push_back(13);

    // range is [1, 13]
    siftup_01(coll, 13);

    //                   {0, 12,/20, 13,/29, 23, 15, 22,/35, 40, 26, 51, 19, 17}
    EXPECT_THAT(coll, 
        ElementsAreArray({0, 12, 20, 13, 29, 23, 15, 22, 35, 40, 26, 51, 19, 17}));
  }

  {
    // not use coll[0], 13 elements 
    vector<int> coll{0, 12, 20, 15, 29, 23, 17, 22, 35, 40, 26, 51, 19};
    // +1, 14 elements
    coll.push_back(13);

    siftup_min_text(coll, 13);

    //                   {0, 12,/20, 13,/29, 23, 15, 22,/35, 40, 26, 51, 19, 17}
    EXPECT_THAT(coll, 
        ElementsAreArray({0, 12, 20, 13, 29, 23, 15, 22, 35, 40, 26, 51, 19, 17}));
  }
}

TEST(Heap, SiftDown)
{
  {
    // when did x[1] = 18
    vector<int> coll{0, 18, 20, 15, 29, 23, 17, 22, 35, 40, 26, 51, 19};
    siftdown_01(coll, coll.size()-1);
    EXPECT_THAT(coll,
        ElementsAreArray({0,15,20,17,29,23,18,22,35,40,26,51,19}));
  }
  {
    // when did x[1] = 18
    vector<int> coll{0, 18, 20, 15, 29, 23, 17, 22, 35, 40, 26, 51, 19};
    siftdown_min_text(coll, coll.size()-1);
    EXPECT_THAT(coll,
        ElementsAreArray({0,15,20,17,29,23,18,22,35,40,26,51,19}));
  }
}


// template <typename T>
// class PriQueue {
//   public:
//     PriQueue(int m)
//     {
//       maxsize_ = m;
//       x_ = new T[maxsize_ +1];
//       n_ = 0;
//     }
// 
//     void insert(T t)
//     {
//       if (n_ >= maxsize_)
//         error;
// 
//       x_[++n] = t;
// 
//       // pre : heap(1, n-1)
//       siftup();
//       // post: heap(1, n)
//     }
// 
//     int extractmin()
//     {
//       T t;
// 
//       if (n_ < 1)
//         error;
// 
//       t = x_[1];
// 
//       x_[1] = x[n--];
// 
//       // pre : heap(2, n)
//       siftdown();
//       // post: heap(1, n)
// 
//       return t;
//     }
// 
//   private:
//     int n_, maxsize_;
//     T *x_;
// };

template <typename T>
class PriQueue {
  public:
    PriQueue(int size)
    {
      maxsize_ = size;
      coll_ = new T[maxsize_ +1];
      n_ = 0;
    }
    ~PriQueue()
    {
      if (coll_)
        delete coll_;
      n_ = 0;
    }

    void insert(T value)
    {
      int parent{};

      // error
      if (n_ >= maxsize_)
        return;

      coll_[++n_] = value;

      // heap(1, n-1)
      // siftup()
      // heap(1, n)
      // siftup(n) 
      for (int i = n_; i > 1 && coll_[i] < coll_[parent = i/2]; i = parent) 
        swap(coll_[i], coll_[parent]);
    }

    int extractmin()
    {
      T min_value{};
      int child{};

      // error
      if (n_ < 1)
        return -1;

      min_value = coll_[1];

      coll_[1] = coll_[n_--];

      // heap(2, n)
      // siftdown(n)
      // heap(1,n)

      for (int i = 1; (child = i*2) <= n_; i = child)
      {
        if (child+1 <= n_ && coll_[child] > coll_[child+1])
          child++;

        if (coll_[i] <= coll_[child])
          break;

        swap(coll_[i], coll_[child]);
      }

      return min_value;
    }

  private:
    int n_, maxsize_;
    T *coll_;
};


// simply use `opposite` comparisons
template <typename T>
class PriQueueMax {
  public:
    PriQueueMax(int size)
    {
      maxsize_ = size;
      coll_ = new T[maxsize_ +1];
      n_ = 0;
    }
    ~PriQueueMax()
    {
      if (coll_)
        delete coll_;
      n_ = 0;
    }

    void insert(T value)
    {
      int parent{};

      // error
      if (n_ >= maxsize_)
        return;

      coll_[++n_] = value;

      // heap(1, n-1)
      // siftup()
      // heap(1, n)
      // siftup(n) 
      for (int i = n_; i > 1 && coll_[i] > coll_[parent = i/2]; i = parent) 
        swap(coll_[i], coll_[parent]);
    }

    int extractmin()
    {
      T min_value{};
      int child{};

      // error
      if (n_ < 1)
        return -1;

      min_value = coll_[1];

      coll_[1] = coll_[n_--];

      // heap(2, n)
      // siftdown(n)
      // heap(1,n)

      for (int i = 1; (child = i*2) <= n_; i = child)
      {
        if (child+1 <= n_ && coll_[child] < coll_[child+1])
          child++;

        if (coll_[i] >= coll_[child])
          break;

        swap(coll_[i], coll_[child]);
      }

      return min_value;
    }

  private:
    int n_, maxsize_;
    T *coll_;
};


template <typename T, typename F = std::less<T>>
class PriQueueTemplate {
  public:
    PriQueueTemplate(int size)
    {
      maxsize_ = size;
      coll_ = new T[maxsize_ +1];
      n_ = 0;
      f_ = F();
    }
    ~PriQueueTemplate()
    {
      if (coll_)
        delete coll_;
      n_ = 0;
    }

    void insert(T value)
    {
      int parent{};

      // error
      if (n_ >= maxsize_)
        return;

      coll_[++n_] = value;

      // heap(1, n-1)
      // siftup()
      // heap(1, n)
      // siftup(n) 
      for (int i = n_; i > 1 && f_(coll_[i], coll_[parent = i/2]); i = parent) 
        swap(coll_[i], coll_[parent]);
    }

    int extractmin()
    {
      T min_value{};
      int child{};

      // error
      if (n_ < 1)
        return -1;

      min_value = coll_[1];

      coll_[1] = coll_[n_--];

      // heap(2, n)
      // siftdown(n)
      // heap(1,n)

      for (int i = 1; (child = i*2) <= n_; i = child)
      {
        if (child+1 <= n_ && !f_(coll_[child], coll_[child+1]))
          child++;

        if (f_(coll_[i], coll_[child]) || coll_[i] == coll_[child])
          break;

        swap(coll_[i], coll_[child]);
      }

      return min_value;
    }

  private:
    int n_, maxsize_;
    T *coll_;
    F f_;
};


TEST(Heap, PriQueue)
{
  // min
  {
    vector<int> coll;

    int n = 10;
    PriQueue<int> pq(n);

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
    PriQueue<int> pq(n);

    for (int i = 0; i < n; ++i)
      pq.insert(coll[i]);

    for (int i = 0; i < n; ++i)
      collo.push_back(pq.extractmin());

    EXPECT_THAT(collo, ElementsAreArray({0,1,2,3,4,5,6,7,8,9}));
  }

  // max
  {
    vector<int> coll;

    int n = 10;
    PriQueueMax<int> pq(n);

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
    PriQueueMax<int> pq(n);

    for (int i = 0; i < n; ++i)
      pq.insert(coll[i]);

    for (int i = 0; i < n; ++i)
      collo.push_back(pq.extractmin());

    EXPECT_THAT(collo, ElementsAreArray({9,8,7,6,5,4,3,2,1,0}));
  }

  // min, template
  {
    vector<int> coll;

    int n = 10;
    PriQueueTemplate<int> pq(n);

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
    PriQueueTemplate<int> pq(n);

    for (int i = 0; i < n; ++i)
      pq.insert(coll[i]);

    for (int i = 0; i < n; ++i)
      collo.push_back(pq.extractmin());

    EXPECT_THAT(collo, ElementsAreArray({0,1,2,3,4,5,6,7,8,9}));
  }

  // max, template
  {
    vector<int> coll;

    int n = 10;
    PriQueueTemplate<int, std::greater<int>> pq(n);

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
    PriQueueTemplate<int, std::greater<int>> pq(n);

    for (int i = 0; i < n; ++i)
      pq.insert(coll[i]);

    for (int i = 0; i < n; ++i)
      collo.push_back(pq.extractmin());

    EXPECT_THAT(collo, ElementsAreArray({9,8,7,6,5,4,3,2,1,0}));
  }
}


void sort_heap_min(vector<int> &coll)
{
  int n = coll.size() - 1;

  // convert input to heap
  for (int i = 2; i <= n; ++i)
    siftup_min_text(coll, i);

  // // convert heap to sorted
  // for (int i = n; i > 1; --i)
  // {
  //   swap(coll[1], coll[i]);
  //   siftdown_min_text(coll, i-1);
  // }

  // convert heap to sorted
  for (int i = n; i > 1;)
  {
    swap(coll[1], coll[i]);
    siftdown_min_text(coll, --i);
  }
}

TEST(Heap, Sort)
{
  {
    // add the dummpy at 0 to make heap
    vector<int> coll{0,4,5,6,7,8,9,1,2,3,0};

    sort_heap_min(coll);

    // descending order
    EXPECT_THAT(coll, ElementsAreArray({0,9,8,7,6,5,4,3,2,1,0}));
  }
  
  // max verison will make ascending order
  //
  // {
  //   // add the dummpy at 0 to make heap
  //   vector<int> coll{0,4,5,6,7,8,9,1,2,3,0};
  //
  //   sort_heap_02(coll);
  //
  //   EXPECT_THAT(coll, ElementsAreArray({0,0,1,2,3,4,5,6,7,8,9}));
  // }
  // {
  //   vector<int> coll{ 0, 33, 2, 31, 5, 30, 6, 12, 10, 13, 15, 17, 29, 3 };
  //   sort_heap_02(coll); 
  //   EXPECT_THAT(coll, 
  //       ElementsAreArray({0, 2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33 }));
  // }
}

int main(int argc, char** argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
