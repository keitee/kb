#include "gmock/gmock.h"

#include <iostream>
#include <list>
#include <deque>
#include <fstream>

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;


// ={=========================================================================

/*

Concurrency

61. Parallel transform algorithm

Write a general-purpose algorithm that applies a given unary function to
transform the elements of a range in parallel. The unary operation used to
transform the range must not invalidate range iterators or modify the elements
of the range. The level of parallelism, that is, the number of execution threads
and the way it is achieved, is an implementation detail.


*/


/*

62. Parallel min and max element algorithms using threads

Implement general-purpose parallel algorithms that find the minimum value and,
respectively, the maximum value in a given range. The parallelism should be
implemented using threads, although the number of concurrent threads is an
implementation detail.lgorithms and Data Structures


*/


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
