#include <iostream>
#include <vector>
#include <algorithm>

// $ time ./solution <numbers.in

const int INPUT_MAX = 10000000;
int coll[INPUT_MAX];


// From Programming Pearl 11.1
//
// for i = [1, n)
//  // invariant: x[0..i-1] is sorted
//  // shift x[i] down to its proper place in x[0..i]
//
// From the swap version which do two operations, move down elements and put the
// saved back when loop ends as 01/02 version. So one update rather than doing
// it every time which runs faster.

int main(void)
{
  int n{};

  // To compare C++ version, use the same cin.
  //
  // while (scanf("%d", &a[n]) != EOF)
  //   ++n;

  while (std::cin >> coll[n])
    ++n;

  // void sort_insertion_05(vector<int> &coll)
  {
    int size = INPUT_MAX;

    for (int unsorted_index = 1; unsorted_index < size; ++unsorted_index)
    {
      int unsorted = coll[unsorted_index];
      int current_index = unsorted_index;

      for (; 0 < current_index && unsorted < coll[current_index-1];
          --current_index)
      {
        // swap current and current-1
        // swap(coll[current_index], coll[current_index-1]);
        coll[current_index] = coll[current_index-1];
      }

      coll[current_index] = unsorted;
    }
  }

  // don't have output part to focus on sorting time rather than input/output

  std::cout << "1.6.1: inseriton sort on array" << std::endl;

  return 0;
}
