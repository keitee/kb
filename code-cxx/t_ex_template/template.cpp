#include "gmock/gmock.h"

#include <iostream>

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;


// ={=========================================================================

/*
template
*/

// 2018.10.15

class ipv4
{
  friend ostream &operator<<(ostream &os, ipv4 ip);
  // friend istream &operator>>(istream &is, const ipv4 ip);
  friend istream &operator>>(istream &is, ipv4 &ip);

  public:
  ipv4(const int value1=0, const int value2=0, 
      const int value3=0, const int value4=0)
    : value1_(value1), value2_(value2), value3_(value3), value4_(value4)
  {
    cout << "ipv4:ctor" << endl;
  }

  ipv4(const ipv4 &rhs)
    : value1_(rhs.value1_), value2_(rhs.value2_), value3_(rhs.value3_), value4_(rhs.value4_)
  {
    cout << "ipv4:copy ctor" << endl;
  }

  private:
  int value1_, value2_, value3_, value4_;
};


ostream &operator<<(ostream &os, const ipv4 ip)
{
  // convert them into string
  stringstream address{};
  address << "ip address is: " << 
    ip.value1_ << "." << ip.value2_ << "." << ip.value3_ << "." << ip.value4_;
  cout << address.str();
  return os;
}

// this causes infinite loop on cin and eventually seg fault because
// ip's ctor is called on and on.
//
// 15.3.2 Input Operator >>
//
// Note that the second argument is modified. To make this possible, the second
// argument is passed by nonconstant reference.
// 
// seg fault
// istream &operator>>(istream &is, const ipv4 ip);
//
// okay. see that tempory is made to maintain constness but don't know why it
// causes a infinite ctor.
//
// no seg fault but result is wrong
// istream &operator>>(istream &is, ipv4 ip);
//
// result is right
// istream &operator>>(istream &is, ipv4 &ip);

istream &operator>>(istream &is, ipv4 &ip)
{
  char delim{};
  is >> ip.value1_>> delim 
    >> ip.value2_ >> delim 
    >> ip.value3_ >> delim
    >> ip.value4_;
  return is;
}


TEST(IPV4, 20181015)
{
  ipv4 address(168, 192, 0, 1);   
  std::cout << address << std::endl;   

  ipv4 ip;   
  std::cout << ip << std::endl;   

  stringstream ss{"10.10.10.10"};
  ss >> ip;   

  EXPECT_THAT(ss.fail(), false);
  std::cout << ip << std::endl;
}


// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
