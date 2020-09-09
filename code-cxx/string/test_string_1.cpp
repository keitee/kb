#include <bitset>
#include <boost/algorithm/string.hpp>
#include <boost/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <cstring>
#include <fstream>
#include <iostream>
#include <regex>
#include <vector>

#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using testing::ElementsAre;
using testing::Eq;
using testing::FloatEq;
using testing::Ne;
using testing::StrEq;

// ={=========================================================================
// *cxx-string-ctor*

TEST(StringCtor, check_various_form)
{
  {
    std::string coll{"12345"};

    EXPECT_EQ(coll.size(), 5);
    EXPECT_EQ(coll.length(), 5);

    // this is error since iterator is not int
    // EXPECT_EQ(coll.end(), 5);

    EXPECT_EQ(*coll.end(), '\0');
  }

  {
    std::string s("nico");
    EXPECT_THAT(s.length(), 4);
  }

  // ascii code 48 is '0'
  {
    std::string s{48};
    EXPECT_THAT(s.size(), 1);
    EXPECT_THAT(s, ElementsAre('0'));
  }

  {
    std::string s(5, 'c');
    EXPECT_THAT(s, ElementsAre('c', 'c', 'c', 'c', 'c'));
  }

  {
    std::string s(5, 'c');
    EXPECT_THAT(s, "ccccc");
  }

  {
    std::string s{"zoo"};
    EXPECT_EQ(s, "zoo");
  }

  {
    std::string s("zoo");
    EXPECT_EQ(s, "zoo");
  }

  {
    std::string coll{};
    const char *message = "this is message";

    // error: invalid operands of types ‘const char [17]’ and ‘const char*’ to binary ‘operator+’
    // coll += "std::string and " + message;
    coll += "std::string and " + std::string(message);
  }
}

// ={=========================================================================
TEST(StringCtor, check_const)
{
  {
    // /** *cxx-cstring*
    //  *  @brief  Construct string as copy of a C string.
    //  *  @param  __s  Source C string.
    //  *  @param  __a  Allocator to use (default is default allocator).
    //  */
    // basic_string(`const` _CharT* __s, const _Alloc& __a = _Alloc());

    // not a problem to construct string from const char*
    // const char s1[] = "this is first message";
    // const char *s2 = "this is first message";

    // both cause error
    //
    // t_string.cpp:29:49: error: invalid conversion from ‘char’ to ‘const
    // char*’ [-fpermissive]
    //
    // const char const_letter = 'A';
    // string string_from_const_letter(const_letter);
    //
    // char non_const_letter = 'A';
    // string string_from_non_const_letter(non_const_letter);
  }

  // *cxx-string-const-ctor-const*

  // ctor and string-append cause error:
  //
  // {
  //   char letter = 'A';
  //
  //   // string.cpp:99:20: error: invalid conversion from ‘char’ to ‘const
  //   // char*’ [-fpermissive]
  //   //      string s(letter);
  //
  //   string s(letter);
  // }
  //
  // {
  //   char letter = 'A';
  //
  //   // string.cpp:99:20: error: invalid conversion from ‘char’ to ‘const
  //   // char*’ [-fpermissive]
  //   //         s.append(letter);
  //
  //   string s;
  //   s.append(letter);
  // }

  // HOW TO GET AROUND?

  // /**
  //  *  @brief  Append a character.
  //  *  @param __c  The character to append.
  //  *  @return  Reference to this string.
  //  */
  // basic_string&
  // operator+=(_CharT __c)
  // {
  //  this->push_back(__c);
  //  return *this;
  // }

  {
    string s;
    char letter = 'A';

    s += letter;

    EXPECT_EQ(s, "A");
  }

  {
    string s;
    char letter = 'A';

    s.append(1, letter);
    EXPECT_EQ(s, "A");
  }
}

#if 0

*os-coredump*

[ RUN      ] StringCtor.check_crash
unknown file: Failure
C++ exception with description "basic_string::_M_construct null not valid" thrown in the test body.
[  FAILED  ] StringCtor.check_crash (0 ms)

may wonder who will do this. However, it happens when making changes and caused
half a day to find out why.

was:

  struct DrmState
  {
    std::string state{};
    int euid{};
  };

DrmController::DrmController(PlaybackService *service)
    : m_state{AS_STATE_UNAVAILABLE, 0}
{
  ...
}

now, due to requirment change, have to make euid as string. so did that in code
but when runs it on the box, keep crashing while cannot see why and when all
changes looks okay. still see crash even when remove all codes changing euid.
Sadly, no backtrace or useful from core since it crashes very early.

the fix:

  struct DrmState
  {
    std::string state{};
    std::string euid{};
  };

DrmController::DrmController(PlaybackService *service)
    : m_state{AS_STATE_UNAVAILABLE, ""}
{
}

The problem is when init std::string with 0 which is left while makeing changes.
WOW.

#endif

// ={=========================================================================
TEST(StringCtor, check_crash)
{
  std::string s = 0;
}

// ={=========================================================================
// string-operations string-back

TEST(StringOperation, check_back)
{
  string s1{"zoo"};
  EXPECT_EQ(s1.back(), 'o');

  // this fails as the same reason as string ctors shown above.
  // EXPECT_EQ(string(s1.back()), "o");

  // /**
  //  *  @brief  Construct string as multiple characters.
  //  *  @param  __n  Number of characters.
  //  *  @param  __c  Character to use.
  //  *  @param  __a  Allocator to use (default is default allocator).
  //  */
  // basic_string(size_type __n, _CharT __c, const _Alloc& __a = _Alloc());

  EXPECT_EQ(string(1, s1.back()), "o");
}

// ={=========================================================================
// string-maxsize

TEST(StringOperation, check_max_size)
{
  string s{};

// #if __GNUC__ && __x86_64__
#if __GNUC__ && !__x86_64__
  EXPECT_THAT(s.max_size(), 4611686018427387897);
#else
  EXPECT_THAT(s.max_size(), 9223372036854775807);
#endif
}

// ={=========================================================================
TEST(StringOperation, check_resize)
{
  string s{};

  s.resize(1000);

  EXPECT_EQ(s.size(), 1000);
}

/*
={=========================================================================
string-substring string-find string-substr
cxx-srring-find
http://www.cplusplus.com/reference/string/string/find/

size_t find (const string& str, size_t pos = 0) const noexcept;
size_t find (const char* s, size_t pos, size_type n) const;

{
  /usr/include/c++/7/bits/basic_string.h

  // @brief  Find position of a string.
  // @param __str  String to locate.
  // @param __pos  Index of character to search from (default 0).
  // @return  Index of start of first occurrence.
  //
  // Starting from @a __pos, searches forward for value of @a __str within
  // this string.  If found, returns the index where it begins.  If not
  // found, returns npos.

  size_type
    find(const basic_string& __str, size_type __pos = 0) const
    { return this->find(__str.data(), __pos, __str.size()); }

  /usr/include/c++/7/bits/basic_string.tcc

  template<typename _CharT, typename _Traits, typename _Alloc>
    typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::
    find(const _CharT* __s, size_type __pos, size_type __n) const
    {
      const size_type __size = this->size();

      // when size of sub, __n, is null. why need this?
      if (__n == 0)
        return __pos <= __size ? __pos : npos;

      if (__pos >= __size)
        return npos;

      const _CharT __elem0 = __s[0];
      const _CharT* const __data = data();
      const _CharT* __first = __data + __pos;
      const _CharT* const __last = __data + __size;

      size_type __len = __size - __pos;

      while (__len >= __n)
      {
        // Find the first occurrence of __elem0:
        __first = traits_type::find(__first, __len - __n + 1, __elem0);
        if (!__first)
          return npos;

        // Compare the full strings from the first occurrence of __elem0.
        // We already know that __first[0] == __s[0] but compare them again
        // anyway because __s is probably aligned, which helps memcmp.
        if (traits_type::compare(__first, __s, __n) == 0)
          return __first - __data;

        __len = __last - ++__first;
      }

      return npos;
    }
}

*/

namespace string_find
{
  std::string::size_type my_find(const std::string &str,
                                 const std::string &sub,
                                 std::string::size_type pos = std::string::npos)
  {
    const auto size_string_ = str.size();
    const auto size_sub_    = sub.size();

    if (size_sub_ <= size_string_)
    {
      const char *string_ = str.c_str();

      auto limit = size_string_ - size_sub_;

      // when not support `pos`
      // std::string::size_type pos{};

      // same as rfind
      pos = std::min((size_string_ - size_sub_), pos);

      do
      {
        // when found
        if (memcmp(string_ + pos, sub.c_str(), size_sub_) == 0)
          return pos;
      } while (pos++ < limit);
    }

    return std::string::npos;
  }
} // namespace string_find

TEST(StringOperation, check_find_char)
{
  std::string coll{"There are two needles in this haystack with needles."};

  {
    // Finds the first character equal to "one of the characters" in the given
    // character sequence. The search considers only the interval [pos, size()).
    //
    // If the character is not present in the interval, "npos" will be
    // returned.Finds the first character equal to one of the characters in the
    // given character sequence. The search considers only the interval [pos,
    // size()). If the character is not present in the interval, npos will be
    // returned.
    //
    // size_type find_first_of( const basic_string& str, size_type pos = 0 )
    // const;

    auto pos = coll.find_first_of("n");
    EXPECT_THAT(pos, 14);
  }

  {
    auto pos = coll.find_last_of("n");
    EXPECT_THAT(pos, 44);
  }

  // "one of the characters"
  {
    auto pos = coll.find_first_of("ne");
    EXPECT_THAT(pos, 2);
  }

  {
    // size_type find( const basic_string& str, size_type pos = 0 ) const;

    auto pos = coll.find("n");
    EXPECT_THAT(pos, 14);
  }

  // strchr, strrchr, strchrnul - locate character in string
  // DESCRIPTION
  // The strchr() function returns a pointer to the first occurrence of the
  // character c in the string s.
  {
    auto ptr = strchr(coll.c_str(), 'n');
    EXPECT_THAT(ptr - coll.c_str(), 14);
  }
}

// ={=========================================================================
TEST(StringOperation, check_find_substring_1)
{
  // try to match "V" from "IV" only for one char so expects no match. However,
  // that's not the case.
  {
    std::string coll1{"IV"};
    std::string coll2{"V"};

    // find sill search
    EXPECT_THAT(coll1.find(coll2), 1);

    // size_type find( const CharT* s, size_type pos, size_type count ) const;
    // 2) Finds the first substring equal to the range [s, s+count). This range
    // may contain null characters. NOTE on "s+count"

    EXPECT_THAT(coll1.find(coll2.c_str(), 0, coll2.size()), 1);
    EXPECT_THAT(coll1.find(coll2.c_str(), 0, coll2.size() - 1), 0);

    EXPECT_THAT(coll1.compare(0, 1, coll2.c_str()), Ne(0));
  }

  // strstr, strcasestr - locate a substring
  //
  // #include <string.h>
  // char *strstr(const char *haystack, const char *needle);
  // char *strcasestr(const char *haystack, const char *needle);
  //
  // DESCRIPTION
  // The strstr() function finds the first occurrence of the substring needle in
  // the string haystack.  The terminating null bytes ('\0') are not compared.
  //
  // The strcasestr() function is like strstr(), but ignores the case of both
  // arguments.
  {
    std::string coll1{"There are two needles in this haystack with needles."};
    std::string coll2{"needle"};

    // first match
    auto found = std::strstr(coll1.c_str(), coll2.c_str());

    EXPECT_THAT((found - coll1.c_str()), 14);
  }

  {
    std::string coll1{"There are two needles in this haystack with needles."};
    std::string coll2{"needle"};

    // first match
    std::size_t found = coll1.find(coll2);
    EXPECT_THAT(found, 14);

    // start from `found+1` and for 6 chars long of "needles are small"
    found = coll1.find("needles are small", found + 1, 6);
    EXPECT_THAT(found, 44);

    found = coll1.find("haystack");
    EXPECT_THAT(found, 30);

    // not found
    found = coll1.find("haystack", 31);
    EXPECT_THAT(found, std::string::npos);

    found = coll1.find('.');
    EXPECT_THAT(found, 51);

    found = coll1.find("park");
    EXPECT_THAT(found, std::string::npos);

    // since npos is unsigned
    found = coll1.find("park");
    EXPECT_THAT(found, -1);
  }

  {
    using namespace string_find;

    std::string coll1{"There are two needles in this haystack with needles."};
    std::string coll2{"needle"};

    // first match
    std::size_t found = my_find(coll1, coll2, 0);
    EXPECT_THAT(found, 14);

    found = my_find(coll1, "haystack", 0);
    EXPECT_THAT(found, 30);

    // not found
    found = my_find(coll1, "haystack", 31);
    EXPECT_THAT(found, std::string::npos);

    found = my_find(coll1, ".", 0);
    EXPECT_THAT(found, 51);

    found = my_find(coll1, "park", 0);
    EXPECT_THAT(found, string::npos);

    // since npos is unsigned
    found = my_find(coll1, "park", 0);
    EXPECT_THAT(found, -1);
  }
}

// ={=========================================================================
// (gdb) b CxxStringOperation_find_substring2_Test::TestBody()
// cxx-string-rfind
TEST(CxxStringOperation, find_substring_2)
{
  {
    std::string coll1{"The sixth sick sheik's sixth sheep's sick."};
    std::string coll2{"sixth"};
    std::string coll3{"The sixth sick sheik's seventh sheep's sick."};

    // search from the end
    auto found = coll1.rfind(coll2);
    coll1.replace(found, coll2.length(), "seventh");

    // now coll1 == coll3
    EXPECT_THAT(coll1, coll3);

    // not found
    found = coll1.rfind(coll2, 0);
    EXPECT_THAT(found, std::string::npos);

    // "starts from 0 and see match single char 'T' at 0. returns 0
    // that is inclusive"
    // THIS description is WRONG see find_substring3
    found = coll1.rfind("The", 0);
    EXPECT_THAT(found, 0);

    // starts from 2 to backwards and see match 'The'. returns 0 since 0 is the
    // first char
    found = coll1.rfind("The", 2);
    EXPECT_THAT(found, 0);

    found = coll1.rfind("The", 3);
    EXPECT_THAT(found, 0);

    found = coll1.rfind("The", 4);
    EXPECT_THAT(found, 0);

    found = coll1.rfind("The", 6);
    EXPECT_THAT(found, 0);

    // starts from 0 and see match 's'
    found = coll1.rfind("sixth", 0);
    EXPECT_THAT(found, std::string::npos);

    found = coll1.rfind("sixth", 4);
    EXPECT_THAT(found, 4);

    found = coll1.rfind("sixth", 5);
    EXPECT_THAT(found, 4);

    found = coll1.rfind("sixth", 6);
    EXPECT_THAT(found, 4);
  }

  {
    std::string const s = "This is a string";
    // search backwards from position 4
    auto found = s.rfind("is", 4);
    EXPECT_THAT(found, 2);
  }

  {
    const std::string request1{"/as/players/2/action/watchlive"};
    const std::string request2{"/as/players/1/action/watchlive"};

    const std::string s = "/as/players/2/";

    // starts from 0
    EXPECT_THAT(request1.rfind(s, 0), 0);
    // search whole
    EXPECT_THAT(request1.rfind(s), 0);

    // starts from 0 ane expects `0 return` but not.
    // EXPECT_THAT(request2.rfind(s, 0), 0);
    // NOTE: WHY and this leads to find_substring3()

    // starts from 0
    EXPECT_THAT(request2.rfind(s, 0), std::string::npos);
    // search whole
    EXPECT_THAT(request2.rfind(s), std::string::npos);
  }

  // when both has the same size but different
  {
    const std::string request1{"/as/players/2/action/watchlive"};
    const std::string request2{"/as/players/1/action/watchlive"};

    EXPECT_THAT(request1.rfind(request2, 0), std::string::npos);
  }
}

/*

/usr/include/c++/7/bits/basic_string.h

@brief  Find last position of a string.
@param __str  String to locate.
@param __pos  Index of character to search back from (default end).
@return  Index of start of last occurrence.

Starting from @a __pos, searches backward for value of @a
__str within this string.  If found, returns the index where
it begins.  If not found, returns npos.

size_type
rfind(const basic_string& __str, size_type __pos = npos) const
_GLIBCXX_NOEXCEPT
{ return this->rfind(__str.data(), __pos, __str.size()); }


/usr/include/c++/7/bits/basic_string.tcc

template<typename _CharT, typename _Traits, typename _Alloc>
typename basic_string<_CharT, _Traits, _Alloc>::size_type
basic_string<_CharT, _Traits, _Alloc>::
rfind(const _CharT* __s, size_type __pos, size_type __n) const
{
  // __size is size of string to search
  const size_type __size = this->size();

  // __n is size of sub-string
  if (__n <= __size)
  {
    __pos = std::min(size_type(__size - __n), __pos);

    const _CharT* __data = _M_data();

    do
    {
      if (traits_type::compare(__data + __pos, __s, __n) == 0)
        return __pos;
    }
    while (__pos-- > 0);
  }
  return npos;
}

/usr/include/c++/7/bits/char_traits.h

static _GLIBCXX17_CONSTEXPR int
compare(const char_type* __s1, const char_type* __s2, size_t __n)
{
  if (__n == 0)
    return 0;
  return __builtin_memcmp(__s1, __s2, __n);
}


cxx-srring-rfind find backwards
https://en.cppreference.com/w/cpp/string/basic_string/rfind

Finds the `last substring` equal to the given character sequence. Search
begins at pos, i.e. the found substring must not begin in a position
following pos.

Return value
Position of the first character of the found substring or `npos` if no such
substring is found.

*/

namespace
{
  std::string::size_type
  my_rfind(const std::string &str,
           const std::string &sub,
           std::string::size_type pos = std::string::npos)
  {
    // get size of the string to be searched
    const std::string::size_type str_size_ = str.size();

    // get size of the string to search
    const std::string::size_type sub_size_ = sub.size();

    // size of string to search shall not bigger than one of to be searched
    if (sub_size_ <= str_size_)
    {
      // use of min() to support `pos` to start. if not support pos, then no
      // need to use min. always starts from the end and use str_size-stb_size.
      //
      // get new pos(starting point) from input pos
      //
      // when sub string is small
      //
      // {0,1,2,3,4,5,6,7,8,9}
      // {0,1,2}
      // 3     , 7            , when pos = 5, min(7,5) is 5
      // 3     , 7            , when pos = 9, min(7,9) is 7
      // 3     , 7            , when pos = 1, min(7,1) is 1
      // 3     , 7            , when pos = 0, min(7,0) is 0
      //
      // wherever input pos is, there is no risk of comparing beyond the end
      // of input since the size of sub string is already taken out and only
      // min() of them is valid. that is, new pos is within 7.
      //
      // when sub string is large
      //
      // {0,1,2,3,4,5,6,7,8,9}
      // {0,1,2,3,4,5,6}
      // 6     , 3            , when pos = 5, min(3,5) is 3
      // 6     , 3            , when pos = 9, min(3,9) is 3
      // 6     , 3            , when pos = 1, min(3,1) is 1
      // 6     , 3            , when pos = 0, min(3,0) is 0
      //
      // since size of sub string is 6 and new pos should leave enough space
      // to compare
      //
      // this is why memcmp is used without worry to compare beyond the end
      //
      // the point is there should be enough space for comparing sub string.

      pos = std::min((str_size_ - sub_size_), pos);

      const char *data_ = str.c_str();

      // use of input arg `pos` rather than defining local one.
      // NOTE:
      // 1. use of do/while and `pos-- > 0` to compare up to index 0.
      // 2. if use while() instead, do not cover when pos is 0; str and sub is
      // equal.
      // 3. when pos is 0, pos-- makes UNIT_MAX but while() use the previous
      // value which is 0.

      do
      {
        // if sub string is found
        if (memcmp(data_ + pos, sub.c_str(), sub_size_) == 0)
          return pos;
      } while (pos-- > 0);
    }

    // can use `pos` but use npos to make explicit.
    // 1. when not found
    // 2. when sub size is bigger than str size
    return std::string::npos;
  }
} // namespace

// ={=========================================================================
TEST(CxxStringOperation, find_substring_3)
{
  {
    std::string coll2{"sixth"};
    std::string coll3{"The sixth sick sheik's sixth sheep's sick."};

    // not found
    auto found = my_rfind(coll3, coll2, 0);
    EXPECT_THAT(found, std::string::npos);

    // found
    found = my_rfind(coll3, "The", 0);
    EXPECT_THAT(found, 0);

    // found from the end.
    found = my_rfind(coll3, coll2);
    EXPECT_THAT(found, 23);
  }

  {
    const std::string request1{"/as/players/2/action/watchlive"};
    const std::string request2{"/as/players/1/action/watchlive"};

    const std::string s = "/as/players/2/";

    // starts from 0
    EXPECT_THAT(my_rfind(request1, s, 0), 0);

    // starts from 0
    EXPECT_THAT(my_rfind(request2, s, 0), std::string::npos);
  }

  // when both has the same size but different
  {
    const std::string request1{"/as/players/2/action/watchlive"};
    const std::string request2{"/as/players/1/action/watchlive"};

    EXPECT_THAT(my_rfind(request1, request2, 0), std::string::npos);
  }
}

// ={=========================================================================
// cxx-string-substr
// is to extract substring and use index
TEST(StringOperation, check_substr)
{
  std::string coll{"interchangeability"};

  // see `cxx-string-length` returns length which is [0, length)
  EXPECT_THAT(coll.length(), 18);

  // return whole string
  EXPECT_THAT(coll.substr(), "interchangeability");

  // return whole string as find('x') returns string::npos
  EXPECT_THAT(coll.substr(0, coll.find('x')), "interchangeability");

  EXPECT_THAT(coll.substr(coll.find('c')), "changeability");

  // [11, end)
  EXPECT_THAT(coll.substr(11), "ability");

  // note that the second is `length`. (start, length)
  EXPECT_THAT(coll.substr(5, 6), "change");

  EXPECT_THAT(coll.substr(0, 0), "");
}

// ={=========================================================================
// string-add-char

TEST(StringOperation, check_add_char)
{
  // cxx-string-push-back
  // void push_back( CharT ch ); (until C++20)
  {
    std::string coll{};

    coll.push_back('s');
    coll.push_back('t');
    coll.push_back('r');

    EXPECT_THAT(coll, "str");
  }

  // no
  // coll.push_back("s");

  // cxx-string-insert
  // string& string::insert (size_type idx, size_type num, char c);
  {
    std::string coll{};

    coll.insert(0, 1, 's');
    coll.insert(0, 1, 't');
    coll.insert(0, 1, 'r');

    EXPECT_EQ(coll, "rts");
  }

  // iterator insert( iterator pos, CharT ch ); (until C++11)
  {
    std::string coll{};

    coll.insert(coll.end(), 's');
    coll.insert(coll.end(), 't');
    coll.insert(coll.end(), 'r');

    EXPECT_EQ(coll, "str");
  }

  // basic_string& insert( size_type index, const basic_string& str );
  // NOTE:
  // coll.insert(pos, ) uses iterator for pos
  {
    std::string coll{};

    coll.insert(0, "s");
    coll.insert(0, "t");
    coll.insert(0, "r");

    EXPECT_EQ(coll, "rts");
  }

  // cxx-string-append
  {
    std::string coll{};

    coll += 's';
    coll += 't';
    coll += 'r';

    EXPECT_EQ(coll, "str");
  }

  // basic_string& append( size_type count, CharT ch ); (until C++20)
  // constexpr basic_string& append( size_type count, CharT ch ); (since C++20)
  {
    std::string coll{};

    coll.append(1, 's');
    coll.append(1, 't');
    coll.append(1, 'r');

    EXPECT_EQ(coll, "str");
  }

  {
    std::string coll{};

    coll.append("s");
    coll.append("t");
    coll.append("r");

    EXPECT_EQ(coll, "str");
  }
}

// ={=========================================================================
// string-erase, string-replace

TEST(StringOperation, EraseReplace)
{
  {
    std::string s = "i18n";

    s.replace(1, 2, "nternationalizatio"); // change of size
    EXPECT_EQ(s, "internationalization");

    s.erase(13); // s: international. note: from 13
    EXPECT_EQ(s, "international");

    s.erase(7, 5); // internaxxxxxl
    EXPECT_EQ(s, "internal");

    // *cxx-string-pop-back*
    // void string::pop_back ()
    // Calling this for an empty string results in undefined behavior.

    s.pop_back(); // s: interna (since C++11)
    EXPECT_EQ(s, "interna");

    s.replace(0, 2, "ex"); // s: externa
    EXPECT_EQ(s, "externa");
  }

  {
    std::string s = "i18n";

    s.replace(1, 2, "nternationalizatio"); // change of size
    EXPECT_EQ(s, "internationalization");

    // use iterator? No, all string find function returns index.
    s.erase(s.find('l'));
    EXPECT_EQ(s, "internationa");
  }

  // when use erase(iterator), it erases only one element.
  {
    std::string s{"abcde"};

    auto it = s.begin();
    ++it;
    s.erase(it);
    EXPECT_THAT(s, "acde");

    s.erase(it);
    EXPECT_THAT(s, "ade");
  }

  {
    // erase(size_type idx, size_type len) removes chars and concatenate them.

    std::string s{"internationalization"};
    s.erase(5, 9);
    EXPECT_EQ(s, "interzation");
  }

  {
    // the replacement string need not be the same size as the substring that
    // it is replacing. cxx-string-replace

    string s = "Niels Stroustrup";
    s.replace(0, 5, "nicholas");
    EXPECT_EQ(s, "nicholas Stroustrup");
  }
}

// cxx-string-assign

TEST(CxxString, check_assign)
{
  {
    std::string coll;
    coll = "assign string";
    EXPECT_THAT(coll, "assign string");
  }

  {
    const std::string coll{"othello"};
    std::string s{};

    // assign "othello" (equivalent to operator =)
    s.assign(coll);
    EXPECT_EQ(s, "othello");

    s.assign(coll, 1, 3);
    EXPECT_EQ(s, "the");

    s.assign(coll, 2, std::string::npos);
    EXPECT_EQ(s, "hello");
  }

  {
    std::string s{};

    s.assign("othello");
    EXPECT_EQ(s, "othello");

    s.assign("the");
    EXPECT_EQ(s, "the");
  }
}

// cxx-string-swap

TEST(CxxString, Swap)
{
  {
    std::string coll{"othello"};
    std::string s{};

    // assign "othello" (equivalent to operator =)
    s.swap(coll);
    EXPECT_EQ(s, "othello");

    // compile error
    // s.swap("the");
    // s.swap(string("the"));

    string str{"the"};
    s.swap(str);
    EXPECT_EQ(s, "the");
  }
}

// string-compare
//
// int compare( const basic_string& str ) const; (until C++11)
// int compare( const basic_string& str ) const noexcept; (since C++11)
//
// Return value
// negative value if `*this` appears before the character sequence specified by
// the arguments, in lexicographical order
//
// zero if both character sequences compare equivalent
//
// positive value if *this appears after the character sequence specified by the
// arguments, in lexicographical order
//
// int compare(size_type pos1, size_type count1, const basic_string& str ) const;

TEST(StringCompare, check_member_function)
{
  {
    std::string coll1{"string compare"};
    std::string coll2{"string compare"};

    // equal
    EXPECT_THAT(0, coll1.compare(coll2));
  }

  {
    std::string coll1{"string compare"};
    std::string coll2{"string compare"};

    // equal
    EXPECT_THAT(true, (coll1 == coll2));
  }

  {
    std::string coll1{};
    std::string coll2{};

    // equal
    EXPECT_THAT(true, (coll1 == coll2));
  }

  // no simple positive/negative value in lexicographical order but difference?
  {
    std::string coll1{"help"};
    std::string coll2{"helphelp"};
    std::string coll3{"h"};

    // help and h
    EXPECT_THAT(coll1.compare("h"), 3);

    // helphelp and h
    EXPECT_THAT(coll2.compare("h"), 7);

    // h and help
    EXPECT_THAT(coll3.compare("help"), -3);
  }

  {
    std::string state{"NotActive"};

    // positive value if *this appears after the character sequence specified by the
    // arguments, in lexicographical order
    // as seen cxx-string, it's confusing to check the return value
    EXPECT_THAT(state.compare({"Active"}), 1);
    EXPECT_THAT(state.compare({"NotAvailable"}), -19);

    // however, other than 0, regarded as `true` in bool
    EXPECT_THAT(!!state.compare({"Active"}), true);
    EXPECT_THAT(!!state.compare({"NotAvailable"}), true);
  }

  // int compare( size_type pos1, size_type count1, const CharT* s ) const;
  // 5) Compares a [pos1, pos1+count1) substring of this string to the
  // null-terminated character sequence beginning at the character pointed to by
  // s with length Traits::length(s) If count1 > size() - pos1 the substring is
  // [pos1, size()).

  {
    std::string coll1{"help"};
    EXPECT_THAT(coll1.compare(0, 1, "h"), 0);
    EXPECT_THAT(coll1.compare(0, 2, "h"), 1);
    EXPECT_THAT(coll1.compare(0, 3, "h"), 2);
    EXPECT_THAT(coll1.compare(0, 4, "h"), 3);
  }
}

// ={=========================================================================
// 4.13 Doing a Case-Insensitive String Comparison

namespace stringcompare
{
  bool caseInsCompare1(const string &s1, const string &s2)
  {
    if (s1.size() != s2.size())
      return false;

    for (auto lhs = s1.cbegin(), rhs = s2.cbegin(); lhs != s1.cend();
         ++lhs, ++rhs)
      if (toupper(*lhs) != toupper(*rhs))
        return false;

    return true;
  }
} // namespace stringcompare

TEST(StringCompare, check_no_case)
{
  using namespace stringcompare;

  const string s1 = "In the BEGINNING...";
  const string s2 = "In the beginning...";

  EXPECT_TRUE(caseInsCompare1(s1, s2));
}

#if 0
// ={=========================================================================
// *cxx-string-trait*

bits/char_traits.h

  /**
   *  @brief  Base class used to implement std::char_traits.
   *
   *  @note For any given actual character type, this definition is
   *  probably wrong.  (Most of the member functions are likely to be
   *  right, but the int_type and state_type typedefs, and the eof()
   *  member function, are likely to be wrong.)  The reason this class
   *  exists is so users can specialize it.  Classes in namespace std
   *  may not be specialized for fundamental types, but classes in
   *  namespace __gnu_cxx may be.
   *
   *  See https://gcc.gnu.org/onlinedocs/libstdc++/manual/strings.html#strings.string.character_types
   *  for advice on how to make use of this class for @a unusual character
   *  types. Also, check out include/ext/pod_char_traits.h.  
   */

  template<typename _CharT>
    struct char_traits
  {
    template<typename _CharT>
      _GLIBCXX14_CONSTEXPR int
      char_traits<_CharT>::
      compare(const char_type* __s1, const char_type* __s2, std::size_t __n)
      {
        for (std::size_t __i = 0; __i < __n; ++__i)
          if (lt(__s1[__i], __s2[__i]))
            return -1;
          else if (lt(__s2[__i], __s1[__i]))
            return 1;
        return 0;
      }
  }

o all functions of char_traits are "static" and it is "stateless" class.

o Traits of basic_string should have all functions of char_traits so the easiest
  way is to derive from char_traits and implements only necessaries in the
  derived.

#endif

namespace stringtraits
{
  struct my_char_traits : public std::char_traits<char>
  {
    // make char higner if they are numbers(digits)
    static int get_real_rank(char c)
    {
      if (isdigit(c))
      {
        return c + 256;
      }

      return c;
    }

    // return true if c1 < c2
    static bool lt(char c1, char c2)
    {
      return get_real_rank(c1) < get_real_rank(c2);
    }

    // this works
    // static int compare(const char *s1, const char *s2, size_t n)
    // {
    //   while (n-- != 0)
    //   {
    //     if (get_real_rank(*s1) < get_real_rank(*s2))
    //     {
    //       return -1;
    //     }

    //     if (get_real_rank(*s1) > get_real_rank(*s2))
    //     {
    //       return 1;
    //     }

    //     ++s1;
    //     ++s2;
    //   }

    //   return 0;
    // }

    // if follows the stl code
    static int compare(const char *s1, const char *s2, size_t n)
    {
      for (std::size_t i = 0; i < n; ++i)
      {
        if (get_real_rank(s1[i]) < get_real_rank(s2[i]))
        {
          return -1;
        }
        else if (get_real_rank(s1[i]) > get_real_rank(s2[i]))
        {
          return 1;
        }
      }
      return 0;
    }
  };
} // namespace stringtraits

TEST(StringCompare, check_traits)
{
  {
    std::string s1{"1a"};
    std::string s2{"a1"};

    // std::cout << "usual string compare : " << std::boolalpha
    // << (s1 < s2) << std::endl;
    //
    // since "numbers" are lower, before, than "alphabets"
    EXPECT_THAT((s1 < s2), true);
  }

  // How can we do the opposite? that is, "numbers" are higher
  {
    using namespace stringtraits;

    std::basic_string<char, my_char_traits> s1{"1a"};
    std::basic_string<char, my_char_traits> s2{"a1"};

    EXPECT_THAT((s1 < s2), false);
  }
}

// cxx-string-clear
//
// void clear(); (until C++11)
// void clear() noexcept;

TEST(String, StringClear)
{
  // it is safe to call clear() on empty coll
  {
    std::string coll{};

    EXPECT_THAT(coll.empty(), true);

    coll.clear();
    coll.clear();
    coll.clear();

    EXPECT_THAT(coll.size(), 0);
  }

  {
    std::string coll{};

    coll = "string add";

    EXPECT_THAT(coll.size(), 10);

    coll.clear();

    EXPECT_THAT(coll.size(), 0);
  }
}

// ={=========================================================================
// string-cstring

// check null char, '\0', which is actually integer 0.
TEST(StringForC, check_null)
{
  {
    std::ostringstream os;

    // in order to get hex value for char 'a', which is 61, have to use (int)
    // cast.
    //
    // std::cout << "a: " << hex << (int)'a' << std::endl;
    // gets "61"
    //
    // std::cout << "a: " << hex << 'a' << std::endl;
    // gets "a"

    os << std::hex << (int)'0';
    EXPECT_THAT(os.str(), "30");

    // reset os
    os.str("");

    os << std::hex << (int)'\0';
    EXPECT_THAT(os.str(), "0");
  }

  // nullptr is not same
  {
    const char *p1 = "";
    const char *p2 = nullptr;

    EXPECT_NE(p1, p2);
  }
}

TEST(StringForC, check_cstring_1)
{
  {
    // std::string do not include a null
    std::ostringstream os;
    std::string sz{"zoo"};

    EXPECT_EQ(sz.size(), 3);

    // c_str() includes null
    EXPECT_EQ(strlen(sz.c_str()), 3);

    // see char
    //
    // Expected: is equal to "zoo"
    //   Actual: "zoo\0"
    //
    // for (size_t i = 0; i < 4; ++i)
    //   os << std::hex << sz.c_str()[i];
    // EXPECT_THAT(os.str(), "zoo\0");
    //
    // HOW to make std::string with "\"?

    os.str("");

    // see ascii hex
    for (size_t i = 0; i < 4; ++i)
      os << std::hex << (int)sz.c_str()[i];
    EXPECT_THAT(os.str(), "7a6f6f0");
  }

  {
    // cstring include a null
    const char cs[] = "zoo";
    EXPECT_EQ(strlen(cs), 3); // excludes null
    EXPECT_EQ(sizeof(cs), 4); // +1 for null
    EXPECT_EQ(sizeof(cs) / sizeof(char), 4);
  }

  {
    // there is std::string ctor(const char*)
    const char s1[] = "0123456789012345";
    std::string coll{s1};

    EXPECT_THAT(sizeof(s1), 17);  // +1 for null
    EXPECT_THAT(strlen(s1), 16);  // not include null
    EXPECT_THAT(coll.size(), 16); // not include null

    // c_str() include a null? yes but note that size() do not include a null.
    EXPECT_THAT(strlen(coll.c_str()), 16);
    EXPECT_THAT(coll.size(), 16);

    // int memcmp(const void *s1, const void *s2, size_t n);
    EXPECT_THAT(memcmp(s1, coll.c_str(), 17), 0);
    EXPECT_THAT(memcmp(s1, coll.c_str(), coll.size() + 1), 0);
  }
}

// when construct a string from array, it will copy up to null? yes
TEST(StringForC, check_cstring_2)
{
  {
    // src_len is 14 which includes a null since std::string.data()
    //                "0123456789012"
    std::string input{"&streamtype=1"};
    char output[256] = {0};

    EXPECT_THAT(input.size(), 13);
    EXPECT_THAT(strlen(input.c_str()), 13);

    EXPECT_THAT(sizeof(output), 256);

    // char *strcpy(char *dest, const char *src);
    // which understand a cstring.
    strcpy(output, input.c_str());

    EXPECT_THAT(sizeof(output), 256);

    // size of result?
    std::string result{output};

    EXPECT_THAT(input, result);

    EXPECT_THAT(result.size(), 13);
    EXPECT_THAT(strlen(result.c_str()), 13);
  }

  {
    //                "0123456789012"
    std::string input{"&streamtype=1"};
    char output[256] = {0};

    EXPECT_THAT(input.size(), 13);
    EXPECT_THAT(strlen(input.c_str()), 13);

    EXPECT_THAT(sizeof(output), 256);

    // char *strcpy(char *dest, const char *src);
    // which understand a cstring.
    strcpy(output, input.c_str());

    EXPECT_THAT(sizeof(output), 256);

    // when use stringstream, it has the same result as using std::string
    std::ostringstream oss{};

    oss << output;

    EXPECT_THAT(input, oss.str());

    EXPECT_THAT(oss.str().size(), 13);
    EXPECT_THAT(strlen(oss.str().c_str()), 13);
  }
}

// return bool if source and target are the same from the end.
// bool strend(char *s, char *t);

namespace stringcstring
{
  bool strend_01(char *s, char *t)
  {
    // assume s is bigger than t
    size_t diff = strlen(s) - strlen(t);

    char *psource = s + diff, *ptarget = t;
    while (*psource)
    {
      if (*psource != *ptarget)
        return false;

      ++psource, ++ptarget;
    }

    return true;
  }

  bool strend_02(char *s, char *t)
  {
    // assume s is bigger than t. uses parameter
    s += strlen(s) - strlen(t);

    while (*s++ == *t++)
    {
      // if side-effect, +1, points NULL then all matches up.
      if (*s == '\0')
        return true;
    }

    return false;
  }
} // namespace stringcstring

// strend(this is first message, ssage) returns 1
// strend(this is first message, xsage) returns 0
// strend(this is first message, ssage) returns 1
// strend(this is first message, xsage) returns 0

TEST(StringForC, check_strend)
{
  using namespace stringcstring;

  char s1[] = "this is first message";
  char t1[] = "ssage";
  char t2[] = "xsage";

  EXPECT_EQ(true, strend_01(s1, t1));
  EXPECT_EQ(false, strend_01(s1, t2));

  EXPECT_EQ(true, strend_02(s1, t1));
  EXPECT_EQ(false, strend_02(s1, t2));
}

/* 
={=========================================================================
string-conversion

Defined in header <string>

int       stoi( const std::string& str, std::size_t* pos = 0, int base = 10 );
int       stoi( const std::wstring& str, std::size_t* pos = 0, int base = 10 );
(1)	(since C++11)

long      stol( const std::string& str, std::size_t* pos = 0, int base = 10 );
long      stol( const std::wstring& str, std::size_t* pos = 0, int base = 10 );
(2)	(since C++11)

long long stoll( const std::string& str, std::size_t* pos = 0, int base = 10 );
long long stoll( const std::wstring& str, std::size_t* pos = 0, int base = 10 );
(3)	(since C++11)

*/

TEST(StringConverison, check_functions)
{
  // std::string to_string( int value );
  // std::string to_string( long value ); ...
  {
    EXPECT_THAT(std::to_string(11), "11");
    EXPECT_THAT(std::to_string(3301), "3301");
  }

  // to string in hex
  {
    char buf[10];
    int value{16};
    snprintf(buf, sizeof(buf), "0x%x", value);
    EXPECT_THAT(std::string(buf), "0x10");
  }

  // from string to x
  {
    // stl functions
    EXPECT_EQ(std::stoi("  77"), 77);
    EXPECT_EQ(std::stoi("  77.7"), 77);

    // std::stoi("-0x77") yields 0 because it parses only -0, interpreting
    // the x as the end of the numeric value found.
    EXPECT_EQ(std::stoi("-0x77"), 0);

    size_t idx{};

    EXPECT_EQ(std::stoi("  42 is the truth", &idx), 42);
    // idx of first unprocessed char
    EXPECT_EQ(idx, 4);

    // use base 16 and 8. 0x42 = 66
    EXPECT_EQ(std::stoi("  42", nullptr, 16), 66) << endl;

    // std::stol("789", &idx, 8) parses only the first character of the string
    // because 8 is not a valid character for octal numbers.

    EXPECT_EQ(std::stoi("789", &idx, 8), 7);
    // idx of first unprocessed char
    EXPECT_EQ(idx, 1);

    // man strtol()
    //
    // #include <stdlib.h>
    // long int strtol(const char *nptr, char **endptr, int base);
    // long long int strtoll(const char *nptr, char **endptr, int base);
    //
    // The string may begin with an arbitrary amount of white space (as deter‐
    // mined by isspace(3)) followed by a single optional '+' or '-' sign.
    //
    // If base is zero or 16, the string may then include a "0x" prefix, and
    // the number will  be read in base 16;

    EXPECT_EQ(std::stol("0x12AB", nullptr, 0), 4779);
    EXPECT_EQ(std::stol("12AB", nullptr, 16), 4779);

    // otherwise, a zero base is taken as 10 (decimal) unless the next character
    // is '0', in which case it  is taken as 8 (octal).

    EXPECT_EQ(std::stol("12345", nullptr, 0), 12345);
    EXPECT_EQ(std::stol("12345", nullptr, 10), 12345);
  }

  // limits
  {
    // *cxx-string-convert-to-string* *cxx-limits*
    long long ll = std::numeric_limits<long long>::max();
    EXPECT_EQ(std::to_string(ll), "9223372036854775807");

    // try to convert back and throws out_of_range
    EXPECT_THROW(std::stoi("9223372036854775807"), std::out_of_range);
  }
}

// ={=========================================================================
// cxx-stringstream
TEST(StringConversion, check_stringstream)
{
  // note that os, buffer, has all inputs from << and seek() moves writing pos.
  // *cxx-string-convert-to-string*

  // to string
  {
    std::ostringstream os;

    os << "decimal : " << 15 << hex << ", hex : " << 15 << endl;
    EXPECT_EQ(os.str(), "decimal : 15, hex : f\n");

    bitset<15> bit_set(5789);
    os << "float : " << 4.67 << ", bitset : " << bit_set << endl;
    EXPECT_EQ(
      os.str(),
      "decimal : 15, hex : f\nfloat : 4.67, bitset : 001011010011101\n");

    os.seekp(0);
    os << "octal : " << oct << 15;
    EXPECT_EQ(
      os.str(),
      "octal : 1715, hex : f\nfloat : 4.67, bitset : 001011010011101\n");
  }

  // to string in hex
  {
    std::ostringstream os;
    // os << "0x" << hex << 16;
    os << std::hex << 16;
    EXPECT_THAT(os.str(), "10");
  }

  // input integer to hex string
  {
    std::string input{"15056"};
    uint32_t value = std::stoi(input.c_str());

    std::ostringstream os;
    os << hex << value;
    EXPECT_THAT(os.str(), "3ad0");
  }

  // input integer and add some. to string
  {
    std::string input{"15056"};
    uint32_t value = std::stoi(input.c_str());

    std::ostringstream os;
    os << 5000 + value;
    EXPECT_THAT(os.str(), "20056");
  }

  // to string
  {
    std::stringstream ss;
    std::vector<string> string_vector{};

    for (int i = 0; i < 4; ++i)
    {
      ss << "player " << i;
      string_vector.push_back(string(ss.str()));
      ss.str("");
    }

    EXPECT_THAT(string_vector,
                ElementsAre("player 0", "player 1", "player 2", "player 3"));
  }

  // convert string to number
  {
    // The following lines read the integer x with the value 3 and the
    // floating-point f with the value 0.7 from the stringstream:

    int x{};
    float f{};

    std::istringstream is{"3.7"};

    // or
    // string input{"3.7"};
    // stringstream is(input);
    //
    // or
    // string input{"3.7"};
    // stringstream is;
    // is.str(input);

    is >> x >> f;

    ASSERT_THAT(x, Eq(3));
    ASSERT_THAT(f, FloatEq(0.7));
  }

  // leading spaces?
  {
    {
      // fail since int is too small
      //  int  max:      2.147.483.647
      //            16.060.300.000.001

      std::stringstream is{"00000000000000000016060300000001"};
      int value{};
      is >> value;
      EXPECT_THAT(value, Ne(16060300000001));
    }

    {
      // okay
      std::stringstream is{"00000000000000000016060300000001"};
      long long value{};
      is >> value;
      EXPECT_THAT(value, 16060300000001);
    }

    {
      // to hex and okay
      std::stringstream is{"00000000000000000016060300000001"};
      long long value{};
      is >> std::hex >> value;
      EXPECT_THAT(value, 6199059442302977);
    }

    // as octal since
    // otherwise, a zero base is taken as 10 (decimal) unless the next character
    // is '0', in which case it  is taken as 8 (octal).
    EXPECT_THAT(std::stoll("00000000000000000016060300000001", nullptr, 0),
                968565456897);

    // "taken as" means see "input" as "base". to 10
    EXPECT_THAT(std::stoll("00000000000000000016060300000001", nullptr, 10),
                16060300000001);

    // to 16
    EXPECT_THAT(std::stoll("00000000000000000016060300000001", nullptr, 16),
                6199059442302977);
  }

  // to number
  {
    std::stringstream is{"1 2 3 4"};
    int value{};
    vector<int> coll{};

    while (is >> value)
      coll.push_back(value);

    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4));
  }

  // to number
  {
    stringstream is{"1234"};
    int value{};

    is >> setw(2) >> value;

    // actually was 1234
    // EXPECT_THAT(value, 12);
  }

  // to number
  {
    string coll{"10101"};
    string token = coll.substr(0, 1);
    EXPECT_EQ(stoi(token), 1);
  }

  // to number
  {
    string coll{"10101"};
    stringstream is{coll.substr(0, 1)};
    int value{};
    is >> value;
    EXPECT_EQ(value, 1);
  }

  // to number(hex)
  {
    stringstream is{"fabc"};
    int value{};

    is >> hex >> value;

    EXPECT_THAT(value, 64188);
  }
}

// ={=========================================================================
TEST(StringConversion, check_boost_lexicalcast)
{
  EXPECT_THAT(boost::lexical_cast<std::string>(11), "11");
  EXPECT_THAT(boost::lexical_cast<std::string>(3301), "3301");
  EXPECT_THAT(boost::lexical_cast<std::string>(0x10), "16");

  EXPECT_THAT(boost::lexical_cast<int>("11"), 11);
  EXPECT_THAT(boost::lexical_cast<int>("3301"), 3301);
}

// ={=========================================================================
TEST(StringConversion, check_narrow_conversion)
{
  // warning/error
  // :464:12: warning: narrowing conversion of ‘5.0e+0’ from ‘double’ to ‘int’
  // inside { } [-Wnarrowing]
  //    int x{5.0};
  //            ^
  // int x{5.0};

  int x = 5.0;
  EXPECT_EQ(x, 5);

  // I am cramming an int into a short. On my (Windows XP) system, an int is
  // four bytes and a short is two. A short is signed, which means that I have
  // 15 bits to represent a number with and, therefore, 32,767 is the maximum
  // positive value it can hold. The above piece of code goes off without a
  // hitch, but when I increment i by one, it goes beyond the range of a short:

  int i{32767};
  EXPECT_EQ(boost::numeric_cast<short>(i), 32767);
  ++i;
  EXPECT_THROW(boost::numeric_cast<short>(i), boost::bad_numeric_cast);
}

// ={=========================================================================
TEST(StringConversion, check_case)
{
  // use cxx-transform
  {
    std::string coll{"CONVERSION"};

    std::transform(coll.begin(), coll.end(), coll.begin(), [](char &ch) {
      return std::tolower(ch);
    });

    EXPECT_THAT(coll, "conversion");
  }

  // use boost
  {
    std::string coll{"CONVERSION"};

    boost::to_lower(coll);

    EXPECT_THAT(coll, "conversion");
  }
}

// ={=========================================================================
// string-pad string-insert
//
// pad(string, lenth, char);
//
// s.insert(pos,num,c)
//
// Inserts num occurrences of the character c at iterator position pos and
// returns the iterator position of the first new character

template <typename T>
void pad(T &s, typename T::size_type n, typename T::value_type c)
{
  if (n > s.size())
    s.append(n - s.size(), c);
}

TEST(StringPad, InsertAppendPad)
{
  // insert
  string s1{"foo"};
  s1.insert(s1.size(), 20 - s1.size(), 'X');
  EXPECT_EQ(s1, "fooXXXXXXXXXXXXXXXXX");

  string s2{"foo"};
  s2.insert(s2.end(), 20 - s2.size(), 'X');
  EXPECT_EQ(s2, "fooXXXXXXXXXXXXXXXXX");

  // append
  string s3{"foo"};
  s3.append(20 - s3.size(), 'X');
  EXPECT_EQ(s3, "fooXXXXXXXXXXXXXXXXX");

  // pad
  string s4{"foo"};
  pad(s4, 20, 'X');
  EXPECT_EQ(s4, "fooXXXXXXXXXXXXXXXXX");

  wstring ws1{L"foo"};
  pad(ws1, 20, 'X');
  EXPECT_EQ(ws1, L"fooXXXXXXXXXXXXXXXXX");
}

TEST(StringPad, AppendNull)
{
  string s1{};

  for (int n = 0; n < 5; ++n)
    s1 += "";

  EXPECT_EQ(s1.length(), 0);
}

// ={=========================================================================
// string-trim
//
// 4.2 Trimming a String
//
// void trim(std::string &s, char c);
//
// remove chars from the end which matches with the given char
//
// string s1{"zoo"};
// rtrim(s1, 'o');
// EXPECT_EQ(s1, "z");
//
// variations: trim whitespace, trim from the left.

// use single call to erase than removing one at a time.

namespace string_trim
{

  void trim_1(string &s, const char c)
  {
    int start = s.size() - 1;

    // failes on "ooo" since don not chacek when start == 0
    //
    // while (start)
    // {
    //   if (s[start] != c)
    //     break;
    //
    //   --start;
    // }

    for (; start >= 0; --start)
    {
      if (s[start] != c)
        break;
    }

    // same as
    // s.erase(start+1, s.size());

    // +1 since start is the pos that is not c
    s.erase(start + 1);
  }

  // if use iterators
  void trim_2(string &s, const char c)
  {
    if (s.empty())
      return;

    auto end = s.end();

    // skip `c` chars if there are and note that `end` is decreased and is
    // checked. hence need to ++end at below.
    //
    // also note that ulike trim_1(), works on "ooo" since `end` is begin.

    for (; end != s.begin() && *--end == c;)
      ;

    if (*end != c)
      ++end;

    s.erase(end, s.end());

    // note: cxx-string-erase
    //
    // case core
    // s.erase(end);
    //
    // okay
    // if (end != s.end()) s.erase(end);
    //
    // okay
    // s.erase(end, s.end());
    //
    // this causes crash on
    //
    // string s1{"zzz"};
    // trim_iter(s1, 'o');
    // EXPECT_EQ(s1, "zzz");
    //
    // since there's no match, end is the same as s.end() after for loop.
    // s.end(pos) is different from s.end(pos, pos) and that explains why it's
    // okay when use erase(pos, pos).
    //
    // okay, understand why it's problem to erase element end poinst to. why
    // didn't get assert when use -D_GLIBCXX_DEBUG.
    //
    // when looked at source:
    //
    // /**
    //  *  @brief  Remove one character.
    //  *  @param __position  Iterator referencing the character to remove.
    //  *  @return  iterator referencing same location after removal.
    //  *
    //  *  Removes the character at @a __position from this string. The value
    //  *  of the string doesn't change if an error is thrown.
    //  */
    // iterator
    //   erase(iterator __position)
    //   {
    //     _GLIBCXX_DEBUG_PEDASSERT(__position >= _M_ibegin()
    //         && __position < _M_iend());
    //     const size_type __pos = __position - _M_ibegin();
    //     _M_mutate(__pos, size_type(1), size_type(0));
    //     _M_rep()->_M_set_leaked();
    //     return iterator(_M_data() + __pos);
    //   }
    //
    // Oh, it does have ASSERT How to enable? Use -D_GLIBCXX_DEBUG_PEDANTIC then
    // gets assert rather than having a core only which need to debug further.
    //
    // /usr/include/c++/4.9/bits/basic_string.h:1399: std::basic_string<_CharT,
    // _Traits, _Alloc>::iterator std::basic_string<_CharT, _Traits,
    // _Alloc>::erase(std::basic_string<_CharT, _Traits, _Alloc>::iterator)
    // [with _CharT = char; _Traits = std::char_traits<char>; _Alloc =
    // std::allocator<char>; std::basic_string<_CharT, _Traits,
    // _Alloc>::iterator = __gnu_cxx::__normal_iterator<char*,
    // std::basic_string<char> >; typename
    // _Alloc::rebind<_CharT>::other::pointer = char*]: Assertion '__position >=
    // _M_ibegin() && __position < _M_iend()' failed.
    // Aborted (core dumped)
  }

} // namespace string_trim

TEST(StringTrim, 2018_11)
{
  using namespace string_trim;

  {
    string s1{"zoo"};
    trim_1(s1, 'o');
    EXPECT_THAT(s1, "z");
  }

  {
    string s1{"zzz"};
    trim_1(s1, 'o');
    EXPECT_THAT(s1, "zzz");
  }

  {
    string s1{"zoo                              "};
    trim_1(s1, ' ');
    EXPECT_THAT(s1, "zoo");
  }

  {
    string s1{"zoo"};
    trim_1(s1, ' ');
    EXPECT_THAT(s1, "zoo");
  }

  {
    string s1{"ooo"};
    trim_1(s1, 'o');
    EXPECT_THAT(s1, "");
  }

  // trim iterator version
  {
    string s1{"zoo"};
    trim_2(s1, 'o');
    EXPECT_THAT(s1, "z");
  }

  {
    string s1{"zzz"};
    trim_2(s1, 'o');
    EXPECT_THAT(s1, "zzz");
  }

  {
    string s1{"zoo                              "};
    trim_2(s1, ' ');
    EXPECT_THAT(s1, "zoo");
  }

  {
    string s1{"zoo"};
    trim_2(s1, ' ');
    EXPECT_THAT(s1, "zoo");
  }

  {
    string s1{"ooo"};
    trim_2(s1, 'o');
    EXPECT_THAT(s1, "");
  }
}

// ={=========================================================================
// string-split string-parse
//
// 4.6 Splitting a String
// You want to split a delimited string into multiple strings. For example, you
// may want to split the string "Name|Address|Phone" into three separate
// strings, "Name", "Address", and "Phone", with the delimiter removed.

// *cxx-string-find* *cxx-string-substr* *cxx-string-replace*
//
// CXXSLR 13.1.1 A First Example: Extracting a Temporary Filename
// make the original example short.

TEST(StringParse, GetExtension)
{
  const string suffix{"tmp"};
  vector<string> coll1{"prog.dat", "mydir", "hello.", "opps.tmp", "end.dat"};
  vector<string> coll2;
  string filename, basename, extname;

  for (auto inputname : coll1)
  {
    auto idx = inputname.find('.');

    // if not have .
    if (idx == string::npos)
    {
      filename = inputname + '.' + suffix;
    }
    else
    {
      basename = inputname.substr(0, idx);
      extname  = inputname.substr(idx + 1);
      filename = basename + '.' + suffix;

      // to use cxx-string-replace
      if (extname == suffix)
      {
        filename = inputname;
        filename.replace(idx + 1, string::npos, "xxx");
      }
    }

    coll2.push_back(filename);
  }

  EXPECT_THAT(
    coll2,
    ElementsAre("prog.tmp", "mydir.tmp", "hello.tmp", "opps.xxx", "end.tmp"));
}

// split input string by delimeter and put them into vector
// void split(const string &s, char delim, vector<string> &coll);
//
// note that substr(idx, `length`); but not substr(idx, idx);

namespace stringsplit_2018_10
{
  // note:
  // this has *undefined* behaviour; sometimes works and sometimes infinite loop
  // since `found` is npos when not found and do ++found and use it loop
  // condition.
  //
  // void split(const string &s, char delim, vector<string> &coll)
  // {
  //   size_t i{};
  //   size_t found{};
  //   std::string token;
  //
  //   for (i = 0; found != std::string::npos; )
  //   {
  //     found = s.find(delim, i);
  //     token = s.substr(i, found-i);
  //     i = ++found;
  //     coll.push_back(token);
  //     // std::cout << "token: " << token << std::endl;
  //   }
  // }

  void split_text(const string &s, char delim, vector<string> &coll)
  {
    string::size_type i     = 0;
    string::size_type found = s.find(delim);

    while (found != string::npos)
    {
      // save the first token
      coll.push_back(s.substr(i, found - i));
      // std::cout << "token: " << s.substr(i, found-i) << std::endl;
      i     = ++found;
      found = s.find(delim, i);

      // if see no more token, save the current
      if (found == string::npos)
      {
        // std::cout << "token: " << s.substr(i, found) << std::endl;
        coll.push_back(s.substr(i, found));
      }
    }
  }

  void split_revised(const string &input, char delim, vector<string> &coll)
  {
    string::size_type i{};
    string::size_type found = input.find(delim, i);

    while (found != string::npos)
    {
      coll.push_back(input.substr(i, found - i));
      i     = ++found;
      found = input.find(delim, i);
    }

    if (found == string::npos)
      coll.push_back(input.substr(i, found - i));
  }
} // namespace stringsplit_2018_10

TEST(StringSplit, ByDelimeter_201810)
{
  using namespace stringsplit_2018_10;

  {
    std::vector<std::string> coll;
    split_text("Smith, Bill, 5/1/2002, Active", ',', coll);
    EXPECT_THAT(coll, ElementsAre("Smith", " Bill", " 5/1/2002", " Active"));
  }

  {
    // fails:
    std::vector<std::string> coll;
    split_text("Smith", ',', coll);
    EXPECT_THAT(coll, ElementsAre());
  }

  {
    std::vector<std::string> coll;

    split_revised("Smith, Bill, 5/1/2002, Active", ',', coll);
    EXPECT_THAT(coll, ElementsAre("Smith", " Bill", " 5/1/2002", " Active"));

    coll.clear();
    split_revised("Smith", ',', coll);
    EXPECT_THAT(coll, ElementsAre("Smith"));
  }
}

// *boost-split* *boost-is-any-of*
// The one function that is in the example that isn’t in the table is
// is_any_of. This is a function template that returns a predicate
// function object that can be used by the trim_if-style functions.

TEST(StringSplit, ByBoost)
{
  {
    vector<string> coll;

    boost::split(coll, "Smith, Bill, 5/1/2002, Active", boost::is_any_of("|"));

    // not found and returns the input
    EXPECT_THAT(coll, ElementsAre("Smith, Bill, 5/1/2002, Active"));
  }

  {
    vector<string> coll;

    boost::split(coll, "Smith, Bill, 5/1/2002, Active", boost::is_any_of(","));

    EXPECT_THAT(coll, ElementsAre("Smith", " Bill", " 5/1/2002", " Active"));
  }

  {
    vector<string> coll;

    boost::split(coll, "Name|Address|Phone", boost::is_any_of("|"));

    EXPECT_THAT(coll, ElementsAre("Name", "Address", "Phone"));
  }

  // *cxx-boost-is-any-of* for multiple delimeters
  {
    vector<string> coll;

    boost::split(coll, "Name|Address,Phone", boost::is_any_of("|,"));

    EXPECT_THAT(coll, ElementsAre("Name", "Address", "Phone"));
  }

  {
    // note: cxx-boost-split see how boost works
    //
    // Value of: svec
    // Expected: has 1 element that is equal to "Name"
    //   Actual: { "", "", "Name" }, which has 3 elements

    vector<string> coll;
    boost::split(coll, "||Name", boost::is_any_of("|"));

    EXPECT_THAT(coll, ElementsAre("", "", "Name"));
  }

  {
    // note: cxx-boost-split see how boost works
    vector<string> coll;
    boost::split(coll, "||", boost::is_any_of("|"));

    EXPECT_THAT(coll, ElementsAre("", "", ""));
  }
}

namespace stringsplit_2018_11
{

  // 2018.11.14

  void split(const string &s, const char delim, vector<string> &coll)
  {
    string token{};
    size_t start{}, end{};

    end = s.find(delim, start);
    for (; end < string::npos;)
    {
      coll.push_back(s.substr(start, end - start));
      start = end + 1;
      end   = s.find(delim, start);
    }

    coll.push_back(s.substr(start, end - start));
  }

} // namespace stringsplit_2018_11

TEST(StringSplit, ByDelimeter_2018_11)
{
  using namespace stringsplit_2018_11;

  {
    std::vector<std::string> coll;

    //         5      11
    split("Smith, Bill, 5/1/2002, Active", ',', coll);
    EXPECT_THAT(coll, ElementsAre("Smith", " Bill", " 5/1/2002", " Active"));

    coll.clear();

    // when not found
    split("Smith", ',', coll);
    EXPECT_THAT(coll, ElementsAre("Smith"));
  }

  {
    std::vector<std::string> coll;
    split("192.169.1.20", '.', coll);
    EXPECT_THAT(coll, ElementsAre("192", "169", "1", "20"));
  }

  {
    std::vector<std::string> coll;
    split("Name|Address|Phone", '|', coll);
    EXPECT_THAT(coll, ElementsAre("Name", "Address", "Phone"));
  }

  {
    // same as how boost works
    vector<string> coll;
    split("||Name", '|', coll);

    EXPECT_THAT(coll, ElementsAre("", "", "Name"));
  }

  {
    // same as how boost works
    vector<string> coll;
    split("||", '|', coll);

    EXPECT_THAT(coll, ElementsAre("", "", ""));
  }
}

namespace string_split_2018_11
{

  // copied from TEST(Stdio, GetLine)

  // {
  //   string s{};
  //   stringstream iss("one|two|three");
  //   vector<string> coll{};

  //   while (getline(iss , s, '|'))
  //     coll.push_back(s);

  //   EXPECT_THAT(coll, ElementsAre("one", "two", "three"));
  // }

} // namespace string_split_2018_11

// ={=========================================================================
// string-token
//
// 4.7, tokenizing a string, C++ codebook
//
// int main( ) {
//    string s = " razzle dazzle giddyup ";
//    string tmp;
//    StringTokenizer st(s);
//    cout << "there are " << st.countTokens( ) << " tokens.\n";
//    while (st.hasMoreTokens( )) {
//        st.nextToken(tmp);
//        cout << "token = " << tmp << '\n';
//    }
// }
//
// * there are couple of points that are different from the book

namespace tokenizer_book
{
  class StringTokenizer
  {
  public:
    StringTokenizer(const string &s, const char *delim = nullptr)
        : str_(s)
    {
      if (!delim)
        delim_ = " \f\n\r\r\v|";
      else
        delim_ = delim;

      // NOTE: *TDD*
      // this causes to fail on the "||Name" and loop on nextToken has
      // {"Name", "Name"}
      //
      // begin_ = str_.find_first_not_of(delim_);
      // end_ = str_.find_first_of(delim_);

      begin_ = str_.find_first_not_of(delim_);
      end_   = str_.find_first_of(delim_, begin_);
    }

    void nextToken(string &s)
    {
      // if (begin_ == string::npos && end_ == string::npos)
      // is when input is null.
      //
      // NOTE:
      // This is not supposed be called since hasMoreTokens() return false.
      // However, when user call it wihtout checking with hasMoreTokens() then
      // do nothing and more defensive.
      //
      // see MakeToken::nextToken()

      if (begin_ != string::npos && end_ != string::npos)
      {
        s      = str_.substr(begin_, end_ - begin_);
        begin_ = str_.find_first_not_of(delim_, end_);
        end_   = str_.find_first_of(delim_, begin_);
      }
      else if (begin_ != string::npos && end_ == string::npos)
      {
        s      = str_.substr(begin_, str_.length() - begin_);
        begin_ = str_.find_first_not_of(delim_, end_);
      }
    }

    size_t countTokens()
    {
      // return if we already counted. possible to call this more than once?
      // this is becuase to have member variable.
      if (count_ != 0)
        return count_;

      size_t count_delim = 0;
      size_t pos_delim   = 0;

      for (;;)
      {
        // break if there are only delimiters.
        if ((pos_delim = str_.find_first_not_of(delim_, pos_delim)) ==
            string::npos)
          break;

        // this is different from the book
        // pos_delim = str_.find_first_of(delim_, pos_delim+1);
        pos_delim = str_.find_first_of(delim_, pos_delim);
        ++count_delim;

        if (pos_delim == string::npos)
          break;
      }

      return count_ = count_delim;
    }

    bool hasMoreTokens() { return begin_ != end_; }

  private:
    StringTokenizer() {}
    string delim_;
    string str_;
    size_t count_{};
    size_t begin_{};
    size_t end_{};
  };

} // namespace tokenizer_book

namespace tokenizer_2018_05_29
{
  // use internal vector and find(). do not have hasMore() since countTokens()
  // returns size of vector.

  // MakeTonen_0529
  //
  // int main( ) {
  //    string tmp;
  //    MakeToken_0529 st("Name|Address|Phone");
  //    cout << "there are " << st.countTokens( ) << " tokens.\n";
  //    while (st.hasMoreTokens( )) {
  //        st.nextToken(tmp);
  //        cout << "token = " << tmp << '\n';
  //    }
  // }
  //
  // 1. countTokens() count number of tokens, cache it, and return it. Then how
  // about hasMoreTokens()? This means that don't need to keep input_ and change
  // it whenever nextToken() gets called. So count number of tokens whenever it
  // gets called and keep a contailer which has tokens.
  //
  // int main( ) {
  //    string tmp;
  //    MakeToken st("Name|Address|Phone");
  //    cout << "there are " << st.countTokens( ) << " tokens.\n";
  //    while (st.countTokens( )) {
  //        st.nextToken(tmp);
  //        cout << "token = " << tmp << '\n';
  //    }
  // }

  // TDD on 2018.05.29
  // * Like cxx-boost-split, "||Name" has {"", "", "Name"}
  // * Has token array member so remove hadMoreToken() and countToken() do not
  // calculate count everytime when gets called.
  // * note: may not work with "|||"

  class MakeToken_0529
  {
  public:
    MakeToken_0529(const string &input, const string &delim = "|")
        : input_(input)
        , delim_(delim)
    {
      size_t i{};
      size_t found = input_.find_first_of(delim_, i);

      while (found != string::npos)
      {
        tokens_.push_back(input_.substr(i, found - i));
        i     = ++found;
        found = input_.find_first_of(delim_, i);
      }

      if (found == string::npos)
        tokens_.push_back(input_.substr(i, found));
    }

    size_t countTokens() { return tokens_.size(); }

    void nextToken(string &token)
    {
      auto it = tokens_.begin();
      if (it != tokens_.end())
      {
        token = *it;
        tokens_.erase(it);
      }
    }

  private:
    const string input_;
    const string delim_;
    size_t number_of_tokens_{};
    vector<string> tokens_{};
  };

} // namespace tokenizer_2018_05_29

TEST(MakeToken_0529, UseVariousInputs)
{
  using namespace tokenizer_2018_05_29;

  std::string token{};
  std::vector<std::string> svec{};

  // 1. "Name|Address|Phone"
  MakeToken_0529 mt1("Name|Address|Phone");
  EXPECT_EQ(3, mt1.countTokens());

  while (mt1.countTokens())
  {
    mt1.nextToken(token);
    svec.push_back(token);
  }

  EXPECT_THAT(svec, ElementsAre("Name", "Address", "Phone"));

  // 2. "Name|Address"
  token.clear();
  svec.clear();
  MakeToken_0529 mt2("Name|Address");
  EXPECT_EQ(2, mt2.countTokens());

  while (mt2.countTokens())
  {
    mt2.nextToken(token);
    svec.push_back(token);
  }

  EXPECT_THAT(svec, ElementsAre("Name", "Address"));

  // 3. "Name"
  token.clear();
  svec.clear();
  MakeToken_0529 mt3("Name");
  EXPECT_EQ(1, mt3.countTokens());

  while (mt3.countTokens())
  {
    mt3.nextToken(token);
    svec.push_back(token);
  }

  EXPECT_THAT(svec, ElementsAre("Name"));

  // 4. "Name|"
  token.clear();
  svec.clear();
  MakeToken_0529 mt4("Name|");
  EXPECT_EQ(2, mt4.countTokens());

  while (mt4.countTokens())
  {
    mt4.nextToken(token);
    svec.push_back(token);
  }

  EXPECT_THAT(svec, ElementsAre("Name", ""));

  // 5. "Name||Address"
  token.clear();
  svec.clear();
  MakeToken_0529 mt5("Name||Address");
  EXPECT_EQ(3, mt5.countTokens());

  while (mt5.countTokens())
  {
    mt5.nextToken(token);
    svec.push_back(token);
  }

  EXPECT_THAT(svec, ElementsAre("Name", "", "Address"));

  // 6. "||Name"
  // exception where end > begin
  token.clear();
  svec.clear();
  MakeToken_0529 mt6("||Name");
  EXPECT_EQ(3, mt6.countTokens());

  while (mt6.countTokens())
  {
    mt6.nextToken(token);
    svec.push_back(token);
  }

  EXPECT_THAT(svec, ElementsAre("", "", "Name"));
}

namespace tokenizer_2018_06_06
{

  // int main( ) {
  //    string s = " razzle dazzle giddyup ";
  //    string tmp;
  //    StringTokenizer st(s);
  //    cout << "there are " << st.countTokens( ) << " tokens.\n";
  //    while (st.hasMoreTokens( )) {
  //        st.nextToken(tmp);
  //        cout << "token = " << tmp << '\n';
  //    }
  // }
  //
  // * not boost-split style which means do not support null token.

  class StringTokenizer_0606
  {
  public:
    StringTokenizer_0606(const string &input, const string &delim = "|")
        : input_(input)
        , delim_(delim)
    {
      pos_token_ = input_.find_first_not_of(delim_, 0);
      pos_delim_ = input_.find_first_of(delim_, pos_token_);
    }

    bool hasMoreTokens() { return pos_token_ != pos_delim_; }

    void nextToken(string &token)
    {
      token = input_.substr(pos_token_, pos_delim_ - pos_token_);

      pos_token_ = input_.find_first_not_of(delim_, pos_delim_);
      pos_delim_ = input_.find_first_of(delim_, pos_token_);
    }

    size_t countTokens()
    {
      if (count_)
        return count_;

      size_t pos_start = input_.find_first_not_of(delim_, 0);
      size_t pos_delim{};

      while ((pos_delim = input_.find_first_of(delim_, pos_start)) !=
             string::npos)
      {
        pos_start = input_.find_first_not_of(delim_, pos_delim);
        ++count_;
      }

      if (pos_start != string::npos && pos_delim == string::npos)
        ++count_;

      return count_;
    }

  private:
    StringTokenizer_0606() {}
    string input_;
    string delim_;
    size_t count_{};
    size_t pos_token_{};
    size_t pos_delim_{};
  };

} // namespace tokenizer_2018_06_06

TEST(StringTokenizer_0606, ParseTokensFromVariosInputs)
{
  using namespace tokenizer_2018_06_06;

  string token{};
  vector<string> coll{};

  StringTokenizer_0606 st1("Name|Address|Phone");
  EXPECT_EQ(st1.countTokens(), 3);

  while (st1.hasMoreTokens())
  {
    st1.nextToken(token);
    coll.push_back(token);
  }

  EXPECT_THAT(coll, ElementsAre("Name", "Address", "Phone"));

  // 2. "Name|Address"
  token.clear();
  coll.clear();
  StringTokenizer_0606 st2("Name|Address");
  EXPECT_EQ(2, st2.countTokens());

  while (st2.hasMoreTokens())
  {
    st2.nextToken(token);
    coll.push_back(token);
  }

  EXPECT_THAT(coll, ElementsAre("Name", "Address"));

  // 3. "Name"
  token.clear();
  coll.clear();
  StringTokenizer_0606 st3("Name");
  EXPECT_EQ(1, st3.countTokens());

  while (st3.hasMoreTokens())
  {
    st3.nextToken(token);
    coll.push_back(token);
  }

  EXPECT_THAT(coll, ElementsAre("Name"));

  // 4. "Name|"
  token.clear();
  coll.clear();
  StringTokenizer_0606 st4("Name|");
  EXPECT_EQ(1, st4.countTokens());

  while (st4.hasMoreTokens())
  {
    st4.nextToken(token);
    coll.push_back(token);
  }

  EXPECT_THAT(coll, ElementsAre("Name"));

  // 5. "Name||Address"
  token.clear();
  coll.clear();
  StringTokenizer_0606 st5("Name||Address");
  EXPECT_EQ(2, st5.countTokens());

  while (st5.hasMoreTokens())
  {
    st5.nextToken(token);
    coll.push_back(token);
  }

  EXPECT_THAT(coll, ElementsAre("Name", "Address"));

  // 6. "||Name"
  // exception where end > begin
  token.clear();
  coll.clear();
  StringTokenizer_0606 st6("||Name");
  EXPECT_EQ(1, st6.countTokens());

  while (st6.hasMoreTokens())
  {
    st6.nextToken(token);
    coll.push_back(token);
  }

  EXPECT_THAT(coll, ElementsAre("Name"));
}

namespace tokenizer_2018_11_split
{

  // use split()

  // o '|' is default delim
  //
  // int main( ) {
  //    string s = " razzle dazzle giddyup ";
  //    string tmp;
  //    StringTokenizer st(s, "|");
  //    cout << "there are " << st.countTokens( ) << " tokens.\n";
  //    while (st.hasMoreTokens( )) {
  //        st.nextToken(tmp);
  //        cout << "token = " << tmp << '\n';
  //    }
  // }

  class StringTokenizer
  {
  public:
    explicit StringTokenizer(const string &s, const char delim = '|')
        : delim_(delim)
    {
      split(s, delim);
    }

    int countTokens() { return token_.size(); }

    // meaning of empty()
    bool hasMoreTokens() { return !token_.empty(); }

    // have to remove one
    void nextToken(string &next)
    {
      // since vector don't have interface to pop from the front
      next = token_.front();
      token_.erase(token_.begin());
    }

  private:
    void split(const string &s, const char delim)
    {
      auto length = s.length();
      auto end    = s.find(delim);
      decltype(end) start{};
      string token;

      while (end < length)
      {
        token = s.substr(start, end - start);
        if (!token.empty())
          token_.push_back(token);

        // this fails on caes which has empty token like "||Name" so have to
        // have to check before inserting and this is improvement from
        // *string-split*
        //
        // token_.push_back(s.substr(start, end - start));

        start = end + 1;
        end   = s.find(delim, start);
      }

      // // this covers either last or single split
      // token_.push_back(s.substr(start, end - start));

      // this covers either last or single split
      token = s.substr(start, end - start);
      if (!token.empty())
        token_.push_back(token);
    }

    vector<string> token_;
    char delim_;
  };

} // namespace tokenizer_2018_11_split

// cases:
//
// "Name|Address|Phone"
// "Name|Address"
// "Name"
// "Name|"
// "Name||Address"
// "||Name"
//  "||"

TEST(StringTokenizer, 2018_11_split)
{
  using namespace tokenizer_2018_11_split;

  // "Name|Address|Phone"
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("Name|Address|Phone");

    EXPECT_EQ(3, st.countTokens());

    while (st.hasMoreTokens())
    {
      st.nextToken(token);
      coll.push_back(token);
    }

    EXPECT_THAT(coll, ElementsAre("Name", "Address", "Phone"));
  }

  // "Name|Address"
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("Name|Address");

    EXPECT_EQ(2, st.countTokens());

    while (st.hasMoreTokens())
    {
      st.nextToken(token);
      coll.push_back(token);
    }

    EXPECT_THAT(coll, ElementsAre("Name", "Address"));
  }

  // "Name"
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("Name");

    EXPECT_EQ(1, st.countTokens());

    while (st.hasMoreTokens())
    {
      st.nextToken(token);
      coll.push_back(token);
    }

    EXPECT_THAT(coll, ElementsAre("Name"));
  }

  // "Name|"
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("Name|");

    EXPECT_EQ(1, st.countTokens());

    while (st.hasMoreTokens())
    {
      st.nextToken(token);
      coll.push_back(token);
    }

    EXPECT_THAT(coll, ElementsAre("Name"));
  }

  // "Name||Address"
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("Name||Address");

    EXPECT_EQ(2, st.countTokens());

    while (st.hasMoreTokens())
    {
      st.nextToken(token);
      coll.push_back(token);
    }

    EXPECT_THAT(coll, ElementsAre("Name", "Address"));
  }

  // "||Name"
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("||Name");

    EXPECT_EQ(1, st.countTokens());

    while (st.hasMoreTokens())
    {
      st.nextToken(token);
      coll.push_back(token);
    }

    EXPECT_THAT(coll, ElementsAre("Name"));
  }

  //  "||"
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("||");

    EXPECT_EQ(0, st.countTokens());
  }

  // this fails
  //
  // //  ""
  // {
  //   std::string token{};
  //   std::vector<std::string> coll{};

  //   StringTokenizer st("");

  //   EXPECT_EQ(0, st.countTokens());

  //   for (int i = 0; i < 5; ++i)
  //   {
  //     st.nextToken(token);
  //     if (!token.empty())
  //       coll.push_back(token);
  //   }

  //   EXPECT_TRUE(coll.empty());
  // }
}

namespace tokenizer_2018_11_find
{

  // use find() family
  // do not use internal coll such as vector which means that do everything on
  // the fly when requested

  // o '|' is default delim
  // o countToken() returns # of tokens of input
  //
  // int main( ) {
  //    string s = " razzle dazzle giddyup ";
  //    string tmp;
  //    StringTokenizer st(s, "|");
  //    cout << "there are " << st.countTokens( ) << " tokens.\n";
  //    while (st.hasMoreTokens( )) {
  //        st.nextToken(tmp);
  //        cout << "token = " << tmp << '\n';
  //    }
  // }

  class StringTokenizer
  {
  public:
    StringTokenizer(const string &s, const char delim = '|')
        : input_(s)
        , delim_(delim)
    {
      count_ = 0;
      start_ = input_.find_first_not_of(delim_);
      end_   = input_.find_first_of(delim_, start_);
    }

    void nextToken(string &s)
    {
      // works for all except when call nextToken() without checking on
      // hasMore(). crashes.
      //
      // s = input_.substr(start_, end_ - start_);
      // start_ = input_.find_first_not_of(delim_, end_);
      // end_ = input_.find_first_of(delim_, start_);

      if (start_ != string::npos && end_ != string::npos)
      {
        s      = input_.substr(start_, end_ - start_);
        start_ = input_.find_first_not_of(delim_, end_);
        end_   = input_.find_first_of(delim_, start_);
      }
      else if (start_ != string::npos && end_ == string::npos)
      {
        s      = input_.substr(start_, end_ - start_);
        start_ = input_.find_first_not_of(delim_, end_);
      }
    }

    bool hasMoreTokens() { return start_ != end_; }

    size_t countTokens()
    {
      if (count_)
        return count_;

      // use two variables:
      //
      // decltype(start_) start{}, end{};
      //
      // start = input_.find_first_not_of(delim_);
      // end = input_.find_first_of(delim_);
      //
      // while (end < string::npos)
      // {
      //   ++count_;
      //   start = input_.find_first_not_of(delim_, end);
      //   end = input_.find_first_of(delim_, start);
      // }

      // do not support cases which has only delims:
      //
      // decltype(start_) run{};
      //
      // run = input_.find_first_of(delim_);
      //
      // while (run < string::npos)
      // {
      //   ++count_;
      //   run = input_.find_first_not_of(delim_, run);
      //   run = input_.find_first_of(delim_, run);
      // }
      //
      // return ++count_;

      // same approach as the book. the difference is that
      // o do find_first_not_of() and this makes no need to increase count out
      // of while loop which enables to return 0.
      //
      // o there is at least one token whenever see a char which is not delim,
      // so find_first_not_of()

      decltype(start_) run{};

      while (run < string::npos)
      {
        if ((run = input_.find_first_not_of(delim_, run)) == string::npos)
          break;

        ++count_;
        run = input_.find_first_of(delim_, run);
      }

      return count_;
    }

  private:
    string input_;
    char delim_;
    size_t start_, end_;
    decltype(start_) count_;
  };

} // namespace tokenizer_2018_11_find

TEST(StringTokenizer, 2018_11_find)
{
  using namespace tokenizer_2018_11_find;

  // "Name|Address|Phone"
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("Name|Address|Phone");

    EXPECT_EQ(3, st.countTokens());

    while (st.hasMoreTokens())
    {
      st.nextToken(token);
      coll.push_back(token);
    }

    EXPECT_THAT(coll, ElementsAre("Name", "Address", "Phone"));
  }

  // "Name|Address"
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("Name|Address");

    EXPECT_EQ(2, st.countTokens());

    while (st.hasMoreTokens())
    {
      st.nextToken(token);
      coll.push_back(token);
    }

    EXPECT_THAT(coll, ElementsAre("Name", "Address"));
  }

  // "Name"
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("Name");

    EXPECT_EQ(1, st.countTokens());

    while (st.hasMoreTokens())
    {
      st.nextToken(token);
      coll.push_back(token);
    }

    EXPECT_THAT(coll, ElementsAre("Name"));
  }

  // "Name|"
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("Name|");

    EXPECT_EQ(1, st.countTokens());

    while (st.hasMoreTokens())
    {
      st.nextToken(token);
      coll.push_back(token);
    }

    EXPECT_THAT(coll, ElementsAre("Name"));
  }

  // "Name||Address"
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("Name||Address");

    EXPECT_EQ(2, st.countTokens());

    while (st.hasMoreTokens())
    {
      st.nextToken(token);
      coll.push_back(token);
    }

    EXPECT_THAT(coll, ElementsAre("Name", "Address"));
  }

  // "||Name"
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("||Name");

    EXPECT_EQ(1, st.countTokens());

    while (st.hasMoreTokens())
    {
      st.nextToken(token);
      coll.push_back(token);
    }

    EXPECT_THAT(coll, ElementsAre("Name"));
  }

  //  "||"
  //  this is different how boost-split works.
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("||");

    EXPECT_EQ(0, st.countTokens());
  }

  //  ""
  {
    std::string token{};
    std::vector<std::string> coll{};

    StringTokenizer st("");

    EXPECT_EQ(0, st.countTokens());

    for (int i = 0; i < 5; ++i)
    {
      st.nextToken(token);
      if (!token.empty())
        coll.push_back(token);
    }

    EXPECT_TRUE(coll.empty());
  }
}

// ={=========================================================================
// string-parse

namespace string_parse
{
  // from asan tool

  // usage:
  //
  // const char *current = "08048000-08056000 r-xp 00000000 03:0c 64593
  // /foo/bar\n";
  //
  // char *next_line = (char *)memchr(current, '\n', strlen(current));
  // EXPECT_TRUE(next_line);
  //
  // *start = ParseHex(&current);
  // EXPECT_EQ(*start, 0x08048000);
  // EXPECT_EQ(*current++, '-');

  // covers hex
  int TranslateDigit(const char c)
  {
    if (c >= '0' && c <= '9')
      return c - '0';

    if (c >= 'a' && c <= 'f')
      return c - 'a' + 10;

    if (c >= 'A' && c <= 'F')
      return c - 'A' + 10;

    return -1;
  }

  // function modifies input pointer variable
  //
  // const char *current = "08048000-...";
  // ParseHex(&current);
  // ParseNumber(&current)
  // {
  //  (*p)++; means that changes current vaiable.
  // }
  //
  // That's okay from compiler pov since this is pointer to const char and const
  // char is not changed, that is, maintained constness.
  //
  // these do not cause error:
  //
  // current++; ++current;

  // like atoi()
  // stops when there is non-translatable char and *p points that char. So that
  // start parse again from where it stops.

  int ParseNumber(const char **p, int base)
  {
    int value{};
    int digit{};

    assert(base >= 2 && base <= 16);

    while ((digit = TranslateDigit(**p)) >= 0 && (digit < base))
    {
      value = base * value + digit;
      // increase pointer to process next char
      (*p)++;
    }

    return value;
  }

  int ParseHex(const char **p) { return ParseNumber(p, 16); }

  int ParseDecimal(const char **p) { return ParseNumber(p, 10); }

  bool IsOneOf(char c, char c1, char c2) { return (c == c1) || (c == c2); }

} // namespace string_parse

TEST(StringParse, Hex)
{
  using namespace string_parse;

  // example from /proc/map
  // address           perms offset  dev   inode       pathname
  // 00400000-00452000 r-xp 00000000 08:02 173521      /usr/bin/dbus-daemon

  int value{}, protection{};
  char filename[100] = {};

  const char *current =
    "08048000-08056000 r-xp 00000000 03:0c 64593   /foo/bar\n";

  // char *next_line = (char *)memchr(current, '\n', strlen(current));
  char *next_line = (char *)strchr(current, '\n');

  value = ParseHex(&current);
  EXPECT_EQ(value, 0x08048000);
  EXPECT_EQ(*current, '-');
  current++;

  value = ParseHex(&current);
  EXPECT_EQ(value, 0x08056000);
  // now use single expr to check and to increase
  EXPECT_EQ(*current++, ' ');

  // as can see, have a control if need to increase pointer to char or not.

  // check read permission
  EXPECT_TRUE(IsOneOf(*current, '-', 'r'));

  if (*current++ == 'r')
    protection |= 1;

  // check write permission
  EXPECT_TRUE(IsOneOf(*current, '-', 'w'));

  if (*current++ == 'w')
    protection |= 2;

  // check exec permission
  EXPECT_TRUE(IsOneOf(*current, '-', 'x'));

  if (*current++ == 'x')
    protection |= 4;

  // check shared permission
  EXPECT_TRUE(IsOneOf(*current, '-', 'p'));

  if (*current++ == 'p')
    protection |= 8;

  EXPECT_EQ(*current++, ' ');

  // offset
  value = ParseHex(&current);
  EXPECT_EQ(value, 0);

  EXPECT_EQ(*current++, ' ');
  ParseHex(&current);
  EXPECT_EQ(*current++, ':');
  ParseHex(&current);
  EXPECT_EQ(*current++, ' ');

  // inode
  ParseDecimal(&current);

  // skip spaces
  while (current < next_line && *current == ' ')
    ++current;

  // filename
  // function suggest that filename can be bigger than filename[]

  int i = 0;
  while (current < next_line)
  {
    if (/* filename && */ i < 100 /* filename_size-1 */)
      filename[i++] = *current;

    ++current;
  }

  if (/* filename && */ i < 100 /* filename_size-1 */)
    filename[i] = 0;

  EXPECT_STREQ(filename, "/foo/bar");
}

// want to get "GMT isdst=0 gmtoff=0" from the line:
// "/etc/localtime  Sun Mar 29 01:00:00 2020 UT = Sun Mar 29 02:00:00 2020 BST isdst=1 gmtoff=3600"

TEST(StringParse, check_case_1)
{
  // clang-format off
  // someone's implementation
  {
    std::string temp{"/etc/localtime  Tue Jan 19 03:14:07 2038 UT = Tue Jan 19 04:14:07 2038 CET isdst=0 gmtoff=3600"};
    struct tm utcTime = {0};

    temp.erase( 0, temp.find( " " ) + 2 ); // Remove "/etc/localtime  " -> 2 spaces

    auto utOff = temp.find( " UT = " );
    std::string date = temp.substr( 0, utOff ); // Capture UTC date "Tue Jan 19 03:14:07 2038"

    // Remove everything to " UT = ", "Tue Jan 19 04:14:07 2038 CET isdst=0 gmtoff=3600" is left
    temp.erase( 0, utOff + sizeof(" UT = ") - 1 );

    // Remove until next 5 spaces, "CET isdst=0 gmtoff=3600" will be left
    auto spaceCount = 5;
    while ( spaceCount > 0 )
    {
      auto spOff = temp.find( " " );
      temp.erase( 0, spOff + 1 );
      spaceCount--;
    }

    char timeZone[5] = {0};
    uint32_t isDst = 0;
    uint32_t gmtOff = 0;
    sscanf( temp.c_str(), "%s isdst=%u gmtoff=%u", timeZone, &isDst, &gmtOff );
  }
  // clang-format on

  // it seems brittle since what if #num of spaces in the line changes and use c
  // code. so my take which use cxx-stringstream

  {
    // clang-format off
    std::stringstream ss{"/etc/localtime  Sun Mar 29 01:00:00 2020 UT = Sun Mar 29 02:00:00 2020 BST isdst=1 gmtoff=3600"};
    // clang-format on

    std::string token{};

    // advance ss up to token we want to get
    for (int i = 0; i < 13; ++i)
    {
      ss >> token;
      // std::cout << "token: " << token << std::endl;
    }

    std::string timezone{};
    ss >> timezone;

    std::string dst{};
    ss >> dst;

    int dstvalue = std::stoi(dst.substr(dst.find('=') + 1));
    EXPECT_THAT(dstvalue, 1);

    std::string offset{};
    ss >> offset;

    int offsetvalue = std::stoi(offset.substr(offset.find('=') + 1));
    EXPECT_THAT(offsetvalue, 3600);
  }
}

// ={=========================================================================
// string-join

void join(const std::vector<std::string> &vector,
          const char delim,
          std::string &joined)
{
  joined.clear();

  for (auto it = vector.cbegin(); it != vector.cend(); ++it)
  {
    joined.append(*it);

    // cxx-iter-arithmetic
    if (it < vector.cend() - 1)
      joined.append(1, delim);
  }
}

TEST(String, JoinSequnenceString_01)
{
  std::vector<string> svec{"fee", "fi", "foe", "fum"};
  std::string s;

  join(svec, '/', s);

  EXPECT_THAT(s, Eq("fee/fi/foe/fum"));
}

/*

26. Joining strings together separated by a delimiter

Write a function that, given a list of strings and a delimiter, creates a new
string by concatenating all the input strings separated with the specified
delimiter. The delimiter must not appear after the last string, and when no
input string is provided, the function must return an empty string.

Example:
input { "this","is","an","example" } and delimiter ' ' (space),
output: "this is an example".

{
   using namespace std::string_literals;

   std::vector<std::string> v1{ "this","is","an","example" };
   std::vector<std::string> v2{ "example" };
   std::vector<std::string> v3{ };

   assert(join_strings(v1, " ") == "this is an example"s);
   assert(join_strings(v2, " ") == "example"s);
   assert(join_strings(v3, " ") == ""s);
}

*/

// to see why the text uses "-1"

// TEST(U26, 2018_11_28)
TEST(String, JoinSequnenceString_02)
{
  ostringstream os;
  std::vector<std::string> coll{"this", "is", "an", "example"};

  copy(coll.begin(), coll.end(), ostream_iterator<string>(os, ","));
  EXPECT_THAT(os.str(), "this,is,an,example,");
}

namespace U26_Text
{
  template <typename Iter>
  string join_string(Iter begin, Iter end, const char *delimeter)
  {
    ostringstream os;

    // copy(begin, end-1, ostream_iterator<string>(os, delimeter));
    // os << *(end-1);

    // *cxx-prev*
    copy(begin, prev(end), ostream_iterator<string>(os, delimeter));
    os << *prev(end);

    return os.str();
  }

  template <typename Coll>
  string join_string(Coll coll, const char *delimeter)
  {
    if (coll.size() == 0)
      return string();

    return join_string(coll.begin(), coll.end(), delimeter);
  }
} // namespace U26_Text

// TEST(U26, Text)
TEST(String, JoinSequnenceString_03)
{
  using namespace U26_Text;

  std::vector<std::string> coll{"this", "is", "an", "example"};

  EXPECT_THAT(join_string(coll, " "), "this is an example");
}

// ={=========================================================================
// 4.10 Finding the nth Instance of a Substring
bool nth_substring(int n, const std::string &s, const std::string &p)
{
  size_t start{0};
  size_t found;

  for (int i = 0; i < n; ++i)
  {
    found = s.find(p, start);

    if (found != std::string::npos)
      start = found + 1;
    else
      return false;
  }

  return true;
}

// int nthSubstr(int n, const string &s, const string &p)
// {
//     string::size_type i = s.find(p); // Find the first occurrence
//     int j;
//     for (j = 1; j < n && i != string::npos; ++j)
//         i = s.find(p, i + 1); // Find the next occurrence
//     if (j == n)
//         return (i);
//     else
//         return (-1);
// }

TEST(String, FindNthSubstring)
{
  const std::string s{"the wind, the sea, the sky, the trees"};
  const std::string p{"the"};

  EXPECT_THAT(nth_substring(1, s, p), true);
  EXPECT_THAT(nth_substring(2, s, p), true);
  EXPECT_THAT(nth_substring(4, s, p), true);
  EXPECT_THAT(nth_substring(5, s, p), false);
}

// ={=========================================================================
// string-raw string-literal

TEST(StringLiteral, check_define_literal)
{
  // error: ‘name’ does not name a type; did you mean ‘nanq’?
  //
  // constexpr name{"this is name"};
  // constexpr name = "this is name";
  // constexpr name{"this is name"s};
  // constexpr name = "this is name"s;

  // error: the type ‘const string {aka const std::__cxx11::basic_string<char>}’
  // of constexpr variable ‘name’ is not literal
  // {
  //   constexpr std::string name{"this is name"};
  // }

  // using namespace std::literals;
  // defined as std::string and can use its member.
  auto name = "this is name"s;
  EXPECT_THAT(name.size(), 12);
}

// outupt string:
//
// { "address": {
//          "road":"Drury Ln",
//          "city":"Fountain",
//          "state":"CO",
//          "country":"US" }}

TEST(StringLiteral, check_raw)
{
  {
    std::ostringstream os1;
    std::ostringstream os2;

    const char message[] = "this is \
                           a multi-line message \
                           and works";
    os1 << message;

    os2 << "this is                            a multi-line message            "
           "                and works";

    EXPECT_THAT(os1.str(), os2.str());
  }

  // they are not the same since string-raw includes `space` as well.
  {
    std::string coll1{R"(1

    22

    333)"};

    std::string coll2{"1\n22\n333"};

    // std::cout << coll1 << std::endl;
    // std::cout << coll2 << std::endl;
  }

  // string-raw works like extended-regex
  {
    std::string coll1("~Query(\"hair\")");
    std::string coll2(R"(~Query("hair"))");

    EXPECT_THAT(coll1, coll2);
  }

  // multi-lines
  {
    std::ostringstream os1;
    std::ostringstream os2;

    // different as do not include `spaces`
    // { "address": {"road":"Drury Ln","city":"Fountain","state":"CO","country":"US" }}
    // const char outx[] = "{ \"address\": {"
    //         "\"road\":\"Drury Ln\","
    //         "\"city\":\"Fountain\","
    //         "\"state\":\"CO\","
    //         "\"country\":\"US\" }}";
    // std::cout << outx << std::endl;

    // have to use `escape` like extended-regex and `newline` but note that it
    // includes `spaces`

    const char out[] = "{ \"address\": {\n\
                        \"road\":\"Drury Ln\",\n\
                        \"city\":\"Fountain\",\n\
                        \"state\":\"CO\",\n\
                        \"country\":\"US\" }}";

    os1 << out;

    // raw-string is better
    const std::string s = R"({ "address": {
         "road":"Drury Ln",
         "city":"Fountain",
         "state":"CO",
         "country":"US" }})";
    os2 << out;

    EXPECT_THAT(os1.str(), os2.str());
  }

  {
    // compile error:
    // std::string json_content1 = R"(
    // {
    //     "Title":"(C/C++)",
    //     "Subtitle":"(Powered by C/C++)",
    //     "Description":"(The world of C/++ developers)",
    //     "MainPage":"cpp",
    //     "Items":null,
    //     "Id":"6"
    // })";

    // okay since do not have "(", ")" in it
    std::string json_content1 = R"(
    {
        "Title":"C/C++",
        "Subtitle":"Powered by C/C++",
        "Description":"The world of C/++ developers",
        "MainPage":"cpp",
        "Items":null,
        "Id":"6"
    })";

    // https://www.drdobbs.com/cpp/new-c-language-features-in-visual-studio/240165945
    // prefix(optional) R "delimiter( raw_characters )delimiter"	(6)	(since C++11)
    //
    // The use of ZZZ as a delimiter in the following lines solves the problem
    // and allows you to embed double quotation marks and parenthesis in the raw
    // literal string:
    //
    // https://www.youtube.com/watch?v=DiZ-az_nJMM

    std::string json_content2 = R"ZZZ(
    {
        "Title":"(C/C++)",
        "Subtitle":"(Powered by C/C++)",
        "Description":"(The world of C/++ developers)",
        "MainPage":"cpp",
        "Items":null,
        "Id":"6"
    })ZZZ";

    std::string json_content3 = R"JSON(
    {
        "Title":"(C/C++)",
        "Subtitle":"(Powered by C/C++)",
        "Description":"(The world of C/++ developers)",
        "MainPage":"cpp",
        "Items":null,
        "Id":"6"
    })JSON";

    // well, don't need to use `delimeter` for json since do not have
    // parenthesis.

    std::string json_content4 = R"(
    {
      "uris": [
        {
          "path": "/as/drm/status",
          "method": "ws",
          "thread": "AS_WS_DRM",
          "content": {
            "status": "UNAVAILABLE"
          }
        }
      ]
    })";
  }
}

// ={=========================================================================
// 4.11 Removing a Substring from a String

void removeSubstrs1(std::string &s, const std::string &p)
{
  auto start = s.find(p);

  if (start != std::string::npos)
  {
    auto first  = s.substr(0, start);
    auto second = s.substr(start + p.size());

    s = first + second;
  }
}

void removeSubstrs2(std::string &s, const std::string &p)
{
  auto start = s.find(p);

  if (start != std::string::npos)
    s.erase(start, p.size());
}

void removeSubstrs3(std::string &s, const std::string &p)
{
  auto size = p.size();

  for (auto start = s.find(p); start != std::string::npos; start = s.find(p))
    s.erase(start, size);
}

TEST(String, RemoveSubString)
{
  string s1{"One fish, two fish, red fish, blue fish"};
  string r1{"One , two fish, red fish, blue fish"};
  string p{"fish"};

  removeSubstrs1(s1, p);

  string s2{"One fish, two fish, red fish, blue fish"};

  removeSubstrs2(s2, p);

  EXPECT_TRUE(s1 == r1);
  EXPECT_TRUE(s2 == r1);

  string s3{"One fish, two fish, red fish, blue fish"};
  string r3{"One , two , red , blue "};

  removeSubstrs3(s3, p);
  EXPECT_THAT(s3, Eq(r3));
}

// ={=========================================================================
// 4.12 Converting a String to Lower- or Uppercase

void toUpper(std::string &s)
{
  for (auto i = s.begin(); i != s.end(); ++i)
    *i = toupper(*i);
}

void toLower(std::string &s)
{
  for (auto i = s.begin(); i != s.end(); ++i)
    *i = tolower(*i);
}

TEST(String, ConvertStringToLowerUpperCase)
{
  string s{"shazam"};

  toUpper(s);
  EXPECT_THAT(s, Eq("SHAZAM"));

  toLower(s);
  EXPECT_THAT(s, Eq("shazam"));
}

// ={=========================================================================
// 4.14 Doing a Case-Insensitive String Search

// modifies s and assumes p is upper-case
bool caseInsFind1(string &s, const string &p)
{
  toUpper(s);

  return s.find(p) != std::string::npos;
}

// do not modify s
bool caseInsFind2(string &s, const string &p)
{
  auto it =
    search(s.begin(), s.end(), p.begin(), p.end(), [](char lhs, char rhs) {
      return toupper(lhs) == rhs;
    });

  return it != s.end();
}

TEST(String, SearchCaseInsensitive)
{
  string s1      = "row, row, row, your boat";
  const string p = "YOUR";

  // // returns 15
  // cout << caseInsFind(s, p) << endl;

  EXPECT_TRUE(caseInsFind1(s1, p));
  cout << s1 << endl;

  string s2 = "row, row, row, your boat";
  EXPECT_TRUE(caseInsFind2(s2, p));
  cout << s2 << endl;
}

// ={=========================================================================
// 4.15 Converting Between Tabs and Spaces in a Text File

TEST(String, ConvertTabToSpace)
{
  ifstream input_file{"input.txt"};
  ofstream output_file{"output_space.txt"};
  char read_char{};

  while (input_file.get(read_char))
  {
    if (read_char == '\t')
      output_file << "   "; // 3 spaces
    else
      output_file << read_char;
  }
}

// the tricky bit is that:
//
// For all other characters, or for fewer than three spaces, whatever is read
// from the input stream is written to the output stream.
//
// that means that have to remember when see spaces less than 3 and output that
// spaces.
//
// ss...sss...s...
//
// sssss...sssssssssss....
//
// the both case is when see not-space char

TEST(String, ConvertSpaceToTab)
{
  ifstream input_file{"output_space.txt"};
  ofstream output_file{"output_tab.txt"};
  char read_char{};
  size_t space_count{};

  while (input_file.get(read_char))
  {
    // if (read_char == '\s')
    if (read_char == ' ')
    {
      ++space_count;

      if (3 == space_count)
      {
        // output_file << '\t';
        output_file.put('\t');
        space_count = 0;
      }
    }
    else
    {
      if (space_count)
      {
        for (size_t i = 0; i < space_count; ++i)
          output_file.put(' ');

        space_count = 0;
      }

      output_file << read_char;
    }
  }
}

// //
// ={=========================================================================
// // 4.16 Wrapping Lines in a Text File
//
// // Two requirements:
// //
// // For example, if you want to wrap text at 72 characters, you would insert a
// // new-line character after every 72 characters in the file.
// //
// // If the file contains human-readable text, you probably want to avoid
// // splitting words.
// //
// // works on a word unit and tmp keeps current word.
// //
// // whenever see a word, current_char is space and last_char is char, write it
// // and clear tmp.
// //
// // cc cccc ccccc cccc[width to wrap]
// //  ^^ - current_char
// //  +--- last_char
// //
// // at the wrap point, write a '\n' and last word. update i since last word is
// // already written on the new line.
//
// void testWrap(istream &in, ostream &out, size_t width)
// {
//     string tmp{};
//     char current_char = '\0';
//     char last_char = '\0';
//     size_t i = 0;
//
//     while (in.get(current_char))
//     {
//         // increase i on every iteration
//         if (++i == width)
//         {
//             ltrimws(tmp);
//             out << '\n'
//                 << tmp;
//             i = tmp.length();
//             tmp.clear();
//         }
//         else if (isspace(current_char) && !isspace(last_char))
//         {
//             out << tmp;
//             tmp.clear();
//         }
//
//         // Since the original approach do not work well in dealing with new
//         line in
//         // the input, this is an attempt to fix. not sure if it works. no
//         further
//         // effort and stops here.
//         //
//         // if (current_char == '\n')
//         //   current_char = ' ';
//
//         tmp += current_char;
//         last_char = current_char;
//     }
// }
//
// TEST(String, WrapLinesInTextFile)
// {
//     ifstream input_file{"input.txt"};
//     ofstream output_file{"output_wrap.txt"};
//
//     testWrap(input_file, output_file, 72);
// }

// ={=========================================================================
// 4.17 Counting the Number of Characters, Words,
// and Lines in a Text File

// kyoupark@kit-debian64:~/git/kb/code-cxx/t_ex_string$ wc input.txt
//   3  23 152 input.txt
//
// String.CountStuffInTextFile
//
// cc: 152
// wc: 23
// lc: 3
//
// String.CountStuffInTextFile2
//
// cc: 149
// wc: 23
// lc: 4

TEST(String, CountStuffInTextFile)
{
  ifstream input_file{"input.txt"};
  size_t word_count{};
  size_t char_count{};
  size_t line_count{};
  char current_char{};
  char last_char{' '};
  string tmp{};

  while (input_file.get(current_char))
  {
    // to count word when current is space and last previous is not space.
    //
    // this has +1 word than wc command since when the input line is:
    // [space]xxx xxx

    // this first space is counted as a word as isspace(curr) is true and
    // isspace(last) is false. To fix this, set last_char's init value.

    if ((isspace(current_char) || current_char == '\n') && !isspace(last_char))
    {
      // cout << "w: " << tmp << endl;
      // tmp.clear();
      ++word_count;
    }

    // to count line
    if (current_char == '\n')
      ++line_count;

    tmp += current_char;
    last_char = current_char;
    ++char_count;
  }

  cout << "cc: " << char_count << endl;
  cout << "wc: " << word_count << endl;
  cout << "lc: " << line_count << endl;
}

TEST(String, CountStuffInTextFile2)
{
  ifstream input_file{"input.txt"};

  char cur  = '\0';
  char last = '\0';
  int chars = 0;
  int words = 0;
  int lines = 0;

  while (input_file.get(cur))
  {
    if (cur == '\n' || (cur == '\f' && last == '\r'))
      lines++;
    else
      chars++;
    if (!std::isalnum(cur) && // This is the end of a
        std::isalnum(last))   // word
      words++;
    last = cur;
  }
  if (chars > 0)
  {                         // Adjust word and line
    if (std::isalnum(last)) // counts for special
      words++;              // case
    lines++;
  }

  cout << "cc: " << chars << endl;
  cout << "wc: " << words << endl;
  cout << "lc: " << lines << endl;
}

// ={=========================================================================
// 4.18 Counting Instances of Each Word in a Text File

TEST(String, CountWordsInTextFile)
{
  ifstream input_file{"input.txt"};
  size_t word_count{};
  string word{};
  std::map<std::string, size_t> wm{};

  while (input_file >> word)
  {
    ++wm[word];
  }

  for (const auto &e : wm)
  {
    (void)e;
    ++word_count;
  }

  cout << "word count: " << word_count << endl;
}

// ={=========================================================================
// 4.19 Add Margins to a Text File

// ={=========================================================================
// 4.20 Justify a Text File

// Note that I set the width inside the loop while I set the right flag prior to
// the loop. I had to do this because the width resets to zero after each write
// to the stream. Format flags are not reset after writes, so I only had to
// initialize them once and be done with it.

TEST(String, JustifyTextFile)
{
  ifstream input_file{"input.txt"};
  ofstream output_file{"justified.txt"};

  size_t width{72};
  string line{};

  output_file.setf(ios_base::right);

  while (input_file)
  {
    output_file.width(width);
    getline(input_file, line);
    output_file << line << '\n';
  }
}

// ={=========================================================================
// 4.21 Squeeze Whitespace to Single Spaces in a Text File
// You have a text file with whitespace of varying lengths in it, and you want
// to reduce every occurrence of a contiguous span of whitespace characters to a
// single space.

TEST(String, SqueezeWhitespaceInTextFile)
{
  ifstream input_file{"input.txt"};
  ofstream output_file{"squeezed.txt"};

  char current_char{};
  char saved_char{};

  while (input_file.get(current_char))
  {
    // save up only on whitespace
    if (isspace(current_char))
    {
      if (saved_char != current_char)
        saved_char = current_char;
    }
    // output saved whitespace when see nonwhitespace char on input
    else
    {
      if (isspace(saved_char))
      {
        output_file << saved_char;
        saved_char = 0;
      }

      output_file << current_char;
    }
  }
}

// operator>>() ignores whitespace, so all I have to do is add a space and each
// chunk of nonwhitespace.

TEST(String, SqueezeWhitespaceInTextFileTwo)
{
  ifstream input_file{"input.txt"};
  ofstream output_file{"squeezed_two.txt"};

  string tmp;

  // grab the first word
  input_file >> tmp;
  output_file << tmp;

  while (input_file >> tmp)
  {
    output_file << ' ';
    output_file << tmp;
  }
}

// ={=========================================================================
// 4.22 Autocorrect Text as a Buffer Changes
// You have a class that represents some kind of text field or document, and as
// text is appended to it, you want to correct automatically misspelled words
// the way Microsoft Word’s Autocorrect feature does.

typedef map<std::string, std::string> StrStrMap;

class TextAutoCorrectField
{
public:
  TextAutoCorrectField() = delete;
  TextAutoCorrectField(const StrStrMap *p)
      : pdict_(p)
  {}

  void append(char c);
  void getText(string &s) { s = buf_; }

private:
  string buf_;
  const StrStrMap *pdict_;
};

void TextAutoCorrectField::append(char c)
{
  // only do when see a word boundary that is a sequence of a char and space or
  // punctions.

  if ((isspace(c) || ispunct(c)) && buf_.length() > 0 &&
      !isspace(buf_[buf_.length() - 1]))
  {
    // Is there ever case where buf_ contails multiple words seperated by ws?
    // in other words, is there any case when i is not 0?
    // yes, there is since buf_ of TextAutoCorrectField becomes corrected line
    // but not a word, so we have to work on the last chunk which just becomes a
    // word. i is the start index of this word.

    auto x = buf_.find_last_of(" \f\n\r\t\v");
    auto i = (x == string::npos) ? 0 : ++x;

    // std::cout << "TextAutoCorrectField::append: i : " << i << std::endl;

    // take out the last word and look it up in the map
    string word                     = buf_.substr(i, buf_.length() - i);
    StrStrMap::const_iterator found = pdict_->find(word);

    if (found != pdict_->end())
    {
      buf_.erase(i, buf_.length() - i);
      buf_ += found->second;
    }
  }

  buf_ += c;
}

TEST(String, AutoCorrectField)
{
  StrStrMap dict{};
  TextAutoCorrectField auto_text(&dict);

  // see that update a map after passed in to TextAutoCorrectField

  dict["taht"]  = "that";
  dict["right"] = "wrong";
  dict["bug"]   = "feature";

  string given_text = "He's right, taht's a bug.";

  std::cout << "original:  " << given_text << std::endl;

  for (const auto e : given_text)
    auto_text.append(e);

  string result_text{};
  auto_text.getText(result_text);

  std::cout << "corrected: " << result_text << std::endl;
}

// ={=========================================================================
// 4.23 Reading a Comma-Separated Text File
// You want to read in a text file that is delimited by commas and new lines (or
// any other pair of delimiters for that matter). Records are delimited by one
// character, and fields within a record are delimited by another. For example,
// a comma-separated text file of employee information may look like the
// following:
// Smith, Bill, 5/1/2002, Active
// Stanford, John, 4/5/1999, Inactive
// Such files are usually interim storage for data sets exported from
// spreadsheets, databases, or other file formats.

void loadCSV(ifstream &ifs, vector<vector<string> *> &data)
{
  using namespace stringsplit_2018_10;

  string line{};

  while (ifs)
  {
    getline(ifs, line);

    auto coll = new vector<string>();

    split_revised(line, ',', *coll);

    data.push_back(coll);

    line.clear();
  }
}

TEST(String, ReadCsvFile)
{
  ifstream ifs{"input.csv"};

  vector<vector<string> *> data;

  loadCSV(ifs, data);

  // loop on vector pointer array
  for (auto e : data)
  {
    // loop on string vector
    for (auto i : *e)
    {
      cout << "|" << i;
    }
    cout << "|" << endl;

    delete e;
  }
}

// ={=========================================================================
// 4.24 Using Regular Expressions to Split a String
// Use Boost’s regex class template. regex enables the use of regular
// expressions on string and text data.

// NOTE: disabled since it causes seg fault(core dumped) sometimes. Need to
// debug but not now.

TEST(DISABLED_String, BoostRegex)
{
  string s = "who, lives:in-a,pineapple    under the sea?";

  // create the reg ex
  boost::regex re(",|:|-|\\s+");
  // create a iterator using a sequence and that reg ex
  boost::sregex_token_iterator p(s.begin(), s.end(), re, -1);
  // create an end of reg ex marker
  boost::sregex_token_iterator end;

  while (p != end)
    cout << *p++ << endl;
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
