#include <boost/lexical_cast.hpp>
#include <chrono>
#include <cstdarg>
#include <forward_list>
#include <fstream>
#include <future>
#include <iostream>
#include <limits>
#include <list>
#include <memory>
#include <random>
#include <regex>
#include <set>
#include <sstream>
#include <thread>
#include <vector>

#include "gmock/gmock.h"

using namespace std;
using namespace std::placeholders;
using namespace testing;

/*
// ={=========================================================================

https://gcc.gnu.org/onlinedocs/gcc/Variable-Attributes.html

6.34 Specifying Attributes of Variables

The keyword __attribute__ allows you to specify special properties of variables,
function parameters, or structure, union, and, in C++, class members. This
__attribute__ keyword is followed by an attribute specification enclosed in
double parentheses. Some attributes are currently defined generically for
variables. Other attributes are defined for variables on particular target
systems.


6.34.1 Common Variable Attributes

cleanup (cleanup_function)

The cleanup attribute runs a function when the variable goes out of scope. This
attribute can only be applied to auto function scope variables; it may not be
applied to parameters or variables with static storage duration. The function
must take one parameter, a pointer to a type compatible with the variable. The
return value of the function (if any) is ignored.

If -fexceptions is enabled, then cleanup_function is run during the stack
unwinding that happens during the processing of the exception. Note that the
cleanup attribute does not allow the exception to be caught, only to perform an
action. It is undefined what happens if cleanup_function does not return
normally.

note: clang also supports 

*/

namespace attribute
{
  // https://echorand.me/site/notes/articles/c_cleanup/cleanup_attribute_c.html
  //
  // I first came across this attribute while reading Understanding and Using C
  // Pointers (Link to my review) where the author discusses RAII. Take a look
  // at how the RAII_VARIABLE macro is defined using the __cleanup__ attribute
  // (In case you didn’t know, ## is concatenation in C macros).

  void clean_up(int *final_value)
  {
    printf("Cleaning up\n");
    printf("Final value: %d\n", *final_value);
  }
} // namespace attribute

// [ RUN      ] GccAttribute.check_variable_attribute
// Test ends
// Cleaning up
// Final value: 5
// [       OK ] GccAttribute.check_variable_attribute (0 ms)

TEST(GccAttribute, check_variable_attribute)
{
  using namespace attribute;

  //  declare cleanup attribute along with initiliazation Without the cleanup
  //  attribute, this is equivalent to:
  //  int avar = 1;

  int avar __attribute__((__cleanup__(clean_up))) = 1;
  avar                                            = 5;
  std::cout << "Test ends\n";
}

// ={=========================================================================

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
