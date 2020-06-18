#include <cassert>
#include <experimental/string_view>
#include <iostream>
#include <string>

#if 0

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
size s2 30

do not allocate mem again for s2.

#endif

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
  std::cout << "size s2 " << s1.size() << std::endl;
}
