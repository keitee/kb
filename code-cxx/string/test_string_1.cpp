#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <bitset>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <regex>

#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using testing::ElementsAre;
using testing::Eq;
using testing::StrEq;
using testing::FloatEq;


// ={=========================================================================
// string-pos

TEST(StringIterator, PosEnd)
{
  string coll{"12345"};

  EXPECT_EQ(coll.size(), 5);
  EXPECT_EQ(coll.length(), 5);

  // this is error since iterator is not int
  // EXPECT_EQ(coll.end(), 5);
  
  EXPECT_EQ(*coll.end(), '\0');
}


// ={=========================================================================
// string-ctors
//
// *cxx-string-ctor*

TEST(String, Ctors)
{
  {
    string s("nico");
    EXPECT_THAT(s.length(), 4);
  }

  {
    string s(5, 'c');
    EXPECT_THAT(s, ElementsAre('c', 'c', 'c', 'c', 'c'));
  }

  {
    string s(5, 'c');
    EXPECT_THAT(s, "ccccc");
  }

  {
    string s{"zoo"};
    EXPECT_EQ(s, "zoo");
  }

  {
    string s("zoo");
    EXPECT_EQ(s, "zoo");
  }
}


TEST(String, CtorsConst)
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
    // t_string.cpp:29:49: error: invalid conversion from ‘char’ to ‘const char*’ [-fpermissive]
    //
    // const char const_letter = 'A';
    // string string_from_const_letter(const_letter);
    //
    // char non_const_letter = 'A';
    // string string_from_non_const_letter(non_const_letter);
  }

  // *cxx-string-const-ctor-const*

  // ctor and append cause error:
  //
  // {
  //   char letter = 'A';
  //
  //   // string.cpp:99:20: error: invalid conversion from ‘char’ to ‘const char*’ [-fpermissive]
  //   //      string s(letter);
  // 
  //   string s(letter);
  // }
  //
  // {
  //   char letter = 'A';
  //
  //   // string.cpp:99:20: error: invalid conversion from ‘char’ to ‘const char*’ [-fpermissive]
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

TEST(CxxString, Assign)
{
  std::string coll;

  coll = "assign string";

  EXPECT_THAT(coll, "assign string");
}

// ={=========================================================================
// string-operations string-back

TEST(StringOperation, Back)
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


// string-maxsize

TEST(StringOperation, MaxSize)
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
// string-substring string-find string-substr

TEST(CxxStringOperation, SubstringFind)
{
  // cxx-srring-find
  // http://www.cplusplus.com/reference/string/string/find/
  {
    std::string coll1{"There are two needles in this haystack with needles."};
    std::string coll2{"needle"};

    std::size_t found = coll1.find(coll2);
    EXPECT_THAT(found, 14);

    // start from `found+1` and for 6 chars long
    found = coll1.find("needles are small", found + 1, 6);
    EXPECT_THAT(found, 44);

    found = coll1.find("haystack");
    EXPECT_THAT(found, 30);

    found = coll1.find('.');
    EXPECT_THAT(found, 51);

    found = coll1.find("park");
    EXPECT_THAT(found, string::npos);

    // since npos is unsigned
    found = coll1.find("park");
    EXPECT_THAT(found, -1);
  }

  // cxx-srring-rfind
  // https://en.cppreference.com/w/cpp/string/basic_string/rfind
  //
  // Finds the last substring equal to the given character sequence. Search
  // begins at pos, i.e. the found substring must not begin in a position
  // following pos.
  //
  // Return value
  // Position of the first character of the found substring

  {
    std::string coll1{"The sixth sick sheik's sixth sheep's sick."};
    std::string coll2{"sixth"};
    std::string coll3{"The sixth sick sheik's seventh sheep's sick."};

    auto found = coll1.rfind(coll2);
    coll1.replace(found, coll2.length(), "seventh");
    EXPECT_THAT(coll1, coll3);

    found = coll1.rfind(coll2, 0);
    EXPECT_THAT(found, string::npos);

    // starts from 0 and see match 'T' at 0. returns 0
    found = coll1.rfind("The", 0);
    EXPECT_THAT(found, 0);

    // starts from 2 and see match 'The'. returns 0 since 0 is the first char
    found = coll1.rfind("The", 2);
    EXPECT_THAT(found, 0);

    found = coll1.rfind("The", 3);
    EXPECT_THAT(found, 0);

    found = coll1.rfind("The", 4);
    EXPECT_THAT(found, 0);

    found = coll1.rfind("The", 6);
    EXPECT_THAT(found, 0);

    // 
    found = coll1.rfind("sixth", 0);
    EXPECT_THAT(found, string::npos);

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
}

// is to extract substring
TEST(CxxStringOperation, SubstringSubStr)
{
  std::string coll{"interchangeability"};

  // see `cxx-string-length` returns length which is [0, length)
  EXPECT_THAT(coll.length(), 18); 

  // return whole string
  EXPECT_THAT(coll.substr(), "interchangeability");

  // return whole string as find('x') returns string::npos
  EXPECT_THAT(coll.substr(0, coll.find('x')), "interchangeability");

  EXPECT_THAT(coll.substr(coll.find('c')), "changeability");

  EXPECT_THAT(coll.substr(11), "ability");

  // note that the second is `length`
  EXPECT_THAT(coll.substr(5,6), "change");
}


// string-insert

TEST(StringOperation, Insert)
{
  // *cxx-string-const-error*
  //
  // char x;
  // string.push_back(x) causes error since x is not const char *. To add char,
  // use
  //
  // string& string::insert (size_type idx, size_type num, char c);
  
  {
    string s{};
    s.insert(0, 1, 'X');
    s.insert(0, 1, 'X');
    s.insert(0, 1, 'X');
    EXPECT_EQ(s, "XXX");
  }
  {
    string s{};
    s.insert(0, 1, 'X');
    s.insert(0, 1, 'Y');
    s.insert(0, 1, 'Z');
    EXPECT_EQ(s, "ZYX");
  }
}

// string-erase, string-replace

TEST(StringOperation, EraseReplace)
{
  {
    std::string s = "i18n";

    s.replace(1,2,"nternationalizatio");   // change of size
    EXPECT_EQ(s, "internationalization");

    s.erase(13);                           // s: international. note: from 13
    EXPECT_EQ(s, "international");

    s.erase(7, 5);                         // internaxxxxxl
    EXPECT_EQ(s, "internal");

    // *cxx-string-pop-back*
    // void string::pop_back ()
    // Calling this for an empty string results in undefined behavior.

    s.pop_back();                          // s: interna (since C++11)
    EXPECT_EQ(s, "interna");

    s.replace(0, 2,"ex");                   // s: externa
    EXPECT_EQ(s, "externa");
  }

  {
    std::string s = "i18n";

    s.replace(1,2,"nternationalizatio");   // change of size
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
    s.replace(0, 5,"nicholas");
    EXPECT_EQ(s, "nicholas Stroustrup");
  }
}


// string-assign, string-swap

TEST(StringOperation, AssignSwap)
{
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

  {
    std::string s{};

    s.assign("othello");
    EXPECT_EQ(s, "othello");

    s.assign("the");
    EXPECT_EQ(s, "the");
  }
}


// string-compare
//
// int compare( const basic_string& str ) const; (until C++11)
// int compare( const basic_string& str ) const noexcept; (since C++11)
//
// Return value
// negative value if *this appears before the character sequence specified by
// the arguments, in lexicographical order
//
// zero if both character sequences compare equivalent
//
// positive value if *this appears after the character sequence specified by the
// arguments, in lexicographical order

TEST(StringOperation, Compare)
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

  {
    std::string coll1{};
    std::string coll2{"string compare"};

    // equal
    EXPECT_THAT(false, (coll1 == coll2));
  }
}


TEST(StringOperation, Add)
{
  std::string coll1{};
  
  coll1 = "string";
  coll1 += " add";

  std::string coll2{"string add"};

  EXPECT_THAT(coll1, coll2);
}

// cxx-string-clear
//
// void clear(); (until C++11)
// void clear() noexcept;

TEST(CxxString, StringClear)
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

TEST(String, CompareStringAndCstring)
{
  string sz{"zoo"};
  EXPECT_EQ(sz.size(), 3);

  char cs[] = "zoo";
  EXPECT_EQ(strlen(cs),3);
  EXPECT_EQ(sizeof(cs)/sizeof(char),4);

  const char *s1  = "this is first message";
  const char s2[] = "this is first message";

  EXPECT_THAT(sizeof(s1), 8);                 // pointer
  EXPECT_THAT(sizeof s1, 8);                  // pointer
  EXPECT_THAT(sizeof *s1, 1);                 // byte
  EXPECT_THAT(sizeof(s2), 22);                // array
  EXPECT_THAT(sizeof(s2)/sizeof(s2[0]), 22);  // array
  EXPECT_THAT(strlen(s2), 21);

  string ss{s1};
  EXPECT_THAT(ss.size(), 21);                  // string
}


// return bool if source and target are the same from the end.
// bool strend(char *s, char *t);

namespace string_cstring 
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
} // namespace

// strend(this is first message, ssage) returns 1
// strend(this is first message, xsage) returns 0
// strend(this is first message, ssage) returns 1
// strend(this is first message, xsage) returns 0

TEST(String, CompareStringFromEnd)
{
  using namespace string_cstring; 

  char s1[] = "this is first message";
  char t1[] = "ssage";
  char t2[] = "xsage";

  EXPECT_EQ(true, strend_01(s1, t1));
  EXPECT_EQ(false, strend_01(s1, t2));

  EXPECT_EQ(true, strend_02(s1, t1));
  EXPECT_EQ(false, strend_02(s1, t2));
}

// why this works? since there is std::string ctor(const char*)

TEST(String, OutputCstring)
{
  ostringstream os;

  char s1[] = "this is first message";
  // cout << s1 << endl;
  os << s1 << endl;
  EXPECT_EQ(os.str(), "this is first message\n");
}


// ={=========================================================================
// string-conversion

TEST(StringConverison, Functions)
{
  // to string
  {
    EXPECT_THAT(to_string(11), "11");
    EXPECT_THAT(to_string(3301), "3301");
  }

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
    // The string may begin with an arbitrary amount of white space (as deter‐
    // mined by isspace(3)) followed by a single optional '+' or '-' sign.  

    // If base is zero or 16, the string may then include a "0x" prefix, and
    // the number  will  be read in base 16; otherwise, a zero base is taken
    // as 10 (decimal) unless the next character is '0', in which case it  is
    // taken as 8 (octal).

    EXPECT_EQ(std::stol("0x12AB", nullptr, 0), 4779);
    EXPECT_EQ(std::stol("12AB", nullptr, 16), 4779);

    // *cxx-string-convert-to-string* *cxx-limits*
    long long ll = std::numeric_limits<long long>::max();
    EXPECT_EQ(std::to_string(ll), "9223372036854775807");

    // try to convert back and throws out_of_range
    EXPECT_THROW(std::stoi("9223372036854775807"), std::out_of_range);
  }
}

TEST(StringConverison, StringStream)
{
  // note that os, buffer, has all inputs from << and seek() moves writing pos.
  // *cxx-string-convert-to-string*

  // to string
  {
    ostringstream os;

    os << "decimal : " << 15 << hex << ", hex : " << 15 << endl;
    EXPECT_EQ(os.str(), "decimal : 15, hex : f\n");

    bitset<15> bit_set(5789);
    os << "float : " << 4.67 << ", bitset : " << bit_set << endl;
    EXPECT_EQ(os.str(), "decimal : 15, hex : f\nfloat : 4.67, bitset : 001011010011101\n");

    os.seekp(0);
    os << "octal : " << oct << 15;
    EXPECT_EQ(os.str(), "octal : 1715, hex : f\nfloat : 4.67, bitset : 001011010011101\n");
  }

  // to string
  {
    stringstream ss;
    vector<string> string_vector{};

    for(int i = 0; i < 4; ++i)
    {
      ss << "player " << i;
      string_vector.push_back(string(ss.str()));
      ss.str("");
    }

    EXPECT_THAT(string_vector, ElementsAre("player 0", "player 1", "player 2", "player 3"));
  }

  // to number
  {
    // The following lines read the integer x with the value 3 and the
    // floating-point f with the value 0.7 from the stringstream:

    int x{};
    float f{};

    istringstream is{"3.7"};

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

  // to number
  {
    stringstream is{"1 2 3 4"};
    int value{};
    vector<int> coll{};

    while (is >> value)
      coll.push_back(value);

    EXPECT_THAT(coll, ElementsAre(1,2,3,4));
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

TEST(StringConverison, BooxtLexicalCast)
{
  EXPECT_THAT(boost::lexical_cast<std::string>(11), "11");
  EXPECT_THAT(boost::lexical_cast<std::string>(3301), "3301");

  EXPECT_THAT(boost::lexical_cast<int>("11"), 11);
  EXPECT_THAT(boost::lexical_cast<int>("3301"), 3301);
}


TEST(StringConverison, NarrowNumericConversion)
{
  // warning/error
  // :464:12: warning: narrowing conversion of ‘5.0e+0’ from ‘double’ to ‘int’ inside { } [-Wnarrowing]
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
    int start = s.size()-1;

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
    s.erase(start+1);
  }

  // if use iterators
  void trim_2(string &s, const char c)
  {
    if (s.empty()) return;

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

} // namespace


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
    trim_1(s1,' ');
    EXPECT_THAT(s1, "zoo");
  }

  {
    string s1{"zoo"};
    trim_1(s1,' ');
    EXPECT_THAT(s1, "zoo");
  }

  {
    string s1{"ooo"};
    trim_1(s1,'o');
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
    trim_2(s1,' ');
    EXPECT_THAT(s1, "zoo");
  }

  {
    string s1{"zoo"};
    trim_2(s1,' ');
    EXPECT_THAT(s1, "zoo");
  }

  {
    string s1{"ooo"};
    trim_2(s1,'o');
    EXPECT_THAT(s1, "");
  }
}


// ={=========================================================================
// string-split 
//
// 4.6 Splitting a String
// You want to split a delimited string into multiple strings. For example, you
// may want to split the string "Name|Address|Phone" into three separate
// strings, "Name", "Address", and "Phone", with the delimiter removed.


// *cxx-string-find* *cxx-string-substr* *cxx-string-replace*
//
// CXXSLR 13.1.1 A First Example: Extracting a Temporary Filename
// make the original example short.

TEST(StringSplit, GetExtension)
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
      extname = inputname.substr(idx+1);
      filename = basename + '.' + suffix;

      // to use cxx-string-replace
      if (extname == suffix)
      {
        filename = inputname;
        filename.replace(idx+1, string::npos, "xxx");
      }
    }

    coll2.push_back(filename);
  }

  EXPECT_THAT(coll2, 
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
  // since `found` is npos when not found and do ++found and use it loop condition.
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
    string::size_type i = 0;
    string::size_type found = s.find(delim);

    while (found != string::npos)
    {
      // save the first token
      coll.push_back(s.substr(i, found-i));
      // std::cout << "token: " << s.substr(i, found-i) << std::endl;
      i = ++found;
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
      coll.push_back(input.substr(i, found-i));
      i = ++found;
      found = input.find(delim, i);
    }

    if (found == string::npos)
      coll.push_back(input.substr(i, found-i));
  }
} // namespace


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

    boost::split(coll, "Smith, Bill, 5/1/2002, Active", 
        boost::is_any_of("|"));

    // not found and returns the input
    EXPECT_THAT(coll, ElementsAre("Smith, Bill, 5/1/2002, Active"));
  }

  {
    vector<string> coll;

    boost::split(coll, "Smith, Bill, 5/1/2002, Active", 
        boost::is_any_of(","));

    EXPECT_THAT(coll, ElementsAre("Smith", " Bill", " 5/1/2002", " Active"));
  }

  {
    vector<string> coll;

    boost::split(coll, "Name|Address|Phone", 
        boost::is_any_of("|"));

    EXPECT_THAT(coll, ElementsAre("Name", "Address", "Phone"));
  }

  // *cxx-boost-is-any-of* for multiple delimeters
  {
    vector<string> coll;

    boost::split(coll, "Name|Address,Phone", 
        boost::is_any_of("|,"));

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


namespace stringsplit_2018_11 {

  // 2018.11.14

  void split(const string &s, const char delim, vector<string> &coll)
  {
    string token{};
    size_t start{}, end{}; 

    end = s.find(delim, start);
    for (; end < string::npos;)
    {
      coll.push_back(s.substr(start, end - start));
      start = end+1;
      end = s.find(delim, start);
    }

    coll.push_back(s.substr(start, end - start));
  }

} // namespace


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


namespace stringsplit_2018_11_strchr {

  // 2018.11.14
  // should try memchar/strchr way? nope.
  
} // namespace

namespace string_split_2018_11 {

  // copied from TEST(Stdio, GetLine)

  // {
  //   string s{};
  //   stringstream iss("one|two|three");
  //   vector<string> coll{};

  //   while (getline(iss , s, '|'))
  //     coll.push_back(s);

  //   EXPECT_THAT(coll, ElementsAre("one", "two", "three"));
  // }
  
} // namespace


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
        end_ = str_.find_first_of(delim_, begin_);
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
            s = str_.substr(begin_, end_ - begin_);
            begin_ = str_.find_first_not_of(delim_, end_);
            end_ = str_.find_first_of(delim_, begin_);
        }
        else if (begin_ != string::npos && end_ == string::npos)
        {
            s = str_.substr(begin_, str_.length() - begin_);
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
        size_t pos_delim = 0;

        for (;;)
        {
            // break if there are only delimiters.
            if ((pos_delim = str_.find_first_not_of(delim_, pos_delim)) == string::npos)
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

    bool hasMoreTokens()
    {
        return begin_ != end_;
    }

  private:
    StringTokenizer() {}
    string delim_;
    string str_;
    size_t count_{};
    size_t begin_{};
    size_t end_{};
};

} // namespace


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
      MakeToken_0529(const string &input, const string &delim = "|"):
        input_(input), delim_(delim)
        {
          size_t i{};
          size_t found = input_.find_first_of(delim_, i);

          while (found != string::npos)
          {
            tokens_.push_back(input_.substr(i, found-i));
            i = ++found;
            found = input_.find_first_of(delim_, i);

          }

          if (found == string::npos)
            tokens_.push_back(input_.substr(i, found));
        }

      size_t countTokens()
      {
        return tokens_.size();
      }

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

} // namespace


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
      : input_(input), delim_(delim)
    {
      pos_token_ = input_.find_first_not_of(delim_, 0);
      pos_delim_ = input_.find_first_of(delim_, pos_token_);
    }

    bool hasMoreTokens()
    {
      return pos_token_ != pos_delim_;
    }

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
      
      while( (pos_delim = input_.find_first_of(delim_, pos_start)) != string::npos)
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

} // namespace


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


namespace tokenizer_2018_11_split {

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
      auto end = s.find(delim);
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
        end = s.find(delim, start);
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

} // namespace


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

namespace tokenizer_2018_11_find {

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
      : input_(s), delim_(delim)
      {
        count_ = 0;
        start_ = input_.find_first_not_of(delim_);
        end_ = input_.find_first_of(delim_, start_);
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
          s = input_.substr(start_, end_ - start_);
          start_ = input_.find_first_not_of(delim_, end_);
          end_ = input_.find_first_of(delim_, start_);
        }
        else if (start_ != string::npos && end_ == string::npos)
        {
          s = input_.substr(start_, end_ - start_);
          start_ = input_.find_first_not_of(delim_, end_);
        }
      }

      bool hasMoreTokens()
      {
        return start_ != end_;
      }

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

} // namespace


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

namespace string_parse {

  // from asan tool
  
  // usage:
  //
  // const char *current = "08048000-08056000 r-xp 00000000 03:0c 64593   /foo/bar\n";
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
      value = base*value + digit;
      // increase pointer to process next char
      (*p)++;
    }

    return value;
  }

  int ParseHex(const char **p)
  {
    return ParseNumber(p, 16);
  }

  int ParseDecimal(const char **p)
  {
    return ParseNumber(p, 10);
  }

  bool IsOneOf(char c, char c1, char c2)
  {
    return (c == c1) || (c == c2);
  }

} // namespace


TEST(StringParse, Hex)
{
  using namespace string_parse;
 
  // example from /proc/map
  // address           perms offset  dev   inode       pathname
  // 00400000-00452000 r-xp 00000000 08:02 173521      /usr/bin/dbus-daemon

  int value{}, protection{};
  char filename[100]={};

  const char *current = "08048000-08056000 r-xp 00000000 03:0c 64593   /foo/bar\n";

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
    if ( /* filename && */ i < 100 /* filename_size-1 */)
      filename[i++] = *current;

    ++current;
  }

  if ( /* filename && */ i < 100 /* filename_size-1 */)
    filename[i] = 0;

  EXPECT_STREQ(filename, "/foo/bar");
}


// ={=========================================================================
// string-join

void join(const std::vector<std::string> &vector, const char delim, std::string &joined)
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
  std::vector<std::string> coll{ "this","is","an","example" };

  copy(coll.begin(), coll.end(), ostream_iterator<string>(os, ","));
  EXPECT_THAT(os.str(), "this,is,an,example,");
}


namespace U26_Text
{
  template<typename Iter>
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

  template<typename Coll>
    string join_string(Coll coll, const char *delimeter)
    {
      if (coll.size() == 0)
        return string();

      return join_string(coll.begin(), coll.end(), delimeter);
    }
}

// TEST(U26, Text)
TEST(String, JoinSequnenceString_03)
{
  using namespace U26_Text;

  std::vector<std::string> coll{ "this","is","an","example" };

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
// string-raw

// { "address": {
//          "road":"Drury Ln",
//          "city":"Fountain",
//          "state":"CO",
//          "country":"US" }}

TEST(String, RawString)
{
    const std::string s = R"({ "address": {
         "road":"Drury Ln",
         "city":"Fountain",
         "state":"CO",
         "country":"US" }})";

    std::cout << s << std::endl;
}


// ={=========================================================================
// 4.11 Removing a Substring from a String

void removeSubstrs1(std::string &s, const std::string &p)
{
    auto start = s.find(p);

    if (start != std::string::npos)
    {
        auto first = s.substr(0, start);
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

    for (auto start = s.find(p);
         start != std::string::npos;
         start = s.find(p))
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
// 4.13 Doing a Case-Insensitive String Comparison

bool caseInsCompare1(const string &s1, const string &s2)
{
    if (s1.size() != s2.size())
        return false;

    for (auto lhs = s1.cbegin(), rhs = s2.cbegin(); lhs != s1.cend(); ++lhs, ++rhs)
        if (toupper(*lhs) != toupper(*rhs))
            return false;

    return true;
}

TEST(String, CompareCaseInsensitive)
{
    const string s1 = "In the BEGINNING...";
    const string s2 = "In the beginning...";

    EXPECT_TRUE(caseInsCompare1(s1, s2));
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
    auto it = search(s.begin(), s.end(), p.begin(), p.end(),
                     [](char lhs, char rhs) {
                         return toupper(lhs) == rhs;
                     });

    return it != s.end();
}

TEST(String, SearchCaseInsensitive)
{
    string s1 = "row, row, row, your boat";
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


// // ={=========================================================================
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
//         // Since the original approach do not work well in dealing with new line in
//         // the input, this is an attempt to fix. not sure if it works. no further
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

    char cur = '\0';
    char last = '\0';
    int chars = 0;
    int words = 0;
    int lines = 0;

    while (input_file.get(cur))
    {
        if (cur == '\n' ||
            (cur == '\f' && last == '\r'))
            lines++;
        else
            chars++;
        if (!std::isalnum(cur) && // This is the end of a
            std::isalnum(last))   // word
            words++;
        last = cur;
    }
    if (chars > 0)
    {                           // Adjust word and line
        if (std::isalnum(last)) // counts for special
            words++;            // case
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

  for(const auto &e : wm)
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

class TextAutoCorrectField {
  public:
    TextAutoCorrectField() = delete;
    TextAutoCorrectField(const StrStrMap *p) : pdict_(p) {}

    void append(char c);
    void getText(string &s) {s = buf_;}

  private:
    string buf_;
    const StrStrMap *pdict_;
};

void TextAutoCorrectField::append(char c)
{
  // only do when see a word boundary that is a sequence of a char and space or
  // punctions.

  if ((isspace(c) || ispunct(c)) &&
      buf_.length() > 0 &&
      !isspace(buf_[buf_.length()-1]))
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
    string word = buf_.substr(i, buf_.length()-i);
    StrStrMap::const_iterator found = pdict_->find(word);

    if (found != pdict_->end())
    {
      buf_.erase(i, buf_.length()-i);
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
  
  dict["taht"] = "that";
  dict["right"] = "wrong";
  dict["bug"] = "feature";

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

void loadCSV(ifstream &ifs, vector<vector<string>*> &data)
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

  vector<vector<string>*> data;

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
