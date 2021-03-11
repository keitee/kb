#include <iostream>
#include <string>
#include <vector>
// #include <exception>
#include <stdexcept> 

std::string to_roman_1(unsigned int value)
{
  for (int i = 0; i < 100; ++i)
  {
    if (i == 50)
      throw std::runtime_error("got 50");
  }
}

int main()
{
  to_roman_1(1900);

  return 0;
}
