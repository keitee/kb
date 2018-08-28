#include <iostream>
#include <vector>
#include <algorithm>

// $ time ./solution <numbers.in

int main(void)
{
  int value{};
  int n{};
  std::vector<int> coll{};
  coll.reserve(10000000);

  // To compare C++ version, use the same cin.
  //
  // while (scanf("%d", &a[n]) != EOF)
  //   ++n;
  
  while (std::cin >> value)
  {
    coll.push_back(value);
    ++n;
  }

  sort(coll.begin(), coll.end());

  // don't have output part to focus on sorting time rather than input/output

  std::cout << "1.6.1: n: " << n << " stl sort on vector" << std::endl;

  return 0;
}
