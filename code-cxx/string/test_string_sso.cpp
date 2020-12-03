#include <cassert>
#include <iostream>
#include <string>

/*

SSO (short string optimisation)

The `cxx-new-operator`, use `cxx-operator-new` (or operator new[]) to allocate
raw memory, runs ctor to construct, and returns pointer to constructed memory.
Raise 'bad_alloc' exception when there is no memory.

if defines it at global scope, overload it for all operator new anf if at class
scode, overload it only for that class.


$ ./test_string_sso

create s1
allocate 31 bytes
size s1 30

create s2
size s2 14

do not allocate mem for s2 since it's less than implementation limit.

https://www.modernescpp.com/index.php/c-17-avoid-copying-with-std-string-view

the strings stores its data on the heap. But that is only true if the string
exceeds an implementation-dependent size. This size for std::string is 15 for
MSVC and GCC and 23 for Clang.

That means on the contrary, small strings are stored directly in the string
object. Therefore, no memory allocation is required.

From now on, my strings will always have at least 30 characters. So, I have not
to reason about small string optimisation.

*/

void *operator new(std::size_t count)
{
  std::cout << "allocate " << count << " bytes\n";
  return malloc(count);
}

int main()
{
  std::cout << "create s1" << std::endl;
  std::string s1{"this is a pretty long sentence"};
  std::cout << "size s1 " << s1.size() << std::endl;

  std::cout << "create s2" << std::endl;
  std::string s2{"short sentence"};
  std::cout << "size s2 " << s2.size() << std::endl;
}
