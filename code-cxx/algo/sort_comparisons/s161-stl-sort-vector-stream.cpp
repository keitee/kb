// 
// Programming Pearl, Column 1, 1.6 Problems
// - sort, stl algorithm on std::vector
// - 10 million items

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

// $ time ./solution <numbers.in

int main(void)
{
  // int value{};
  // int n{};
  std::vector<int> coll{};
  coll.reserve(10000000);

  // To compare C++ version, use the same cin.
  //
  // while (scanf("%d", &a[n]) != EOF)
  //   ++n;
  
  // while (std::cin >> value)
  // {
  //   coll.push_back(value);
  //   ++n;
  // }

  copy(std::istream_iterator<int>(std::cin), std::istream_iterator<int>(),
      back_inserter(coll));

  sort(coll.begin(), coll.end());

  // don't have output part to focus on sorting time rather than input/output

  std::cout << "sort, 1.6.1, input size: " << coll.size() 
    << ", stl algorithm sort on std::vector via iterator" << std::endl;

  return 0;
}
