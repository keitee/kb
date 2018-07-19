#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <bitset>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/cast.hpp>
#include <boost/lexical_cast.hpp>

#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using testing::ElementsAre;
using testing::Eq;
using testing::StrEq;
using testing::FloatEq;


// ={=========================================================================
// string

TEST(String, ConstructFromChars)
{
    char s1[] = "this is first message";
    char *s2 = "this is first message";

    string string_from_char(s1);
    string string_from_const_char(s2);

    // both suffers from:
    // const char const_letter = 'A';
    // char non_const_letter = 'A';
    //
    // t_string.cpp:29:49: error: invalid conversion from ‘char’ to ‘const char*’ [-fpermissive]
    // string string_from_const_letter(const_letter);
    // string string_from_non_const_letter(non_const_letter);
}


// ={=========================================================================
// string ctors

TEST(String, UseStringCtors)
{
    string s1{"zoo"};
    EXPECT_EQ(s1, "zoo");

    string s2("zoo");
    EXPECT_EQ(s1, "zoo");

    char letter = 'A';

    // error: invalid conversion from ‘char’ to ‘const char*’ [-fpermissive]
    // string s3(letter);

    // ok since:
    //
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

    string s4;
    s4 += letter;
    EXPECT_EQ(s4, "A");

    string s5;

    // error: invalid conversion from ‘char’ to ‘const char*’ [-fpermissive]
    // s5.append(letter);

    s5.append(1, letter);
    EXPECT_EQ(s5, "A");
}


// ={=========================================================================
// string operations

TEST(String, UseStringOperations)
{
    string s1{"zoo"};
    EXPECT_EQ(s1.back(), 'o');

    // this fails as the same reason as string ctors shown above.
    // EXPECT_EQ(string(s1.back()), "o");
    
    EXPECT_EQ(string(1, s1.back()), "o");
}

TEST(String, MaxSize)
{
  string s{};
  EXPECT_THAT(s.max_size(), 4611686018427387897);
}


// ={=========================================================================
// string and cstring

// s[0] : z
// s[1] : o
// s[2] : o
// s[3] : 
// s[3] is null

TEST(CxxString, UseStringVsCstring)
{
  string s{"zoo"};

  cout << "s[0] : " << s[0] << endl;  // z
  cout << "s[1] : " << s[1] << endl;  // o
  cout << "s[2] : " << s[2] << endl;  // o
  cout << "s[3] : " << s[3] << endl;  // o

  if (s[3] == 0)
    cout << "s[3] is null" << endl;
  else
    cout << "s[3] is not null" << endl;
}


// ={=========================================================================
// cstring.
// return bool if source and target are the same from the end.
// bool strend(char *s, char *t);

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

// strend(this is first message, ssage) returns 1
// strend(this is first message, xsage) returns 0
// strend(this is first message, ssage) returns 1
// strend(this is first message, xsage) returns 0

TEST(String, CompareStringFromEnd)
{
    char s1[] = "this is first message";
    char t1[] = "ssage";
    char t2[] = "xsage";

    EXPECT_EQ(true, strend_01(s1, t1));
    EXPECT_EQ(false, strend_01(s1, t2));

    EXPECT_EQ(true, strend_02(s1, t1));
    EXPECT_EQ(false, strend_02(s1, t2));
}


// ={=========================================================================
// string-conversion

TEST(StringConverison, ToString)
{
  // use stringstream
  // note that os, buffer, has all inputs from << and seek() moves writing pos.
  // do *cxx-string-convert-to-string*
  ostringstream os;

  os << "decimal : " << 15 << hex << ", hex : " << 15 << endl;
  EXPECT_EQ(os.str(), "decimal : 15, hex : f\n");

  bitset<15> bit_set(5789);
  os << "float : " << 4.67 << ", bitset : " << bit_set << endl;
  EXPECT_EQ(os.str(), "decimal : 15, hex : f\nfloat : 4.67, bitset : 001011010011101\n");

  os.seekp(0);
  os << "octal : " << oct << 15;
  EXPECT_EQ(os.str(), "octal : 1715, hex : f\nfloat : 4.67, bitset : 001011010011101\n");

  // use to_string()
  EXPECT_THAT(to_string(11), "11");
  EXPECT_THAT(to_string(3301), "3301");

  // use lexical_cast
  EXPECT_THAT(boost::lexical_cast<std::string>(11), "11");
  EXPECT_THAT(boost::lexical_cast<std::string>(3301), "3301");

  // use stringstream
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


TEST(StringConverison, ToNumber)
{
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

  {
    // to-number
    stringstream is{"1 2 3 4"};
    int value{};
    vector<int> coll{};

    while (is >> value)
      coll.push_back(value);

    EXPECT_THAT(coll, ElementsAre(1,2,3,4));
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


TEST(StringConverison, NarrowNumericConversion)
{
  // :464:12: warning: narrowing conversion of ‘5.0e+0’ from ‘double’ to ‘int’ inside { } [-Wnarrowing]
  //    int x{5.0};
  //            ^

  int x{5.0};
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
// size of s1 cstring  : 8
// size of s1 cstring  : 8
// size of s1 cstring  : 1
// size of s2 cstring  : 22
// size of s2 cstring  : 22
// strlen of s2 cstring: 21
// size of string      : 21

TEST(String, CompareStringSizes)
{
    const char *s1 = "this is first message";
    const char s2[] = "this is first message";
    cout << "size of s1 cstring  : " << sizeof(s1) << endl;
    cout << "size of s1 cstring  : " << sizeof s1 << endl;
    cout << "size of s1 cstring  : " << sizeof *s1 << endl;
    cout << "size of s2 cstring  : " << sizeof(s2) << endl;
    cout << "size of s2 cstring  : " << sizeof(s2) / sizeof(s2[0]) << endl;
    cout << "strlen of s2 cstring: " << strlen(s2) << endl;

    string s{s1};
    cout << "size of string      : " << s.size() << endl;
}

// ={=========================================================================

// iv : 10
// id : 10
// cv : A
// cv : 65
// cv : 65

TEST(String, PrintVariousTypeToOutputSteram)
{
    string str{"STRING"};
    int iv{10};
    double id{10.0};
    char cv{65};

    cout << "iv : " << iv << endl;
    cout << "id : " << id << endl;
    cout << "cv : " << cv << endl;
    cout << "cv : " << +cv << endl;
    cout << "cv : " << to_string(cv) << endl;

    cout << "str[0] : " << str[0] << endl;
    char ch = str[0];
    cout << "str[0] : " << ch << endl;
}


// ={=========================================================================
// string-pad
// n will be the total lenth of the return

template <typename T>
void pad(T &s, typename T::size_type n, typename T::value_type c)
{
    if (n > s.size())
        s.append(n - s.size(), c);
}

TEST(String, StringInsertAndAppendMember)
{
    // string s1{"foo"};
    // s1.insert(0, 20 - s1.size(), 'X');
    // EXPECT_EQ(s1, "XXXXXXXXXXXXXXXXXfoo");

    string s1{"foo"};
    s1.insert(s1.size(), 20 - s1.size(), 'X');
    EXPECT_EQ(s1, "fooXXXXXXXXXXXXXXXXX");

    // string s2{"foo"};
    // s2.insert(s2.begin(), 20 - s2.size(), 'X');
    // EXPECT_EQ(s2, "XXXXXXXXXXXXXXXXXfoo");

    string s2{"foo"};
    s2.insert(s2.end(), 20 - s2.size(), 'X');
    EXPECT_EQ(s2, "fooXXXXXXXXXXXXXXXXX");

    string s3{"foo"};
    s3.append(20 - s3.size(), 'X');
    EXPECT_EQ(s3, "fooXXXXXXXXXXXXXXXXX");

    string s4{"foo"};
    pad(s4, 20, 'X');
    EXPECT_EQ(s4, "fooXXXXXXXXXXXXXXXXX");

    wstring ws1{L"foo"};
    pad(ws1, 20, 'X');
    EXPECT_EQ(ws1, L"fooXXXXXXXXXXXXXXXXX");
}

TEST(String, StringAppendNull)
{
    string s1{};

    for (int n = 0; n < 5; ++n)
        s1 += "";

    EXPECT_EQ(s1.length(), 0);
}


// ={=========================================================================
// string-insert
// n will be the total lenth of the return

TEST(String, Insert)
{
  string s{};
  s.insert(0, 1, 'X');
  s.insert(0, 1, 'X');
  s.insert(0, 1, 'X');
  EXPECT_EQ(s, "XXX");
}


// ={=========================================================================
// 4.2 Trimming a String

// to trim `some` number of char from the end
void rtrim(std::string &s, char c)
{
    if (s.empty())
        return;

    auto it = s.end();
    for (; it != s.begin() && *--it == c;)
        ;

    // move it one back since it is increased in for loop one more
    //
    // note: when string s do not have any c, for loop condition runs once and
    // 'it' is end()-1. so ++it makes it is end(). so erase() has no effect.

    if (*it != c)
        ++it;

    s.erase(it, s.end());
}

void rtrim_0607(std::string &s, char c)
{
  if (s.empty())
    return;

  while (s[s.length()-1] == c)
    s.pop_back();
}

// to trim `whitespace` from the end
void rtrimws(std::string &s)
{
    if (s.empty())
        return;

    auto it = s.end();
    for (; it != s.begin() && isspace(*--it);)
        ;

    // move it one back since increased in for
    ++it;

    s.erase(it, s.end());
}

// to trim `whitespace` from the start
void ltrimws(std::string &s)
{
    if (s.empty())
        return;

    auto it = s.begin();
    for (; it != s.end() && isspace(*it); ++it)
        ;

    s.erase(s.begin(), it);
}

// z
// s: zoo                              , size : 33
// s: zoo, size : 3

TEST(Trim, UseTrims)
{
    string s1{"zoo"};
    rtrim(s1, 'o');
    EXPECT_EQ(s1, "z");

    string s3{"zzz"};
    rtrim(s3, 'o');
    EXPECT_EQ(s3, "zzz");

    string s11{"zoo"};
    rtrim_0607(s11, 'o');
    EXPECT_EQ(s1, "z");

    string s33{"zzz"};
    rtrim_0607(s33, 'o');
    EXPECT_EQ(s3, "zzz");


    string s2{"zoo                              "};
    rtrimws(s2);
    EXPECT_EQ(s2, "zoo");

    string s4{"                              zoo"};
    ltrimws(s4);
    EXPECT_EQ(s4, "zoo");

    string s5{"zoo"};
    ltrimws(s5);
    EXPECT_EQ(s5, "zoo");
}


// ={=========================================================================
// 4.6 Splitting a String
// You want to split a delimited string into multiple strings. For example, you
// may want to split the string "Name|Address|Phone" into three separate
// strings, "Name", "Address", and "Phone", with the delimiter removed.

// note:
// this has *undefined* behaviour; sometimes works and sometimes infinite loop
// since found is npos when not found and do ++found and use it loop condition.

void split(const string &s, char delim, vector<string> &coll)
{
  size_t i{};
  size_t found{};
  std::string token;
 
  for (i = 0; found != std::string::npos; )
  {
    found = s.find(delim, i);
    token = s.substr(i, found-i);
    i = ++found;
    coll.push_back(token);
    // std::cout << "token: " << token << std::endl;
  }
}

TEST(DISABLED_SplitString, UseSplitDraftVersion)
{
  std::vector<std::string> coll;

  split("Smith, Bill, 5/1/2002, Active", ',', coll); 
  EXPECT_THAT(coll, ElementsAre("Smith", " Bill", " 5/1/2002", " Active"));
}

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

TEST(SplitString, UseSplitTextVersion)
{
  std::vector<std::string> coll;

  split_text("Smith, Bill, 5/1/2002, Active", ',', coll); 
  EXPECT_THAT(coll, ElementsAre("Smith", " Bill", " 5/1/2002", " Active"));

  // note: fails:
  //
  // t_string.cpp:497: Failure
  // Value of: coll
  // Expected: has 1 element that is equal to "Smith"
  //   Actual: {}
  // [  FAILED  ] String.SplitString (1 ms)
  //
  // coll.clear();
  // split_text("Smith", ',', coll); 
  // EXPECT_THAT(coll, ElementsAre("Smith"));
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

TEST(SplitString, UseSplitRevisedVersion)
{
  std::vector<std::string> coll;

  split_revised("Smith, Bill, 5/1/2002, Active", ',', coll); 
  EXPECT_THAT(coll, ElementsAre("Smith", " Bill", " 5/1/2002", " Active"));

  coll.clear();
  split_revised("Smith", ',', coll); 
  EXPECT_THAT(coll, ElementsAre("Smith"));
}


TEST(SplitString, UseSplitBoostVersion)
{
  std::string str1{"Name|Address|Phone"};
  std::vector<std::string> svec1;

  boost::split(svec1, str1, boost::is_any_of("|"));

  EXPECT_THAT(svec1, ElementsAre("Name", "Address", "Phone"));

  // note: cxx-boost-split see how boost works
  //
  // Value of: svec
  // Expected: has 1 element that is equal to "Name"
  //   Actual: { "", "", "Name" }, which has 3 elements

  std::string str2{"||Name"};
  std::vector<std::string> svec2;

  boost::split(svec2, str2, boost::is_any_of("|"));

  EXPECT_THAT(svec2, ElementsAre("", "", "Name"));
}


// ={=========================================================================
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

        // *TDD* fix-on-next-token
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

TEST(StringTokenizer, UseString_Input1)
{
    std::string token{};
    std::vector<std::string> svec{};

    StringTokenizer st("Name|Address|Phone");

    EXPECT_EQ(3, st.countTokens());

    while (st.hasMoreTokens())
    {
        st.nextToken(token);
        svec.push_back(token);
    }

    EXPECT_THAT(svec, ElementsAre("Name", "Address", "Phone"));
}

TEST(StringTokenizer, UseString_Input2)
{
    std::string token{};
    std::vector<std::string> svec{};

    StringTokenizer st("Name|Address");

    EXPECT_EQ(2, st.countTokens());

    while (st.hasMoreTokens())
    {
        st.nextToken(token);
        svec.push_back(token);
    }

    EXPECT_THAT(svec, ElementsAre("Name", "Address"));
}

TEST(StringTokenizer, UseString_Input3)
{
    std::string token{};
    std::vector<std::string> svec{};

    StringTokenizer st("Name");

    EXPECT_EQ(1, st.countTokens());

    while (st.hasMoreTokens())
    {
        st.nextToken(token);
        svec.push_back(token);
    }

    EXPECT_THAT(svec, ElementsAre("Name"));
}

TEST(StringTokenizer, UseString_Input4)
{
    std::string token{};
    std::vector<std::string> svec{};

    StringTokenizer st("Name|");

    EXPECT_EQ(1, st.countTokens());

    while (st.hasMoreTokens())
    {
        st.nextToken(token);
        svec.push_back(token);
    }

    EXPECT_THAT(svec, ElementsAre("Name"));
}

TEST(StringTokenizer, UseString_Input5)
{
    std::string token{};
    std::vector<std::string> svec{};

    StringTokenizer st("Name||Address");

    EXPECT_EQ(2, st.countTokens());

    while (st.hasMoreTokens())
    {
        st.nextToken(token);
        svec.push_back(token);
    }

    EXPECT_THAT(svec, ElementsAre("Name", "Address"));
}

TEST(StringTokenizer, UseString_Input6)
{
    std::string token{};
    std::vector<std::string> svec{};

    StringTokenizer st("||Name");

    EXPECT_EQ(1, st.countTokens());

    while (st.hasMoreTokens())
    {
        st.nextToken(token);
        svec.push_back(token);
    }

    EXPECT_THAT(svec, ElementsAre("Name"));
}


// * not boost-split style which means do not support null token.

class MakeToken
{
  public:
    MakeToken(const std::string &input, const std::string &delim = "|")
      : str_(input), delim_(delim)
      {
        begin_ = str_.find_first_not_of('|');
        end_ = str_.find_first_of('|', begin_);
      }

    // get token and updates begin_ and end_ for the next 
    //
    // note: here check if nextToken() is called more than # of tokens which
    // hasMoreTokens() is for. If do not support this, then can make it simple
    // as no check is required. 
    void nextToken(std::string &token)
    {
      // there are more tokens to process
      if (end_ != string::npos)
      {
        // cxx-string-substr substr(start, length)
        token = str_.substr(begin_, end_ - begin_);
        begin_ = str_.find_first_not_of('|', end_);
        end_ = str_.find_first_of('|', begin_);
      }

      // seen the hand and to see it, run it input "xxx" as
      // TEST(DISABLED_MakeToken, ParseTokensFromInput)

      else if (begin_ != string::npos && end_ == string::npos)
      {
        token = str_.substr(begin_);
        // to make the condition to stop
        begin_ = end_ = string::npos;
      }
    }

    // do not update states since it just counts
    int countTokens()
    {
      auto begin = str_.find_first_not_of('|');
      auto end = str_.find_first_of('|', begin);

      while (begin != end)
      {
        ++count_;
        begin = str_.find_first_not_of('|', end);
        end = str_.find_first_of('|', begin);
      }

      return count_;
    }

    // to do this, need to have member variables to keep state
    bool hasMoreTokens() const
    {
      return begin_ != end_;
    }

  private:
    MakeToken() = default;
    string str_{};
    string delim_{};
    int count_{};
    size_t begin_{};
    size_t end_{};
};

// to see nextToken() hang
TEST(DISABLED_MakeToken, ParseTokensFromInput)
{
    std::string token{};
    std::vector<std::string> svec{};

    // 3. "Name"
    token.clear();
    svec.clear();
    MakeToken mt3("Name");
    EXPECT_EQ(1, mt3.countTokens());

    while (mt3.hasMoreTokens())
    {
        mt3.nextToken(token);
        svec.push_back(token);
    }

    EXPECT_THAT(svec, ElementsAre("Name"));
}

TEST(DISABLED_MakeToken, ParseTokensFromVariosInputs)
{
    std::string token{};
    std::vector<std::string> svec{};

    // 1. "Name|Address|Phone"
    MakeToken mt1("Name|Address|Phone");
    EXPECT_EQ(3, mt1.countTokens());

    while (mt1.hasMoreTokens())
    {
        mt1.nextToken(token);
        svec.push_back(token);
    }

    EXPECT_THAT(svec, ElementsAre("Name", "Address", "Phone"));

    // 2. "Name|Address"
    token.clear();
    svec.clear();
    MakeToken mt2("Name|Address");
    EXPECT_EQ(2, mt2.countTokens());

    while (mt2.hasMoreTokens())
    {
        mt2.nextToken(token);
        svec.push_back(token);
    }

    EXPECT_THAT(svec, ElementsAre("Name", "Address"));

    // 3. "Name"
    token.clear();
    svec.clear();
    MakeToken mt3("Name");
    EXPECT_EQ(1, mt3.countTokens());

    while (mt3.hasMoreTokens())
    {
        mt3.nextToken(token);
        svec.push_back(token);
    }

    EXPECT_THAT(svec, ElementsAre("Name"));

    // 4. "Name|"
    token.clear();
    svec.clear();
    MakeToken mt4("Name|");
    EXPECT_EQ(1, mt4.countTokens());

    while (mt4.hasMoreTokens())
    {
        mt4.nextToken(token);
        svec.push_back(token);
    }

    EXPECT_THAT(svec, ElementsAre("Name"));

    // 5. "Name||Address"
    token.clear();
    svec.clear();
    MakeToken mt5("Name||Address");
    EXPECT_EQ(2, mt5.countTokens());

    while (mt5.hasMoreTokens())
    {
        mt5.nextToken(token);
        svec.push_back(token);
    }

    EXPECT_THAT(svec, ElementsAre("Name", "Address"));

    // 6. "||Name"
    // exception where end > begin
    token.clear();
    svec.clear();
    MakeToken mt6("||Name");
    EXPECT_EQ(1, mt6.countTokens());

    while (mt6.hasMoreTokens())
    {
        mt6.nextToken(token);
        svec.push_back(token);
    }

    EXPECT_THAT(svec, ElementsAre("Name"));
}

TEST(String, ParseTokensMoreThanHad)
{
    std::string token{};
    std::vector<std::string> svec{};

    MakeToken mt1("Name|Address|Phone");
    EXPECT_EQ(3, mt1.countTokens());

    for (int i = 0; i < 6; ++i)
    {
        mt1.nextToken(token);
        svec.push_back(token);
    }

    // *TDD*
    // when calls it more than # of elements, do not clear token input string so
    // has the same string. Need to clear input in this case?
    EXPECT_THAT(svec, ElementsAre("Name", "Address", "Phone", "Phone", "Phone", "Phone"));
}


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

TEST(MakeToken_0529, UseCountTokens)
{
  MakeToken_0529 st("Name|Address|Phone");

  EXPECT_THAT(st.countTokens(), Eq(3));
  EXPECT_THAT(st.countTokens(), Eq(3));
}

TEST(MakeToken_0529, UseGetTokens)
{
  vector<string> coll{};
  MakeToken_0529 st("Name|Address|Phone");
  string token;

  while (st.countTokens())
  {
    st.nextToken(token);
    coll.push_back(token);
  }

  EXPECT_THAT(coll, ElementsAre("Name", "Address", "Phone"));
}

TEST(MakeToken_0529, UseVariousInputs)
{
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


// ={=========================================================================
// StringTokenizer_0606
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
// class StringTokenizer {
//
//   private:
//     StringTokenizer() {}
//     string delim_;
//     string str_;
//     size_t count_{};
//     size_t begin_{};
//     size_t end_{};
// };
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

TEST(StringTokenizer_0606, UseCountTokens)
{
  StringTokenizer_0606 st1("Name|Address|Phone");
  EXPECT_EQ(st1.countTokens(), 3);

  StringTokenizer_0606 st2("Name|Address");
  EXPECT_EQ(2, st2.countTokens());

  StringTokenizer_0606 st3("Name");
  EXPECT_EQ(1, st3.countTokens());

  StringTokenizer_0606 st4("Name||Address");
  EXPECT_EQ(2, st4.countTokens());

  StringTokenizer_0606 st5("||Name");
  EXPECT_EQ(1, st5.countTokens());

  StringTokenizer_0606 st6("|||");
  EXPECT_EQ(0, st6.countTokens());
}

TEST(StringTokenizer_0606, ParseTokensFromVariosInputs)
{
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


// ={=========================================================================
void join(const std::vector<std::string> &vector, const char delim, std::string &joined)
{
    joined.clear();

    for (auto it = vector.cbegin(); it != vector.cend(); ++it)
    {
        joined += *it;

        // cxx-iter-arithmetic
        if (it < vector.cend() - 1)
            joined += delim;
    }
}

TEST(String, JoinSequnenceString)
{
    std::vector<string> svec{"fee", "fi", "foe", "fum"};
    std::string s;

    join(svec, '/', s);

    EXPECT_THAT(s, Eq("fee/fi/foe/fum"));
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
// use raw string

// { "address": {
//          "road":"Drury Ln",
//          "city":"Fountain",
//          "state":"CO",
//          "country":"US" }}

TEST(String, UseRawString)
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

// ={=========================================================================
// 4.16 Wrapping Lines in a Text File

// Two requirements:
//
// For example, if you want to wrap text at 72 characters, you would insert a
// new-line character after every 72 characters in the file.
//
// If the file contains human-readable text, you probably want to avoid
// splitting words.
//
// works on a word unit and tmp keeps current word.
//
// whenever see a word, current_char is space and last_char is char, write it
// and clear tmp.
//
// cc cccc ccccc cccc[width to wrap]
//  ^^ - current_char
//  +--- last_char
//
// at the wrap point, write a '\n' and last word. update i since last word is
// already written on the new line.

void testWrap(istream &in, ostream &out, size_t width)
{
    string tmp{};
    char current_char = '\0';
    char last_char = '\0';
    size_t i = 0;

    while (in.get(current_char))
    {
        // increase i on every iteration
        if (++i == width)
        {
            ltrimws(tmp);
            out << '\n'
                << tmp;
            i = tmp.length();
            tmp.clear();
        }
        else if (isspace(current_char) && !isspace(last_char))
        {
            out << tmp;
            tmp.clear();
        }

        // Since the original approach do not work well in dealing with new line in
        // the input, this is an attempt to fix. not sure if it works. no further
        // effort and stops here.
        //
        // if (current_char == '\n')
        //   current_char = ' ';

        tmp += current_char;
        last_char = current_char;
    }
}

TEST(String, WrapLinesInTextFile)
{
    ifstream input_file{"input.txt"};
    ofstream output_file{"output_wrap.txt"};

    testWrap(input_file, output_file, 72);
}

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
