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
ES.71: Prefer a range-for-statement to a for-statement when there is a choice

Enforcement

Look at loops, if a traditional loop just looks at each element of a sequence,
and there are no side effects on what it does with the elements, rewrite the
loop to a ranged-for loop.

https://docs.microsoft.com/en-us/cpp/code-quality/c26817?view=vs-2019

*/

/*
// ={=========================================================================

https://docs.microsoft.com/en-us/cpp/code-quality/c26818?view=vs-2019

C26818

04/22/2020
 
Switch statement does not cover all cases. Consider adding a 'default' label (es.79).

Remarks

This check covers the missing default label in switch statements that switch
over a non-enumeration type, such as int, char, and so on.

For more information, see ES.79: Use default to handle common cases (only) in
the C++ Core Guidelines.

https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#es79-use-default-to-handle-common-cases-only

Example

This example has a missing default label when switching over an int.

void fn1();
void fn2();

void foo(int a)
{
    switch (a)
    {
        case 0:
            fn1();
            break;
        case 2:
            fn2();
            break;
    }
}

Solution

To fix this issue, insert a default label to cover all remaining cases.

void fn1();
void fn2();
void default_action();

void foo(int a)
{
    switch (a)
    {
        case 0:
            fn1();
            break;
        case 2:
            fn2();
            break;
        default:
            default_action();
            break;
    }
}

If no default action needs to be taken, insert an empty default label to
indicate that the other cases haven't been forgotten.

void fn1();
void fn2();

void foo(int a)
{
    switch (a)
    {
        case 0:
            fn1();
            break;
        case 2:
            fn2();
            break;
        default:
            // do nothing for the rest of the cases
            break;
    }
}

ES: Expressions and statements

ES.79: Use default to handle common cases (only)

Reason
Code clarity. Improved opportunities for error detection.


https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html

-Wswitch
Warn whenever a switch statement has an index of enumerated type and lacks a
case for one or more of the named codes of that enumeration. (The presence of a
    default label prevents this warning.) 

case labels outside the enumeration range also provoke warnings when this option
is used (even if there is a default label). This warning is enabled by -Wall.


-Wswitch-default
Warn whenever a switch statement does not have a default case.


-Wswitch-enum
Warn whenever a switch statement has an index of enumerated type and lacks a
case for one or more of the named codes of that enumeration. case labels outside
the enumeration range also provoke warnings when this option is used. 

The only difference between -Wswitch and this option is that this option gives a
warning about an omitted enumeration code even if there is a default label.

target_compile_options(test_core PUBLIC -g -Wswitch-default)

test_core_guideline.cpp: In function ‘void core_es_79::foo(int)’:
test_core_guideline.cpp:129:12: warning: switch missing default case [-Wswitch-default]
     switch (a)
            ^
*/

namespace core_es_79
{
  void fn1() { EXPECT_THAT(true, true); }

  void fn2() { EXPECT_NE(true, false); }

  void foo(int a)
  {
    switch (a)
    {
      case 0:
        fn1();
        break;
      case 2:
        fn2();
        break;
    }
  }
} // namespace core_es_79

TEST(CxxCoreGuideline, es_79_switch_and_enum)
{
  using namespace core_es_79;

  foo(0);
}

/*
// ={=========================================================================
ES.78: Don't rely on implicit fallthrough in switch statements


test_core_guideline.cpp: In function ‘void core_es_80::foo(int)’:
test_core_guideline.cpp:190:12: warning: this statement may fall through [-Wimplicit-fallthrough=]
         fn1();
         ~~~^~
test_core_guideline.cpp:192:7: note: here
       case 2:
       ^~~~

https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html

-Wimplicit-fallthrough

-Wimplicit-fallthrough is the same as -Wimplicit-fallthrough=3 and
-Wno-implicit-fallthrough is the same as -Wimplicit-fallthrough=0.

The -Wimplicit-fallthrough=3 warning is enabled by -Wextra.

1. C++17 provides a standard way to suppress the -Wimplicit-fallthrough warning using [[fallthrough]];

2. The whole body of the C or C++ style comment should match the given regular
expressions listed below. The option argument n specifies what kind of comments
are accepted:

*/

namespace core_es_78
{
  void fn1() { EXPECT_THAT(true, true); }

  void fn2() { EXPECT_NE(true, false); }

  void foo(int a)
  {
    switch (a)
    {
      case 0:
        fn1();
        // break;
        // this removes warning.
        // FALLTHRU
      case 2:
        fn2();
        break;
      default:
        break;
    }
  }
} // namespace core_es_78

TEST(CxxCoreGuideline, es_78_switch_fallthrough)
{
  using namespace core_es_78;

  foo(0);
}

// ={=========================================================================

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
