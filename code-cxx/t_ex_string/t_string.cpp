#include "gmock/gmock.h"

#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <bitset>
#include <boost/algorithm/string.hpp>


// g++ -g -std=c++0x t_override.cpp

using namespace std;
using testing::ElementsAre;
using testing::Eq;
using testing::StrEq;
using testing::FloatEq;


// ={=========================================================================
// string

TEST(CxxString, ConstructFromChars)
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

TEST(CxxString, UseStringCtors)
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

TEST(CxxString, UseStringOperations)
{
    string s1{"zoo"};
    EXPECT_EQ(s1.back(), 'o');

    // this fails as the same reason as string ctors shown above.
    // EXPECT_EQ(string(s1.back()), "o");
    
    EXPECT_EQ(string(1, s1.back()), "o");
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

TEST(CxxStringTest, CompareStringFromEnd)
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
// string-stringstream

// note that os, buffer, has all inputs from << and seek() moves writing pos.
// do *cxx-string-convert-to-string*
//
// decimal : 15, hex : f
// ---------
// decimal : 15, hex : f
// float : 4.67, bitset : 001011010011101
// ---------
// octal : 1715, hex : f
// float : 4.67, bitset : 001011010011101

TEST(CxxStringTest, UseOutputStringStream)
{
  ostringstream os;

  os << "decimal : " << 15 << hex << ", hex : " << 15 << endl;
  cout << os.str();

  cout << "---------" << endl;
  bitset<15> bit_set(5789);
  os << "float : " << 4.67 << ", bitset : " << bit_set << endl;
  cout << os.str();

  cout << "---------" << endl;
  os.seekp(0);
  os << "octal : " << oct << 15;
  cout << os.str();
}

// do *cxx-string-convert-to-number*
// The following lines read the integer x with the value 3 and the
// floating-point f with the value 0.7 from the string s:

TEST(CxxStringTest, UseInputStringStream)
{
  int x{};
  float f{};
  istringstream is{"3.7"};

  is >> x >> f;
 
  ASSERT_THAT(x, Eq(3));
  ASSERT_THAT(f, FloatEq(0.7));
}


TEST(CxxStringTest, UseOutputStringStreamX)
{
  stringstream ss;
  vector<string> string_vector{};

  // for(int i = 0; i < 4; ++i)
  // {
  //   ss << "player " << i << ", ";
  //   string_vector.push_back(string(ss.str()));
  //   ss.str("");
  // }
  
  for(int i = 0; i < 4; ++i)
  {
    string str = "player " + to_string(i);
    string_vector.push_back(str);
  }
 
  copy(string_vector.begin(), string_vector.end(), ostream_iterator<string>(cout, " "));
  cout << endl;
}


// ={=========================================================================
// size of s1 cstring  : 8
// size of s1 cstring  : 8
// size of s1 cstring  : 1
// size of s2 cstring  : 22
// size of s2 cstring  : 22
// strlen of s2 cstring: 21
// size of string      : 21

TEST(CxxStringTest, CompareStringSizes)
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

TEST(CxxStringTest, PrintVariousTypeToOutputSteram)
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

TEST(CxxStringTest, StringInsertAndAppendMember)
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

TEST(CxxStringTest, StringAppendNull)
{
    string s1{};

    for (int n = 0; n < 5; ++n)
        s1 += "";

    EXPECT_EQ(s1.length(), 0);
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
    // note: when string s do not have any c, for loop condition runs once adn
    // 'it' is end()-1. so ++it makes it is end(). so erase() has no effect.

    if (*it != c)
        ++it;

    s.erase(it, s.end());
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

TEST(CxxStringTest, TrimCharOrWhitespaceFromEnd)
{
    string s1{"zoo"};
    rtrim(s1, 'o');
    EXPECT_EQ(s1, "z");

    string s2{"zoo                              "};
    rtrimws(s2);
    EXPECT_EQ(s2, "zoo");

    string s3{"zzz"};
    rtrim(s3, 'o');
    EXPECT_EQ(s3, "zzz");

    string s4{"                              zoo"};
    ltrimws(s4);
    EXPECT_EQ(s4, "zoo");

    string s5{"zoo"};
    ltrimws(s5);
    EXPECT_EQ(s5, "zoo");
}


// ={=========================================================================
// Q: to split the string "Name|Address|Phone" into three separate strings,
// "Name", "Address", and "Phone", with the delimiter removed.
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

class MakeToken
{
  public:
    MakeToken(const std::string &input, const std::string &delim = "|")
        : str_(input), delim_(delim)
    {
        // fix-on-next-token
        begin_ = str_.find_first_not_of('|');
        end_ = str_.find_first_of('|', begin_);
    }

    // updates begin_ and end_ state
    void nextToken(std::string &token)
    {
        // there are more tokens to process
        if (end_ != string::npos)
        {
            // cxx-string-substr
            // substr(start, length) but not substr(start, end);
            token = str_.substr(begin_, end_ - begin_);
            begin_ = str_.find_first_not_of('|', end_);
            end_ = str_.find_first_of('|', begin_);
        }

        // seen the last token ending with no delim, |"xxx"
        //
        // * TDD: can check if nextToken() is called more than # of tokens.
        // this cause
        // else if(end_ == string::npos)
        //

        else if (begin_ != string::npos && end_ == string::npos)
        {
            token = str_.substr(begin_);
            // to make the condition to stop
            begin_ = end_ = string::npos;
        }
    }

    // do not update states since it just counts
    // * this is not only complicated but also makes wrong result as
    // fix-on-next-token e.g., "||Name" produce 2.

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

TEST(CxxStringTest, MakeTokensFromString)
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

TEST(CxxStringTest, MakeTokensFromString_ExpectMoreTokens)
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

// ={=========================================================================
// C++ codebook, 4.7, tokenizing a string
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

    // can implement like MakeToken::nextToken()
    // * do not change s when input is null.
    // * TDD: can check if nextToken() is called more than # of tokens.
    void nextToken(string &s)
    {
        // if (begin_ == string::npos && end_ == string::npos)
        // is when input is null.

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
            // find a char and break if there are only delimiters.
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

TEST(CxxStringTest, StringTokenizerFromString_Input1)
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

TEST(CxxStringTest, StringTokenizerFromString_Input2)
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

TEST(CxxStringTest, StringTokenizerFromString_Input3)
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

TEST(CxxStringTest, StringTokenizerFromString_Input4)
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

TEST(CxxStringTest, StringTokenizerFromString_Input5)
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

TEST(CxxStringTest, StringTokenizerFromString_Input6)
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

// ={=========================================================================

TEST(CxxStringTest, BoostSplit_Input1)
{
    std::string str{"Name|Address|Phone"};
    std::vector<std::string> svec;

    boost::split(svec, str, boost::is_any_of("|"));

    // for (auto &e : svec)
    //     cout << e << endl;

    EXPECT_THAT(svec, ElementsAre("Name", "Address", "Phone"));
}

// *TN* see how boost works
//
// Value of: svec
// Expected: has 1 element that is equal to "Name"
//   Actual: { "", "", "Name" }, which has 3 elements

TEST(CxxStringTest, BoostSplit_Input2)
{
    std::string str{"||Name"};
    std::vector<std::string> svec;

    boost::split(svec, str, boost::is_any_of("|"));

    EXPECT_THAT(svec, ElementsAre("", "", "Name"));
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

TEST(CxxStringTest, JoinSequnenceString)
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

TEST(CxxStringTest, FindNthSubstring)
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

TEST(CxxStringTest, UseRawString)
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

TEST(CxxStringTest, RemoveSubString)
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

TEST(CxxStringTest, ConvertStringToLowerUpperCase)
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

TEST(CxxStringTest, CompareCaseInsensitive)
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

TEST(CxxStringTest, SearchCaseInsensitive)
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

TEST(CxxStringTest, ConvertTabToSpace)
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

TEST(CxxStringTest, ConvertSpaceToTab)
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

TEST(CxxStringTest, WrapLinesInTextFile)
{
    ifstream input_file{"input.txt"};
    ofstream output_file{"output_wrap.txt"};

    testWrap(input_file, output_file, 72);
}

// ={=========================================================================
// 4.17 4.17 Counting the Number of Characters, Words,
// and Lines in a Text File

// kyoupark@kit-debian64:~/git/kb/code-cxx/t_ex_string$ wc input.txt
//   3  23 152 input.txt
//
// CxxStringTest.CountStuffInTextFile
//
// cc: 152
// wc: 23
// lc: 3
//
// CxxStringTest.CountStuffInTextFile2
//
// cc: 149
// wc: 23
// lc: 4

TEST(CxxStringTest, CountStuffInTextFile)
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

TEST(CxxStringTest, CountStuffInTextFile2)
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

TEST(CxxStringTest, CountWordsInTextFile)
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
    cout << word_count << ": " << e.first << ": " << e.second << endl;
    ++word_count;
  }
}


// ={=========================================================================
// 4.19 Add Margins to a Text File

// ={=========================================================================
// 4.20 Justify a Text File

// Note that I set the width inside the loop while I set the right flag prior to
// the loop. I had to do this because the width resets to zero after each write
// to the stream. Format flags are not reset after writes, so I only had to
// initialize them once and be done with it.

TEST(CxxStringTest, JustifyTextFile)
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

TEST(CxxStringTest, SqueezeWhitespaceInTextFile)
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

TEST(CxxStringTest, SqueezeWhitespaceInTextFileTwo)
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
    
    auto i = buf_.find_last_of(" \f\n\r\t\v");
    i = (i == string::npos) ? 0 : ++i;

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

TEST(CxxStringTest, AutoCorrectField)
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

int main(int argc, char **argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
    }
