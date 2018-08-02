#include <iostream>

const int INPUT_MAX = 10000000;

int a[INPUT_MAX];


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
// There's an implementation detail to keep in mind though: the array must
// either be declared as a global variable, so that it's allocated in BSS
// segment, or allocated dynamically with malloc(). If this 10-million long
// array of integers is declared as a local variable inside main(), it will eat
// up 10000000*sizeof(int) bytes of stack. If sizeof(int) is 4, we would need
// about 40 MB of stack. Typically, the stack can't afford that, so the program
// would just segfault.

// An example input file is provided in numbers.in in this column's root
// directory. This file contains every number from 0 to 9999999 in random order.


int intcomp(const void *x, const void *y)
{
  return *(const int *)x - *(const int *)y;
}

int main(void)
{
  int n{};

  // To compare C++ version, use the same cin.
  //
  // while (scanf("%d", &a[n]) != EOF)
  //   ++n;
  
  while (std::cin >> a[n])
    ++n;

  qsort(a, n, sizeof(int), intcomp);

  // don't have output part to focus on sorting time rather than input/output
  // for (int i = 0; i < n; i++)
  //   std::cout << a[i] << std::endl;

  std::cout << "1.6.1: qsort" << std::endl;

  return 0;
}
