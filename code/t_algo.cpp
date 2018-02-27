#include <iostream>
#include <cstring>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <bitset>

// g++ -g -std=c++0x t_override.cpp

using namespace std;

// ={=========================================================================
void t_algo_reverse_01()
{
    char input[] = "REVERSE IT";

    cout << "input : " << input << endl;

    // o. should return through argument and no const
    // o. most efficient
    // o. strlen()-1 since array indexing is always [0, size-1], or [0,size)
    //
    // void reverse(char *str);
    {
      char *start = input;
      char *end = input+(strlen(input)-1);

      for (char temp; start < end;)
      {
          // swap(start, end)
          temp = *start;
          *start++ = *end;
          *end-- = temp;
      }
    }

    cout << "result: " << input << endl;
}

void t_algo_reverse_02()
{
    char input[] = "REVERSE IT";

    cout << "input : " << input << endl;

    // o. ansic, p62
    //
    // void reverse(char *str);
    {
      int start{}, end{};
      char temp{}; 

      for (end = strlen(input)-1; start < end; ++start, --end)
      {
          // swap(start, end)
          temp = input[start], input[start] = input[end], input[end] = temp;
      }
    }

    cout << "result: " << input << endl;
}

void t_algo_reverse_03()
{
    char input[] = "REVERSE IT";
    std::string rstring{};

    cout << "input : " << input << endl;

    // o. if can use std::string and can return although c version is better.
    // o. use *cpp-reverse-iterator*
    //
    // void reverse(char *str);
    {
        std::string istring{input};

        // return std::string{istring.crend(), istring.crbegin()};
        rstring = std::string{istring.crbegin(), istring.crend()};
    }

    cout << "result: " << rstring << endl;
}

void t_algo_reverse_03_01()
{
    char input[] = "REVERSE IT";
    std::string rstring{};

    cout << "input : " << input << endl;

    // when want a copy
    // void reverse(char *str);
    {
        std::string istring{input};

        rstring.assign(istring.crbegin(), istring.crend());
    }

    cout << "result: " << rstring << endl;
}

void t_algo_reverse_03_02()
{
    char input[] = "REVERSE IT";
    std::string rstring{};

    cout << "input : " << input << endl;

    // when want to modify it itself
    // void reverse(char *str);
    {
        std::string istring{input};

        std::reverse(istring.begin(), istring.end());
        rstring = istring;
    }

    cout << "result: " << rstring << endl;
}

void t_algo_reverse_04()
{
    char input[] = "REVERSE IT";
    std::string rstring{};

    cout << "input : " << input << endl;

    // o. if can use std::string and can return although c version is better.
    // o. from C++PL 280
    //
    // void reverse(char *str);
    {
        std::string istring{input};
        std::string reverse{};

        for ( size_t size = istring.size(); size > 0; --size)
            reverse.push_back(istring[size-1]);

        // return reverse;
        rstring = reverse;
    }

    cout << "result: " << rstring << endl;
}

// void t_algo_reverse_05()
// {
//     char input[] = "REVERSE IT";
//     std::string rstring{};
// 
//     cout << "input : " << input << endl;
// 
//     // o. use algorithm? it's overkill.
//     //
//     // void reverse(char *str);
//     {
//         // copy the elements of coll1 into coll3 by inserting them at the front
//         // *cpp-reverse* reverses the order of the elements
//         deque<int> coll3;
//         copy (coll1.cbegin(), coll1.cend(),  // source
//                 front_inserter(coll3));          // destination
//     }
// 
//     cout << "result: " << rstring << endl;
// }


// ={=========================================================================
// Write a program to reverse a string with all its duplicates removed. Only the
// last instance of a character in the reverse string has to appear. Also, the
// following conditions are to be satisfied: Assume only Capital Letters.

void t_algo_reverse_06()
{
    char input[] = "JTVAKAVISHAAAL";
    std::string sresult{};

    cout << "input : " << input << endl;

    // o. assume that input is ASCII and is all upper case chars. so can use
    // unsigned int to check if it's a duplicate or not. if needs more range to
    // cover then need to use something else.
    //
    // o. from the net
    //
    // std::string reverse_without_duplicate(const std::string str);
    {
        std::string sin{input};
        std::string sout{};
        unsigned int bappeared{};

        // remove duplicates from input
        for (size_t i = 0, size = sin.size(); i < size; ++i)
        {
            // only if not appeared before
            if (!(bappeared & (1 << (sin[i] - 'A'))))
            {
                sout += sin[i];
                bappeared |= (1 << (sin[i]-'A'));
            }
        }

        // return reverse;
        std::string sreturn{sout.crbegin(), sout.crend()};
        sresult = sreturn;
    }

    cout << "result: " << sresult << endl;
}

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
                [](const pair<size_t,size_t> &e)
                { if (e.second % 2) return true; }
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
void t_algo_if_unique_01()
{
    // false
    // char input[] = "abcdefghijklmnopqa";

    // true
    char input[] = "abcdefghijklmnopqr";
    bool breturn{true};

    cout << "input : " << input << endl;

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
    //
    // bool unique(const char *str);
    {
        std::bitset<256> bset{};

        for (char *start = input; *start; ++start)
        {
            if (bset[*start])
            {
                // return false;
                breturn = false; break;
            }
            else
                bset[*start] = 1;
        }

        // return true;
    }

    // result
    cout << "result: " << breturn << endl;
}


void t_algo_if_unique_02()
{
    // false
    // char input[] = "abcdefghijklmnopqa";

    // true
    char input[] = "abcdefghijklmnopqr";
    bool breturn{true};

    cout << "input : " << input << endl;

    // bool unique(const char *str);
    {
        std::string unique_set{};

        for (char *start = input; *start; ++start)
        {
            if (unique_set.find(*start) != string::npos)
            {
                // return false;
                breturn = false; break;
            }
            else
                unique_set += *start;
        }

        // return true;
    }

    // result
    cout << "result: " << breturn << endl;
}

// ={=========================================================================
// anagram
void t_algo_anagram_01()
{
    // true
    string one ="PARK";
    // string two ="APRK";
    // string two ="CARK";
    string two ="PAAA";
    bool breturn{true};

    cout << "input string: " << one << ", " << two << endl;

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
    //
    // bool anagram(const string one, const string two);
    { 
        if (one.size() != two.size())
            breturn = false;    // return false;

        sort(one.begin(), one.end());
        sort(two.begin(), two.end());

        breturn = ((one == two) ? true : false);
    }

    // result
    cout << "result: " << breturn << endl;
}

void t_algo_anagram_02()
{
    // true
    string one ="PARK";
    // string two ="APRK";
    // string two ="CARK";
    string two ="PAAA";
    bool breturn{true};

    cout << "input string: " << one << ", " << two << endl;

    // bool anagram(const string one, const string two);
    { 
        bitset<256> bset{};

        if (one.size() != two.size())
            breturn = false;    // return false;

        for (const auto &e : one)
            bset[e] = 1;

        for (const auto &e : two)
        {
            if (!bset[e])
            {
                breturn = false; 
                break;
            }
        }

        // return true;
    }

    // result
    cout << "result: " << breturn << endl;
}

// ={=========================================================================
// // find first unique byte
// void t_algo_find_first_unique()
// {
//     const vector<unsigned char> input{ 20, 11, 23, 33, 34, 54, 44, 38, 215, 126, 101, 20, 11, 20, 54, 54, 44, 38, 38, 215, 126, 215, 23};
//     unsigned char breturn{};
// 
//     // o. do hot check in the input. each value should be less than 256 and the
//     // num of input less than uint max
//    
//     // char find_first_unique(const vector<char> &input);
//     { 
//         unsigned int cset_occurance[256]={}, cset_order[256]={};
// 
//         // time O(n), space O(1)
//         // builds occurance and order set
//         size_t input_order{};
//         for (auto start = input.cbegin(); start != input.cend(); ++start)
//         {
//             ++cset_occurance[*start];
//             cset_order[*start] = input_order;
//             ++input_order;
//         }
// 
//         // time O(1), space O(1)
//         // search the first and unique byte
//         
//         // o. can use <limits> facility to get unit max
//         size_t saved_order{numeric_limits<size_t>::max()};
// 
//         // const unsigned int UINT_MAX_ORDER = ~((unsigned int)0);
//         // unsigned int saved_order = UINT_MAX_ORDER;
// 
//         unsigned char saved_input{};
// 
//         for (auto start = 0; start < 256; ++start)
//         {
//             // see unique
//             if (cset_occurance[start] == 1)
//             {
//                 if (cset_order[start] < saved_order)
//                 {
//                     saved_order = cset_order[start];
//                     saved_input = start;
//                 }
//             }
//         }
// 
//         // o. to print a char to int, have to define a var as int or to defind a
//         // char and to use +saved_input trick.
//         // http://www.cs.technion.ac.il/users/yechiel/c++-faq/print-char-or-ptr-as-number.html
//         cout << "saved_order: " << saved_order << ", saved_input: " << +saved_input << endl;
//         cout << "saved_order: " << saved_order << ", saved_input: " << static_cast<unsigned>(saved_input) << endl;
// 
//         // return saved_input;
//         breturn = saved_input;
//     }
// 
//     // result
//     cout << "result: " << +breturn << endl;
//     cout << "result: " << static_cast<unsigned>(breturn) << endl;
//     
//     // = 15 ======
//     // saved_order: 3, saved_input: 33
//     // saved_order: 3, saved_input: 33
//     // result: 33
//     // result: 33
// }

// * time O(n) space O(1)
// * Do not check input.
// * Each input value are less than 256 adn the # of input are less than unit
//   max.
// * What does "first" mean? first unique byte seen or first unique byte in
//   input order?
//
//   Not byte seen first since every byte is seen first at first time. The
//   problem is that it can be shown in later of a stream.

// = 15 ======
// saved input : 33
// saved order : 3
// result : 33

void t_algo_find_first_unique()
{
    const vector<unsigned char> input_stream{20, 11, 23, 33, 34, 54, 44, 38, 215, 126, 101, 20, 11, 20, 54, 54, 44, 38, 38, 215, 126, 215, 23};
    unsigned char result{};

    // unsigned char find_first_unique(const vector<char> &input);
    {
        // *TN* size of order depends on the size of input? NO.
        size_t occurance[256]={}, order[256]={};

        // build occurance and order
        size_t input_order{};

        for (const auto e: input_stream)
        {
            ++occurance[e];
            order[e] = input_order++;
        }

        // find the first byte
        unsigned char saved_input{};
        // *cxx-limits*
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

        cout << "saved input : " << +saved_input << endl;
        cout << "saved order : " << saved_order << endl;

        result = saved_input;
    }

    cout << "result : " << static_cast<size_t>(result) << endl;
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

void t_algo_find_longest_01()
{
    // since std::vector/array is not useful here. not cstring array and string
    // is useful since provices [] access and do not include '\0'.

    // = 17 ======
    // longest char      : F
    // longest occurance : 18
    const string input = "AAABBCCCCDDDEEFFFFFFFFFFFFFFFFFFHHHSSSSSSSSSS";

    //
    // = 17 ======
    // longest char      : C
    // longest occurance : 4
    // const string input = "AAABBCCCCDDD";

    size_t input_size = input.size();
    char result{};

    // pair<char, size_t> find_longest(size_t size, const vector<char> &input);
    {
        char current_char{}, longest_char{};
        size_t current_occurance{}, longest_occurance{};

        // take the first
        current_char = input[0];
        current_occurance = 1;

        for (size_t i = 1; i < input_size; ++i)
        {
            // if see the different char. use XOR and looks fancy?
            if (current_char^input[i])
            {
                // save it if it's the longest so far
                if(current_occurance > longest_occurance)
                {
                    longest_occurance = current_occurance;
                    longest_char = current_char;
                }

                // reset and start a search again
                current_char = input[i];
                current_occurance = 1;
            }
            // if see the same before
            else
                ++current_occurance;
        }

        cout << "longest char      : " << longest_char << endl;
        cout << "longest occurance : " << longest_occurance << endl;
    }
}

void t_algo_find_longest_02()
{
    // = 18 ======
    // skipped : 2
    // skipped : 3
    // skipped : 17
    // longest char      : F
    // longest occurance : 18
    const string input = "AAABBCCCCDDDEEFFFFFFFFFFFFFFFFFFHHHSSSSSSSSSS";
    
    // = 18 ======
    // longest char      : C
    // longest occurance : 4
    // const string input = "AAABBCCCCDDD";

    size_t input_size = input.size();
    char result{};

    // pair<char, size_t> find_longest(size_t size, const vector<char> &input);
    {
        char current_char{}, longest_char{};
        size_t current_occurance{}, longest_occurance{};

        // take the first
        current_char = input[0];
        current_occurance = 1;

        
        for (size_t i = 1; i < input_size; ++i)
        {
            // if see the different char. use XOR and looks fancy?
            if (current_char^input[i])
            {
                // save it if it's the longest so far
                if (current_occurance > longest_occurance)
                {
                    longest_occurance = current_occurance;
                    longest_char = current_char;
                }

                size_t check_skip = i + (current_occurance-1);

                if (check_skip > input_size)
                    break;

                if (input[i]^input[check_skip])
                {
                    cout << "skipped : " << current_occurance-1 << endl;
                    i += current_occurance-1;
                }

                // reset and start a search again
                current_char = input[i];
                current_occurance = 1;
            }
            // if see the same before
            else
                ++current_occurance;
        }

        cout << "longest char      : " << longest_char << endl;
        cout << "longest occurance : " << longest_occurance << endl;
    }
}


int main()
{
    // cout << "= 18 ======" << endl;
    // t_algo_find_longest_02();

    // cout << "= 17 ======" << endl;
    // t_algo_find_longest_01();

    // cout << "= 16 ======" << endl;
    // t_algo_find_intersect();

    // cout << "= 15 ======" << endl;
    // t_algo_find_first_unique();

    cout << "= 13 ======" << endl;
    t_algo_if_unique_02();

    cout << "= 12 ======" << endl;
    t_algo_if_unique_01();
    
    // cout << "= 01 ======" << endl;
    // t_algo_reverse_01();

    // cout << "= 02 ======" << endl;
    // t_algo_reverse_02();
   
    // cout << "= 03_02 ======" << endl;
    // t_algo_reverse_03_02();
    
    // cout << "= 03_01 ======" << endl;
    // t_algo_reverse_03_01();

    // cout << "= 03 ======" << endl;
    // t_algo_reverse_03();

    // cout << "= 04 ======" << endl;
    // t_algo_reverse_04();

    // cout << "= 06 ======" << endl;
    // t_algo_reverse_06();

    // = 06 ======
    // input : JTVAKAVISHAAAL
    // result: LHSIKAVTJ
    
    // cout << "= 07 ======" << endl;
    // t_algo_find_number_01();

    // cout << "= 08 ======" << endl;
    // t_algo_find_number_02();

    // cout << "= 09 ======" << endl;
    // t_algo_find_number_03();

    // cout << "= 10 ======" << endl;
    // t_algo_swap_01();

    // cout << "= 11 ======" << endl;
    // t_algo_swap_02();
    

    // cout << "= 13 ======" << endl;
    // t_algo_anagram_01();

    // cout << "= 14 ======" << endl;
    // t_algo_anagram_02();
}
