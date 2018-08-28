#include <iostream>

/*
1.6.2

How would you implement bit vectors using bitwise logical operations(such as
and, or an shift)?
*/ 

const unsigned int BITSPERWORD = 32;
const unsigned int SHIFT = 5;
const unsigned int MASK = 0x1F;
// #define MASK (~(~0 << SHIFT))

const int INPUT_MAX = 10000000;

// bit vector to represent a array of bits. Why +1? Since 0-31 bits falls to
// array[0] and 32-63 falls to array[1], and so on. SIZE is num of bits to
// represent and BITSPERWORD is num of bits of a word(int). So there should be
// one array at least for 0-31 bits. 

int a[1 + INPUT_MAX/BITSPERWORD];

// if use char unit:
//
// #define CHAR_BIT_LOG 3 /* assumes that CHAR_BIT == 8, so log_2(8) = 3 */
// #define MASK (~(~0 << CHAR_BIT_LOG))
// #define setbit(a, x) ((a)[(x) >> CHAR_BIT_LOG] |= 1 << ((x) & MASK))
// #define isset(a, x) ((a)[(x) >> CHAR_BIT_LOG] & (1 << ((x) & MASK)))
// 
// char arr[(MAXINT-1)/CHAR_BIT+1];

void set_bit(int pos)
{
  // MASK
  // MASK is 11111...1 for [31-0] bits. By &, make only [31-0] bits valid and
  // not others and effectively pos - 31 for values which are > 32. that is:
  // 32 -> 0
  // 33 -> 1
  // ..
  //
  // [pos >> SHIFT]
  // pos is int and right shift on int may cause problem? Not in [] since it
  // is unsigned.
  //
  // Here, ">> 5" menas to devide 2^5, 32 which is num of bits of a word. so
  // find array index that pos falls on and this matches up 1 << (pos & MASK)

  a[pos >> SHIFT] |= ( 1 << (pos & MASK));
}

void clear_bit(int pos)
{
  a[pos >> SHIFT] &= ~( 1 << (pos & MASK));
}

bool test_bit(int pos)
{
  return (a[pos >> SHIFT] & ( 1 << (pos & MASK))) ? true : false;
}

int main(void)
{
  int value; 
  int n{};

  // To compare C++ version, use the same cin.
  //
  // while (scanf("%d", &a[n]) != EOF)
  //   ++n;
  
  while (std::cin >> value)
  {
    set_bit(value);
    ++n;
  }

  // To verify:
  // for (int i = 0; i < INPUT_MAX; ++i)
  //   if (!test_bit(i))
  //     std::cout << "i " << i << " is not set" << std:: endl;

  // don't have output part to focus on sorting time rather than input/output

  // sizeof(a): 1,250,004
  std::cout << "sizeof(a): " << sizeof(a) << std::endl;
  std::cout << "1.6.1: n: " << n << " bit vector" << std::endl;

  return 0;
}
