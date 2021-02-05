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

namespace attribute_function
{
#ifdef __GNUC__
#define NORETURN __attribute__((__noreturn__))
#else
#define NORETURN
#endif

  NORETURN static void terminate_1(bool useExit3)
  {
    std::cout << "terminate is called\n";

    if (useExit3)
      exit(EXIT_FAILURE);
    else
      _exit(EXIT_FAILURE);
  }

#ifdef __GNUC__
  __attribute__((__noreturn__))
#endif
  static void
  terminate_2(bool useExit3)
  {
    std::cout << "terminate is called\n";

    if (useExit3)
      exit(EXIT_FAILURE);
    else
      _exit(EXIT_FAILURE);
  }

  int cause_warning_1()
  {
    std::cout << "it's to cuase warning\n";
    terminate_1(true);
  }

  int cause_warning_2()
  {
    std::cout << "it's NOT to cuase warning\n";
    terminate_2(true);
  }
} // namespace attribute_function

/*
// ={=========================================================================

when see:

#ifdef __GNUC__
    // This macro stops 'gcc -Wall' complaining that "control reaches
    // end of non-void function" if we use the following functions to
    // terminate main() or some other non-void function.

#define NORETURN __attribute__ ((__noreturn__))
#else
#define NORETURN
#endif

void usageErr(const char *format, ...) NORETURN ;

However, this is for declarations in a header since:

/test_gcc.cpp: At global scope:
/test_gcc.cpp:97:18: error: attributes are not allowed on a function-definition
 #define NORETURN __attribute__ ((__noreturn__))

ok, runs it to check:

[ 54%] Building CXX object CMakeFiles/test_gcc.dir/test_gcc.cpp.o
: In function ‘int attribute::cause_warning()’:
: warning: no return statement in function returning non-void [-Wreturn-type]
   }
   ^
[ 57%] Linking CXX executable test_gcc

So cause_warning_2() do not cause this warning since termiate_2 use "noreturn"
attribute. Unlike the case, no need to use this attribue in caller,
cause_warning()?

https://gcc.gnu.org/onlinedocs/gcc-3.2/gcc/Function-Attributes.html

Declaring Attributes of Functions

In GNU C, you declare certain things about functions called in your program
which help the compiler optimize function calls and check your code more
carefully.

noreturn

A few standard library functions, such as abort and exit, cannot return. GCC
knows this automatically. Some programs define their own functions that never
return. You can declare them noreturn to tell the compiler this fact. For
example,

          void fatal () __attribute__ ((noreturn));
          
          void
          fatal (...)
          {
            ... // Print error message. ...
            exit (1);
          }
          
The noreturn keyword tells the compiler to assume that fatal cannot return. It
can then optimize without regard to what would happen if fatal ever did return.

This makes slightly better code. More importantly, it helps avoid spurious
warnings of uninitialized variables.

*/

TEST(GccAttributeFunction, no_return)
{
  using namespace attribute_function;

  cause_warning_1();

  cause_warning_2();
}

// ={=========================================================================

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
