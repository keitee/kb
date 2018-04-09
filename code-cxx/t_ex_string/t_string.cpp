#include <iostream>
#include <cstring>
#include <vector>
#include <boost/algorithm/string.hpp>

#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using testing::ElementsAre;
using testing::Eq;

// ={=========================================================================

// cstring.
// return bool if source and target are the same from the end.
// bool strend(char *s, char *t);

bool strend_01(char *s, char *t)
{
    // assume s is bigger than t
    size_t diff = strlen(s) - strlen(t);

    char *psource = s+diff, *ptarget = t;
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
    cout << "size of s2 cstring  : " << sizeof(s2)/sizeof(s2[0]) << endl;
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
// n will be the total lenth of the return

template<typename T>
void pad(T &s, typename T::size_type n, typename T::value_type c)
{
    if (n > s.size())
        s.append(n - s.size(), c);
}

TEST(CxxStringTest, StringInsertAndAppendMember)
{
    string s1{"foo"};
    s1.insert(0, 20 - s1.size(), 'X');
    EXPECT_EQ( s1, "XXXXXXXXXXXXXXXXXfoo");

    string s2{"foo"};
    s2.insert(s2.begin(), 20 - s2.size(), 'X');
    EXPECT_EQ( s2, "XXXXXXXXXXXXXXXXXfoo");

    string s3{"foo"};
    s3.append(20 - s3.size(), 'X');
    EXPECT_EQ( s3, "fooXXXXXXXXXXXXXXXXX");

    string s4{"foo"};
    pad(s4, 20, 'X');
    EXPECT_EQ( s4, "fooXXXXXXXXXXXXXXXXX");

    string s5{"foo"};
    s5.append(20 - s5.size(), 'X');
    EXPECT_EQ( s5, "fooXXXXXXXXXXXXXXXXX");

    wstring ws1{L"foo"};
    pad(ws1, 20, 'X');
    EXPECT_EQ( ws1, L"fooXXXXXXXXXXXXXXXXX");
}

TEST(CxxStringTest, StringAppendNull)
{
    string s1{};

    for(int n = 0; n < 5; ++n)
        s1 += "";

    EXPECT_EQ(s1.length(), 0);
}


// ={=========================================================================

// to trim `some` number of char from the end
void rtrim(std::string &s, char c)
{
    if (s.empty())
        return;

    auto it = s.end();
    for (; it != s.begin() && *--it == c;)
        ;

    // move it one back since increased in for
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


// z
// s: zoo                              , size : 33
// s: zoo, size : 3

TEST(CxxStringTest, TrimCharOrWhitespaceFromEnd)
{
    string s1{"zoo"};
    rtrim(s1, 'o');
    EXPECT_EQ( s1, "z");

    string s2{"zoo                              "};
    rtrimws(s2);
    EXPECT_EQ( s2, "zoo");
}

// ={=========================================================================
// string ctors

TEST(CxxStringTest, RunStringCtors)
{
    string s1{"zoo"};
    EXPECT_EQ( s1, "zoo");

    string s2("zoo");
    EXPECT_EQ( s1, "zoo");

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
    EXPECT_EQ( s4, "A");

    string s5;

    // error: invalid conversion from ‘char’ to ‘const char*’ [-fpermissive]
    // s5.append(letter);
    
    s5.append(1, letter);
    EXPECT_EQ( s5, "A");
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

            else if( begin_ != string::npos && end_ == string::npos)
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

    while(mt1.hasMoreTokens())
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

    while(mt2.hasMoreTokens())
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

    while(mt3.hasMoreTokens())
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

    while(mt4.hasMoreTokens())
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

    while(mt5.hasMoreTokens())
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

    while(mt6.hasMoreTokens())
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
        else if(begin_ != string::npos && end_ == string::npos)
        {
            s = str_.substr(begin_, str_.length()-begin_);
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

    while(st.hasMoreTokens())
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

    while(st.hasMoreTokens())
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

    while(st.hasMoreTokens())
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

    while(st.hasMoreTokens())
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

    while(st.hasMoreTokens())
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

    while(st.hasMoreTokens())
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
        if (it < vector.cend()-1)
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

int main(int argc, char** argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
