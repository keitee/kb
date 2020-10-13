#if 0

NOTE: from cxx-17

#include <iostream>
#include <string>

./test_string_view
31 bytes allocates
true
30 bytes allocates
false

void* operator new(std::size_t count) {
  std::cout << count << " bytes allocates " << std::endl;
  return malloc(count);
}

// return true if string has "very"
bool contains_very(const std::string& str) {
  return str.find("very") != std::string::npos;
}

int main() {
  std::cout << std::boolalpha << contains_very("c++ string is very easy to use")
            << std::endl;

  std::cout << contains_very("c++ string is not easy to use") << std::endl;
}


This approach uses temporary std::string when use contains_very with literal.
string_view can do better since it only "read" and do not own string. so no
allocation happens when create string_view with std::string or literals.

if underlying string that string_view refers to is gone but string_view tries to
access then it is undefined.


./test_string_view
true
false
37 bytes allocates
--------------------
false

#include <cassert>
#include <experimental/string_view>
#include <iostream>

void* operator new(std::size_t count) {
  std::cout << count << " bytes allocates " << std::endl;
  return malloc(count);
}

// return true if string has "very"
bool contains_very(std::experimental::string_view str)
{
  return str.find("very") != std::experimental::string_view::npos;
}

int main() {
  // no allocation when use string_view
  std::cout << std::boolalpha << contains_very("c++ string is very easy to use")
            << std::endl;

  // no allocation when use string_view
  std::cout << contains_very("c++ string is not easy to use") << std::endl;

  // allocation when use string
  std::string str = "some long long long long long string";
  std::cout << "--------------------" << std::endl;
  std::cout << contains_very(str) << std::endl;
}


Also, string_view do not allocate when use substr.

./test_string_view
string -----
30 bytes allocates
--------------------
21 bytes allocates
sometimes string is

string_view -----
--------------------
sometimes string is

#endif

#include <cassert>
#include <experimental/string_view>
#include <iostream>

void *operator new(std::size_t count)
{
  std::cout << count << " bytes allocates " << std::endl;
  return malloc(count);
}

int main()
{
  std::cout << "string -----" << std::endl;
  std::string s = "sometimes string is very slow";
  std::cout << "--------------------" << std::endl;
  std::cout << s.substr(0, 20) << std::endl << std::endl;

  std::cout << "string_view -----" << std::endl;
  std::experimental::string_view sv = s;
  std::cout << "--------------------" << std::endl;
  std::cout << sv.substr(0, 20) << std::endl;
}
