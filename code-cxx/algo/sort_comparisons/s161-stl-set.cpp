// 
// Programming Pearl, Column 1, 1.6 Problems
// - sort, stl set container
// - 10 million items

#include <iostream>
#include <set>

// $ time ./solution <numbers.in

int main(void)
{
  int value{};
  int n{};
  std::set<int> coll{};

  // To compare C++ version, use the same cin.
  //
  // while (scanf("%d", &a[n]) != EOF)
  //   ++n;
  
  while (std::cin >> value)
  {
    coll.insert(value);
    ++n;
  }

  // don't have output part to focus on sorting time rather than input/output

  std::cout << "sort, 1.6.1, input size: " << n 
    << ", stl set container" << std::endl;

  return 0;
}
