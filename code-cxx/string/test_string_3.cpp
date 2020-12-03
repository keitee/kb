// #include <experimental/string_view>
#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <string_view>

#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp

using namespace std;

using testing::ElementsAre;
using testing::Eq;
using testing::FloatEq;
using testing::StrEq;

/*
// ={=========================================================================
string-view cxx-17
https://www.modernescpp.com/index.php/c-17-what-s-new-in-the-library

A std::string_view is a non-owning reference to a string. It represents a "view"
of a sequence of characters.

The question remains. Why do we need a std::string_view? The answer is easy.
It's quite cheap to copy a std::string_view. A std::string_view only needs two
information: the pointer to the character sequence and their length. 


In contrary to std::string, std::string_view allocates no memory.

*/

// ={=========================================================================
TEST(StringView, use_view)
{
  // Moves the start of the view forward by n characters.
  // sv.remove_prefix();
  {
    std::string str{"   A lot of space"};
    std::string_view sv = str;

    sv.remove_prefix(std::min(sv.find_first_not_of(" "), sv.size()));

    // std::cout << "str : " << str << std::endl
    //           << "sv  : " << sv << std::endl;

    EXPECT_THAT(str, "   A lot of space");
    EXPECT_THAT(sv, "A lot of space");
  }

  // Moves the end of the view back by n characters.
  // sv.remove_suffix();
  //
  // [ RUN      ] StringView.string_view
  // arr : A lot of space, size=17
  // sv  : A lot of space, size=14
  // [       OK ] StringView.string_view (0 ms)

  {
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

    std::string_view sv(arr, sizeof arr);

    auto trimPos = sv.find('\0');
    if (trimPos != sv.npos)
      sv.remove_suffix(sv.size() - trimPos);

    std::cout << "arr : " << arr << ", size=" << sizeof arr << std::endl
              << "sv  : " << sv << ", size=" << sv.size() << std::endl;
  }
}

/*
O(n) versus O(1)
std::string and std::string_view have both a method substr. The method of the
std::string returns a substring but the method of the std::string_view returns a
view of a substring. This sounds not so thrilling. But there is a big difference
between both methods. std::string::substr has linear complexity.
std::string_view::substr has constant complexity. That means that the
performance of the operation on the std::string is directly dependent on the
size of the substring but the performance of the operation on the
std::string_view is independent of the size of the substring.

Now I'm curious. Let's make a simple performance comparison.

without "-O2"

[ RUN      ] StringView.substr_performace
grimms size: 502271
time for string substr               : 1153100186 seconds
time for string_view substr         : 74111297 seconds
time for string/time for string_view: 15
[       OK ] StringView.substr_performace (1893 ms)

with "-O2" optimisation

[ RUN      ] StringView.substr_performace
grimms size: 502271
time for string substr              : 152542582 seconds
time for string_view substr         : 5270641 seconds
time for string/time for string_view: 28
[       OK ] StringView.substr_performace (417 ms)


The optimisation makes no big difference in the case of std::string but a great
difference in the case of std::string_view. Making a substring with
std::string_view is about 45 times faster than using std::string. If that is not
a reason to use std::string_view? 

*/

namespace string_view
{
  // The count is 30. Therefore, no small string optimisation kicks in. 
  static const int COUNT  = 30;

  static const int LOOP = 10000000;
} // namespace string_view

// ={=========================================================================
TEST(StringView, substr_performace)
{
  using namespace string_view;

  // https://www.gnu.org/software/bash/manual/bash.txt
  std::ifstream ifs("bash.txt");

  std::stringstream ss{};
  ss << ifs.rdbuf();
  std::string grimms = ss.str();

  auto size = grimms.size();

  std::cout << "grimms size: " << size << std::endl;

  // random values
  std::random_device seed;
  std::mt19937 engine(seed());
  std::uniform_int_distribution<> dist{0, size - COUNT - 2};
  std::vector<int> randValues{};

  for (auto i = 0; i < LOOP; ++i)
    randValues.push_back(dist(engine));

  auto start = std::chrono::steady_clock::now();

  for (auto i = 0; i < LOOP; ++i)
    grimms.substr(randValues[i], COUNT);

  auto time_for_string = std::chrono::steady_clock::now() - start;

  std::cout << "time for string substr              : "
            << time_for_string.count() << " seconds\n";

  // string_view
  std::string_view sv{grimms};

  start = std::chrono::steady_clock::now();

  for (auto i = 0; i < LOOP; ++i)
    sv.substr(randValues[i], COUNT);

  auto time_for_string_view = std::chrono::steady_clock::now() - start;

  std::cout << "time for string_view substr         : " << time_for_string_view.count()
            << " seconds\n";

  // ratio
  std::cout << "time for string/time for string_view: "
            << time_for_string.count() / time_for_string_view.count()
            << std::endl;
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
