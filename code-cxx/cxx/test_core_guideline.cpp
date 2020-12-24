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
={=========================================================================
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

TEST(CxxCoreP, 9_dont_waste)
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
https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#i-interfaces

I: Interfaces

An interface is a contract between two parts of a program. Precisely stating
what is expected of a supplier of a service and a user of that service is
essential. Having good (easy-to-understand, encouraging efficient use, not
error-prone, supporting testing, etc.) interfaces is probably the most important
single aspect of code organization.

={=========================================================================
I.12: Declare a pointer that must not be null as not_null

Reason To help avoid dereferencing nullptr errors. To improve performance by
avoiding redundant checks for nullptr.Example

int length(const char* p);            // it is not clear whether length(nullptr) is valid

length(nullptr);                      // OK?

int length(not_null<const char*> p);  // better: we can assume that p cannot be nullptr

int length(const char* p);            // we must assume that p can be nullptr

By stating the intent in source, implementers and tools can provide better
diagnostics, such as finding some classes of errors through static analysis, and
perform optimizations, such as removing branches and null tests.

Note not_null is defined in the guidelines support library.Note The assumption
that the pointer to char pointed to a C-style string (a zero-terminated string
of characters) was still implicit, and a potential source of confusion and
errors. Use czstring in preference to const char*.

// we can assume that p cannot be nullptr
// we can assume that p points to a zero-terminated array of characters
int length(not_null<zstring> p);

Note: length() is, of course, std::strlen() in disguise.

*/

/*
https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#f-functions

F: Functions

A function specifies an action or a computation that takes the system from one
consistent state to the next. It is the fundamental building block of programs.

It should be possible to name a function meaningfully, to specify the
requirements of its argument, and clearly state the relationship between the
arguments and the result. An implementation is not a specification. Try to think
about what a function does as well as about how it does it. Functions are the
most critical part in most interfaces, so see the interface rules.

={=========================================================================
F.20: For “out” output values, prefer return values to output parameters

Reason 
A return value is self-documenting, whereas a & could be either in-out or
out-only and is liable to be misused. This includes large objects like standard
containers that use implicit move operations for performance and to avoid
explicit memory management.

If you have multiple values to return, use a tuple or similar multi-member type.

Example

// OK: return pointers to elements with the value x
vector<const int*> find_all(const vector<int>&, int x);

// Bad: place pointers to elements with value x in-out
void find_all(const vector<int>&, vector<const int*>& out, int x);

Note A struct of many (individually cheap-to-move) elements may be in aggregate
expensive to move. 


It is not recommended to return a const value. Such older advice is now
obsolete; it does not add value, and it interferes with move semantics.

const vector<int> fct();    // bad: that "const" is more trouble than it is worth

vector<int> g(const vector<int>& vx)
{
    // ...
    fct() = vx;   // prevented by the "const"
    // ...
    return fct(); // expensive copy: move semantics suppressed by the "const"
}

The argument for adding const to a return value is that it prevents (very rare)
accidental access to a temporary. The argument against is prevents (very
frequent) use of move semantics.

Exceptions
For non-value types, such as types in an inheritance hierarchy, return the object by unique_ptr or shared_ptr.
If a type is expensive to move (e.g., array<BigPOD>), consider allocating it on the free store and return a handle (e.g., unique_ptr), or passing it in a reference to non-const target object to fill (to be used as an out-parameter).
To reuse an object that carries capacity (e.g., std::string, std::vector) across multiple calls to the function in an inner loop: treat it as an in/out parameter and pass by reference.
Example
struct Package {      // exceptional case: expensive-to-move object
    char header[16];
    char load[2024 - 16];
};

Package fill();       // Bad: large return value
void fill(Package&);  // OK

int val();            // OK
void val(int&);       // Bad: Is val reading its argument
Enforcement
Flag reference to non-const parameters that are not read before being written to and are a type that could be cheaply returned; they should be “out” return values.
Flag returning a const value. To fix: Remove const to return a non-const value instead.


={=========================================================================
F.21: To return multiple “out” values, prefer returning a struct or tuple

Reason 

A return value is self-documenting as an “output-only” value. Note that C++ does
have multiple return values, by convention of using a tuple (including pair),
possibly with the extra convenience of tie or structured bindings (C++17) at the
  call site. 

Prefer using a named struct where there are "semantics" to the returned value.
Otherwise, a "nameless tuple" is useful in generic code.

Example

// BAD: output-only parameter documented in a comment
int f(const string& input,
    string& output_data // output only
    )
{
    // ...
    output_data = something();
    return status;
}

// GOOD: self-documenting
tuple<int, string> f(const string& input)
{
    // ...
    return make_tuple(status, something());
}

C++98’s standard library already used this style, because a pair is like a two-element tuple. For example, given a set<string> my_set, consider:

// C++98
result = my_set.insert("Hello");
if (result.second) do_something_with(result.first);    // workaround
With C++11 we can write this, putting the results directly in existing local variables:

Sometype iter;                                // default initialize if we haven't already
Someothertype success;                        // used these variables for some other purpose

tie(iter, success) = my_set.insert("Hello");   // normal return value
if (success) do_something_with(iter);
With C++17 we are able to use “structured bindings” to declare and initialize the multiple variables:

if (auto [ iter, success ] = my_set.insert("Hello"); success) do_something_with(iter);
Exception Sometimes, we need to pass an object to a function to manipulate its state. In such cases, passing the object by reference T& is usually the right technique. Explicitly passing an in-out parameter back out again as a return value is often not necessary. For example:
istream& operator>>(istream& is, string& s);    // much like std::operator>>()

for (string s; cin >> s; ) {
    // do something with line
}
Here, both s and cin are used as in-out parameters. We pass cin by (non-const) reference to be able to manipulate its state. We pass s to avoid repeated allocations. By reusing s (passed by reference), we allocate new memory only when we need to expand s’s capacity. This technique is sometimes called the “caller-allocated out” pattern and is particularly useful for types, such as string and vector, that needs to do free store allocations.

To compare, if we passed out all values as return values, we would something like this:

pair<istream&, string> get_string(istream& is)  // not recommended
{
    string s;
    is >> s;
    return {is, s};
}

for (auto p = get_string(cin); p.first; ) {
    // do something with p.second
}
We consider that significantly less elegant with significantly less performance.

For a truly strict reading of this rule (F.21), the exception isn’t really an exception because it relies on in-out parameters, rather than the plain out parameters mentioned in the rule. However, we prefer to be explicit, rather than subtle.

Note In many cases, it may be useful to return a specific, user-defined type. For example:
struct Distance {
    int value;
    int unit = 1;   // 1 means meters
};

Distance d1 = measure(obj1);        // access d1.value and d1.unit
auto d2 = measure(obj2);            // access d2.value and d2.unit
auto [value, unit] = measure(obj3); // access value and unit; somewhat redundant
                                    // to people who know measure()
auto [x, y] = measure(obj4);        // don't; it's likely to be confusing
The overly-generic pair and tuple should be used only when the value returned represents independent entities rather than an abstraction.

Another example, use a specific type along the lines of variant<T, error_code>, rather than using the generic tuple.

Enforcement
Output parameters should be replaced by return values. An output parameter is one that the function writes to, invokes a non-const member function, or passes on as a non-const.

*/

/*

ES: Expressions and statements
={=========================================================================

Expressions and statements are the lowest and most direct way of expressing
actions and computation. Declarations in local scopes are statements.

For naming, commenting, and indentation rules, see NL: Naming and layout.

General rules:

Statement rules:

ES.70: Prefer a switch-statement to an if-statement when there is a choice
ES.71: Prefer a range-for-statement to a for-statement when there is a choice
ES.72: Prefer a for-statement to a while-statement when there is an obvious loop variable
ES.73: Prefer a while-statement to a for-statement when there is no obvious loop variable
ES.74: Prefer to declare a loop variable in the initializer part of a for-statement
ES.75: Avoid do-statements
ES.76: Avoid goto
ES.77: Minimize the use of break and continue in loops
ES.78: Don't rely on implicit fallthrough in switch statements
ES.79: Use default to handle common cases (only)
ES.84: Don't try to declare a local variable with no name
ES.85: Make empty statements visible
ES.86: Avoid modifying loop control variables inside the body of raw for-loops
ES.87: Don't add redundant == or != to conditions

Arithmetic rules:

ES.100: Don't mix signed and unsigned arithmetic
ES.101: Use unsigned types for bit manipulation
ES.102: Use signed types for arithmetic
ES.103: Don't overflow
ES.104: Don't underflow
ES.105: Don't divide by integer zero
ES.106: Don't try to avoid negative values by using unsigned
ES.107: Don't use unsigned for subscripts, prefer gsl::index

={=========================================================================
ES.71: Prefer a range-for-statement to a for-statement when there is a choice

Enforcement

Look at loops, if a traditional loop just looks at each element of a sequence,
and there are no side effects on what it does with the elements, rewrite the
loop to a ranged-for loop.

https://docs.microsoft.com/en-us/cpp/code-quality/c26817?view=vs-2019

*/

/*
={=========================================================================

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


={=========================================================================
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

TEST(CxxCoreEs, 79_switch_and_enum)
{
  using namespace core_es_79;

  foo(0);
}

/*
={=========================================================================
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

TEST(CxxCoreEs, 78_switch_fallthrough)
{
  using namespace core_es_78;

  foo(0);
}

/*
={=========================================================================
ES.100: Don't mix signed and unsigned arithmetic +1

Reason
Avoid wrong results.

Example
int x = -3;
unsigned int y = 7;

cout << x - y << '\n';  // unsigned result, possibly 4294967286
cout << x + y << '\n';  // unsigned result: 4
cout << x * y << '\n';  // unsigned result, possibly 4294967275

It is harder to spot the problem in more realistic examples.

Note

Unfortunately, C++ uses signed integers for array subscripts and the standard
library uses unsigned integers for container subscripts. This precludes
consistency. Use gsl::index for subscripts; see ES.107.

Enforcement

Compilers already know and sometimes warn.

(To avoid noise) Do not flag on a mixed signed/unsigned comparison where one of
the arguments is sizeof or a call to container .size() and the other is
ptrdiff_t.


={=========================================================================
ES.101: Use unsigned types for bit manipulation +1

Reason
Unsigned types support bit manipulation without surprises from sign bits.

Example
unsigned char x = 0b1010'1010;
unsigned char y = ~x;   // y == 0b0101'0101;

Note
Unsigned types can also be useful for modulo arithmetic. However, if you want
modulo arithmetic add comments as necessary noting the reliance on wraparound
behavior, as such code can be surprising for many programmers.

Enforcement
Just about impossible in general because of the use of unsigned subscripts in
the standard library


={=========================================================================
ES.102: Use signed types for arithmetic +1

Reason
Because most arithmetic is assumed to be signed; x - y yields a negative number
when y > x except in the rare cases where you really want modulo arithmetic.


Example
Unsigned arithmetic can yield surprising results if you are not expecting it.
This is even more true for mixed signed and unsigned arithmetic.

template<typename T, typename T2>
T subtract(T x, T2 y)
{
    return x - y;
}

void test()
{
    int s = 5;
    unsigned int us = 5;
    cout << subtract(s, 7) << '\n';       // -2
    cout << subtract(us, 7u) << '\n';     // 4294967294
    cout << subtract(s, 7u) << '\n';      // -2
    cout << subtract(us, 7) << '\n';      // 4294967294
    cout << subtract(s, us + 2) << '\n';  // -2
    cout << subtract(us, s + 2) << '\n';  // 4294967294
}

Here we have been very explicit about what's happening, but if you had seen 

us - (s + 2) or 
s += 2; ...; us - s

would you reliably have suspected that the result would print as 4294967294?

Exception
Use unsigned types if you really want modulo arithmetic - add comments as
necessary noting the reliance on overflow behavior, as such code is going to be
surprising for many programmers.


Example
The standard library uses unsigned types for subscripts. The built-in array uses
signed types for subscripts. This makes surprises (and bugs) inevitable.

int a[10];
for (int i = 0; i < 10; ++i) a[i] = i;
vector<int> v(10);

// compares signed to unsigned; some compilers warn, but we should not

for (gsl::index i = 0; i < v.size(); ++i) v[i] = i;

int a2[-2];         // error: negative size

// OK, but the number of ints (4294967294) is so large that we should get an
// exception

vector<int> v2(-2);


Use gsl::index for subscripts; see ES.107.


={=========================================================================
ES.103: Don't overflow

Reason
Overflow usually makes your numeric algorithm meaningless. Incrementing a value
beyond a maximum value can lead to memory corruption and undefined behavior.


Example, bad

int a[10];
a[10] = 7;   // bad, array bounds overflow

for (int n = 0; n <= 10; ++n)
    a[n] = 9;   // bad, array bounds overflow


Example, bad

int n = numeric_limits<int>::max();
int m = n + 1;   // bad, numeric overflow


Example, bad

int area(int h, int w) { return h * w; }
auto a = area(10'000'000, 100'000'000);   // bad, numeric overflow


Exception
Use unsigned types if you really want modulo arithmetic.

Alternative: For critical applications that can afford some overhead, use a
range-checked integer and/or floating-point type.


={=========================================================================
ES.104: Don't underflow

Reason

Decrementing a value beyond a minimum value can lead to memory corruption and
undefined behavior.

Example, bad

int a[10];
a[-2] = 7;   // bad

int n = 101;
while (n--)
    a[n - 1] = 9;   // bad (twice)

Exception
Use unsigned types if you really want modulo arithmetic.


={=========================================================================
ES.105: Don't divide by integer zero

Reason
The result is undefined and probably a crash.

Note
This also applies to %.

Example, bad
int divide(int a, int b)
{
    // BAD, should be checked (e.g., in a precondition)
    return a / b;
}

Example, good
int divide(int a, int b)
{
    // good, address via precondition 
    // (and replace with contracts once C++ gets them)

    Expects(b != 0);
    return a / b;
}

double divide(double a, double b)
{
    // good, address via using double instead
    return a / b;
}

Alternative: For critical applications that can afford some overhead, use a
range-checked integer and/or floating-point type.

Enforcement
Flag division by an integral value that could be zero


={=========================================================================
ES.106: Don't try to avoid negative values by using unsigned

Reason
Choosing unsigned implies many changes to the usual behavior of integers,
including modulo arithmetic, can suppress warnings related to overflow, and
  opens the door for errors related to signed/unsigned mixes. Using unsigned
  doesn't actually eliminate the possibility of negative values.

Example
unsigned int u1 = -2;   // Valid: the value of u1 is 4294967294
int i1 = -2;
unsigned int u2 = i1;   // Valid: the value of u2 is 4294967294
int i2 = u2;            // Valid: the value of i2 is -2

These problems with such (perfectly legal) constructs are hard to spot in real
code and are the source of many real-world errors. 

Consider:

unsigned area(unsigned height, unsigned width) 
{ return height*width; } // [see also](#Ri-expects)
// ...
int height;
cin >> height;
auto a = area(height, 2);   // if the input is -2 a becomes 4294967292

Remember that -1 when assigned to an unsigned int becomes the largest unsigned
int. Also, since unsigned arithmetic is modulo arithmetic the multiplication
didn't overflow, it wrapped around.

Example
unsigned max = 100000;    // "accidental typo", I mean to say 10'000
unsigned short x = 100;
while (x < max) x += 100; // infinite loop

Had x been a signed short, we could have warned about the undefined behavior
upon overflow.


Alternatives
o use signed integers and check for x >= 0
o use a positive integer type
o use an integer subrange type
o Assert(-1 < x)

For example

struct Positive {
    int val;
    Positive(int x) :val{x} { Assert(0 < x); }
    operator int() { return val; }
};

int f(Positive arg) { return arg; }

int r1 = f(2);
int r2 = f(-2);  // throws


Enforcement
See ES.100 Enforcements.


={=========================================================================
ES.107: Don't use unsigned for subscripts, prefer gsl::index

Reason
To avoid signed/unsigned confusion. To enable better optimization. To enable
better error detection. To avoid the pitfalls with auto and int.

Example, bad
vector<int> vec = {...};

for (int i = 0; i < vec.size(); i += 2)                    // might not be big enough
    cout << vec[i] << '\n';

for (unsigned i = 0; i < vec.size(); i += 2)               // risk wraparound
    cout << vec[i] << '\n';

for (auto i = 0; i < vec.size(); i += 2)                   // might not be big enough
    cout << vec[i] << '\n';

for (vector<int>::size_type i = 0; i < vec.size(); i += 2) // verbose
    cout << vec[i] << '\n';

for (auto i = vec.size()-1; i >= 0; i -= 2)                // bug
    cout << vec[i] << '\n';

for (int i = vec.size()-1; i >= 0; i -= 2)                 // might not be big enough
    cout << vec[i] << '\n';

(since: Unfortunately, C++ uses signed integers for array subscripts and the standard
library uses unsigned integers for container subscripts.
so "i" can be negative and goes big number)


Example, good
vector<int> vec = {...};

for (gsl::index i = 0; i < vec.size(); i += 2)             // ok
    cout << vec[i] << '\n';

for (gsl::index i = vec.size()-1; i >= 0; i -= 2)          // ok
    cout << vec[i] << '\n';

Note
The built-in array uses signed subscripts. The standard-library containers use
unsigned subscripts. Thus, no perfect and fully compatible solution is possible
(unless and until the standard-library containers change to use signed
 subscripts someday in the future). Given the known problems with unsigned and
signed/unsigned mixtures, better stick to (signed) integers of a sufficient
size, which is guaranteed by gsl::index.

Example
template<typename T>
struct My_container {
public:
    // ...
    T& operator[](gsl::index i);    // not unsigned
    // ...
};

Example
??? demonstrate improved code generation and potential for error detection ???

Alternatives

o use algorithms
o use range-for
o use iterators/pointers

Enforcement
Very tricky as long as the standard-library containers get it wrong.

(To avoid noise) Do not flag on a mixed signed/unsigned comparison where one of
the arguments is sizeof or a call to container .size() and the other is
ptrdiff_t.

*/

/*
Why infinite loop? since x is unsigned and max is less than "max", it wraps
around between [?, 65535]

ushort max   : 65535

so infinite loop

*/
TEST(CxxCoreEs, 106_infinite_loop)
{
  {
    unsigned max     = 100000; // "accidental typo", I mean to say 10'000
    unsigned short x = 100;
    while (x < max)
      x += 100; // infinite loop
  }

  // warning: comparison between signed and unsigned integer expressions [-Wsign-compare]
  // while (x < max) x += 100; // infinite loop

  {
    unsigned max   = 100000; // "accidental typo", I mean to say 10'000
    signed short x = 100;
    while (x < max)
      x += 100; // infinite loop
  }
}

/*

"Also, since unsigned arithmetic is modulo arithmetic the multiplication
didn't overflow, it wrapped around."

is it the case only for "unsigned"? nope.

max : 0
max : 1
max : 2
max : 3
max : 4
max : 5
max : 6
max : 7
max : 8
max : 9
max : -32768
max : -32767
max : -32766
max : -32765
max : -32764
max : -32763
max : -32762
max : -32761
max : -32760
max : -32759


https://www.gnu.org/software/gnulib/manual/html_node/Wraparound-Arithmetic.html

15.7.3 Wraparound Arithmetic with Signed Integers

Signed integer arithmetic has undefined behavior on overflow in C. Although
almost all modern computers use two’s complement signed arithmetic that is
well-defined to wrap around, C compilers routinely optimize assuming that signed
integer overflow cannot occur, which means that a C program cannot easily get at
the underlying machine arithmetic. For example, on a typical machine with 32-bit
two’s complement int the expression INT_MAX + 1 does not necessarily yield
INT_MIN, because the compiler may do calculations with a 64-bit register, or may
generate code that traps on signed integer overflow.

*/

TEST(CxxCoreEs, 106_signed_wraps)
{
  {
    // ushort max   : 65535
    unsigned short max{65535};

    for (int i = 0; i < 10; ++i)
    {
      ++max;
      std::cout << "max : " << max << std::endl;
    }
  }

  {
    // short  max   : 32767
    short max{32767};
    for (int i = 0; i < 10; ++i)
    {
      ++max;
      std::cout << "max : " << max << std::endl;
    }
  }
}

// ={=========================================================================

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
