#include <iostream>
#include <string>
#include <vector>

std::string to_roman_1(unsigned int value)
{
  std::string result{};

  const std::vector<std::pair<int, std::string>> table = {
    std::make_pair(1000, "M"),
    std::make_pair(900, "CM"),
    std::make_pair(500, "D"),
    std::make_pair(400, "CD"),
    std::make_pair(100, "C"),
    std::make_pair(90, "XC"),
    std::make_pair(50, "L"),
    std::make_pair(40, "XL"),
    std::make_pair(10, "X"),
    std::make_pair(9, "IX"),
    std::make_pair(5, "V"),
    std::make_pair(4, "IV"),
    std::make_pair(1, "I")};

  for (const auto &e : table)
  {
    while (value >= e.first)
    {
      value -= e.first;
      result.append(e.second);
    }
  }

  // return result;
}

int main()
{
  to_roman_1(1900);

  return 0;
}
