#include <iostream>
#include <cstring>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <bitset>

#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;

// ={=========================================================================

// * use in/out parameter
// * cstring but efficient
// * strlen()-1 since array indexing is always [0, size-1], or [0,size) and in
//   this code, [begin, end] but not [begin, end)

void reverse_string_01(char *input)
{
    char *begin = input;
    char *end = input + strlen(input)-1;
    char temp{};

    for (; begin < end; ++begin, --end)
    {
        // swap(begin, end);
        temp = *begin;
        *begin = *end;
        *end = temp;
    }
}

TEST(CxxAlgoTest, ReverseCstring)
{
    char input[] = "REVERSE IT";
    reverse_string_01(input);
    EXPECT_THAT(input, StrEq("TI ESREVER"));
}

// ansic, p62

void reverse_string_02(char *input)
{
    int str{}, end{};
    char temp{}; 

    for (end = strlen(input)-1; str < end; ++str, --end)
    {
        // swap(str, end)
        temp = input[str], input[str] = input[end], input[end] = temp;
    }
}

TEST(CxxAlgoTest, ReverseCstringAnsiVersion)
{
    char input[] = "REVERSE IT";
    reverse_string_02(input);
    EXPECT_THAT(input, StrEq("TI ESREVER"));
}


// * if can use std::string and can return although c version is better.
// * use *cpp-reverse-iterator*

std::string reverse_string_03(const std::string &input)
{
    return std::string(input.crbegin(), input.crend());
}

TEST(CxxAlgoTest, ReverseCppStringUseRevrseIterator)
{
    std::string input{"REVERSE IT"};
    EXPECT_THAT(reverse_string_03(input), Eq("TI ESREVER"));
}


// when want to modify input itself

void reverse_string_04(std::string &input)
{
    std::reverse(input.begin(), input.end());
}

TEST(CxxAlgoTest, ReverseCppStringUseAlgorithm)
{
    std::string input{"REVERSE IT"};
    reverse_string_04(input);
    EXPECT_THAT(input, Eq("TI ESREVER"));
}

std::string reverse_string_05(const std::string &input)
{
    std::string reversed{};

    for (auto len = input.size(); len > 0; --len)
        reversed.push_back(input[len-1]);

    return reversed;
}

TEST(CxxAlgoTest, ReverseCppStringUsePushBack)
{
    std::string input{"REVERSE IT"};
    EXPECT_THAT(reverse_string_05(input), Eq("TI ESREVER"));
}


// Write a program to reverse a string with all its duplicates removed. Only the
// last instance of a character in the reverse string has to appear. Also, the
// following conditions are to be satisfied: Assume only Capital Letters.
//
// o. assume that input is ASCII and is all upper case chars. so can use
// unsigned int to check if it's a duplicate or not. if needs more range to
// cover then need to use something else.
//
// o. from the net

std::string reverse_string_06(const std::string &input)
{
    std::string sin{input};
    std::string sout{};
    unsigned int bappeared{};

    // remove duplicates from input
    for (size_t i = 0, size = sin.size(); i < size; ++i)
    {
        // only if not appeared before. use bitwise
        if (!(bappeared & (1 << (sin[i] - 'A'))))
        {
            sout += sin[i];
            bappeared |= (1 << (sin[i]-'A'));
        }
    }

    // return reverse;
    return std::string{sout.crbegin(), sout.crend()};
}

TEST(CxxAlgoTest, ReverseCppStringRemoveDuplicates)
{
    std::string input{"JTVAKAVISHAAAL"};
    EXPECT_THAT(reverse_string_06(input), Eq("LHSIKAVTJ"));
}


// ={=========================================================================

template <typename T>
void PRINT_ELEMENTS_MAP(T col, const string mesg, const string sep = ", ")
{
    cout << "=" << mesg << endl;

    for ( auto &e : col)
        cout << "{" << e.first << ", " << e.second << "}" << sep;
}


// ={=========================================================================
// find a number seen odd times
void t_algo_find_number_01()
{
    const vector<unsigned int> input{2, 4, 6, 8, 10, 12, 10, 8, 6, 4, 12, 12, 4, 2, 4};
    unsigned int uiresult{};

    // unsigned int find_a_number(const vector<unsigned int> &input);
    {
        unsigned int uireturn{};

        for (const auto e : input)
            uireturn ^= e;

        // return the result;
        uiresult = uireturn;
    }

    // result: 12
    cout << "result: " << uiresult << endl;
}

// find a number seen odd times
void t_algo_find_number_02()
{
    const vector<unsigned int> input{2, 4, 6, 8, 10, 12, 10, 8, 6, 4, 12, 12, 4, 2, 4};
    size_t uiresult{};

    // unsigned int find_a_number(const vector<unsigned int> &input);
    {
        map<size_t, size_t> imap{};

        // put them into a map<key, count>
        for (const auto e : input)
            ++imap[e];

        auto pos_return = find_if( imap.cbegin(), imap.cend(),
                [] (const pair<size_t,size_t> &e)
                { 
                    if (e.second % 2) 
                        return true; 

                    return false;
                }
                );

        // return the result;
        // return (pos_return != imap.cend()) ? pos_return->first : 0;
        uiresult = (pos_return != imap.cend()) ? pos_return->first : 0;
    }

    // result: 12
    cout << "result: " << uiresult << endl;
}

// find a number seen odd times
void t_algo_find_number_03()
{
    const vector<unsigned int> input{2, 4, 6, 8, 10, 12, 10, 8, 6, 4, 12, 12, 4, 2, 4};
    size_t uiresult{};

    // unsigned int find_a_number(const vector<unsigned int> &input);
    {
        size_t uireturn{};
        multiset<size_t> imset{input.cbegin(), input.cend()};

        // put them into a map<key, count>
        for (const auto e : imset)
        {
            if ( imset.count(e) % 2)
            {
                uireturn = e;
                break;
            }
        }

        // return the result;
        uiresult = uireturn;
    }

    // result: 12
    cout << "result: " << uiresult << endl;
}

// ={=========================================================================
// swap without a temporary
void t_algo_swap_01()
{
    int a = 9, b = 4;

    cout << "a: " << a << ", b:" << b << endl;

    // void swap(int &a, int &b);
    {
        a = a + b;
        b = a - b;      // b = a
        a = a - b;      // a = b
    }

    // result
    cout << "a: " << a << ", b:" << b << endl;
}


// X XOR  X  = 0
// X XOR  0  = X
// X XOR  1  = ~X    // X XOR (~0) = ~X
// X XOR ~X  = 1     
// 
// 00000000000000000000111111000000 // x
// 00000000000000000000111111000000 // x^0
// 11111111111111111111000000111111 // x^(~0) but not x^1
// 
// x =  1010; y = 0011;          // before
// x =  1001 =  1010  ^ 0011     // x = x^y
// y =  1010 = [1001] ^ 0011     // y = x^y, y = (x^y)^y = (x^0) = x
// x = [0011] =  1001 ^ [1010]   // x = x^y, x = (x^y)^x = (y^0) = y
// x = 0011; y = 1010;           // after

void t_algo_swap_02()
{
    int a = 9, b = 4;

    cout << "a: " << a << ", b:" << b << endl;

    // void swap(int &a, int &b);
    {
        a = a ^ b;
        b = a ^ b;      // b = a^b = (a^b)^b = a^0 = a
        a = a ^ b;      // a = a^b = (a^b)^a = b^0 = b
    }

    // result
    cout << "a: " << a << ", b:" << b << endl;
}


// ={=========================================================================
// find if a string has all unique chars
// 
// o. Space? assume ASCII 256 chars
//    ask for clarity. Since it's only for alphabet uppercase then use 32
//    bset and reduce space requirement.
//
// o. One simple optimization. return false if the length of input string is greater
//    than the number of uniques chars in the set; e.g., ASCII, 256
// 
//    if( sizeString > 256 ) return false;
//    
// o. cstring or std::string?
//
// o. time O(n) and space O(1)

// use lookup table

bool if_unique_01(const char *str)
{
    std::bitset<256> bset{};

    for (; *str; ++str)
    {
        if (bset[*str])
        {
            return false;
        }
        else
            bset[*str] = 1;
    }

    return true;
}


// use cxx-string-find()

bool if_unique_02(const char *str)
{
    std::string unique_set{};

    for (; *str; ++str)
    {
        if (unique_set.find(*str) != string::npos)
        {
            return false;
        }
        else
            unique_set += *str;
    }

    return true;
}

TEST(CxxAlgoTest, FindIfHasAllUniqueChars)
{
    EXPECT_THAT(if_unique_01("abcdefghijklmnopqa"), false);
    EXPECT_THAT(if_unique_01("abcdefghijklmnopqr"), true);

    EXPECT_THAT(if_unique_02("abcdefghijklmnopqa"), false);
    EXPECT_THAT(if_unique_02("abcdefghijklmnopqr"), true);
}


// ={=========================================================================
// anagram
//
// o. Space? assume ASCII 256 chars
//    ask for clarity. Since it's only for alphabet uppercase then use 32
//    bset and reduce space requirement.
//
// o. One simple optimization. return false if the length of input strings
//    are different
// 
// o. cstring or std::string?
//
// o. time O(n) and space O(1)

bool find_anagram_01(string one, string two)
{ 
    if (one.size() != two.size())
        return false;

    sort(one.begin(), one.end());
    sort(two.begin(), two.end());

    return (one == two) ? true : false;
}

bool find_anagram_02(const string one, const string two)
{ 
    if (one.size() != two.size())
        return false;

    bitset<256> bset{};

    for (const auto &e : one)
        bset[e] = 1;

    for (const auto &e : two)
    {
        if (!bset[e])
        {
            return false; 
        }
    }

    return true;
}

TEST(CxxAlgoTest, FindAnagram)
{
    EXPECT_TRUE(find_anagram_01("PARK", "APRK"));
    EXPECT_TRUE(find_anagram_02("PARK", "APRK"));

    EXPECT_FALSE(find_anagram_01("PARK", "APRKPARK"));
    EXPECT_FALSE(find_anagram_02("PARK", "APRKPARK"));

    EXPECT_FALSE(find_anagram_01("PARK", "CARK"));
    EXPECT_FALSE(find_anagram_02("PARK", "CARK"));

    EXPECT_FALSE(find_anagram_01("PARK", "PAAA"));

    // find_anagram_02() fails on:
    // EXPECT_FALSE(find_anagram_02("PARK", "PAAA"));
}


// To pass when there are duplicates in the input:
//  1. remove duplicates
//  2. move size check after removing duplicates.

bool find_anagram_03(string one, string two)
{ 
    bitset<256> bset{};

    auto one_end_unique = unique( one.begin(), one.end() );
    one.erase( one_end_unique, one.end() );

    auto two_end_unique = unique( two.begin(), two.end() );
    two.erase( two_end_unique, two.end() );

    if (one.size() != two.size())
        return false;

    for (const auto &e : one)
        bset[e] = 1;

    for (const auto &e : two)
    {
        if (!bset[e])
        {
            return false; 
        }
    }

    return true;
}

TEST(CxxAlgoTest, FindAnagramWhenDuplicated)
{
    // find_anagram_02() fails on:
    EXPECT_FALSE(find_anagram_03("PARK", "PAAA"));
}


// ={=========================================================================
// // find first unique byte
// * time O(n) space O(1)
// * Do not check input.
// * Each input value are less than 256 adn the # of input are less than unit
//   max.
// * What does "first" mean? first unique byte seen or first unique byte in
//   input order?
//
//   Not byte seen first since every byte is seen first at first time. The
//   problem is that it can be shown in later of a stream.

unsigned char find_first_unique(const vector<unsigned char> &input)
{
    // *TN* size of order depends on the size of input? NO.
    size_t occurance[256]={}, order[256]={};

    // build occurance and order
    size_t input_order{};

    for (const auto e: input)
    {
        ++occurance[e];
        // here order starts from 1
        order[e] = ++input_order;
    }

    // find the first byte
    unsigned char saved_input{};
    // *cxx-limits*
    //
    // const unsigned int UINT_MAX_ORDER = ~((unsigned int)0);
    // unsigned int saved_order = UINT_MAX_ORDER;
    size_t saved_order{numeric_limits<size_t>::max()};

    for (auto i = 0; i < 256; ++i)
    {
        if ((occurance[i] == 1) && (order[i] < saved_order))
        {
            // *TN* i refers to input value
            saved_input = i;
            saved_order = order[i];
        }
    }

    // o. to print a char to int, have to define a var as int or to defind a
    // char and to use +saved_input trick.
    // http://www.cs.technion.ac.il/users/yechiel/c++-faq/print-char-or-ptr-as-number.html
    cout << "saved input : " << +saved_input << endl;
    cout << "saved order : " << saved_order << endl;

    return saved_input;
}

TEST(CxxAlgoTest, FindFirstUniqueByte)
{
    const vector<unsigned char> input_stream{20, 11, 23, 33, 34, 54, 44, 38, 215, 126, 101, 20, 11, 20, 54, 54, 44, 38, 38, 215, 126, 215, 23};
    EXPECT_THAT(find_first_unique(input_stream),Eq(33));
}


// ={=========================================================================
// find if rectangles intersect

// To check assumption:
// 
//     +--------+ top(x2, y2)
//     |        |
//     |        |
//     +--------+
//  bot(x1,y1)
//
//  or top can be upper left and bot can be down right
//  since depending on how Rect is defined, checking can be different. See
//  is_point_in_rect(). 
//
//  this affect also the order of arguments to Rect(). However, not much point
//  of supporting no-assumption case.
//
// This is from ANSIC 130. Handles point rather than each value.

struct Point
{
    // not a default ctor so have to define it
    Point(const int x, const int y) : x_(x), y_(y) {}
    int x_{};
    int y_{};
};

struct Rect
{
    // not a default ctor so have to define it
    // Rect(bot, top);
    Rect(const Point &a, const Point &b) : bot_(a), top_(b) {}
    Point bot_;
    Point top_;
};

// bot.x <= x <= top.x AND bot.y <= y <= top.y
// should be AND
//
// if want to have no assumption then:
//
// return ((r.bot.x <= p.x && p.x <= r.top.x) || (r.top.x <= p.x && p.x <= r.bot.x)) && 
//        ((r.bot.y <= p.y && p.y <= r.top.y) || (r.top.y <= p.y && p.y <= r.bot.y));

bool is_point_in_rect(const Point &point, const Rect &rect)
{
    return ((rect.bot_.x_ <= point.x_) && (point.x_ <= rect.top_.x_)) &&
        ((rect.bot_.y_ <= point.y_) && (point.y_ <= rect.top_.y_));
}

void print_result(const Rect &a, const Rect &b, bool result)
{
    cout << "a: bot (" << a.bot_.x_ << ", " << a.bot_.y_ << ")" 
        << " : top (" << a.top_.x_ << ", " << a.top_.y_ << ")" << endl;

    cout << "b: bot (" << b.bot_.x_ << ", " << b.bot_.y_ << ")" 
        << " : top (" << b.top_.x_ << ", " << b.top_.y_ << ")" << endl;

    cout << (result ? "intersected" : "not intersected") << endl;
}


// = 16 ======
// a: bot (10, 10) : top (20, 20)
// b: bot (15, 15) : top (25, 25)
// intersected
// a: bot (10, 10) : top (20, 20)
// b: bot (20, 20) : top (25, 25)
// intersected

void t_algo_find_intersect()
{
    // bool intersect(rect a, rect b);
    {
        Rect a(Point(10,10), Point(20,20));
        Rect b(Point(15,15), Point(25,25));

        bool result = is_point_in_rect(a.bot_, b) || is_point_in_rect(a.top_, b);

        print_result(a, b, result);
    }
    {

        Rect a(Point(10,10), Point(20,20));
        Rect b(Point(20,20), Point(25,25));

        bool result = is_point_in_rect(a.bot_, b) || is_point_in_rect(a.top_, b);

        print_result(a, b, result);
    }
}


// ={=========================================================================
// find the longest sequence of input char array

MATCHER_P(EqPair, expected, "")
{
    return arg.first == expected.first &&
        arg.second == expected.second;
}

// this is the second try and works fine.
pair<char, size_t> find_longest_01(const string &input)
{
    size_t current_occurance{0}, longest_occurance{0};
    char current_char{0}, longest_char{0};

    for( auto letter : input )
    {
        if( letter != current_char )
        {
            current_char = letter;
            current_occurance = 1;
        }
        // if( letter == current_char )
        else
            ++current_occurance;

        if( current_occurance > longest_occurance )
        {
            longest_char = current_char;
            longest_occurance = current_occurance;
        }
    }

    return pair<char, size_t>(longest_char, longest_occurance);
}


TEST(CxxAlgoTest, FindLongestSequence1)
{
    const string input1{"AAABBCCCCDDDEEFFFFFFFFFFFFFFFFFFHHHSSSSSSSSSS"};
    EXPECT_THAT(find_longest_01(input1), 
            EqPair(pair<char, size_t>('F', 18)));

    const string input2{"AAABBCCCCDDD"};
    EXPECT_THAT(find_longest_01(input2), 
            EqPair(pair<char, size_t>('C', 4)));

    const string input3{"AAAAAAAAAAAA"};
    EXPECT_THAT(find_longest_01(input3), 
            EqPair(pair<char, size_t>('A', 12)));
}


// Firstly, looks better but it fails when the input has one long sequence.
pair<char, size_t> find_longest_02(const string &input)
{
    char current_char{}, longest_char{};
    size_t current_occurance{}, longest_occurance{};

    for (auto letter : input)
    {
        // if see the different char. use XOR and looks fancy?
        if (current_char^letter)
        {
            // save it if it's the longest so far
            if(current_occurance > longest_occurance)
            {
                longest_occurance = current_occurance;
                longest_char = current_char;
            }

            // reset and str a search again
            current_char = letter;
            current_occurance = 1;
        }
        // if see the same before
        else
            ++current_occurance;
    }

    return pair<char, size_t>(longest_char, longest_occurance);
}

TEST(CxxAlgoTest, FindLongestSequence2)
{
    const string input1{"AAABBCCCCDDDEEFFFFFFFFFFFFFFFFFFHHHSSSSSSSSSS"};
    EXPECT_THAT(find_longest_02(input1), 
            EqPair(pair<char, size_t>('F', 18)));

    const string input2{"AAABBCCCCDDD"};
    EXPECT_THAT(find_longest_02(input2), 
            EqPair(pair<char, size_t>('C', 4)));

    // fails on this case
    // const string input3{"AAAAAAAAAAAA"};
    // EXPECT_THAT(find_longest_02(input3), 
    //         EqPair(pair<char, size_t>('A', 12)));
}


// To do better than O(n), can skip some chars in searching a sequence.
pair<char, size_t> t_algo_find_longest_02(const string &input)
{
    char current_char{}, longest_char{};
    size_t current_occurance{}, longest_occurance{};

    size_t input_size = input.size();

    // take the first
    current_char = input[0];
    current_occurance = 1;
    
    // *TN* 
    // should kepp ++i here. Otherwise, would have no increase of i and
    // compare the same input in a loop.
    for (size_t i = 1; i < input_size; ++i)
    {
        // if see the different char. use XOR and looks fancy?
        if (current_char^input[i])
        {
            // save the previous sequence if it's the longest so far
            if (current_occurance > longest_occurance)
            {
                longest_occurance = current_occurance;
                longest_char = current_char;
            }

            // see i and i + (current longest sequence-1) and skip them in
            // between if they are different
            //
            // if they are the same, don't skip so don't change i
            size_t check_skip = i + (current_occurance-1);

            // *TN*
            // Have to have this check. Otherwise, access to out of range
            // of input.
            if (check_skip > input_size)
                break;

            // if they are different
            if (input[i]^input[check_skip])
            {
                cout << "skipped : " << current_occurance-1 << endl;
                i += current_occurance-1;
            }

            // reset and str a search again
            current_char = input[i];
            current_occurance = 1;
        }
        // if see the same before
        else
            ++current_occurance;
    }

    cout << "longest char      : " << longest_char << endl;
    cout << "longest occurance : " << longest_occurance << endl;

    return pair<char, size_t>(longest_char, longest_occurance);
}

TEST(CxxAlgoTest, FindLongestSequenceBetter)
{
    const string input1{"AAABBCCCCDDDEEFFFFFFFFFFFFFFFFFFHHHSSSSSSSSSS"};
    EXPECT_THAT(t_algo_find_longest_02(input1), 
            EqPair(pair<char, size_t>('F', 18)));

    const string input2{"AAABBCCCCDDD"};
    EXPECT_THAT(t_algo_find_longest_02(input2), 
            EqPair(pair<char, size_t>('C', 4)));

    const string input3{"AAAABCBBBBBCCCCDDD"};
    EXPECT_THAT(t_algo_find_longest_02(input3), 
            EqPair(pair<char, size_t>('B', 5)));
}

TEST(CxxAlgoTest, DISABLED_FailLongestSequenceBetter)
{
    const string input3{"AAAABCCCCCCCC"};
    EXPECT_THAT(t_algo_find_longest_02(input3), 
            EqPair(pair<char, size_t>('C', 8)));
}


// ={=========================================================================
// atoi
//
// * input type? digits only? no space?
// * input size?
// * what base? 10 or 2?
// * sign support? 

// from ansic, p43.
//
// when base is 10.
//
// this is 'naive' implementation since no error handlings and return 0 when
// failed to convert. compare to strtol
//
// there is no check on the end of string input? '0' is not the same as
// 0(null) and when see any other than numbers, for loops ends.

uint32_t atoi_navie(const char *str)
{
    uint32_t value{0}, i{0};

    for (; str[i] >= '0' && str[i] <= '9'; ++i)
    {
        value = value*10 + (str[i] - '0');
    }

    return value;
}

// use isdigit()

uint32_t atoi_isdigit(const char *str)
{
    uint32_t value{0}, i{0};

    for (; isdigit(str[i]); ++i)
    {
        value = value*10 + (str[i] - '0');
    }

    return value;
}

// To support sign and leading space

uint32_t atoi_sign(const char *str)
{
    uint32_t value{0}, i{0}, sign{0};

    while (isspace(str[i]))
        ++i;

    // check sign but don't need to increase i
    sign = str[i] == '-' ? -1 : 1;

    // have to increase
    if (str[i] == '-' || str[i] == '+')
        ++i;

    for (; isdigit(str[i]); ++i)
    {
        value = value*10 + (str[i] - '0');
    }

    return sign*value;
}

// use base 2. value is right but not representation

// Chapter 7: String Fundamentals, 207
// #!/usr/bin/python
//
// B = '1101'
// I = 0
//
// while B != '':
//     I = I*2 + (ord(B[0]) - ord('0'))
//     B = B[1:]
//
// print(I)

uint32_t atoi_binary(const char *str)
{
    uint32_t value{0}, i{0};

    for (; str[i] >= '0' && str[i] <= '9'; ++i)
    {
        value = value*2 + (str[i] - '0');
    }

    return value;
}


// use base 16. htoi
//
// From ansic, exercise 2-3. 
//
// Write the function htoi(s), which converts a string of hexadecimal digits
// (including an 'optional' 0x or 0X) into its equivalent integer value. The
// allowable digits are 0 through 9, a through f, and A through F.
//
// isxdigit()
//        checks for hexadecimal digits, that is, one of
//        0 1 2 3 4 5 6 7 8 9 a b c d e f A B C D E F.

// previous try
//
// int htoi(char s[])
// {
//   int n, i = 0, v = 0;
// 
//   // optional 0x or 0X
//   if(s[0] == '0' && ( s[1] == 'x' || s[1] == 'X' ))
//     i = 2;
// 
//   // isxdigit()
//   // checks for a hexadecimal digits, that is, one of 
//   // 0 1 2 3 4 5 6 7 8 9 a b c d e f A B C D E F.
//    
//   for(n = 0; isxdigit(s[i]); i++)
//   {
//     if( s[i] >= '0' && s[i] <= '9' )
//       v = s[i] - '0';
//     else if( s[i] >= 'a' && s[i] <= 'f' )
//       v = s[i] - 'a' + 10;
//     else 
//       v = s[i] - 'A' + 10;
// 
//     n = n*16 + v;
//   }
// 
//   return n;
// }

uint32_t atoi_hex(const char *str)
{
    uint32_t value{0}, i{0};
    const std::string hex{"0123456789abcdef"};

    for (; hex.find(std::tolower(str[i])) != std::string::npos; ++i)
    {
        value = value*16 + hex.find(std::tolower(str[i]));
    }

    return value;
}


TEST(CxxAlgoAtoiTest, RunWithVariousValues)
{
    EXPECT_THAT(atoi_navie("123"), Eq(123));
    EXPECT_THAT(atoi_isdigit("123"), Eq(123));

    EXPECT_THAT(atoi_sign("123"), Eq(123));
    EXPECT_THAT(atoi_sign("-123"), Eq(-123));

    EXPECT_THAT(atoi_binary("1101"), Eq(13));

    EXPECT_THAT(atoi_hex("1a"), Eq(26));
    EXPECT_THAT(atoi_hex("1A"), Eq(26));
}


// ={=========================================================================
// itoa
//
// * input type? digits only? no space?
// * input size?
// * what base? 10 or 2?
// * sign support? 

// from ansic, p43.
//
// when base is 10.
//
// there is no check on the end of string input? '0' is not the same as
// 0(null) and when see any other than numbers, for loops ends.

std::string itoa_navie(const int input)
{
    int value{input};
    char letter{0};
    std::string result{};

    for (; value;)
    {
        letter = '0' + (value % 10);
        result += letter;
        value /= 10;
    }

    return std::string(result.crbegin(), result.crend());
}

std::string itoa_no_reverse(const int input)
{
    std::string result{};
    std::string temp;
    char letter{0};
    int value{input};

    for(; value;)
    {
        letter = '0' + (value % 10);
        temp += letter;
        result.insert( 0, temp );
        value /= 10;
    }

    return result;
}

TEST(CxxAlgoItoaTest, RunWithVariousValues)
{
    EXPECT_THAT(itoa_navie(123), Eq("123"));
    EXPECT_THAT(itoa_no_reverse(123), Eq("123"));
}


// ={=========================================================================
// count same bits between two integers
//
// A = 35 = 10 0011
// B =  9 =    1001

// Ans = 2 because only counts bit positions which are valid position in both
// integers.

// From ansic, p50. 
// The function counts the number of 1 bits in its integer argument. 
//
// 1. The key is not to use sizeof operator
// 2. unsigned argument
// 3. use independent of type.

// returns bit position starting from 1 since input >> is evalueated after
// ++count.
uint32_t count_bits_01(const uint32_t value)
{
    uint32_t count{};
    uint32_t input = value;

    // do not need to check like: if (input &1) to increase count for every
    // interation since when runs out 1, input becomes 0 and the loop ends. 
    for (; input != 0; input >>= 1)
        ++count;

    return count;
}

// page 51. exercise 2-9. In a two's complement number system, x &= (x-1) deletes
// the rightmost 1-bit in x. Explain why. Use this observation to write a 'faster'
// version of bitcount.
// 
// Answer:
// 
// If x is odd, then (x-1) has the same bit representation as x except that the
// rightmost 1-bit becomes a 0. In this case, (x & (x-1)) == (x-1).
// 
// x = 5: 5(101) & 4(100) = 100  // 101 -> 100 by having rightmost 1 to 0
// 
// If x is even, the end result of anding(&) x and x-1 has the rightmost 1 of x to 0.
// 
// x = 4: 4(100) & 3(11)  = 0    // 100 -> 0   by having rightmost 1 to 0
//          ^ rightmost 1
// x = 6: 6(110) & 5(101) = 100  // 110 -> 100 by having rightmost 1 to 0
//           ^ rightmost 1
// x = 8: 8(1000) & 7(111) = 0   // 1000 -> 0  by having rightmost 1 to 0
// 
// 000   0     All even numbers has tailing 0s and it becomes 1 when minus 1
// 001   1
// 010   2
// 011   3
// 100   4
// 101   5
// 110   6
// 111   7
// ...
// 
// note: This is about careful observation but not a mechanism of borrowing a carry
// for example. For both odd and even case, has the effect of having rightmost 1 to
// 0. So clear 1 from x one by one and no need to check on if to count bits.
// 
// note: And(&) is faster than shift operation? Yes and also there is no `if` in
// the loop.
// 
// int bitcount(unsigned x)
// {
//   int b;
// 
//   for (b = 0; x != 0; x &= (x-1))
//     b++;
//   return b;
// }

// but this fails. WHY?
uint32_t count_bits_02(const uint32_t value)
{
    uint32_t count{};
    uint32_t input = value;

    for (; input != 0; input &= (input-1))
        ++count;

    return count;
}


// returns bit position starting from 0 since input >> is evalueated before
// ++count.

uint32_t count_bits_03(const uint32_t value)
{
    uint32_t count{};
    uint32_t input = value;

    for (; input && (input >>=1);)
        ++count;

    return count;
}

TEST(CxxAlgoTest, FindNumberOfBits)
{
    // A = 35 = 10 0011
    // B =  9 =    1001
    EXPECT_THAT(count_bits_01(35), Eq(6));
    EXPECT_THAT(count_bits_01(9), Eq(4));

    // EXPECT_THAT(count_bits_02(35), Eq(6));
    // EXPECT_THAT(count_bits_02(9), Eq(4));

    EXPECT_THAT(count_bits_03(35), Eq(5));
    EXPECT_THAT(count_bits_03(9), Eq(3));
}

uint32_t find_same_number_of_bits(const uint32_t first, const uint32_t second)
{
    // get the smaller between inputs
    uint32_t small = first > second ? second : first;

    // find the position of highest 1. position starts from 0.
    // however, assumes 32 bits
    //
    // uint32_t top_pos{};
    // uint32_t value = small;
    //
    // for (int i = 0; i < 32; ++i)
    // {
    //     if (value & 1u)
    //         top_pos = i;
    //
    //     value >>= 1;
    // }

    // see *ex-bitcount*
    // uint32_t count_bits_03(const uint32_t value)
    // { 
    uint32_t top_pos{};

    for (; small && (small >>=1);)
        ++top_pos;
    // }

    // cout << "top_pos: " << top_pos << endl;

    // when use substraction, have to decide which one is bigger to have non
    // negative number. 
    // uint32_t diff = first - second;
    
    // so xor is better.
    uint32_t diff = first^second;
    uint32_t count{};

    for(uint32_t i = 0; i <= top_pos; ++i)
    {
        if(!(diff & 1u))
            ++count;

        diff >>= 1;
    }

    return count;
}

TEST(CxxAlgoTest, FindNumberOfBitsBetweenTwoIntegers)
{
    // 35 = 10 0011
    // 9 =     1001
    //          ^ ^
    EXPECT_THAT(find_same_number_of_bits(35, 9), Eq(2));

    // 55 = 10 0111
    // 5 =      101
    //          ^ ^
    EXPECT_THAT(find_same_number_of_bits(55, 5), Eq(2));
}


int main(int argc, char** argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
