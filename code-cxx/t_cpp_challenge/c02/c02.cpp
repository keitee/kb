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

class ipv4
{
  friend ostream &operator<<(ostream &os, ipv4 ip);
  // friend ostream &operator>>(ostream &is, ipv4 ip);
  friend istream &operator>>(istream &is, ipv4 ip);

  public:
  ipv4(const int value1=0, const int value2=0, 
      const int value3=0, const int value4=0)
    : value1_(value1), value2_(value2), value3_(value3), value4_(value4)
  {
    cout << "ipv4:ctor" << endl;
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
// 1. have tried to use only one input. not?
// istream &operator>>(istream &is, const ipv4 ip)

// istream &operator>>(istream &is, ipv4 ip)
istream &operator>>(istream &is, const ipv4 ip)
{
  cin >> ip.value1_;
  // cin >> ip.value1_ >> ip.value2_ >> ip.value3_ >> ip.value4_;
  return is;
}


TEST(LanguageFeatures, IPV4)
{
  ipv4 address(168, 192, 0, 1);   
  std::cout << address << std::endl;   

  ipv4 ip;   
  std::cout << ip << std::endl;   
  std::cin >> ip;   

  if(!std::cin.fail())      
    std::cout << ip << std::endl;
}


// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
