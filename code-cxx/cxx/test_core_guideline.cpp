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
P: Philosophy

P.9: Don't waste time or space
https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#Rp-waste

https://devblogs.microsoft.com/cppblog/new-safety-rules-in-c-core-check/?fbclid=IwAR2HJjPz8nTUnjEJyHHXyblk1m6b91tO60G863Z65OqB9nJ07AWL7IF_CrM#expensive-copy-with-the-auto-keyword

Expensive copy with the auto keyword

Here, the type of password resolves to std::string, even though the return type
of getPassword() is a const-reference to a string. The resulting behavior is
that the contents of PasswordManager::password get copied into the local
variable password.

This difference in behavior between assigning a reference and a pointer to a
variable marked auto is non-obvious, resulting in potentially unwanted and
unexpected copying.

https://docs.microsoft.com/en-us/cpp/code-quality/c26820?view=vs-2019

This check covers non-obvious and easy-to-miss behavior when assigning a
reference to a variable marked auto. The type of the auto variable is resolved
to a value rather than a reference, and an implicit copy is made.

*/

// copied from:
// TEST(CxxReference, check_return_and_auto)

namespace cxx_reference
{
  class Screen
  {
  private:
    int move;
    int set;

  public:
    Screen()
        : move(0)
        , set(0)
    {}

    Screen &set_move_1(int val)
    {
      move = val;
      return *this;
    }

    Screen &set_move_2(int val) { move = val; }

    Screen set_move_3(int val)
    {
      move = val;
      return *this;
    }

    Screen set_move_4(int val) { move = val; }

    void set_set(int val) { set = val; }

    std::string print()
    {
      std::ostringstream os{};
      os << "move " << move << ", set " << set;
      return os.str();
    }
  };

  class Foo
  {
  private:
    std::string _name;

  public:
    std::string &get_name_1() { return _name; }

    std::string &get_name_2()
    {
      std::string name{"get_name_2"};
      return name;
    }

    std::string print()
    {
      std::ostringstream os{};
      os << _name;
      return os.str();
    }
  };
} // namespace cxx_reference

TEST(CxxCoreGuideline, p9_dont_waste)
{
  using namespace cxx_reference;

  // set_move_1() returns reference and use "auto &"
  {
    Screen scr{};

    auto &screen = scr.set_move_1(5);
    screen.set_set(10);
    EXPECT_THAT(scr.print(), "move 5, set 10");
  }

  // set_move_1() returns reference and use "auto". *cxx-auto*
  {
    Screen scr{};

    auto screen = scr.set_move_1(5);
    screen.set_set(10);
    EXPECT_THAT(scr.print(), "move 5, set 0");
  }

  // return string reference of private member
  {
    Foo foo;

    std::string &ret = foo.get_name_1();

    ret.assign("get_name_1");

    EXPECT_THAT(foo.print(), "get_name_1");
  }

  // return string reference of private member
  {
    Foo foo;

    auto &ret = foo.get_name_1();

    ret.assign("get_name_1");

    EXPECT_THAT(foo.print(), "get_name_1");
  }

  // return string copy of private member
  {
    Foo foo;

    // *cxx-auto* NOTE WHY??
    auto ret = foo.get_name_1();

    ret.assign("get_name_1");

    EXPECT_THAT(ret, "get_name_1");

    EXPECT_THAT(foo.print(), "");
  }
}

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
