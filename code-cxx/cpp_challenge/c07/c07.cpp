#include "gmock/gmock.h"

#include <iostream>
#include <list>
#include <deque>
#include <fstream>
#include <thread>

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;


/*
={=============================================================================
Concurrency

61. Parallel transform algorithm

Write a general-purpose algorithm that applies a given unary function to
transform the elements of a range in parallel. The unary operation used to
transform the range must not invalidate range iterators or modify the elements
of the range. The level of parallelism, that is, the number of execution threads
and the way it is achieved, is an implementation detail.

The general-purpose function std::transform() applies a given function to a
range and stores the result in another (or the same) range. The requirement for
this problem is implementing a parallel version of such a function. 

A general-purpose one would take iterators as arguments to define the first and
one-past-last element of the range. Because the unary function is applied in the
same manner to all the elements of the range, it is fairly simple to parallelize
the operation. For this task, we will be using threads. Since it is not
specified how many threads should be running at the same time, we could use
std::thread::hardware_concurrency(). This function returns a hint for the number
of concurrent threads supported by the implementation.

A parallel version of the algorithm performs better than a sequential
implementation only if the size of the range exceeds a particular threshold,
which may vary with compilation options, platform, or hardware. In the following
implementation that threshold is set to 10,000 elements. As a further exercise,
you could experiment with various thresholds and range sizes to see how the
execution time changes.

The following function, ptransform(), implements the parallel transform
algorithm as requested. It simply calls std::transform() if the range size does
not exceed the defined threshold. Otherwise, it splits the range into several
equal parts, one for each thread, and calls std::transform() on each thread for
a particular subrange. In this case, the function blocks the calling thread
until all the worker threads finish execution:

template <typename RandomAccessIterator, typename F>
void ptransform(RandomAccessIterator begin, RandomAccessIterator end, 
                F&& f)
{
   auto size = std::distance(begin, end);
   if (size <= 10000)
   {
      std::transform(begin, end, begin, std::forward<F>(f));
   }
   else
   {
      std::vector<std::thread> threads;
      int thread_count = std::thread::hardware_concurrency();
      auto first = begin;
      auto last = first;
      size /= thread_count;
      for (int i = 0; i < thread_count; ++i)
      {
         first = last;
         if (i == thread_count - 1) last = end;
         else std::advance(last, size);

         threads.emplace_back([first, last, &f]() {
            std::transform(first, last, first, std::forward<F>(f));
         });
      }

      for (auto & t : threads) t.join();
   }
}

The function palter(), shown as follows, is a helper function that applies
ptransform() to an std::vector and returns another std::vector with the result:

template <typename T, typename F>
std::vector<T> palter(std::vector<T> data, F&& f)
{
   ptransform(std::begin(data), std::end(data),
              std::forward<F>(f));
   return data;
}

The function can be used as follows (a complete example can be found in the
    source code accompanying this book):

int main()
{
   std::vector<int> data(1000000);
   // init data
   auto result = palter(data, [](int const e) {return e * e; });
}

In C++17, a series of standard general-purpose algorithms, including
std::transform(), have overloads that implement a parallel version of the
algorithm that can be executed according to a specified execution policy.

*/

namespace U61_Text
{

  template <typename Iterator, typename F>
    void parallel_transform(Iterator begin, Iterator end, F&& f) 
    {
      auto size = std::distance(begin, end);
      if (size <= 10000)
      {
        std::transform(begin, end, begin, std::forward<F>(f));
      }
      else
      {
        std::vector<std::thread> threads;

        // since it's 1 for VM
        // int thread_count = std::thread::hardware_concurrency();

        int thread_count = 4;

        auto first = begin;
        auto last = first;
        size /= thread_count;

        for (int i = 0; i < thread_count; ++i)
        {
          first = last;

          if (i == thread_count -1)
            last = end;
          else
            std::advance(last, size);

          // *cxx-vector-emplace*
          threads.emplace_back(
              [first, last, &f]()
              {
              std::transform(first, last, first, std::forward<F>(f));
              });
        }

        for (auto &t : threads)
          t.join();
      }
    }

  template <typename T, typename F>
    std::vector<T> palter(std::vector<T> data, F&& f)
    {
      parallel_transform(std::begin(data), std::end(data),
          std::forward<F>(f));
      return data;
    }

} // namespace


TEST(U61, Text)
{
  using namespace U61_Text;

  // initialize all 2. this is different from the text since it will have all 0
  // and the result of parallen_transform is also 0 in that case.

  std::vector<int> data(1000000, 2);

  auto result = palter(data, [](int const e) {return e * e; });

  // initialize all 4
  std::vector<int> expected(1000000, 4);

  EXPECT_THAT(result, expected);
}


/*
={=============================================================================
62. Parallel min and max element algorithms using threads

Implement general-purpose parallel algorithms that find the minimum value and,
respectively, the maximum value in a given range. The parallelism should be
implemented using threads, although the number of concurrent threads is an
implementation detail.lgorithms and Data Structures

{
   std::vector<int> data(count);
   // init data
   auto rmin = pmin(std::begin(data), std::end(data));
   auto rmax = pmax(std::begin(data), std::end(data));
}

*/

// if take the same approach as U61

namespace U62_2019_01_14
{
  template <typename Iterator>
    typename std::iterator_traits<Iterator>::value_type 
    paralle_min_element(Iterator begin, Iterator end)
    {
      using T = typename std::iterator_traits<Iterator>::value_type;

      T result;

      auto size = std::distance(begin, end);
      if (size <= 10000)
      {
        std::min_element(begin, end);
      }
      else
      {
        std::vector<std::thread> threads;

        int thread_count = 4;
        auto first = begin;
        auto last = first;
        size /= thread_count;

        std::vector<T> results(thread_count);

        for (int i = 0; i < thread_count; ++i)
        {
          first = last;

          if (i == thread_count -1)
            last = end;
          else 
            std::advance(last, size);

          threads.emplace_back([&]()
              {
                results[i] = *std::min_element(first, last);
              });
        }

        for (auto &t : threads)
          t.join();

        for (auto &e : results)
          cout << "r: " << e << endl;

        result = *std::min_element(results.begin(), results.end());
      }

      return result;
    }

} // namespace

TEST(U62, Text)
{
  using namespace U62_2019_01_14;

  // initialize all 2. this is different from the text since it will have all 0
  // and the result of parallen_transform is also 0 in that case.

  std::vector<int> data(1000000, 2);

  auto result = paralle_min_element(data.begin(), data.end());

  EXPECT_THAT(result, 2);
}


/*

63. Parallel min and max element algorithms using asynchronous functions

Implement general-purpose parallel algorithms that find the minimum value and,
respectively, the maximum value in a given range. The parallelism should be
implemented using asynchronous functions, although the number of concurrent
functions is an implementation detail.

*/

/*

64. Parallel sort algorithm

Write a parallel version of the sort algorithm as defined for problem 53. Sort
Algorithm, in Chapter 6, Algorithms and Data Structures, which, given a pair of
random access iterators to define its lower and upper bounds, sorts the elements
of the range using the quicksort algorithm. The function should use the
comparison operators for comparing the elements of the range. The level of
parallelism and the way to achieve it is an implementation detail.

*/

/*

43. Meeting time for multiple time zones

Write a function that, given a list of meeting participants and their time
zones, displays the local meeting time for each participant.

*/

/*

44. Monthly calendar

Write a function that, given a year and month, prints to the console the month
calendar. The expected output format is as follows (the example is for December
2017):

Mon Tue Wed Thu Fri Sat Sun
                  1   2   3
  4   5   6   7   8   9  10
 11  12  13  14  15  16  17
 18  19  20  21  22  23  24
 25  26  27  28  29  30  31

*/


// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
