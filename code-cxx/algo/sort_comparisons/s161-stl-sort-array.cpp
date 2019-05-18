// 
// Programming Pearl, Column 1, 1.6 Problems
// - sort, stl algorithm on array
// - 10 million items

#include <iostream>
#include <vector>
#include <algorithm>

// $ time ./solution <numbers.in

const int INPUT_MAX = 10000000;
int a[INPUT_MAX];

int main(void)
{
  int n{};

  // To compare C++ version, use the same cin.
  //
  // while (scanf("%d", &a[n]) != EOF)
  //   ++n;
  
  while (std::cin >> a[n])
    ++n;

  // std::sort(std::begin(a), std::end(a));
  std::sort(std::begin(a), std::begin(a)+n);

  // don't have output part to focus on sorting time rather than input/output

  std::cout << "sort, 1.6.1, input size: " << n 
    << ", stl algorithm sort on array" << std::endl;

  return 0;
}
