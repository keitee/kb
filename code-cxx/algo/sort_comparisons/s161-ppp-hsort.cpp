#include <iostream>
#include <algorithm>

// $ time ./solution <numbers.in

const int INPUT_MAX = 10000000;
int coll[INPUT_MAX];

using namespace std;

// From Programming Pearl 14

namespace algo_sort_heap
{
  // for version from text
  // void siftup_min_text(vector<int> &coll, int n)
  void siftup_min_text(int n)
  {
    int i, p;
    for (i = n; i > 1 && coll[p = i/2] > coll[i]; i = p)
      swap(coll[i], coll[p]);
  }

  // void siftdown_min_text(vector<int> &coll, int n)
  void siftdown_min_text(int n)
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

  // void sort_heap_min(vector<int> &coll)
  void sort_heap_min(int size)
  {
    // int n = coll.size() - 1;
    int n = size;

    // convert input to heap
    for (int i = 2; i <= n; ++i)
      siftup_min_text(i);

    // convert heap to sorted
    for (int i = n; i > 1; --i)
    {
      swap(coll[1], coll[i]);
      siftdown_min_text(i-1);
    }
  }
} // namespace

int main(void)
{
  using namespace algo_sort_heap;

  int n{};

  // To compare C++ version, use the same cin.
  //
  // while (scanf("%d", &a[n]) != EOF)
  //   ++n;

  while (std::cin >> coll[n])
    ++n;

  sort_heap_min(n);

  // don't have output part to focus on sorting time rather than input/output

  std::cout << "sort, 1.6.1, input size: " << n << ", ppp version heap sort" << std::endl;

  return 0;
}
