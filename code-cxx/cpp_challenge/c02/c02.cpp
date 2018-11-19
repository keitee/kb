#include "gmock/gmock.h"

#include <iostream>

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;


// ={=========================================================================

/*

15. IPv4 data type                                                        

Write a class that represents an IPv4 address. Implement the functions required
to be able to read and write such addresses from or to the console. The user
should be able to input values in dotted form, such
as 127.0.0.1 or 168.192.0.100. This is also the form in which IPv4 addresses
should be formatted to an output stream.

*/

namespace ipv4_2018_10 {

// 2018.10.15

// usage:
//
// ipv4 address(168, 192, 0, 1);   
// std::cout << address << std::endl;   
// cin >> ip;   

class ipv4
{
  friend ostream &operator<<(ostream &os, ipv4 ip);

  // friend istream &operator>>(istream &is, const ipv4 ip);
  // friend istream &operator>>(istream &is, ipv4 ip);

  friend istream &operator>>(istream &is, ipv4 &ip);

  public:
  ipv4(const int value1=0, const int value2=0, 
      const int value3=0, const int value4=0)
    : value1_(value1), value2_(value2), value3_(value3), value4_(value4)
  {}

  ipv4(const ipv4 &rhs)
    : value1_(rhs.value1_), value2_(rhs.value2_), value3_(rhs.value3_), value4_(rhs.value4_)
  {}

  string return_address()
  {
    return to_string(value1_) + "." + to_string(value2_) + "." 
      + to_string(value3_) + "." + to_string(value4_);
  }

  private:
  int value1_, value2_, value3_, value4_;
};


ostream &operator<<(ostream &os, const ipv4 ip)
{
  os << ip.value1_ << "." << ip.value2_ << "." << ip.value3_ << "." << ip.value4_;
  return os;
}

// this causes infinite loop on cin and eventually seg fault because
// ip's ctor is called on and on.
//
// 15.3.2 Input Operator >>
//
// Note that the second argument is modified. To make this possible, the second
// argument is passed by non-constant reference.
// 
// no seg fault but result is wrong since uses a copy
// istream &operator>>(istream &is, ipv4 ip);
//
// result is right
// istream &operator>>(istream &is, ipv4 &ip);
//
// seg fault when use:
//
// istream &operator>>(istream &is, const ipv4 ip);
//
// see that tempory is made to maintain constness but don't know why it
// causes a infinite ctor.
// 
// run gdb
//
// os >> address;
//
// causes call operator>>() which calls ipv4 ctor since it needs const copy.
//
// operator>>() calls
//
// is >> ip.value1_>> delim;
//
// this calls operator>>() which calls ipv4 ctor since it needs const copy.
//
// operator>>() calls
//
// is >> ip.value1_>> delim;
//
// ... cxx-recursive
// 
// even if operator>>() is called with const copy, it needs another const copy
// when execute "is >> ip.value1_>> delim;"
//
// when use istream &operator>>(istream &is, ipv4 ip)
//
// is >> ip.value1_>> delim;
//
// do not cause to call operator>>() again.
//
// so the key is when operator>>() is called ip is not writable so create one
// and this cause recursive. 


// istream &operator>>(istream &is, const ipv4 ip)
// istream &operator>>(istream &is, ipv4 ip)

istream &operator>>(istream &is, ipv4 &ip)
{
  char delim{};
  is >> ip.value1_>> delim 
    >> ip.value2_ >> delim 
    >> ip.value3_ >> delim
    >> ip.value4_;
  return is;
}

} // namespace

// (gdb) 
// b IPV4_20181015_Test::TestBody()
// b ipv4_2018_10::ipv4::ipv4(int,int,int,int)

TEST(IPV4_U15, 20181015)
{
  using namespace ipv4_2018_10;

  {
    stringstream os;
    ipv4 address(168, 192, 0, 1);   
    os << address;
    EXPECT_THAT(os.fail(), false);
    EXPECT_EQ(os.str(), "168.192.0.1");
  }

  {
    stringstream os;
    ipv4 address;   
    os << address;
    EXPECT_EQ(os.str(), "0.0.0.0");
  }

  {
    stringstream os{"10.20.30.40"};
    ipv4 address;   
    os >> address;
    EXPECT_THAT(os.fail(), false);
    EXPECT_EQ(address.return_address(), "10.20.30.40");
  }
}


namespace ip4_2018_11_14 
{
  // usage:
  //
  // ipv4 address(168, 192, 0, 1);   
  // std::cout << address << std::endl;   
  // cin >> ip;   

  class ipv4
  {
    friend istream &operator>>(istream &is, ipv4 &ip);
    friend ostream &operator<<(ostream &os, const ipv4 &ip);

    public:
      explicit ipv4(int value1 = 0, int value2 = 0, int value3 = 0, int value4 = 0):
        value1_(value1), value2_(value2), value3_(value3), value4_(value4) {}

      string return_address()
      {
        return to_string(value1_) + "." + to_string(value2_) + "." 
          + to_string(value3_) + "." + to_string(value4_);
      }

    private:
      int value1_{}, value2_{}, value3_{}, value4_{};
  };

  // input and output ops
  istream &operator>>(istream &is, ipv4 &ip)
  {
    char delim{};
    is >> ip.value1_ >> delim 
      >> ip.value2_ >> delim 
      >> ip.value3_ >> delim 
      >> ip.value4_;

    return is;
  }

  ostream &operator<<(ostream &os, const ipv4 &ip)
  {
    os << ip.value1_ << "." 
      << ip.value2_ << "." 
      << ip.value3_ << "." 
      << ip.value4_;
    return os;
  }

} // namespace

TEST(IPV4_U15, 20181114)
{
  using namespace ip4_2018_11_14;

  {
    stringstream os;
    ipv4 address(168, 192, 0, 1);   
    os << address;
    EXPECT_THAT(os.fail(), false);
    EXPECT_EQ(os.str(), "168.192.0.1");
  }

  {
    stringstream os;
    ipv4 address;   
    os << address;
    EXPECT_EQ(os.str(), "0.0.0.0");
  }

  {
    stringstream os{"10.20.30.40"};
    ipv4 address;   
    os >> address;
    EXPECT_THAT(os.fail(), false);
    EXPECT_EQ(address.return_address(), "10.20.30.40");
  }
}


// textbook
namespace u15_book
{

class ipv4
{
  public:

    // ctors

    constexpr ipv4() : data_{{0}} {}
    constexpr ipv4(unsigned const char a, unsigned const char b,
        unsigned const char c, unsigned const char d):
      data_{{a,b,c,d}} {}

    constexpr ipv4(unsigned long a):
      data_{{static_cast<unsigned char>((a >> 24) & 0xFF),
        static_cast<unsigned char>((a >> 16) & 0xFF),
        static_cast<unsigned char>((a >> 8) & 0xFF),
        static_cast<unsigned char>((a & 0xFF))}} {}

    // copy controls

    ipv4(const ipv4 &rhs):
      data_(rhs.data_) {}

    ipv4 &operator=(const ipv4 &rhs)
    {
      data_ = rhs.data_;
      return *this;
    }

    // friends

    friend std::ostream &operator<<(std::ostream &os, const ipv4 &a)
    {
      os << static_cast<int>(a.data_[0]) << '.'
        << static_cast<int>(a.data_[1]) << '.'
        << static_cast<int>(a.data_[2]) << '.'
        << static_cast<int>(a.data_[3]);
      return os;
    }

    friend std::istream &operator>>(std::istream &is, ipv4 &a)
    {
      char d1, d2, d3;
      int b1, b2, b3, b4;

      is >> b1 >> d1 >> b2 >> d2 >> b3 >> d3 >> b4;

      if (d1 == '.' && d2 == '.' && d3 == '.')
        a = ipv4(b1, b2, b3, b4);
      else
        is.setstate(std::ios_base::failbit);

      return is;
    }

  private:
    std::array<unsigned char, 4> data_;
};

} // namespace


// TEST(IPV4, Text)
// {
//   // using namespace textbook;
// 
//   textbook::ipv4 address(168, 192, 0, 1);   
//   std::cout << address << std::endl;   
// 
//   textbook::ipv4 ip;   
//   std::cout << ip << std::endl;   
//   std::cin >> ip;   
// 
//   if(!std::cin.fail())      
//     std::cout << ip << std::endl;
// }


// ={=========================================================================

/*

16. Enumerating IPv4 addresses in a range

Write a program that allows the user to input two IPv4 addresses representing a
range and list all the addresses in that range. Extend the structure defined for
the previous problem to implement the requested functionality.

{
   std::cout << "input range: ";
   ipv4 a1, a2;
   std::cin >> a1 >> a2;

   if (a2 > a1)
   {
      for (ipv4 a = a1; a <= a2; a++)
      {
         std::cout << a << std::endl;
      }
   }
   else 
   {
      std::cerr << "invalid range!" << std::endl;
   }
}

To be able to enumerate IPv4 addresses in a given range, it should first be
possible to compare IPv4 values. Therefore, we should implement at least
operator<, but the following listing contains implementation for all comparison
operators: ==, !=, <, >, <=, and >=. Also, in order to increment an IPv4 value,
implementations for both the prefix and postfix operator++ are provided. The
following code is an extension of the IPv4 class from the previous problem:

*/

// unit16
namespace u16_2018_11_19
{
  // usage:
  //
  // ipv4 address(168, 192, 0, 1);   
  // std::cout << address << std::endl;   
  // cin >> ip;   

  class ipv4
  {
    friend istream &operator>>(istream &is, ipv4 &ip);
    friend ostream &operator<<(ostream &os, const ipv4 &ip);

    public:
    explicit ipv4(int value1 = 0, int value2 = 0, int value3 = 0, int value4 = 0):
      value1_(value1), value2_(value2), value3_(value3), value4_(value4) {}

    string return_address()
    {
      return to_string(value1_) + "." + to_string(value2_) + "." 
        + to_string(value3_) + "." + to_string(value4_);
    }

    vector<int> return_list() const
    {
      return vector<int>{value1_, value2_, value3_, value4_};
    }

    // pre and post.
    //
    // Q: How about when value_4 > 255?

    ipv4 &operator++()
    {
      ++value4_;
      return *this;
    }

    // post
    const ipv4 operator++(int)
    {
      const ipv4 old = *this;
      ++(*this);
      return old;
    }

    private:
    int value1_{}, value2_{}, value3_{}, value4_{};
  };

  // input and output ops
  istream &operator>>(istream &is, ipv4 &ip)
  {
    char delim{};
    is >> ip.value1_ >> delim 
      >> ip.value2_ >> delim 
      >> ip.value3_ >> delim 
      >> ip.value4_;

    return is;
  }

  ostream &operator<<(ostream &os, const ipv4 &ip)
  {
    os << ip.value1_ << "." 
      << ip.value2_ << "." 
      << ip.value3_ << "." 
      << ip.value4_;
    return os;
  }

  // o do not need to be friend
  //
  // o could use if or tenary op to make comparison. better way? if ipv4 supports
  // iterator then would be simpler. so have a function to return vector which
  // supports access or iterator

  //  a < b excludes a == b and a > b. 
  //
  //  Also 
  //
  //  ip(value1, value2, value3, value4), 
  //  ip(value1, value2, value3, value4)
  //
  //  ip == ip means equal but not value1 == value1

  bool operator<(const ipv4 &a, const ipv4 &b)
  {
    auto list_a = a.return_list();
    auto list_b = b.return_list();

    // assumes that ipv4 has always four field set
    for (auto ai = list_a.begin(), bi = list_b.begin();
        ai != list_a.end(); ++ai, ++bi)
    {
      if (*ai > *bi)
        return false;
    }

    return (list_a == list_b) ? false : true;
  }

  // cannot use like this since operator<() only supports '<' case and ! makes
  // it supports '==' and '>' which is different to support '>' only.
  //
  // bool operator>(const ipv4 &a, const ipv4 &b)
  // {
  //   return !operator<(a, b);
  // }

  bool operator>(const ipv4 &a, const ipv4 &b)
  {
    auto list_a = a.return_list();
    auto list_b = b.return_list();

    // assumes that ipv4 has always four field set
    for (auto ai = list_a.begin(), bi = list_b.begin();
        ai != list_a.end(); ++ai, ++bi)
    {
      if (*ai < *bi)
        return false;
    }

    return (list_a == list_b) ? false : true;
  }

  bool operator<=(const ipv4 &a, const ipv4 &b)
  {
    auto list_a = a.return_list();
    auto list_b = b.return_list();

    // assumes that ipv4 has always four field set
    for (auto ai = list_a.begin(), bi = list_b.begin();
        ai != list_a.end(); ++ai, ++bi)
    {
      if (*ai > *bi)
        return false;
    }

    return true;
  }

  // cannot use like this since
  //
  // bool operator>=(const ipv4 &a, const ipv4 &b)
  // {
  //   return !operator<=(a, b);
  // }

  bool operator>=(const ipv4 &a, const ipv4 &b)
  {
    auto list_a = a.return_list();
    auto list_b = b.return_list();

    // assumes that ipv4 has always four field set
    for (auto ai = list_a.begin(), bi = list_b.begin();
        ai != list_a.end(); ++ai, ++bi)
    {
      if (*ai < *bi)
        return false;
    }

    return true;
  }

} // namespace


TEST(IPV4_U16, 20181119)
{
  using namespace u16_2018_11_19;

  {
    ipv4 a(10,209,60,1), b(10,209,60,2);

    EXPECT_TRUE(a < b);
    EXPECT_TRUE(b > a);
  }

  {
    ipv4 a(10,209,60,1), b(10,209,60,1);

    EXPECT_FALSE(a < b);
    EXPECT_FALSE(a > b);

    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(a >= b);
  }

  {
    ipv4 a(10,209,60,1);
    a++;
    EXPECT_EQ(a.return_address(), "10.209.60.2");
    ++a;
    EXPECT_EQ(a.return_address(), "10.209.60.3");
  }
}


// unit16
// do not copy the whole source here and repeat the key idea/difference 

namespace u16_book
{

  // copied from namespace ip4_2018_11_14 
  // {
  // usage:
  //
  // ipv4 address(168, 192, 0, 1);   
  // std::cout << address << std::endl;   
  // cin >> ip;   

  class ipv4
  {
    friend istream &operator>>(istream &is, ipv4 &ip);
    friend ostream &operator<<(ostream &os, const ipv4 &ip);

    public:
    explicit ipv4(int value1 = 0, int value2 = 0, int value3 = 0, int value4 = 0):
      value1_(value1), value2_(value2), value3_(value3), value4_(value4) {}

    string return_address()
    {
      return to_string(value1_) + "." + to_string(value2_) + "." 
        + to_string(value3_) + "." + to_string(value4_);
    }

    //
    unsigned long to_long() const
    {
      unsigned long value{};

      // value = (unsigned long)(value1_ << 24) 
      //   | (unsigned long)(value2_ << 16) 
      //   | (unsigned long)(value3_ << 8) 
      //   | (unsigned long)value4_;

      value = static_cast<unsigned long>(value1_ << 24) 
        | static_cast<unsigned long>(value2_ << 16) 
        | static_cast<unsigned long>(value3_ << 8) 
        | static_cast<unsigned long>(value4_);

      // '|' means '+'
      // unsigned long value_sum{};
      // value_sum = (unsigned long)(value1_ << 24) 
      //   + (unsigned long)(value2_ << 16) 
      //   + (unsigned long)(value3_ << 8) 
      //   + (unsigned long)value4_;
      //
      // if (value != value_sum)
      //   cout << "** differ **" << endl;

      return value;
    }

    // pre. the text uses another ctor() instead
    ipv4 &operator++()
    {
      unsigned long value = 1 + (*this).to_long();

      value1_ = static_cast<int>(value >> 24) & 0xFF;
      value2_ = static_cast<int>(value >> 16) & 0xFF;
      value3_ = static_cast<int>(value >> 8) & 0xFF;
      value4_ = static_cast<int>(value) & 0xFF;

      return *this;
    }

    // post
    const ipv4 operator++(int)
    {
      ipv4 old = *this;
      ++(*this);
      return old;
    }

    private:
    int value1_{}, value2_{}, value3_{}, value4_{};
  };

  // input and output ops
  istream &operator>>(istream &is, ipv4 &ip)
  {
    char delim{};
    is >> ip.value1_ >> delim 
      >> ip.value2_ >> delim 
      >> ip.value3_ >> delim 
      >> ip.value4_;

    return is;
  }

  ostream &operator<<(ostream &os, const ipv4 &ip)
  {
    os << ip.value1_ << "." 
      << ip.value2_ << "." 
      << ip.value3_ << "." 
      << ip.value4_;
    return os;
  }
  
  // } // namespace


  // operator <, >
  bool operator<(const ipv4 &a, const ipv4 &b)
  {
    if (a.to_long() < b.to_long())
      return true;

    return false;
  }

  bool operator>(const ipv4 &a, const ipv4 &b)
  {
    return (a.to_long() > b.to_long()) ? true : false;
  }     

  bool operator<=(const ipv4 &a, const ipv4 &b)
  {
    return (a.to_long() <= b.to_long()) ? true : false;
  }     

  bool operator>=(const ipv4 &a, const ipv4 &b)
  {
    return (a.to_long() >= b.to_long()) ? true : false;
  }     
}

TEST(IPV4_U16, Text)
{
  using namespace u16_book;

  {
    ipv4 a(10,209,60,1), b(10,209,60,2);

    EXPECT_TRUE(a < b);
    EXPECT_TRUE(b > a);
  }

  {
    ipv4 a(10,209,60,1), b(10,209,60,1);

    EXPECT_FALSE(a < b);
    EXPECT_FALSE(a > b);

    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(a >= b);
  }

  {
    ipv4 a(10,209,60,1);
    a++;
    EXPECT_EQ(a.return_address(), "10.209.60.2");
    ++a;
    EXPECT_EQ(a.return_address(), "10.209.60.3");
  }
}


// ={=========================================================================

/*

17. Creating a 2D array with basic operations

Write a class template that represents a two-dimensional array container with
methods for element access (at() and data()), capacity querying, iterators,
filling, and swapping. It should be possible to move objects of this type.

{
   // element access
   array2d<int, 2, 3> a {1, 2, 3, 4, 5, 6};
   for (size_t i = 0; i < a.size(1); ++i)
      for (size_t j = 0; j < a.size(2); ++j)
      a(i, j) *= 2;

   // iterating
   std::copy(std::begin(a), std::end(a), 
      std::ostream_iterator<int>(std::cout, " "));
 
   // filling 
   array2d<int, 2, 3> b;
   b.fill(1);

   // swapping
   a.swap(b);

   // moving
   array2d<int, 2, 3> c(std::move(b));
}

*/

// unit17
namespace u17_2018_11_20
{
  template<typename Row, typename Col>
  calss array2d
} // namespace


// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
