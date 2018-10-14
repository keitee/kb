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

TEST(CH3, IPV4)
{
  const auto inputs{ 
    make_pair(3, 12), 
    make_pair(2, 34), 
    make_pair(13, 245)
  };

  unsigned int r1{}, r2{};

  for (const auto e : inputs)
  {
    r1 = gcd_recursive(e.first, e.second);
    r2 = gcd_iteratitive(e.first, e.second);

    // cout << "r1: " << r1 << ", r2: " << r2 << endl;

    EXPECT_EQ(r1, r2);
  }
}


// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
