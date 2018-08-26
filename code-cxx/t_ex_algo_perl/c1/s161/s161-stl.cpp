#include <iostream>
#include <set>

// $ time ./solution <numbers.in

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

  std::cout << "1.6.1: stl set" << std::endl;

  return 0;
}
