#include "gmock/gmock.h"

#include <iostream>

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;


// ={=========================================================================

/*

2. Greatest common divisor

The greatest common divisor (gcd in short) of two or more non-zero integers,
also known as the greatest common factor (gcf), highest common factor (hcf),
greatest common measure (gcm), or highest common divisor, is the greatest
positive integer that divides all of them. There are several ways the gcd could
be computed; an efficient method is Euclid's algorithm. For two integers, the
algorithm is:

gcd(a,0) = a
gcd(a,b) = gcd(b, a mod b)

*/

unsigned int gcd_recursive(unsigned int const a, unsigned int const b)
{
  return (b == 0) ? a : gcd_recursive(b, a % b);
}

unsigned int gcd_iteratitive(unsigned int a, unsigned int b)
{
  while (b != 0)
  {
    unsigned int r = a % b;
    a = b;
    b = r;
  }
  return a;
}

TEST(CH3,GreatestCommonDivisor)
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


/*

3. The least common multiple (lcm) of two or more non-zero integers, also known
as the lowest common multiple, or smallest common multiple, is the smallest
positive integer that is divisible by all of them.

*/


// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
