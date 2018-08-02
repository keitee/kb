#include <iostream>
#include <set>

// changes:
//
//       void qsort_r(void *base, size_t nmemb, size_t size,
//                  int (*compar)(const void *, const void *, void *),
//                  void *arg);
//
// The  comparison  function must return an integer less than, equal to, or
// greater than zero if the first argument is considered to be respectively less
// than, equal to, or greater than the second. If two members compare as equal,
// their order in the sorted array is undefined.
//
// $ time ./solution <numbers.in
//

int main(void)
{
  int value{};
  std::set<int> coll{};

  // To compare C++ version, use the same cin.
  //
  // while (scanf("%d", &a[n]) != EOF)
  //   ++n;
  
  while (std::cin >> value)
    coll.insert(value);

  // don't have output part to focus on sorting time rather than input/output

  std::cout << "1.6.1: stl" << std::endl;

  return 0;
}
