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
  friend istream &operator>>(istream &is, ipv4 ip);
  // friend istream &operator>>(istream &is, ipv4 &ip);

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
istream &operator>>(istream &is, ipv4 ip)
{
  char delim{};
  is >> ip.value1_>> delim;
  // is >> ip.value1_>> delim 
  //   >> ip.value2_ >> delim 
  //   >> ip.value3_ >> delim
  //   >> ip.value4_;
  return is;
}

} // namespace

// (gdb) 
// b IPV4_20181015_Test::TestBody()
// b ipv4_2018_10::ipv4::ipv4(int,int,int,int)

TEST(IPV4, 20181015)
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

TEST(IP4, 20181114)
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
namespace ipv4_textbook
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


// TEST(IPV4, textbook)
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
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
