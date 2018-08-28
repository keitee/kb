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

  std::cout << "1.6.1: n: " << n << " stl sort on array" << std::endl;

  return 0;
}
