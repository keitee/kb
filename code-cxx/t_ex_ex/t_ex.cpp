#include "gmock/gmock.h"

#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <string>
<<<<<<< HEAD
#include <bitset>
=======
#include <chrono>
#include <thread>
>>>>>>> 2b315751c49bfc4250ceed8e7a5ead381afd6f88
#include <boost/algorithm/string.hpp>


// g++ -g -std=c++0x t_override.cpp

using namespace std;
using testing::ElementsAre;
using testing::Eq;
using testing::StrEq;
using testing::FloatEq;


// ={=========================================================================
// MakeTonen
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


// TEST(StringStream, X)
// {
//   size_t number_of_entries{};
//   cin >> number_of_entries;

//   map<string, string> phone_map{};

//   for (auto i = 0; i < number_of_entries; ++i)
//   {
//     // string line;
//     // getline(cin >> ws, line);
//     // stringstream ss(line);

//     string name, number;
//     stringstream ss;
//     ss >> name;
//     ss >> number;

//     phone_map[name] = number;
//   }

//   string query;

//   while (cin >> query)
//   {
//     if (phone_map.find(query) != phone_map.end())
//       cout << query << "=" << phone_map[query] << endl;
//     else
//       cout << "Not found" << endl;
//   }
// }

template<typename Policy_T>
struct ConstrainedValue
{
  public:
    // public typedefs
    //
    // t_ex.cpp:435:22: error: expected nested-name-specifier before ‘Policy_T’
    //      typedef typename Policy_T policy_type;
    //                       ^
    // typedef typename Policy_T policy_type;

    typedef Policy_T policy_type;
    typedef typename Policy_T::value_type value_type;
    typedef ConstrainedValue self;
    // default constructor
    ConstrainedValue( ) : m(Policy_T::default_value) { }
    ConstrainedValue(const self& x) : m(x.m) { }
    ConstrainedValue(const value_type& x) 
    { 
      cout << "converting ctor" << endl;
      Policy_T::assign(m, x); 
    }

    operator value_type( ) const { return m; }

    // uses the policy defined assign function
    void assign(const value_type& x) {
      Policy_T::assign(m, x);
    }

    // assignment operations
    self& operator=(const value_type& x) { assign(x); return *this; }
    self& operator+=(const value_type& x) { assign(m + x); return *this; }
    self& operator-=(const value_type& x) { assign(m - x); return *this; }
    self& operator*=(const value_type& x) { assign(m * x); return *this; }
    self& operator/=(const value_type& x) { assign(m / x); return *this; }
    self& operator%=(const value_type& x) { assign(m % x); return *this; }
    self& operator>>=(int x) { assign(m >> x); return *this; }
    self& operator<<=(int x) { assign(m << x); return *this; }

    // unary operations
    self operator-( ) { return self(-m); }
    self operator+( ) { return self(+m); }
    self operator!( ) { return self(!m); }
    self operator~( ) { return self(~m); }
    // binary operations
    friend self operator+(self x, const value_type& y) { return x += y; }
    friend self operator-(self x, const value_type& y) { return x -= y; }
    friend self operator*(self x, const value_type& y) { return x *= y; }
    friend self operator/(self x, const value_type& y) { return x /= y; }
    friend self operator%(self x, const value_type& y) { return x %= y; }
    friend self operator+(const value_type& y, self x) { return x += y; }
    friend self operator-(const value_type& y, self x) { return x -= y; }
    friend self operator*(const value_type& y, self x) { return x *= y; }
    friend self operator/(const value_type& y, self x) { return x /= y; }
    friend self operator%(const value_type& y, self x) { return x %= y; }
    friend self operator>>(self x, int y) { return x >>= y; }
    friend self operator<<(self x, int y) { return x <<= y; }
    // stream operators
    friend ostream& operator<<(ostream& o, self x) { o << x.m; return o; }
    friend istream& operator>>(istream& i, self x) {
      value_type tmp; i >> tmp; x.assign(tmp); return i;
    }
    // comparison operators
    friend bool operator<(const self& x, const self& y) { return x.m < y.m; }
    friend bool operator>(const self& x, const self& y) { return x.m > y.m; }
    friend bool operator<=(const self& x, const self& y) { return x.m <= y.m; }
    friend bool operator>=(const self& x, const self& y) { return x.m >= y.m; }
    friend bool operator==(const self& x, const self& y) { return x.m == y.m; }
    friend bool operator!=(const self& x, const self& y) { return x.m != y.m; }
  private:
    value_type m;
};

template<int Min_N, int Max_N>
struct RangedIntPolicy
{
  typedef int value_type;
  const static value_type default_value = Min_N;
  static void assign(value_type& lvalue, const value_type& rvalue) {
    if ((rvalue < Min_N) || (rvalue > Max_N)) {
      throw range_error("out of valid range");
    }
    lvalue = rvalue;
  }
};

typedef ConstrainedValue< RangedIntPolicy<1582, 4000> > GregYear;
typedef ConstrainedValue< RangedIntPolicy<1, 12> > GregMonth;
typedef ConstrainedValue< RangedIntPolicy<1, 31> > GregDayOfMonth;

void gregOutputDate(GregDayOfMonth d, GregMonth m, GregYear y) {
  cout << m << "/" << d << "/" << y << endl;
}

TEST(Ranged, X)
{
  try {
    gregOutputDate(14, 7, 2005);
  }
  catch(...) {
    cerr << "whoops, shouldn't be here" << endl;
  }
  try {
    gregOutputDate(1, 13, 1582);
    // gregOutputDate(1, 5, 1148);
    cerr << "whoops, shouldn't be here" << endl;
  }
  catch(...) {
    cerr << "are you sure you want to be using a Gregorian Calendar?" << endl;
  }
}


// ={=========================================================================
string convert_to_roman(unsigned int arabic) 
{
  string convert{};

  // note:
  // 1. the order of element in the map matters
  // 2. do not need 6?? in the map since it follows the same addition rule
  // 3. to fix a warning on signed int and unsigned int comparion, use "u"
  // suffix.

  const auto lookup_table = {
    make_pair(1000, "M"),
    make_pair(900, "CM"),
    // make_pair(600, "DC"),
    make_pair(500, "D"),
    make_pair(400, "CD"),
    make_pair(100, "C"),
    //
    make_pair(90, "XC"),
    // make_pair(60, "LX"),
    make_pair(50, "L"),
    make_pair(40, "XL"),
    make_pair(10, "X"),
    //
    make_pair(9, "IX"),
    // make_pair(6, "VI"),
    make_pair(5, "V"),
    make_pair(4, "IV"),
    make_pair(1, "I")
  };

  for (const auto e : lookup_table)
  {
    while (e.first <= arabic)
    {
      arabic -= e.first;
      convert += e.second;
    }
  }

  // cout << "converted: " << convert << endl;

  return convert;
}

TEST(RomanConverter, CanConvertPositiveDigits) 
{
  EXPECT_THAT(convert_to_roman(1), Eq("I"));
  EXPECT_THAT(convert_to_roman(2), Eq("II"));
  EXPECT_THAT(convert_to_roman(3), Eq("III"));
  EXPECT_THAT(convert_to_roman(4), Eq("IV"));
  EXPECT_THAT(convert_to_roman(5), Eq("V"));
  EXPECT_THAT(convert_to_roman(6), Eq("VI"));
  EXPECT_THAT(convert_to_roman(7), Eq("VII"));
  EXPECT_THAT(convert_to_roman(8), Eq("VIII"));
  EXPECT_THAT(convert_to_roman(9), Eq("IX"));
  EXPECT_THAT(convert_to_roman(10), Eq("X"));
  EXPECT_THAT(convert_to_roman(11), Eq("XI"));
  EXPECT_THAT(convert_to_roman(12), Eq("XII"));
  EXPECT_THAT(convert_to_roman(13), Eq("XIII"));
  EXPECT_THAT(convert_to_roman(16), Eq("XVI"));
  EXPECT_THAT(convert_to_roman(17), Eq("XVII"));
  EXPECT_THAT(convert_to_roman(18), Eq("XVIII"));
  EXPECT_THAT(convert_to_roman(20), Eq("XX"));
  EXPECT_THAT(convert_to_roman(23), Eq("XXIII"));
  EXPECT_THAT(convert_to_roman(41), Eq("XLI"));
  EXPECT_THAT(convert_to_roman(45), Eq("XLV"));
  EXPECT_THAT(convert_to_roman(50), Eq("L"));
  EXPECT_THAT(convert_to_roman(80), Eq("LXXX"));
  EXPECT_THAT(convert_to_roman(91), Eq("XCI"));
  EXPECT_THAT(convert_to_roman(95), Eq("XCV"));
  EXPECT_THAT(convert_to_roman(100), Eq("C"));
  EXPECT_THAT(convert_to_roman(122), Eq("CXXII"));
  EXPECT_THAT(convert_to_roman(152), Eq("CLII"));
  EXPECT_THAT(convert_to_roman(196), Eq("CXCVI"));
  EXPECT_THAT(convert_to_roman(247), Eq("CCXLVII"));
  EXPECT_THAT(convert_to_roman(288), Eq("CCLXXXVIII"));
  EXPECT_THAT(convert_to_roman(298), Eq("CCXCVIII"));
  EXPECT_THAT(convert_to_roman(500), Eq("D"));
  EXPECT_THAT(convert_to_roman(1000), Eq("M"));
  EXPECT_THAT(convert_to_roman(1513), Eq("MDXIII"));
  EXPECT_THAT(convert_to_roman(2999), Eq("MMCMXCIX"));
  EXPECT_THAT(convert_to_roman(3447), Eq("MMMCDXLVII"));
}

// ={=========================================================================
// cxx-bit cxx-numeric-limit
//
// The addition operation in the CPU is agnostic to whether the integer is
// signed or unsigned. The bit representation is the same. 
//
// Here `negate` means that -value but not ~value which is bitwise operation.
// 
// If you `negate` 0x80000000, _MIN, you get the same again and that is
// something to look out for because there is `no-change` in bit representation.
// This means abs() has no effect when fed the largest negative number. So bit
// representation is 'agnostic' to whether it's signed or unsigned.

TEST(Bit, MaxNegagiveIsSpecial)
{
  // get max negative, ???_MIN
  int int_min = (~((unsigned int)0) >> 1)+1;

  bitset<32> bitset_int_min(int_min);
  EXPECT_EQ(bitset_int_min.to_string(), "10000000000000000000000000000000");

  // what'd happen when negate ???_MIN?
  int negate_min = -int_min;
  bitset<32> bitset_negate_min(negate_min);
  EXPECT_EQ(bitset_negate_min.to_string(), "10000000000000000000000000000000");
}

TEST(Bit, GetLimts)
{
  // fails
  // unsigned int int_max = (~((int)0)) >> 1;
  // int int_max = (~((int)0)) >> 1;
  
  // okays
  // int int_max = (~((unsigned int)0)) >> 1;
  // unsigned int int_max = (~((unsigned int)0)) >> 1;
  
  unsigned int uint_max = ~((unsigned int)0);
  int int_max = uint_max >> 1;
  int int_min = int_max + 1;
  
  // bitset<32> bitsetx{int_max};
  // cout << bitsetx << endl;

  EXPECT_EQ(uint_max, numeric_limits<unsigned int>::max());
  EXPECT_EQ(int_max, numeric_limits<int>::max());
  EXPECT_EQ(int_min, numeric_limits<int>::min());
}

// ={=========================================================================

class Generator
{
  public:
    int operator() () { return rand() % 50; }
};

using PortfolioIterator = vector<unsigned int>::iterator;

PortfolioIterator RearrangeByQuantity(PortfolioIterator begin,
    PortfolioIterator end, unsigned int max_quanity)
{
  // how to get T of coll such as algo-remove? here, assumes that we know T
  vector<unsigned int> coll;

  PortfolioIterator start = begin;
  PortfolioIterator current{};

  for (; start != end; ++start)
  {
    // not use push_back() since void push_back()
    if (*start <= max_quanity)
      current = coll.insert(coll.end(), *start);
  }

  start = begin;

  for (; start != end; ++start)
  {
    if (*start > max_quanity)
      coll.push_back(*start);
  }

  copy(coll.begin(), coll.end(), ostream_iterator<unsigned int>(cout, ","));
  cout << endl;

  return ++current;
}

// Q: If run it standalone, it runs slower than on in GTEST. WHY?
//
// #include <iostream>
// #include <chrono>
// #include <thread>
// 
// using namespace std;
// 
// typedef bool (*UPDATEFUNC)(int);
// 
// bool UpdateProgress(int percent)
// {
//   cout << flush << "\r" << percent << " % complete...";
//   // cout << "\r" << percent << "% complete...";
//   return true;
// }
// 
// int main()
// {
//   UPDATEFUNC f = UpdateProgress;
// 
//   for (long l = 0; l < 100000000; ++l)
//   {
//     if (l % 1000000 == 0)
//       f(l / 1000000);
// 
//     for (long x = 0; x < 100; ++x)
//       x = x; 
// 
//     // this_thread::sleep_for(std::chrono::milliseconds{1});
//   }
// 
//   cout << endl;
// 
//   return EXIT_SUCCESS;
// }


typedef bool (*UPDATEFUNC)(int);

bool UpdateProgress(int percent)
{
  cout << flush << "\r" << percent << "% complete...";
  return true;
}

TEST(Progress, X)
{
  UPDATEFUNC f = UpdateProgress;

  for (long l = 0; l < 100000000; ++l)
  {
    if (l % 1000000 == 0)
      f(l / 1000000);

    for (long x = 0; l < 1000000; ++l)
      x = x; 

    // this_thread::sleep_for(std::chrono::milliseconds{10});
  }

  cout << endl;
}


// ={=========================================================================

int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
