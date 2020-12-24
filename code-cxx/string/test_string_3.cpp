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
using testing::Ne;
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
https://www.modernescpp.com/index.php/c-17-avoid-copying-with-std-string-view

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

basic_string substr( size_type pos = 0, size_type count = npos ) const; 
(until C++20)

constexpr basic_string substr( size_type pos = 0, size_type count = npos ) const; 
(since C++20)

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

namespace cxx_string
{
  void foo1(const std::string &text)
  {
    // do some processing on input text.
    std::string token = text.substr(0, 3);
  }

  void foo2(std::string &text)
  {
    // do some processing on input text.
    std::string token = text.substr(0, 3);
  }

  // Where the cost of constructing an std::string object is a concern, the
  // readability and ease of usage are frequently compromised by using 
  // const char* and length parameters.
  // Better performance, but have to give up benefits of std::string interface.

  void foo3(const char* str, size_t length) 
  {
    // do some processing on input text.
    std::string token{str};
  }

  void foo4(std::string_view text)
  {
    // do some processing on input text.
    std::string_view token = text.substr(0, 3);
  }
}

/*
// ={=========================================================================
  **
   *  @class basic_string_view <string_view>
   *  @brief  A non-owning reference to a string.
   *
   *  @ingroup strings
   *  @ingroup sequences
   *
   *  @tparam _CharT  Type of character
   *  @tparam _Traits  Traits for character type, defaults to
   *                   char_traits<_CharT>.
   *
   *  A basic_string_view looks like this:
   *
   *  @code
   *    _CharT*    _M_str
   *    size_t     _M_len
   *  @endcode
   *
  template<typename _CharT, typename _Traits = std::char_traits<_CharT>>
    class basic_string_view
    {
    private:
      size_t	    _M_len;
      const _CharT* _M_str;
    };

https://isocpp.org/files/papers/N3762.html

*/

TEST(StringView, points_to_make_1)
{
  using namespace cxx_string;

  {
    // this works but creates a temporary
    foo1("this is input");

    // this causes compile error
    // foo2("this is input");
  }

  {
    const char *input = "this is input";

    foo1(input);

    // this causes compile error again regardless of:
    // const char *input = "this is input";
    // char *input = "this is input";
    //
    // foo2(input);
  }

  // NOTE:
  // https://www.nextptr.com/tutorial/ta1217154594/cplusplus-stdstring_view-for-better-performance-an-example-use-case
  // Point1. No std::string allocation
  // The motivation behind std::string_view is that it is quite common for
  // functions to require a read-only reference to an "std::string-like object"
  // where the exact type of the object does not matter. "The drawback" of using
  // const std::string& in those situations is that it requires creating an
  // std::string object.

  {
    const char *input = "this is input";

    foo4("this is input");
    foo4(input);
  }
}

/*
NOTE:
Point2.

An Example: OSI Symbols of Option Contracts

Assume a hypothetical trading system application that uses a large number of
option contract OSI symbols (e.g., "AAPL 131101C00470000"). An OSI symbol is a
21-character long identifier that encodes various attributes of an option
contract. The application loads a delimited list of all the symbols from a file
to a buffer. Then the symbols from the buffer are split and stored in an
std::unordered_set of std::string objects:

// A type alias for symbol 
using Symbol = std::string;

// A routine to split and load the symbols in a collection
template<typename C>
void loadSymbols(const char* source, size_t len, 
                    char delim, C& coll) {

 const char* first = source;
 const char* last = source+len;

 while(true) {
  // find delimiter location
  auto delimPos = std::find(first, last, delim);

  // check if delimiter found
  if(delimPos == last)
   break; // no more delimiter

  // Insert the Symbol in coll
  coll.insert(Symbol(first, delimPos - first));

  // advance the first pointer for next token
  first = delimPos + 1;
 }
}

// Somewhere else 

// buffer holding '|' delimited symbol list.  Could be a std::vector<char>
std::string buf = "SPX   191115C02820000|"
                  "SPX   191115P02820000|"
                  // many more symbols...
                  ;

// the symbols collection
std::unordered_set<Symbol> symbols;

 // load symbols 
loadSymbols(buf.c_str(), buf.size(), '|', symbols);

At various places in the application, the symbols are searched from the symbols
collection, copied when necessary, and stored to other STL containers when
needed. But nowhere, the symbols are modified. It is costly in terms of
performance and memory usage to have a vast number of the std::string objects,
mainly when the dynamic memory allocation is involved. To minimize dynamic
allocation, a typical implementation of std::string is optimized to store a
small string within itself in a char array; this is called short/small string
optimization (SSO). However, the small-string size for optimization is
implementation-dependent and could very well be below 21-chars.

This application can undoubtedly benefit from using std::string_view. The first
thing we have to do is change the type alias of symbol to be of type
std::string_view instead of std::string:

// A type alias for symbol 
using Symbol = std::string_view;

NOTE:
Point3.

it is important to consider here is that the buffer that holds the symbol list
should live for the lifetime of the application; otherwise, all the symbol
std::string_views would be invalidated.

https://en.cppreference.com/w/cpp/string/basic_string_view
It is the programmer's responsibility to ensure that std::string_view does not
outlive the pointed-to character array:

*/

namespace cxx_string_point_3
{
  std::string foo1()
  {
    return "foo1";
  }

  std::string foo2()
  {
    return "This size for std::string is 15 for MSVC and GCC and 23 for Clang.";
  }
}

TEST(StringView, points_to_make_3)
{
  using namespace cxx_string_point_3;

  // *cxx-return* *cxx-temporary-lifetime* so its lifetime is extended
  {
    const std::string &value = foo1();

    EXPECT_THAT(value.size(), 4);
    EXPECT_THAT(strlen(value.c_str()), 4);
  }

  // ok, works fine.
  {
    std::string_view value = foo1();

    EXPECT_THAT(value.size(), 4);
    EXPECT_THAT(strlen(std::string(value).c_str()), 4);
  }

  // *cxx-return* *cxx-temporary-lifetime* so its lifetime is extended
  {
    const std::string &value = foo2();

    EXPECT_THAT(value.size(), 66);
    EXPECT_THAT(strlen(value.c_str()), 66);
  }

  // still hold when considering SSO? NO
  {
    std::string_view value = foo2();

    EXPECT_THAT(value.size(), 66);

    std::string coll{value};

    EXPECT_THAT(coll.size(), 66);

    // for (const auto e: coll)
    //   std::cout << e << std::endl;
    //
    // [ RUN      ] StringView.points_to_make_3
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // e
    // 
    // f
    // o
    // r
    // 
    // s
    // t
    // d
    // :
    // :
    // s
    // t
    // r
    // i
    // n
    // g
    // 
    // i
    // s
    // 
    // 1
    // 5
    // 
    // f
    // o
    // r
    // 
    // M
    // S
    // V
    // C
    // 
    // a
    // n
    // d
    // 
    // G
    // C
    // C
    // 
    // a
    // n
    // d
    // 
    // 2
    // 3
    // 
    // f
    // o
    // r
    // 
    // C
    // l
    // a
    // n
    // g
    // .
    // [       OK ] StringView.points_to_make_3 (1 ms)

    // WHY??
    EXPECT_THAT(strlen(coll.c_str()), 0);
    EXPECT_THAT(strlen(std::string(value).c_str()), 0);
  }

  // making it const reference? Still NO.
  {
    const std::string_view &value = foo2();

    EXPECT_THAT(value.size(), 66);

    std::string coll{value};

    EXPECT_THAT(coll.size(), 66);

    // WHY??
    EXPECT_THAT(strlen(coll.c_str()), Ne(66));
    EXPECT_THAT(strlen(std::string(value).c_str()), Ne(0));
  }
}

/*

NOTE:
Point4.
that the std::string_view does not have the c_str() interface to return a
null-terminated string. We would have to convert the std::string_view to std::
string wherever a null-terminated string is required:

error: ‘class std::basic_string_view<char>’ has no member named ‘c_str’;

*/

TEST(StringView, points_to_make_4)
{
  // it adds "null" atomatically
  const char cstring[] = "string";

  {
    std::string coll{"string"};

    EXPECT_THAT(coll.size(), 6); 

    // should be the same since string.c_str() adds "null"
    EXPECT_THAT(memcmp(cstring, coll.c_str(), 6+1), 0); 
  }

  {
    auto coll = "string"sv; // 'sv' is a string_view literal

    EXPECT_THAT(coll.size(), 6); 

    // should be the same since string_view.c_str() do NOT adds "null"
    //
    // https://en.cppreference.com/w/cpp/string/basic_string_view/data
    // Notes
    // Unlike std::basic_string::data() and string literals, data() may return a
    // pointer to a buffer that is not null-terminated. Therefore it is typically a
    // mistake to pass data() to a routine that takes just a const CharT* and expects a
    // null-terminated string.

    // EXPECT_THAT(memcmp(cstring, coll.data(), 6 + 1), Ne(0));
    // even if it returns 0, but it's undefined.
    EXPECT_THAT(memcmp(cstring, coll.data(), 6 + 1), 0);
  }

  // so should convert it to std::string
  {
    auto coll = "string"sv; // 'sv' is a string_view literal

    EXPECT_THAT(strlen(std::string(coll).c_str()), 6); 
  }
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
