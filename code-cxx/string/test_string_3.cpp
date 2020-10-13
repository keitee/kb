#include <experimental/string_view>
#include <iostream>
#include <string>

#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp

using namespace std;

using testing::ElementsAre;
using testing::Eq;
using testing::FloatEq;
using testing::StrEq;

// ={=========================================================================
// string-view

// cxx-17 feature
TEST(StringCxx17, check_string_view)
{
  std::string str                        = "   A lot of space";
  std::experimental::string_view strView = str;

  strView.remove_prefix(
    std::min(strView.find_first_not_of(" "), strView.size()));
  std::cout << "str     : " << str << std::endl
            << "strView : " << strView << std::endl;

  std::cout << std::endl;

  char arr[] = {'A',
                ' ',
                'l',
                'o',
                't',
                ' ',
                'o',
                'f',
                ' ',
                's',
                'p',
                'a',
                'c',
                'e',
                '\0',
                '\0',
                '\0'};
  std::experimental::string_view strView2(arr, sizeof arr);

  auto trimPos = strView2.find('\0');
  if (trimPos != strView2.npos)
    strView2.remove_suffix(strView2.size() - trimPos);

  std::cout << "arr     : " << arr << ", size=" << sizeof arr << std::endl
            << "strView2: " << strView2 << ", size=" << strView2.size()
            << std::endl;
}

void *operator new(std::size_t count)
{
  std::cout << "   " << count << " bytes" << std::endl;
  return malloc(count);
}

namespace
{
  void getString(const std::string &str) {}
} // namespace

TEST(StringCxx17, check_string_view1)
{
  std::cout << "std::string" << std::endl;
  std::string small  = "0123456789";
  std::string substr = small.substr(5);
  std::cout << "    " << substr << std::endl;
  std::cout << std::endl;

  std::cout << "getString" << std::endl;
  getString(small);
  getString("0123456789");
  const char message[] = "0123456789";
  getString(message);
  std::cout << std::endl;
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
