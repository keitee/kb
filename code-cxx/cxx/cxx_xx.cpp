//
// https://www.geeksforgeeks.org/compile-32-bit-program-64-bit-gcc-c-c/
//
// to build it as 32 bits on 64 bit gcc
//
// g++ -m32 -Wall ../cxx_xx.cpp
// g++ -Wall ../cxx_xx.cpp

#include <iostream>

using namespace std;

// ={=========================================================================

int main(int argc, char **argv)
{
#if defined(__LP64__) || defined(_LP64)
  std::cout << "LP64" << std::endl;
#else
  std::cout << "LP32" << std::endl;
#endif

#if __GNUC__ && __x86_64__
  std::cout << "__x86_64__ " << std::endl;
#else
  std::cout << "__x86_32__ " << std::endl;
#endif

  std::cout << "size of (int) is                : " << sizeof(int) << std::endl;
  std::cout << "size of (unsigned int) is       : " << sizeof(unsigned int)
            << std::endl;

  std::cout << "size of (long) is               : " << sizeof(long)
            << std::endl;
  std::cout << "size of (unsigned long) is      : " << sizeof(unsigned long)
            << std::endl;

  std::cout << "size of (long int) is           : " << sizeof(long int) << endl;
  std::cout << "size of (unsigned long int) is  : " << sizeof(unsigned long int)
            << endl;

  std::cout << "size of (long long) is          : " << sizeof(long long)
            << endl;
  cout << "size of (unsigned long long) is  : " << sizeof(unsigned long long)
       << endl;

  int *pint;
  unsigned int *puint;

  std::cout << "size of (* int) is              : " << sizeof pint << endl;
  std::cout << "size of (* unsigned int) is     : " << sizeof puint << endl;

  std::cout << "size of (size_t) is             : " << sizeof(size_t) << endl;
  std::cout << "size of (uint32_t) is           : " << sizeof(uint32_t) << endl;
  std::cout << "size of (uint64_t) is           : " << sizeof(uint64_t) << endl;

  uint64_t value{10};

#if !defined(__LP64__) || !defined(_LP64)
  // 32 bits
  // warning: format ‘%ld’ expects argument of type ‘long int’, but argument 2 has type ‘uint64_t {aka long long unsigned int}’ [-Wformat=]
  // printf("uint64 value is %ld\n", value);
  printf("uint64 value is %lld\n", value);
#else
  // 64 bits
  printf("uint64 value is %ld\n", value);
  // warning: format ‘%lld’ expects argument of type ‘long long int’, but argument 2 has type ‘uint64_t {aka long unsigned int}’ [-Wformat=]
  // printf("uint64 value is %lld\n", value);
#endif
}
