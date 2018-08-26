#include <iostream>
#include <algorithm>

// $ time ./solution <numbers.in

const int INPUT_MAX = 10000000;
int coll[INPUT_MAX];

using namespace std;

// From Programming Pearl 11.1

// note that swap happens on the same index and `lastsmall` vary. 

// int build_partition(vector<int> &coll, int first, int last)
int build_partition(int first, int last)
{
  int lastsmall = first;

  // as with binary search, chance to overflow
  int pivot = (first + last)/2;

  int pivot_value = coll[pivot];

#ifdef SORT_QUICK_DEBUG
  cout << "build i(" << first << ", " << last << "): ";

  for (int i = first; i <= last; ++i)
    cout << coll[i] << ", ";

  cout << endl;
#endif // SORT_QUICK_DEBUG

  // move it to the first pos
  swap(coll[first], coll[pivot]);

  for (int i = first+1; i <= last; ++i)
  {
    if (coll[i] < pivot_value)
      swap(coll[++lastsmall], coll[i]);
  }

  swap(coll[first], coll[lastsmall]);

#ifdef SORT_QUICK_DEBUG
  cout << "build o(" << first << ", " << last << "): ";

  for (int i = first; i <= last; ++i)
    cout << coll[i] << ", ";

  cout << endl;
#endif // SORT_QUICK_DEBUG

  return lastsmall;
}

// void sort_quick_01(vector<int> &coll, int first, int last)
void sort_quick_01(int first, int last)
{
  int lastsmall{};

  // when has more than one element
  if (first < last)
  {
    lastsmall = build_partition(first, last);
    sort_quick_01(first, lastsmall-1);
    sort_quick_01(lastsmall+1, last);
  }
}

int main(void)
{
  int n{};

  // To compare C++ version, use the same cin.
  //
  // while (scanf("%d", &a[n]) != EOF)
  //   ++n;

  while (std::cin >> coll[n])
    ++n;

  sort_quick_01(0, INPUT_MAX-1);

  // don't have output part to focus on sorting time rather than input/output

  std::cout << "1.6.1: quick sort on array" << std::endl;

  return 0;
}
