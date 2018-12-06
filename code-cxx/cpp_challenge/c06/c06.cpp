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

Algorithms and Data Structures

45. Priority queue

Write a data structure that represents a priority queue that provides constant
time lookup for the largest element, but has logarithmic time complexity for
adding and removing elements. A queue inserts new elements at the end and
removes elements from the top. By default, the queue should use operator< to
compare elements, but it should be possible for the user to provide a comparison
function object that returns true if the first argument is less than the second.
The implementation must provide at least the following operations:

push() to add a new element
pop() to remove the top element
top() to provide access to the top element
size() to indicate the number of elements in the queue
empty() to indicate whether the queue is empty

*/


/*

40. Number of days between two dates

Write a function that, given two dates, returns the number of days between the
two dates. The function should work regardless of the order of the input dates.

*/


/*

41. Day of the week

Write a function that, given a date, determines the day of the week. This
function should return a value between 1 (for Monday) and 7 (for Sunday).

*/

/*

42. Day and week of the year

Write a function that, given a date, returns the day of the year (from 1 to 365
or 366 for leap years) and another function that, for the same input, returns
the calendar week of the year.

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
