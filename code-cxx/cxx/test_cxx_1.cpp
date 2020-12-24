#include <any>
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
#include <variant>
#include <vector>

#include "gmock/gmock.h"

using namespace std;
using namespace std::placeholders;
using namespace testing;

/*
={=========================================================================
warning: variable ‘first’ set but not used [-Wunused-but-set-variable]

https://stackoverflow.com/questions/23998283/variable-warning-set-but-not-used

because

int none[5];        // declared, not set to anything

And then:

none[i] = number1;  // a value has been set, but it's not being used for anything

as the code is, you have: "none" set but not used; exactly what the compiler said.

*/

TEST(CxxErrorWarning, not_used)
{
  {
    // warning: variable ‘first’ set but not used [-Wunused-but-set-variable]
    size_t first{0};
    first = 1;

    // this make the warning go away
    // std::cout << first << std::endl;
  }
}

/*
// ={=========================================================================
cxx-allocator

alloc.allocate(size_type n);

Allocates n * sizeof(T) bytes of uninitialized storage by calling ::operator
new(std::size_t)

*/

TEST(CxxMemoryModel, check_allocator)
{
  // do allocation only
  {
    std::allocator<std::string> alloc;

    auto const p = alloc.allocate(10);

    // ? size?
    EXPECT_THAT(sizeof(p), 8);
    EXPECT_THAT(sizeof(*p), 32);

    alloc.deallocate(p, 10);
  }

  // do allocation and construction
  // ? why sizeof are always the same regardless of string length
  {
    std::vector<std::string> expected{};

    std::allocator<std::string> alloc;

    // auto const data = alloc.allocate(10);
    // NOTE: compile error when use `const`
    auto data = alloc.allocate(10);

    // back it up.
    // NOTE: shall use `coll` to deallocate and access. crashes otherwise.
    auto const coll = data;

    EXPECT_THAT(sizeof(coll), 8);
    EXPECT_THAT(sizeof(*coll), 32);

    // std::cout << "sizeof p " << sizeof(data) << std::endl;
    // std::cout << "sizeof *p " << sizeof(*data) << std::endl;

    // calls ctor. From *cxx-11*, supports many ctors of the 'type'.
    //
    // The first argument to construct() must be a pointer to unconstructed
    // space allocated by allocate(). The second and subsequent argument
    // determine ctor to use.

    alloc.construct(data++, "looooooooooooooooooooooooooooooooooooong string1");
    alloc.construct(data++, "looooooooooooooooooooooooooooooooooooong string2");
    alloc.construct(data++, "looooooooooooooooooooooooooooooooooooong string3");

    EXPECT_THAT(sizeof(coll), 8);
    EXPECT_THAT(sizeof(*coll), 32);

    // std::cout << "sizeof p " << sizeof(data) << std::endl;
    // std::cout << "sizeof *p " << sizeof(*data) << std::endl;

    // for (int i = 0; i < 3; i++)
    //   expected.emplace_back(coll[i]);
    // EXPECT_THAT(expected, ElementsAre("string1", "string2", "string3"));

    // alloc.destroy(p) calls dtor

    alloc.destroy(--data);
    alloc.destroy(--data);
    alloc.destroy(data);

    EXPECT_THAT(sizeof(coll), 8);
    EXPECT_THAT(sizeof(*coll), 32);

    // std::cout << "sizeof p " << sizeof(data) << std::endl;
    // std::cout << "sizeof *p " << sizeof(*data) << std::endl;

    alloc.deallocate(coll, 10);
  }

  // do allocation and construction
  {
    std::allocator<std::string> alloc;

    // NOTE: no compile error in this case where use uninitialized_copy
    auto const data = alloc.allocate(10);

    const std::vector<std::string> coll{"string1", "string2", "string3"};

    // o The library defines two that calls `construct()` on each element in
    //   unconstructed memory.
    //
    // o Returns the position after the last initialized element.
    //   Like cxx-copy, return its incremented destination iterator.
    //
    // to sum, do "construct and copy" to destination
    //
    // uninitialized_copy_n(b, n, b2);
    // uninitialized_fill(b, e, t);
    // uninitialized_fill_n(b, n, t);

    std::uninitialized_copy(coll.cbegin(), coll.cend(), data);

    alloc.deallocate(data, 10);
  }
}

/*
// ={=========================================================================
cxx-size cxx-sizeof

LP64
__x86_64__
size of (enum) is               : 4
size of (unsigned short) is     : 2
size of (int) is                : 4
size of (unsigned int) is       : 4
size of (unsigned) is           : 4
size of (long) is               : 8
size of (unsigned long) is      : 8
size of (long int) is           : 8
size of (unsigned long int) is  : 8
size of (long long) is          : 8
size of (unsigned long long) is : 8
size of (* int) is              : 8
size of (* unsigned int) is     : 8
size of (size_t) is             : 8
size of (uint8_t) is            : 1
size of (uint16_t) is           : 2
size of (uint32_t) is           : 4
size of (uint64_t) is           : 8

LP32
__x86_32__
size of (int) is                : 4
size of (unsigned int) is       : 4
size of (long) is               : 4
size of (unsigned long) is      : 4
size of (long int) is           : 4
size of (unsigned long int) is  : 4
size of (long long) is          : 8
size of (unsigned long long) is : 8
size of (* int) is              : 4
size of (* unsigned int) is     : 4
size of (size_t) is             : 4

size of (uint32_t) is           : 4
size of (uint64_t) is           : 8

LP32 (arm embedded)
size of (int) is                : 4
size of (unsigned int) is       : 4
size of (long) is               : *4
size of (unsigned long) is      : *4
size of (long int) is           : *4
size of (unsigned long int) is  : *4
size of (long long) is          : 8
size of (unsigned long long) is : 8
size of (* int) is              : *4
size of (* unsigned int) is     : *4
size of (size_t) is             : *4
size of (uint8_t) is            : 1
size of (uint16_t) is           : 2
size of (uint32_t) is           : 4
size of (uint64_t) is           : 8


case:
the problem is when run the same code, converting ints to json, gets different
json output in the end. found out that type size is different and math output
are different for the same code between embedded and PC.

code

{
  auto now = std::time(0);
  std::cout << "current utc      : " << now << std::endl;
  std::cout << "sizeof  utc      : " << sizeof now << std::endl;

  auto now_to_ms_1 = now * 1000;
  std::cout << "current utc in ms: " << now_to_ms_1 << std::endl;
  std::cout << "sizeof  utc in ms: " << sizeof now_to_ms_1 << std::endl;

  uint64_t now_to_ms_2 = static_cast<uint64_t>(now * 1000);
  std::cout << "current utc in ms: " << now_to_ms_2 << std::endl;
  std::cout << "sizeof  utc in ms: " << sizeof now_to_ms_2 << std::endl;

  uint64_t now_to_ms_3 = now*1000;
  std::cout << "current utc in ms: " << now_to_ms_3 << std::endl;
}

Linux skyxione 4.9.119 #1 SMP PREEMPT Wed Sep 30 22:03:33 UTC 2020 armv7l GNU/Linux

current utc      : 1601566641
sizeof  utc      : 4
current utc in ms: -456160408
sizeof  utc in ms: 4
current utc in ms: 18446744073253391208
sizeof  utc in ms: 8
current utc in ms: 18446744073253391208

Linux kit-ubuntu 5.4.0-48-generic #52~18.04.1-Ubuntu SMP Thu Sep 10 12:50:22 UTC 2020 x86_64 x86_64 x86_64 GNU/Linux

current utc      : 1601566595
sizeof  utc      : 8
current utc in ms: 1601566595000
sizeof  utc in ms: 8
current utc in ms: 1601566595000
sizeof  utc in ms: 8
current utc in ms: 1601566595000

NOTE:
Why "std::time(0) * 1000" is different on embedded? Use ms time from chrono and
use division from it to get ms. this shows right value

{
  std::chrono::milliseconds milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() );
  uint64_t chrono_ms_count = milliseconds.count();

  auto chrono_sec_count = chrono_ms_count / 1000;
}

current chrono utc in ms : 1601567967363
current chrono utc in sec: 1601567967

so use this approach.

*cxx-auto*

the problem is that "auto now = std::time(0);" selects "int" which is 4 on
embedded platform. Then "now * 1000" causes overflow and then caues a big value
when casted to uint64_t later.

when "explicityl" say uint64_t to use big enough type, all goes well:

{
  // from:
  auto now = std::time(0);

  // to:
  uint64_t now = std::time(0);
}

current utc      : 1601580428
sizeof  utc      : 8
current utc in ms: 1601580428000
sizeof  utc in ms: 8
current utc in ms: 1601580428000
sizeof  utc in ms: 8
current utc in ms: 1601580428000
current chrono utc in ms : 1601580428052
current chrono utc in sec: 1601580428
1

*/

namespace
{
  enum State
  {
    IdleState,
    StartedState,
    PlayingSuperState,
    PendingState,
    PlayingState,
    BlockedState,
    StoppedState
  };
}

// NOTE that "enum to int" has no compile error but "int to enum" causes compile
// error see cxx-static-cast

// ={=========================================================================
TEST(CxxType, sizes)
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

  std::cout << "size of (enum) is               : " << sizeof(State)
            << std::endl;

  std::cout << "size of (unsigned short) is     : " << sizeof(unsigned short)
            << std::endl;

  std::cout << "size of (int) is                : " << sizeof(int) << std::endl;
  std::cout << "size of (unsigned int) is       : " << sizeof(unsigned int)
            << std::endl;

  std::cout << "size of (unsigned) is           : " << sizeof(unsigned)
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
  std::cout << "size of (uint8_t) is            : " << sizeof(uint8_t) << endl;
  std::cout << "size of (uint16_t) is           : " << sizeof(uint16_t) << endl;
  std::cout << "size of (uint32_t) is           : " << sizeof(uint32_t) << endl;
  std::cout << "size of (uint64_t) is           : " << sizeof(uint64_t) << endl;
}

/*
// ={=========================================================================
cxx-limits

ushort max   : 65535
ushort min   : 0
short  max   : 32767
short  min   : -32768
u     max    : 4294967295
u     min    : 0
uint   max   : 4294967295
uint   min   : 0
 int   max   : 2147483647
 int   min   : -2147483648
 long max    : 9223372036854775807
 long min    : -9223372036854775808
long long max: 9223372036854775807
long long min: -9223372036854775808
int64 max    : 9223372036854775807
int64 min    : -9223372036854775808
uint64 max   : 18446744073709551615
uint64 min   : 0

*/
TEST(CxxType, limits)
{
  std::cout << "ushort max: " << std::numeric_limits<unsigned short>::max()
            << std::endl;
  std::cout << "ushort min: " << std::numeric_limits<unsigned short>::min()
            << std::endl;

  std::cout << "short  max: " << std::numeric_limits<short>::max() << std::endl;
  std::cout << "short  min: " << std::numeric_limits<short>::min() << std::endl;

  std::cout << "u     max: " << std::numeric_limits<unsigned>::max()
            << std::endl;
  std::cout << "u     min: " << std::numeric_limits<unsigned>::min()
            << std::endl;

  std::cout << "uint  max: " << std::numeric_limits<unsigned int>::max()
            << std::endl;
  std::cout << "uint  min: " << std::numeric_limits<unsigned int>::min()
            << std::endl;

  std::cout << " int  max: " << std::numeric_limits<int>::max() << std::endl;
  std::cout << " int  min: " << std::numeric_limits<int>::min() << std::endl;

  std::cout << " long max: " << LONG_MAX << std::endl;
  std::cout << " long min: " << LONG_MIN << std::endl;

  std::cout << "long long max: " << std::numeric_limits<long long>::max()
            << std::endl;
  std::cout << "long long min: " << std::numeric_limits<long long>::min()
            << std::endl;

  std::cout << " int64 max: " << std::numeric_limits<int64_t>::max()
            << std::endl;
  std::cout << " int64 min: " << std::numeric_limits<int64_t>::min()
            << std::endl;

  std::cout << "uint64 max: " << std::numeric_limits<uint64_t>::max()
            << std::endl;
  std::cout << "uint64 min: " << std::numeric_limits<uint64_t>::min()
            << std::endl;
}

/*
// ={=========================================================================
cxx-overflow see TEST(CxxCoreEs)

int       : -1
int       : -1
int       : i

long long : -1
long long : -2
long long : x

auto      : -2
auto      : i

so addition is "integer addition"

auto      : 9223372036854775807
auto      : l

*/

TEST(CxxType, overflow)
{
  // long long max: 9223372036854775807
  auto value0{9223372036854775807};

  //  int   max: 2147483647
  int value1{2147483647};

  //  int   max: 2147483647
  int value2{2147483647};

  {
    (value1 + value2);

    (value1 + value2) / 2;

    std::cout << "int       : " << (value1 + value2) / 2 << std::endl;

    int value3{};

    value3 = (value1 + value2) / 2;

    std::cout << "int       : " << value3 << std::endl;
    std::cout << "int       : " << typeid(value3).name() << std::endl;
  }

  // https://gcc.gnu.org/onlinedocs/gcc/Integer-Overflow-Builtins.html
  //
  // Built-in Function: bool __builtin_add_overflow (type1 a, type2 b, type3 *res)
  // Built-in Function: bool __builtin_sadd_overflow (int a, int b, int *res)
  //
  // If the stored result is equal to the infinite precision result, the
  // built-in functions return false, otherwise they return true.
  //
  // false : not overflow

  {
    int result{};

    EXPECT_THAT(__builtin_add_overflow(value1, value2, &result), true);
    EXPECT_THAT(__builtin_sadd_overflow(value1, value2, &result), true);

    std::cout << "result       : " << result << std::endl;

    EXPECT_THAT(__builtin_add_overflow_p(value1, 1, (int)0), true);
  }

  // The following built-in functions allow checking if simple arithmetic
  // operation would overflow.
  //
  // Built-in Function: bool __builtin_add_overflow_p (type1 a, type2 b, type3 c)
  // Built-in Function: bool __builtin_sub_overflow_p (type1 a, type2 b, type3 c)
  // Built-in Function: bool __builtin_mul_overflow_p (type1 a, type2 b, type3 c)
  //
  // These built-in functions are similar to __builtin_add_overflow,
  // __builtin_sub_overflow, or __builtin_mul_overflow, except that they don’t
  // store the result of the arithmetic operation anywhere and the last argument
  // is not a pointer, but some expression with integral type other than
  // enumerated or boolean type.

  // The built-in functions promote the first two operands into infinite
  // precision signed type and perform addition on those promoted operands.
  //
  // The result is then cast to "the type of the third argument."
  //
  // If the cast result is equal to the infinite precision result, the built-in
  // functions return false, otherwise they return true.
  //
  // false : not overflow

  // "The value" of the third argument is ignored, just the side effects in the
  // third argument are evaluated, and no integral argument promotions are
  // performed on the last argument. If the third argument is a bit-field, the
  // type used for the result cast has the precision and signedness of the given
  // bit-field, rather than precision and signedness of the underlying type.

  // For example, the following macro can be used to portably check, at compile-time, whether or not adding two constant integers will overflow, and perform the addition only when it is known to be safe and not to trigger a -Woverflow warning.
  //
  // #define INT_ADD_OVERFLOW_P(a, b) \
  //    __builtin_add_overflow_p (a, b, (__typeof__ ((a) + (b))) 0)
  //
  // enum {
  //     A = INT_MAX, B = 3,
  //     C = INT_ADD_OVERFLOW_P (A, B) ? 0 : A + B,
  //     D = __builtin_add_overflow_p (1, SCHAR_MAX, (signed char) 0)
  // };

  {
    long long value4{};

    value4 = (value1 + value2) / 2;

    std::cout << "long long : " << value4 << std::endl;

    long long value5{};

    value5 = (value1 + value2);

    std::cout << "long long : " << (value5) << std::endl;
    std::cout << "long long : " << typeid(value5).name() << std::endl;
  }

  {
    auto value6 = (value1 + value2);

    std::cout << "auto      : " << (value6) << std::endl;
    std::cout << "auto      : " << typeid(value6).name() << std::endl;

    std::cout << "auto      : " << value0 << std::endl;
    std::cout << "auto      : " << typeid(value0).name() << std::endl;
  }
}

// ={=========================================================================
TEST(CxxType, null)
{
  // check null char, '\0', which is actually integer 0.
  {
    std::ostringstream os;

    // in order to get hex value for char 'a', which is 61, have to use (int)
    // cast.
    //
    // std::cout << "a: " << hex << (int)'a' << std::endl;
    // gets "61"
    //
    // std::cout << "a: " << hex << 'a' << std::endl;
    // gets "a"

    os << std::hex << (int)'0';
    EXPECT_THAT(os.str(), "30");

    // reset os
    os.str("");

    os << std::hex << (int)'\0';
    EXPECT_THAT(os.str(), "0");
  }

  // nullptr is not same
  {
    const char *p1 = "";
    const char *p2 = nullptr;

    EXPECT_NE(p1, p2);
  }
}

// ={=========================================================================
// cxx-cast cxx-conversion cxx-printf

TEST(CxxType, check_types)
{
  // converts int to float
  // values {55.000000, 140.000000, 436.000000, 246.000000}
  // values {55.000000, 140.000000, 0.227083, 0.227778}
  {
    int x{55};
    int y{140};

    unsigned width{436};
    unsigned height{246};

    float value[4];

    value[0] = static_cast<float>(x);
    value[1] = static_cast<float>(y);
    value[2] = static_cast<float>(width);
    value[3] = static_cast<float>(height);

    printf("values {%f, %f, %f, %f}\n", value[0], value[1], value[2], value[3]);

    value[2] = static_cast<float>(width) / 1920.0f;
    value[3] = static_cast<float>(height) / 1080.0f;

    printf("values {%f, %f, %f, %f}\n", value[0], value[1], value[2], value[3]);
  }

  {
    int value0{1920};
    float value1{1920.0f};

    // convert int to float
    float value2 = float(value0);

    EXPECT_TRUE((value2 / value1) == 1.0);
    EXPECT_TRUE((value2 / value1) == 1);
  }
}

// f, F
// The `double  argument` is rounded and converted to decimal notation in
// the style [-]ddd.ddd, where the number of digits after the
// decimal-point character is equal to the precision specification.  If
// the  precision  is missing,  it  is  taken as 6; if the precision is
// explicitly zero, no decimal-point character appears.  If a decimal
// point appears, at least one digit appears before it.

// ={=========================================================================
TEST(CxxType, check_types_double)
{
  // cxx-double
  // double max: 1.79769e+308
  // double min: 2.22507e-308
  // values {368720064, 0} NOTE: "368720064" keeps changing whenever runs.
  // values 1.08917e+09
  // values {1089165312.000000, 1089165312.000000}
  {
    std::cout << "double max: " << std::numeric_limits<double>::max()
              << std::endl;
    std::cout << "double min: " << std::numeric_limits<double>::min()
              << std::endl;

    double value{1089165312};

    printf("values {%d, %ld}\n", value, value);
    std::cout << "values " << value << std::endl;
    printf("values {%f, %lf}\n", value, value);
  }

  // `time_t, which is a signed integer`, can represent
  // values {1089165312.000000, 1089165}
  {
    double value{1089165312};
    time_t time = static_cast<time_t>(value / 1000.0f);

    printf("values {%f, %ld}\n", value, time);
  }

  // double value: 0
  // double value is not < or > 0
  {
    double value{0};
    printf("double value: %g\n", value);

    if (value < 0)
      printf("double value is < 0\n");
    else if (value > 0)
      printf("double value is > 0\n");
    else
      printf("double value is not < or > 0\n");
  }
}

/*
// ={=========================================================================
https://docs.microsoft.com/en-us/cpp/cpp/type-conversions-and-type-safety-modern-cpp?view=msvc-160

Signed - unsigned conversions

A signed integral type and its unsigned counterpart are always the same size,
but they differ in how the bit pattern is "interpreted" for value transformation.
The following code example demonstrates what happens when the same bit pattern
is interpreted as a signed value and as an unsigned value. The bit pattern
stored in both num and num2 never changes from what is shown in the earlier
illustration.

*/

TEST(CxxType, narrow_conversion)
{
  // warnings
  {
    int x{100};
    int y{200};

    // warning: narrowing conversion of ‘x’ from ‘int’ to ‘unsigned int’ inside { } [-Wnarrowing]
    //      unsigned int ux{x};
    //
    // warning: narrowing conversion of ‘y’ from ‘int’ to ‘unsigned int’ inside { } [-Wnarrowing]
    //      unsigned int uy{y};

    unsigned int ux{x};
    unsigned int uy{y};

    EXPECT_THAT(ux, 100);
    EXPECT_THAT(uy, 200);
  }

  // no warnings. so it is because *cxx-uniform-init* but not actual narrow
  // conversion (data loss) warning.
  {
    int x{100};
    int y{200};

    unsigned int ux = x;
    unsigned int uy = y;

    EXPECT_THAT(ux, 100);
    EXPECT_THAT(uy, 200);
  }
}

/*
={=========================================================================
cxx-variant

*/
namespace cxx_variant
{
  struct Argument
  {
    enum class Type
    {
      Bool,
      Int,
      UInt,
      Double,
      String
    } _type;

    // NOTE:
    // 1. cannot use in-class init for union
    // 2.
    // cxx-error: initializations for multiple members of
    // since it's cxx-union
    // explicit Base(bool b)
    //     : bool_(b)
    //     , i_(0)
    //     , ui_(0)
    //     , real_(0.0f)
    // {}
    union Base
    {
      bool _b;
      int _i;
      unsigned int _ui;
      double _d;

      // default ctor is necessary since `string` type needs it and otherwise
      // it's error
      explicit Base()
          : _d(0.0f)
      {}
      explicit Base(bool b)
          : _b(b)
      {}
      explicit Base(int i)
          : _i(i)
      {}
      explicit Base(unsigned int ui)
          : _ui(ui)
      {}
      explicit Base(double d)
          : _d(d)
      {}
    } _base;

    std::string _string;

    explicit Argument(bool value)
        : _type(Type::Bool)
        , _base(value)
    {}

    explicit Argument(int value)
        : _type(Type::Int)
        , _base(value)
    {}

    explicit Argument(unsigned int value)
        : _type(Type::UInt)
        , _base(value)
    {}

    explicit Argument(double value)
        : _type(Type::Double)
        , _base(value)
    {}

    explicit Argument(const std::string &value)
        : _type(Type::String)
        , _string(value)
    {}

    // getters *cxx-conversion-op* cxx-operator-conversion
    operator bool() const
    {
      if (_type != Type::Bool)
      {
        // std::cout << "the type saved is not bool\n";
        return false;
      }

      return _base._b;
    }

    operator int() const
    {
      if (_type != Type::Int)
      {
        // std::cout << "the type saved is not int\n";
        return std::numeric_limits<int>::max();
      }

      return _base._i;
    }

    operator unsigned int() const
    {
      if (_type != Type::UInt)
      {
        // std::cout << "the type saved is not unsigned int\n";
        return std::numeric_limits<unsigned int>::max();
      }

      return _base._ui;
    }

    operator double() const
    {
      if (_type != Type::Double)
      {
        // std::cout << "the type saved is not double int\n";
        return std::nan("");
      }

      return _base._d;
    }

    operator std::string() const
    {
      if (_type != Type::String)
      {
        // std::cout << "the type saved is not string\n";
        return "";
      }

      return _string;
    }
  }; // struct
} // namespace cxx_variant

// ={=========================================================================
TEST(CxxTypeVariant, custom_variant)
{
  using namespace cxx_variant;

  // error since it do not have default ctor
  {
    // Argument arg1;
  }

  {
    Argument arg(true);
    EXPECT_THAT((bool)arg, true);
  }

  {
    Argument arg(3301);
    EXPECT_THAT((bool)arg, false);
  }

  {
    Argument arg(3301);
    EXPECT_THAT((int)arg, 3301);
  }

  {
    Argument arg(3301.0);
    EXPECT_THAT((double)arg, 3301.0);
  }

  {
    // since ctor is explicit
    Argument arg(std::string("string variant"));
    EXPECT_THAT((std::string)arg, std::string("string variant"));
  }
}

namespace cxx_variant_1
{
  // with insert() interface
  class VariantMap
  {
  private:
    struct Variant
    {
      enum Type
      {
        Boolean,
        Integer,
        Double,
        String
      } m_type;

      union Basic
      {
        bool boolean;
        int integer;
        double real;

        Basic() = default;

        explicit Basic(bool b)
            : boolean(b)
        {}
        explicit Basic(int i)
            : integer(i)
        {}
        explicit Basic(double d)
            : real(d)
        {}
      } m_basic;

      std::string m_string;

      explicit Variant(bool b)
          : m_type(Boolean)
          , m_basic(b)
      {}
      explicit Variant(int i)
          : m_type(Integer)
          , m_basic(i)
      {}
      explicit Variant(double d)
          : m_type(Double)
          , m_basic(d)
      {}
      explicit Variant(const std::string &str)
          : m_type(String)
          , m_string(str)
      {}
      explicit Variant(std::string &&str)
          : m_type(String)
          , m_string(std::move(str))
      {}
      explicit Variant(const char *str)
          : m_type(String)
          , m_string(str)
      {}
    }; // struct Variant

    std::map<std::string, Variant> m_map;

  public:
    VariantMap()                   = default;
    VariantMap(const VariantMap &) = default;
    VariantMap(VariantMap &&)      = default;

    // insert for bool values
    void insert(const std::string &key, bool value)
    {
      m_map.emplace(key, value);
    }

    void insert(std::string &&key, bool value)
    {
      m_map.emplace(std::move(key), value);
    }

    // insert for int values
    void insert(const std::string &key, int value)
    {
      m_map.emplace(key, value);
    }

    void insert(std::string &&key, int value)
    {
      m_map.emplace(std::move(key), value);
    }

    // insert for string values
    void insert(const std::string &key, const std::string &value)
    {
      m_map.emplace(key, value);
    }

    void insert(std::string &&key, std::string &&value)
    {
      m_map.emplace(std::move(key), std::move(value));
    }

    void clear() { m_map.clear(); }

    bool empty() const { return m_map.empty(); }

  public:
    struct Visitor
    {
      virtual void operator()(const std::string &, bool) {}
      virtual void operator()(const std::string &, int) {}
      virtual void operator()(const std::string &, double) {}
      virtual void operator()(const std::string &, const std::string &) {}
    };

    void visit(Visitor &visitor) const
    {
      for (const auto &e : m_map)
      {
        const std::string &key = e.first;
        const Variant &value   = e.second;

        switch (value.m_type)
        {
          case Variant::Boolean:
            // visitor.operator()(key, value.m_basic.boolean);
            visitor(key, value.m_basic.boolean);
            break;
          case Variant::Integer:
            visitor.operator()(key, value.m_basic.integer);
            break;
          case Variant::Double:
            visitor.operator()(key, value.m_basic.real);
            break;
          case Variant::String:
            visitor.operator()(key, value.m_string);
            break;
        }
      }
    }
  };
} // namespace cxx_variant_1

/*
// ={=========================================================================

case example to build dbus message, "a(sa{sv})"

{
    // Vistor object that populates the reply dictionary
    struct SystemStatusVisitor : ASVariantMap::Visitor
    {
        sd_bus_message * const reply;

        SystemStatusVisitor(sd_bus_message *msg)
                : reply(msg)
        { }

        void appendDictEntry(const std::string& key, const char *type, const void *value)
        {
            sd_bus_message_open_container(reply, SD_BUS_TYPE_DICT_ENTRY, "sv");
            sd_bus_message_append_basic(reply, 's', key.c_str());

            sd_bus_message_open_container(reply, SD_BUS_TYPE_VARIANT, type);
            sd_bus_message_append_basic(reply, *type, value);
            sd_bus_message_close_container(reply);

            sd_bus_message_close_container(reply);
        }

        void operator()(const std::string& k, bool v) override
        {
            int boolValue = v;
            appendDictEntry(k, "b", &boolValue);
        }
        void operator()(const std::string& k, int v) override
        {
            appendDictEntry(k, "i", &v);
        }
        void operator()(const std::string& k, double v) override
        {
            appendDictEntry(k, "d", &v);
        }
        void operator()(const std::string& k, const std::string& v) override
        {
            appendDictEntry(k, "s", v.c_str());
        }
    };

    // open the dictionary container
    rc = sd_bus_message_open_container(msg, SD_BUS_TYPE_ARRAY, "(sa{sv})");

    // const std::map<std::string, ASVariantMap>& systemStatusUpdate

    for ( auto statusUpdate : systemStatusUpdate ) {

        rc = sd_bus_message_open_container(msg, SD_BUS_TYPE_STRUCT, "sa{sv}");

        // add the arguments
        rc = sd_bus_message_append(msg, "s", statusUpdate.first.c_str());

        // open the dictionary container
        rc = sd_bus_message_open_container(msg, SD_BUS_TYPE_ARRAY, "{sv}");

        // Apply the visitor to all entries in the map, this will populate the
        // dictionary in the reply message
        statusUpdate.second.visit(SystemStatusVisitor(msg));

        // Close the dictionary container
        rc = sd_bus_message_close_container(msg);

        // Close the dictionary container
        rc = sd_bus_message_close_container(msg);
    }

    // Close the dictionary container
    rc = sd_bus_message_close_container(msg);
}
*/

// ={=========================================================================
TEST(CxxTypeVariant, variant_and_visitor_1)
{
  using namespace cxx_variant_1;

  // override "Visitor"
  struct CustomVisitor : VariantMap::Visitor
  {
    std::vector<std::string> coll;

    CustomVisitor() = default;

    virtual void operator()(const std::string &key, bool value) override
    {
      coll.emplace_back("{" + key + ":b}");
    }

    virtual void operator()(const std::string &key, int value) override
    {
      coll.emplace_back("{" + key + ":i}");
    }

    virtual void operator()(const std::string &key, double value) override
    {
      coll.emplace_back("{" + key + ":d}");
    }

    virtual void operator()(const std::string &key,
                            const std::string &value) override
    {
      coll.emplace_back("{" + key + ":" + value + "}");
    }
  };

  VariantMap vmap;
  CustomVisitor visitor;

  vmap.insert("key1", true);
  vmap.insert("key2", false);

  vmap.insert("key3", 3);
  vmap.insert("key4", 4);

  // vmap.insert("key5", "variant1");
  // vmap.insert("key6", "variant2");
  //
  // will print: why? see cxx-bool-conversion-cxx-conversion-to-bool
  //
  // {key5:b}
  // {key6:b}

  vmap.insert("key5", std::string("variant1"));
  vmap.insert("key6", std::string("variant2"));

  // call visit with the custom visitor
  vmap.visit(visitor);

  EXPECT_THAT(visitor.coll.size(), 6);

  // for (const auto e : visitor.coll)
  // {
  //   cout << e << endl;
  // }

  EXPECT_THAT(visitor.coll,
              ElementsAre("{key1:b}",
                          "{key2:b}",
                          "{key3:i}",
                          "{key4:i}",
                          "{key5:variant1}",
                          "{key6:variant2}"));
}

namespace cxx_variant_2
{
  // with operator<<() interface and use std::list<> instead
  class VariantMap
  {
  private:
    struct Variant
    {
      enum Type
      {
        Boolean,
        Integer,
        Double,
        String
      } m_type;

      union Basic
      {
        bool boolean;
        int integer;
        double real;

        Basic() = default;

        explicit Basic(bool b)
            : boolean(b)
        {}
        explicit Basic(int i)
            : integer(i)
        {}
        explicit Basic(double d)
            : real(d)
        {}
      } m_basic;

      std::string m_string;

      explicit Variant(bool b)
          : m_type(Boolean)
          , m_basic(b)
      {}
      explicit Variant(int i)
          : m_type(Integer)
          , m_basic(i)
      {}
      explicit Variant(double d)
          : m_type(Double)
          , m_basic(d)
      {}
      explicit Variant(const std::string &str)
          : m_type(String)
          , m_string(str)
      {}
      explicit Variant(std::string &&str)
          : m_type(String)
          , m_string(std::move(str))
      {}
      explicit Variant(const char *str)
          : m_type(String)
          , m_string(str)
      {}
    }; // struct Variant

    std::list<Variant> m_map;

  public:
    VariantMap()                   = default;
    VariantMap(const VariantMap &) = default;
    VariantMap(VariantMap &&)      = default;

    // write(push) arg from message
    // template <typename T>
    //   DBusMessage &operator<<(const T &arg);

    // write(push) arg from message
    template <typename T>
    VariantMap &operator<<(const T &arg)
    {
      // as changed to use std::list<>
      m_map.emplace_back(arg);
      return *this;
    }

    void clear() { m_map.clear(); }

    bool empty() const { return m_map.empty(); }

  public:
    struct Visitor
    {
      virtual void operator()(bool) {}
      virtual void operator()(int) {}
      virtual void operator()(double) {}
      virtual void operator()(const std::string &) {}
    };

    // void visit(Visitor &&visitor) const
    // this is original code and which means it uses a temporary like:

    void visit(Visitor &visitor) const
    {
      for (const auto &e : m_map)
      {
        const Variant &value = e;

        switch (value.m_type)
        {
          case Variant::Boolean:
            // visitor.operator()(value.m_basic.boolean);
            visitor(value.m_basic.boolean);
            break;
          case Variant::Integer:
            visitor.operator()(value.m_basic.integer);
            break;
          case Variant::Double:
            visitor.operator()(value.m_basic.real);
            break;
          case Variant::String:
            visitor.operator()(value.m_string);
            break;
        }
      }
    }
  };
} // namespace cxx_variant_2

// ={=========================================================================
TEST(CxxTypeVariant, variant_and_visitor_2)
{
  using namespace cxx_variant_2;

  // override "Visitor"
  struct CustomVisitor : VariantMap::Visitor
  {
    std::vector<std::string> coll;

    CustomVisitor() = default;

    virtual void operator()(bool value) override { coll.emplace_back("{b}"); }

    virtual void operator()(int value) override { coll.emplace_back("{i}"); }

    virtual void operator()(double value) override { coll.emplace_back("{d}"); }

    virtual void operator()(const std::string &value) override
    {
      coll.emplace_back("{s:" + value + "}");
    }
  };

  VariantMap vmap;
  CustomVisitor visitor;

  vmap << true;
  vmap << false;

  vmap << 3;
  vmap << 4;

  vmap << "variant1";
  vmap << "variant2";

  // call visit with the custom visitor
  vmap.visit(visitor);

  EXPECT_THAT(visitor.coll.size(), 6);

  EXPECT_THAT(
    visitor.coll,
    ElementsAre("{b}", "{b}", "{i}", "{i}", "{s:variant1}", "{s:variant2}"));
}

// ={=========================================================================
// cxx-variant cxx-17
// #include <variant>
// std::variant is implementation of "one-of"

TEST(CxxTypeVariant, cxx_variant)
{
  {
    // specify types to hold
    std::variant<int, std::string, double> v = 1;

    // v is std::string
    v = "abc";

    // v.index() is used to know which type it currently holds.
    EXPECT_THAT(v.index(), 1);

    // to get values, use type or index
    EXPECT_THAT(std::get<std::string>(v), "abc");
    EXPECT_THAT(std::get<1>(v), "abc");

    // v is double
    v = 3.14;
    EXPECT_THAT(v.index(), 2);

    // to get values, use type or index
    EXPECT_THAT(std::get<double>(v), 3.14);
    EXPECT_THAT(std::get<2>(v), 3.14);
  }

  {
    // if not set value, use ctor of the first type. "int" in this case.
    // also as with std::optional, do not cause dynamic allocation and
    // std::variant size follows the biggest of T param types.
    std::variant<int, std::string, double> v;

    EXPECT_THAT(v.index(), 0);
    EXPECT_THAT(std::get<0>(v), 0);
  }
}

// ={=========================================================================
// cxx-tuple
// Unlike std::variant, can have the same T type in multiple times.

TEST(CxxTypeTuple, make_tuple)
{
  // tup1
  std::tuple<int, float, std::string> tup1{41, 6.3, "nico"};

  EXPECT_THAT(std::get<0>(tup1), 41);
  EXPECT_THAT(std::get<1>(tup1), 6.3);
  EXPECT_THAT(std::get<2>(tup1), "nico");

  // tup2
  auto tup2 = make_tuple(22, 44, "two");

  EXPECT_THAT(std::get<0>(tup2), 22);
  EXPECT_THAT(std::get<1>(tup2), 44);
  EXPECT_THAT(std::get<2>(tup2), "two");

  // assign
  std::get<1>(tup1) = std::get<1>(tup2);

  // see tup1<1> is updated
  EXPECT_THAT(std::get<0>(tup1), 41);
  EXPECT_THAT(std::get<1>(tup1), 44);
  EXPECT_THAT(std::get<2>(tup1), "nico");

  // support compare
  EXPECT_THAT(tup1 > tup2, true);

  // support copy
  tup1 = tup2;

  EXPECT_THAT(std::get<0>(tup1), 22);
  EXPECT_THAT(std::get<1>(tup1), 44);
  EXPECT_THAT(std::get<2>(tup1), "two");
}

/*
// ={=========================================================================
template< class... Types >
tuple<Types&...> tie( Types&... args ) noexcept;
*/

TEST(CxxTypeTuple, tie)
{
  {
    std::tuple<int, float, string> tup1{41, 6.3, "nico"};

    int i;
    float f;
    string s;

    auto tup = make_tuple(std::ref(i), std::ref(f), std::ref(s));

    // if prints out tup here before assigning value, then get undefined value

    // copied
    tup = tup1;

    EXPECT_THAT(get<0>(tup), 41);
    EXPECT_THAT(get<1>(tup), 6.3);
    EXPECT_THAT(get<2>(tup), "nico");

    EXPECT_THAT(i, 41);
    EXPECT_THAT(f, 6.3);
    EXPECT_THAT(s, "nico");

    // when change i,f,s, changes tup as well

    i = 51;
    f = 7.3;
    s = "nico nico";

    EXPECT_THAT(get<0>(tup), 51);
    EXPECT_THAT(get<1>(tup), 7.3);
    EXPECT_THAT(get<2>(tup), "nico nico");

    // more convenient way to access

    EXPECT_THAT(i, 51);
    EXPECT_THAT(f, 7.3);
    EXPECT_THAT(s, "nico nico");
  }

  // cxx-tie do the same
  {
    std::tuple<int, float, string> tup1{41, 6.3, "nico"};

    int i;
    float f;
    string s;

    auto tup = std::tie(i, f, s);

    tup = tup1;

    // shows the same

    EXPECT_THAT(get<0>(tup), 41);
    EXPECT_THAT(get<1>(tup), 6.3);
    EXPECT_THAT(get<2>(tup), "nico");

    EXPECT_THAT(i, 41);
    EXPECT_THAT(f, 6.3);
    EXPECT_THAT(s, "nico");

    // when change i,f,s, changes tup as well

    i = 51;
    f = 7.3;
    s = "nico nico";

    EXPECT_THAT(get<0>(tup), 51);
    EXPECT_THAT(get<1>(tup), 7.3);
    EXPECT_THAT(get<2>(tup), "nico nico");

    // more convenient way to access

    EXPECT_THAT(i, 51);
    EXPECT_THAT(f, 7.3);
    EXPECT_THAT(s, "nico nico");
  }
}

// ={=========================================================================
// cxx-17
// "structured binding" provides easy access than using get<>
TEST(CxxTypeTuple, structured_binding_on_tuple)
{
  {
    std::tuple<int, float, std::string> tup1{41, 6.3, "nico"};
    auto [age, score, name] = tup1;

    EXPECT_THAT(age, 41);
    EXPECT_THAT(score, 6.3);
    EXPECT_THAT(name, "nico");
  }

  // compile error
  // {
  //   auto [age, score, name] = make_tuple{41, 6.3, "nico"};
  // }

  {
    std::tuple<int, float, std::string> tup1{41, 6.3, "nico"};
    auto [age, score, name] = tup1;

    EXPECT_THAT(age, 41);
    EXPECT_THAT(score, 6.3);
    EXPECT_THAT(name, "nico");

    // support assign
    age = 51;

    EXPECT_THAT(age, 51);

    // do not change tup1
    EXPECT_THAT(std::get<0>(tup1), 41);
  }

  // use reference like std::tie
  {
    std::tuple<int, float, std::string> tup1{41, 6.3, "nico"};
    auto &[age, score, name] = tup1;

    EXPECT_THAT(age, 41);
    EXPECT_THAT(score, 6.3);
    EXPECT_THAT(name, "nico");

    age = 51;

    EXPECT_THAT(age, 51);

    // change tup1 as well
    EXPECT_THAT(std::get<0>(tup1), 51);
  }
}

// ={=========================================================================
// cxx-17
// "structured binding" can be used on user type and pair
TEST(CxxTypeTuple, structured_binding_on_other_type)
{
  {
    struct Data
    {
      int age;
      double score;
      std::string name;
    };

    Data data{41, 6.3, "nico"};

    auto [yourage, yourscore, yourname] = data;

    EXPECT_THAT(yourage, 41);
    EXPECT_THAT(yourscore, 6.3);
    EXPECT_THAT(yourname, "nico");
  }

  {
    std::map<int, std::string> coll{{3, "hi"}, {5, "hello"}};

    for (auto &[key, value] : coll)
    {
      std::cout << "{" << key << ", " << value << "}" << std::endl;
    }
  }
}

/*
// ={=========================================================================
cxx-optional cxx-17

https://devblogs.microsoft.com/cppblog/stdoptional-how-when-and-why/

The need for “sometimes-a-thing”

How do you write a function that "optionally" accepts or returns an object? The
traditional solution is to choose one of the potential values as a sentinel to
indicate the absence of a value:

void maybe_take_an_int(int value = -1); // an argument of -1 means "no value"
int maybe_return_an_int();              // a return value of -1 means "no value"

This works reasonably well when one of the representable values of the type
never occurs in practice. It’s less great when there’s no obvious choice of
sentinel and you want to be able to pass all representable values. 


// approach #1
If that’s the case, the typical approach is to use a separate boolean to
indicate whether the optional parameter holds a valid value:

void maybe_take_an_int(int value = -1, bool is_valid = false);
void or_even_better(pair<int,bool> param = std::make_pair(-1, false));

This is also feasible, but awkward. The “two distinct parameters” technique of
maybe_take_an_int requires the caller to pass two things instead of one to
represent a single notion, and fails silently when the caller forgets the bool
and simply calls maybe_take_an_int(42). 


// approach #2
pair<int, bool> maybe_return_an_int();

The use of pair in the other two functions avoids those problems, but it’s
possible for the user of the pair to forget to check the bool and potentially
use a garbage value in the int. Passing std::make_pair(42, true) or
std::make_pair(whatever, false) is also hugely different than passing 42 or
nothing - we’ve made the interface hard to use.


The need for “not-yet-a-thing”

How do you write a class with a member object whose initialization is delayed,
i.e., optionally contains an object? For whatever reason, you do not want to
initialize this member in a constructor. The initialization may happen in a
later mandatory call, or it may happen only on request. When the object is
destroyed the member must be destroyed only if it has been initialized.

Another possible solution to many of the above problems is to dynamically
allocate the “optional” value and pass it via pointer – ideally std::unique_ptr.
Given that we C++ programmers are accustomed to using pointers, this solution
has good usability: a null pointer indicates the no-value condition, * is used
to access the value, std::make_unique<int>(42) is only slightly awkward compared
to return 42 and unique_ptr handles the deallocation for us automatically. Of
course usability is not the only concern; readers accustomed to C++’s
zero-overhead abstractions will immediately pounce upon this solution and
complain that dynamic allocation is orders of magnitude more expensive than
simply returning an integer. 

We’d like to solve this class of problem without requiring dynamic allocation.


You can initialize an optional with an actual T value, or default-initialize it
(or initialize with std::nullopt) to put it in the “empty” state. 

// so std::nullopt is not a value and is something to represent "state"
// compile error
// EXPECT_THAT(value1.value(), std::nullopt);

Returning to the concrete example, your function that looks up a string given an
integer can simply return optional<string>. This avoids the problems of the
suggested solutions; we can

o easily discern the no-value case from the value-found case, unlike for the
  “return a default value” solution,

o report the no-value case without using exception handling machinery, which is
  likely too expensive if such cases are frequent rather than exceptional,


// Like GetValueFromMap example below

o avoid leaking implementation details to the caller as would be necessary to
  expose an “end” iterator with which they could compare a returned iterator.


Solving the delayed initialization problem is straightforward: we simply add an
optional<T> member to our class. The standard library implementer is responsible
for getting the placement new handling correct, and std::optional already
handles all of the special cases for the copy/move constructors/assignment
operators:

using T = // some object type //

struct S {
  bool is_initialized = false;
  alignas(T) unsigned char maybe_T[sizeof(T)];

  void construct_the_T(int arg) {
    assert(!is_initialized);
    new (&maybe_T) T(arg);
    is_initialized = true;
  }

  T& get_the_T() {
    assert(is_initialized);
    return reinterpret_cast<T&>(maybe_T);
  }

  ~S() {
    if (is_initialized) {
      get_the_T().~T(); // destroy the T
    }
  }

  // ... lots of code ...
};


using T = // some object type //

struct S {

  optional<T> maybe_T;    

  // note: not use S() ctor to build a member so "delayed initialization"

  void construct_the_T(int arg) {
    // We need not guard against repeat initialization;
    // optional's emplace member will destroy any 
    // contained object and make a fresh one.        
    maybe_T.emplace(arg);
  }

  T& get_the_T() { 
    assert(maybe_T);
    return *maybe_T;    
    // Or, if we prefer an exception when maybe_T is not initialized:
    // return maybe_T.value();
  }

  // ... No error-prone handwritten special member functions! ...
};


optional is particularly well-suited to the delayed initialization problem
because it is itself an instance of delayed initialization. The contained T may
be initialized at construction, or sometime later, or never. Any contained T
must be destroyed when the optional is destroyed. The designers of optional have
already answered most of the questions that arise in this context.

Conclusions

Any time you need a tool to express “value-or-not-value”, or “possibly an
answer”, or “object with delayed initialization”, you should reach into your
toolbox for std::optional. 

Using a "vocabulary type" for these cases raises the level of abstraction, making
it easier for others to understand what your code is doing. The declarations
optional<T> f(); and void g(optional<T>); express intent more clearly and
concisely than do pair<T, bool> f(); or void g(T t, bool is_valid);. Just as is
the case with words, adding to our vocabulary of types increases our capacity to
describe complex problems simply - it makes us more efficient.


https://www.bfilipek.com/2018/05/using-optional.html

By adding the boolean flag to other types, you can achieve a thing called
“nullable types”. As mentioned, the flag is used to indicate whether the value
is available or not. Such wrapper represents an object that might be empty in an
expressive way (so not via comments :))

While you can achieve “null-ability” by using unique values (-1, infinity,
    nullptr), it’s not as clear as the separate wrapper type. Alternatively, you
could even use std::unique_ptr<Type> and treat the empty pointer as not
initialized - this works, but comes with the cost of allocating memory for the
object.

Optional types - that come from functional programming world - bring type safety
and "expressiveness" Most of other languages have something similar: for example
std::option in Rust, Optional<T> in Java, Data.Maybe in Haskell.

std::optional was added in C++17 and brings a lot of experience from
boost::optional that was available for many years. Since C++17 you can just
#include <optional> and use the type.

Such wrapper is still a value type (so you can copy it, via deep copy). What’s
more, std::optional doesn’t need to allocate any memory on the free store.

std::optional is a part of C++ vocabulary types along with std::any,
  std::variant and std::string_view.


When to use

Usually, you can use an optional wrapper in the following scenarios:

o If you want to represent a nullable type nicely.
  Rather than using unique values (like -1, nullptr, NO_VALUE or something) For
  example, user’s middle name is optional. You could assume that an empty string
  would work here, but knowing if a user entered something or not might be
  important. With std::optional<std::string> you get more information.

o Return a result of some computation (processing) that fails to produce a value
  and is not an error.

  For example finding an element in a dictionary: if there’s no element under a
  key it’s not an error, but we need to handle the situation.

o To perform lazy-loading of resources.

  For example, a resource type has no default constructor, and the construction
  is substantial. So you can define it as std::optional<Resource> (and you can
      pass it around the system), and then load only if needed later.

o To pass optional parameters into functions.

While sometimes the decision to use optional might be blurry, you shouldn’t use
it for error handling. As it best suits the cases when the value is empty and
it’s a normal state of the program.


Examples of std::optional

Here are two a few longer examples where std::optional fits nicely.

User name with an optional nickname and age

#include <iostream>
#include <optional>

class UserRecord
{
  public:
    UserRecord (const std::string& name, std::optional<std::string> nick, std::optional<int> age)
      : mName{name}, mNick{nick}, mAge{age}
    {
    }

    friend std::ostream& operator << (std::ostream& stream, const UserRecord& user);

  private:
    std::string mName;
    std::optional<std::string> mNick;
    std::optional<int> mAge;
};

{
  UserRecord tim { "Tim", "SuperTim", 16 };
  UserRecord nano { "Nathan", std::nullopt, std::nullopt };
}


https://www.bfilipek.com/2018/04/refactoring-with-c17-stdoptional.html
Refactoring with C++17 std::optional

1.

bool CheckSelectionVer1(const ObjSelection &objList, 
                        bool *pOutAnyCivilUnits, 
                        bool *pOutAnyCombatUnits, 
                        int *pOutNumAnimating);

ObjSelection sel;

bool anyCivilUnits { false };
bool anyCombatUnits {false};
int numAnimating { 0 };
if (CheckSelectionVer1(sel, &anyCivilUnits, &anyCombatUnits, &numAnimating))
{
  // ...
}

Cons:

o Look at the caller’s code: we have to create all the variables that will hold
the outputs. For sure it looks like a code duplication if you call the function
is many places.

o Output parameters: Core guidelines suggests not to use them.
F.20: For “out” output values, prefer return values to output parameters

o If you have raw pointers you have to check if they are valid.

o What about extending the function? What if you need to add another output
param?


2. use tuple and structured bindings

std::tuple<bool, bool, bool, int> 
CheckSelectionVer2(const ObjSelection &objList)
{
    if (!objList.IsValid())
        return {false, false, false, 0};

    // local variables:
    int numCivilUnits = 0;
    int numCombat = 0;
    int numAnimating = 0;

    // scan...

    return {true, numCivilUnits > 0, numCombat > 0, numAnimating };
}

auto [ok, anyCivil, anyCombat, numAnim] = CheckSelectionVer2(sel);
if (ok)
{
  // ...
}

Pros:

o No need to check raw pointers
o Code is quite expressive

Cons:

o easy to forget the order of outputs from the tuple.
o when you’d like to add another output value, you have to extend this tuple and
the caller site.


3. use structure

struct SelectionData
{
    bool anyCivilUnits { false };
    bool anyCombatUnits { false };
    int numAnimating { 0 };
};

std::pair<bool, SelectionData> CheckSelectionVer3(const ObjSelection &objList)
{
    SelectionData out;

    if (!objList.IsValid())
        return {false, out};

    // scan...

    return {true, out};
}

if (auto [ok, selData] = CheckSelectionVer3(sel); ok)
{
    // ...
} 

Pros:

o the code is the logical structure and extensibility. If you want to add a new
parameter then just extend the structure.


4. use std::optional

From cppreference - std::optional:

The class template std::optional manages an optional contained value, i.e. a
value that may or may not be present.  A common use case for optional is the
return value of a function that may fail. As opposed to other approaches, such
as std::pair<T,bool>, optional handles "expensive-to-construct objects well" and
is more readable, as the intent is expressed explicitly.

std::optional<SelectionData> CheckSelection(const ObjSelection &objList)
{   
    if (!objList.IsValid())
        return { };

    SelectionData out;   

    // scan...

    return {out};
}


if (auto ret = CheckSelection(sel); ret.has_value())
or
if (auto ret = CheckSelection(sel))
{
    // access via *ret or even ret->
    // ret->numAnimating
}

Pros:

o Clean and expressive form

o Efficient: Implementations of optional are not permitted to use additional
storage, such as dynamic memory, to allocate its contained value. The contained
value shall be allocated in a region of the optional storage suitably aligned
for the type T. Don’t worry about extra memory allocations.


Wrap up

On the other hand, this new implementation omits one important aspect: error
handling. Now, there’s no way to know what was the reason why a value wasn’t
computed. With the previous version, where std::pair was used, we had a chance
to return some error code to indicate the reason.

Here’s what I’ve found in Boost:

  It is recommended to use optional<T> in situations where there is exactly one,
  clear (to all parties) reason for having no value of type T, and where the
  lack of value is as natural as having any regular value of T

In other words, std::optional version looks ok, only when we accept invalid
selection as a “natural” case in the app

// no error handing is required.

*/

namespace cxx_optional
{
  // https://modoocode.com/309

  // When key is not found in the map, returns default ctored std::string
  // However, the problem is that when there is element which has empty string,
  // not able to distinguish between two cases; not found and stting is empty.

  std::string GetValueFromMap_1(const std::map<int, std::string> &m, int key)
  {
    auto it = m.find(key);
    if (it != m.end())
    {
      return it->second;
    }

    return std::string();
  }

  // use std::pair
  // can use boolean flag so that can distinguish. However, still create empty
  // std::string.

  std::pair<std::string, bool>
  GetValueFromMap_2(const std::map<int, std::string> &m, int key)
  {
    auto it = m.find(key);
    if (it != m.end())
    {
      return std::make_pair(it->second, true);
    }

    return std::make_pair(std::string(), false);
  }

  // can use std::optional which can have value or not and not allocate when it
  // don't have value; for example, empty std::string before. The point of using
  // std::optional is it do not allocation when don't have value.

  std::optional<std::string>
  GetValueFromMap_3(const std::map<int, std::string> &m, int key)
  {
    auto it = m.find(key);
    if (it != m.end())
    {
      // ok since std::optional has ctor for T.
      return it->second;
    }

    // std::nullopt is in #include <utility>
    // OK
    // return std::nullopt;
    // OK as well
    return {};
  }
} // namespace cxx_optional

// ={=========================================================================
// cxx-17
TEST(CxxTypeOptional, used_on_return)
{
  using namespace cxx_optional;

  {
    std::map<int, std::string> coll{{1, "hi"}, {2, "hello"}, {3, "hiroo"}};

    auto ret = GetValueFromMap_3(coll, 2);

    EXPECT_THAT(ret.value(), "hello");

    // if calls .value() when don't have, raise std::bad_optional_access
    // exception. so use .has_value() first.

    ret = GetValueFromMap_3(coll, 4);

    EXPECT_THAT(ret.has_value(), false);
  }
}

// ={=========================================================================
TEST(CxxTypeOptional, init)
{
  // default is std::nullopt
  {
    std::optional<int> value1;
    EXPECT_THAT(value1.has_value(), false);

    std::optional<float> value2{std::nullopt};
    EXPECT_THAT(value2.has_value(), false);
  }

  {
    // direct
    std::optional<int> value1{10};
    EXPECT_THAT(value1.has_value(), true);

    // use deduction
    std::optional value2(10);
    EXPECT_THAT(value2.has_value(), true);
  }

  {
    auto value1 = std::make_optional(3.0);
    auto value2 = std::make_optional<std::complex<double>>(3.0, 4.0);
  }

  // The “in_place” construction is also supported in other types like any and
  // variant.
  // and saves the creation of a temporary object.

  {
    std::optional<std::vector<int>> value1{{1, 2, 3}};
    std::optional<std::vector<int>> value2{std::in_place, {1, 2, 3}};
  }
}

// ={=========================================================================
// support cxx-bool-conversion
TEST(CxxTypeOptional, bool_conversion)
{
  {
    // direct
    std::optional<int> value1{10};
    EXPECT_THAT(value1.has_value(), true);

    // direct
    std::optional<int> value2{10};
    EXPECT_THAT(!!value2, true);

    // /home/keitee/git/kb/code-cxx/cxx/test_cxx_1.cpp:2211: Failure
    // Value of: value3
    // Expected: is equal to 8-byte object <01-00 00-00 01-00 00-00>
    //   Actual: 8-byte object <0A-00 00-00 01-00 00-00> (of type std::optional<int>)
    //
    // std::optional<int> value3{10};
    // EXPECT_THAT(value3, true);

    // must be ture on if
    std::optional<int> value4{10};
    if (value4)
      EXPECT_THAT(true, true);
    else
      EXPECT_THAT(true, false);

    // must be false on if since std::nullopt is default
    std::optional<int> value5{};
    if (value5)
      EXPECT_THAT(true, false);
    else
      EXPECT_THAT(true, true);
  }
}

// ={=========================================================================
// NO compile error
//
// TEST(CxxTypeOptional, can_use_reference)
// {
//   std::string coll{"reference"};

//   std::optional<std::string &> value{coll};

//   EXPECT_THAT(value.has_value(), true);

//   EXPECT_THAT(*value, "reference");

//   value.emplace("can use reference");

//   EXPECT_THAT(coll, "can use reference");
// }

// ={=========================================================================
TEST(CxxTypeOptional, operations)
{
  {
    std::optional<std::string> value1{};

    EXPECT_THAT(value1.has_value(), false);

    value1.emplace("value1");

    EXPECT_THAT(value1.has_value(), true);

    // access T
    // a bad_optional_access exception if there is none,
    // and not bother with checking:
    EXPECT_THAT(value1.value(), "value1");
    EXPECT_THAT(*value1, "value1");

    // call dtor and do ctor
    value1.emplace("mark");

    EXPECT_THAT(value1.has_value(), true);

    // call dtor for "mark". value1 = std::nullopt do the same
    value1.reset();
  }

  {
    std::optional<std::string> value1{};

    EXPECT_THAT(value1.has_value(), false);

    // compile error
    // EXPECT_THAT(value1.value(), std::nullopt);

    EXPECT_THAT(value1.value_or("default"), "default");
  }

  {
    std::optional<std::string> value1{};

    // assign new values
    value1.emplace("fred");

    // can use T
    value1 = std::string("joe");
    value1 = "joe";
  }
}

// ={=========================================================================
TEST(CxxTypeOptional, cost)
{
  int value1{};
  EXPECT_THAT(sizeof value1, 4);

  std::optional<int> value2{};
  EXPECT_THAT(sizeof value2, 8);
}

namespace cxx_optional
{
  template <typename Coll, typename Key>
  auto lookup(const Coll &coll, const Key &key)
  {
    auto it = coll.find(key);
    return it != coll.cend() ? std::make_optional(it->second) : std::nullopt;
  }
} // namespace cxx_optional

// ={=========================================================================
TEST(CxxTypeOptional, examples)
{
  // from TEST(StringConversion, string_to_number_max)
  // long max    : 9223372036854775807
  // long min    : -9223372036854775808

  // std::stol uses exception
  {
    EXPECT_THAT(std::stol("9223372036854775806", nullptr, 10),
                9223372036854775806);

    EXPECT_THAT(std::stol("9223372036854775807", nullptr, 10),
                9223372036854775807);

    EXPECT_THROW(std::stol("92233720368547758077", nullptr, 10),
                 std::out_of_range);
  }

  // strtol uses LONG_MAX and errno for overflow cases. use errno to discern the
  // valid cases where input is LONG_MAX.
  //
  // so can use "optional" for this overflow case. not error case and is in
  // "long int" return value range.
  //
  // RETURN VALUE
  // The strtol() function returns the result of the conversion, unless the
  // value would underflow or overflow.
  //
  // If an underflow occurs, strtol() returns LONG_MIN. If an overflow occurs,
  // strtol() returns LONG_MAX.
  //
  // In both cases, errno is set to ERANGE. Precisely the same holds for
  // strtoll() (with LLONG_MIN and LLONG_MAX instead of LONG_MIN and LONG_MAX).

  {
    EXPECT_THAT(std::strtol("9223372036854775806", nullptr, 10),
                9223372036854775806);

    EXPECT_THAT(std::strtol("9223372036854775807", nullptr, 10),
                9223372036854775807);

    // overflow
    EXPECT_THAT(std::strtol("92233720368547758077", nullptr, 10),
                9223372036854775807);
  }

  // Like GetValueFromMap_3(), "cleaner interface"
  // https://www.bfilipek.com/2018/06/optional-examples-wall.html
  // Cleaner interface for map.find

  {
    using namespace cxx_optional;

    const std::map<int, int> squares{{1, 1}, {2, 4}, {3, 9}, {4, 16}};

    if (const auto ret = lookup(squares, 2))
      EXPECT_THAT(*ret, 4);
    else
      std::cout << "unknown square" << std::endl;
  }
}

/*
// ={=========================================================================
cxx-any cxx-17

I believe in a lot of cases we can limit the set of supported types, and that’s
why std::variant might be a better choice. Of course, it gets tricky when you
implement a library without knowing the final applications - so you don’t know
the possible types that will be stored in an object.

*/

// ={=========================================================================
TEST(CxxTypeAny, examples)
{
  std::any a{12};

  // set any value
  a = std::string{"std::any"};
  a = 16;

  // read it as int but not as std::string
  EXPECT_THAT(std::any_cast<int>(a), 16);
  EXPECT_THROW(std::any_cast<std::string>(a), std::bad_any_cast);

  // reset and check if it has any value
  a.reset();
  EXPECT_THAT(a.has_value(), false);

  // can use it in a collection
  std::map<std::string, std::any> coll;
  coll["integer"] = 10;
  coll["string"] = std::string{"Hello"};
  coll["float"] = 1.0f;

  // *cxx-structured-binding* *cxx-typeid*
  for (auto &[key, value] : coll)
  {
    if (value.type() == typeid(int))
      EXPECT_THAT(std::any_cast<int>(value), 10);
    else if (value.type() == typeid(std::string))
      EXPECT_THAT(std::any_cast<std::string>(value), "Hello");
    else if (value.type() == typeid(float))
      EXPECT_THAT(std::any_cast<float>(value), 1.0f);
  }
}

namespace cxx_any
{
  struct MyType
  {
    MyType(int value1, int value2) : value1_(value1), value2_(value2) {}
    int value1_;
    int value2_;
  };
} // namespace cxx_any

// ={=========================================================================
TEST(CxxTypeAny, init)
{
  using namespace cxx_any;

  // default init; it's empty
  {
    std::any a;
    EXPECT_THAT(a.has_value(), false);
  }

  // init with an object
  {
    std::any a1{10};
    EXPECT_THAT(a1.has_value(), true);

    std::any a2{MyType(10,11)};
  }

  // in_place
  {
    std::any a1{std::in_place_type<MyType>, 10, 11};
    std::any a2{std::in_place_type<std::string>, "Hello"};
  }

  // make_any
  {
    std::any a1 = std::make_any<std::string>("Hello");
  }
}

// ={=========================================================================
// to change the currently stored value, two options.
TEST(CxxTypeAny, change_value)
{
  using namespace cxx_any;

  std::any a{};

  // assign
  // The crucial part of being safe for std::any is not to leak any resources.
  // To achieve this behaviour std::any will destroy any active object before
  // assigning a new value.

  a = MyType{10, 11};
  a = std::string{"Hello"};

  // emplace
  a.emplace<float>(1.5f);
  a.emplace<std::vector<int>>({10, 11, 12, 13});
  a.emplace<MyType>(10, 11);
}

/*
// ={=========================================================================
cxx-array cxx-sizeof cxx-init

CPR 114 Explicitly Initialising Array Elements

*/
TEST(CxxArray, init_form)
{
  {
    int arr_1[10]{};
    int arr_1_expected[10]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    // if the dimension is greater than the number of initialisers, any
    // remanining elements are value initialised.

    int arr_2[10]{1};
    int arr_2_expected[10]{1, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    // if we omit the dimension, the compiler infers it from the the number of
    // initialisers

    int arr_3[10]{1};
    int arr_3_expected[]{1, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    EXPECT_THAT(sizeof arr_1_expected, 10);
    EXPECT_THAT(sizeof arr_2_expected, 10);
    EXPECT_THAT(sizeof arr_3_expected, 10);

    EXPECT_THAT(
      memcmp((const void *)arr_1, (const void *)arr_1_expected, sizeof arr_1),
      0);

    EXPECT_THAT(
      memcmp((const void *)arr_2, (const void *)arr_2_expected, sizeof arr_2),
      0);

    EXPECT_THAT(
      memcmp((const void *)arr_3, (const void *)arr_3_expected, sizeof arr_3),
      0);
  }

  // *cxx-uniform-initialization*
  {
    char s2[]{"012345678901234567890"};
    int arr[20]{33};

    // includes a null
    EXPECT_THAT(sizeof(s2), 22);
    EXPECT_THAT(sizeof s2, 22);

    EXPECT_THAT(sizeof(arr), 20 * 4);
    EXPECT_THAT(sizeof arr, 20 * 4);
  }
}

/*
// ={=========================================================================

Also note that in C99 or C11 standards, there is feature called “flexible array
members”, which works same as the above.

But C++ standard (till C++11) doesn’t support variable sized arrays. The C++11
standard mentions array size as a constant-expression See (See 8.3.4 on page 179
of N3337). So the above program may not be a valid C++ program. The program may
work in GCC compiler, because GCC compiler provides an extension to support
them.

https://stackoverflow.com/questions/52363783/c-variable-sized-object-may-not-be-initialized

The problem here is that you're using not one but two extensions.

The first extension, as noted already, is that you're using C99 VLA's in C++.
That's a documented GCC extension.

https://gcc.gnu.org/onlinedocs/gcc/Variable-Length.html

6.20 Arrays of Variable Length

Variable-length automatic arrays are allowed in ISO C99, and as an extension GCC
accepts them in C90 mode and in C++. These arrays are declared like any other
automatic arrays, but with a length that is not a constant expression. The
storage is allocated at the point of declaration and deallocated when the block
scope containing the declaration exits. For example:

*/

TEST(CxxArray, variable_length_automatic_array)
{
  // constant size
  {
    int table[100]{0};

    EXPECT_THAT(sizeof table / sizeof(int), 100);
  }

  // vairable size. However, some compilers emits error:
  // error: variable-sized object ‘arr’ may not be initialized.
  // see above on why.
  {
    int size{100};

    int table[size]{0};

    EXPECT_THAT(sizeof table / sizeof(int), 100);
  }

  // std::vector may be alternative
  {
    int size{100};

    std::vector<int> table(size, 0);

    EXPECT_THAT(table.size(), 100);
  }
}

namespace cxx_array
{
  struct nlist
  {
    struct nlist *next;
    char *name;
    char *defn;
  };
} // namespace cxx_array

// ={=========================================================================
TEST(CxxArray, size)
{
  using namespace cxx_array;

  // array, sizeof, strlen and valid index:
  //
  //   0   1   2   3   4   5
  //   D   S   M   :   !   \0
  //
  // valid array index     [0, 6) [0, size(), sizeof()) or [0, 5] [0, size()-1]
  // valid cstring index   [0, 5) [0, strlen()) or [0, 4] [0, strlen()-1]

  // {
  // cxx.cpp:77:14: warning: deprecated conversion from string constant to
  // ‘char*’ [-Wwrite-strings]
  // char *s1 = "this is first message"; is a pointer
  // }

  // 21 chars, [0, 20]
  char s2[]   = "012345678901234567890";
  int arr[20] = {33};

  {
    // sizeof (type)
    // sizeof expression/object

    EXPECT_EQ(sizeof(s2), 22);
    EXPECT_EQ(sizeof s2, 22);

    // "*s2" is a char object
    EXPECT_THAT(sizeof(*s2), 1);
    EXPECT_THAT(sizeof *s2, 1);
  }

  // array size
  {
    EXPECT_EQ(sizeof(s2), 22);
    EXPECT_EQ(sizeof(s2) / sizeof(s2[0]), 22);

    EXPECT_EQ(sizeof(arr), 20 * 4);
    EXPECT_EQ(sizeof(arr) / sizeof(arr[0]), 20);
  }

  // pointer size
  {
    int *pint{nullptr};
    EXPECT_EQ(sizeof(pint), 8);

    struct nlist *plist;
    EXPECT_EQ(sizeof(plist), 8);

    // 8 or else?
    // Here a pointer points to a 'type' and this is a struct in this case.
    EXPECT_EQ(sizeof(*plist), 24);
  }

  {
    struct EventPayload
    {
      std::string state{};
    };

    EXPECT_THAT(sizeof(EventPayload), 32);
    EXPECT_THAT(sizeof(EventPayload *), 8);
  }

  // todo: cleanup
  {
    const char *s1  = "this is first message";
    const char s2[] = "this is first message";

    EXPECT_THAT(sizeof(s1), 8);                  // pointer
    EXPECT_THAT(sizeof s1, 8);                   // pointer
    EXPECT_THAT(sizeof *s1, 1);                  // byte
    EXPECT_THAT(sizeof(s2), 22);                 // array
    EXPECT_THAT(sizeof(s2) / sizeof(s2[0]), 22); // array
    EXPECT_THAT(strlen(s2), 21);
  }
}

// ={=========================================================================
// cxx-pointer
TEST(CxxArray, access)
{
  int coll[] = {10, 11, 12, 13, 14, 15, 16};

  // array name is a pointer to the first element
  int *arr = coll;

  // access elements via []
  EXPECT_THAT(arr[0], 10);
  EXPECT_THAT(arr[2], 12);
  EXPECT_THAT(arr[4], 14);
  EXPECT_THAT(arr[6], 16);

  // access elements via pointer arithmetic
  EXPECT_THAT(*(arr + 0), 10);
  EXPECT_THAT(*(arr + 2), 12);
  EXPECT_THAT(*(arr + 4), 14);
  EXPECT_THAT(*(arr + 6), 16);

  // Segmentation fault (core dumped) right away when use std::vector but use
  // int array, can survive but still undefined behaviour
  EXPECT_THAT(coll[-1], 0);
  // Value of: coll[-10]
  // Expected: is equal to 0
  //   Actual: 1893152854 (of type int)
  EXPECT_THAT(coll[-10], 0);
}

// ={=========================================================================
// cxx-arith

TEST(Arith, Comparison)
{
  int iv{8};
  double dv{7.2};

  if (iv < dv)
    cout << "double is bigger" << endl;
  else
    cout << "int is bigger" << endl;
}

// ={=========================================================================
// cxx-switch

namespace
{
  enum class VALUES
  {
    E1,
    E2,
    E3,
    E4
  };
}

TEST(CxxExpression, show_switch)
{
#if 0
  // when use "-Wswitch-enum" which is not on by -Wall:
  // warning: enumeration value ‘E3’ not handled in switch [-Wswitch-enum]
  // warning: enumeration value ‘E4’ not handled in switch [-Wswitch-enum]
  {
    ostringstream os;
    VALUES value{VALUES::E1};

    switch (value)
    {
      case VALUES::E1:
        os << "value is 1";
        break;

      case VALUES::E2:
        os << "value is 2";
        break;

      default:
        os << "value is 2";
        break;
    }
  }
#endif

  {
    ostringstream os;
    int value{2};

    switch (value)
    {
      case 1:
        os << "value is 1";
        EXPECT_THAT(value, 1);
        break;

      case 2:
        os << "value is 2";
        EXPECT_THAT(value, 2);
        break;
    }

    EXPECT_THAT(os.str(), "value is 2");
  }

  // the point is that `case 8 | 16:` is a case for single value which is
  // 24. The value of "or" of 8 and 16

  {
    ostringstream os;
    int value{8};

    switch (value)
    {
      case 1:
        os << "value is 1";
        EXPECT_THAT(value, 1);
        break;

      case 8 | 16:
        os << "value is 24";
        EXPECT_THAT(value, 24);
        break;

      default:
        os << "value is default";
        EXPECT_THAT(value, 8);
        break;
    }

    EXPECT_THAT(os.str(), "value is default");
  }

  {
    ostringstream os;
    int value{24};

    switch (value)
    {
      case 1:
        os << "value is 1";
        EXPECT_THAT(value, 1);
        break;

      case 8 | 16:
        os << "value is 24";
        EXPECT_THAT(value, 24);
        break;
    }

    EXPECT_THAT(os.str(), "value is 24");
  }

  // what if there is no default handled? runs no statements.
  {
    ostringstream os;
    int value{30};

    switch (value)
    {
      case 1:
        os << "value is 1";
        EXPECT_THAT(value, 1);
        break;

      case 8 | 16:
        os << "value is 24";
        EXPECT_THAT(value, 24);
        break;
    }

    EXPECT_THAT(os.str(), "");
  }
}

// ={=========================================================================
namespace cxx_if
{
  bool f1(std::string &s, bool flag)
  {
    s += "f1 return true,";
    return flag;
  }

  bool f2(std::string &s, bool flag)
  {
    s += "f2 return true,";
    return flag;
  }

  bool f3(std::string &s, bool flag)
  {
    s += "f3 return true";
    return flag;
  }
} // namespace cxx_if

// ={=========================================================================
// cxx-if
TEST(CxxExpression, if_chain)
{
  using namespace cxx_if;

  // see that a sequence of calls are called if all returns true. that is, only
  // when first "if" fails, second "if" runs.
  {
    std::string ret{};

    if (!f1(ret, true))
      std::cout << "f1 return false" << std::endl;
    else if (!f2(ret, true))
      std::cout << "f2 return false" << std::endl;
    else if (!f3(ret, true))
      std::cout << "f3 return false" << std::endl;

    EXPECT_THAT(ret, "f1 return true,f2 return true,f3 return true");
  }

  {
    std::string ret{};

    if (!f1(ret, false))
      std::cout << "f1 return false" << std::endl;
    else if (!f2(ret, false))
      std::cout << "f2 return false" << std::endl;
    else if (!f3(ret, false))
      std::cout << "f3 return false" << std::endl;

    EXPECT_THAT(ret, "f1 return true,");
  }
}

// ={=========================================================================
// "else if(0 == (e % 2))" means effectively
// "else if((e >= 3) && (0 == (e % 2))) so has "implicit" condition.
TEST(CxxExpression, if_else)
{
  std::vector<int> coll{1, 2, 3, 4, 5, 6, 7};
  std::vector<int> result1{};
  std::vector<int> result2{};

  for (auto e : coll)
  {
    if (e < 3)
      result1.push_back(e);
    else if (0 == (e % 2))
      result2.push_back(e);
  }

  EXPECT_THAT(result2, ElementsAre(4, 6));
}

/*
// ={=========================================================================
cxx-if cxx-17

Init statement for if/switch

New versions of the if and switch statements for C++:

if (init; condition) and switch (init; condition).

variable in init section is visible only inside the if and else statements, so
it doesn't leak.

*/

TEST(CxxExpression, if_init)
{
  if (int value = 1; value)
    EXPECT_THAT(true, true);
}

// ={=========================================================================
TEST(CxxExpression, show_loop)
{
  const std::string input{"A man, a plan, a canal: Panama"};

  // when use while
  {
    std::vector<size_t> coll{};

    size_t start{};

    while (start < 8)
    {
      coll.push_back(start);

      if (!isalnum(input[start]))
      {
        ++start;
        continue;
      }

      ++start;
    }

    EXPECT_THAT(coll, ElementsAre(0, 1, 2, 3, 4, 5, 6, 7));
  }

  // when use for.
  // note that "++start" in for gets executed when do "continue"
  {
    std::vector<size_t> coll{};

    size_t start{};

    for (; start < 8; ++start)
    {
      coll.push_back(start);

      if (!isalnum(input[start]))
      {
        ++start;
        continue;
      }

      // ++start;
    }

    EXPECT_THAT(coll, ElementsAre(0, 1, 3, 4, 5, 7));
  }
}

// ={=========================================================================
// cxx-pair
// typeid : St16initializer_listIKSt4pairIiPKcEE

namespace cxx_pair
{
  template <typename T1, typename T2>
  std::ostream &operator<<(std::ostream &os, std::pair<T1, T2> const &p)
  {
    os << "{" << get<0>(p) << ", " << get<1>(p) << "}";
    return os;
  }
} // namespace cxx_pair

TEST(Pair, MakePair)
{
  // gcc 4.9.2 emits no error but 6.3.0 emits error:
  //
  // cxx.cpp:36:5: error: direct-list-initialization of ‘auto’ requires exactly
  // one element [-fpermissive]
  //
  // };
  // ^
  // cxx.cpp:36:5: note: for deduction to ‘std::initializer_list’, use
  // copy-list-initialization (i.e. add ‘ =’ before the ‘{’)
  //
  // const auto pair_map{
  //     make_pair(10, "X"),
  //     make_pair(9, "IX"),
  //     make_pair(5, "V")
  // };

  {
    const auto pair_map = {make_pair(10, "X"),
                           make_pair(9, "IX"),
                           make_pair(5, "V")};

    auto it = pair_map.begin();
    EXPECT_THAT(it->first, 10);
    EXPECT_THAT(it->second, "X");
  }

  {
    std::vector<std::pair<size_t, bool>> coll{{5, false},
                                              {3, false},
                                              {7, false},
                                              {1, true},
                                              {2, false},
                                              {8, false},
                                              {9, false}};

    auto found_value = std::numeric_limits<size_t>::max();
    decltype(found_value) found_index{}, index{};

    for (auto const &e : coll)
    {
      // if not visited before and see min value

      // if (!e.second && e.first < found_value)
      if (!get<1>(e) && get<0>(e) < found_value)
      {
        found_value = get<0>(e);
        found_index = index;
      }

      ++index;
    }

    EXPECT_THAT(found_value, 2);
    EXPECT_THAT(found_index, 4);
  }

  {
    using namespace cxx_pair;

    auto p = make_pair(80, 88);

    cout << p << endl;
  }
}

// cxx-pair-reference cxx-ref

TEST(Pair, PackReference)
{
  int i{10};
  int j{20};

  // no connetion between i, j and val pair
  {
    auto val = make_pair(i, j);

    ++val.first;
    ++val.second;

    EXPECT_THAT(val.first, 11);
    EXPECT_THAT(val.second, 21);
    EXPECT_THAT(i, 10);
    EXPECT_THAT(j, 20);
  }

  {
    // *cxx-ref*
    auto val = make_pair(std::ref(i), std::ref(j));

    ++val.first;
    ++val.second;

    EXPECT_THAT(val.first, 11);
    EXPECT_THAT(val.second, 21);

    // they are changed now
    EXPECT_THAT(i, 11);
    EXPECT_THAT(j, 21);
  }
}

TEST(Pair, Initialisation)
{
  // initializer_list that has pairs

  // gcc 4.9.2 is fine but 6.3.0 emits error:
  //
  // cxx.cpp:36:5: error: direct-list-initialization of ‘auto’ requires exactly
  // one element [-fpermissive]
  //
  // };
  // ^
  // cxx.cpp:36:5: note: for deduction to ‘std::initializer_list’, use
  // copy-list-initialization (i.e. add ‘ =’ before the ‘{’)
  //
  // const auto pair_map{
  //     make_pair(10, "X"),
  //     make_pair(9, "IX"),
  //     make_pair(5, "V")
  // };

  const auto pair_init_01 = {make_pair(10, "X"),
                             make_pair(9, "IX"),
                             make_pair(5, "V")};

  (void)pair_init_01;

  const auto pair_init_02 = {make_pair(10, "X"),
                             make_pair(9, "IX"),
                             make_pair(5, "V")};

  (void)pair_init_02;

  // vector that has pairs

  std::vector<std::pair<int, string>> pair_init_03{make_pair(10, "X"),
                                                   make_pair(9, "IX"),
                                                   make_pair(5, "V")};

  std::vector<std::pair<int, string>> pair_init_04{{10, "X"},
                                                   {9, "IX"},
                                                   {5, "V"}};

  // vector that has tuples

  std::vector<std::tuple<int, string, int>> tuple_init_01{
    make_tuple(10, "X", 1),
    make_tuple(9, "IX", 2),
    make_tuple(5, "V", 3)};

  // cxx.cpp:165:3: error: converting to ‘std::tuple<int,
  // std::basic_string<char, std::char_traits<char>, std::allocator<char> >,
  // int>’
  //
  // from initializer list would use explicit constructor ‘constexpr std::tuple<
  // <template-parameter-1-1> >::tuple(_UElements&& ...) [with _UElements =
  // {int, const char (&)[2], int}; <template-parameter-2-2> = void; _Elements =
  // {int, std::basic_string<char, std::char_traits<char>, std::allocator<char>
  //
  // >, int}]’
  //    };
  //    ^
  //
  // why error fot both? However, std-pair has no explicit ctors
  //
  // /usr/include/c++/4.9/tuple
  // /// Primary class template, tuple
  // template<typename... _Elements>
  //   class tuple : public _Tuple_impl<0, _Elements...>
  //   {
  //     typedef _Tuple_impl<0, _Elements...> _Inherited;
  //
  //     explicit
  //     constexpr tuple(const _Elements&... __elements)
  //     : _Inherited(__elements...) { }
  //
  // std::vector<std::tuple<int,string, int>> tuple_init_02{
  //   {10, "X", 1},
  //   {9, "IX", 2},
  //   {5, "V", 3}
  // };
  //
  // std::vector<std::tuple<int,string, int>> tuple_init_02 = {
  //   {10, "X", 1},
  //   {9, "IX", 2},
  //   {5, "V", 3}
  // };
}

// see *cxx-pair-comparison*
TEST(CxxPair, check_comparison)
{
  // check on .first
  {
    auto p1 = make_pair(1, 2);
    auto p2 = make_pair(3, 2);

    EXPECT_THAT(p1 > p2, false);
    EXPECT_THAT(p1 < p2, true);
  }

  // if .first is equal and see .second
  {
    auto p1 = make_pair(1, 2);
    auto p2 = make_pair(1, 3);

    EXPECT_THAT(p1 > p2, false);
    EXPECT_THAT(p1 < p2, true);
  }
}

// ={=========================================================================
// cxx-ctor

namespace cxx_ctor
{
#if 0

shows that cannot create object since ctor() is private.

{
  class Base3
  {
  private:
    int value_;

  public:
    ~Base3() = default;

  private:
    Base3()  = default;
  };
} // namespace cxx_copy_control

TEST(CxxCtor, Private)
{
  using namespace cxx_ctor;

  Base3 b1;
}
#endif

  class Base1
  {
  public:
    Base1(const std::string &name) { (void)name; }

  private:
    int value_;
  };

  // okay since uses Ctor1(const std::string &name) ctor
  class User
  {
  public:
    User()
        : member("construction with init list")
    {}

  private:
    Base1 member;
  };
} // namespace cxx_ctor

// ={=========================================================================
// to see that default ctor is necessary
TEST(CxxCtor, show_default_ctor_is_necessary)
{
  // cause compile error
  // error: no matching function for call to ‘cxx_ctor::Base1::Base1()’
  //      Base1 o;
  //
  // since:
  //
  // `If defines any other ctor for the class, the compiler do not generates`
  // default ctor. So if a class requires control to init an object in one case,
  // then the class is likely to require control in all cases.
  //
  // {
  //   using namespace cxx_ctor;
  //   Base1 o;
  // }

  {
    using namespace cxx_ctor;
    Base1 o("another ctor");
  }
}

#if 0 // TO DELETE
// *cxx-unused*
// The only purpose of the parameter is to 'distinguish' prefix from postfix
// function invocation. Many compilers issue warnings if you fail to use named
// parameters in the body of the function to which they apply, and this can be
// annoying. To avoid such warnings, a common strategy is to omit names for
// parameters you don't plan to use; that is what is been done above.

namespace cxx_ctor
{
  class foo
  {
  public:
    explicit foo(int &value)
        : value_(value)
    {
      cout << "foo(int)" << endl;
    }

    foo(int &value, int)
        : value_(value)
    {
      cout << "foo(int, int)" << endl;
    }

  private:
    int value_;
  };
} // namespace cxx_ctor

TEST(CxxCtor, Parameters)
{
  using namespace cxx_ctor;

  int value{10};

  // *cxx-error*
  // : error: cannot bind non-const lvalue reference of type ‘int&’ to an rvalue
  // of type ‘int’ foo f1(10);
  // foo f1(10);

  foo f1(value);
  foo f2(value, 30);
}
#endif

// CPL, 17.4.3 Delegating Constructors
//
// [ RUN      ] CxxCtor.check_delegating_ctor
// CtorCallsCtor(int, string)
// CtorCallsCtor(string)
// CtorCallsCtor(int, string)
// [       OK ] CxxCtor.check_delegating_ctor (0 ms)

namespace cxx_ctor
{
  class CtorCallsCtor
  {
  public:
    CtorCallsCtor(std::string const &name)
        : CtorCallsCtor(10, name)
    {
      std::cout << "CtorCallsCtor(string)" << std::endl;
    }

    CtorCallsCtor(int value, std::string const &name)
        : value_(value)
        , name_(name)
    {
      std::cout << "CtorCallsCtor(int, string)" << std::endl;
    }

    int get_value() const { return value_; }

    std::string get_name() const { return name_; }

  private:
    int value_;
    std::string name_;
  };
} // namespace cxx_ctor

// ={=========================================================================
TEST(CxxCtor, delegating_ctor)
{
  using namespace cxx_ctor;

  CtorCallsCtor o1("cxx-oo");
  EXPECT_THAT(o1.get_value(), 10);
  EXPECT_THAT(o1.get_name(), "cxx-oo");

  CtorCallsCtor o2(10, "cxx-oo");
  EXPECT_THAT(o2.get_value(), 10);
  EXPECT_THAT(o2.get_name(), "cxx-oo");
}

namespace cxx_ctor
{
  class CtorUseDefaultArgument
  {
  public:
    CtorUseDefaultArgument(std::string const &bookNo,
                           unsigned sold    = 10,
                           unsigned revenue = 20)
        : bookNo_(bookNo)
        , units_sold_(sold)
        , revenue_(revenue)
    {}

    // to make it easy to check values
    std::string bookNo_;
    unsigned units_sold_;
    unsigned revenue_;
  };

  class CtorUseInClassInit
  {
  public:
    // CtorUseInClassInit(std::string const &bookNo, unsigned sold, unsigned
    // revenue)
    //   :bookNo_(bookNo), units_sold_(sold), revenue_(revenue)
    // {}

    CtorUseInClassInit(std::string const &bookNo)
        : bookNo_(bookNo)
    {}

    std::string bookNo_;
    unsigned units_sold_{10};
    unsigned revenue_{20};
    unsigned value_{};
  };
} // namespace cxx_ctor

// ={=========================================================================
TEST(CxxCtor, default_argument)
{
  using namespace cxx_ctor;

  {
    CtorUseDefaultArgument o("ctor");

    EXPECT_THAT(o.bookNo_, "ctor");
    EXPECT_THAT(o.units_sold_, 10);
    EXPECT_THAT(o.revenue_, 20);
  }

  {
    CtorUseDefaultArgument o("ctor", 30);

    EXPECT_THAT(o.bookNo_, "ctor");
    EXPECT_THAT(o.units_sold_, 30);
    EXPECT_THAT(o.revenue_, 20);
  }

  {
    CtorUseDefaultArgument o("ctor", 30, 40);

    EXPECT_THAT(o.bookNo_, "ctor");
    EXPECT_THAT(o.units_sold_, 30);
    EXPECT_THAT(o.revenue_, 40);
  }

  // error since there is no such ctor
  //
  // {
  //   // error: no matching function for call to
  //   // ‘cxx_ctor::CtorUseInClassInit::CtorUseInClassInit(const char [5])’
  //
  //   CtorUseInClassInit o("ctor");
  //
  //   EXPECT_THAT(o.bookNo_, "ctor");
  //   EXPECT_THAT(o.units_sold_, 10);
  //   EXPECT_THAT(o.revenue_, 20);
  // }

  // works now when changes ctor. so note that cxx-inclass-init has the same
  // effect as cxx-default-argument.

  {
    CtorUseInClassInit o("ctor");

    EXPECT_THAT(o.bookNo_, "ctor");
    EXPECT_THAT(o.units_sold_, 10);
    EXPECT_THAT(o.revenue_, 20);
    EXPECT_THAT(o.value_, 0);
  }
}

// ={=========================================================================
// cxx-ctor-init-forms

// To sum, cxx-init-copy-form uses copy ctor which is counted as implicit
// conversion. So it cannot use with explicit copy ctor and in cases more
// conversion since only one conversion is allowed at a time.

namespace ctor_init_1
{
  class Foo
  {
  public:
    Foo()
        : mesg_()
    {}

    // mesg_ is updated only here
    Foo(const std::string &mesg)
        : mesg_(mesg)
    {
      os_ << mesg_ << "+string converting ctor";
      mesg_ = os_.str();
    }

    // copy-ctor
    Foo(const Foo &foo)
    {
      mesg_ = foo.mesg_;
      os_ << mesg_ << "+copy ctor";
      mesg_ = os_.str();
    }

    // // added
    // Foo(const char *mesg)
    //     : mesg_(mesg)
    // {
    //   os_ << mesg_ << " and char converting ctor";
    // }

    // copy-assign
    Foo &operator=(Foo const &foo)
    {
      mesg_ = foo.mesg_;
      os_ << mesg_ << "+copy assign";
      return *this;
    }

    std::string return_mesg() { return os_.str(); }

  private:
    std::ostringstream os_{};
    std::string mesg_{};
  };
} // namespace ctor_init_1

namespace ctor_init_2
{
  class Foo
  {
  public:
    Foo()
        : mesg_()
    {}

    // mesg_ is updated only here
    Foo(const string &mesg)
        : mesg_(mesg)
    {
      os_ << mesg_ << "+string converting ctor";
      mesg_ = os_.str();
    }

    // copy-ctor
    Foo(const Foo &foo)
    {
      mesg_ = foo.mesg_;
      os_ << mesg_ << "+copy ctor";
      mesg_ = os_.str();
    }

    // added
    Foo(const char *mesg)
        : mesg_(mesg)
    {
      os_ << mesg_ << "+char converting ctor";
    }

    // copy-assign
    Foo &operator=(Foo const &foo)
    {
      mesg_ = foo.mesg_;
      os_ << mesg_ << "+copy assign";
      return *this;
    }

    string return_mesg() { return os_.str(); }

  private:
    ostringstream os_{};
    string mesg_{};
  };
} // namespace ctor_init_2

// ={=========================================================================
TEST(CxxCtor, check_init_forms)
{
  // direct init. conversion from char * to string before calling ctor and which
  // is "Foo(const string &mesg)" since it's best match
  {
    using namespace ctor_init_1;

    Foo foo1("+direct init");
    EXPECT_THAT(foo1.return_mesg(), "+direct init+string converting ctor");
  }

  // direct init and no conversion since there is "Foo(const char *mesg)" and
  // it's best match
  {
    using namespace ctor_init_2;

    // conversion from char * to string before calling ctor
    Foo foo1("+direct init");
    EXPECT_THAT(foo1.return_mesg(), "+direct init+char converting ctor");
  }

  {
    using namespace ctor_init_1;

    Foo foo1("+direct init"); // direct and conversion
    Foo foo2(foo1);           // direct and copy ctor
    EXPECT_THAT(foo2.return_mesg(),
                "+direct init+string converting ctor+copy ctor");
  }
  {
    using namespace ctor_init_1;
    Foo foo1("+copy init");
    Foo foo2 = foo1;
    EXPECT_THAT(foo2.return_mesg(),
                "+copy init+string converting ctor+copy ctor");
  }

  {
    using namespace ctor_init_1;
    Foo foo1{"+copy init"};
    Foo foo2 = foo1;
    EXPECT_THAT(foo2.return_mesg(),
                "+copy init+string converting ctor+copy ctor");
  }

  // NOTE: do not call "copy ctor" since uses move which is synthesized.
  {
    using namespace ctor_init_1;
    Foo foo2 = Foo{"+copy init"}; // copy-init and copy ctor
    EXPECT_THAT(foo2.return_mesg(), "+copy init+string converting ctor");
  }

  // error since "only one conversion is allowed for copy-init form"
  // cxx.cpp:533:16: error: conversion from ‘const char [14]’ to non-scalar
  // type ‘ctor_init::Foo’ requested
  //       Foo foo2 = "use copy init";
  //                  ^
  // {
  //   Foo foo2 = "use copy init";
  //   EXPECT_THAT(foo2.return_mesg(), "use copy init and copy ctor");
  // }

  // uses move and why not error? since has "Foo(const char *mesg)" and only one
  // conversion which is "char *" to Foo.
  {
    using namespace ctor_init_2;

    {
      Foo foo2 = "+copy init";
      EXPECT_THAT(foo2.return_mesg(), "+copy init+char converting ctor");
    }
  }
}

// has "explicit" keyword
namespace ctor_init_explicit
{
  class Foo
  {
  public:
    Foo()
        : mesg_()
    {}

    explicit Foo(const string &mesg)
        : mesg_(mesg)
    {
      os_ << mesg_ << "+string converting ctor";
      mesg_ = os_.str();
    }

    explicit Foo(const Foo &foo)
    {
      mesg_ = foo.mesg_;
      os_ << mesg_ << "+copy ctor";
      mesg_ = os_.str();
    }

    // added
    explicit Foo(const char *mesg)
        : mesg_(mesg)
    {
      os_ << mesg_ << "+char converting ctor";
    }

    string return_mesg() { return os_.str(); }

  private:
    ostringstream os_{};
    string mesg_{};
  };
} // namespace ctor_init_explicit

// ={=========================================================================
TEST(CxxCtor, check_explicit_init)
{
  using namespace ctor_init_explicit;

  // copy
  {
    // conversion from char * to string before calling ctor
    Foo foo1("+direct init");
    Foo foo2(foo1);
    EXPECT_THAT(foo2.return_mesg(), "+direct init+copy ctor");
  }

  // // copy
  // {
  //   Foo foo1("use copy init");
  //
  //  when use explicit Foo(const Foo &foo);
  //
  //   // cxx.cpp:628:16: error: no matching function for call to
  //   // ‘ctor_init_explicit::Foo::Foo(ctor_init_explicit::Foo&)’
  //
  //   Foo foo2 = foo1;
  //   EXPECT_THAT(foo2.return_mesg(), "use copy init and copy ctor");
  // }

  // copy error
  // // cxx.cpp:533:16: error: conversion from ‘const char [14]’ to non-scalar
  // type ‘ctor_init::Foo’ requested
  // //      Foo foo2 = "use copy init";
  // //                 ^
  // {
  //   Foo foo2 = "use copy init";
  //   EXPECT_THAT(foo2.return_mesg(), "use copy init and copy ctor");
  // }
}

namespace ctor_init_parent_and_child
{
  class Parent;

  class Child
  {
  private:
    const Parent *m_parent{nullptr};
    std::string m_name{};

  public:
    explicit Child(Parent *parent)
        : m_parent{parent}
        , m_name{"child"}
    {
      std::cout << "name : " << m_name << std::endl;
    }
  };

  class Parent
  {
  private:
    std::string m_name{};
    std::unique_ptr<Child> m_child{};

  public:
    explicit Parent()
        : m_child(std::make_unique<Child>(this))
        , m_name{"parent"}
    {
      std::cout << "name : " << m_name << std::endl;
    }
  };
} // namespace ctor_init_parent_and_child

// ={=========================================================================
// [ RUN      ] CxxCtor.check_parent_and_child
// name : child
// name : parent
// [       OK ] CxxCtor.check_parent_and_child (0 ms)
TEST(CxxCtor, check_parent_and_child)
{
  using namespace ctor_init_parent_and_child;

  auto parent = std::make_shared<Parent>();
}

/*
={=========================================================================
cxx-init-list
*/
namespace cxx_init_list
{
  class Foo
  {
  public:
    Foo()
        : mesg_()
    {}

    // mesg_ is updated only here
    Foo(const std::string &mesg)
        : mesg_(mesg)
    {
      os_ << mesg_ << " and converting ctor";
      mesg_ = os_.str();
    }

    Foo(const Foo &foo)
    {
      mesg_ = foo.mesg_;
      os_ << mesg_ << " and copy ctor";
      mesg_ = os_.str();
    }

    Foo(std::initializer_list<std::string> values)
    {
      for (auto &e : values)
        os_ << e << ", ";
    }

    std::string return_mesg() { return os_.str(); }

  private:
    std::ostringstream os_{};
    std::string mesg_{};
  };

  // to show that it can be used in a function and return vector than print out

  // *cxx-const* *cxx-temporary* *cxx-reference*

  std::vector<string>
  error_message_1(std::string const &message,
                  const std::initializer_list<std::string> &ls)
  {
    std::vector<string> coll{};

    coll.push_back(message);

    for (auto const &e : ls)
      coll.push_back(e);

    return coll;
  }

  // const is must to use temporary and if not, non-const compile error

  std::vector<int> error_message_2(const std::initializer_list<int> &ls)
  {
    std::vector<int> coll{};

    for (auto const &e : ls)
      coll.push_back(e);

    return coll;
  }

  // no error when use tempoarary since it not use reference

  std::vector<int> error_message_3(std::initializer_list<int> ls)
  {
    std::vector<int> coll{};

    for (auto const &e : ls)
      coll.push_back(e);

    return coll;
  }

  Foo returnFoo() { return {"one", "two", "three"}; }
} // namespace cxx_init_list

// ={=========================================================================
TEST(CxxCtor, check_init_list_1)
{
  using namespace cxx_init_list;

  // use list init
  {
    std::vector<int> coll{1, 2, 3, 4, 5};
    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4, 5));
  }

  // since it is Foo's vector, each of vector is constructed by using converting
  // and copy ctor
  {
    std::vector<Foo> coll{Foo("one"), Foo("two"), Foo("three")};
    EXPECT_THAT(coll[0].return_mesg(), "one and converting ctor and copy ctor");
  }

  // since only one conversion is allowed
  {
    std::vector<Foo> coll{std::string("one"),
                          std::string("two"),
                          std::string("three")};
    EXPECT_THAT(coll[0].return_mesg(), "one and converting ctor and copy ctor");
  }

  // use functor
  // std::string return_mesg() { return os_.str(); }
  {
    Foo foo("one");
    std::vector<std::function<std::string(Foo &)>> coll{&Foo::return_mesg};
    EXPECT_THAT(coll[0](foo), "one and converting ctor");
  }

  // shows how to use list init in function
  {
    // when no Foo(std::initializer_list<std::string> values);
    //
    // cxx.cpp:719:35: error: no matching function for call to
    // ‘ctor_init_list::Foo::Foo(<brace-enclosed initializer list>)’
    //
    //      Foo foo1{"one", "two", "three"};

    Foo foo1{"one", "two", "three"};
    EXPECT_THAT(foo1.return_mesg(), "one, two, three, ");
  }

  {
    EXPECT_THAT(
      error_message_1("error", {string("one"), string("two"), string("three")}),
      ElementsAre("error", "one", "two", "three"));
  }

  {
    EXPECT_THAT(error_message_2({1, 2, 3, 4, 5}), ElementsAre(1, 2, 3, 4, 5));
  }

  {
    EXPECT_THAT(error_message_3({1, 2, 3, 4, 5}), ElementsAre(1, 2, 3, 4, 5));
  }

  // see that brace-init can be used to return Foo without specifying it.
  {
    auto foo1 = returnFoo();
    EXPECT_THAT(foo1.return_mesg(), "one, two, three, ");
  }
}

namespace cxx_init_list
{
  int equi_index_1(const int A[], int n) { return 3; }

  int equi_index_2(const std::vector<int> &A) { return 3; }
} // namespace cxx_init_list

// ={=========================================================================
TEST(CxxCtor, check_init_list_2)
{
  using namespace cxx_init_list;

  // OK
  {
    const std::vector<std::pair<int, std::string>> table = {
      std::make_pair(1000, "M"),
      std::make_pair(900, "CM"),
      std::make_pair(500, "D")};
  }

  // OK
  {
    const auto table = {std::make_pair(1000, "M"),
                        std::make_pair(900, "CM"),
                        std::make_pair(500, "D")};
  }

  // error: direct-list-initialization of ‘auto’ requires
  // exactly one element [-fpermissive]
  //
  // {
  //   const auto table{
  //     std::make_pair(1000, "M"),
  //     std::make_pair(900, "CM"),
  //     std::make_pair(500, "D")
  //   };
  // }

  // error: unable to deduce ‘const std::initializer_list<const auto>’
  // from ‘{{1000, "M"},
  //
  // {
  //   const auto table{
  //     {1000, "M"},
  //     {900, "CM"},
  //     {500, "D"}
  //   };
  // }

  // OK
  {
    const std::initializer_list<std::pair<unsigned int, std::string>> table = {
      std::make_pair(1001, "M"),
      std::make_pair(900, "CM"),
      std::make_pair(500, "D")};
  }

  // OK
  {
    const std::initializer_list<std::pair<unsigned int, std::string>> table{
      std::make_pair(1001, "M"),
      std::make_pair(900, "CM"),
      std::make_pair(500, "D")};
  }

  // when use function
  // error: on define function<> when has array argument
  // std::vector<std::function<int(const int[], int)>> imps{equi_index_1};
  //
  // HOW?
  {
    auto imps = {equi_index_1};

    const int coll[]{-7, 1, 5, 2, -4, 3, 0};

    for (const auto &f : imps)
    {
      EXPECT_THAT(f(coll, 7), 3);
    }
  }

  // OK
  {
    auto imps = {equi_index_2};

    const std::vector<int> coll{-7, 1, 5, 2, -4, 3, 0};

    for (const auto &f : imps)
    {
      EXPECT_THAT(f(coll), 3);
    }
  }

  // OK, same
  {
    std::initializer_list<std::function<int(const std::vector<int> &)>> imps = {
      equi_index_2};

    const std::vector<int> coll{-7, 1, 5, 2, -4, 3, 0};

    for (const auto &f : imps)
    {
      EXPECT_THAT(f(coll), 3);
    }
  }
}

/*
={=========================================================================
C++ Primer 99

List Initializer or Element Count?

In a few cases, what initialization means depends on whether we use "curly
braces" or "parenthesies" to pass the initilizer(s).

When use parentheses, are saying that the values we supply are to be used to
"construct" the object.

When use brace, are saying that, "if possible", we want to "list initialize" the
object.

If use braces and there is "no way" to use the initializers to "list initialize"
the object, then those values will be used to "construct" the object.

*/

TEST(CxxCtor, init_list_3)
{
  // Q: represent vector's size or element value?
  //
  // /**
  //  *  @brief  Creates a %vector with default constructed elements.
  //  *  @param  __n  The number of elements to initially create.
  //  *  @param  __a  An allocator.
  //  *
  //  *  This constructor fills the %vector with @a __n default
  //  *  constructed elements.
  //  */
  // vector(size_type __n, const allocator_type& __a = allocator_type());
  {
    // size
    std::vector<int> coll1(10);
    EXPECT_THAT(coll1.size(), 10);

    // value. use list init version
    std::vector<int> coll2{10};
    EXPECT_THAT(coll2.size(), 1);

    // value. use list init version

    // NOTE: it's now fails on gcc (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0
    // but it is okay before. means that it is now to use list init
    //
    std::vector<int> coll3{10};
    EXPECT_THAT(coll2.size(), Ne(10));
  }

  // /**
  //  *  @brief  Creates a %vector with copies of an exemplar element.
  //  *  @param  __n  The number of elements to initially create.
  //  *  @param  __value  An element to copy.
  //  *  @param  __a  An allocator.
  //  *
  //  *  This constructor fills the %vector with @a __n copies of @a __value.
  //  */
  //
  // /**
  //  *  @brief  Builds a %vector from an initializer list.
  //  *  @param  __l  An initializer_list.
  //  *  @param  __a  An allocator.
  //  *
  //  *  Create a %vector consisting of copies of the elements in the
  //  *  initializer_list @a __l.
  //  *
  //  *  This will call the element type's copy constructor N times
  //  *  (where N is @a __l.size()) and do no memory reallocation.
  //  */
  // vector(initializer_list<value_type> __l,
  // const allocator_type& __a = allocator_type());
  {
    // size, init value
    std::vector<int> coll1(10, 1);
    EXPECT_THAT(coll1.size(), 10);

    // value. use list init version
    std::vector<int> coll2{10, 1};
    EXPECT_THAT(coll2.size(), 2);
  }

  {
    // error: invalid conversion from ‘const char*’ to
    // ‘std::vector<std::__cxx11::basic_string<char> >::size_type {aka long unsigned int}’
    //
    // since among vector ctors, it matches to
    //
    // vector(size_type __n, const allocator_type& __a = allocator_type());
    //
    // but cannot convert to "size_type"
    //
    // std::vector<std::string> coll1("hi");

    std::vector<std::string> coll2{"hi"};
    EXPECT_THAT(coll2.size(), 1);
  }

  // uses
  // vector(size_type __n, const allocator_type& __a = allocator_type());
  {
    // why not list init? since T is string?
    std::vector<std::string> coll1{10};
    EXPECT_THAT(coll1.size(), 10);

    std::vector<std::string> coll2{10, "hi"};
    EXPECT_THAT(coll2.size(), 10);
  }
}

// cxx-ctor-access

namespace cxx_ctor_access
{
  class Bar
  {
  public:
    Bar()
        : mesg_()
    {}

    Bar(string const &mesg)
        : mesg_(mesg)
    {}

    // copy ctor
    Bar(Bar const &bar)
    {
      mesg_ = bar.mesg_;
      os_ << mesg_ << " and copy ctor";
    }

    string return_mesg() { return os_.str(); }

  private:
    ostringstream os_;
    string mesg_;
  };

  class Foo
  {
  public:
    Foo()
        : mesg_()
    {}

    Foo(string const &mesg)
        : mesg_(mesg)
    {}

    // copy ctor without const
    Foo(Foo &foo)
    {
      mesg_ = foo.mesg_;
      os_ << mesg_ << " and copy ctor";
      foo.mesg_ = "xxx";
    }

    // raise error only when that's different type:
    //
    // cxx.cpp: In constructor
    // ‘cxx_ctor_access::Foo::Foo(cxx_ctor_access::Bar&)’: cxx.cpp:835:14:
    // error: ‘std::string cxx_ctor_access::Bar::mesg_’ is private
    //        string mesg_;
    //               ^
    // cxx.cpp:855:13: error: within this context
    //          bar.mesg_ = "xxx";
    //              ^

    Foo(Bar &bar)
    {
      (void)bar;
      // both cause errors
      // mesg_ = bar.mesg_;
      // bar.mesg_ = "xxx";
    }

    string return_mesg() { return os_.str(); }

  private:
    ostringstream os_;
    string mesg_;
  };

} // namespace cxx_ctor_access

// ={=========================================================================
TEST(Ctor, Access)
{
  using namespace cxx_ctor_access;

  Foo foo1{"foo1"};

  Foo foo2(foo1);
  EXPECT_THAT(foo2.return_mesg(), "foo1 and copy ctor");
}

// ={=========================================================================
// cxx-dtor

namespace cxx_dtor
{

  class NoVirtualDtorBase
  {
  public:
    NoVirtualDtorBase() { std::cout << "\tno virtual ctor: base" << std::endl; }

    ~NoVirtualDtorBase()
    {
      std::cout << "\tno virtual dtor: base" << std::endl;
    }

  private:
    int base_;
  };

  class DerivedFromNoVirtual : public NoVirtualDtorBase
  {
  public:
    DerivedFromNoVirtual()
    {
      std::cout << "\tno virtual ctor: derived" << std::endl;
    }

    ~DerivedFromNoVirtual()
    {
      std::cout << "\tno virtual dtor: derived" << std::endl;
    }

  private:
    int derived_;
  };

  class VirtualDtorBase
  {
  public:
    VirtualDtorBase() { std::cout << "\tvirtual ctor: base" << std::endl; }

    virtual ~VirtualDtorBase()
    {
      std::cout << "\tvirtual dtor: base" << std::endl;
    }

  private:
    int value_;
  };

  class DerivedFromVirtual : public VirtualDtorBase
  {
  public:
    DerivedFromVirtual()
    {
      std::cout << "\tvirtual ctor: derived" << std::endl;
    }

    ~DerivedFromVirtual()
    {
      std::cout << "\tvirtual dtor: derived" << std::endl;
    }

  private:
    int derived_;
  };

} // namespace cxx_dtor

// {
//         no virtual ctor: base
//         no virtual ctor: derived
//         no virtual dtor: base
// }
// {
//         virtual ctor: base
//         virtual ctor: derived
//         virtual dtor: derived
//         virtual dtor: base
// }
// {
//         no virtual ctor: base
//         no virtual ctor: derived
// }
//         no virtual dtor: derived
//         no virtual dtor: base

TEST(Dtor, NoVirtualDtorProblem)
{
  using namespace cxx_dtor;

  {
    cout << "{" << endl;
    NoVirtualDtorBase *pbase = new DerivedFromNoVirtual;

    // https://stackoverflow.com/questions/7403883/derived-class-with-non-virtual-destructor
    // If you don't do delete in the above manner, then it will be fine.
    delete pbase;

    cout << "}" << endl;
  }

  {
    cout << "{" << endl;
    VirtualDtorBase *pbase = new DerivedFromVirtual;
    delete pbase;
    cout << "}" << endl;
  }

  {
    cout << "{" << endl;
    DerivedFromNoVirtual object;
    cout << "}" << endl;
  }
}

// Why dtor should be virtual in cxx-abc?

namespace cxx_dtor
{

  class AbstractBase
  {
  public:
    AbstractBase()
        : base_(0)
    {
      std::cout << "\tabstract ctor: base" << std::endl;
    }

    ~AbstractBase() { std::cout << "\tabstract dtor: base" << std::endl; }

    virtual int get_value() = 0;

  private:
    int base_;
  };

  class DerivedFromAbstract : public AbstractBase
  {
  public:
    DerivedFromAbstract()
        : derived_(10)
    {
      std::cout << "\tabstract ctor: derived" << std::endl;
    }

    ~DerivedFromAbstract()
    {
      std::cout << "\tabstract dtor: derived" << std::endl;
    }

    virtual int get_value() override { return derived_; };

  private:
    int derived_;
  };

  class AbstractBaseNoDtor
  {
  public:
    AbstractBaseNoDtor()
        : base_(0)
    {
      std::cout << "\tabstract ctor: base" << std::endl;
    }

    // ~AbstractBase()
    // { std::cout << "\tabstract dtor: base" << std::endl; }

    virtual int get_value() = 0;

  private:
    int base_;
  };

  class DerivedFromAbstractNoDtor : public AbstractBaseNoDtor
  {
  public:
    DerivedFromAbstractNoDtor()
        : derived_(10)
    {
      std::cout << "\tabstract ctor: derived" << std::endl;
    }

    ~DerivedFromAbstractNoDtor()
    {
      std::cout << "\tabstract dtor: derived" << std::endl;
    }

    virtual int get_value() override { return derived_; };

  private:
    int derived_;
  };

} // namespace cxx_dtor

TEST(Dtor, AbstractBaseClassNoCompileError)
{
  {
    using namespace cxx_dtor;

    DerivedFromAbstract *pabc1;
    DerivedFromAbstract *pabc2;

    (void)pabc1;
    (void)pabc2;
  }

  {
    using namespace cxx_dtor;

    DerivedFromAbstract abc;
  }

  {
    using namespace cxx_dtor;

    DerivedFromAbstractNoDtor *pabc1;
    DerivedFromAbstractNoDtor *pabc2;

    (void)pabc1;
    (void)pabc2;
  }

  {
    using namespace cxx_dtor;

    DerivedFromAbstract abc;
  }
}

// shows *cxx-dtor-non-virtual-destruction-problem*

TEST(Dtor, AbstractBaseClassNeedVirtualDtor)
{
  using namespace cxx_dtor;

  cout << "{" << endl;
  AbstractBaseNoDtor *pbase = new DerivedFromAbstractNoDtor;

  // cxx.cpp:1108:10: warning: deleting object of abstract class type
  // ‘cxx_dtor::AbstractBaseNoDtor’ which has non-virtual destructor will cause
  // undefined behaviour [-Wdelete-non-virtual-dtor]
  //    delete pbase;

  delete pbase;
  cout << "}" << endl;
}

// ={=========================================================================
// cxx-copy-control
// use ifdef since this test causes compile error

#if 0
namespace cxx_copy_control
{
  class CopyControlBase1
  {
  private:
    int value_;

  public:
    CopyControlBase1()  = default;
    ~CopyControlBase1() = default;

  private:
    CopyControlBase1(const CopyControlBase1 &base)
    {
      (void)base;
      cout << "copy-ctor: base" << endl;
    } // @11

    CopyControlBase1 &operator=(const CopyControlBase1 &base)
    {
      (void)base;
      cout << "copy-assign: base" << endl;
      return *this;
    }
  };

  class Derived1 : public CopyControlBase1
  {
  public:
    void print() { cout << "derived get shout" << endl; };
  };

  class CopyControlBase2
  {
  private:
    int value_;

  public:
    CopyControlBase2()  = default;
    ~CopyControlBase2() = default;

  public:
    CopyControlBase2(const CopyControlBase2 &base) = delete;
    CopyControlBase2 &operator=(const CopyControlBase2 &base) = delete;
  };

  class Derived2 : public CopyControlBase2
  {
  public:
    void print() { cout << "derived get shout" << endl; };
  };
} // namespace cxx_copy_control

TEST(CxxCopyControl, PrivateAndDelete)
{
  using namespace cxx_copy_control;

  {
    CopyControlBase1 b1, b2;

    // error: ‘cxx_copy_control::CopyControlBase1::CopyControlBase1
    // (const cxx_copy_control::CopyControlBase1&)’
    // is private within this context
    CopyControlBase1 b3(b1);
  }

  {
    CopyControlBase2 b1, b2;

    // error: use of deleted function
    // ‘cxx_copy_control::CopyControlBase2::CopyControlBase2
    // (const cxx_copy_control::CopyControlBase2&)’
    CopyControlBase2 b3(b1);
  }

  {
    CopyControlBase2 b1(100);
    CopyControlBase2 b2;

    // error: use of deleted function ‘void
    // copy_control_assign::CopyControlBaseUseDelete::operator=(const
    // copy_control_assign::CopyControlBaseUseDelete&)’
    //    d2 = d1;
    //       ^

    b2 = b1;
  }

  {
    Derived2 b1, b2;

    // error: use of deleted function
    // ‘cxx_copy_control::Derived2::Derived2(const cxx_copy_control::Derived2&)’
    //      Derived2 b3(b1);
    //                    ^
    // note: ‘cxx_copy_control::Derived2::Derived2(const
    // cxx_copy_control::Derived2&)’ is implicitly deleted because the default
    // definition would be ill-formed:
    //    class Derived2 : public CopyControlBase2
    //          ^~~~~~~~
    // error: use of deleted function
    // ‘cxx_copy_control::CopyControlBase2::CopyControlBase2(const
    // cxx_copy_control::CopyControlBase2&)’ note: declared here
    //      CopyControlBase2(const CopyControlBase2 &base) = delete;
    //      ^~~~~~~~~~~~~~~~

    Derived2 b3(b1);
  }
}
#endif

// ={=========================================================================
// cxx-enum

namespace cxxenum
{
  // unscoped
  // enum { red, yellow, green } color;       // same as above so cause error
  enum color
  {
    red,
    yellow,
    green
  };

  // scoped
  enum class peppers
  {
    yellow,
    red,
    green
  };
} // namespace cxxenum

TEST(CxxEnum, check_scoped_and_unscoped)
{
  using namespace cxxenum;

  // explicitly use unscoped enumerators and implicitly use unscoped
  {
    int value_1 = color::yellow;
    int value_2 = yellow;
    EXPECT_THAT(value_1, value_2);
  }

  // both cases the compile cxx-error
  //
  // {
  //   enum color { red, yellow, green };          // unscoped
  //   enum {red, yellow, green};                  // unscoped and unnamed
  // }
  //
  // {
  //   enum color { red, yellow, green };        // unscoped
  //   enum stoplight { red, yellow, green };    // unscoped
  // }
  //
  // enum {red, yellow, green}; causes errors for all enumerators
  //
  // cxx.cpp:1497:9: error: redeclaration of ‘red’
  //    enum {red, yellow, green};                  // unscoped and unnamed
  //          ^~~
  // cxx.cpp:1495:16: note: previous declaration ‘cxx_enum::color red’
  //    enum color { red, yellow, green };          // unscoped
  //                 ^~~

  // so can use enumerators of unscoped enum and this allows *cxx-enum-hack*
  // which is the implicit conversion to an integer
  {
    // to integer
    int value_1 = yellow;
    int coll[green];

    EXPECT_THAT(8, sizeof(coll));

    // error from integer to enum
    // error: invalid conversion from ‘int’ to ‘cxxenum::color’
    // int value_2 {10};
    // color c = value_2;
  }

  // scoped enum do strong type-safety because they do not implicitly convert to
  // or from integer values

  {
    // However, followings cause type error:
    // cxx-error: invalid conversion from ‘int’ to ‘color’ [-fpermissive]
    // note: it's interesting and unscoped is also type?
    //
    // color color_selected = 2;
    //
    // OK
    // color color_selected = yellow;

    // cxx-error: type error, cannot convert ‘peppers’ to ‘int’ in
    // initialization int value_1 = peppers::red;
    //
    // cxx.cpp:1527:43: error: no match for ‘operator<<’
    //  (operand types are ‘std::basic_ostream<char>’ and ‘cxx_enum::peppers’)
    //      std::cout << "peppers::yellow value " << peppers::yellow <<
    //      std::endl;
    //
    // std::cout << "peppers::yellow value " << peppers::yellow << std::endl;
    //
    // okay and can use in this way
    // << setw(20) << left << (e.status == procstatus::running ? "Running" :
    // "Suspended")
  }
}

namespace cxx_enum_in_class
{
  enum class named_and_scoped
  {
    red,
    yellow,
    green
  };

  class Foo
  {
    // private:
    //   enum {red, yellow, green};

  public:
    enum
    {
      red,
      yellow,
      green
    };

  private:
    std::string name_;

    // private:
    //   named_and_scoped color_;

  public:
    named_and_scoped color_;

  public:
    Foo()
        : name_("")
    {}
  };
} // namespace cxx_enum_in_class

TEST(CxxEnum, InClass)
{
  using namespace cxx_enum_in_class;

  // as expected for scoped enum
  {
    named_and_scoped value_1, value_2;
    value_1 = named_and_scoped::red;
    value_2 = named_and_scoped::yellow;

    EXPECT_THAT(value_1, named_and_scoped::red);
    EXPECT_THAT(value_2, named_and_scoped::yellow);
  }

  // use enum in a class? As with unscoped enum, can use cxx-enum-hack but have
  // to use "Foo::" prefix as scoped enum. Also, is under cxx-access-control.
  // benefit?

  {
    // when use `private`
    // cxx.cpp:1563:20: error: ‘cxx_enum::Foo::<unnamed enum> green’ is private
    // within this context
    //   int flag3 = Foo::green;
    //                    ^~~~~
    // Since it's in class, this scoped but not named

    int value_1 = Foo::green;

    // cxx.cpp:1587:19: error: ‘green’ was not declared in this scope
    //      int value_2 = green;
    // int value_2 = green;

    EXPECT_THAT(value_1, 2);
    EXPECT_THAT(2, Foo::green);

    int coll[Foo::green];
    EXPECT_THAT(8, sizeof(coll));
  }

  {
    Foo foo;
    foo.color_ = named_and_scoped::red;
  }
}

enum class EnumFlags : char
{
  SPORT = 1,
  KIDS  = 2,
  MUSIC = 4
};

constexpr EnumFlags operator|(EnumFlags lhs, EnumFlags rhs)
{
  // C++PL 220
  // explicit converison is nessary since enum class does not support implicit
  // conversions.
  return static_cast<EnumFlags>(static_cast<char>(lhs) |
                                static_cast<char>(rhs));
}

constexpr EnumFlags operator&(EnumFlags lhs, EnumFlags rhs)
{
  return static_cast<EnumFlags>(static_cast<char>(lhs) &
                                static_cast<char>(rhs));
}

class ScopedEnum
{
public:
  int checkFlags(EnumFlags flag)
  {
    int result{};

    switch (flag)
    {
      // used constexpr on oeprator| and & since someone might want to
      // use them in constant expression.
      case EnumFlags::SPORT:
        cout << "has sport flag" << endl;
        result = 0;
        break;

      case EnumFlags::KIDS:
        cout << "has kids flas" << endl;
        result = 1;
        break;

      case EnumFlags::MUSIC:
        cout << "has music flag" << endl;
        result = 2;
        break;

        // *cxx-switch*
        // to avoid warning
        // warning: case value ‘5’ not in enumerated type ‘EnumFlags’ [-Wswitch]
        // case EnumFlags::SPORT|EnumFlags::MUSIC:
        //     cout << "has sport and music flag" << endl;
        //     result = 3;
        //     break;

      default:
        cout << "has unknown flag" << endl;
        result = 100;
    }

    return result;
  }
};

// [ RUN      ] Enum.ScopedEnum
// has sport flag
// has kids flas
// has music flag
// has unknown flag
// [       OK ] Enum.ScopedEnum (0 ms)

TEST(CxxEnum, ScopeAndType)
{
  ScopedEnum scoped;

  EXPECT_EQ(0, scoped.checkFlags(EnumFlags::SPORT));
  EXPECT_EQ(1, scoped.checkFlags(EnumFlags::KIDS));
  EXPECT_EQ(2, scoped.checkFlags(EnumFlags::MUSIC));

  // to avoid warning
  // EXPECT_EQ(3, scoped.checkFlags(EnumFlags::SPORT|EnumFlags::MUSIC));

  EXPECT_EQ(100, scoped.checkFlags(EnumFlags::SPORT | EnumFlags::KIDS));
  // EXPECT_EQ(100, scoped.checkFlags(200));
}

// ={=========================================================================
// cxx-rvo

namespace cxx_rvo
{
  // Note: All methods have side effects
  struct Snitch
  {
    Snitch(int value)
        : value_(value)
    {
      cout << "c'tor" << endl;
    }
    ~Snitch() { cout << "d'tor" << endl; }

    Snitch(const Snitch &) { cout << "copy c'tor" << endl; }
    Snitch(Snitch &&) { cout << "move c'tor" << endl; }

    Snitch &operator=(const Snitch &)
    {
      cout << "copy assignment" << endl;
      return *this;
    }

    Snitch &operator=(Snitch &&)
    {
      cout << "move assignment" << endl;
      return *this;
    }

    int getValue() const { return value_; }

  private:
    int value_{0};
  };

  Snitch ExampleRVO()
  {
    std::cout << "in ExampleRVO: " << std::endl;
    return Snitch(100);
  }

  std::vector<Snitch> ReturnVector()
  {
    // vector<Snitch> ivec(1000000000, 1);
    // vector(n, elem); creates n elements
    std::vector<Snitch> coll(10, Snitch(200));
    cout << "in ReturnVector: size of vector: " << coll.size() << endl;
    return coll;
  }

  Snitch createSnitch() { return Snitch(200); }

  void foo(Snitch s) { cout << "snitch value is: " << s.getValue() << endl; }

} // namespace cxx_rvo

// [ RUN      ] CxxRVO.check_single_construction
// in ExampleRVO:
// c'tor
// d'tor
// [       OK ] CxxRVO.check_single_construction (0 ms)

TEST(CxxRVO, check_single_construction)
{
  using namespace cxx_rvo;

  Snitch snitch = ExampleRVO();
}

// [ RUN      ] CxxRVO.check_copy_to_vector
// c'tor
// copy c'tor   // 0
// copy c'tor
// copy c'tor
// copy c'tor
// copy c'tor
// copy c'tor
// copy c'tor
// copy c'tor
// copy c'tor
// copy c'tor   // 9
// d'tor
// in ReturnVector: size of vector: 10
// d'tor
// d'tor
// d'tor
// d'tor
// d'tor
// d'tor
// d'tor
// d'tor
// d'tor
// d'tor
// [       OK ] CxxRVO.check_copy_to_vector (0 ms)

TEST(CxxRVO, check_copy_to_vector)
{
  using namespace cxx_rvo;

  std::vector<Snitch> coll = ReturnVector();

  cout << "return from ReturnVector: size of vector: " << coll.size() << endl;
}

// [ RUN      ] CxxRVO.check_argument
// c'tor
// snitch value is: 200
// d'tor
// [       OK ] CxxRVO.check_argument (0 ms)

TEST(CxxRVO, check_argument)
{
  using namespace cxx_rvo;

  foo(Snitch(200));
}

// [ RUN      ] CxxRVO.check_assignment
// c'tor
// ----------
// c'tor
// move assignment
// d'tor
// d'tor
// [       OK ] CxxRVO.check_assignment (0 ms)

TEST(CxxRVO, check_assignment)
{
  using namespace cxx_rvo;

  // ctor
  Snitch s = createSnitch();
  cout << "----------" << endl;

  // assign
  s = createSnitch();
}

// [ RUN      ] CxxRVO.check_when_rvo_on
// c'tor
// d'tor
// [       OK ] CxxRVO.check_when_rvo_on (0 ms)
// [ RUN      ] CxxRVO.check_when_rvo_off
// c'tor
// c'tor
// move c'tor
// d'tor
// d'tor
// d'tor
// [       OK ] CxxRVO.check_when_rvo_off (0 ms)

namespace cxx_rvo
{
  Snitch createSnitch2(bool runtime_condition)
  {
    Snitch a(100), b(100);
    if (runtime_condition)
    {
      return a;
    }
    else
    {
      return b;
    }
  }
} // namespace cxx_rvo

TEST(CxxRVO, check_when_rvo_on)
{
  using namespace cxx_rvo;

  // ctor
  Snitch s = createSnitch();
}

TEST(CxxRVO, check_when_rvo_off)
{
  using namespace cxx_rvo;

  // ctor
  Snitch s = createSnitch2(true);
}

/*
={=========================================================================
cxx-swap
*/
namespace cxxswap
{
  void swap_by_value(int x, int y)
  {
    int temp;
    temp = x;
    x    = y;
    y    = temp;
  }

  void swap_by_pointer(int *x, int *y)
  {
    int temp;
    temp = *x;
    *x   = *y;
    *y   = temp;
  }

  void swap_by_reference(int &x, int &y)
  {
    int temp;
    temp = x;
    x    = y;
    y    = temp;
  }
} // namespace cxxswap

TEST(CxxSwap, check_difference)
{
  using namespace cxxswap;

  // no swap happens since uses a copy
  {
    int x = 10, y = 20;
    swap_by_value(x, y);
    EXPECT_EQ(x, 10);
    EXPECT_EQ(y, 20);
  }

  // swap
  {
    int x = 10, y = 20;
    swap_by_pointer(&x, &y);
    EXPECT_EQ(x, 20);
    EXPECT_EQ(y, 10);
  }

  // swap, no need to pass its address
  {
    int x = 10, y = 20;
    swap_by_reference(x, y);
    EXPECT_EQ(x, 20);
    EXPECT_EQ(y, 10);
  }

  // *cxx-swap* std-swap uses reference
  {
    int x = 10, y = 20;
    std::swap(x, y);
    EXPECT_EQ(x, 20);
    EXPECT_EQ(y, 10);
  }
}

// ={=========================================================================
// see how swap works on vector
// void swap( vector& other ); (until C++17)
TEST(CxxSwap, check_container)
{
  std::vector<int> coll1{1, 2, 3, 4};
  std::vector<int> coll2{5, 6, 7, 8, 9, 10};
  EXPECT_THAT(coll1.size(), 4);
  EXPECT_THAT(coll2.size(), 6);

  // swap them
  swap(coll1, coll2);

  EXPECT_THAT(coll1.size(), 6);
  EXPECT_THAT(coll1, ElementsAre(5, 6, 7, 8, 9, 10));

  EXPECT_THAT(coll2.size(), 4);
  EXPECT_THAT(coll2, ElementsAre(1, 2, 3, 4));
}

// ={=========================================================================
// how swap elements on vector?
//
// 9.3.4 iter_swap()
//
// #include <algorithm>
//
// void iter_swap (ForwardIterator1 pos1, ForwardIterator2 pos2)
//
// Swaps the values to which iterators pos1 and pos2 refer.
// The iterators don’t need to have the same type. However, the values must be
// assignable.

TEST(CxxSwap, check_elements)
{
  {
    std::vector<int> coll{1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9));

    // std::iter_swap(coll.begin(), next(coll.begin()));
    std::iter_swap(coll.begin(), ++coll.begin());
    EXPECT_THAT(coll, ElementsAre(2, 1, 3, 4, 5, 6, 7, 8, 9));

    // std::iter_swap(coll.begin(), prev(coll.end()));
    std::iter_swap(coll.begin(), --coll.end());
    EXPECT_THAT(coll, ElementsAre(9, 1, 3, 4, 5, 6, 7, 8, 2));
  }

  // use operator[]() since [] returns reference
  {
    std::vector<int> coll{1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9));

    std::swap(coll[0], coll[1]);
    EXPECT_THAT(coll, ElementsAre(2, 1, 3, 4, 5, 6, 7, 8, 9));

    std::swap(coll[0], coll[8]);
    EXPECT_THAT(coll, ElementsAre(9, 1, 3, 4, 5, 6, 7, 8, 2));
  }

  // use operator*()
  //
  // // Forward iterator requirements
  // reference
  // operator*() const _GLIBCXX_NOEXCEPT
  // { return *_M_current; }
  {
    std::vector<int> coll{1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9));

    std::vector<int>::iterator one, two;
    one = coll.begin();
    two = one + 1;
    swap(*one, *two); // calls std::swap()
    EXPECT_THAT(coll, ElementsAre(2, 1, 3, 4, 5, 6, 7, 8, 9));

    one = coll.begin();
    two = one + 8;
    swap(*one, *two);
    EXPECT_THAT(coll, ElementsAre(9, 1, 3, 4, 5, 6, 7, 8, 2));
  }

  // on array and works
  {
    int coll[]{1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9));

    int *e1 = coll + 0;
    int *e2 = coll + 2;

    std::swap(*e1, *e2);
    EXPECT_THAT(coll, ElementsAre(3, 2, 1, 4, 5, 6, 7, 8, 9));
  }

  // on array and fails
  // {
  //   int coll[]{1, 2, 3, 4, 5, 6, 7, 8, 9};
  //   EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9));

  //   int *e1 = coll+0;
  //   int *e2 = coll+2;

  //   std::swap(e1, e2);
  //   EXPECT_THAT(coll, ElementsAre(3, 2, 1, 4, 5, 6, 7, 8, 9));
  // }
}

/*
// ={=========================================================================
// cxx-reference

There is no reference to a reference. but there's a pointer to a pointer. 

This do not mean it's a compile error but mean that don't have the same as
pointer has. This rrval binds to ivalue but not to rval.

*/

TEST(CxxReference, no_reference_to_reference)
{
  {
    int ivalue{10};

    int &rval  = ivalue;
    int &rrval = rval;
  }

  {
    int ivalue{10};

    int &rval  = ivalue;
    int &rrval = ivalue;
  }
}

/*
// ={=========================================================================
[ RUN      ] CxxReference.no_address
ival : 100
iref : 100
irref: 100
ival : 0x7ffe9c457384
iref : 0x7ffe9c457384
irref: 0x7ffe9c457384
[       OK ] CxxReference.no_address (0 ms)
*/

TEST(CxxReference, no_address)
{
  int ival   = 100;
  int &iref  = ival;
  int &irref = ival;

  cout << "ival : " << ival << endl;
  cout << "iref : " << iref << endl;
  cout << "irref: " << irref << endl;

  cout << "ival : " << &ival << endl;
  cout << "iref : " << &iref << endl;
  cout << "irref: " << &irref << endl;
}

// ={=========================================================================
namespace cxx_reference
{
  void f1(char *buffer) { sprintf(buffer, "is filled by f1 %d\n", 10); }

  void f2(char *&buffer) { sprintf(buffer, "is filled by f2 %d\n", 10); }

  // regardless of whether or not uses f3, gets an error:
  //
  // error: invalid conversion from ‘char’ to ‘char*’ [-fpermissive]
  //      sprintf(buffer, "is filled by pointer %d\n", 10);
  //              ^^^^^^
  // void f3(char &buffer)
  // { sprintf(buffer, "is filled by pointer %d\n", 10); }

} // namespace cxx_reference

TEST(CxxReference, check_example)
{
  using namespace cxx_reference;

  {
    char buffer[100];

    f1(buffer);

    // since *cxx-array* the point is that name of array is
    // "rvalue of char *".
    //
    // error: cannot bind non-const lvalue reference of type ‘char*&’
    // to an rvalue of type ‘char*’
    // f2(buffer);
  }

  // this works
  {
    char *buffer = new char[100];
    f1(buffer);

    std::string coll{buffer};
    EXPECT_THAT(coll, "is filled by f1 10\n");
  }

  {
    char *buffer = new char[100];
    f2(buffer);

    std::string coll{buffer};
    EXPECT_THAT(coll, "is filled by f2 10\n");
  }
}

// ={=========================================================================
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

// ={=========================================================================
TEST(CxxReference, check_return)
{
  using namespace cxx_reference;

  // shows that the call operator and dot operator has the same precedence and
  // left associative. so can use the result of a call to call a member of the
  // resulting object.

  // set_move_1() returns reference
  {
    Screen scr{};

    scr.set_move_1(5).set_set(10);
    EXPECT_THAT(scr.print(), "move 5, set 10");
  }

  // set_move_2() returns reference
  {
    Screen scr{};

    scr.set_move_2(5).set_set(10);
    EXPECT_THAT(scr.print(), "move 5, set 10");
  }

  // set_move_3() returns copy so set_set() do not change scr
  {
    Screen scr{};

    scr.set_move_3(5).set_set(10);
    EXPECT_THAT(scr.print(), "move 5, set 0");
  }

  // set_move_4() returns copy
  {
    Screen scr{};

    scr.set_move_4(5).set_set(10);
    EXPECT_THAT(scr.print(), "move 5, set 0");
  }
}

// ={=========================================================================
// see difference when use "cxx-auto"
//
// https://devblogs.microsoft.com/cppblog/new-safety-rules-in-c-core-check/?fbclid=IwAR2HJjPz8nTUnjEJyHHXyblk1m6b91tO60G863Z65OqB9nJ07AWL7IF_CrM#expensive-copy-with-the-auto-keyword
// Expensive copy with the auto keyword
//
// Here, the type of password resolves to std::string, even though the return
// type of getPassword() is a const-reference to a string. The resulting
// behavior is that the contents of PasswordManager::password get copied into
// the local variable password.
//
// This difference in behavior between assigning a reference and a pointer to a
// variable marked auto is non-obvious, resulting in potentially unwanted and
// unexpected copying.

TEST(CxxReference, check_return_and_auto)
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
1. as with *cxx-return-cxx-temporary* where having a reference of the returned
  tempoary in caller site is valid and allowed. so the following should not
  crash? NO since here return reference but cxx-return-cxx-temporary returns
  temporaray full object.

*/

TEST(CxxReference, DISABLED_check_return_crash)
{
  using namespace cxx_reference;

  // return string reference of tempory
  {
    Foo foo;

    auto &ret = foo.get_name_2();

    ret.assign("get_name_2");

    EXPECT_THAT(foo.print(), "get_name_2");
  }
}

// ={=========================================================================
// cxx-reference-and-access

// Q: does it work to access private of that?
//
// bool operator==(const Location &that)
// {
//   return longitude_ == that.longitude_ &&
//     latitude_ == that.latitude_;
// }
//
// while use of reference member is error when returning whole object but use of
// reference member is find when returning that reference member

class AUsePrivateMemberThroughReference
{
private:
  int val_;

public:
  AUsePrivateMemberThroughReference()
      : val_(10)
  {}

  int &returnReference() { return val_; }

  // cxx.cpp: In member function ‘virtual void
  // Reference_UsePrivateMemberThroughReference_Test::TestBody()’:
  // cxx.cpp:800:9: error: ‘int AUsePrivateMemberThroughReference::val_’ is
  // private
  //      int val_;
  //          ^
  // cxx.cpp:840:9: error: within this context
  //      ref.val_ = 100;

  // AUsePrivateMemberThroughReference &returnReference()
  // {
  //     return *this;
  // }

  void changePrivate(AUsePrivateMemberThroughReference &that)
  {
    that.val_ = 200;
  }

  void printValue() const { std::cout << "value: " << val_ << std::endl; }
};

TEST(Reference, AccessOnReference)
{
  AUsePrivateMemberThroughReference firstObjectWithValue10;

  firstObjectWithValue10.printValue();
  int &ref = firstObjectWithValue10.returnReference();
  ref      = 100;
  firstObjectWithValue10.printValue();

  // error
  // firstObjectWithValue10.printValue();
  // AUsePrivateMemberThroughReference &ref =
  // firstObjectWithValue10.returnReference(); ref.val_ = 100;
  // firstObjectWithValue10.printValue();

  AUsePrivateMemberThroughReference secondObjectWithValue10;
  secondObjectWithValue10.changePrivate(firstObjectWithValue10);
  firstObjectWithValue10.printValue();
}

// ={=========================================================================
// cxx-time

TEST(CxxTime, ratio)
{
  // typedef std::ratio<5, 3> 5/3, FiveThirds;
  using FiveThirds = std::ratio<5, 3>;

  // Numerator and denominator. 5/3
  EXPECT_THAT(FiveThirds::num, 5);
  EXPECT_THAT(FiveThirds::den, 3);

  // Numerator and denominator
  EXPECT_THAT(std::nano::num, 1);
  EXPECT_THAT(std::nano::den, 1000000000LL);

  EXPECT_THAT(std::micro::num, 1);
  EXPECT_THAT(std::micro::den, 1000000LL);

  EXPECT_THAT(std::milli::num, 1);
  EXPECT_THAT(std::milli::den, 1000LL);

  std::ratio<100> rn;
  EXPECT_THAT(rn.num, 100);
  EXPECT_THAT(rn.den, 1);
}

// ={=========================================================================
// The local date and time is: Tue Jun 12 12:49:12 2018
// The local date and time is: Tue Jun 12 12:49:12 2018
// The UTC date and time is: Tue Jun 12 11:49:12 2018

TEST(CxxTime, system_call)
{
  // #include <time.h>
  // time_t time(time_t *timep);
  // time_t now = time(0);
  //
  // time() system call returns the number of seconds since the Epoch. i.e., the
  // same value that gettimeofday() returns in the tv_sec field of its tv
  // argument.

  auto now = std::time(0);

  // The ctime() function provides a simple method of converting a time_t value
  // `into printable form.` The ctime() function automatically accounts for
  // local timezone and DST settings when performing the conversion.
  //
  // Returns pointer to statically allocated string `terminated by newline` and
  // \0 on success, or NULL on error

  cout << "The local date and time is: " << std::ctime(&now);

  // The gmtime() and localtime() functions convert a time_t value into a
  // so-called brokendown time. The broken-down time is placed in a statically
  // allocated structure whose address is returned as the function result.

  // tm *localtm = localtime(&now);
  auto localtm = std::localtime(&now);
  cout << "The local date and time is: " << std::asctime(localtm);

  // tm *gmtm = gmtime(&now);
  auto gmtm = std::gmtime(&now);
  if (gmtm != nullptr)
  {
    cout << "The UTC date and time is: " << std::asctime(gmtm);
  }
}

TEST(CxxTime, conversion)
{
  auto now = std::time(0);

  std::cout << "current utc      : " << now << std::endl;

  auto now_to_ms_1 = now * 1000;
  std::cout << "current utc in ms: " << now_to_ms_1 << std::endl;

  uint64_t now_to_ms_2 = static_cast<uint64_t>(now * 1000);
  std::cout << "current utc in ms: " << now_to_ms_2 << std::endl;

  uint64_t now_to_ms_3 = now * 1000;
  std::cout << "current utc in ms: " << now_to_ms_3 << std::endl;
}

// ={=========================================================================
// The local date and time is: Tue Jun 12 14:42:18 2018
// The local date and time is: Tue Jun 12 14:42:28 2018

TEST(CxxTime, check_sleep_for)
{
  // tm *localtm = localtime(&now);
  time_t now = time(0);
  auto prev  = localtime(&now);
  cout << "The local date and time is: " << asctime(prev);

  std::this_thread::sleep_for(chrono::seconds(10));

  now       = time(0);
  auto curr = localtime(&now);
  cout << "The local date and time is: " << asctime(curr);

  // why 0?
  EXPECT_THAT((curr->tm_sec - prev->tm_sec), 0);
}

// ={=========================================================================
// Arithmetic Duration Operations
//
// You can compute with durations in the expected way (see Table 5.21):
//
// o You can process the sum, difference, product, or quotient of two durations.
// o You can add or subtract ticks or other durations.
// o You can compare two durations.
//
// The important point here is that the unit type of two durations involved in
// such an operation might be different.
//
// Table 5.21. Arithmetic Operations of durations

TEST(CxxTime, check_duration_1)
{
  std::chrono::seconds twentySeconds(20);
  EXPECT_THAT(twentySeconds.count(), 20);

  std::chrono::hours aDay(24);
  EXPECT_THAT(aDay.count(), 24);

  std::chrono::milliseconds oneMillisecond(1);
  EXPECT_THAT(oneMillisecond.count(), 1);
}

// ={=========================================================================
// You can also convert durations into durations of different units, as long
// as there is an implicit type conversion. Thus, you can convert hours into
// seconds but not the other way around. For example:
//
// Table 5.22. Other Operations and Types of durations
//
// d.count()             Returns ticks of the duration d
// duration_cast<D>(d)   Returns duration d explicitly converted into type D

// [ RUN      ] CxxTime.check_duration_2
// 188209768194270 ms
// 3742327457174483840 ns
// [       OK ] CxxTime.check_duration_2 (0 ms)

TEST(CxxTime, check_duration_2)
{
  {
    std::chrono::seconds twentySeconds{20}; // 20 seconds
    std::chrono::hours aDay{24};            // 24 hours
    std::chrono::milliseconds ms;           // NOTE NOT milliseconds

    // 24 hours and 20 seconds in milliseconds?
    // 24 * 60 * 60 * 1000 + 20 * 1000

    ms += twentySeconds + aDay; // 86,420,000 milliseconds
    --ms;                       // 86,419,999 milliseconds
    ms *= 2;                    // 172,839,998 milliseconds

    std::cout << ms.count() << " ms" << std::endl;

    // convert ms to ns
    std::cout << std::chrono::nanoseconds(ms).count() << " ns" << std::endl;
  }

  {
    std::chrono::seconds twentySeconds{20}; // 20 seconds
    std::chrono::hours aDay{24};            // 24 hours
    std::chrono::milliseconds ms{};         // 0 milliseconds

    // 24 hours and 20 seconds in milliseconds?
    // 24 * 60 * 60 * 1000 + 20 * 1000

    ms += twentySeconds + aDay; // 86,420,000 milliseconds
    --ms;                       // 86,419,999 milliseconds
    ms *= 2;                    // 172,839,998 milliseconds

    // std::cout << ms.count() << " ms" << std::endl;
    EXPECT_THAT(ms.count(), 172839998);

    // convert ms to ns
    // std::cout << std::chrono::nanoseconds(ms).count() << " ns" << std::endl;
    EXPECT_THAT(std::chrono::nanoseconds(ms).count(), 172839998000000);
  }
}

// ={=========================================================================
// *cxx-time-duration-cast*
//
// A typical example is code that segments a duration into different units. For
// example, the following code segments a duration of milliseconds into the
// corresponding hours, minutes, seconds, and milliseconds
//
// raw: [2 of 3600/1]::[0 of 60/1]::[55 of 1/1]::[42 of 1/1000]
//      02::00::55::42

namespace cxx_time
{
  template <typename Unit, typename Ratio>
  ostream &operator<<(ostream &os, const chrono::duration<Unit, Ratio> &d)
  {
    os << "[" << d.count() << " of " << Ratio::num << "/" << Ratio::den << "]";
    return os;
  }
} // namespace cxx_time

TEST(CxxTime, check_duration_cast_1)
{
  using namespace cxx_time;

  std::chrono::milliseconds ms{7255042};

  // convert ms to hours
  std::chrono::hours hh = std::chrono::duration_cast<chrono::hours>(ms);
  EXPECT_THAT(hh.count(), 2);

  // take remians which means take hours out
  std::chrono::minutes mm = std::chrono::duration_cast<std::chrono::minutes>(
    ms % std::chrono::hours(1));

  std::chrono::seconds ss = std::chrono::duration_cast<std::chrono::seconds>(
    ms % std::chrono::minutes(1));

  std::chrono::milliseconds msec =
    std::chrono::duration_cast<std::chrono::milliseconds>(
      ms % std::chrono::seconds(1));

  std::chrono::nanoseconds nsec1 =
    std::chrono::duration_cast<std::chrono::nanoseconds>(
      ms % std::chrono::seconds(1));

  uint32_t nsec2 = ((ms % std::chrono::seconds(1)).count() * 1000000L);

  std::ostringstream os{};

  os << hh << "::" << mm << "::" << ss << "::" << msec << "::" << nsec1;

  EXPECT_THAT(os.str(),
              "[2 of 3600/1]::[0 of 60/1]::[55 of 1/1]::[42 of "
              "1/1000]::[42000000 of 1/1000000000]");

  os.str("");

  // see that same count() function used as operator<<() above

  os << hh.count() << "::" << mm.count() << "::" << ss.count()
     << "::" << msec.count() << "::" << nsec1.count();

  EXPECT_THAT(os.str(), "2::0::55::42::42000000");
  EXPECT_THAT(nsec1.count(), nsec2);
}

// ={=========================================================================
// As we have seen, implicit conversions to a more precise unit type are always
// possible. However, conversions to a coarser unit type are not, because you
// might lose information.
//
// For example, when converting an integral value of 42,010 milliseconds into
// seconds, the resulting integral value, 42, means that the precision of having
// a duration of 10 milliseconds over 42 seconds gets lost. But you can still
// explicitly force such a conversion with a duration_cast. For example:
//
// std::chrono::seconds sec(55);
//
// std::chrono::minutes m1 = sec; // ERROR
//
// std::chrono::minutes m2 =
//    std::chrono::duration_cast<std::chrono::minutes>(sec); // OK

// TEST(CxxTime, check_duration_cast_2)
// {
//   using namespace cxx_time;

//   std::chrono::milliseconds ms{7255042};

//   // convert ms to hours
//   std::chrono::hours hh{};

//   // error: no match for ‘operator=’ (operand types are ‘std::chrono::hours
//   // {aka std::chrono::duration<long int, std::ratio<3600> >}’ and
//   // ‘std::chrono::milliseconds {aka std::chrono::duration<long int, std::ratio<1, 1000> >}’)
//   //    hh = ms;

//   hh = ms;

//   EXPECT_THAT(hh.count(), 2);
// }

// ={=========================================================================
// cxx-time-crono-clock

// the following function prints the properties of a clock
// C represents clock
template <typename C>
void print_clock_data(ostringstream &os)
{
  using namespace std;

  os << "- precision: ";

  // clock::period
  // Yields the type of the unit type (equivalent to clock::duration::period)

  typedef typename C::period P;

  // /usr/include/c++/4.9/ratio
  // typedef ratio<                     1000, 1> kilo;

  if (ratio_less_equal<P, std::milli>::value)
  {
    // This class template alias generates a ratio type that is the
    // multiplication of the ratio types R1 and R2.
    //
    // The resulting type is the same as if ratio_multiply was defined as:
    //
    // template <typename R1, typename R2>
    // using ratio_multiply = std::ratio < R1::num * R2::num, R1::den * R2::den
    // >;

    typedef typename ratio_multiply<P, std::kilo>::type TT;
    os << fixed << double(TT::num) / TT::den << " milliseconds" << endl;
  }
  else
  {
    os << fixed << double(P::num) / P::den << " seconds" << endl;
  }

  // clock::is_steady
  // Yields true if the clock is steady

  os << "- is ready: " << boolalpha << C::is_steady << endl;
}

TEST(CxxTime, check_various_clock)
{
  ostringstream os;

  os << "system_clock: " << endl;
  print_clock_data<std::chrono::system_clock>(os);

  os << "high_resolution_clock: " << endl;
  print_clock_data<std::chrono::high_resolution_clock>(os);

  os << "steady_clock: " << endl;
  print_clock_data<std::chrono::steady_clock>(os);

  // TN: this is different from CLR result

  char expected[] = "system_clock: \n"
                    "- precision: 0.000001 milliseconds\n"
                    "- is ready: false\n"
                    "high_resolution_clock: \n"
                    "- precision: 0.000001 milliseconds\n"
                    "- is ready: false\n"
                    "steady_clock: \n"
                    "- precision: 0.000001 milliseconds\n"
                    "- is ready: true\n";

  EXPECT_THAT(os.str(), expected);
}

// ={=========================================================================
TEST(CxxTime, check_steady_clock)
{
  {
    // now() is static function
    auto system_start = chrono::system_clock::now();

    sleep(10);

    auto diff = chrono::system_clock::now() - system_start;
    auto sec  = chrono::duration_cast<chrono::seconds>(diff);

    // cout << "this programs runs: " << sec.count() << " seconds" << endl;
    EXPECT_THAT(sec.count(), 10);
  }

  {
    // now() is static function
    auto system_start = chrono::steady_clock::now();

    sleep(10);

    auto diff = chrono::steady_clock::now() - system_start;
    auto sec  = chrono::duration_cast<chrono::seconds>(diff);

    // cout << "this programs runs: " << sec.count() << " seconds" << endl;
    EXPECT_THAT(sec.count(), 10);
  }
}

// ={=========================================================================
// cxx-time-timepoint

std::string as_string(const std::chrono::system_clock::time_point &tp)
{
  // static convenience function
  // Note also that this convenience function probably will work only for
  // system_clocks, the only clocks that provide an interface for conversions
  // to and from time_t.

  std::time_t time = std::chrono::system_clock::to_time_t(tp);

  // std::string ts = std::ctime(&time);
  // to get UTC:
  std::string ts = std::asctime(gmtime(&time));

  // remove trailing newline
  ts.resize(ts.size() - 1);

  return ts;
}

// util/chrono1.cpp
// epoch: Thu Jan  1 01:00:00 1970
// now  : Thu Apr 12 10:52:00 2018
// min  : Tue Sep 21 00:11:29 1677
// max  : Sat Apr 12 00:47:16 2262

// epoch: Thu Jan  1 01:00:00 1970
//
// Note that it’s 1 o’clock rather than midnight. This may look a bit
// surprising, but remember that the conversion to the calendar time with
// ctime() inside asString() takes the time zone into account. Thus, the UNIX
// epoch used here  which, again, is not always guaranteed to be the epoch of
// the system time  started at 00:00 in Greenwich, UK. In my time zone, Germany,
// it was 1 a.m. at that moment, so in my time zone the epoch started at 1 a.m.
// on January 1, 1970. Accordingly, if you start this program, your output is
// probably different, according to your time zone, even if your system uses the
// same epoch in its system clock.

// epoch: Thu Jan  1 00:00:00 1970
// now  : Thu Apr 12 10:01:32 2018
// min  : Tue Sep 21 00:12:44 1677  // this is bigger and different from the book
// max  : Fri Apr 11 23:47:16 2262  // this is bigger and different from the book

TEST(CxxTime, check_timepoint)
{
  {
    // print the epoch of this system clock

    // is equivalent to:
    // std::chrono::time_point<std::chrono::system_clock> tp
    // Thus, the default constructor, which yields the epoch, creates a timepoint,
    std::chrono::system_clock::time_point tp;
    cout << "epoch: " << as_string(tp) << endl;

    tp = std::chrono::system_clock::now();
    cout << "now  : " << as_string(tp) << endl;

    tp = std::chrono::system_clock::time_point::min();
    cout << "min  : " << as_string(tp) << endl;

    tp = std::chrono::system_clock::time_point::max();
    cout << "max  : " << as_string(tp) << endl;
  }
}

// ={=========================================================================
// now in duration: 1599840788414732866
// now in seconds : 1599840788
// now : 1599840923
// now : 1599840923

TEST(CxxTime, check_now)
{
  // tp.time_since_epoch()
  // Returns the duration between the epoch and timepoint tp
  {
    auto tp = std::chrono::system_clock::now();

    // compile error since no operator<<() is defined.
    // std::cout << "now  : " << tp.time_since_epoch() << std::endl;
    std::cout << "now in duration: " << tp.time_since_epoch().count()
              << std::endl;

    std::cout
      << "now in seconds : "
      << chrono::duration_cast<chrono::seconds>(tp.time_since_epoch()).count()
      << std::endl;
  }

  // static std::chrono::time_point<std::chrono::system_clock> now() noexcept;
  std::time_t time =
    std::chrono::system_clock::to_time_t(chrono::system_clock::now());

  std::cout << "now : " << time << std::endl;
  std::cout << "now : " << std::time(0) << std::endl;
}

// ={=========================================================================
TEST(CxxTime, check_timepoint_arithmetic)
{
  std::ostringstream os{};

  // one day as seconds
  typedef chrono::duration<int, ratio<3600 * 24>> Days;

  chrono::time_point<chrono::system_clock> tp;

  // allow adjusting timepoints by using timepoint arithmetic.

  // add 1 day, 23 hours, and 55 minutes to the epoch
  tp += Days(1) + chrono::hours(23) + chrono::minutes(55);
  os << "later : " << as_string(tp) << endl;

  // diff in minutes
  auto diff = tp - chrono::system_clock::time_point();
  os << "diff : " << chrono::duration_cast<chrono::minutes>(diff).count()
     << " minutes" << endl;

  // minus 1 year (hoping it is valid and not a leap year
  tp -= chrono::hours(24 * 365);
  os << "-1 year: " << as_string(tp) << endl;

  // 3600 is 1 year
  tp -= chrono::duration<int, ratio<3600 * 24 * 365>>(50);
  os << "-50 years: " << as_string(tp) << endl;

  // 3600 is 1 year
  // works since the min is bigger and different from the book

  tp -= chrono::duration<int, ratio<3600 * 24 * 365>>(50);
  os << "-50 years: " << as_string(tp) << endl;

  char expected[] = "later : Fri Jan  2 23:55:00 1970\n"
                    "diff : 2875 minutes\n"
                    "-1 year: Thu Jan  2 23:55:00 1969\n"
                    "-50 years: Wed Jan 15 23:55:00 1919\n"
                    "-50 years: Tue Jan 26 23:55:00 1869\n";

  EXPECT_THAT(os.str(), expected);
}

// 12/04/18
// Tuesday 12/04/18 02PM

TEST(Time, Facet)
{
  ostringstream os;

  auto now   = chrono::system_clock::now();
  time_t t   = chrono::system_clock::to_time_t(now);
  tm *tm_now = localtime(&t);

  locale loc;
  const time_put<char> &tp = use_facet<time_put<char>>(loc);

  // %x Locale’s preferred date representation Jul 12 1998
  // 06/12/18

  tp.put(cout, cout, ' ', tm_now, 'x');
  cout << endl;

  // use format string
  // Tuesday 06/12/18 04PM

  string fmt = "%A %x %I%p\n";
  tp.put(cout, cout, ' ', tm_now, fmt.c_str(), fmt.c_str() + fmt.size());
}

// time taken (in ms) : 939987
// time taken (in ns) : 939         // since integer division
// time taken (in ns) : 939.987
// time taken (in ns) : 939
//
// time taken (in ms) : 911476
// time taken (in ns) : 911
// time taken (in ns) : 911.476
// time taken (in ns) : 911

// the name "Timer" is not representative. TimeLog?

namespace cxx_time_elapsed
{
  struct Timer
  {
    Timer(const std::string &text)
        : text_(text)
    {
      start_ = chrono::system_clock::now();
    }
    ~Timer()
    {
      auto elapsed       = chrono::system_clock::now() - start_;
      auto elapsed_in_ms = chrono::duration_cast<chrono::microseconds>(elapsed);
      auto elapsed_in_ns = chrono::duration_cast<chrono::milliseconds>(elapsed);

      // when this is in scope, this uses:
      //
      // ostream &operator<<(ostream &os, const chrono::duration<Unit,Ratio>
      // &d);
      //
      // othrewise, it's error
      // cout << "time taken (in sec) : " << elapsed << endl;

      cout << "time taken (in ms) : " << elapsed_in_ms.count() << endl;

      // // *cxx-integer-division*
      // cout << "time taken (in ns) : " << elapsed_in_ms.count()/1000 << endl;
      // cout << "time taken (in ns) : " << elapsed_in_ms.count()/1000.0 <<
      // endl; cout << "time taken (in ns) : " << elapsed_in_ms.count()*0.001 <<
      // endl; cout << "time taken (in ns) : " << elapsed_in_ns.count() << endl;

      cout << "time taken (in ns) : " << elapsed_in_ns.count() << " for "
           << text_ << endl;
    }

    std::string text_;
    chrono::system_clock::time_point start_;
  };

} // namespace cxx_time_elapsed

TEST(CxxTime, check_elapsed_time)
{
  using namespace cxx_time_elapsed;

  {
    Timer timer("StringConcat");

    string result{};

    const unsigned int lots{500000};
    for (unsigned int i{0}; i < lots; i++)
    {
      string user{"user" + to_string(i)};

      result += user;
    }
  }

  {
    Timer timer("StringAppend");

    string result{};

    const unsigned int lots{500000};
    for (unsigned int i{0}; i < lots; i++)
    {
      string user{"user" + to_string(i)};

      result.append(user);
    }
  }
}

// like lap feature of stopwatch
//
// copied from someone's code at YV.
//
// {
//   PerfCounter counter;
//
//   for(int i=0;i<10000;++i)
//   {
//     int out;
//     sscanf("42","%d",&out);
//   }
//   counter.snap("scanf int");
//
//   for(int i=0;i<10000;++i)
//   {
//     int out;
//     std::stringstream ss("42");
//     ss >> out;
//   }
//   counter.snap("stringstream int");
//
//   for(int i=0;i<10000;++i)
//   {
//     int out = boost::lexical_cast<int>("42");
//   }
//   counter.snap("boost::lexical_cast<int>");
//   std::cout << counter.dump() << std::endl;
// }
//
// Start -> scanf int took 2112us
// scanf int -> stringstream int took 27859us
// stringstream int -> boost::lexical_cast<int> took 910us
// boost::lexical_cast<int> -> end took 0us

namespace cxx_time_lap
{
  // design decision:
  // in order to get diff betwen two snaps, required to access two snap. whether
  // snap() or dump() do the work to calc diff? use dump() here

  class Snapper
  {
  public:
    explicit Snapper() { start_ = chrono::system_clock::now(); }

    void snap(const std::string text)
    {
      SnapTime st{};
      st.tp          = chrono::system_clock::now();
      st.description = text;
      list_.push_back(st);
    }

    void dump()
    {
      std::string previous{"start"};
      decltype(start_) previous_time = start_;

      for (const auto &e : list_)
      {
        auto diff = e.tp - previous_time;
        cout << previous << " -> " << e.description << " took "
             << chrono::duration_cast<chrono::microseconds>(diff).count()
             << " us" << endl;

        previous      = e.description;
        previous_time = e.tp;
      }
    }

  private:
    struct SnapTime
    {
      chrono::system_clock::time_point tp{};
      std::string description{};
    };

    std::list<SnapTime> list_;
    chrono::system_clock::time_point start_;
  };

  class PerfCounter
  {
  public:
    PerfCounter();
    ~PerfCounter();

    void snap(const string &name_);
    void dump();

  private:
    // *cxx-nested-class*
    // since the original code uses class but makes it all public, use
    // struct instead.
    struct CounterData
    {
      CounterData(const string &name_)
          : name{name_}
          , pnext{nullptr}
      {}
      string name;
      CounterData *pnext;
      struct timespec ts;
    };

    // singly list but has end as well.
    CounterData *phead;
    CounterData *pend;

    // utility function
    CounterData *createSnap(const string &name_);
  };

  PerfCounter::PerfCounter()
  {
    // create a start node
    phead = pend = createSnap("start");
  }

  PerfCounter::~PerfCounter()
  {
    // clean up a list
    for (CounterData *psnap = phead; psnap;)
    {
      phead = psnap->pnext;
      std::cout << "delete " << psnap->name << std::endl;
      delete psnap;
      psnap = phead;
    }
  }

  void PerfCounter::snap(const string &name_)
  {
    CounterData *psnap = createSnap(name_);

    pend->pnext = psnap;
    pend        = psnap;
  }

  // utility function to have common code in one place
  // note that use of cpp-nested-class type, `PerfCounter::CounterData`
  // Otherwise, see compile errors.
  //
  // CounterData *PerfCounter::createSnap(const string &name_) {}
  //
  // perfcounter.cpp:45:1: error: ‘CounterData’ does not name a type
  //  CounterData *PerfCounter::createSnap(const string &name_)
  //  ^

  PerfCounter::CounterData *PerfCounter::createSnap(const string &name_)
  {
    CounterData *psnap = new CounterData(name_);
    clock_gettime(CLOCK_MONOTONIC, &(psnap->ts));
    return psnap;
  }

  // cpp-stringstream
  void PerfCounter::dump()
  {
    std::stringstream ss{};
    uint32_t countSnap{1};

    // only when there are two nodes to use
    for (CounterData *pstart = phead; pstart && pstart->pnext;
         pstart              = pstart->pnext)
    {
      ss << "snap: " << countSnap << ": ";
      ss << pstart->name << " -> " << pstart->pnext->name << " took ";

      // time diff in us from current to next
      uint64_t timeDiff =
        (pstart->pnext->ts.tv_sec * 1000000 +
         pstart->pnext->ts.tv_nsec / 1000) -
        (pstart->ts.tv_sec * 1000000 + pstart->ts.tv_nsec / 1000);

      ss << timeDiff << "us" << std::endl;
      ++countSnap;
    }

    std::cout << ss.str();
  }

} // namespace cxx_time_lap

// on VM:
//
// start -> scanf int took 1609 us
// scanf int -> stringstream int took 22991 us
// stringstream int -> boost::lexical_cast<int> took 2445 us
// start -> scanf int took 1709 us
// scanf int -> stringstream int took 23523 us
// stringstream int -> boost::lexical_cast<int> took 2065 us
// ---------
// snap: 1: start -> scanf int took 1706us
// snap: 2: scanf int -> stringstream int took 23524us
// snap: 3: stringstream int -> boost::lexical_cast<int> took 2065us
// delete start
// delete scanf int
// delete stringstream int
// delete boost::lexical_cast<int>

TEST(Time, Snapper)
{
  using namespace cxx_time_lap;

  {
    Snapper counter;

    for (int i = 0; i < 10000; ++i)
    {
      int out;
      sscanf("42", "%d", &out);
    }
    counter.snap("scanf int");

    for (int i = 0; i < 10000; ++i)
    {
      int out;
      std::stringstream ss("42");
      ss >> out;
    }
    counter.snap("stringstream int");

    for (int i = 0; i < 10000; ++i)
    {
      int out = boost::lexical_cast<int>("42");
      (void)out;
    }
    counter.snap("boost::lexical_cast<int>");
    counter.dump();
  }

  {
    Snapper sn;
    PerfCounter pc;

    for (int i = 0; i < 10000; ++i)
    {
      int out;
      sscanf("42", "%d", &out);
    }
    sn.snap("scanf int");
    pc.snap("scanf int");

    for (int i = 0; i < 10000; ++i)
    {
      int out;
      std::stringstream ss("42");
      ss >> out;
    }
    sn.snap("stringstream int");
    pc.snap("stringstream int");

    for (int i = 0; i < 10000; ++i)
    {
      int out = boost::lexical_cast<int>("42");
      (void)out;
    }
    sn.snap("boost::lexical_cast<int>");
    pc.snap("boost::lexical_cast<int>");

    sn.dump();
    cout << "---------" << endl;
    pc.dump();
  }
}

// ={=========================================================================
// cxx-static

namespace cxx_static
{
  // shows static member shall be defined outside of class
  class FooStatic1
  {
  private:
    static const size_t MAX_CODE_LENGTH{4}; // *TN* no define

    // cause an error
    // static const std::string DIGIT_NOT_FOUND{"*"};
    static const std::string DIGIT_NOT_FOUND;

  public:
    struct Data
    {
      std::string name{};
      int value{};
    };

    static Data data;

  public:
    FooStatic1() {}
  };

  const std::string FooStatic1::DIGIT_NOT_FOUND{"*"};

  FooStatic1::Data FooStatic1::data = {"FooStatic1", 100};
} // namespace cxx_static

// show class member(static member) shall be defined outside
TEST(CxxStatic, check_member)
{
  using namespace cxx_static;

  FooStatic1 foo;

  // is it going to be null by default init? no
  EXPECT_THAT(FooStatic1::data.name, "FooStatic1");
}

namespace cxx_static
{
  // c++ cookbook, 8.4 Automatically Adding New Class Instances to a Container

  class StaticClass
  {
  protected:
    int value_{};
    size_t id_{};

    string name_{};
    static list<StaticClass *> instances_;
    static size_t track_id_;

  public:
    StaticClass(int value, string name = "static class")
        : value_(value)
        , name_(name)
    {
      id_ = ++track_id_;
      instances_.push_back(this);
    }

    ~StaticClass()
    {
      auto it = find(instances_.begin(), instances_.end(), this);
      if (it != instances_.end())
        instances_.erase(it);
    }

  public:
    static void ShowList()
    {
      cout << "ShowList: " << instances_.size() << endl;
      for (const auto &e : instances_)
      {
        cout << "ShowList: name : " << e->name_ << endl;
        cout << "ShowList: value: " << e->value_ << endl;
        cout << "ShowList: id   : " << e->id_ << endl;
      }
    }
  };

  list<StaticClass *> StaticClass::instances_;
  size_t StaticClass::track_id_ = 0;

} // namespace cxx_static

// ShowList: 3
// ShowList: name : instance 1
// ShowList: value: 1
// ShowList: id   : 1
// ShowList: name : instance 2
// ShowList: value: 10
// ShowList: id   : 2
// ShowList: name : instance 3
// ShowList: value: 100
// ShowList: id   : 3

TEST(CxxStatic, static_count_instances)
{
  using namespace cxx_static;

  {
    StaticClass sc1(1, "instance 1");
    StaticClass sc2(10, "instance 2");
    StaticClass sc3(100, "instance 3");
    StaticClass::ShowList();
  }

  // when no instance is created
  // ShowList: 0
  {
    StaticClass::ShowList();
  }
}

namespace cxx_static
{
  class Foo
  {
  public:
    Foo() {}

    static void createInstance()
    {
      std::cout << "Foo::createInstance()" << std::endl;
    }

    static void getName() { std::cout << "Foo::getName()" << std::endl; }
  };

  class Bar : public Foo
  {
  public:
    Bar() {}
    static void createInstance()
    {
      std::cout << "Bar::createInstance()" << std::endl;
    }
  };
} // namespace cxx_static

// [ RUN      ] CxxStatic.check_in_inheritance
// Foo::createInstance()
// Foo::getName()
//
// Bar::createInstance()
// Foo::getName()
//
// Foo::createInstance()
// Foo::getName()
//
// Bar::createInstance()
// Foo::getName()
//
// Foo::createInstance()
// Foo::getName()
// [       OK ] CxxStatic.check_in_inheritance (0 ms)

// see TEST(CxxOverride, check_condition_fail_when_no_virtual)
// NOTE
// as with usual override, inheritance, static member functions are no different
// even if it can make "stateless" class. members which are not defined in
// derived, they are inherited, that is, found by cxx-name-lookup

TEST(CxxStatic, check_in_override)
{
  using namespace cxx_static;

  Foo::createInstance();
  Foo::getName();

  Bar::createInstance();
  Bar::getName();

  Foo foo;
  foo.createInstance();
  foo.getName();

  Bar bar;
  bar.createInstance();
  bar.getName();

  // see foo's version since it is not virtual
  Foo *p = new Bar;
  p->createInstance();
  p->getName();
  delete p;
}

namespace cxxstatic
{
  class FooStatic_1
  {
  public:
    FooStatic_1() {}

    static int createInstance()
    {
      static int m_count = 0;

      m_count++;

      // std::cout << "FooStatic2::createInstance(): count: " << m_count
      //           << std::endl;

      return m_count;
    }
  };

  // from singleton implementation
  class FooStatic_2
  {
  private:
    std::string *m_name{nullptr};

    FooStatic_2(std::string *name)
    {
      m_name = name;
      std::cout << "m_name : " << m_name << std::endl;
    }

  public:
    static FooStatic_2 &getInstance(std::string *name = nullptr)
    {
      static FooStatic_2 instance(name);
      return instance;
    }

    ~FooStatic_2()
    {
      if (m_name)
        delete m_name;
    }

    FooStatic_2(const FooStatic_2 &) = delete;
    FooStatic_2 &operator=(const FooStatic_2 &) = delete;

    // return true if m_name is not null
    bool checkName() const { return m_name ? true : false; }
  };

} // namespace cxxstatic

// see that static variable is initialised once when it is created.

TEST(CxxStatic, check_initialised_once)
{
  using namespace cxxstatic;

  {
    EXPECT_THAT(1, FooStatic_1::createInstance());
    EXPECT_THAT(2, FooStatic_1::createInstance());
    EXPECT_THAT(3, FooStatic_1::createInstance());
  }

  // NOTE: since it's singleton, each test should be run one by one. that is,
  // comment out the rest and leave one; build and execute. repeat for each to
  // get pass.

  // as expected, call getInstance(pointer) first so that pointer is set.
  {
    FooStatic_2::getInstance(new std::string{"singleton"});
    EXPECT_THAT(FooStatic_2::getInstance().checkName(), true);
  }

  {
    FooStatic_2::getInstance(nullptr);
    EXPECT_THAT(FooStatic_2::getInstance().checkName(), false);
  }

  // if call getInstance() first then no way to set pointer.
  {
    FooStatic_2::getInstance();
    FooStatic_2::getInstance(new std::string{"singleton"});
    EXPECT_THAT(FooStatic_2::getInstance().checkName(), false);
  }
}

namespace cxx_static
{
  class FooThread
  {
  private:
    // error: ISO C++ forbids in-class initialization of non-const static
    // member ‘cxx_static::FooThread::m_value1’
    // static int m_value1{10};

    static int m_value1;
    static int m_value2;
    static std::vector<int> m_coll;
    std::string m_name;

    static FooThread *m_self;

  public:
    FooThread()
        : m_name("foo")
    {
      m_self = this;
    }
    ~FooThread() {}
    void print()
    {
      std::cout << "name   : " << m_name << ", value1 : " << m_value1
                << ", value2 : " << m_value2 << std::endl;

      std::cout << "coll   : " << m_coll[0] << ", " << m_coll[1] << ", "
                << m_coll[2] << ", " << m_coll[3] << std::endl;

      std::cout << "self   : " << m_self << std::endl;
    }
  };

  int FooThread::m_value1{10};
  int FooThread::m_value2{20};
  std::vector<int> FooThread::m_coll{10, 20, 30, 40};
  FooThread *FooThread::m_self{nullptr};
} // namespace cxx_static

// TEST(CxxStatic, static_in_thread)
// {
//   using namespace cxx_static;

//   FooThread foo;

//   // print from thread1
//   foo.print();

//   // print from thread2
//   auto f1 = std::async(std::launch::async, [&]() {
//     std::this_thread::sleep_for(chrono::milliseconds(1300));
//     foo.print();
//   });

//   // print from thread3
//   auto f2 = std::async(std::launch::async, [&]() {
//     std::this_thread::sleep_for(chrono::milliseconds(1300));
//     foo.print();
//   });

//   // print from thread1
//   foo.print();

//   f1.get();
//   f2.get();
// }

TEST(CxxStatic, static_in_thread)
{
  using namespace cxx_static;

  auto foo = make_shared<FooThread>();

  // print from thread1
  foo->print();

  // print from thread2
  auto f1 = std::async(std::launch::async, [&]() {
    std::this_thread::sleep_for(chrono::milliseconds(1300));
    foo->print();
  });

  // print from thread3
  auto f2 = std::async(std::launch::async, [&]() {
    std::this_thread::sleep_for(chrono::milliseconds(1300));
    foo->print();
  });

  // print from thread1
  foo->print();

  f1.get();
  f2.get();
}

// ={=========================================================================
// cxx-isspace

// isspace(' '): 8192
// isspace(0)  : 0
//
//       isspace()
//              checks for white-space characters.  In the "C" and "POSIX"
//              locales, these are: space, form-feed  ('\f'), newline ('\n'),
//              carriage return ('\r'), horizontal tab ('\t'), and vertical tab
//              ('\v').

TEST(CxxFeaturesTest, UseIsspace)
{
  cout << "isspace(' '): " << isspace(' ') << endl;
  cout << "isspace(0)  : " << isspace(0) << endl;
}

// ={=========================================================================
// cxx-callable cxx-predicate
TEST(CxxCallable, predefined)
{
  {
    std::plus<int> call;
    EXPECT_THAT(call(10, 20), 30);
  }

  {
    std::equal_to<int> call;
    EXPECT_THAT(call(10, 10), true);
    EXPECT_THAT(call(10, 20), false);
  }
}

// cxx-progress
//
// Q: If run it standalone, it runs slower than on in GTEST. WHY?
//
// #include <iostream>
// #include <chrono>
// #include <thread>
//
// using namespace std;
//
// typedef bool (*UPDATEFUNC)(int);
//
// bool UpdateProgress(int percent)
// {
//   cout << flush << "\r" << percent << " % complete...";
//   // cout << "\r" << percent << "% complete...";
//   return true;
// }
//
// int main()
// {
//   UPDATEFUNC f = UpdateProgress;
//
//   for (long l = 0; l < 100000000; ++l)
//   {
//     if (l % 1000000 == 0)
//       f(l / 1000000);
//
//     for (long x = 0; x < 100; ++x)
//       x = x;
//
//     // this_thread::sleep_for(std::chrono::milliseconds{1});
//   }
//
//   cout << endl;
//
//   return EXIT_SUCCESS;
// }

namespace cxx_function
{
  // typedef bool (*UPDATEFUNC)(int);
  using UPDATEFUNC = bool (*)(int);

  bool updateProgress(int percent)
  {
    std::cout << std::flush << "\r" << percent << "% complete...";
    return true;
  }
} // namespace cxx_function

// ={=========================================================================
TEST(CxxCallable, function_pointer)
{
  using namespace cxx_function;

  UPDATEFUNC f = updateProgress;

  for (long l = 0; l < 100000000; ++l)
  {
    if (l % 1000000 == 0)
      f(l / 1000000);

    for (long x = 0; l < 1000000; ++l)
      x = x;

    // this_thread::sleep_for(std::chrono::milliseconds{10});
  }

  std::cout << std::endl;
}

/*
// ={=========================================================================
However, we can’t simply assign the result of dlsym() to such a pointer, as in
the following example:

funcp = dlsym(handle, symbol);

The reason is that the C99 standard forbids assignment between a function
pointer and void *. The solution is to use the following (somewhat clumsy) cast:

*(void **) (&funcp) = dlsym(handle, symbol);

Having used dlsym() to obtain a pointer to the function, we can then call the
function using the usual C syntax for dereferencing function pointers:

res = (*funcp)(somearg);

*/

TEST(CxxCallable, function_pointer_cast)
{
  using namespace cxx_function;

  {
    void *func;

    // error: invalid conversion from ‘bool (*)(int)’ to ‘void*’ [-fpermissive]
    // func = updateProgress;

    func = (void *)updateProgress;
    func = *(void **)updateProgress;
    func = *((void **)(&updateProgress));

    // cast function pointer's address to void** and then dereference it to make
    // it "void *"

    // TODO:
    // since all three compiles well and check on dlsym context.
  }
}

namespace cxx_callable
{
  class Foo
  {
  public:
    Foo(size_t value = 0)
        : value_(value)
    {}

    void update_10() noexcept { value_ += 10; }

    void update_20() noexcept { value_ += 20; }

    void update_30() noexcept { value_ += 30; }

    void update(int value) noexcept { value_ += value; }

    size_t get_value() const noexcept { return value_; }

  private:
    size_t value_;
  };

  size_t print_value(Foo &foo) { return foo.get_value(); }

} // namespace cxx_callable

/*
// ={=========================================================================
NOTE NOTE on the syntax since "has to specify the target". 
For example, update_10() takes no argument but has to say, "Foo &":

std::function<void(Foo &)> op = &Foo::update_10;
                   ^^^^^
error: conversion from ‘std::__cxx11::string (cxx_init_list::Foo::*)() {aka
std::__cxx11::basic_string<char> (cxx_init_list::Foo::*)()}’ to non-scalar
type ‘std::function<std::__cxx11::basic_string<char>()>’ requested

    std::function<std::string()> f = &Foo::return_mesg;
*/

TEST(CxxCallable, function_template)
{
  using namespace cxx_callable;

  // expect that nullptr if not set
  // NOTE: EXPECT_THAT(op, nullptr); will make compile error
  {
    std::function<void(Foo &)> call;

    if (call == nullptr)
      EXPECT_THAT(true, true);
    else
      EXPECT_THAT(true, false);
  }

  // when use std::function, have to speficy `target`
  {
    Foo foo                         = Foo(100);
    std::function<void(Foo &)> call = &Foo::update_10;

    // specify the target object
    //
    // that performing a function call without having a 'target' to call throws
    // an exception of type std::bad_function_call (see Section 4.3.1, page 43):
    //
    // std::function<void(int,int)> f;
    //
    // f(33,66);
    //
    // throws std::bad_function_call

    call(foo);

    EXPECT_THAT(foo.get_value(), 110);
  }

  // compile error as `call` is typed as function pointer
  // {
  //   Foo foo = Foo(100);
  //   auto call = &Foo::update_10;
  //   call(foo);
  //   EXPECT_THAT(foo.get_value(), 110);
  // }

  // when use more args
  {
    Foo foo                              = Foo(100);
    std::function<void(Foo &, int)> call = &Foo::update;
    call(foo, 50);

    EXPECT_THAT(foo.get_value(), 150);
  }

  // the signature to std::function specify whether the argument is copy or
  // reference; target in this examples.
  //
  // std::function<void(Foo &)>
  // std::function<void(Foo)>
  {
    // note: use the same `foo`
    {
      Foo foo{100};

      std::function<void(Foo &)> fo1 = &Foo::update_20;
      fo1(foo);
      EXPECT_THAT(foo.get_value(), 120);

      fo1(foo);
      EXPECT_THAT(foo.get_value(), 140);

      // use copy and do not change
      std::function<void(Foo)> fo2 = &Foo::update_20;
      fo2(foo);
      EXPECT_THAT(foo.get_value(), 140);

      fo2(foo);
      EXPECT_THAT(foo.get_value(), 140);
    }

    // use reference
    {
      std::vector<Foo> coll = {Foo(1), Foo(2), Foo(3)};
      std::vector<size_t> result{};

      std::function<void(Foo &)> op = &Foo::update_20;

      std::for_each(coll.begin(), coll.end(), op);

      // to get result out and algo-transform requires unary predicate
      std::transform(coll.begin(),
                     coll.end(),
                     std::back_inserter(result),
                     print_value);

      EXPECT_THAT(result, ElementsAre(21, 22, 23));
    }

    // use copy so not updated
    {
      std::vector<Foo> coll = {Foo(1), Foo(2), Foo(3)};
      std::vector<size_t> result{};

      std::function<void(Foo)> op = &Foo::update_20;

      std::for_each(coll.begin(), coll.end(), op);

      // to get result out and cxx-transform requires unary predicate
      std::transform(coll.begin(),
                     coll.end(),
                     std::back_inserter(result),
                     print_value);

      EXPECT_THAT(result, ElementsAre(1, 2, 3));
    }
  }
}

// ={=========================================================================
// Use `cxx-mem-fn` to let the compiler deduce the member's type and to
// generate a callable object. The callable generated by `mem_fn` can be
// called on either an object or a pointer.
//
// mem_fn() *always*needs target and not flexible as std::bind().
//
// std::bind(&foo::closeConsole, this, std::placeholders::_1);
//
// the point is std::bind() is more flexible.

TEST(CxxCallable, memfn)
{
  using namespace cxx_callable;

  // when use std::function, have to speficy `target`
  {
    Foo foo = Foo(100);

    std::function<void(Foo &)> call = &Foo::update_10;

    call(foo);

    EXPECT_THAT(foo.get_value(), 110);
  }

  // *cxx-error* since no default ctor for std::mem_fn()
  // {
  //   auto op = std::mem_fn();
  // }

  // need target
  {
    Foo foo = Foo(100);

    auto call = std::mem_fn(&Foo::update_10);

    call(foo);

    EXPECT_THAT(foo.get_value(), 110);
  }

  // use args
  {
    Foo foo = Foo(100);
    auto op = std::mem_fn(&Foo::update);

    op(foo, 100);

    EXPECT_THAT(foo.get_value(), 200);
  }

  // cxx-error compile error
  // {
  //   Foo foo = Foo(100);
  //   auto op = std::mem_fn(&Foo::update_10, &foo);
  //   op();
  //   EXPECT_THAT(foo.get_value(), 110);
  // }

  // cxx-for-each() pass reference
  //
  // namespace algo_code
  // {
  //   template <typename _InputIterator, typename _Function>
  //   _Function
  //   for_each(_InputIterator __first, _InputIterator __last, _Function __f)
  //   {
  //     for (; __first != __last; ++__first)
  //       __f(*__first);
  //     return _GLIBCXX_MOVE(__f);
  //   }
  //
  //   template <typename _InputIterator,
  //             typename _OutputIterator,
  //             typename _UnaryOperation>
  //   _OutputIterator transform(_InputIterator __first,
  //                             _InputIterator __last,
  //                             _OutputIterator __result,
  //                             _UnaryOperation __unary_op)
  //   {
  //     for (; __first != __last; ++__first, ++__result)
  //       // note: write to output to output iterator and unary
  //       *__result = __unary_op(*__first);
  //     return __result;
  //   }
  // } // namespace algo_code
  {
    std::vector<Foo> coll = {Foo(1), Foo(2), Foo(3)};
    std::vector<size_t> result{};

    auto op = std::mem_fn(&Foo::update_10);

    std::for_each(coll.begin(), coll.end(), op);

    // to get result out and algo-transform requires unary predicate
    std::transform(coll.begin(),
                   coll.end(),
                   back_inserter(result),
                   print_value);

    EXPECT_THAT(result, ElementsAre(11, 12, 13));
  }

  // use pointer
  {
    Foo foo = Foo(100);

    // see how to define pointer to member function
    // error
    // void (*op)(void);
    void (Foo::*op)(void);

    // `Unlike ordinary function pointer, no automatic conversion` between a
    // member funtion and a pointer to that function.
    // must explicitly use address-of operator
    // error
    // op = Foo::update_10;
    op = &Foo::update_10;

    // see how to call
    // error: must use ‘.*’ or ‘->*’ to call pointer-to-member
    // function in ‘op (...)’, e.g. ‘(... ->* op) (...)’
    //      op(foo);
    //            ^
    // op(foo);
    //
    // When initialize a pointer to member, that pointer does 'not' yet point to
    // any data. Supply the object when we dereference that pointer. Analogous
    // to the member access operators, . and ->,
    (&foo->*op)();

    EXPECT_THAT(foo.get_value(), 110);
  }

  {
    std::vector<decltype(&Foo::update_10)> coll{&Foo::update_10,
                                                &Foo::update_20,
                                                &Foo::update_30};

    Foo foo = Foo(100);

    // OK
    (&foo->*coll[0])();

    EXPECT_THAT(foo.get_value(), 110);
  }

  {
    auto f = std::mem_fn(&Foo::update_10);

    // std::initializer_list<decltype(f)> coll{std::mem_fn(&Foo::update_10),
    //                                         std::mem_fn(&Foo::update_20),
    //                                         std::mem_fn(&Foo::update_30)};

    auto coll = {std::mem_fn(&Foo::update_10),
                 std::mem_fn(&Foo::update_20),
                 std::mem_fn(&Foo::update_30)};

    Foo foo = Foo(100);

    auto it = coll.begin();

    (*it)(foo);

    EXPECT_THAT(foo.get_value(), 110);
  }

  // use std::function
  {
    // error: could not convert ‘{std::mem_fn<void() noexcept,
    // cxx_callable::Foo>(&cxx_callable::Foo::update_10), std::mem_fn<void()
    // noexcept, cxx_callable::Foo>(&cxx_callable::Foo::update_20),
    // std::mem_fn<void() noexcept,
    // cxx_callable::Foo>(&cxx_callable::Foo::update_30)}’ from ‘<brace-enclosed
    // initializer list>’ to ‘std::vector<std::function<void()> >’
    //
    // std::mem_fn(&Foo::update_30)};
    //
    // std::vector<std::function<void()>> coll = {std::mem_fn(&Foo::update_10),
    //                                            std::mem_fn(&Foo::update_20),
    //                                            std::mem_fn(&Foo::update_30)};

    std::vector<std::function<void(Foo &)>> coll = {
      std::mem_fn(&Foo::update_10),
      std::mem_fn(&Foo::update_20),
      std::mem_fn(&Foo::update_30)};

    Foo foo = Foo(100);

    coll[0](foo);

    EXPECT_THAT(foo.get_value(), 110);
  }
}

namespace cxx_callable
{
  bool check_size(std::string const &s, std::string::size_type size)
  {
    return s.size() >= size;
  }

  bool shorter_than_lhs(std::string const &lhs, std::string const &rhs)
  {
    return lhs.size() > rhs.size();
  }

  void increase(int &i) { ++i; }
} // namespace cxx_callable

// ={=========================================================================
// *cxx-bind*

TEST(CxxCallable, bind)
{
  using namespace cxx_callable;

  // use *cxx-callable-predefined* and it is not template function
  //
  // cxx-error: no matching function for call to ‘std::plus<int>::plus(int, int)’
  // {
  //   EXPECT_THAT(std::plus<int>(10, 20), 30);
  // }
  {
    std::plus<int> call;
    EXPECT_THAT(call(10, 20), 30);
  }

  // cxx-min, template function
  {
    EXPECT_THAT(std::min(10, 20), 10);
  }

  // expect true if size >= 6
  {
    auto call = std::bind(check_size, std::placeholders::_1, 6);

    EXPECT_THAT(call("hello"), false);
    EXPECT_THAT(call("hello!"), true);
  }

  // show when bind() composites
  {
    std::vector<std::string> coll{"bind", "do", "wonders"};

    size_t size{6};

    // use lambda
    auto found1 =
      std::find_if(coll.cbegin(), coll.cend(), [size](std::string const &e) {
        return e.size() >= size;
      });

    auto found2 =
      std::find_if(coll.cbegin(), coll.cend(), [](std::string const &e) {
        return e.size() >= 6;
      });

    // use bind
    auto found3 =
      std::find_if(coll.cbegin(),
                   coll.cend(),
                   std::bind(check_size, std::placeholders::_1, size));

    auto found4 = std::find_if(coll.cbegin(),
                               coll.cend(),
                               std::bind(check_size, std::placeholders::_1, 6));

    EXPECT_THAT(*found1, "wonders");
    EXPECT_THAT(*found1, *found3);
    EXPECT_THAT(*found2, *found4);

    // don't have to use arguments to bind() and found the first as function
    // object always returns true
    auto found5 = std::find_if(coll.cbegin(),
                               coll.cend(),
                               std::bind(check_size, "wonders", 6));

    EXPECT_THAT(*found5, "bind");
  }

  // show when bind() changes order of arguments
  {
    auto fo1 =
      std::bind(shorter_than_lhs, std::placeholders::_1, std::placeholders::_2);
    auto fo2 =
      std::bind(shorter_than_lhs, std::placeholders::_2, std::placeholders::_1);

    EXPECT_THAT(fo1("bind", "wonders"), false);
    EXPECT_THAT(fo2("bind", "wonders"), true);
  }

  // cxx-bind-nested
  {
    std::plus<int> fo;
    EXPECT_THAT(fo(7, 10), 17);

    // cxx-bind and _1 holds the first argument
    auto plus10 = std::bind(std::plus<int>(), std::placeholders::_1, 10);
    EXPECT_THAT(plus10(7), 17);

    // 17*2 = 34. see that inner fobj itself is used as argument
    // NOTE: how bind() works when the first param is callable?
    auto plus10times2 =
      std::bind(std::multiplies<int>(),
                std::bind(std::plus<int>(), std::placeholders::_1, 10),
                2);
    EXPECT_THAT(plus10times2(7), 34);

    // 49*7 = 343. used multiple times
    auto pow3 = std::bind(std::multiplies<int>(),
                          std::bind(std::multiplies<int>(),
                                    std::placeholders::_1,
                                    std::placeholders::_1),
                          std::placeholders::_1);
    EXPECT_THAT(pow3(7), 343);

    auto inversDivide = std::bind(std::divides<double>(),
                                  std::placeholders::_2,
                                  std::placeholders::_1);
    EXPECT_NEAR(inversDivide(49, 7), 0.142857, 0.001);
  }

  {
    std::set<int, std::greater<int>> coll1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::deque<int> coll2;

    // initialised
    EXPECT_THAT(coll1, ElementsAre(9, 8, 7, 6, 5, 4, 3, 2, 1));

    // *algo-transform*
    std::transform(
      coll1.cbegin(),
      coll1.cend(),                               // source
      std::back_inserter(coll2),                  // destination
      std::bind(std::multiplies<int>(), _1, 10)); // unary operation

    // transformed
    EXPECT_THAT(coll2, ElementsAre(90, 80, 70, 60, 50, 40, 30, 20, 10));

    // algo-replace-if
    std::replace_if(coll2.begin(),
                    coll2.end(),                        // range
                    std::bind(equal_to<int>(), _1, 70), // criterion
                    42);                                // new value

    // replaced
    EXPECT_THAT(coll2, ElementsAre(90, 80, 42, 60, 50, 40, 30, 20, 10));

    // *algo-remove-if*
    //
    // The expressions are evaluated 'from' the 'inside' to the outside. So
    // inner callable "returns 'bool' type" and outer callable uses that as
    // element type. No function call involved. That's why logical_and<bool>().
    //
    // template<typename _Tp>
    // struct logical_and : public binary_function<_Tp, _Tp, bool>
    // {
    //   bool
    //   operator()(const _Tp& __x, const _Tp& __y) const
    //   { return __x && __y; }
    // };
    //
    // return bind(greater_equal<int>(), _1, 50) && bind(less_equal<int>(), _1,
    // 80);
    //
    // __x is not "bind(greater_equal<int>(), _1, 50)" and is return value.

    coll2.erase(
      std::remove_if(coll2.begin(),
                     coll2.end(),
                     std::bind(logical_and<bool>(),
                               std::bind(greater_equal<int>(), _1, 50),
                               std::bind(less_equal<int>(), _1, 80))),
      coll2.end());

    // replaced
    EXPECT_THAT(coll2, ElementsAre(90, 42, 40, 30, 20, 10));
  }

  // std::placeholders:: How it works?
  {
    using namespace std::placeholders;

    EXPECT_THAT(std::bind(std::plus<int>(), _1, 10)(30), 40);
    EXPECT_THAT(std::bind(std::plus<int>(), _1, 10)(30, 40), 40);
    EXPECT_THAT(std::bind(std::plus<int>(), _1, 10)(30, 40, 50), 40);
    EXPECT_THAT(std::bind(std::plus<int>(), _2, 10)(30, 40, 50), 50);
    EXPECT_THAT(std::bind(std::plus<int>(), _3, 10)(30, 40, 50), 60);
  }

  // cxx-bind-argument-type
  // Always have to specify the argument type of the predefined function object
  // used. If the type doesn't match, a type conversion is forced, or the
  // expression results in a compile-time error.
  {
    using namespace std::placeholders;

    EXPECT_THAT(std::bind(std::plus<int>(), _1, 10)(30), 40);

    // cxx-error
    //
    // :31:42: error: no match for call to '(std::_Bind_helper<false,
    //   std::plus<std::basic_string<char> >, const std::_Placeholder<1>&,
    //   int>::type {aka std::_Bind<std::plus<std::basic_string<char>
    //   >(std::_Placeholder<1>,int)>}) (int)'
    //
    // In file included from /usr/include/c++/4.7/bits/stl_algo.h:68:0,
    //                  from /usr/include/c++/4.7/algorithm:63,
    //                  from t_bind.cpp:5:
    // /usr/include/c++/4.7/functional:1140:11: note: candidates are:
    //
    // EXPECT_THAT(std::bind(std::plus<std::string>(), _1, 10)(30), 40);
  }
}

// ={=========================================================================
// *cxx-bind* on member functions
TEST(CxxCallable, bind_and_member_function)
{
  using namespace cxx_callable;

  // use std::function<> but have to use target
  {
    Foo foo{100};
    std::function<void(Foo &)> call = &Foo::update_10;

    call(foo);

    EXPECT_THAT(foo.get_value(), 110);
  }

  // use cxx-bind and use target as well
  {
    Foo foo{100};
    auto call = std::bind(&Foo::update_10, _1);

    // see that pass `foo` object to op
    call(foo);

    EXPECT_THAT(foo.get_value(), 110);
  }

  // use cxx-bind and set target in bind call so don't need to set target when
  // call.
  //
  // error: no match for call to ‘(std::function<void(cxx_callable::Foo&)>) ()’
  // when use since call() requires a target:
  //
  // std::function<void(Foo &)> call = &Foo::update_10;
  // call();
  {
    Foo foo{100};
    auto call = std::bind(&Foo::update_10, &foo);

    // see that no target object
    // NOTE: depending on how a callable is used; call() or call(foo), callable
    // has also a different form.

    call();

    EXPECT_THAT(foo.get_value(), 110);
  }

  // void update(int value) noexcept;
  {
    Foo foo = Foo(100);
    std::function<void(int)> call =
      std::bind(&Foo::update, &foo, std::placeholders::_1);

    call(50);

    EXPECT_THAT(foo.get_value(), 150);
  }

  // `specify the target object`
  //
  // that performing a function call without having a target to call throws
  // an exception of type std::bad_function_call (see Section 4.3.1, page 43):
  //
  // std::function<void(int,int)> f;
  //
  // f(33,66);
  //
  // throws std::bad_function_call
  //
  // *cxx-this*
  //
  // As with *cxx-this*, same as Foo::update_10(&foo);
  //
  // Member function access the object through an extra, implicit parameter
  // named `this`. The compiler passes the address of `total` to the implicit
  // `this` parameter `as if` rewrites this call as:
  //
  // total.isbn();
  //
  // (where Sales_data::isbn(Sales_data *const this);)
  //
  // Sales_data::isbn(&total);
}

// ={=========================================================================
// *cxx-bind* cxx-bind and reference
// shows that bind() internally uses copy of passed arguments.
//
// void increase(int &i) { ++i; }
//
// NOTE: see TEST(EventLoop, check_invoke_do_copy);

TEST(CxxCallable, bind_and_copy)
{
  using namespace cxx_callable;

  {
    int value{};

    std::bind(increase, value)(); // same as fo(value) but no effect on value
    std::bind(increase, value)(); // same as fo(value) but no effect on value

    EXPECT_THAT(value, 0);

    // *cxx-ref*
    std::bind(increase, std::ref(value))(); // increased
    std::bind(increase, std::ref(value))();

    EXPECT_THAT(value, 2);
  }

  // it is when function object is called, that function object gets called
  // with reference within cxx-for-each
  //
  // template<typename _InputIterator, typename _Function>
  //   _Function
  //   for_each(_InputIterator __first, _InputIterator __last, _Function __f)
  //   {
  //     // note: call op but not use return of __f
  //
  //     for (; __first != __last; ++__first)
  //       __f(*__first);
  //
  //     return _GLIBCXX_MOVE(__f);
  //   }
  {
    std::vector<Foo> coll = {Foo(1), Foo(2), Foo(3)};
    std::vector<size_t> result{};

    auto fo = std::bind(&Foo::update_10, _1);

    {
      std::for_each(coll.begin(), coll.end(), fo);

      // to get result out and algo-transform requires unary predicate
      std::transform(coll.begin(),
                     coll.end(),
                     std::back_inserter(result),
                     print_value);

      EXPECT_THAT(result, ElementsAre(11, 12, 13));
    }
  }

  {
    std::vector<Foo> coll = {Foo(1), Foo(2), Foo(3)};
    std::vector<size_t> result{};

    std::function<void(Foo &)> op = &Foo::update_10;

    {
      std::for_each(coll.begin(), coll.end(), op);

      // to get result out and algo-transform requires unary predicate
      std::transform(coll.begin(),
                     coll.end(),
                     std::back_inserter(result),
                     print_value);

      EXPECT_THAT(result, ElementsAre(11, 12, 13));
    }
  }

  // from cxx-bind to cxx-function
  {
    Foo foo{100};
    auto fbind = std::bind(&Foo::update_10, _1);

    std::function<void(Foo &)> ffunc = fbind;

    ffunc(foo);

    EXPECT_THAT(foo.get_value(), 110);
  }
}

namespace cxx_callable
{
  class EventLoopPrivate
  {
  public:
    bool invoke(std::function<void()> &&f) const
    {
      if (f)
        f();

      // TODO?
      return true;
    }
  };

  class EventLoop
  {
  private:
    // std::shared_ptr<EventLoopPrivate> m_private;
    EventLoopPrivate m_private;

  public:
    explicit EventLoop()
    // : m_private(std::make_shared<EventLoopPrivate>())
    {}

    bool invokeImpl(std::function<void()> &&f) const
    {
      // NOTE:
      // WHY Error? is that because it is not template like invokeMethod?
      // or std::move() is required when forward calls to another class??
      //
      // error: cannot bind rvalue reference of type ‘std::function<void()>&&’
      // to lvalue of type ‘std::function<void()>’
      //
      //        return m_private.invoke(f);

      // ok:
      return m_private.invoke(std::move(f));
    }

    template <typename F>
    inline bool invokeMethod(F &&f) const
    {
      // do not make difference
      // return invokeImpl(std::move(f));
      return invokeImpl(f);
    }

    // do not make difference:
    // template <typename F, typename... Args>
    //   inline bool invokeMethod(F &&f, Args&&... args) const
    //   {
    //     return invokeImpl(
    //         std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    //   }

    template <typename F, typename... Args>
    inline bool invokeMethod(F &&f, Args &&... args) const
    {
      return invokeImpl(std::bind(f, std::forward<Args>(args)...));
    }
  };

  class Target
  {
  private:
    std::string m_name;
    int m_value;

  public:
    Target(std::string name = "target", int value = 0)
        : m_name(name)
        , m_value(value)
    {}

    std::string getName()
    {
      std::cout << "getName : " << m_name << std::endl;
      return m_name;
    }

    int getValue()
    {
      std::cout << "getValue : " << m_value << std::endl;
      return m_value;
    }

    void setNameAndValue(std::string name, int value)
    {
      m_name  = name;
      m_value = value;
      std::cout << "setNameAndValue : " << m_name << ", " << m_value
                << std::endl;
    }
  };
} // namespace cxx_callable

// ={=========================================================================
TEST(CxxCallable, bind_embedded_target)
{
  using namespace cxx_callable;

  EventLoop loop;
  Target t1("t1", 100);

  // target is embedded so void()
  loop.invokeMethod(std::bind(&Target::getValue, &t1));
  loop.invokeMethod(std::bind(&Target::getName, &t1));

  auto f1 = std::bind(&Target::getValue, &t1);
  loop.invokeMethod(f1);

  auto lambda = []() { std::cout << "lambda gets called" << std::endl; };

  // no target and void()
  loop.invokeMethod(lambda);

  loop.invokeMethod(&Target::setNameAndValue, &t1, "case", 200);

  // no since have to have target
  // loop.invokeMethod(&Target::setNameAndValue, "case", 200);
}

// ={=========================================================================
TEST(CxxCallable, Pointer)
{
  using namespace cxx_callable;

  // use pointer
  {
    Foo foo = Foo(100);

    // `Unlike ordinary function pointer, no automatic conversion` between a
    // member funtion and a pointer to that function.
    // must explicitly use address-of operator
    //
    // error
    // op = Foo::update_10;

    // see how to define pointer to member function
    void (Foo::*op)(void);
    op = &Foo::update_10;

    // cxx.cpp:2631:11: error: must use ‘.*’ or ‘->*’ to call pointer-to-member
    // function in ‘op (...)’, e.g. ‘(... ->* op) (...)’
    //      op(foo);
    //            ^
    // op(foo);

    // When initialize a pointer to member, that pointer does 'not' yet point to
    // any data. Supply the object when we dereference that pointer. Analogous
    // to the member access operators, . and ->,

    // see how to call
    Foo *pfoo = &foo;
    (pfoo->*op)();

    EXPECT_THAT(foo.get_value(), 110);

    // see how to call
    (foo.*op)();
    EXPECT_THAT(foo.get_value(), 120);
  }

#if 0 // to avoid warning
  // to show member function do not in object
  // f1 member address 0x43e3c4 | object address: 0x7ffeaf5f0c90
  // f2 member address 0x43e3c4 | object address: 0x7ffeaf5f0c80
  {
    Foo f1, f2;

    cout << "f1 member address " << (void *)(&Foo::update_10)
         << " | object address: " << (void *)&f1 << endl;

    cout << "f2 member address " << (void *)&Foo::update_10
         << " | object address: " << (void *)&f2 << endl;

    // as said above, this is error:
    // cxx.cpp:2676:42: error: invalid use of non-static member function ‘void
    // cxx_function::Foo::update_10()’ 
    //
    // cout << "f2 member address " << Foo::update_10 <<
  }
#endif
}

// ={=========================================================================
// cxx-lambda
TEST(CxxCallable, lambda_capture_and_return_1)
{
  {
    auto func = []() {
      std::string value{"this is a callback"};
      return value;
    };

    EXPECT_THAT(func(), "this is a callback");
  }

  // parameter list is optional. deduced return is integer
  {
    auto func = [] { return 42; };

    EXPECT_THAT(func(), 42);
  }

  // deduced return is double
  {
    auto func = [] { return 42.0; };

    EXPECT_THAT(func(), 42.0);
  }

  // no deduction on return
  {
    auto func = []() -> double { return 42; };

    EXPECT_THAT(func(), 42.0);
  }

  // same as above but use cxx-lambda-arg
  {
    int x = 42;
    int y = 42;

    auto func = [](const int first, int &second) { ++second; };

    x = 77;
    func(x, y);
    func(x, y);

    EXPECT_THAT(x, 77);
    EXPECT_THAT(y, 44);
  }
}

// ={=========================================================================
// the both causes:
// warning: control reaches end of non-void function [-Wreturn-type]
//
// the case was when thought it's lambda returning nothing, causes this warning
// and it turns out that somewhere in the middle of lambda, there is code return
// boolean so warning.
//
// If add "return;" at the end of lambda to say it returns nothing, becomes
// error
// error: inconsistent types ‘bool’ and ‘void’ deduced for lambda return type

TEST(CxxCallable, lambda_capture_and_return_2)
{
  {
    auto f1 = [] {
      std::string value{"this is a callback"};

      if (value.empty())
        return true;

      // return;
    };

    f1();
  }

  {
    auto f1 = [] {
      std::string value{"this is a callback"};

      if (!value.empty())
        return true;

      std::cout << "value: " << value << std::endl;
    };

    f1();
  }
}

/*
// ={=========================================================================
When define lambda, compiler generates a new `unnamed class` so when pass it to
a function, defines both a new type and an object of that type. So the data
member of a lambda are initialized when a lambda object is created. That means
that `the variable captured by a lambda is 'local' to a lambda.`

// value or reference

v1 = 42;
auto f = [v1] { return v1; };
v1 = 0;
auto j = f();     // j is 42. f is callable

v1 = 42;
auto f = [&v1] { return v1; };
v1 = 0;
auto j = f();     // j is 0.

Unlike parameters, the value of captured variable is `copied` when the lambda is
`created, not when it is called (executed).` When pass lambda such as return a
lambda, careful for the same reason that a function must not return a
reference to a local variable. If possible, avoid capturing pointers or
reference.

*/

TEST(CxxCallable, lambda_capture)
{
  // copy. lambda uses copy and caputre it when it's defined.
  {
    int x{42};

    auto call = [x] {
      EXPECT_THAT(x, 42);
      EXPECT_THAT(x, 42);
    };

    x = 77;

    call();
    call();

    EXPECT_THAT(x, 77);
  }

  // reference
  {
    int x{42};

    auto call = [&x] {
      EXPECT_THAT(x, 77);
      EXPECT_THAT(x, 77);
    };

    x = 77;

    call();
    call();

    EXPECT_THAT(x, 77);
  }
}

// ={=========================================================================
TEST(CxxCallable, lambda_mutable)
{
  // CXXSLR-3.1.10 Lambdas
  // [=] means that the outer scope is passed to the lambda by value. Thus, you
  // can `read` but not modify all data that was readable where the lambda was
  // defined.

  // x is read and y is write
  {
    int x = 42;
    int y = 42;

    auto call = [x, &y] {
      ++y;

      // *cxx-error*
      // cxx.cpp:2646:9: error: increment of read-only variable ‘x’
      //        ++x;
      // ++x;
    };

    x = 77;

    call();
    call();

    EXPECT_THAT(x, 77);
    EXPECT_THAT(y, 44);
  }

  // To have a mixture of passing by value and passing by reference, you can
  // declare the lambda as mutable. In that case, objects are passed by value,
  // but inside the function object defined by the lambda, you have write access
  // to the passed value.
  //
  // NOTE Ok but `x` do not change so what's the point???

  {
    int x = 42;
    int y = 42;

    auto func = [x, &y]() mutable {
      ++y;

      // no error since uses `mutable`
      ++x;
    };

    func();
    func();

    // not 44
    EXPECT_THAT(x, 42);
    EXPECT_THAT(y, 44);
  }

  // CPL 11.4.3.4
  {
    std::vector<int> coll{1, 2, 3, 4, 5};
    std::vector<int> result{0, 0, 0, 0, 0};

    int count = coll.size();

    EXPECT_THAT(count, 5);

    // generate() assigns the values that are generated by a call of op()
    // to each element in the range [beg,end).
    std::generate(result.begin(), result.end(), [count]() mutable {
      return --count;
    });

    EXPECT_THAT(count, 5);

    EXPECT_THAT(result, ElementsAre(4, 3, 2, 1, 0));
  }
}

// initialized  : over quick red fox jumps red the slow turtle the
// sorted       : fox jumps over quick red red slow the the turtle
// eliminated   : fox jumps over quick red slow the turtle
// stable sorted: fox red the over slow jumps quick turtle
// 3 words of length 5 or longer
// for_each     : fox red the over slow jumps quick turtle
// jumps quick turtle

namespace cxx_function_lambda
{
  template <typename T>
  inline void PRINT_ELEMENTS(const T &coll, const std::string &opt = "")
  {
    std::cout << opt;

    for (const auto &elem : coll)
      std::cout << elem << ' ';

    std::cout << std::endl;
  }

  // *algo-sort* *algo-unique*
  void eliminate_duplicates(vector<string> &words)
  {
    sort(words.begin(), words.end());
    auto unique_end = unique(words.begin(), words.end());
    words.erase(unique_end, words.end());
  }

  bool check_size(string const &s, string::size_type sz)
  {
    return s.size() >= sz;
  }

  // keep items in words which is >= sz
  void biggies(vector<string> &words, vector<string>::size_type sz)
  {
    eliminate_duplicates(words);

    // PRINT_ELEMENTS(words, "eliminated: ");

    sort(words.begin(), words.end(), [](string const &a, string const &b) {
      return a.size() < b.size();
    });

    // PRINT_ELEMENTS(words, "statle sorted: ");

    // get an iter of the first element whose size is >= sz

    // the problem is that find_if() uses *cxx-predicate* which is unary but we
    // need two args.
    //
    // *cxx-lambda* version
    //
    // error when use:
    //
    // [](string const& e)
    //
    // cxx.cpp:2254:30: error: ‘sz’ is not captured
    //          { return e.size() >= sz; });
    //                               ^
    //
    // works when use:
    //
    // [sz](string const& e)

    // auto wc = find_if(words.begin(), words.end(),
    //     [=](string const& e)
    //     { return e.size() >= sz; });

    // *cxx-bind* version
    auto wc =
      find_if(words.begin(), words.end(), std::bind(check_size, _1, sz));

    // get the number of elements that are its size >= sz
    // use *cxx-iter-arithmetic* since it's vector
    auto num = words.end() - wc;

    EXPECT_THAT(num, 3);

    vector<string> result{};

    // *cxx-lambda-arg*
    for_each(wc, words.end(), [&](string const &e) { result.push_back(e); });

    EXPECT_THAT(result, ElementsAre("jumps", "quick", "turtle"));
  }
} // namespace cxx_function_lambda

TEST(CxxCallable, lambda_Biggies)
{
  using namespace cxx_function_lambda;

  vector<string> coll{"over",
                      "quick",
                      "red",
                      "fox",
                      "jumps",
                      "red",
                      "the",
                      "slow",
                      "turtle",
                      "the"};

  // PRINT_ELEMENTS( coll, "initialized  : ");

  biggies(coll, 5);
}

// Suppose that you search in a collection for the first element with a value
// that is between x and y.

// If need to use in more than one or two places, use function than a lambda.
// However, it is not easy to write function to replace a lambda that captures
// local variables. For example, find_if takes unary predicate and see how to
// pass more than one as this example.

TEST(CxxCallable, lambda_Compare)
{
  deque<int> coll = {1, 3, 19, 5, 13, 7, 11, 2, 17};

  int x{5};
  int y{12};

  // use lambda
  {
    auto pos =
      find_if(coll.begin(), coll.end(), [=](int e) { return e > x && e < y; });

    EXPECT_THAT(*pos, 7);
  }

  // use cxx-bind
  {
    auto pos = find_if(coll.begin(),
                       coll.end(),                       // range
                       bind(logical_and<bool>(),         // search criterion
                            bind(greater<int>(), _1, x), // _1 > x
                            bind(less<int>(), _1, y)));  // _1 < y

    EXPECT_THAT(*pos, 7);
  }

  // before cxx-11, other ways to do

  // handwritten loop
  {
    deque<int>::iterator pos;

    for (pos = coll.begin(); pos != coll.end(); ++pos)
      if (*pos > x && *pos < y)
        break;

    EXPECT_THAT(*pos, 7);
  }

  // use cxx-fobj cxx-predicate
  {
    class Pred
    {
    public:
      Pred(int x, int y)
          : x_(x)
          , y_(y)
      {}
      bool operator()(int value) const { return value > x_ && value < y_; }

    private:
      int x_;
      int y_;
    };

    auto pos = find_if(coll.begin(), coll.end(), Pred(x, y));

    EXPECT_THAT(*pos, 7);
  }
}

// Passing C++ captureless lambda as function pointer to C API
// https://www.nextptr.com/tutorial/ta1188594113/passing-cplusplus-captureless-lambda-as-function-pointer-to-c-api
//
// This begs the question, what makes capture-less lambda expressions so
// different that they can be passed as function pointers?
//
// The function-call operator is const unless the lambda is declared mutable. A
// capture-less lambda also has a similar closure type except that there are no
// nonstatic data members in it. However, a capture-less lambda has an
// additional function-pointer conversion operator declared in it. It is that
// function-pointer conversion operator that makes a capture-less lambda
// convertible to a function pointer. The cppreference describes it as:
//
// This user-defined conversion function is only defined if the capture list of
// the lambda-expression is empty. It is a public, constexpr, (since C++17)
// non-virtual, non-explicit, const noexcept (since C++14) member function of
// the closure object.
//
// CPL 11.4.5: A lambda that captures nothing can be assigned to a pointer to
// funciton of an appropriate type.

namespace cxx_callable
{
  class Worker
  {
  private:
    pthread_t id_;
    bool stopped_{false};
    const std::string waits_{"\\|/-"};
    int count_{0};

    void run()
    {
      while (!stopped_)
      {
        std::cout << flush << "\r" << waits_[count_ % 5];
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        ++count_;
      }

      std::cout << std::endl;
    }

  public:
    int start()
    {
      return pthread_create(
        &id_,
        nullptr,
        [](void *self) -> void * {
          static_cast<Worker *>(self)->run();
          return nullptr;
        },
        this);
    }

    void stop()
    {
      stopped_ = true;
      pthread_join(id_, nullptr);
    }
  };

  // class Worker1
  // {
  // private:
  //   const std::string waits_{"\\|/-"};
  //   int count_{0};
  // public:
  //   void *run(void *)
  //   {
  //     while (count_ < 1000)
  //     {
  //       std::cout << flush << "\r" << waits_[count_ % 5];
  //       std::this_thread::sleep_for(std::chrono::milliseconds(10));
  //       ++count_;
  //     }
  //     std::cout << std::endl;
  //   }
  // };

  // int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
  // void*(*start_routine) (void *), void *arg);

  static void *func1(void *arg)
  {
    const char *s = (char *)arg;
    // cout << "thread id: " << std::this_thread::get_id() << endl;
    printf("%s, sleeps for 10s, getpid=%d\n", s, getpid());
    sleep(10);

    // return the length of input message
    return (void *)strlen(s);
  }
} // namespace cxx_callable

// shows the same pid but not tid
TEST(CxxCallable, lambda_and_c)
{
  using namespace cxx_callable;

  // use `capture-less` lambda
  {
    Worker w;

    if (0 == w.start())
    {
      std::this_thread::sleep_for(std::chrono::seconds(10));
      w.stop();
    }
  }

  // // can use member function?
  // // error: cannot convert ‘std::_Bind<void* (cxx_callable::Worker1::*(cxx_callable::Worker1*, std::_Placeholder<1>))(void*)>’
  // // to ‘void* (*)(void*)’ for argument ‘3’ to ‘int pthread_create(pthread_t*, const pthread_attr_t*, void* (*)(void*), void*)’
  // {
  //   Worker1 w;
  //   pthread_t t;
  //
  //   auto call = std::bind(&Worker1::run, &w, std::placeholders::_1);
  //
  //   pthread_create( &t, nullptr, call, (void*) &w);
  // }

  // static function works
  {
    char coll[] = "Hello world";
    pthread_t t;
    void *res;
    int s;

    EXPECT_THAT(strlen(coll), 11);

    // this works since it's static
    s = pthread_create(&t, NULL, func1, (void *)coll);
    if (s != 0)
    {
      printf("pthread_create failed");
      exit(EXIT_FAILURE);
    }

    printf("main pid = %d\n", getpid());

    s = pthread_join(t, &res);
    if (s != 0)
    {
      printf("pthread_join failed");
      exit(EXIT_FAILURE);
    }

    // get return
    EXPECT_THAT((long)res, 11);
  }

  // use cxx-lambda
  {
    char coll[] = "Hello world";
    pthread_t t;
    void *res;
    int s;

    EXPECT_THAT(strlen(coll), 11);

    // can pass any through "arg"
    // int Worker::start() {
    //  return
    //  pthread_create(&threadId_,nullptr,
    //   [](void* self) -> void* { // capture-less lambda
    //      static_cast<Worker*>(self)->run();
    //      return nullptr;
    //   },
    //   this);  // 'this' as argument to lambda
    // }

    s = pthread_create(
      &t,
      nullptr,
      [](void *arg) -> void * {
        const char *s = (char *)arg;
        // cout << "thread id: " << std::this_thread::get_id() << endl;
        printf("%s, sleeps for 10s, getpid=%d\n", s, getpid());
        sleep(10);

        // return the length of input message
        return (void *)strlen(s);
      },
      (void *)coll);

    if (s != 0)
    {
      printf("pthread_create failed");
      exit(EXIT_FAILURE);
    }

    printf("main pid = %d\n", getpid());

    s = pthread_join(t, &res);
    if (s != 0)
    {
      printf("pthread_join failed");
      exit(EXIT_FAILURE);
    }

    // get return
    EXPECT_THAT((long)res, 11);
  }
}

/* other example

  static void commandLineHandler(char *line);

  m_rl_callback_handler_install(prompt.toLatin1().constData(),
    commandLineHandler);

  to:

  m_rl_callback_handler_install(prompt.toLatin1().constData(),
      [](char *line) -> void
      {
        printf("sleeps for 10s\n");
      });
*/

// ={=========================================================================
// cxx-smart-ptr cxx-sp

namespace cxx_smart_pointer
{
  // private ctor
  class SmartFoo1
  {
  private:
    std::string m_name;

  private:
    explicit SmartFoo1()
    {
      std::cout << "SmartFoo1()" << std::endl;
      m_name = "foo1";
    }

    explicit SmartFoo1(const std::string name)
        : m_name(name)
    {
      std::cout << "SmartFoo1(std::string)" << std::endl;
    }

  public:
    ~SmartFoo1() = default;

    static std::shared_ptr<SmartFoo1> getFoo1();
    static std::shared_ptr<SmartFoo1> getFoo2();
  };

  std::shared_ptr<SmartFoo1> SmartFoo1::getFoo1()
  {
    return std::shared_ptr<SmartFoo1>();
  }

  std::shared_ptr<SmartFoo1> SmartFoo1::getFoo2()
  {
    // NOTE:
    // error: ‘cxx_smart_pointer::SmartFoo1::SmartFoo1()’ is private within this context
    // return std::make_shared<SmartFoo1>();

    // but this works
    return std::shared_ptr<SmartFoo1>(new SmartFoo1);
  }

  // public ctor
  class SmartFoo2
  {
  private:
    std::string m_name;

  public:
    explicit SmartFoo2(const std::string &name = "Foo")
        : m_name(name)
    {
      std::cout << "SmartFoo2::SmartFoo2: " << m_name << std::endl;
    }

    ~SmartFoo2()
    {
      std::cout << "SmartFoo2::~SmartFoo2: " << m_name << std::endl;
    }

    std::string getName() const { return m_name; }
  };
} // namespace cxx_smart_pointer

// ={=========================================================================
TEST(CxxSmartPointer, check_ctor)
{
  using namespace cxx_smart_pointer;

  {
    std::shared_ptr<std::string> pNico(new std::string("nico")); // OK
  }

  {
    std::shared_ptr<std::string> pNico{new std::string("nico")}; // OK
  }

  {
    std::shared_ptr<string> pNico = std::make_shared<string>("nico"); // OK
  }

  // // NO since requires cxx-copy-init but shared ptr's ctor are explicit
  // {
  //   shared_ptr<string> pNico = new string("nico");
  // }

  // this is *cxx-sp-ctor-empty* and do not create SmartFoo1. sp is null
  {
    std::shared_ptr<SmartFoo1> foo = std::shared_ptr<SmartFoo1>(); // OK
    EXPECT_THAT(foo, nullptr);
  }

  // cxx-error:
  // new_allocator.h:120:4:
  // error: ‘cxx_smart_pointer::SmartFoo1::SmartFoo1()’ is private within this context
  // {
  //   std::shared_ptr<SmartFoo1> foo = std::make_shared<SmartFoo1>();
  // }

  // have public ctors and see difference between using make_shared() and
  // shared_ptr(null)
  {
    std::shared_ptr<SmartFoo2> foo = std::shared_ptr<SmartFoo2>(); // OK
    EXPECT_THAT(foo, nullptr);
  }
  {
    std::shared_ptr<SmartFoo2> foo = std::make_shared<SmartFoo2>(); // OK
    EXPECT_THAT(foo, Ne(nullptr));
    EXPECT_THAT(foo->getName(), "Foo");
  }

  {
    std::shared_ptr<SmartFoo2> foo = std::make_shared<SmartFoo2>("foo2"); // OK
  }

  // to see how use std::string's constructors
  {
    // points to '9999999999'
    shared_ptr<string> p4 = std::make_shared<string>(10, '9');

    // points to empty vector<string>
    auto p6 = make_shared<vector<string>>();
  }

  // cxx-up
  {
    std::unique_ptr<std::string> up(new std::string);
    // causes compile error
    // EXPECT_THAT(up, true);

    // to get around, this works.
    EXPECT_THAT(!!up, true);

    // string itself is empty. std::string().empty()
    EXPECT_THAT(up->empty(), true);
  }

  // since *cxx-14* and
  // template< class T, class... Args >
  //  unique_ptr<T> make_unique( Args&&... args );
  //
  // https://gcc.gnu.org/projects/cxx-status.html
  //
  // C++14 Support in GCC
  // GCC has full support for the previous revision of the C++ standard, which
  // was published in 2014.
  //
  // This mode is the default in GCC 6.1 and above; it can be explicitly
  // selected with the -std=c++14 command-line flag, or -std=gnu++14 to enable
  // GNU extensions as well.

  {
    auto up = std::make_unique<std::string>("unique pointer");
    EXPECT_THAT(*up, "unique pointer");
  }

  // construct sp from up
  {
    std::unique_ptr<std::string> up = std::make_unique<std::string>("unique");

    // up is null once sp is constructed. have to be rvalue
    std::shared_ptr<std::string> sp(std::move(up));

    // up do not have use_count()
    // EXPECT_THAT(up.use_count(), 0);
    EXPECT_THAT(!!up, false);
    EXPECT_THAT(sp.use_count(), 1);
  }

  {
    // 1. must be static since cannot get object to call getFoo().
    // 2. so SmartFoo can only be created from this
    // 3. cxx-move

    // but getFoo1 is wrong implementation
    std::shared_ptr<SmartFoo1> sp1 = SmartFoo1::getFoo1();
    EXPECT_THAT(sp1, nullptr);

    std::shared_ptr<SmartFoo1> sp2 = SmartFoo1::getFoo2();
    EXPECT_THAT(sp2, Not(nullptr));
  }
}

TEST(CxxSmartPointer, check_copy_1)
{
  auto p = std::make_shared<int>(42);

  // p.use++
  auto q(p);

  // all prints 2 since it's in the same group
  EXPECT_THAT(p.use_count(), 2);
  EXPECT_THAT(q.use_count(), 2);

  EXPECT_THAT(*p, 42);
  EXPECT_THAT(*q, 42);

  auto r = std::make_shared<int>(52);

  // q.use++ and r.use--. destroies a object which r pointed.
  r = q;

  // all prints 3 since all are in the same group
  EXPECT_THAT(p.use_count(), 3);
  EXPECT_THAT(q.use_count(), 3);
  EXPECT_THAT(r.use_count(), 3);

  EXPECT_THAT(*r, 42);
}

TEST(CxxSmartPointer, check_copy_2)
{
  auto p = std::make_shared<int>(42);

  // two p.use++
  auto q(p);
  auto r(p);

  // all prints 3 since it's in the same group
  EXPECT_THAT(p.use_count(), 3);
  EXPECT_THAT(q.use_count(), 3);

  EXPECT_THAT(*p, 42);
  EXPECT_THAT(*q, 42);

  // going to change use count? no
  r = q;
  q = p;
  p = r;
  r = q;

  // all prints 3 since all are in the same group
  EXPECT_THAT(p.use_count(), 3);
  EXPECT_THAT(q.use_count(), 3);
  EXPECT_THAT(r.use_count(), 3);
}

// it is okay to copy around as long as f1, f2 remains.

TEST(CxxSmartPointer, check_copy_3)
{
  using namespace cxx_smart_pointer;

  auto f1 = std::make_shared<SmartFoo2>("foo1");
  auto f2 = std::make_shared<SmartFoo2>("foo2");

  auto prev(f1);
  auto curr(f2);

  // all prints 2 for each group
  EXPECT_THAT(f1.use_count(), 2);
  EXPECT_THAT(f2.use_count(), 2);

  // prev = f2, curr = f1
  prev = curr;
  curr = f1;

  // all prints 2 for each group
  EXPECT_THAT(f1.use_count(), 2);
  EXPECT_THAT(f2.use_count(), 2);

  // prev = f1, curr = f2
  prev = curr;
  curr = f2;

  // all prints 2 for each group
  EXPECT_THAT(f1.use_count(), 2);
  EXPECT_THAT(f2.use_count(), 2);
}

// o Double delete *cxx-sp-shared-ownership*
//
// Have to ensure that only `one group of shared pointers` owns an object.
//
// int* p = new int;
// shared_ptr<int> sp1(p);
// shared_ptr<int> sp2(p);       // new shared ptr group and double delete.
//
// For this reason, you should always directly initialize a smart pointer the
// moment you create the object with its associated resource:
//
// shared_ptr<int> sp1(new int);
// shared_ptr<int> sp2(sp1);     // OK

TEST(CxxSmartPointer, check_group_or_ownership)
{
  {
    int *p = new int{42};

    // create sp1
    std::shared_ptr<int> sp1(p);

    // p.use++
    // As for "auto sp2(sp1);", create another sp in the same group
    auto sp2 = sp1;

    // all prints 2 since it's in the same group
    EXPECT_THAT(sp1.use_count(), 2);
    EXPECT_THAT(sp2.use_count(), 2);

    EXPECT_THAT(*sp1, 42);
    EXPECT_THAT(*sp2, 42);
  }

  {
    int *p = new int{42};

    // create sp1
    std::shared_ptr<int> sp1(p);

    // create sp1 and sp2 first
    std::shared_ptr<int> sp2;
    std::shared_ptr<int> sp3;

    // and set
    sp2 = sp1;
    sp3 = sp1;

    // all prints 2 since it's in the same group
    EXPECT_THAT(sp1.use_count(), 3);
    EXPECT_THAT(sp2.use_count(), 3);

    EXPECT_THAT(*sp1, 42);
    EXPECT_THAT(*sp2, 42);
  }

  {
    int *p = new int{42};

    // create sp1
    std::shared_ptr<int> sp1(p);

    // However, this creates another group/owner of sp and cause `double delete`
    std::shared_ptr<int> sp2(p);

    // all prints 1 since it's NOT in the same group
    EXPECT_THAT(sp1.use_count(), 1);
    EXPECT_THAT(sp2.use_count(), 1);

    EXPECT_THAT(*sp1, 42);
    EXPECT_THAT(*sp2, 42);
  }
}

// common for shared and unique
TEST(CxxSmartPointer, check_reset)
{
  // 1. sp, shared structure, and referenced object are *separate* entity.
  //
  // 2. although shared count is 2, q.use_count() return 0 since cxx-sp-code
  // returns 0 when sp is empty(null)
  //
  // EXPECT_THAT(q.use_count(), 0);

  {
    auto p = std::make_shared<int>(42);

    // use++
    auto q(p);
    auto r(p);

    // all are in the same group
    EXPECT_THAT(p.use_count(), 3);
    EXPECT_THAT(q.use_count(), 3);

    // q is out
    q.reset();

    EXPECT_THAT(p.use_count(), 2);

    // shows q.reset() do independantly
    EXPECT_THAT(q.use_count(), 0);

    EXPECT_THAT(r.use_count(), 2);
  }

  {
    auto p = std::make_shared<int>(42);

    // use++
    auto q(p);
    auto r(p);

    // all are in the same group
    EXPECT_THAT(p.use_count(), 3);
    EXPECT_THAT(q.use_count(), 3);

    // q is out and same as cxx-sp-reset()
    q = nullptr;

    EXPECT_THAT(p.use_count(), 2);
    EXPECT_THAT(q.use_count(), 0);
    EXPECT_THAT(r.use_count(), 2);
  }

  {
    auto p = std::make_shared<int>(42);

    // use++
    auto q(p);
    auto r(p);

    // all are in the same group
    EXPECT_THAT(p.use_count(), 3);
    EXPECT_THAT(q.use_count(), 3);

    // q is out and multiple reset() are fine
    q.reset();
    q.reset();
    q.reset();
    q.reset();

    EXPECT_THAT(p.use_count(), 2);
    EXPECT_THAT(q.use_count(), 0);
    EXPECT_THAT(r.use_count(), 2);
  }
}

// ={=========================================================================
// common
TEST(CxxSmartPointer, bool_support)
{
  {
    auto p = std::make_shared<int>(42);

    // use++
    auto q(p);
    auto r(p);

    // all are in the same group
    EXPECT_THAT(p.use_count(), 3);
    EXPECT_THAT(q.use_count(), 3);

    // q is out
    q.reset();

    EXPECT_THAT(p.use_count(), 2);
    EXPECT_THAT(q.use_count(), 0);
    EXPECT_THAT(r.use_count(), 2);

    EXPECT_TRUE(p);

    // cxx-sp-bool, q is nullptr
    EXPECT_FALSE(q);

    EXPECT_TRUE(r);
  }

  {
    std::unique_ptr<int> up{new int(100)};

    EXPECT_TRUE(up);

    // cxx-error on gmock matcher
    // EXPECT_THAT(up, nullptr);
    EXPECT_THAT(!!up, true);
    EXPECT_THAT(up != nullptr, true);

    EXPECT_THAT(*up, 100);
  }

  {
    std::unique_ptr<int> up{};

    EXPECT_FALSE(up);
    EXPECT_THAT(!!up, false);

    EXPECT_THAT(up == nullptr, true);
  }
}

// CXXSLR 5.2 Smart Pointers

TEST(CxxSmartPointer, check_shared_example_1)
{
  std::shared_ptr<std::string> pNico{new std::string("nico")};
  std::shared_ptr<std::string> pJutta{new std::string("jutta")};

  // capitalise the first, cxx-string-replace

  (*pNico)[0] = 'N';
  pJutta->replace(0, 1, "J");

  // put them multiple times in a coll

  std::vector<std::shared_ptr<std::string>> whoMadeCoffee;
  whoMadeCoffee.push_back(pJutta);
  whoMadeCoffee.push_back(pJutta);
  whoMadeCoffee.push_back(pNico);
  whoMadeCoffee.push_back(pJutta);
  whoMadeCoffee.push_back(pNico);

  {
    std::ostringstream os;
    for (auto const &sp : whoMadeCoffee)
      os << *sp << ",";

    EXPECT_THAT(os.str(), "Jutta,Jutta,Nico,Jutta,Nico,");
  }

  // overwrite name

  *pNico = "Nicolai";

  {
    std::ostringstream os;
    for (auto const &sp : whoMadeCoffee)
      os << *sp << ",";

    EXPECT_THAT(os.str(), "Jutta,Jutta,Nicolai,Jutta,Nicolai,");
  }

  EXPECT_THAT(whoMadeCoffee[0].use_count(), 4);
}

namespace cxx_smart_pointer
{
  class SmartBase1
  {
  public:
    virtual ~SmartBase1() = default;
    virtual std::string getName() { return "It's SmartBase1"; }
  };

  class SmartDerived1 : public SmartBase1
  {
  public:
    virtual ~SmartDerived1() = default;
    std::string getName() override { return "It's SmartDerived1"; }
  };

  class SmartBase2
  {
  public:
    virtual ~SmartBase2()         = default;
    virtual std::string getName() = 0;
  };

  class SmartDerived2 : public SmartBase2
  {
  public:
    virtual ~SmartDerived2() = default;
    std::string getName() override { return "It's SmartDerived2"; }
  };
} // namespace cxx_smart_pointer

TEST(CxxSmartPointer, check_under_inheritance)
{
  using namespace cxx_smart_pointer;

  {
    std::shared_ptr<SmartBase1> bsp = std::make_shared<SmartDerived1>();

    EXPECT_THAT(bsp->getName(), "It's SmartDerived1");
  }

  {
    std::shared_ptr<SmartBase2> bsp = std::make_shared<SmartDerived2>();

    EXPECT_THAT(bsp->getName(), "It's SmartDerived2");
  }
}

// ={=========================================================================
// cxx-smart-ptr cxx-up

#if 0
:10:27: error: use of deleted function ‘std::unique_ptr<_Tp,
  _Dp>::unique_ptr(const std::unique_ptr<_Tp, _Dp>&) [with _Tp =
  std::basic_string<char>; _Dp = std::default_delete<std::basic_string<char> >;
  std::unique_ptr<_Tp, _Dp> = std::unique_ptr<std::basic_string<char> >]’

:12:8: error: use of deleted function ‘std::unique_ptr<_Tp, _Dp>&
std::unique_ptr<_Tp, _Dp>::operator=(const std::unique_ptr<_Tp, _Dp>&) [with _Tp
= std::basic_string<char>; _Dp = std::default_delete<std::basic_string<char> >;
std::unique_ptr<_Tp, _Dp> = std::unique_ptr<std::basic_string<char> >]’

TEST(CxxSmartPointer, check_unique_not_support_copy)
{
  {
    std::unique_ptr<std::string> p1(new std::string("nico"));

    // *cxx-error*
    unique_ptr<std::string> p2(p1);


    unique_ptr<std::string> p3;

    // *cxx-error*
    p3 = p2;
  }

  {
    // cxx.cpp:1696:36: error: conversion from ‘std::string* {aka
    // std::basic_string<char>*}’ to non-scalar type
    // ‘std::unique_ptr<std::basic_string<char> >’ requested

    unique_ptr<std::string> p1 = new string;
  }
}
#endif

namespace cxx_sp_shared
{
  int dtor_count{};

  class Foo
  {
    // intentionally made it public
  public:
    int id;

  public:
    Foo(int val = 1)
        : id(val)
    {
      cout << "Foo ctor(" << id << ")" << endl;
    }

    ~Foo()
    {
      dtor_count++;
      cout << "Foo dtor(" << id << ")" << endl;
    }

    int getId() const { return id; }
  };
} // namespace cxx_sp_shared

#if 0
// see unique_ptr<const T>

TEST(CxxSmartPointer, check_unique_const)
{
    using namespace cxx_sp_shared;

    // ok
    {
      std::unique_ptr<Foo> p1(new Foo(1));

      p1->id = 100;

      EXPECT_THAT(p1->getId(), 100);
    }

    // now it causes compile error
    {
      std::unique_ptr<const Foo> p1(new Foo(1));

      // error: assignment of member ‘cxx_sp_shared::Foo::id’ in read-only
      // object

      p1->id = 100;

      EXPECT_THAT(p1->getId(), 100);
    }
}
#endif

TEST(CxxSmartPointerUnique, check_move_assign_1)
{
  using namespace cxx_sp_shared;

  std::unique_ptr<Foo> p1(new Foo(1));
  std::unique_ptr<Foo> p2(new Foo(2));

  // f1 is gone. p1->f2. p2 is null
  p1 = std::move(p2);

  EXPECT_TRUE(p1);
  EXPECT_FALSE(p2);
}

TEST(CxxSmartPointerUnique, check_move_assign_2)
{
  // Foo ctor(1)
  // Foo ctor(2)
  // Foo dtor(1)
  // Foo ctor(3)
  // Foo dtor(2)
  // Foo dtor(3)

  {
    using namespace cxx_sp_shared;

    // reset global counter
    cxx_sp_shared::dtor_count = 0;

    // empty up
    std::unique_ptr<Foo> up;

    // not support copy assign for lvalue but support for rvalue
    up = std::move(std::unique_ptr<Foo>(new Foo(1)));
    up = std::move(std::unique_ptr<Foo>(new Foo(2)));
    // Foo(1) is gone
    EXPECT_THAT(dtor_count, 1);

    up = std::move(std::unique_ptr<Foo>(new Foo(3)));
    // Foo(2) is gone
    EXPECT_THAT(dtor_count, 2);

    up = nullptr;
    // Foo(3) is gone
    EXPECT_THAT(dtor_count, 3);
  }

  // Foo ctor(1)
  // Foo ctor(2)
  // Foo ctor(3)
  // Foo ctor(4)
  //
  // Foo dtor(2)
  // Foo dtor(1)
  // Foo dtor(4)
  // Foo dtor(3)

  {
    using namespace cxx_sp_shared;

    std::unique_ptr<Foo> p1(new Foo(1));
    std::unique_ptr<Foo> p2(new Foo(2));
    std::unique_ptr<Foo> p3(new Foo(3));
    std::unique_ptr<Foo> p4(new Foo(4));

    EXPECT_TRUE(p3);

    // f2 is gone, p2->f3, p3 is null
    p2 = std::move(p3);
    EXPECT_FALSE(p3);

    // p3->f1, p1 is null
    p3 = std::move(p1);

    // f1 is gone, p3 is null since p1 is already null
    p3 = std::move(p1);
    EXPECT_FALSE(p3);
  }
}

namespace cxx_sp_shared
{
  unique_ptr<Foo> source()
  {
    unique_ptr<Foo> ret(new Foo);
    cout << "source: create up" << endl;
    cout << "source: owns " << ret.get() << endl;
    cout << "source: ends" << endl;

    // *cxx-return-cxx-move*
    return ret;
  }

  void sink(unique_ptr<Foo> p)
  {
    cout << "sink: owns " << p.get() << endl;
    cout << "sink: ends" << endl;
  }
} // namespace cxx_sp_shared

// [ RUN      ] CxxFeaturesTest.UseUniqueSinkSource
// call source()
// Foo ctor(1)
// source: create up
// source: owns 0x21d1be0
// source: ends
// main: owns 0x21d1be0
// sink: owns 0x21d1be0
// sink: ends
// Foo dtor(1)
// main: ends
// [       OK ] CxxFeaturesTest.UseUniqueSinkSource (0 ms)

TEST(CxxSmartPointerUnique, check_sink_source_pattern)
{
  using namespace cxx_sp_shared;

  cout << "call source()" << endl;
  unique_ptr<Foo> up = source();

  cout << "main: owns " << up.get() << endl;

  sink(move(up));

  cout << "main: ends" << endl;
}

// use unique_ptr with stl collections
TEST(CxxSmartPointerUnique, check_use_with_coll)
{
  std::vector<std::unique_ptr<std::string>> coll;

  // compile error since used copy context
  // {
  //   std::unique_ptr<std::string> up(new std::string("unique"));
  //   coll.push_back(up);
  // }

  // no compile error since used move context
  {
    std::unique_ptr<std::string> up1(new std::string("unique"));
    coll.push_back(std::move(up1));
  }

  // no compile error
  {
    coll.emplace_back(new std::string("unique"));
  }

  // no compile error
  {
    coll.push_back(std::make_unique<std::string>("unique"));
  }
}

namespace cxx_sp_delete
{
  class ClassA
  {
  public:
    ClassA(std::string mesg = {})
        : m_mesg(mesg)
    {}

    std::string getMessage() const { return m_mesg; }

  private:
    std::string m_mesg;
  };

  // Since there is no way to get return from operator() becuase it called
  // unique_ptr code inside, have to use outputting message.
  class DebugDeleteClassA
  {
  public:
    DebugDeleteClassA(ostream &os = cerr)
        : m_os(os)
    {}

    void operator()(ClassA *p)
    {
      m_os << p->getMessage() << " is deleted" << std::endl;
      delete p;
    }

  private:
    // *cxx-reference-member*
    ostream &m_os;
  };

  // generic version since T will be deduced in *cxx-template-member*
  class DebugDelete
  {
  public:
    DebugDelete(ostream &os = cerr)
        : m_os(os)
    {}

    template <typename T>
    void operator()(T *p)
    {
      m_os << p->getMessage() << " is deleted" << std::endl;
      delete p;
    }

  private:
    ostream &m_os;
  };

  void function_delete(std::string *str)
  {
    if (str == nullptr)
      std::cout << "str is null" << std::endl;

    std::cout << *str << " is deleted" << std::endl;
    delete str;
  }

  class DebugDelete2
  {
  public:
    void operator()(string *str)
    {
      std::cout << *str << " is deleted" << std::endl;
      delete str;
    }
  };

} // namespace cxx_sp_delete

TEST(CxxSmartPointer, check_deleter)
{
  using namespace cxx_sp_delete;

  // unique_ptr uses type of deleter and creates deleter object internally
  {
    std::unique_ptr<ClassA, DebugDeleteClassA> up(
      new ClassA("unique with deleter"));

    EXPECT_THAT(up->getMessage(), "unique with deleter");
  }

  // another unique_ptr ctor which explicitly creates deleter object.
  {
    std::unique_ptr<ClassA, DebugDeleteClassA> up(
      new ClassA("unique with deleter"),
      DebugDeleteClassA());

    EXPECT_THAT(up->getMessage(), "unique with deleter");
  }

  {
    std::unique_ptr<ClassA, DebugDelete> up(new ClassA("unique with deleter"),
                                            DebugDelete());

    EXPECT_THAT(up->getMessage(), "unique with deleter");
  }

  // *cxx-error*
  // {
  //   std::unique_ptr<std::string, decltype(function_delete)> up(new
  //   string("unique"), function_delete);
  // }

  // see that the way of using function pointer
  {
    std::unique_ptr<std::string, void (*)(std::string *)> up(
      new std::string("unique with function deleter1"),
      function_delete);
  }

  {
    using unique_ptr_with_function =
      std::unique_ptr<std::string, void (*)(std::string *)>;

    unique_ptr_with_function up(
      new std::string("unique with function deleter2"),
      function_delete);
  }

  // okay when nullptr is used? Yes since unique_ptr do have checks before
  // calling deleter
  {
    using unique_ptr_with_function =
      std::unique_ptr<std::string, void (*)(std::string *)>;

    unique_ptr_with_function up(nullptr, function_delete);
  }

  // shared pointer takes object as argument
  {
    std::shared_ptr<std::string> sp1(new std::string("nico on function"),
                                     function_delete);
    std::shared_ptr<std::string> sp2(new std::string("jutta on function"),
                                     function_delete);
  }

  {
    std::shared_ptr<std::string> sp1(new std::string("nico on functor"),
                                     DebugDelete2());

    std::shared_ptr<std::string> sp2(new std::string("jutta on functor"),
                                     DebugDelete2());
  }

  {
    // *cxx-lambda*
    std::shared_ptr<string> sp1(new string("nico"), [](string *str) {
      std::cout << *str << " is deleted" << std::endl;
      delete str;
    });
  }
}

// name: Jutta ,name: Jutta ,name: Nico ,name: Jutta ,name: Nico ,
// name: Jutta ,name: Jutta ,name: Nicolai ,name: Jutta ,name: Nicolai ,
// deleting Nicolai
// deleting Jutta

TEST(CxxSmartPointer, sp_DeleteTime)
{
  using namespace cxx_sp_delete;

  shared_ptr<string> pnico(new string("nico"), function_delete);
  shared_ptr<string> pjutta(new string("jutta"), function_delete);

  // uppercase the first char
  (*pnico)[0] = 'N';
  pjutta->replace(0, 1, "J");

  // put them in a container
  vector<shared_ptr<string>> coll;
  coll.push_back(pjutta);
  coll.push_back(pjutta);
  coll.push_back(pnico);
  coll.push_back(pjutta);
  coll.push_back(pnico);

  for (auto e : coll)
    cout << "name: " << *e << " ,";
  cout << endl;

  // overwrite name
  *pnico = "Nicolai";

  // print again to see changes
  for (auto e : coll)
    cout << "name: " << *e << " ,";
  cout << endl;

  // jutta's use count
  EXPECT_THAT(coll[0].use_count(), 4);

  pjutta = nullptr;

  EXPECT_THAT(coll[0].use_count(), 3);

  // pico's use count
  EXPECT_THAT(coll[2].use_count(), 3);

  coll.resize(2);

  pnico = nullptr;

  // WHY 0?
  //
  // *cxx-sp-shared-check-users* do not rely on use_count()
  // use_count() may not be reliable since C++P 453 reads "may be a slow
  // operation, intended primarily for debugging purpose"

  EXPECT_THAT(pnico.use_count(), 0);
  EXPECT_THAT(pjutta.use_count(), 0);
}

// Foo ctor(1)
// Foo ctor(2)
// Foo ctor(3)
// Foo dtor(2)    // p2.reset() frees p2
// -----------
// -----------    // p3.reset() frees nothing and just change referrring object
// Foo dtor(1)    // p3.reset() frees p1
// -----------
// end of main
// Foo dtor(3)

TEST(CxxSmartPointer, sp_unique_DeleteReleaseReset)
{
  using namespace cxx_sp_shared;

  unique_ptr<Foo> p1(new Foo(1));
  unique_ptr<Foo> p2(new Foo(2));
  unique_ptr<Foo> p3(new Foo(3));

  p2.reset(p3.release());

  // free int(2) and p3 is null

  EXPECT_TRUE(p1);
  EXPECT_TRUE(p2);
  EXPECT_FALSE(p3);

  cout << "-----------" << endl;

  p3.reset(p1.release());

  // p1 is null
  EXPECT_FALSE(p1);
  EXPECT_TRUE(p2);
  EXPECT_TRUE(p3);

  cout << "-----------" << endl;

  p3.reset(p1.release());

  // p1 is null
  EXPECT_FALSE(p1);
  EXPECT_TRUE(p2);
  EXPECT_FALSE(p3);

  cout << "-----------" << endl;

  cout << "end of main" << endl;
}

namespace cxx_sp_use_count
{
  class Foo : public std::enable_shared_from_this<Foo>
  {
  private:
    int id;

  public:
    Foo(int val)
        : id(val)
    {
      cout << "Foo ctor(" << id << ")" << endl;
    }

    void value() { std::cout << "Foo value: " << id << std::endl; }

    ~Foo() { cout << "Foo dtor(" << id << ")" << endl; }

    std::shared_ptr<Foo> getFoo() { return shared_from_this(); }
  };
} // namespace cxx_sp_use_count

// Is use_count() reliable?

TEST(CxxSmartPointer, sp_UseCount)
{
  using namespace cxx_sp_use_count;

  shared_ptr<Foo> p1(new Foo(1));
  shared_ptr<Foo> p2(p1);

  // shared so it's up to 2
  EXPECT_THAT(p1.use_count(), 2);
  EXPECT_THAT(p2.use_count(), 2);

  // release p1 and down to 1
  p1 = nullptr;

  // *cxx-sp-bool*
  EXPECT_THAT(p1, nullptr);
  EXPECT_FALSE(p1);

  EXPECT_THAT(p1.use_count(), 0);
  EXPECT_THAT(p2.use_count(), 1);

  // do it again but no effect
  p1 = nullptr;
  EXPECT_THAT(p1.use_count(), 0);
  EXPECT_THAT(p2.use_count(), 1);

  // release p2
  p2 = nullptr;

  EXPECT_THAT(p1.use_count(), 0);
  EXPECT_THAT(p2.use_count(), 0);

  // do it again but no effect
  p2 = nullptr;
  EXPECT_THAT(p1.use_count(), 0);
  EXPECT_THAT(p2.use_count(), 0);

  p2 = nullptr;
  EXPECT_THAT(p1.use_count(), 0);
  EXPECT_THAT(p2.use_count(), 0);
}

// started from a quesion; what's going to happen when the shared pointer from
// shared_from_this()? will it delete the pointee?
// So as shown below, the parent on which shared_from_this() is called is also
// shared pointer and if not, "bad_weak_ptr" will be thrown

TEST(CxxSmartPointer, sp_SharedFromThis1)
{
  using namespace cxx_sp_use_count;

  try
  {
    Foo f(1);

    {
      auto sp = f.getFoo();
      sp->value();
      EXPECT_THAT(sp.use_count(), 1);
    }
  } catch (exception &e)
  {
    std::cout << "e.what : " << e.what() << std::endl;
  }
}

TEST(CxxSmartPointer, sp_SharedFromThis2)
{
  using namespace cxx_sp_use_count;

  try
  {
    std::shared_ptr<Foo> sp = std::make_shared<Foo>(1);

    {
      auto f = sp->getFoo();
      f->value();
      EXPECT_THAT(f.use_count(), 2);
    }

    // sp is still available?
    sp->value();
    EXPECT_THAT(sp.use_count(), 1);

  } catch (exception &e)
  {
    std::cout << "e.what : " << e.what() << std::endl;
  }
}

// ={=========================================================================
// cxx-smart-ptr-weak
// 5.2.2 Class weak_ptr

TEST(CxxSmartPointer, weak_GuardedAccess)
{
  // Can explicitly convert a weak_ptr into a shared_ptr by using a
  // corresponding shared_ptr constructor. If there is no valid referenced
  // object, this constructor will throw a `bad_weak_ptr exception`

  {
    auto sp = std::make_shared<int>(42);

    // wp is created out of sp
    std::weak_ptr<int> wp(sp);

    // wp is not involved in reference of sp. since it's weak, creating wp
    // doesn't change the reference count of p
    EXPECT_THAT(sp.use_count(), 1);
    EXPECT_THAT(wp.use_count(), 1);

    // sp is created out of wp. lock() checks whether the object to which the
    // weak_ptr points still exist
    if (auto p = wp.lock())
    {
      EXPECT_THAT(sp.use_count(), 2);
      EXPECT_THAT(wp.use_count(), 2);
    }

    // since p is only around in if()
    EXPECT_THAT(sp.use_count(), 1);
    EXPECT_THAT(wp.use_count(), 1);

    // cxx-error compile error since cxx-sp-weak do not support bool conversion
    // but cxx-sp does.
    //
    // error: could not convert ‘wp’ from ‘std::weak_ptr<int>’ to ‘bool’
    // if (wp)
    //  std::cout << "wp is not null" << std::endl;
  }

  // to show cxx-sp supports bool conversion
  {
    auto sp = std::make_shared<int>(42);

    // wp is created out of sp
    std::weak_ptr<int> wp(sp);

    // wp is not involved in reference of sp
    EXPECT_THAT(sp.use_count(), 1);
    EXPECT_THAT(wp.use_count(), 1);

    // sp is created out of wp and cxx-sp supports bool conversion.
    std::shared_ptr<int> p(wp);
    if (p)
    {
      EXPECT_THAT(sp.use_count(), 2);
      EXPECT_THAT(wp.use_count(), 2);
    }

    EXPECT_THAT(sp.use_count(), 2);
    EXPECT_THAT(sp.use_count(), 2);
  }

  // Can call expired(), which returns true if use_count() is zero, false
  // otherwise. This option is equivalent to checking whether use_count() is
  // equal to 0 but might be 'faster'.

  {
    auto sp = std::make_shared<int>(42);
    std::weak_ptr<int> wp(sp);

    // wp is not involved in reference of sp
    EXPECT_THAT(sp.use_count(), 1);
    EXPECT_THAT(wp.use_count(), 1);

    // release sp
    sp = nullptr;

    EXPECT_FALSE(sp);
    EXPECT_THAT(wp.use_count(), 0);

    // since sp is gone, wp.lock() returns null shared pointer
    // because the last owner of the object released the object in the meantime
    // — lock() yields an empty shared_ptr.

    auto rp = wp.lock();

    // rp is shared_ptr
    EXPECT_FALSE(rp);
    EXPECT_THAT(rp, nullptr);

    // expired() uses use_count() as well
    EXPECT_THAT(wp.expired(), true);
  }

  // so can use use_count() to see if sp's object is still around
  {
    auto sp = std::make_shared<int>(42);

    // wp is created out of sp
    std::weak_ptr<int> wp(sp);

    // wp is not involved in reference of sp. since it's weak, creating wp
    // doesn't change the reference count of p
    EXPECT_THAT(sp.use_count(), 1);
    EXPECT_THAT(wp.use_count(), 1);

    // sp is created out of wp. lock() checks whether the object to which the
    // weak_ptr points still exist
    if (wp.use_count())
    {
      EXPECT_THAT(sp.use_count(), 1);
      EXPECT_THAT(wp.use_count(), 1);
    }
  }

  // You can explicitly convert a weak_ptr into a shared_ptr by using a
  // corresponding shared_ptr constructor. If there is no valid referenced
  // object, this constructor will throw a bad_weak_ptr exception.

  {
    auto sp = std::make_shared<int>(42);
    std::weak_ptr<int> wp(sp);

    // wp is not involved in reference of sp
    EXPECT_THAT(sp.use_count(), 1);

    // release sp
    sp = nullptr;

    // EXPECT_THROW(coll.pop(), ReadEmptyStack);
    try
    {
      std::shared_ptr<int> osp(wp);
    } catch (exception &e)
    {
      std::ostringstream os;
      os << e.what();
      EXPECT_THAT(os.str(), "bad_weak_ptr");
    }

    EXPECT_THAT(wp.expired(), true);
  }

  // how to set wp with sp?
  {
    std::weak_ptr<int> wp;

    EXPECT_THAT(wp.expired(), true);
    EXPECT_THAT(wp.lock(), nullptr);
    EXPECT_THAT(wp.use_count(), 0);

    // *cxx-error*
    // EXPECT_THAT(wp, nullptr);

    // assign wp from make_shared() directly and do not work
    wp = std::make_shared<int>(42);

    EXPECT_THAT(wp.expired(), true);
    EXPECT_THAT(wp.use_count(), 0);

    auto sp = wp.lock();

    EXPECT_THAT(wp.expired(), true);
    EXPECT_THAT(wp.use_count(), 0);

    EXPECT_THAT(sp, nullptr);
  }

  // use copy assign
  {
    std::weak_ptr<int> wp;

    EXPECT_THAT(wp.expired(), true);
    EXPECT_THAT(wp.lock(), nullptr);
    EXPECT_THAT(wp.use_count(), 0);

    auto sp = std::make_shared<int>(42);

    // copy assign
    wp = sp;

    EXPECT_THAT(wp.expired(), false);
    EXPECT_THAT(wp.use_count(), 1);

    auto spwp = wp.lock();

    // because the referenced object is still around

    EXPECT_THAT(wp.expired(), false);
    EXPECT_THAT(wp.use_count(), 2);

    EXPECT_THAT(spwp, Not(nullptr));

    // separation between sp and wp

    EXPECT_THAT(*sp, 42);
    EXPECT_THAT(*spwp, 42);
    EXPECT_THAT(sp.use_count(), 2);

    // reset wp but do not affect sp
    wp.reset();

    EXPECT_THAT(wp.expired(), true);
    EXPECT_THAT(wp.lock(), nullptr);
    EXPECT_THAT(wp.use_count(), 0);

    EXPECT_THAT(*sp, 42);
    EXPECT_THAT(*spwp, 42);
    EXPECT_THAT(sp.use_count(), 2);
  }
}

namespace cxx_sp_weak_problem
{
  class Person
  {
  public:
    string name_;
    shared_ptr<Person> mother_;
    shared_ptr<Person> father_;
    vector<shared_ptr<Person>> kids_;

    Person(string const &name,
           shared_ptr<Person> mother = nullptr,
           shared_ptr<Person> father = nullptr)
        : name_(name)
        , mother_(mother)
        , father_(father)
    {}

    ~Person() { cout << "delete: " << name_ << endl; }
  };

  shared_ptr<Person> init_family(string const &name)
  {
    shared_ptr<Person> mom(new Person(name + "'s mom"));
    shared_ptr<Person> dad(new Person(name + "'s dad"));
    shared_ptr<Person> kid(new Person(name, mom, dad));

    mom->kids_.push_back(kid);
    dad->kids_.push_back(kid);

    return kid;
  }

} // namespace cxx_sp_weak_problem

/*
                                     mom, dad <---
                                               \  \
mom [ 0, 0, kids ]   dad [ 0, 0, kids ]   kid [ m, f, kids ]
               \ (shared or weak)   \
                -> kid               -> kid

[ RUN      ] SharedPointerWeak.CyclicReference
nico's family exists
- nico is shared 3 times
- name of 1st kid of nico's mom: nico
jim's family exists
- jim is shared 3 times
- name of 1st kid of jim's mom: jim
[       OK ] SharedPointerWeak.CyclicReference (1 ms)

if we release our last handle to the family — either by assigning a new person
or nullptr to p or by leaving the scope of p at the end of main() — none of the
Persons gets released, because each still has at least one shared pointer
referring to it. As a result, the destructor of each Person, which would print
“delete name,” never gets called:

Solution?
*/

TEST(SmartPointer, WeakCyclicReference)
{
  using namespace cxx_sp_weak_problem;

  // return kids, which is 'nico'
  shared_ptr<Person> p = init_family("nico");

  cout << "nico's family exists" << endl;
  cout << "- nico is shared " << p.use_count() << " times" << endl;
  cout << "- name of 1st kid of nico's mom: " << p->mother_->kids_[0]->name_
       << endl;

  // cxx-cross-reference
  // assign op decrease count of p but no one from nico family is deleted since
  // each has a reference to them.
  //
  // same for jim family.

  p = init_family("jim");

  cout << "jim's family exists" << endl;
  cout << "- jim is shared " << p.use_count() << " times" << endl;
  cout << "- name of 1st kid of jim's mom: " << p->mother_->kids_[0]->name_
       << endl;
}

namespace cxx_sp_weak_solution
{
  class Person
  {
  public:
    string name_;
    shared_ptr<Person> mother_;
    shared_ptr<Person> father_;

    // vector<shard_ptr<Person>> kids_;
    vector<weak_ptr<Person>> kids_;

    Person(string const &name,
           shared_ptr<Person> mother = nullptr,
           shared_ptr<Person> father = nullptr)
        : name_(name)
        , mother_(mother)
        , father_(father)
    {}

    ~Person() { cout << "delete: " << name_ << endl; }
  };

  shared_ptr<Person> init_family(string const &name)
  {
    shared_ptr<Person> mom(new Person(name + "'s mom"));
    shared_ptr<Person> dad(new Person(name + "'s dad"));
    shared_ptr<Person> kid(new Person(name, mom, dad));

    mom->kids_.push_back(kid);
    dad->kids_.push_back(kid);

    return kid;
  }

} // namespace cxx_sp_weak_solution

// As soon as we lose our handle into a kid created - either by assigning a new
// value to p or by leaving main() - the kid's object of the family loses its
// last owner, which has the effect that both parents lose their last owner. So
// 'all' objects, initially created by new, are deleted now so that their
// destructors get called since weak pointer don't increase count.

// [ RUN      ] SharedPointerWeak.CyclicReferenceSolution
// nico's family exists
// - nico is shared 1 times
// - name of 1st kid of nico's mom: nico
// delete: nico
// delete: nico's dad
// delete: nico's mom
// jim's family exists
// - jim is shared 1 times
// - name of 1st kid of jim's mom: jim
// delete: jim
// delete: jim's dad
// delete: jim's mom
// [       OK ] SharedPointerWeak.CyclicReferenceSolution (2 ms)

TEST(SmartPointer, WeakCyclicReferenceSolution)
{
  using namespace cxx_sp_weak_solution;

  // return kids, which is 'nico'
  shared_ptr<Person> p = init_family("nico");

  cout << "nico's family exists" << endl;
  cout << "- nico is shared " << p.use_count() << " times" << endl;
  cout << "- name of 1st kid of nico's mom: ";

  // cout << p->mother_->kids_[0]->name_ << endl; changes to:
  cout << p->mother_->kids_[0].lock()->name_ << endl;

  p = init_family("jim");

  cout << "jim's family exists" << endl;
  cout << "- jim is shared " << p.use_count() << " times" << endl;
  cout << "- name of 1st kid of jim's mom: ";

  // cout << p->mother_->kids_[0]->name_ << endl; changes to:
  cout << p->mother_->kids_[0].lock()->name_ << endl;
}

namespace cxx_sp_weak_problem
{
  class Resource
  {
  public:
    explicit Resource()
        : name_("resouce")
        , count_(0)
    {
      std::cout << "Resource::Resource" << std::endl;
    }

    ~Resource() { std::cout << "Resource::~Resource" << std::endl; }

    std::string get_name() const { return name_; }

    int get_count() const { return count_; }

    void increase_count() { ++count_; }

    void decrease_count() { --count_; }

  private:
    std::string name_;
    int count_;
  };

  class ResourceManager
  {
  public:
    explicit ResourceManager()
    {
      std::cout << "ResourceManager::ResourceManager" << std::endl;
    }

    std::shared_ptr<Resource> get_resource()
    {
      auto res = res_.lock();

      // if resource is around
      if (res)
      {
        std::cout
          << "ResourceManager::get_resource: res is around and return sp"
          << std::endl;
        return res;
      }
      else
      {
        std::cout << "ResourceManager::get_resource: res is not around and "
                     "return new one"
                  << std::endl;
        auto sp = std::shared_ptr<Resource>{new Resource()};
        res_    = sp;
        return sp;
      }
    }

  private:
    std::weak_ptr<Resource> res_;
  };

} // namespace cxx_sp_weak_problem

/*
# The problem:
#
# The issue is that when runs a production box for long time, HDD didn't go to
# spin-down state when it's supposed to do and do not meet power consumption
# requirement. Interestingly, only happens on production box and for days run.
#
# In Fusion MW, PDM manages HDD resouce and provides APIs to clients. Various
# clients blindly calls API to get or release HDD resouce.
#
# So PDM server has own count to make call to HDD up(when count 0->1) or
down(1->0) # based on number of calls from clients. Also, FDM client maintains
own count to # control PDM API calls to make release/request calls.
#
# The problem is that FDM client which has own count get the count wrong and do
# not make release call to PDM. This leaves PDM count one more and this prevents
# HDD down when other clients tries to do. THis make HDD is on since because PDM
# uses this count to control spin down or on of HDD via calls to device.
#
# How about using smart pointers to this problem?
*/

TEST(SmartPointer, WeakResourceManagerSolution)
{
  using namespace cxx_sp_weak_problem;

  ResourceManager rm;

  auto client1 = rm.get_resource();
  auto client2 = rm.get_resource();

  EXPECT_THAT(client1.use_count(), 2);

  std::cout << "clients use res..." << std::endl;

  client1->increase_count();
  client2->increase_count();
  client1->increase_count();

  EXPECT_THAT(client2->get_count(), 3);

  // finishes use of resource
  client1.reset();
  client2.reset();

  std::cout << "no client and res shall be released..." << std::endl;

  EXPECT_THAT(client1.use_count(), 0);

  // get resource again
  client1 = rm.get_resource();
  client2 = rm.get_resource();

  EXPECT_THAT(client1.use_count(), 2);

  std::cout << "clients use res..." << std::endl;

  client1->increase_count();
  client2->increase_count();
  client1->increase_count();

  EXPECT_THAT(client2->get_count(), 3);

  // finishes use of resource
  client1.reset();

  // but client2 still use resource
  // *cxx-sp-use-count*
  EXPECT_THAT(client1.use_count(), 0);

  EXPECT_THAT(client2.use_count(), 1);
  EXPECT_THAT(client2->get_count(), 3);

  // another client use the same resource
  auto client3 = rm.get_resource();
  client3->increase_count();
  client3->increase_count();
  EXPECT_THAT(client3->get_count(), 5);
}

// ={=========================================================================
// cxx-smart-ptr cxx-sp-own

// From CPP code challenge, 21. System handle wrapper
//
// template <typename>
// class unique_ptr<T>
// {
//  no copy support
//  move support
//  operator bool();
//  T* get();
//  T* release();
//  void reset(T*);
//  void swap(uniqie_ptr<T> &);
// }
//
// + non-member swap
// + self-assign
//
// note that this version uses T which provides pointer and close() in it.
// changes it to delete().

namespace cxx_sp_unique_own_version
{
  struct pointer_int_trait
  {
    typedef int value_type;
    using pointer = value_type *;

    // *cxx-static*
    // without static, cause build error:
    //
    // cxx.cpp:2243:24: error: cannot call member function
    // ‘void
    // cxx_sp_unique_own_version::pointer_int_trait::deleter(cxx_sp_unique_own_version::pointer_int_trait::pointer)’
    // without object
    //
    //           T::deleter(p_);

    static void deleter(pointer p) { delete p; }
  };

  template <typename T>
  class unique_own
  {
    using pointer = typename T::pointer;

  public:
    // no copy support

    // not a build error but don't need to do this as *cxx-scope*
    // unique_own(const unique_own<T> &) = delete;
    // unique_own<T> &operator=(const unique_own<T> &) = delete;

    unique_own(const unique_own &) = delete;
    unique_own &operator=(const unique_own &) = delete;

    // ctor & dtor
    explicit unique_own(typename T::pointer p = nullptr)
        : p_(p)
    {}

    // move support
    // handle moved-from by using release() and moved-to by using reset().

    unique_own(unique_own &&other) noexcept
        : p_(other.release())
    {
      // don't need to do since it's ctor and p_ is not assigned.
      // if (p_)
      //   T::deleter(p_);
      //
      // p_ = other.p_;
    }

    unique_own &operator=(unique_own &&other) noexcept
    {
      // if (p_)
      //   T::deleter(p_);
      //
      // p_ = other.p_;

      if (this != &other)
        reset(other.release());

      return *this;
    }

    ~unique_own() noexcept
    {
      if (p_)
      {
        // this is not `typename`
        T::deleter(p_);
      }
    }

    pointer release() noexcept
    {
      auto p = p_;
      p_     = nullptr;
      return p;
    }

    // free resource and set p
    void reset(pointer p = nullptr) noexcept
    {
      // check self-assign
      if (p_ != p)
      {
        T::deleter(p_);
        p_ = p;
      }
    }

    // swap
    void swap(unique_own &other) const noexcept { std::swap(p_, other.p_); }

    // *cxx-overload-oparator-bool*
    // as with unique_ptr, must have const to avoid const error
    explicit operator bool() const noexcept { return p_ != nullptr; }

    // *cxx-overload-oparator-dereference*
    typename T::value_type &operator*() const noexcept { return *p_; }

  private:
    pointer p_;
  };

  template <typename T>

  // *cxx-error*
  // void swap(unique_own &lhs, unique_own &rhs)
  //
  // okay
  // void swap(unique_own<T> &lhs, unique_own<T> &rhs)
  //
  // okay
  void swap(const unique_own<T> &lhs, const unique_own<T> &rhs)
  {
    lhs.swap(rhs);
  }

  using unique_own_int = unique_own<pointer_int_trait>;

} // namespace cxx_sp_unique_own_version

TEST(SmartPointer, OwnUnique)
{
  using namespace cxx_sp_unique_own_version;

  {
    unique_own_int up{new int(100)};

    EXPECT_TRUE(up);
    EXPECT_THAT(*up, 100);
  }

  {
    unique_ptr<int> p1(new int(1));
    unique_ptr<int> p2(new int(2));
    unique_ptr<int> p3(new int(3));
    unique_ptr<int> p4(new int(4));

    EXPECT_TRUE(p3);

    p2 = std::move(p3); // p1->F1   , p2->F3, p3->null
    p3 = std::move(p1); // p1->null , p2->F3, p3->F1
    p3 = std::move(p1); // p1->null , p2->F3, p3->null

    EXPECT_FALSE(p3);
  }

  {
    unique_ptr<int> p1(new int(1));
    unique_ptr<int> p2(new int(2));
    unique_ptr<int> p3(new int(3));

    p2.reset(p3.release());

    // free int(2) and p3 is null

    EXPECT_TRUE(p1);
    EXPECT_TRUE(p2);
    EXPECT_FALSE(p3);

    p3.reset(p1.release());

    // p1 is null
    EXPECT_FALSE(p1);
    EXPECT_TRUE(p2);
    EXPECT_TRUE(p3);

    // frees int(1) and set p3 null
    p3.reset(p1.release());

    // p1 is null
    EXPECT_FALSE(p1);
    EXPECT_TRUE(p2);
    EXPECT_FALSE(p3);
  }

  {
    unique_own_int up1{new int(100)};
    unique_own_int up2{new int(200)};

    EXPECT_TRUE(up1);
    EXPECT_THAT(*up1, 100);

    EXPECT_TRUE(up2);
    EXPECT_THAT(*up2, 200);

    swap(up1, up2);

    EXPECT_TRUE(up1);
    EXPECT_THAT(*up1, 200);

    EXPECT_TRUE(up2);
    EXPECT_THAT(*up2, 100);
  }
}

// note that shared internal object has two: T and reference count.
//
// template <typename>
// class shared_ptr<T>
// {
//  copy support
//  - no move support
//  operator bool();
//  T* get();
//  - no T* release(); shared_ptr do not support this
//  void reset(T*);
//  void swap(uniqie_ptr<T> &);
//  operator *()
//  operator ->()
//  use_count()
// }
//
// + non-member swap
// + self-assign

namespace cxx_sp_shared_own_version
{
  template <typename T>
  class shared_own
  {
  public:
    // ctor and dtor
    explicit shared_own(T *p = nullptr) noexcept
        : p_(p)
        , pcount_(new size_t(1))
    {}

    ~shared_own() noexcept
    {
      if (--*pcount_ == 0)
      {
        delete p_;
        delete pcount_;
      }
    }

    // copy support
    shared_own(const shared_own &other)
    {
      p_      = other.p_;
      pcount_ = other.pcount_;
      ++*pcount_;
    }

    shared_own &operator=(const shared_own &other)
    {
      // this requires operator==()
      // if (*this != other)

      if (this != &other)
      {
        if (--*pcount_ == 0)
        {
          delete p_;
          delete pcount_;
        }

        p_      = other.p_;
        pcount_ = other.pcount_;
        ++*pcount_;
      }

      return *this;
    }

    // *cxx-overload-operators*
    T &operator*() const noexcept { return *p_; }

    // *cxx-overload-operators*
    T *operator->() const noexcept { return p_; }

    explicit operator bool() const noexcept { return p_ != nullptr; }

    size_t use_count() { return *pcount_; }

    // swap
    void swap(shared_own &other) noexcept
    {
      std::swap(p_, other.p_);
      std::swap(pcount_, other.pcount_);
    }

    // reset
    // use *cxx-tempprary* to delete lhs.
    void reset(T *p = nullptr) noexcept { shared_own<T>(p).swap(*this); }

    //
    T *get() noexcept { return p_; }

  private:
    T *p_; // pointee
    size_t *pcount_;
  };

  template <typename T>
  void swap(shared_own<T> &lhs, shared_own<T> &rhs)
  {
    lhs.swap(rhs);
  }

} // namespace cxx_sp_shared_own_version

TEST(SmartPointer, OwnShared)
{
  using namespace cxx_sp_shared_own_version;

  {
    auto p = shared_own<int>(new int{42});

    // p.use++
    auto q(p);

    // all prints 2
    EXPECT_THAT(p.use_count(), 2);
    EXPECT_THAT(*p, 42);

    EXPECT_THAT(q.use_count(), 2);
    EXPECT_THAT(*q, 42);

    auto r = shared_own<int>(new int{52});

    // q.use++ and r.use--. destroies a object which r pointed.
    r = q;

    // all prints 3
    EXPECT_THAT(p.use_count(), 3);
    EXPECT_THAT(q.use_count(), 3);
    EXPECT_THAT(r.use_count(), 3);
    EXPECT_THAT(*p, 42);
    EXPECT_THAT(*q, 42);
    EXPECT_THAT(*r, 42);
  }

  {
    shared_own<int> up1{new int(100)};
    shared_own<int> up2{new int(200)};

    EXPECT_TRUE(up1);
    EXPECT_THAT(*up1, 100);

    EXPECT_TRUE(up2);
    EXPECT_THAT(*up2, 200);

    swap(up1, up2);

    EXPECT_TRUE(up1);
    EXPECT_THAT(*up1, 200);

    EXPECT_TRUE(up2);
    EXPECT_THAT(*up2, 100);
  }
}

// ={=========================================================================
// cxx-range-for

// TEST(CxxFeaturesTest, DISABLED_UseRangeForOnInteger)
// {
//   int num_loops{10};
//
//   for (const auto &e : num_loops)
//   {
//     cout << "e : " << e << endl;
//   }
// }

// hash(one): 16780311998597636082
// hash(two): 4939359993625789802

// ={=========================================================================
// cxx-hash

TEST(CxxFeaturesTest, UseHashOnString)
{
  cout << "hash(one): " << string("xxx") << endl;

  // error
  // cout << "hash(one): " << hash<string>("one") << endl;

  cout << "hash(one): " << hash<string>()("one") << endl;
  cout << "hash(two): " << hash<string>()("two") << endl;
}

// ={=========================================================================
// cxx-bool cxx-check

// cxx-operator-logical-not
//
// 1.
//
// bool T::operator!() const;
// bool operator!(const T &a);
//
// (ex)
// std::shared_ptr<IBleRcuContoller> proxy =
// std::make_shared<BleRcuContollerProxy>(mIpcClientService, mInputHandler);
// ASSERT_TRUE(!!proxy);
//
// https://stackoverflow.com/questions/206106/is-a-safe-way-to-convert-to-bool-in-c
//
// Q: Is !! a safe way to convert to bool in C++?
//
// typedef long T;       // similar warning with void * or double
// T t = 0;
// bool b = t;           // performance warning: forcing 'long' value to 'bool'
// b = t ? true : false; // ok
// b = !!t;
//
// The argument of the ! operator and the first argument of the ternary operator
// are both implicitly converted to bool, so !! and ?: are IMO silly redundant
// decorations of the cast. I vote for
//
// b = (t != 0);
//
// No implicit conversions.
//
//
// 2.
//
// It's a trick to convert to bool.
//
// For primitive types, yes, it's essentially equivalent to:
//
// !(notABool != 0)
//
// which in turn is equivalent to:
//
// (bool)notABool
//
// For non-primitive types, it will be a compiler error, unless you've
// overloaded operator!, in which case, it might do anything.

TEST(CxxBool, check_conversion)
{
  {
    bool value{false};

    EXPECT_THAT(value, false);
    EXPECT_THAT(!value, true);
    EXPECT_THAT(!!value, false);
  }

  // for non-bool type, `!!` returns bool value of the given variable.
  {
    int value{10};

    EXPECT_THAT(!value, false);
    EXPECT_THAT(!!value, true);

    EXPECT_THAT((bool)value, true);
  }

  {
    std::string state{"NotActive"};

    // positive value if *this appears after the character sequence specified by the
    // arguments, in lexicographical order
    // as seen cxx-string, it's confusing to check the return value
    EXPECT_THAT(state.compare({"Active"}), 1);
    EXPECT_THAT(state.compare({"NotAvailable"}), -19);

    // however, other than 0, regarded as `true` in bool
    EXPECT_THAT(!!state.compare({"Active"}), true);
    EXPECT_THAT(!!state.compare({"NotAvailable"}), true);
  }
}

TEST(CxxBool, check_default_value)
{
  bool value{};
  EXPECT_EQ(value, false);
}

namespace __cxx_check
{
  void CheckFailed(const char *file,
                   int line,
                   const char *cond,
                   unsigned int v1,
                   unsigned int v2)
  {
    printf("Sanitizer CHECK failed: %s:%d %s (%d, %d)\n",
           file,
           line,
           cond,
           v1,
           v2);
  }

  /*
  original code from asan. see '\' at while(0 which has no difference.

  #define CHECK_IMPL(c1, op, c2) \
    do { \
      unsigned int v1 = (unsigned int)(c1); \
      unsigned int v2 = (unsigned int)(c2); \
      if (!(v1 op v2)) \
        CheckFailed(__FILE__, __LINE__, \
          "(" #c1 ") " #op " (" #c2 ")", v1, v2); \
    } while (false) \
  */

#define CHECK_IMPL(c1, op, c2)                                                 \
  do                                                                           \
  {                                                                            \
    unsigned int v1 = (unsigned int)(c1);                                      \
    unsigned int v2 = (unsigned int)(c2);                                      \
    if (!(v1 op v2))                                                           \
      CheckFailed(__FILE__, __LINE__, "(" #c1 ") " #op " (" #c2 ")", v1, v2);  \
  } while (false)

#define CHECK(a) CHECK_IMPL((a), !=, 0)
#define CHECK_EQ(a, b) CHECK_IMPL((a), ==, (b))
#define CHECK_NE(a, b) CHECK_IMPL((a), !=, (b))
#define CHECK_LT(a, b) CHECK_IMPL((a), <, (b))
#define CHECK_LE(a, b) CHECK_IMPL((a), <=, (b))
#define CHECK_GT(a, b) CHECK_IMPL((a), >, (b))
#define CHECK_GE(a, b) CHECK_IMPL((a), >=, (b))

} // namespace __cxx_check

TEST(CxxBool, check_usage)
{
  using namespace __cxx_check;

  // Like assert(), CHECK(x) expect x is true.
  //
  // if("Address is not in memory and not in shadow?")
  // becomes true
  //
  // CHECK("Address is not in memory and not in shadow?");
  // this do not work since cannot cast to const char* to unsigned int
  //
  // if( (0 && "Address is not in memory and not in shadow?"))
  // becomes false
  //
  // Sanitizer CHECK failed: cxx.cpp:1322 ((0 && "Address is not in memory and
  // not in shadow?")) != (0) (0, 0) Sanitizer CHECK failed: cxx.cpp:1325 ((100
  // != 100)) != (0) (0, 0)
  //
  // CHECK("unable to unmap" && 0);

  CHECK(0 && "Address is not in memory and not in shadow?");
  CHECK("Address is not in memory and not in shadow?" && 0);

  CHECK(100 != 101);
  CHECK(100 != 100);
}

/*
// ={=========================================================================

if remove "insert for bool values", then both will use string version.
Looks like it's to do with name resolution.

but there is no bool conversion of string. Then how it uses bool version?

NO. it turns out that's because there is implicit pointer to bool conversion and
that's what's happening here.

[ RUN      ] String.check_string_bool_1
insert(std::string &&key, bool value)
key: key1, value: 1
insert(std::string &&key, std::string &&value)
key: key2, value: value2
[       OK ] String.check_string_bool_1 (0 ms)
[ RUN      ] String.check_string_bool_2
check_bool: 1
[       OK ] String.check_string_bool_2 (0 ms)

this "implicit pointer to bool conversion" explains how this works:


see cxx-bool-conversion-cxx-conversion-to-bool

same from SO:

https://stackoverflow.com/questions/14770252/string-literal-matches-bool-overload-instead-of-stdstring

"Hello World" is a string literal of type "array of 12 const char" which can be
converted to a "pointer to const char" which can in turn be converted to a bool.
That's precisely what is happening. The compiler prefers this to using
std::string's conversion constructor.

A conversion sequence involving a conversion constructor is known as a
user-defined conversion sequence. The conversion from "Hello World" to a bool is
a standard conversion sequence. The standard states that a standard conversion
sequence is always better than a user-defined conversion sequence (§13.3.3.2/2):

a standard conversion sequence (13.3.3.1.1) is a better conversion sequence than
a user-defined conversion sequence or an ellipsis conversion sequence

This "better conversion sequence" analysis is done for each argument of each
viable function (and you only have one argument) and the better function is
chosen by overload resolution.

If you want to make sure the std::string version is called, you need to give it
an std::string:

*/

namespace cxx_bool
{
  // // insert for bool values
  void insert(const std::string &key, bool value)
  {
    std::cout << "insert(const std::string &key, bool value)" << std::endl;
    std::cout << "key: " << key << ", value: " << value << std::endl;
  }

  void insert(std::string &&key, bool value)
  {
    std::cout << "insert(std::string &&key, bool value)" << std::endl;
    std::cout << "key: " << key << ", value: " << value << std::endl;
  }

  // insert for string values
  void insert(const std::string &key, const std::string &value)
  {
    std::cout << "insert(const std::string &key, const std::string &value)"
              << std::endl;
    std::cout << "key: " << key << ", value: " << value << std::endl;
  }

  void insert(std::string &&key, std::string &&value)
  {
    std::cout << "insert(std::string &&key, std::string &&value)" << std::endl;
    std::cout << "key: " << key << ", value: " << value << std::endl;
  }

  void check_bool(bool value)
  {
    std::cout << "check_bool: " << value << std::endl;
  }
} // namespace cxx_bool

// ={=========================================================================
TEST(CxxBool, bool_conversion)
{
  using namespace cxx_bool;

  {
    insert("key1", "value1");
    insert("key2", std::string("value2"));
  }

  {
    check_bool("value1");

    bool expected1 = "value1";
    EXPECT_THAT(expected1, true);

    bool expected2 = "";
    EXPECT_THAT(expected2, true);

    char *ptr{nullptr};
    bool expected3 = ptr;
    EXPECT_THAT(expected3, false);
  }

  {
    std::string coll{};

    // error: could not convert ‘coll’ from
    // ‘std::__cxx11::string {aka std::__cxx11::basic_string<char>}’ to ‘bool’
    //
    // if (coll)
    //   std::cout << "coll" << std::endl;

    EXPECT_THAT(coll.empty(), true);
  }
}

// ={=========================================================================
// cxx-stream cxx-io

TEST(CxxStream, stdio_input)
{
  {
    int i{};
    double d{};
    string s{};

    // show the same result when use cin. To emulate input:
    std::stringstream iss("10\n4.0\nThis is a text\n");

    iss >> i;
    iss >> d;

    // see that it DO NOT read a line
    iss >> s;

    EXPECT_EQ(i, 10);
    EXPECT_EQ(d, 4.0);
    EXPECT_EQ(s, "This");
  }

  // works even when there is no newlines
  {
    int i{};
    double d{};
    string s{};

    // show the same result when use cin. To emulate input:
    std::stringstream iss("10 4.0 This is a text");

    iss >> i;
    iss >> d;
    iss >> s;

    EXPECT_EQ(i, 10);
    EXPECT_EQ(d, 4.0);
    EXPECT_EQ(s, "This");
  }

  {
    int i{};
    double d{};
    string s{};
    int i1, i2, i3, i4;

    std::stringstream iss("10\n4.0\n1 2 3 4\n");

    iss >> i;
    iss >> d;
    iss >> i1;
    iss >> i2;
    iss >> i3;
    iss >> i4;

    vector<int> coll{i1, i2, i3, i4};

    EXPECT_EQ(i, 10);
    EXPECT_EQ(d, 4.0);
    EXPECT_THAT(coll, ElementsAre(1, 2, 3, 4));
  }
}

// ={=========================================================================
// how can get a whole line?

TEST(CxxStream, std_getline)
{
  {
    int i{};
    double d{};
    string s{};

    // why s is null?
    // "If you read token by token, the newline character is not a special
    // character. In this case, the tokens might contain a newline character."

    std::stringstream iss("10\n4.0\nThis is a text\n");

    iss >> i;
    iss >> d;
    std::getline(iss, s);

    EXPECT_EQ(i, 10);
    EXPECT_EQ(d, 4.0);
    EXPECT_EQ(s, "");
  }

  // ok, ignore them
  {
    int i{};
    double d{};
    string s{};

    std::stringstream iss("10\n4.0\nThis is a text\n");

    iss >> i;
    iss >> d;
    iss.ignore(numeric_limits<streamsize>::max(), '\n');
    std::getline(iss, s);

    EXPECT_EQ(i, 10);
    EXPECT_EQ(d, 4.0);
    EXPECT_EQ(s, "This is a text");
  }

  // so use stream-maniplulator ws to remove whitespaces
  {
    int i{};
    double d{};
    string s{};

    std::stringstream iss("10\n4.0\nThis is a text\n");

    iss >> i;
    iss >> d;
    std::getline(iss >> ws, s);

    EXPECT_EQ(i, 10);
    EXPECT_EQ(d, 4.0);
    EXPECT_EQ(s, "This is a text");
  }

  // set `separator`
  // template< class CharT, class Traits, class Allocator >
  // std::basic_istream<CharT,Traits>& getline( std::basic_istream<CharT,Traits>&& input,
  //                                            std::basic_string<CharT,Traits,Allocator>& str,
  //                                            CharT delim );
  {
    std::string s{};
    std::stringstream iss("one|two|three");
    std::vector<std::string> coll{};

    while (std::getline(iss, s, '|'))
      coll.push_back(s);

    EXPECT_THAT(coll, ElementsAre("one", "two", "three"));
  }

  // why loop once? useful?
  {
    char buf[100];
    int bufsize = 100;

    stringstream iss("one|two|three");
    vector<string> coll{};

    // read from iss and stores them to buf.
    while (iss.get(buf, bufsize, '|'))
      coll.push_back(buf);

    EXPECT_THAT(coll, ElementsAre("one"));
  }

  // useful?
  {
    char buf[100];
    int bufsize = 100;

    stringstream iss("one|two|three");
    vector<string> coll{};

    while (iss.getline(buf, bufsize, '|'))
      coll.push_back(buf);

    EXPECT_THAT(coll, ElementsAre("one", "two", "three"));
  }
}

// 15.4.2 Member Functions Accessing the State of Streams
//
// The current state of the flags can be determined by the member functions, as
// presented in Table 15.4.
//
// good()              Returns true if the stream is OK (goodbit is “set”)
// eof()               Returns true if end-of-file was hit (eofbit is set)
// fail()              Returns true if an error has occurred (failbit or badbit is set)
// bad()               Returns true if a fatal error has occurred (badbit is set)
// rdstate()           Returns the currently set flags
// clear()             Clears all flags
// clear(state)        Clears all and sets state flags
// setstate(state)     Sets additional state flags

// ={=========================================================================
TEST(CxxStream, state)
{
  int i1{}, i2{}, i3{}, i4{};

  // 15.3.3 Input/Output of Special Types
  //
  // Numeric Types
  // When reading numeric values, the input must start with at least one digit.
  // Otherwise, the numeric value will be set to 0 and the failbit (see Section
  // 15.4.1, page 758) is set:
  //
  // int x;
  // std::cin >> x; // assigns 0 to x, if the next character does not fit
  //
  // However, if there is no input or if the failbit is set already, calling the
  // input operator will not modify x. This also applies to bool.

  // fail
  stringstream iss6("1.2.3.4\n");

  iss6 >> i1;
  iss6 >> i2;
  iss6 >> i3;
  iss6 >> i4;

  vector<int> coll1{i1, i2, i3, i4};
  EXPECT_THAT(coll1, ElementsAre(1, 0, 0, 0));
  EXPECT_THAT(iss6.fail(), true);

  // fail
  stringstream iss7("1.2.3.4\n");
  i1 = i2 = i3 = i4 = 0;
  int idummy;

  iss7 >> i1;
  iss7 >> idummy;
  iss7 >> i2;
  iss7 >> idummy;
  iss7 >> i3;
  iss7 >> idummy;
  iss7 >> i4;

  vector<int> coll2{i1, i2, i3, i4};
  EXPECT_THAT(coll2, ElementsAre(1, 0, 0, 0));
  EXPECT_THAT(iss7.fail(), true);

  // okay
  stringstream iss8("1.2.3.4\n");
  i1 = i2 = i3 = i4 = 0;
  char cdummy;

  iss8 >> i1;
  iss8 >> cdummy;
  iss8 >> i2;
  iss8 >> cdummy;
  iss8 >> i3;
  iss8 >> cdummy;
  iss8 >> i4;

  vector<int> coll3{i1, i2, i3, i4};
  EXPECT_THAT(coll3, ElementsAre(1, 2, 3, 4));
  EXPECT_THAT(iss8.good(), true);
}

// ={=========================================================================
// [ RUN      ]
// date: 05/17/20
// time: 08:24:17

// [       OK ]

// put_time is one of manipulators
// Table 15.9. Manipulators Provided by the C++ Standard Library
// template< class CharT >
// /*unspecified*/ put_time( const std::tm* tmb, const CharT* fmt );

TEST(CxxStream, manipulator_put_time)
{
  auto now = chrono::system_clock::now();
  time_t t = chrono::system_clock::to_time_t(now);
  tm *tm   = localtime(&t);
  std::cout << put_time(tm, "date: %x\ntime: %X\n") << endl;
}

// 15.6.2 How Manipulators Work
// Manipulators are implemented using a very simple trick that not only enables
// the convenient manipulation of streams but also demonstrates the power
// provided by function overloading.
//
// // ostream
//
//   /**
//    *  @brief  Write a newline and flush the stream.
//    *
//    *  This manipulator is often mistakenly used when a simple newline is
//    *  desired, leading to poor buffering performance.  See
//    *  https://gcc.gnu.org/onlinedocs/libstdc++/manual/streambufs.html#io.streambuf.buffering
//    *  for more on this subject.
//   */
//   template<typename _CharT, typename _Traits>
//     inline basic_ostream<_CharT, _Traits>&
//     endl(basic_ostream<_CharT, _Traits>& __os)
//     { return flush(__os.put(__os.widen('\n'))); }
//
// The member function widen() is used to convert the newline character into the
// character set currently used by the stream. See Section 15.8, page 790, for
// more details.

// The same effect as “writing” the manipulator can also be achieved by calling
// this expression directly.  An advantage to using the function notation is
// that it is not necessary to provide the namespace for the manipulator:
//
// endl(std::cout)
//
// The reason is that, according to ADL (argument-dependent lookup, also known
// as Koenig lookup), functions are looked up in the namespaces where their
// arguments are defined if they are not found otherwise.

// ={=========================================================================
TEST(CxxStream, check_manipulators_work)
{
  std::cout << "check how minipulators work";
  endl(std::cout);
}

// 15.6.3 User-Defined Manipulators

namespace cxx_io
{
  // istream& istream::ignore (streamsize count, int delim)
  // The third form ignores up to count characters until delim is extracted and
  // discarded.
  //
  // so the following function defines a manipulator that ignores all characters
  // until end-of-line:

  template <typename T, typename traits>
  inline std::basic_istream<T, traits> &
  ignoreline(std::basic_istream<T, traits> &strm)
  {
    strm.ignore(std::numeric_limits<std::streamsize>::max(), strm.widen('\n'));
    return strm;
  }

  // TODO:
  // As written, there are multiple ways to define your own manipulator taking
  // arguments.
} // namespace cxx_io

// ={=========================================================================
TEST(CxxStream, check_manipulators_user_defined)
{
  using namespace cxx_io;

  // to simulate user input
  std::istringstream iss{"this is line1\n"
                         "this is line2\n"
                         "this is line3\n"};

  // gets the first
  {
    std::string line{};
    std::getline(iss, line);
    EXPECT_THAT(line, "this is line1");
  }

  // ignore the second line and gets the third. not the second? since iss moved
  // its pos while reading the first
  {
    std::string line{};
    iss >> ignoreline;
    std::getline(iss, line);
    EXPECT_THAT(line, "this is line3");
  }
}

/*
CLR-15.6 Manipulators

The standard manipulators with arguments are defined in the header file
<iomanip>, which must be included to work with the standard manipulators
taking arguments: #include <iomanip>


CLR-15.7.1 Format Flags
Table 15.9. Manipulators Provided by the C++ Standard Library

setw(val)
Sets the field width of the next input and output to val (corresponds to
width())

setfill(c)
Defines c as the fill character (corresponds to fill())

left
Left-adjusts the value

right
Right-adjusts the value

internal
Left-adjusts the sign and right-adjusts the value

Table 15.17. Manipulators for Adjustment

[ RUN      ] CxxStream.manipulators
value: +3301, value: -3301
value: +3301, value: -3301
| start             | end               | perms             | offset            | pathname
             | start               | end             | perms            | offset                              | pathname
             | start               | end             | perms            | offset                              | pathname
*/

// ={=========================================================================
TEST(CxxStream, manipulators_1)
{
  // showpos Forces writing a positive sign on positive numbers
  // noshowpos Forces not writing a positive sign on positive numbers
  // uppercase Forces uppercase letters for numeric values
  // nouppercase Forces lowercase letters for numeric values

  int value = 3301;
  cout.setf(ios::showpos | ios::uppercase);
  cout << "value: " << value << ", value: " << -value << endl;

  // not all flags are supported as manipulator object so have to use
  // setiosflags()

  cout << setiosflags(ios::showpos | ios::uppercase) << "value: " << value
       << ", value: " << -value << endl;

  // address           perms offset  dev   inode       pathname
  // 00400000-00452000 r-xp 00000000 08:02 173521      /usr/bin/dbus-daemon

  // cout.width(8);
  cout.setf(std::ios::left);
  cout << setw(20) << "| start" << setw(20) << "| end" << setw(20) << "| perms"
       << setw(20) << "| offset" << setw(40) << "| pathname" << endl;

  cout.setf(std::ios::right);
  cout << setw(20) << "| start" << setw(20) << "| end" << setw(20) << "| perms"
       << setw(20) << "| offset" << setw(40) << "| pathname" << endl;

  cout << setw(20) << right << "| start" << setw(20) << "| end" << setw(20)
       << "| perms" << setw(20) << "| offset" << setw(40) << "| pathname"
       << endl;
}

TEST(CxxStream, manipulators_2)
{
  std::ostringstream os{};

  int value = 3301;

  {
    os.str("");

    os << std::setw(8) << value;

    EXPECT_THAT(os.str(), "    3301");
  }

  {
    os.str("");

    os << std::setfill('0') << std::setw(8) << value;

    EXPECT_THAT(os.str(), "00003301");
  }
}

// 15.7.6 Floating-Point Notation
// If ios::fixed is set, floatingpoint values are printed using decimal notation.
//
// setprecision      Set decimal precision (function )
//
// *cxx-round*
// In all cases, the remainder is not cut off `but rounded.`

// ={=========================================================================
TEST(CxxStream, check_manipulators_float)
{
  double value = 8.809030;
  std::ostringstream os;

  os << std::fixed << std::setprecision(2) << value;

  EXPECT_THAT(os.str(), "8.81");
}

/*
15.9.1 File Stream Classes

The following program first opens the file charset.out, writes the current
character set — all characters for the values between 32 and 255 — into this
file, and outputs its contents:

If a file stream object is constructed with a string or C-string as an
argument, opening the file for reading and/or writing is attempted
automatically. Whether this attempt was successful is reflected in the
stream’s state. Thus, the state should be examined after construction


15.9.3 File Flags

A set of flags is defined in the class ios_base for precise control over the
processing mode of a file (Table 15.33). These flags are of type openmode, which
is a bitmask type similar to fmtflags.

Flag        Meaning
in          Opens for reading (default for ifstream)
out         Opens for writing (default for ofstream)
app         Always appends at the end when writing
ate         Positions at the end of the file after opening (“at end”)
trunc       Removes the former file contents
binary      Does not replace special characters

Table 15.33. Flags for Opening Files

*/

// ={=========================================================================
TEST(CxxStream, fstream_1)
{
  {
    std::ofstream file("charset_1.out");

    EXPECT_THAT(file.is_open(), true);

    // file opened check
    if (!file)
    {
      std::cerr << "can't open output file \""
                << "charset.out" << std::endl;
      return;
    }

    // write character set
    for (int i = 32; i < 256; ++i)
    {
      file << "value : " << setw(3) << i << "   "
           << "char : " << static_cast<char>(i) << endl;
    }
  } // close file automatically
}

// ={=========================================================================
TEST(CxxStream, fstream_2)
{
  {
    std::ofstream file("charset_2.out");

    // file opened check and use function
    if (file.is_open() == false)
    {
      std::cerr << "can't open output file \""
                << "charset.out" << std::endl;
      return;
    }

    // write character set
    for (int i = 32; i < 256; ++i)
    {
      file << "value : " << setw(3) << i << "   "
           << "char : " << static_cast<char>(i) << endl;
    }
  } // close file automatically
}

// ={=========================================================================
TEST(CxxStream, fstream_3)
{
  // Instead of copying the file contents character by character, you could also
  // output the whole contents in one statement by passing a pointer to the
  // stream buffer of the file as an argument to operator <<:

  {
    std::ifstream file("charset_2.out");
    std::ofstream out("charset_3.out");

    // file opened?
    if (!file)
    {
      std::cerr << "can't open output file \""
                << "charset.out" << std::endl;
      return;
    }

    // OK
    // std::cout << file.rdbuf();

    out << file.rdbuf();

  } // close file automatically
}

// ={=========================================================================
// this corresponds to the UNIX program cat

TEST(CxxStream, fstream_member_operations)
{
  {
    std::ifstream file{};

    file.open("charset_1.out");

    // file opened?
    if (!file)
    {
      std::cerr << "can't open output file \""
                << "charset.out" << std::endl;
      return;
    }

    // std::cout << file.rdbuf();

    // clear eofbit and failbit
    //
    // this is a case when `file` is used for multiple files as in the original
    // example but not in this example.
    //
    // Note that after the processing of a file, clear() must be called to clear
    // the state flags that are set at end-of-file. This is required because the
    // stream object is used for multiple files.
    //
    // Note that open() never clears any state flags. Thus, if a stream was not
    // in a good state after closing and reopening it, you still have to call
    // clear() to get to a good state. This is also the case if you open a
    // different file file.clear();

    file.close();
  } // close file automatically
}

namespace cxx_io
{
  // note that use istream to take ifstream.
  //
  // VOD.L, 1, 100, 184.0, 183.7, VOD.X, 2, 100, 189.0, 183.8, VOD.L, 3, 100, 185.0, 183.9

  std::vector<string> getVector(std::istream &is)
  {
    std::vector<std::string> svec;
    std::string s;

    while (is >> s)
      svec.push_back(s);

    return svec;
  }
} // namespace cxx_io

// ={=========================================================================
// $ cat input.txt
// VOD.L 1 100 184.0 183.7 VOD.X 2 100 189.0 183.8 VOD.L 3 100 185.0 183.9

TEST(CxxStream, fstream_input)
{
  using namespace cxx_io;

  std::ifstream file{"input.txt"};

  std::vector<std::string> svec = getVector(file);

  std::stringstream result{};
  std::string expected{"VOD.L, 1, 100, 184.0, 183.7, VOD.X, 2, 100, 189.0, "
                       "183.8, VOD.L, 3, 100, 185.0, 183.9, "};

  for (const auto &e : svec)
    result << e << ", ";

  EXPECT_THAT(result.str(), expected);
}

// ={=========================================================================
//
// 15.9.3 File Flags
//
// A set of flags is defined in the class ios_base for precise control over the
// processing mode of a file
//
// in    Opens for reading (default for ifstream)
// out   Opens for writing (default for ofstream)
// trunc Removes the former file contents

// Table 15.34 correlates the various combinations of flags with the strings
// used in the interface of C’s function for opening files: fopen().
//
// ios_base Flags    Meaning                                                             C Mode
// in                Reads (file must exist)                                             "r"
// out               Empties and writes (creates if necessary)                           "w"
// out|trunc         Empties and writes (creates if necessary)                           "w"
// out|app           Appends (creates if necessary)                                      "a"
// app               Appends (creates if necessary)                                      "a"
// in|out            Reads and writes; initial position is the start (file must exist)   "r+"
// in|out|trunc      Empties, reads, and writes (creates if necessary)                   "w+"
// in|app            Updates at end (creates if necessary)                               "a+"
// in|out|app        Updates at end (creates if necessary)                               "a+"
//
// Table 15.34. Meaning of Open Modes in C++

TEST(CxxStream, fstream_flags)
{
  {
    std::ofstream file("flags_1.out");

    // file opened?
    if (!file)
    {
      std::cerr << "can't open output file \""
                << "charset.out" << std::endl;
      return;
    }

    // write character set
    for (int i = 32; i < 256; ++i)
    {
      file << "value : " << setw(3) << i << "   "
           << "char : " << static_cast<char>(i) << endl;
    }
  } // close file automatically

  // read check
  {
    std::ifstream file("flags_1.out");

    std::string line{};
    int count{};

    while (std::getline(file, line))
      ++count;

    EXPECT_THAT(count, Ne(0));
  }

  // ok, empty it
  {
    std::ofstream file("flags_1.out");
  }

  // read check
  {
    std::ifstream file("flags_1.out");

    std::string line{};
    int count{};

    while (std::getline(file, line))
      ++count;

    EXPECT_THAT(count, 0);
  }
}

namespace cxx_stream
{
  const std::string expected_2 = R"({
	"documentId" : "1563970127340",
	"services" : 
	[
		{
			"c" : "21",
			"dvbtriplet" : "318.4.8583",
			"servicetypes" : 
			[
				"DTT",
				"OTT"
			],
			"sf" : "sd",
			"sid" : "M13e-4-2187",
			"t" : "Rai 4",
			"xsg" : 8
		},
		{
			"c" : "24",
			"dvbtriplet" : "318.4.8585",
			"servicetypes" : 
			[
				"DTT"
			],
			"sf" : "sd",
			"sid" : "M13e-4-2189",
			"t" : "Rai Movie",
			"xsg" : 8
		}
	],
	"version" : 1
})";
}

// ={=========================================================================

TEST(CxxStream, fstream_write_string)
{
  using namespace cxx_stream;

  {
    // NOTE: if use "fstream", no error and no file gets created.

    std::ofstream ofs{"fstream_write_string.json"};
    ofs << expected_2;
  }
}

// ={=========================================================================
// read a file into a string in single line
TEST(CxxStream, fstream_read_string_1)
{
  using namespace cxx_stream;

  std::ifstream ifs{"fstream_write_string.json"};

  if (ifs.is_open())
  {
    std::string contents{std::istreambuf_iterator<char>{ifs}, {}};
    // std::cout << contents << std::endl;
    EXPECT_THAT(contents, expected_2);
  }
}

// ={=========================================================================
TEST(CxxStream, fstream_read_string_2)
{
  using namespace cxx_stream;

  std::ifstream ifs{"fstream_write_string.json"};
  std::stringstream ss{};

  if (ifs.is_open())
  {
    // since cxx-stringstream has
    // basic_ostream& operator<<( std::basic_streambuf<CharT, Traits>* sb); (9)

    ss << ifs.rdbuf();

    std::string contents{ss.str()};
    // std::cout << contents << std::endl;
    EXPECT_THAT(contents, expected_2);
  }
}

#if 0
TEST(CxxStream, fstream_wildcard)
{
  const std::string files{"../*.cpp"};

  // parse any addition config files
  for (const std::string &file : files)
  {
    std::cout << "file: " << file << std::endl;
  }
}
#endif

// ={=========================================================================
// cxx-stringstream
//
// 806 Chapter 15: Input/Output Using Stream Classes
// A string stream can be created with the flags for the file open modes (see
// Section 15.9.3, page 796) and/or an existing string.

TEST(CxxStream, stringstream_1)
{
  // With the flag ios::ate, the characters written to a string stream can be
  // appended to an existing string:

  {
    std::string s{"value: "};
    std::stringstream os{s, std::ios::out | std::ios::ate};

    EXPECT_THAT(os.str(), "value: ");

    os << "is unknown";

    // appended
    EXPECT_THAT(os.str(), "value: is unknown");

    // The string s itself is not modified.
    EXPECT_THAT(s, "value: ");
  }

  // not appended
  {
    std::string s{"value: "};
    std::stringstream os{s};

    EXPECT_THAT(os.str(), "value: ");

    os << "is unknown";

    EXPECT_THAT(os.str(), "is unknown");

    // The string s itself is not modified.
    EXPECT_THAT(s, "value: ");
  }

  // ??
  // // don't need flags for ostringstream
  // {
  //   std::string s{"value: "};
  //   std::ostringstream os{s};
  //   os << "is unknown";
  //   EXPECT_THAT(os.str(), "is unknown");
  //   // The string s itself is not modified.
  //   EXPECT_THAT(s, "value: ");
  // }

  // so initial value is reset when do "<<"
}

// ={=========================================================================
TEST(CxxStream, stringstream_2)
{
  {
    std::stringstream os{};

    EXPECT_THAT(os.str().size(), 0);

    os << "0123456789";
    EXPECT_THAT(os.str().size(), 10);

    os << "0123456789";
    EXPECT_THAT(os.str().size(), 20);

    os << "0123456789";
    EXPECT_THAT(os.str().size(), 30);

    os << "0123456789";
    EXPECT_THAT(os.str().size(), 40);
  }
}

namespace cxx_io
{
  std::tuple<std::string, std::string, std::string> parseName(std::string name)
  {
    std::string s1{}, s2{}, s3{};

    std::istringstream{name} >> s1 >> s2 >> s3;

    if (s3.empty())
      return std::tuple<std::string, std::string, std::string>(std::move(s1),
                                                               "",
                                                               std::move(s2));
    else
      return std::tuple<std::string, std::string, std::string>(std::move(s1),
                                                               std::move(s2),
                                                               std::move(s3));
  }
} // namespace cxx_io

// ={=========================================================================
TEST(CxxStream, stringstream_move)
{
  using namespace cxx_io;

  auto ret1 = parseName("Nicolai M. Josuttis");

  EXPECT_THAT(std::get<0>(ret1), "Nicolai");
  EXPECT_THAT(std::get<1>(ret1), "M.");
  EXPECT_THAT(std::get<2>(ret1), "Josuttis");
}

namespace cxx_io
{
  void hexMultiplicationTable(std::streambuf *buffer, int num)
  {
    std::ostream hexout{buffer};

    hexout << std::hex << std::showbase;

    for (int i = 1; i <= num; ++i)
    {
      for (int j = 1; j <= 10; ++j)
        hexout << i * j << ' ';

      hexout << std::endl;
    }
  } // does NOT close buffer

  // NOTE: do now work
  void hex_dump(const std::string &str)
  {
    auto num = str.size();

    for (int i = 0; i <= (num / 16); ++i)
    {
      for (int j = 0; j < 16; ++j)
      {
        std::cout << std::hex << std::showbase << str[i * 16 + j] << ' ';
      }

      std::cout << std::endl;
    }
  } // does NOT close buffer
} // namespace cxx_io

// ={=========================================================================
TEST(CxxStream, check_streambuf)
{
  using namespace cxx_io;

  {
    std::cout << "we print 2 lines hex" << std::endl;

    // EXPECT_THAT(oss.length(), 10);
    hexMultiplicationTable(std::cout.rdbuf(), 2);

    std::cout << "That was the output of 2 hex lines " << endl;
  }

  {
    std::cout << "------------------------" << std::endl;
    std::string coll{"this is message"};
    hex_dump(coll);
    std::cout << "------------------------" << std::endl;
  }
}

// ={=========================================================================
TEST(CxxMove, rvalue_and_lvalue)
{
  {
    int x = 10;
    int *pint;

    pint = &(++x);

    // *cxx-error* since & requires lvalue
    // : error: lvalue required as unary ‘&’ operand
    // pint = &(x++);
  }

  {
    int i = 42;

    int &r = i; // okay

    // cxx-error, cannot bind  `lvalue(from)` to `rvalue-reference`
    // : error: cannot bind ‘int’ lvalue to ‘int&&’
    // int &&rr = i;

    // cxx-error, cannot bind `rvalue(from)` to `lvalue-reference`
    // i*42 is rvalue
    // error: invalid initialization of non-const reference of type ‘int&’ from
    // an rvalue of type ‘int’
    // int &r2 = i*42;

    // okay to bind `rvalue(from)` to `const-lvalue-reference`
    const int &r3 = i * 42;

    int &&r4 = i * 42;
    (r4);
  }
}

namespace cxx_move
{
  std::string foo()
  {
    return std::string("foo");
  }
}

/*
// ={=========================================================================

Functions that return a non-reference type, along with the arithmetic,
relational, bitwise, and `cxx-postfix` operations, all yield rvalues.

string f();
string &r2{f()};

error when f() returns rvalue since returns a value and cannot bind rvalue to
lvalue reference.

NO. NO error and works

[ RUN      ] CxxMove.rvalue_and_return
[       OK ] CxxMove.rvalue_and_return (0 ms)

*/
TEST(CxxMove, rvalue_and_return)
{
  using namespace cxx_move;

  std::string coll{foo()};

  EXPECT_THAT(coll, "foo");
}

// ={=========================================================================
// cxx-move

namespace cxx_move
{
  // supports move context but not on members, m_name. so use copy for m_name
  class Move1
  {
  private:
    std::string m_name;
    int m_value;

  public:
    explicit Move1(std::string name = "", int value = 0)
        : m_name(name)
        , m_value(value)
    {}

    // copy controls
    Move1(const Move1 &rhs)
        : m_name(rhs.m_name)
        , m_value(rhs.m_value)
    {
      std::cout << "Move1(const Move &rhs)" << std::endl;
    }

    Move1 &operator=(const Move1 &rhs)
    {
      std::cout << "Move1 &operator=(const Move &rhs)" << std::endl;
      m_name  = rhs.m_name;
      m_value = rhs.m_value;
      return *this;
    }

    // NOTE: std::string m_name uses its copy ctor
    // move controls
    Move1(Move1 &&rhs)
        : m_name(rhs.m_name)
        , m_value(rhs.m_value)
    {
      std::cout << "Move1(Move1 &&rhs)" << std::endl;
    }

    Move1 &operator=(Move1 &&rhs)
    {
      std::cout << "Move1 &operator=(Move &&rhs)" << std::endl;
      m_name  = rhs.m_name;
      m_value = rhs.m_value;
      return *this;
    }

    bool isNameEmpty() const { return m_name.empty(); }

    void setMembers1(std::string &&name, int &&value)
    {
      m_name  = name;
      m_value = value;
      std::cout << "name: " << m_name << ", value: " << m_value << std::endl;
    }

    void setMembers2(std::string &&name, int &&value)
    {
      // NOTE: still need to move on std::string
      m_name  = std::move(name);
      m_value = std::move(value);

      std::cout << "name: " << m_name << ", value: " << m_value << std::endl;
    }

    void printMembers() const
    {
      std::cout << "name: " << m_name << ", value: " << m_value << std::endl;
    }
  };

  // use cxx-overload
  void set_move(const Move1 &m)
  {
    // m.setMembers("set_move", 30);
    std::cout << "set_move1(const &)" << std::endl;
    m.printMembers();
  }

  void set_move(Move1 &&m)
  {
    std::cout << "set_move1(&&)" << std::endl;
    m.printMembers();
  }

  // supports move context for members as well, m_name
  class Move2
  {
  private:
    std::string m_name;
    int m_value;

  public:
    explicit Move2(std::string name = "", int value = 0)
        : m_name(name)
        , m_value(value)
    {}

    // copy controls
    Move2(const Move2 &rhs)
        : m_name(rhs.m_name)
        , m_value(rhs.m_value)
    {
      std::cout << "Move2(const Move2 &rhs)" << std::endl;
    }

    Move2 &operator=(const Move2 &rhs)
    {
      std::cout << "Move2 &operator=(const Move2 &rhs)" << std::endl;
      m_name  = rhs.m_name;
      m_value = rhs.m_value;
      return *this;
    }

    // move controls
    Move2(Move2 &&rhs)
        : m_name(std::move(rhs.m_name))
        , m_value(rhs.m_value)
    {
      std::cout << "Move2(Move2 &&rhs)" << std::endl;
    }

    Move2 &operator=(Move2 &&rhs)
    {
      std::cout << "Move2 &operator=(Move2 &&rhs)" << std::endl;
      m_name  = std::move(rhs.m_name);
      m_value = rhs.m_value;
      return *this;
    }

    bool isNameEmpty() const { return m_name.empty(); }

    void setMembers(std::string &&name, int &&value)
    {
      m_name  = name;
      m_value = value;
      std::cout << "name: " << m_name << ", value: " << m_value << std::endl;
    }

    void printMembers() const
    {
      std::cout << "name: " << m_name << ", value: " << m_value << std::endl;
    }
  };
} // namespace cxx_move

// ={=========================================================================
// [ RUN      ] CxxMove.move_signal1
// Move1(const Move &rhs)
// Move1(const Move &rhs)
// Move1(const Move &rhs)
// [       OK ] CxxMove.move_signal1 (1 ms)
// [ RUN      ] CxxMove.move_signal11
// Move1 &operator=(const Move &rhs)
// Move1 &operator=(const Move &rhs)
// [       OK ] CxxMove.move_signal11 (0 ms)

// uses copy ctor
TEST(CxxMove, copy_context_1)
{
  using namespace cxx_move;

  std::vector<Move1> coll;

  Move1 m1{"m1", 10};
  Move1 m2{"m2", 20};

  coll.push_back(m1);
  coll.push_back(m2);

  // string member is not empty since it's copied
  EXPECT_THAT(m1.isNameEmpty(), false);
  EXPECT_THAT(m2.isNameEmpty(), false);
}

// ={=========================================================================
// uses copy assign
TEST(CxxMove, copy_context_2)
{
  using namespace cxx_move;

  std::vector<Move1> coll;

  Move1 m1{"m1", 10};
  Move1 m2{"m2", 20};

  Move1 m3;
  Move1 m4;

  m3 = m1;
  m4 = m2;

  // string member is not empty since it's copied
  EXPECT_THAT(m1.isNameEmpty(), false);
  EXPECT_THAT(m2.isNameEmpty(), false);
}

// ={=========================================================================
// [ RUN      ] CxxMove.move_signal2
// Move1(Move1 &&rhs)
// Move1(Move1 &&rhs)
// Move1(const Move &rhs)
// [       OK ] CxxMove.move_signal2 (0 ms)
// [ RUN      ] CxxMove.move_signal22
// Move1 &operator=(Move &&rhs)
// Move1 &operator=(Move &&rhs)
// [       OK ] CxxMove.move_signal22 (0 ms)

// std::move() should signal move version? Yes for Move1 but not string member
// since it's uses copy so wll not be empty
TEST(CxxMove, copy_context_3)
{
  using namespace cxx_move;

  std::vector<Move1> coll;

  Move1 m1{"m1", 10};
  Move1 m2{"m2", 20};

  coll.push_back(std::move(m1));
  coll.push_back(std::move(m2));

  // string member is not empty since it's copied
  EXPECT_THAT(m1.isNameEmpty(), false);
  EXPECT_THAT(m2.isNameEmpty(), false);
}

// ={=========================================================================
TEST(CxxMove, copy_context_4)
{
  using namespace cxx_move;

  std::vector<Move1> coll;

  Move1 m1{"m1", 10};
  Move1 m2{"m2", 20};

  Move1 m3;
  Move1 m4;

  m3 = std::move(m1);
  m4 = std::move(m2);

  // string member is not empty since it's copied
  EXPECT_THAT(m1.isNameEmpty(), false);
  EXPECT_THAT(m2.isNameEmpty(), false);
}

// ={=========================================================================
// Ok, uses Move2 which has real move controls but here uses copy
TEST(CxxMove, copy_context_5)
{
  using namespace cxx_move;

  std::vector<Move2> coll;

  Move2 m1{"m1", 10};
  Move2 m2{"m2", 20};

  coll.push_back(m1);
  coll.push_back(m2);

  // string member is not empty since it's copied
  EXPECT_THAT(m1.isNameEmpty(), false);
  EXPECT_THAT(m2.isNameEmpty(), false);
}

// ={=========================================================================
// MOVE!
TEST(CxxMove, move_context_1)
{
  using namespace cxx_move;

  std::vector<Move2> coll;

  Move2 m1{"m1", 10};
  Move2 m2{"m2", 20};

  coll.push_back(std::move(m1));
  coll.push_back(std::move(m2));

  // string member is *moved* so it's empty
  EXPECT_THAT(m1.isNameEmpty(), true);
  EXPECT_THAT(m2.isNameEmpty(), true);
}

// ={=========================================================================
// `std::move() doesn't itself do any moving`, but merely signal function
// matching or resolution and trigger cxx-overload
TEST(CxxMove, move_signal_1)
{
  using namespace cxx_move;

  {
    Move1 m;

    const std::string cname("cmove");

    std::string name("move");
    int value{10};

    // lvalue(from) to rvalue error
    // cxx-error: cannot bind
    // ‘std::__cxx11::string {aka std::__cxx11::basic_string<char>}’ lvalue to
    // ‘std::__cxx11::string&& {aka std::__cxx11::basic_string<char>&&}’
    //
    // m.setMembers(name, value);

    // lvalue to rvalue error
    // m.setMembers(cname, cvalue);

    m.setMembers1("move", 10);

    m.setMembers1(std::move(name), std::move(value));
    EXPECT_THAT(name.empty(), false);

    // `setMembers2` is version that do move in it so name should be empty.
    m.setMembers2(std::move(name), std::move(value));
    EXPECT_THAT(name.empty(), true);
  }
}

/*
// ={=========================================================================
show cases where still need to use "std::move()" to signal move context

1. example 1   

{
  template<typename _Container>
    class insert_iterator
    : public iterator<output_iterator_tag, void, void, void, void>
    {
      insert_iterator&
        operator=(typename _Container::value_type&& __value)
        {
          iter = container->insert(iter, `std::move(__value)`);
          ++iter;
          return *this;
        }
    }
}

2. example 2

void setMembers2(std::string &&name, int &&value)
{
  // NOTE: still need to move on std::string
  m_name  = std::move(name);
  m_value = std::move(value);
}

3.

EventLoop::EventLoop(EventLoop &&other)
    : m_private(std::move(other.m_private))
{}

*/

TEST(CxxMove, move_signal_2)
{
  using namespace cxx_move;

  // which will be called? no copy or move controls are called
  //
  // *cxx-reference-binding*
  // Can bind `rvalue` to `const-lvalue-reference` assume that the moved is
  // const and that means nothing will be changed so why bother to copy of it?
  // so no copy or move.

  {
    Move1 m1{"m1", 10};
    Move1 m2{"m2", 20};

    // set_move1(&&)
    set_move(std::move(m1));

    // set_move1(const &)
    set_move(m2);

    // set_move1(&&)
    set_move(std::move(m1));
  }
}

// ={=========================================================================
// cxx-slice-off

namespace cxx_slice_off1
{
  class Foo
  {
  protected:
    std::string m_name{};

  public:
    Foo(const std::string &name = "Foo")
        : m_name(name)
    {}
    virtual ~Foo() {}
    virtual std::string name() { return m_name; }
  };

  class FooFoo : public Foo
  {
  private:
    std::string m_middle_name{};

  public:
    FooFoo(const std::string &name, const std::string &middle)
        : Foo(name)
        , m_middle_name(middle)
    {}

    ~FooFoo() {}

    std::string name() override { return m_name + ":" + m_middle_name; }
  };
} // namespace cxx_slice_off1

TEST(CxxSlice, see_slice_1)
{
  using namespace cxx_slice_off1;

  FooFoo foo1("Foo1", "Hoo1");
  EXPECT_THAT(foo1.name(), "Foo1:Hoo1");

  FooFoo foo2("Foo2", "Hoo2");
  EXPECT_THAT(foo2.name(), "Foo2:Hoo2");

  // copied since default copy assign used
  foo1 = foo2;
  EXPECT_THAT(foo1.name(), "Foo2:Hoo2");

  Foo foo3("Bar1");
  EXPECT_THAT(foo3.name(), "Bar1");

  // copy assign
  foo3 = foo2;

  // slice-off
  EXPECT_THAT(foo3.name(), "Foo2");
}

namespace cxx_slice_off2
{
  class Foo
  {
  protected:
    std::string m_name{};

  public:
    Foo(const std::string &name = "Foo")
        : m_name(name)
    {}
    virtual ~Foo() {}

    Foo(const Foo &) = delete;
    Foo &operator=(const Foo &) = delete;

    virtual std::string name() { return m_name; }
  };

  class FooFoo : public Foo
  {
  private:
    std::string m_middle_name{};

  public:
    FooFoo(const std::string &name, const std::string &middle)
        : Foo(name)
        , m_middle_name(middle)
    {}

    ~FooFoo() {}

    std::string name() override { return m_name + ":" + m_middle_name; }
  };
} // namespace cxx_slice_off2

// since compile error
// NOTE: yes, raise compile error for slice off cases but also not able to copy
// as well.

#if 0
TEST(CxxSlice, see_slice_2)
{
  using namespace cxx_slice_off2;

  FooFoo foo1("Foo1", "Hoo1");
  EXPECT_THAT(foo1.name(), "Foo1:Hoo1");

  FooFoo foo2("Foo2", "Hoo2");
  EXPECT_THAT(foo2.name(), "Foo2:Hoo2");

  // copied since default copy assign used
  // now compile error
  foo1 = foo2;
  EXPECT_THAT(foo1.name(), "Foo2:Hoo2");

  Foo foo3("Bar1");
  EXPECT_THAT(foo3.name(), "Bar1");

  // copy assign
  // now compile error
  foo3 = foo2;

  // slice-off
  EXPECT_THAT(foo3.name(), "Foo2");
}
#endif

// ={=========================================================================
// cxx-override

namespace cxxoverride
{
  namespace nooverride
  {
    class Base
    {
    public:
      Base()
          : base_(10)
      {}

      virtual int get_value() { return base_; }

    private:
      int base_;
    };

    class Derived : public Base
    {
    public:
      Derived()
          : derived_(20)
      {}

      virtual int get_value(int value)
      {
        (void)value;
        return derived_;
      };

    private:
      int derived_;
    };
  } // namespace nooverride

  namespace yesoverride
  {
    class Base
    {
    public:
      Base()
          : base_(10)
      {}

      virtual int get_value() { return base_; }

    private:
      int base_;
    };

    class Derived : public Base
    {
    public:
      Derived()
          : derived_(20)
      {}

      virtual int get_value() { return derived_; };

    private:
      int derived_;
    };
  } // namespace yesoverride
} // namespace cxxoverride

TEST(CxxOverride, check_condition)
{
  using namespace cxxoverride;

  // override happens
  {
    yesoverride::Derived derived;
    yesoverride::Base *pbase = &derived;
    EXPECT_THAT(pbase->get_value(), 20);
  }

  // No override since arg is different and do not meet *cxx-override-condition*
  // Hence no vtable update and base version called.
  {
    nooverride::Derived derived;
    nooverride::Base *pbase = &derived;
    EXPECT_THAT(pbase->get_value(), 10);
  }

  // : error: no matching function for call to
  // ‘cxx_override::no_override::Derived::get_value()’
  //      EXPECT_THAT(pbase->get_value(), 20);
  //
  // no override happens and effectively defines a new function in the derived,
  // inner scope, name found and stops lookup. so hide the name in the base in
  // *cxx-name-lookup*
  //
  // {
  //   Derived derived;
  //
  //   // see Derived
  //   Derived* pbase = &derived;
  //
  //   EXPECT_THAT(pbase->get_value(), 20);
  // }
}

namespace cxx_override
{
  namespace no_virtual
  {
    class Base
    {
    public:
      Base()
          : base_(10)
      {}

      int get_value() { return base_; }
      std::string getName() { return "Base"; }

    private:
      int base_;
    };

    class Derived : public Base
    {
    public:
      Derived()
          : derived_(20)
      {}

      int get_value() { return derived_; };

    private:
      int derived_;
    };
  } // namespace no_virtual
} // namespace cxx_override

// cxx-override-keyword
// if use "override" keyword, gets compile error than wrong result. that is main
// reason to use override keyword

TEST(CxxOverride, check_condition_fail_when_no_virtual)
{
  using namespace cxx_override::no_virtual;

  // No override happens since there is no virtual used. so call base version.
  {
    Derived derived;
    Base *pbase = &derived;
    EXPECT_THAT(pbase->get_value(), 10);
  }

  // call derived version
  {
    Derived derived;

    // see Derived
    Derived *pderived = &derived;

    EXPECT_THAT(pderived->get_value(), 20);
  }

  // call member which is only in base
  {
    Derived derived;

    // see Derived
    Derived *pderived = &derived;

    EXPECT_THAT(pderived->getName(), "Base");
  }
}

namespace cxx_override
{
  namespace with_virtual
  {
    class Base
    {
    public:
      Base()
          : base_(10)
      {}

      virtual int get_value() { return base_; }

    private:
      int base_;
    };

    class Derived : public Base
    {
    public:
      Derived()
          : derived_(20)
      {}

      int get_value() { return derived_; };

    private:
      int derived_;
    };
  } // namespace with_virtual
} // namespace cxx_override

TEST(CxxOverride, check_condition_okay)
{
  using namespace cxx_override::with_virtual;

  {
    Derived derived;
    Base *pbase = &derived;

    // now override works
    EXPECT_THAT(pbase->get_value(), 20);
  }

  {
    Derived derived;

    // see Derived
    Derived *pderived = &derived;

    EXPECT_THAT(pderived->get_value(), 20);
  }
}

namespace cxx_override
{
  namespace with_virtual_and_private
  {
    class Base
    {
    public:
      Base()
          : base_(10)
      {}

      virtual int get_value() { return base_; }

    private:
      int base_;
    };

    class Derived : private Base
    {
    public:
      Derived()
          : derived_(20)
      {}

      int get_value() { return derived_; };

    private:
      int derived_;
    };
  } // namespace with_virtual_and_private
} // namespace cxx_override

// TEST(Override, Condition_4)
// {
//   using namespace cxx_override::with_virtual_and_private;
//
//   {
//     Derived derived;
//
//     // *cxx-error*
//     // cxx.cpp:4864:20: error: ‘cxx_override::with_virtual_and_private::Base’
//     is an inaccessible base of
//     ‘cxx_override::with_virtual_and_private::Derived’
//     //
//     //      Base* pbase = &derived;
//     //                     ^
//     // means that *cxx-override* must use public inheritance
//
//     Base* pbase = &derived;
//
//     // now override works
//     EXPECT_THAT(pbase->get_value(), 20);
//   }
//
//   {
//     Derived derived;
//
//     // see Derived
//     Derived* pderived = &derived;
//
//     EXPECT_THAT(pderived->get_value(), 20);
//   }
// }

// cxx-override when not implemented pure virtual member function

namespace cxx_override
{
  namespace no_pure
  {
    class Base
    {
    public:
      Base()
          : base_(10)
      {}

      virtual int get_value() = 0;

    private:
      int base_;
    };

    class Derived : public Base
    {
    public:
      Derived()
          : derived_(20)
      {}

      virtual int get_value() { return derived_; };

    private:
      int derived_;
    };

    class DerivedNoPure : public Base
    {
    public:
      DerivedNoPure()
          : derived_(20)
      {}

      // without this, compile fails and with this, will see link error in the
      // end.
      // gcc (Debian 4.9.2-10) 4.9.2
      // : undefined reference to `vtable for
      // cxx_override::no_pure::DerivedNoPure' collect2: error: ld returned 1
      // exit status

      virtual int get_value();

    private:
      int derived_;
    };
  } // namespace no_pure
} // namespace cxx_override

TEST(CxxOverride, check_condition_fail_when_pure_not_implemented)
{
  using namespace cxx_override::no_pure;

  {
    Derived derived;
    Base *pbase = &derived;
    EXPECT_THAT(pbase->get_value(), 20);
  }

  {
    Derived derived;

    // see Derived
    Derived *pderived = &derived;

    EXPECT_THAT(pderived->get_value(), 20);
  }

  // {
  //   DerivedNoPure derived;

  //   // see Derived
  //   DerivedNoPure* pderived = &derived;

  //   EXPECT_THAT(pderived->get_value(), 20);
  // }
}

// ={=========================================================================
// cxx-rtti cxx-cast

namespace cxx_rtti
{

  class RttiBase
  {
  public:
    // to make it polymorphic
    // ~RttiBase() {}
    virtual ~RttiBase() {}

  private:
    int id_;
  };

  class RttiDerived : public RttiBase
  {
  private:
    int value_;
  };

} // namespace cxx_rtti

TEST(Rtti, UseTypeid)
{
  using namespace cxx_rtti;

  RttiBase b, bb;
  RttiDerived d;
  bool result{};

  result = (typeid(b) == typeid(d));
  EXPECT_EQ(result, false);

  result = (typeid(b) == typeid(bb));
  EXPECT_EQ(result, true);

  result = (typeid(d) == typeid(RttiDerived));
  EXPECT_EQ(result, true);
}

TEST(Rtti, DynamicCast)
{
  using namespace cxx_rtti;

  {
    RttiDerived d;

    auto result = dynamic_cast<RttiBase *>(&d);
    EXPECT_TRUE(result != NULL);

    RttiBase *bp = dynamic_cast<RttiBase *>(&d);

    // "derived is a subclass of base"
    EXPECT_THAT(bp, Ne(nullptr));
  }

  {
    try
    {
      RttiDerived dp1;

      // upcast
      RttiBase *pbase1 = new RttiDerived;

      RttiBase *pbase2 = new RttiBase;

      RttiBase *pbase3;
      RttiDerived *pderived;

      // derived to derived, okay.
      //
      // *cxx-dtor-non-virtual-destruction-problem* when no virtual keyword in
      // class definition
      //
      // cxx.cpp:4138:51: error: cannot dynamic_cast ‘pbase1’ (of type ‘class
      // cxx_rtti::RttiBase*’) to type ‘class cxx_rtti::RttiDerived*’ (source
      // type is not polymorphic)
      //        pderived = dynamic_cast<RttiDerived*>(pbase1);

      pderived = dynamic_cast<RttiDerived *>(pbase1);
      EXPECT_THAT(pderived, Ne(nullptr));

      // downcast from base to derived, fails since pbase2 is not complete
      // objedct
      pderived = dynamic_cast<RttiDerived *>(pbase2);
      EXPECT_THAT(pderived, nullptr);

      // upcast from derived to base

      pbase3 = dynamic_cast<RttiBase *>(pbase1);
      EXPECT_THAT(pbase3, Ne(nullptr));

      pbase3 = dynamic_cast<RttiBase *>(&dp1);
      EXPECT_THAT(pbase3, Ne(nullptr));

    } catch (exception &e)
    {
      cout << "Exception: " << e.what();
    }
  }
}

/*
// ={=========================================================================
cxx-regex

14.1 The Regex Match and Search Interface

(2) (since C++11)
explicit basic_regex( const CharT* s,
  flag_type f = std::regex_constants::ECMAScript );

*/

TEST(CxxRegex, match_1)
{
  // find XML/HTML tagged value (using fefault syntax)
  // As with strings, this is a specialization of class std::basic_regex<> for
  // the character type char.
  {
    std::regex reg("<.*>.*</.*>");
    bool found = std::regex_match("<tag>value</tag>", reg);

    EXPECT_EQ(found, true);
  }

  // find XML/HTML tagged value (tags before and after the value must match)
  //
  // Here, we use the concept of “grouping.” We use “(...)” to define a
  // so-called capture group, to which we refer later on with the regular
  // expression “\1”. Note, however, that we specify the regular expression as
  // an ordinary character sequence, so we have to specify the “character \
  // followed by the character 1” as “\\1”.
  //
  // Alternatively, we could use a raw string, which was introduced with C++11
  // It starts with “R"(” and ends with “)"”.
  //
  // R"(<(.*)>.*</\1>)" is equivalent to: "<(.*)>.*</\\1>"

  {
    std::regex reg("<(.*)>.*</\\1>");
    bool found = std::regex_match("<tag>value</tag>", reg);

    EXPECT_EQ(found, true);
  }
  {
    std::regex reg(R"(<(.*)>.*</\1>)");
    auto found = std::regex_match("<tag>value</tag>", reg);

    EXPECT_EQ(found, true);
  }

  // find XML/HTML tagged value (using grep syntax)
  //
  // What we introduce here as special characters for regular expressions is
  // part of the grammar they have. Note that the C++ standard library supports
  // various grammars. The default grammar is a “modified ECMAScript grammar,”
  // which is introduced in detail in Section 14.8, page 738. But the next
  // statements show how a different grammar can be used:
  //
  // C++PL 37.1.1
  // The regex library can recognize several variants of the notation for
  // regular expressions (§37.2).  Here, I first present the default notation
  // used, a variant of the ECMA standard used for ECMAScript (more commonly
  // known as JavaScript).
  //
  // that is C++ regex is BRE by default

  {
    regex reg("<\\(.*\\)>.*</\\1>", regex_constants::grep);
    auto found = std::regex_match("<tag>value</tag>", reg);

    EXPECT_EQ(found, true);
  }

  // use C string as reg expression (needs explicit cast to regex)
  {
    auto found =
      std::regex_match("<tag>value</tag>", std::regex("<.*>.*</.*>"));

    EXPECT_EQ(found, true);
  }
}

/*
// ={=========================================================================
the difference of regex_match() and regex_search()

regex_match() checks whether the *whole* character sequence matches a
regular expression.

regex_search() checks whether the character sequence *partially* matches a
regular expression.

*/
TEST(CxxRegex, match_and_search)
{
  // fails to match
  {
    auto found = std::regex_match("XML tag: <tag>value</tag>",
                                  std::regex("<(.*)>.*</\\1>"));

    EXPECT_EQ(found, false);
  }

  // but search is fine
  {
    auto found = std::regex_search("XML tag: <tag>value</tag>",
                                   std::regex("<(.*)>.*</\\1>"));

    EXPECT_EQ(found, true);
  }

  // when extend 'expression'
  {
    auto found = std::regex_match("XML tag: <tag>value</tag>",
                                  std::regex(".*<(.*)>.*</\\1>.*"));

    EXPECT_EQ(found, true);
  }

  {
    auto found = std::regex_search("XML tag: <tag>value</tag>",
                                   std::regex(".*<(.*)>.*</\\1>.*"));

    EXPECT_EQ(found, true);
  }
}

// ={=========================================================================
TEST(CxxRegex, match_2)
{
  string data{"XML tag: <tag-name>the value</tag-name>."};

  {
    std::regex rx(R"(<([\w-]+)>(.*)<(\/[\w-]+)>)");

    // for returned details of the match
    std::smatch m;

    auto found = regex_search(data, m, rx);
    EXPECT_TRUE(found);
  }

  {
    // ok
    regex rx("<(.*)>(.*)</(\\1)>");

    // for returned details of the match
    std::smatch m;

    auto found = regex_search(data, m, rx);
    EXPECT_TRUE(found);
  }
}

/*
// ={=========================================================================

C++PL 37.1

That last pattern is useful for parsing XML. It finds tag/end-of-tag markers.
Note that I used a non-greedy match (a lazy match), .∗? , for the subpattern
between the tag and the end tag. Had I used plain .∗ , this input would have
caused a problem:

Always look for the <b>bright</b> side of <b>life</b>.

A greedy match for the first subpattern would match the first < with the last >
. A greedy match on the second subpattern would match the first <b> with the
last </b> . Both would be correct behavior, but unlikely what the programmer
wanted.

NOTE: NOT work as described??

*/

TEST(CxxRegex, match_and_greedy)
{
  {
    std::string data{"<tag>value</tag>"};

    // has only one sub_match
    std::regex reg(R"(<(.*)>.*</\1>)");

    std::smatch m;

    auto found = std::regex_match(data, m, reg);

    EXPECT_EQ(found, true);

    EXPECT_THAT(m.size(), 2);

    EXPECT_THAT(m[0].str(), "<tag>value</tag>");
    EXPECT_THAT(m[1].str(), "tag");
  }

  {
    std::string data{"Always look for the <b>bright</b> side of <b>life</b>"};

    std::regex reg(R"(<(.*)>.*</\1>)");

    std::smatch m;

    auto found = std::regex_match(data, m, reg);

    EXPECT_EQ(found, false);

    EXPECT_THAT(m.size(), 0);
  }

  {
    std::string data{"Always look for the <b>bright</b> side of <b>life</b>"};

    std::regex reg(R"(<(.*)>(.*)</\1>)");

    std::smatch m;

    auto found = std::regex_match(data, m, reg);

    EXPECT_EQ(found, false);

    EXPECT_THAT(m.size(), 0);
  }

  {
    std::string data{"Always look for the <b>bright</b> side of <b>life</b>"};

    std::regex reg(R"(.*<(.*)>(.*)</\1>)");

    std::smatch m;

    auto found = std::regex_match(data, m, reg);

    EXPECT_EQ(found, true);

    EXPECT_THAT(m.size(), 3);

    EXPECT_THAT(m[0].str(),
                "Always look for the <b>bright</b> side of <b>life</b>");
    EXPECT_THAT(m[1].str(), "b");
    EXPECT_THAT(m[2].str(), "life");
  }

  {
    std::string data{"Always look for the <b>bright</b> side of <b>life</b>"};

    std::regex reg(R"(.*<(.*?)>(.*?)</\1>)");

    std::smatch m;

    auto found = std::regex_match(data, m, reg);

    EXPECT_EQ(found, true);

    EXPECT_THAT(m.size(), 3);

    EXPECT_THAT(m[0].str(),
                "Always look for the <b>bright</b> side of <b>life</b>");
    EXPECT_THAT(m[1].str(), "b");
    EXPECT_THAT(m[2].str(), "life");
  }
}

/*
// ={=========================================================================
CXXSLR-14.2 Dealing with Subexpressions

In this example, we can demonstrate the use of match_results objects, which can
be passed to regex_match() and regex_search() to get details of matches. Class
std::match_results<> is a template that has to get instantiated by the iterator
type of the characters processed. The C++ standard library provides some
predefined instantiations:

smatch: for details of matches in strings
cmatch: for details of matches in C-strings (const char*)
...

Thus, if we call regex_match() or regex_search() for C++ strings, type smatch
has to be used; for ordinary string literals, type cmatch has to be used.

Regex: <(.*)>(.*)</(\1)>
XML tag: <tag-name>the value</tag-name>.
        | | m[1] | |  m[2] |  | m[3] | |
| prefix|             m[0]            | suffix

In general, the match_results object contains:

o A sub_match object m[0] for all the matched characters

o A prefix(), a sub_match object that represents all characters before
 the first matched character

o A suffix(), a sub_match object that represents all characters after
 the last matched character

*/

TEST(CxxRegex, match_result)
{
  std::string data{"XML tag: <tag-name>the value</tag-name>."};

  // ok
  std::regex rx(R"(<([\w-]+)>(.*)<\/([\w-]+)>)");

  // for returned details of the match
  std::smatch m;

  auto found = std::regex_search(data, m, rx);

  EXPECT_TRUE(found);

  if (found)
  {
    EXPECT_THAT(m.empty(), false);

    // size() yields the number of "sub_match" objects (including m[0]).

    EXPECT_THAT(m.size(), 4);

    // member function str() to yield the matched string as a whole (calling
    // str() or str(0)) or the nth matched substring (calling str(n)), which is
    // empty if no matched substring exists (thus, passing an n greater than
    // size() is valid)
    // that is m[0]

    EXPECT_THAT(m.str(), "<tag-name>the value</tag-name>");
    EXPECT_THAT(m[0].str(), "<tag-name>the value</tag-name>");

    // member function length() to yield the length of the matched string as a
    // whole (calling length() or length(0)) or the length of the nth matched
    // substring (calling length(n)), which is 0 if no matched substring exists
    // (thus, passing an n greater than size() is valid)

    EXPECT_THAT(m.length(), 30);

    // member function position() to yield the position of the matched string as
    // a whole (calling position() or position(0)) or the position of the nth
    // matched substring (calling length(n))

    EXPECT_THAT(m.position(), 9);
    EXPECT_THAT(m.position(0), 9);
    EXPECT_THAT(m.prefix().str(), "XML tag: ");
    EXPECT_THAT(m.suffix().str(), ".");

    EXPECT_THAT(m.position(1), 10);
    EXPECT_THAT(m[1].str(), "tag-name");
    EXPECT_THAT(m.str(1), "tag-name");
    EXPECT_THAT(m[1].matched, true);

    EXPECT_THAT(m.position(2), 19);
    EXPECT_THAT(m[2].str(), "the value");
    EXPECT_THAT(m.str(2), "the value");
    EXPECT_THAT(m[2].matched, true);

    EXPECT_THAT(m.position(3), 30);
    EXPECT_THAT(m[3].str(), "tag-name");
    EXPECT_THAT(m.str(3), "tag-name");
    EXPECT_THAT(m[3].matched, true);

    // use iterator and operator<<() writes the characters to a stream
    std::ostringstream os;

    for (auto pos = m.begin(); pos != m.end(); ++pos)
    {
      os << *pos << ", " << pos->length() << endl;
    }

    // works
    // char expected[] = "<tag-name>the value</tag-name>, 30\n"
    //   "tag-name, 8\n"
    //   "the value, 9\n"
    //   "tag-name, 8\n";
    // EXPECT_THAT(os.str(), expected);

    // works
    // string expected("<tag-name>the value</tag-name>, 30\ntag-name, 8\nthe
    // value, 9\ntag-name, 8\n"); EXPECT_THAT(os.str(), expected);

    EXPECT_THAT(os.str(),
                "<tag-name>the value</tag-name>, 30\ntag-name, "
                "8\nthe value, 9\ntag-name, 8\n");
  }
}

/*
// ={=========================================================================

14.7 Regex Exceptions

When regular expressions are parsed, things can become very complicated. The C++
standard li- brary provides a special exception class to deal with
regular-expression exceptions. This class is derived from std::runtime_error
(see Section 4.3.1, page 41) and provides an additional mem- ber code() to yield
an error code. This might help to find out what’s wrong if an exception is
thrown when processing regular expressions.

Unfortunately, the error codes returned by code() are implementation specific,
so it doesn’t help to print them directly. Instead, you have to use something
like the following header file to deal with regex exceptions in a reasonable
way:

*/

namespace cxx_regex
{
  template <typename T>
  std::string regexCode(T code)
  {
    switch (code)
    {
      case std::regex_constants::error_collate:
        return "error_collate: "
               "regex has invalid collating element name";
      case std::regex_constants::error_ctype:
        return "error_ctype: "
               "regex has invalid character class name";
      case std::regex_constants::error_escape:
        return "error_escape: "
               "regex has invalid escaped char. or trailing escape";
      case std::regex_constants::error_backref:
        return "error_backref: "
               "regex has invalid back reference";
      case std::regex_constants::error_brack:
        return "error_brack: "
               "regex has mismatched ’[’ and ’]’";
      case std::regex_constants::error_paren:
        return "error_paren: "
               "regex has mismatched ’(’ and ’)’";
      case std::regex_constants::error_brace:
        return "error_brace: "
               "regex has mismatched ’{’ and ’}’";
      case std::regex_constants::error_badbrace:
        return "error_badbrace: "
               "regex has invalid range in {} expression";
      case std::regex_constants::error_range:
        return "error_range: "
               "regex has invalid character range, such as ’[b-a]’";
      case std::regex_constants::error_space:
        return "error_space: "
               "insufficient memory to convert regex into finite state";
      case std::regex_constants::error_badrepeat:
        return "error_badrepeat: "
               "one of *?+{ not preceded by valid regex";
      case std::regex_constants::error_complexity:
        return "error_complexity: "
               "complexity of match against regex over pre-set level";
      case std::regex_constants::error_stack:
        return "error_stack: "
               "insufficient memory to determine regex match";
    }
    return "unknown/non-standard regex error code";
  }
} // namespace cxx_regex

// Because we use the grep grammar here but do escape the characters { and },
// the program might have an output such as the following:

TEST(CxxRegex, exception)
{
  using namespace cxx_regex;

  try
  {
    // initialize regular expression with invalid syntax:
    regex pat("\\\\.*index\\{([^}]*)\\}",
              regex_constants::grep | regex_constants::icase);
  } catch (const regex_error &e)
  {
    // cerr << "regex_error: \n"
    //   << " what(): " << e.what() << "\n"
    //   << " code(): " << regexCode(e.code()) << endl;

    EXPECT_THAT(std::string(e.what()),
                "Unexpected character in brace expression.");
    EXPECT_THAT(regexCode(e.code()),
                "error_badbrace: regex has invalid range in {} expression");
  }
}

// ={=========================================================================
TEST(CxxRegex, match_class_1)
{
  std::string data{"/as/services/0/0"};

  {
    try
    {
      // note: ctor can also raise exception
      std::regex reg(R"(/as/services/\d/\d)");

      bool found{};

      found = std::regex_search(data, reg);

      EXPECT_TRUE(found);
    } catch (const regex_error &e)
    {
      std::cout << "what: " << e.what() << ", code: " << e.code() << std::endl;
    }
  }

  {
    try
    {
      // note: ctor can also raise exception
      std::regex reg{"/as/services/[[:digit:]]/[[:digit:]]"};

      bool found{};

      found = std::regex_search(data, reg);

      EXPECT_TRUE(found);
    } catch (const regex_error &e)
    {
      std::cout << "what: " << e.what() << ", code: " << e.code() << std::endl;
    }
  }
}

// ={=========================================================================
TEST(CxxRegex, match_class_2)
{
  std::vector<std::string> coll{"/as/services/0/0",
                                "/as/services/10/20",
                                "/as/services/100/100",
                                "/as/services/234/0"};

  {
    std::regex reg{"/as/services/\\d+/\\d+"};

    for (const auto &e : coll)
    {
      auto found = std::regex_search(e, reg);

      EXPECT_TRUE(found);
    }
  }

  {
    std::regex reg{R"("/as/services/\d+/\d+")"};

    for (const auto &e : coll)
    {
      auto found = std::regex_search(e, reg);

      EXPECT_TRUE(found);
    }
  }
}

// ={=========================================================================
// cxx-bit cxx-bitset cxx-numeric-limit
//
// The addition operation in the CPU is agnostic to whether the integer is
// signed or unsigned. The bit representation is the same.
//
// Here `negate` means that -value but not ~value which is bitwise operation.
//
// If you `negate` 0x80000000, _MIN, you get the same again and that is
// something to look out for because there is `no-change` in bit representation.
// This means abs() has no effect when fed the largest negative number. So bit
// representation is 'agnostic' to whether it's signed or unsigned.

TEST(CxxBit, check_bitset)
{
  // cxx-bitset-code
  //
  // usr/include/c++/4.9/bitset/
  //
  //       /// Initial bits bitwise-copied from a single word (others set to
  //       zero).
  // #if __cplusplus >= 201103L
  //       constexpr bitset(unsigned long long __val) noexcept
  //       : _Base(_Sanitize_val<_Nb>::_S_do_sanitize_val(__val)) { }
  // #else
  //       bitset(unsigned long __val)
  //       : _Base(__val)
  //       { _M_do_sanitize(); }
  // #endif
  //
  // {
  //   int value = 1024;
  //
  //   // warning: narrowing conversion of ‘value’ from ‘int’ to ‘long long
  //   unsigned int’ inside { } [-Wnarrowing] bitset<32> bitsetx{value};
  //   EXPECT_EQ(bitsetx.to_string(), "00000000000000000000010000000000");
  // }

  {
    unsigned int value = 1024;
    std::bitset<32> coll{value};
    EXPECT_EQ(coll.to_string(), "00000000000000000000010000000000");
  }

  // note: can use variable to set size of bitset.
  {
    const int x = 40;
    std::bitset<x> coll(1U);
    EXPECT_THAT(coll.size(), 40);
  }

  {
    std::bitset<32> coll(1U);
    EXPECT_EQ(coll.to_string(), "00000000000000000000000000000001");

    EXPECT_EQ(coll.any(), true);
    EXPECT_EQ(coll.none(), false);
    EXPECT_EQ(coll.all(), false);
    EXPECT_EQ(coll.count(), 1);
    EXPECT_EQ(coll.size(), 32);

    coll.flip();
    EXPECT_EQ(coll.count(), 31);
    coll.reset();
    EXPECT_EQ(coll.count(), 0);
  }

  {
    std::bitset<32> bset_1{1U};
    EXPECT_EQ(bset_1.to_string(), "00000000000000000000000000000001");

    std::bitset<32> bset_2{};
    EXPECT_EQ(bset_2.to_string(), "00000000000000000000000000000000");

    // 2) Sets the bit at position pos to the value value.
    bset_2.set(0);

    EXPECT_THAT(bset_1, bset_2);
  }

  // see the use of bitset and bitset only supports fixed size.
  //
  // How can use bitset with dynamic size since the size is constant expression?
  // Options are:
  //
  // o. vector<bool>
  // o. boost has a dynamic_bitset you can use.

  {
    unsigned short short11 = 1024;
    std::bitset<16> bitset11{short11};
    EXPECT_EQ(bitset11.to_string(), "0000010000000000");

    unsigned short short12 = short11 >> 1; // 512
    std::bitset<16> bitset12{short12};
    EXPECT_EQ(bitset12.to_string(), "0000001000000000");

    unsigned short short13 = short11 >> 10; // 1
    std::bitset<16> bitset13{short13};
    EXPECT_EQ(bitset13.to_string(), "0000000000000001");

    unsigned short short14 = short11 >> 11; // 0
    std::bitset<16> bitset14{short14};
    EXPECT_EQ(bitset14.to_string(), "0000000000000000");
  }
}

// bitset::set()
// 1) Sets all bits to true.
// 2) Sets the bit at position pos to the value value.

TEST(CxxBit, check_bitset_set)
{
  std::bitset<16> coll{};

  // no bits are set
  EXPECT_EQ(coll.count(), 0);

  // 1) Sets all bits to true.
  coll.set();
  EXPECT_EQ(coll.count(), 16);

  coll.reset();
  EXPECT_EQ(coll.count(), 0);

  coll.set(3);
  EXPECT_EQ(coll.to_string(), "0000000000001000");
}

/*
// ={=========================================================================
 vector bool size: 40
 bitset      size: 128
 array bit size  : 128
 array bool size : 1000
*/

TEST(CxxBit, check_size_on_different_types)
{
  const int size{1000};

  std::vector<bool> coll_vector_bool(size);
  std::bitset<size> coll_bitset{1};
  int coll_array_bit[size / 32 + 1];
  bool coll_array_bool[size];

  {
    EXPECT_THAT(coll_vector_bool.size(), size);

#if __GNUC__ && __x86_64__
    // EXPECT_THAT(sizeof(coll_vector_bool), 72); // 64 bits

    // gcc (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0
    std::cout << "vector bool size: " << sizeof(coll_vector_bool) << "\n";
#else
    EXPECT_THAT(sizeof(coll_vector_bool), 1); // 32 bits, need to update
#endif
  }

  {
    EXPECT_THAT(coll_bitset.size(), size);

// 64 bits
#if __GNUC__ && __x86_64__
    std::cout << "bitset      size: " << sizeof(coll_bitset) << "\n";
#else
    EXPECT_THAT(sizeof(coll_bitset), 1);      // 32 bits
#endif
  }

  {
    // 1000/32 = 31, 31+1 = 32, 32*4 = 128
    // EXPECT_THAT(sizeof(coll_array_bit), 128);
    std::cout << "array bit size  : " << sizeof(coll_array_bit) << "\n";
  }

  {
    // EXPECT_THAT(sizeof(coll_array_bool), size);
    std::cout << "array bool size : " << sizeof(coll_array_bool) << "\n";
  }
}

// Programming Pearl, Column 01, Q 02
// How would you implement bit vectors using bitwise logical operations?
//
// C and old C++ programs usually use type long for arrays of bits and
// manipulate them with the bit operators, such as &, |, and ~.

namespace cxx_bit_array
{
  const unsigned int BITSPERWORD = 32;
  const unsigned int SHIFT       = 5;
  const unsigned int MASK        = 0x1F;
  const unsigned int SIZE        = 60;

  // bit array represents a array of bits. Why +1? Since
  //
  // ...  32-63      0-31
  // ...  array[1]   array[0]
  //
  // SIZE is num of bits to represent and BITSPERWORD is num of bits of a
  // word(int). So there should be one array at least for 0-31 bits.

  int a[1 + SIZE / BITSPERWORD];

  void set_bit(int pos)
  {
    // MASK
    // MASK is 11111(31) to convert pos into [31-0] bits. By &, make only [31-0]
    // bits valid and not others and effectively pos - 31 for values
    // which are > 32. that is:
    //
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

    a[pos >> SHIFT] |= (1 << (pos & MASK));
  }

  void clear_bit(int pos) { a[pos >> SHIFT] &= ~(1 << (pos & MASK)); }

  bool test_bit(int pos)
  {
    return (a[pos >> SHIFT] & (1 << (pos & MASK))) ? true : false;
  }
} // namespace cxx_bit_array

TEST(CxxBit, check_bit_array)
{
  using namespace cxx_bit_array;

  auto array_size = sizeof(a) / sizeof(a[0]);

  EXPECT_THAT(array_size, 2);

  set_bit(35);
  set_bit(45);
  EXPECT_EQ(test_bit(45), true);

  // 45        35     31
  // 10000000001000 | 0.....
  // 0x2008         | 0x0
  //
  // for (int i = 0; i < (int)array_size; ++i)
  // {
  //   cout << ":" << hex << (a[i]);
  // }
  // cout << endl;

  clear_bit(45);
  EXPECT_EQ(test_bit(35), true);
  EXPECT_EQ(test_bit(45), false);

  // 35     31
  // 1000 | 0...

  // for (int i = 0; i < (int)array_size; ++i)
  // {
  //   cout << ":" << hex << (a[i]);
  // }
  // cout << endl;
}

/*
// ={=========================================================================
*cxx-bool*

[ RUN      ] CxxBit.check_performace_on_vector_bool
[       OK ] CxxBit.check_performace_on_vector_bool (277 ms)
[ RUN      ] CxxBit.check_performace_on_bitset
[       OK ] CxxBit.check_performace_on_bitset (255 ms)
[ RUN      ] CxxBit.check_performace_on_array_bit
[       OK ] CxxBit.check_performace_on_array_bit (50 ms)
[ RUN      ] CxxBit.check_performace_on_array_bool
[       OK ] CxxBit.check_performace_on_array_bool (34 ms) <<<<<<<<

NOTE array bool is fastest.

*/

// same as uamespace cxx_bit_array
namespace cxx_bit_performace
{
  const unsigned int BITSPERWORD = 32;
  const unsigned int SHIFT       = 5;
  const unsigned int MASK        = 0x1F;
  const unsigned int SIZE        = 1000;
  const unsigned int LOOP        = 10000;

  int coll_array_bit[1 + SIZE / BITSPERWORD];

  void coll_array_set(int pos)
  {
    coll_array_bit[pos >> SHIFT] |= (1 << (pos & MASK));
  }

  void coll_array_clear(int pos)
  {
    coll_array_bit[pos >> SHIFT] &= ~(1 << (pos & MASK));
  }

  bool coll_array_test(int pos)
  {
    return (coll_array_bit[pos >> SHIFT] & (1 << (pos & MASK))) ? true : false;
  }
} // namespace cxx_bit_performace

// ={=========================================================================
TEST(CxxBit, check_performace_on_vector_bool)
{
  using namespace cxx_bit_performace;

  std::vector<bool> coll(SIZE);

  for (size_t loop = 0; loop < LOOP; ++loop)
  {
    for (size_t i = 0; i < SIZE; ++i)
      coll[i] = true;

    for (size_t i = 0; i < SIZE; ++i)
      coll[i] = false;
  }
}

// ={=========================================================================
TEST(CxxBit, check_performace_on_bitset)
{
  using namespace cxx_bit_performace;

  std::bitset<SIZE> coll{};

  for (size_t loop = 0; loop < LOOP; ++loop)
  {
    for (size_t i = 0; i < SIZE; ++i)
      coll.set(i);

    for (size_t i = 0; i < SIZE; ++i)
      coll.reset(i);
  }
}

// ={=========================================================================
TEST(CxxBit, check_performace_on_array_bit)
{
  using namespace cxx_bit_performace;

  for (size_t loop = 0; loop < LOOP; ++loop)
  {
    for (size_t i = 0; i < SIZE; ++i)
      coll_array_set(i);

    for (size_t i = 0; i < SIZE; ++i)
      coll_array_clear(i);
  }
}

// ={=========================================================================
TEST(CxxBit, check_performace_on_array_bool)
{
  using namespace cxx_bit_performace;

  bool coll[SIZE];

  for (size_t loop = 0; loop < LOOP; ++loop)
  {
    for (size_t i = 0; i < SIZE; ++i)
      coll[i] = true;

    for (size_t i = 0; i < SIZE; ++i)
      coll[i] = false;
  }
}

// ={=========================================================================
TEST(CxxBit, check_size_on_array_bool)
{
  using namespace cxx_bit_performace;

  bool coll[SIZE];

  auto size = sizeof coll;

  EXPECT_THAT(size, SIZE);
}

// ={=========================================================================
TEST(CxxBit, bit_RightShift)
{
  // fail
  {
    // "01111111111111111111111111111111" is expected but gets
    // "11111111111111111111111111111111"

    unsigned int int_max_1 = (~((int)0)) >> 1;
    int int_max_2          = (~((int)0)) >> 1;
    EXPECT_EQ(int_max_1, numeric_limits<unsigned int>::max());
    EXPECT_NE(int_max_1, numeric_limits<int>::max());

    // this is wrong
    std::bitset<32> coll{int_max_1};
    EXPECT_EQ(coll.to_string(), "11111111111111111111111111111111");

    EXPECT_EQ(int_max_2, numeric_limits<unsigned int>::max());
    EXPECT_NE(int_max_2, numeric_limits<int>::max());
  }

  // okay
  {
    unsigned int uint_max_1 = (((unsigned)~0) >> 1);
    unsigned int uint_max_2 = ((unsigned)~0 >> 1); // okay, since cast is higher
    EXPECT_EQ(uint_max_1, numeric_limits<int>::max());
    EXPECT_EQ(uint_max_2, numeric_limits<int>::max());
  }

  // okay
  {
    int uint_max_1 = (((unsigned)~0) >> 1);
    int uint_max_2 = ((unsigned)~0 >> 1); // okay, since cast is higher
    EXPECT_EQ(uint_max_1, numeric_limits<int>::max());
    EXPECT_EQ(uint_max_2, numeric_limits<int>::max());
  }

  // signed int and unsigned int
  {
    // >>> 2**31-1
    // 2147483647
    EXPECT_EQ(2147483647, numeric_limits<int>::max());

    // >>> 2**32-1
    // 4294967295
    EXPECT_EQ(4294967295, numeric_limits<unsigned int>::max());
  }

  // okay
  {
    // input3 works since the result is `independant` but assigned to unsigned.
    unsigned int input = ~0;
    input >>= 1;
    EXPECT_EQ(input, numeric_limits<int>::max());
  }

  {
    unsigned int uint_max = ~((unsigned int)0);

    // cxx.cpp:3195:56: warning: left shift count >= width of type
    // unsigned int uint_max_two = (((unsigned int)1U) << 32) -1;

    int int_max = uint_max >> 1;
    int int_min = int_max + 1;

    bitset<32> bitsetx{(unsigned int)int_max};
    EXPECT_EQ(bitsetx.to_string(), "01111111111111111111111111111111");

    EXPECT_EQ(uint_max, numeric_limits<unsigned int>::max());

    EXPECT_EQ(int_max, numeric_limits<int>::max());
    EXPECT_EQ(int_min, numeric_limits<int>::min());
  }

  // why is that different? the main point is *cxx-shift-right-shift*
  //
  // 1. when use `bitwise not`, the size and signness is `independant`. The
  // result `depends on the other operand` and done at `compile-time`.
  //
  // so ~(0) makes "111...11" which don't have size and signness.
  //
  // 2. The signess must be known to compiler when do shift to have guaranteed
  // result. Since do not know signness, use `signed by default` and gets 1
  // for MSB when right-shift
  //
  // *cxx-shift-right-shift*
  // the point is that must use `unsigned` to do `right-shift`
  // in order to have guaranteed 0 values.
  //
  // so (~(0)>>1) makes "111...11"
  //
  // This is why glibc macro uses unsigned type which set size and signness.
  // from glibc and see exercise 2-1 for examples.
  //
  // # ifndef ULONG_MAX
  // #  define ULONG_MAX ((unsigned long int) ~(unsigned long int) 0)
  // # endif
  // # ifndef LONG_MAX
  // #  define LONG_MAX ((long int) (ULONG_MAX >> 1))
  // # endif

  // left-shift do not matter
  {
    unsigned int value = (1 << 10);
    bitset<32> coll{value};
    EXPECT_EQ(coll.to_string(), "00000000000000000000010000000000");
  }

  {
    // get bits which has [5, 0]th bis on. e.g. 0001.1111
    // in this case, do not need to specify unsigned.

    int value{};

    const int POS_TO_TURN_ON = 5;
    value                    = ~(~0 << POS_TO_TURN_ON);
    EXPECT_THAT(value, 0x1F);
  }
}

// *cxx-twos-complement*
TEST(CxxBit, bit_MaxNegagiveIsSpecial)
{
  // get max negative, ???_MIN
  int int_min = (~((unsigned int)0) >> 1) + 1;

  bitset<32> bitset_int_min(int_min);
  EXPECT_EQ(bitset_int_min.to_string(), "10000000000000000000000000000000");

  // what'd happen when negate ???_MIN?
  int negate_min = -int_min;
  bitset<32> bitset_negate_min(negate_min);
  EXPECT_EQ(bitset_negate_min.to_string(), "10000000000000000000000000000000");
}

// Why '%' produce negative value?
//
// 51
// -10
// 90
// 6
// -76
// 65
// 69
// 11
// 86
// 64
//
// The reason is that rand() makes 32 bit random numbers and bigrand() makes
// even bigger so overflow happens. overflow happens and return value becomes
// negative.
//
// As ANSIC says:
//
// "The direction of truncation for / and the sign of result for % are machine
// dependent for negative operands, as is the action taken on overflow or
// underflow".

namespace bit_overflow
{
  int bigrand() { return RAND_MAX * rand() + rand(); }
} // namespace bit_overflow

TEST(CxxBit, bit_Overflow)
{
  using namespace bit_overflow;

  for (int i = 0; i < 10; ++i)
    cout << (bigrand() % 100) << endl;
}

/*
// ={=========================================================================
cxx-min-cxx-max cxx-max-element

template< class T >
const T& min( const T& a, const T& b );

template< class T >
T min( std::initializer_list<T> ilist );

template< class T >
std::pair<T,T> minmax( std::initializer_list<T> ilist);

*/

TEST(CxxMinMax, check_order)
{
  auto value1 = std::max(10, 20);
  auto value2 = std::max(20, 10);

  EXPECT_TRUE(value1 == value2);
}

namespace cxx_min_max
{
  bool AbsLess(int elem1, int elem2) { return abs(elem1) < abs(elem2); }
} // namespace cxx_min_max

TEST(CxxMinMax, use)
{
  {
    EXPECT_THAT(std::max({2, 3, 4, 5, 6, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6}), 6);
    EXPECT_THAT(std::min({2, 3, 4, 5, 6, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6}), -3);

    // compile error so have to use xxx_element().
    // EXPECT_THAT(std::max(coll), 6);
    // EXPECT_THAT(std::min(coll), -3);
  }

  {
    auto result = std::minmax({2, 3, 4, 5, 6, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6});

    EXPECT_THAT(result.first, -3);
    EXPECT_THAT(result.second, 6);
  }
}

TEST(CxxMinMax, check_difference)
{
  using namespace cxx_min_max;

  {
    std::deque<int> coll{2, 3, 4, 5, 6, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6};

    // NOTE: does it matter since we want max value but not index of max
    // element? yes if want to know the pos of it
    //
    // If more than one minimum or maximum element exists, min_element() and
    // max_element() return `the first` found; minmax_element() returns the first
    // minimum but the last maximum element, so max_element() and minmax_element()
    // don’t yield the same maximum element.

    EXPECT_THAT(*max_element(coll.begin(), coll.end()), 6);
    EXPECT_THAT(*min_element(coll.begin(), coll.end()), -3);

    // get the first
    EXPECT_THAT(distance(coll.begin(), max_element(coll.begin(), coll.end())),
                4);

    // return iterator pair
    // Note also that minmax_element() yields `the last maximum`, so the distance
    // 9.
    auto minmax = minmax_element(coll.begin(), coll.end());
    EXPECT_THAT(*(minmax.first), -3); // first minimum
    EXPECT_THAT(*(minmax.second), 6); // last maximum

    // last maximum is 6 which is the last element so minmax returns the last
    // max.
    EXPECT_THAT(distance(coll.begin(), minmax.second), coll.size() - 1);

    // see difference
    EXPECT_THAT(distance(minmax.first, minmax.second), 9);
    EXPECT_THAT(distance(min_element(coll.begin(), coll.end()),
                         max_element(coll.begin(), coll.end())),
                -1);

    // min/max of absolute values
    EXPECT_THAT(*min_element(coll.begin(), coll.end(), AbsLess), 0);
    EXPECT_THAT(*max_element(coll.begin(), coll.end(), AbsLess), 6);
  }
}

// ={=========================================================================
// cxx-integer-division

// From ansic 1.2., this is a program to implement a formula "C = (5/9)(F-32)"
// to print a table of Fahrenheit and Celsius.
//
// print Fahrenheit-Celsius table

// fah:   0 cel:   0
// fah:  20 cel: -17
// fah:  40 cel:  -6
// fah:  60 cel:   4
// fah:  80 cel:  15
// fah: 100 cel:  26
//
// celsius = 5 * (fahr-32) / 9;      // okay
// celsius = 5/9 * (fahr-32);        // not okay
//
// Since `integer-division` truncates; any fractional part is discarded so 5/9
// would be trancated to zero.

TEST(CxxIntegerDivision, fahrenheit_celsius)
{
  int fah{}, cel{};
  int high{100}, step{20};

  while (fah <= high)
  {
    cout << " fah: " << setw(3) << setfill(' ') << fah << " cel: " << setw(3)
         << setfill(' ') << cel << endl;
    cel = 5 * (fah - 32) / 9;
    fah += step;
  }
}

// "5/9" is still integer division.
//
// If an arithmetic operator has integer operands, an integer operation is
// performed. If an arithmetic operator has one floating-point operand and one
// integer operand, howerver, the integer will be converted to floating point
// before the operation is done.
//
// "while(fhar <= upper)" and "fahr-32" would be converted to floating point.
//
// note: Nevertheless, writing floating-point constants with explicit decimal
// point 'emphasizes' their floating-point nature for human readers.

// fah: 0.000000 cel: 0.000000
// fah: 20.000000 cel: -17.777779
// fah: 40.000000 cel: -6.666667
// fah: 60.000000 cel: 4.444445
// fah: 80.000000 cel: 15.555555
// fah: 100.000000 cel: 26.666666

// ={=========================================================================
TEST(CxxIntegerDivision, fahrenheit_celsius_use_float)
{
  float fah{}, cel{}; // see
  int high{100}, step{20};

  // no warnings
  while (fah <= high)
  {
    cout << " fah: " << setw(3) << setfill(' ') << fah << " cel: " << setw(3)
         << setfill(' ') << cel << endl;

    // cel = 5 * (fah-32) / 9;
    // cel = 5/9 * (fah-32);        // not okay
    // cel = 5/9 * (fah-32.0);      // not okay

    cel = 5 / 9.0 * (fah - 32); // okay
    // cel = 5.0/9.0 * (fah-32.0);  // okay

    fah += step;
  }
}

// value
// ----- x 100
// total
//
// [ RUN      ] CxxIntegerDivision.percentage
//  value:  10, %   :   0
//  value:  40, %   :   0
//  value: 100, %   :   0
//  value: 200, %   :   0
//  value: 250, %   :   0
//  value: 356, %   :   0
//  value: 450, %   :   0
//  value: 600, %   : 100
//
//  value:  10, %   :   1
//  value:  40, %   :   6
//  value: 100, %   :  16
//  value: 200, %   :  33
//  value: 250, %   :  41
//  value: 356, %   :  59
//  value: 450, %   :  75
//  value: 600, %   : 100
//
//  value:  10, %   :   0
//  value:  40, %   :   0
//  value: 100, %   :   0
//  value: 200, %   :   0
//  value: 250, %   :   0
//  value: 356, %   :   0
//  value: 450, %   :   0
//  value: 600, %   : 100
// [       OK ] CxxIntegerDivision.percentage (0 ms)

// ={=========================================================================
TEST(CxxIntegerDivision, percentage)
{
  int total{600}, perce{};
  vector<int> values{10, 40, 100, 200, 250, 356, 450, 600};

  // wrong
  for (auto value : values)
  {
    perce = value / total * 100;
    cout << " value: " << setw(3) << setfill(' ') << value
         << ", %   : " << setw(3) << setfill(' ') << perce << endl;
  }

  // right
  for (auto value : values)
  {
    perce = (value * 100) / total;
    cout << " value: " << setw(3) << setfill(' ') << value
         << ", %   : " << setw(3) << setfill(' ') << perce << endl;
  }

  // wrong
  for (auto value : values)
  {
    perce = (value / total) * 100;
    cout << " value: " << setw(3) << setfill(' ') << value
         << ", %   : " << setw(3) << setfill(' ') << perce << endl;
  }
}

// ={=========================================================================
TEST(CxxIntegerDivision, on_minus_value)
{
  EXPECT_THAT(((60 - 50) / 10), 1);
  EXPECT_THAT(((50 - 50) / 10), 0);

  EXPECT_THAT(((40 - 50) / 10), -1);
  EXPECT_THAT(((34 - 50) / 10), -1);
}

// ={=========================================================================
// cxx-shift cxx-floor cxx-ceil

TEST(CxxIntegerDivision, shift_operation)
{
  int value = 9;

  EXPECT_THAT(value / 2, 4);
  EXPECT_THAT(value >> 1, 4);

  // FLOOR(3)
  //
  // double floor(double x);
  // float floorf(float x);
  // long double floorl(long double x);
  //
  // Link with -lm.
  //
  // DESCRIPTION
  // These functions return the largest integral value that is not greater than
  // x.
  //
  // For example, floor(0.5) is 0.0, and floor(-0.5) is -1.0.
  //
  // CEIL(3)
  //
  // DESCRIPTION These functions return the smallest integral value that is not
  // less than x.
  //
  // For example, ceil(0.5) is 1.0, and ceil(-0.5) is 0.0.
  //
  // ROUND(3)
  //
  // DESCRIPTION
  // These  functions  round  x to the nearest integer, but round halfway cases
  // away from zero (regardless of the current rounding direction, see fenv(3)),
  // instead of to the nearest even integer like rint(3).
  //
  // For example, round(0.5) is 1.0, and round(-0.5) is -1.0.

  float result = value / 2.0;

  EXPECT_THAT(ceil(result), 5);
  EXPECT_THAT(floor(result), 4);
  EXPECT_THAT(round(result), 5);

  EXPECT_THAT(round(0.4), 0);
  EXPECT_THAT(round(0.5), 1);
  EXPECT_THAT(round(0.6), 1);

  EXPECT_THAT(floor(-2.1), -3);
  EXPECT_THAT(ceil(-2.1), -2);
}

// ={=========================================================================
// cxx-operator

// Minimum and Maximum Value Operators
//
// GCC’s g++ compiler adds the <? and >? operators, which, respectively, return
// the minimum and maximum of two integer arguments. For example, the following
// statement would assign 10 to the min variable:
//
// min = 10 <? 15 ;
//
// Similarly, the following statement would assign 15 to the max variable:
//
// max = 10 >? 15;
//
// NOTE: DO NOT COMPILE
//
// TEST(Operator, MinMax)
// {
//   int value{};
//
//   // min
//   value = 10 <? 20;
//   EXPECT_THAT(value, 10);
//
//   // max
//   value = 10 >? 20;
//   EXPECT_THAT(value, 10);
// }

TEST(Operator, PrefixPostfix)
{
  int i = 2;

  i++;
  EXPECT_THAT(i, 3);

  // error: lvalue required as increment operand
  // i++++;

  ++++i;
  EXPECT_THAT(i, 5);
}

// ={=========================================================================
// cxx-template

namespace cxx_template
{
  template <typename T>
  int compare(const T &a, const T &b)
  {
    if (a < b)
      return -1;
    if (b < a)
      return 1;
    return 0;
  }
} // namespace cxx_template

TEST(CxxTemplate, function)
{
  using namespace cxx_template;

  // `template-parameter-type` T is "int"
  // int compare(const int& v1, const int& v2)

  EXPECT_THAT(compare(1, 2), -1);
  EXPECT_THAT(compare(2, 1), 1);
  EXPECT_THAT(compare(2, 2), 0);

  // T is "vector<int>" and  instantiates to
  // "int compare(const vector<int>&, const vector<int>&)"

  std::vector<int> coll1{1, 2, 3}, coll2{1, 2, 4};
  EXPECT_THAT(compare(coll1, coll2), -1);
  EXPECT_THAT(compare(coll2, coll1), 1);
}

namespace cxx_template
{
  template <typename T>
  class VectorColl
  {
  private:
    std::vector<T> _coll;

  public:
    VectorColl() { _coll = {1, 2, 3, 4, 5}; }

    std::vector<T> getColl() { return _coll; }
  };
} // namespace cxx_template

// ={=========================================================================
TEST(CxxTemplate, class)
{
  using namespace cxx_template;

  VectorColl<int> coll;
  EXPECT_THAT(coll.getColl(), ElementsAre(1, 2, 3, 4, 5));
}

// ={=========================================================================
namespace cxx_template
{
  // Q: strcmp returns +5/-5 than 1/-1

  int internal_strcmp(const char *p1, const char *p2)
  {
    while (true)
    {
      unsigned int c1 = *p1;
      unsigned int c2 = *p2;

      if (c1 == c2)
        break;
      if (c1 != c2)
        return c1 < c2 ? -1 : 1;
    }

    return 0;
  }

  // *cxx-array*. non-type-parameter version
  template <unsigned N, unsigned M>
  int compare(const char (&p1)[N], const char (&p2)[M])
  {
    return internal_strcmp(p1, p2);
  }

  // specialisation version
  template <>
  int compare(const char *const &p1, const char *const &p2)
  {
    return internal_strcmp(p1, p2);
  }
} // namespace cxx_template

TEST(CxxTemplate, specialisation_1)
{
  using namespace cxx_template;

  {
    // cxx-strcmp which return 0 in case both are the same
    EXPECT_THAT(!strcmp("strcmp", "strcmp"), true);
    EXPECT_THAT(!internal_strcmp("strcmp", "strcmp"), true);

    EXPECT_THAT(strcmp("strcmp", "strcmpx"), Not(true));
    EXPECT_THAT(internal_strcmp("strcmp", "strcmpx"), Not(true));
  }

  {
    // use non-type version
    EXPECT_THAT(compare("mon", "hi"), 1);
    EXPECT_THAT(compare("hi", "mon"), -1);

    // cxx.cpp:2829:33: error: call of overloaded ‘compare(const char [3], const
    // char [3])’ is ambiguous

    // EXPECT_THAT(compare("hi", "hi"), 0);

    // use specialisation version.
    const char *p1 = "hi";
    const char *p2 = "mon";
    EXPECT_THAT(compare(p1, p2), -1);
  }
}

/*
// ={=========================================================================
use macro for specialisation.

#define SDBUS_READ_BASIC_TYPE(type, ctype, defaultValue)                        \
    template<>                                                                  \
    type SDBusArgumentReader::readValue< type >() const                         \
    {                                                                           \
        type value = defaultValue;                                              \
        int rc = sd_bus_message_read_basic(mMessage, ctype, &value);            \
        if (rc < 0)                                                             \
        {                                                                       \
            AS_LOG_WARNING("failed to read value of type '%c' (%d - %s)",       \
                           ctype, rc, sdBusErrorString(rc));                    \
        }                                                                       \
        return value;                                                           \
    }

SDBUS_READ_BASIC_TYPE(bool,     SD_BUS_TYPE_BOOLEAN, false)
SDBUS_READ_BASIC_TYPE(int32_t,  SD_BUS_TYPE_INT32,   INT32_MAX)
SDBUS_READ_BASIC_TYPE(uint32_t, SD_BUS_TYPE_UINT32,  UINT32_MAX)
SDBUS_READ_BASIC_TYPE(int64_t,  SD_BUS_TYPE_INT64,   INT64_MAX)
SDBUS_READ_BASIC_TYPE(uint64_t, SD_BUS_TYPE_UINT64,  UINT64_MAX)
SDBUS_READ_BASIC_TYPE(double,   SD_BUS_TYPE_DOUBLE,  UINT64_MAX)

template<>
std::string SDBusArgumentReader::readValue<std::string>() const
{
    const char *str = nullptr;

    int rc = sd_bus_message_read_basic(mMessage, SD_BUS_TYPE_STRING, &str);
    if ((rc < 0) || !str)
    {
        AS_LOG_WARNING("failed to read string value");
        return std::string();
    }

    return std::string(str);
}

NOTE:
1 "specialisation" is done without the original code

2 they are all the same code other than std::string type case. then why
  specialisation? If use usual template, should have three type parameters.

*/
namespace cxx_template
{
  class SDBusArgumentReader
  {
  private:
    std::stringstream ss_{};

  public:
    SDBusArgumentReader()  = default;
    ~SDBusArgumentReader() = default;

    template <typename T>
    T readValue() const;
  };

// *cxx-template-specialization*
#define SDBUS_READ_BASIC_TYPE(type, ctype, defaultValue)                       \
  template <>                                                                  \
  type SDBusArgumentReader::readValue<type>() const                            \
  {                                                                            \
    type value = defaultValue;                                                 \
    if (typeid(value).name() == typeid(bool).name())                           \
      return true;                                                             \
    if (typeid(value).name() == typeid(int32_t).name())                        \
      return 1;                                                                \
    return value;                                                              \
  }

  SDBUS_READ_BASIC_TYPE(bool, "b", false)
  SDBUS_READ_BASIC_TYPE(int32_t, "i", INT32_MAX)

  class SDBusMessage
  {
  private:
    const SDBusArgumentReader mReader;

  public:
    template <typename T>
    const SDBusMessage &operator>>(T &arg) const;
  };

  template <typename T>
  const SDBusMessage &SDBusMessage::operator>>(T &arg) const
  {
    arg = mReader.readValue<T>();
    return *this;
  }

  // *cxx-template-explicit-instantiation*
  template const SDBusMessage &SDBusMessage::operator>><bool>(bool &) const;
  template const SDBusMessage &SDBusMessage::operator>>
    <int32_t>(int32_t &) const;
} // namespace cxx_template

TEST(CxxTemplate, specialisation_2)
{
  using namespace cxx_template;

  SDBusMessage message;

  bool read1{false};
  int read2{-1};

  message >> read1 >> read2;

  EXPECT_THAT(read1, true);
  EXPECT_THAT(read2, 1);
}

// can use value also as a type but nontype template parameters are limited:
//
// https://en.cppreference.com/w/cpp/language/template_parameters
// A non-type template parameter must have a structural type, which is one of
// the following types (optionally cv-qualified, the qualifiers are ignored):
//
// std::array example:
//
// std::array<int, 8> coll = {};
//
// EXPECT_THAT(coll.size(), 8);
//
// template<typename _Tp, std::size_t _Nm>
//   struct array
//   {
//     // ...
//   }

namespace cxx_template
{
  template <typename T, int size>
  class FileBuf
  {
  public:
    int get_size() { return sizeof(array_) / sizeof(T); }

  private:
    T array_[size];
  };

  template <int MIN, int MAX>
  struct RangedIntPolicy
  {
    typedef int value_type;
    value_type value_ = MIN;
    // value_type value_{MIN};

    int assign(value_type value)
    {
      if ((value < MIN) || (value > MAX))
        return -1;

      value_ = value;
      return 0;
    }
  };

  template <typename T, int num>
  T add_num(const T t)
  {
    return t + num;
  }

  enum class Mode
  {
    throw_,
    terminate_,
    ignore_
  };

  // can come from build env
  // constexpr Mode current_mode = CURRENT_MODE;
  constexpr Mode current_mode = Mode::throw_;

  // constexpr int current_level = CURRENT_LEVEL;
  constexpr int current_level = 1;

  constexpr int default_level = 1;

  constexpr bool level(int n) { return n <= current_level; }

  struct Error : std::runtime_error
  {
    Error(const std::string &p)
        : runtime_error(p)
    {}
  };

  std::string compose(const char *file, int line, const std::string &message)
  {
    std::ostringstream os{"("};
    os << file << ", " << line << "):" << message;
    return os.str();
  }

  // original code. C++ PL 361
  template <bool condition = level(default_level), typename E = Error>
  void dynamic1(bool assertion,
                const string &message = "Assert::dynamic failed")
  {
    std::cout << "assertion: " << assertion << ", condition: " << condition
              << "\n";

    // like assert(A), if A is true, do nothing.
    if (assertion)
    {
      std::cout << "assertion is true\n";
      return;
    }

    // if A is false, do action as to mode

    if (current_mode == Mode::throw_)
    {
      // throw E(message);
      std::cout << "throw E{" << message << "}\n";
    }

    if (current_mode == Mode::terminate_)
    {
      // std::terminate();
      std::cout << "std::terminate\n";
    }
  }

#if 0
  // specialisation and do nothing
  template<>
    void dynamic1<false, Error>(bool, const string &)
    {}

  // default (current) action
  void dynamic1(bool A, const string &message)
  {
    dynamic1<true, Error>(A, message);
  }

  // default (current) action and default message
  void dynamic1(bool A) { dynamic1<true, Error>(A); }
#endif

  // original code is for having leven and action selection at runtime. so added
  // condifion in if check.
  template <bool condition = level(default_level), typename E = Error>
  void dynamic2(bool assertion,
                const string &message = "Assert::dynamic failed")
  {
    std::cout << "assertion: " << assertion << ", condition: " << condition
              << "\n";

    // like assert(A), if A is true, do nothing.
    if (condition && assertion)
    {
      std::cout << "assertion is true\n";
      return;
    }

    // if A is false, do action as to mode

    if (current_mode == Mode::throw_)
    {
      // throw E(message);
      std::cout << "throw E{" << message << "}\n";
    }

    if (current_mode == Mode::terminate_)
    {
      // std::terminate();
      std::cout << "std::terminate\n";
    }
  }
} // namespace cxx_template

// ={=========================================================================
TEST(CxxTemplate, use_non_type_argument_1)
{
  using namespace cxx_template;

  {
    FileBuf<int, 20> fbuf;
    EXPECT_THAT(fbuf.get_size(), 20);
  }

  {
    RangedIntPolicy<10, 100> rint;
    EXPECT_THAT(rint.assign(9), -1);
    EXPECT_THAT(rint.assign(101), -1);
    EXPECT_THAT(rint.assign(30), 0);
  }

  // parse error
  // EXPECT_THAT(add_num<int, 5>(10), 15);

  auto ret = add_num<int, 5>(10);
  EXPECT_THAT(ret, 15);

  ret = add_num<int, 15>(10);
  EXPECT_THAT(ret, 25);
}

/*
// ={=========================================================================

when there are overloads

[ RUN      ] CxxTemplate.use_non_type_argument_2

assertion: 1, condition: 1
assertion is true

assertion: 1, condition: 1
assertion is true

[       OK ] CxxTemplate.use_non_type_argument_2 (0 ms)


when there are no overloads

[ RUN      ] CxxTemplate.use_non_type_argument_2

assertion: 1, condition: 0
assertion is true

assertion: 0, condition: 0
throw E{/home/keitee/git/kb/code-cxx/cxx/test_cxx_1.cpp, 13044):range problem}

assertion: 1, condition: 1
assertion is true

assertion: 1, condition: 1
assertion is true

[       OK ] CxxTemplate.use_non_type_argument_2 (0 ms)
*/

TEST(CxxTemplate, use_non_type_argument_2)
{
  using namespace cxx_template;

  int value1{11};

  dynamic1<level(2), Error>((1 < value1 && value1 < 20),
                            compose(__FILE__, __LINE__, "range problem"));

  int value2{21};

  dynamic1<level(2), Error>((1 < value2 && value2 < 20),
                            compose(__FILE__, __LINE__, "range problem"));

  dynamic1((1 < value1 && value1 < 20),
           compose(__FILE__, __LINE__, "range problem"));

  dynamic1((1 < value1 && value1 < 20));
}

/*
// ={=========================================================================

[ RUN      ] CxxTemplate.use_non_type_argument_3
assertion: 1, condition: 0
assertion is true
assertion: 0, condition: 0
throw E{/home/keitee/git/kb/code-cxx/cxx/test_cxx_1.cpp, 12999):range problem}


condition is false but not return on first if check?

assertion: 1, condition: 0
throw E{/home/keitee/git/kb/code-cxx/cxx/test_cxx_1.cpp, 13006):range problem}

assertion is false but how get throw?

assertion: 0, condition: 0
throw E{/home/keitee/git/kb/code-cxx/cxx/test_cxx_1.cpp, 13011):range problem}
[       OK ] CxxTemplate.use_non_type_argument_3 (0 ms)

*/

TEST(CxxTemplate, use_non_type_argument_3)
{
  using namespace cxx_template;

  int value1{11};

  dynamic2<level(2), Error>((1 < value1 && value1 < 20),
                            compose(__FILE__, __LINE__, "range problem"));

  int value2{21};

  dynamic2<level(2), Error>((1 < value2 && value2 < 20),
                            compose(__FILE__, __LINE__, "range problem"));
}

namespace cxx_template_default
{
  // `This shows how function-object is useful` *cxx-functor*
  // default template argument, the 'type' of callable
  // default function argument, F()
  //
  // like cxx-unique-ptr, should create F() internally.

  template <typename T, typename F = less<T>>
  int compare_1(const T &a, const T &b, F f = F())
  {
    if (f(a, b))
      return -1;
    if (f(b, a))
      return 1;
    return 0;
  }

  // this is wrong.
  template <typename T, typename F = less<T>>
  int compare_2(const T &a, const T &b, F f)
  {
    if (f(a, b))
      return -1;
    if (f(b, a))
      return 1;
    return 0;
  }

  // so if don't use default F then this is the same
  template <typename T, typename F>
  int compare_3(const T &a, const T &b, F f)
  {
    if (f(a, b))
      return -1;
    if (f(b, a))
      return 1;
    return 0;
  }

  template <typename T = int>
  struct Numbers
  {
    Numbers(T value = 0)
        : value_(value)
    {}

    T value_;
  };
} // namespace cxx_template_default

/*
// ={=========================================================================
Primer Plus 16.1 666

o can define "typedef" to that "instantiated class":

  typedef std::vector<std::string> coll;

o *cxx-11* let us define "type alias" for class template:

*/
namespace cxx_template
{
  template <typename T>
  using twin = std::pair<T, T>;

  // can fix one or more of parameters
  template <typename T>
  using partNo = std::pair<T, unsigned>;
} // namespace cxx_template

TEST(CxxTemplate, type_alias)
{
  using namespace cxx_template;

  {
    twin<std::string> author{"book", "name"};
    EXPECT_THAT(author.first, "book");
    EXPECT_THAT(author.second, "name");

    twin<int> value{10, 100};
    EXPECT_THAT(value.first, 10);
    EXPECT_THAT(value.second, 100);
  }

  {
    partNo<std::string> books{"book", 1000};
    EXPECT_THAT(books.first, "book");
    EXPECT_THAT(books.second, 1000);

    partNo<int> code{3301, 1000};
    EXPECT_THAT(code.first, 3301);
    EXPECT_THAT(code.second, 1000);
  }
}

// ={=========================================================================
TEST(CxxTemplate, default_type_argument)
{
  using namespace cxx_template_default;

  // use default F.
  {
    // 1 < 2
    EXPECT_THAT(compare_1(1, 2), -1);
    // 2 > 1
    EXPECT_THAT(compare_1(2, 1), 1);
    // 2 == 2
    EXPECT_THAT(compare_1(2, 2), 0);
  }

  // override default F.
  {
    // 1 < 2
    EXPECT_THAT(compare_1(1, 2, std::greater<int>()), 1);
    // 2 > 1
    EXPECT_THAT(compare_1(2, 1, std::greater<int>()), -1);
    // 2 == 2
    EXPECT_THAT(compare_1(2, 2, std::greater<int>()), 0);
  }

  // compile error
  // // use default F.
  // {
  //   // 1 < 2
  //   EXPECT_THAT(compare_2(1, 2), -1);
  //   // 2 > 1
  //   EXPECT_THAT(compare_2(2, 1), 1);
  //   // 2 == 2
  //   EXPECT_THAT(compare_2(2, 2), 0);
  // }

  // override default F.
  {
    // 1 < 2
    EXPECT_THAT(compare_2(1, 2, std::greater<int>()), 1);
    // 2 > 1
    EXPECT_THAT(compare_2(2, 1, std::greater<int>()), -1);
    // 2 == 2
    EXPECT_THAT(compare_2(2, 2, std::greater<int>()), 0);
  }

  // do not use default.
  {
    // 1 < 2
    EXPECT_THAT(compare_3(1, 2, std::greater<int>()), 1);
    // 2 > 1
    EXPECT_THAT(compare_3(2, 1, std::greater<int>()), -1);
    // 2 == 2
    EXPECT_THAT(compare_3(2, 2, std::greater<int>()), 0);
  }

  {
    vector<int> coll1{1, 2, 3}, coll2{1, 2, 4};
    EXPECT_THAT(compare_1(coll1, coll2), -1);
    EXPECT_THAT(compare_1(coll2, coll1), 1);
  }

  {
    Numbers<> o1(10);
    Numbers<int> o2(10);
    EXPECT_THAT(o1.value_, o2.value_);
  }

  // compile error
  // {
  //   Numbers<> o1(10);
  //   Numbers<std::string> o2("value");
  //   EXPECT_THAT(o1.value_, o2.value_);
  // }
}

namespace cxx_template
{
  class DebugDelete
  {
  public:
    DebugDelete(std::ostream &os = std::cerr)
        : os_(os)
    {}

    // *cxx-functor*
    // as with any function template, T is deduced by the compiler

    template <typename T>
    void operator()(T *p) const
    {
      os_ << "deleting " << typeid(p).name() << ", p = " << p << endl;
      delete p;
    }

  private:
    // *cxx-reference-member
    std::ostream &os_;
  };
} // namespace cxx_template

// ={=========================================================================
TEST(CxxTemplate, memeber_template)
{
  using namespace cxx_template;

  // deleting Pd, p = 0x9be7008
  // deleting Pi, p = 0x9be7018
  {
    double *pd = new double();
    int *pi    = new int();
    DebugDelete d;
    d(pd);
    d(pi);
  }

  // see TEST(CxxSmartPointer, check_deleter);
  //
  // deleting PSs, p = 0x1d5e830
  // deleting Pi, p = 0x1d5e850
  {
    std::unique_ptr<int, DebugDelete> pi(new int, DebugDelete());
    std::unique_ptr<std::string, DebugDelete> ps(new std::string,
                                                 DebugDelete());
  }
}

namespace cxx_template_return_type
{
  // use cxx-iter-trait but limited when T is iterator
  template <typename T>
  typename std::iterator_traits<T>::value_type &return_element_01(T first,
                                                                  T last)
  {
    (void)last;
    return *first;
  }

  template <typename T>
  auto return_element_02(T first, T last) ->
    typename std::iterator_traits<T>::reference
  {
    (void)last;
    return *first;
  }

  // use cxx-trailing-return before cxx-14
  template <typename T>
  auto return_element_03(T first, T last) -> decltype(*first)
  {
    (void)last;
    return *first;
  }

  // : error: ‘first’ was not declared in this scope
  // template <typename Iterator>
  // decltype(*first) return_element_04(Iterator first, Iterator last)
  // {
  //   return *first;
  // }

  // 1.3. C++ Templates The Complete Guide Second Edition
  // 1.3.2 Deducing the Return Type

  template <typename T>
  T max_01(T const &a, T const &b)
  {
    return b < a ? a : b;
  }

  // declare that the return type is derived from what operator?: yields:

  template <typename T1, typename T2>
  auto max_02(T1 a, T2 b) -> decltype(b < a ? a : b)
  {
    return b < a ? a : b;
  }

  // *cxx-14*
  // Since C++14, this is possible by simply not declaring any return type (you
  // still have to declare the return type to be auto):
  //
  // template <typename T1, typename T2>
  //   auto max_02(T1 a, T2 b)
  //   {
  //     return b < a ? a : b;
  //   }
  //
  // indicates that the actual return type must be deduced from the return
  // statements in the function body. In fact, using true as the condition for
  // operator?: in the declaration is enough:
  //
  // Note that an initialization of type auto always decays. This also applies
  // to return values when the return type is just auto.

  template <typename T1, typename T2>
  auto max_03(T1 a, T2 b) -> decltype(true ? a : b)
  {
    return b < a ? a : b;
  }

  // *cxx-type-trait-decay* *cxx-decay*
  // However, in any case this definition has a significant drawback: It might
  // happen that the return type is a reference type, because under some
  // conditions T might be a reference. For this reason you should return the
  // type decayed from T, which looks as follows:
  //
  // o Yields the decayed type of T.
  // o In detail, for type T the following transformations are performed:
  // – First, remove_reference (see Section D.4 on page 729) is applied.
  //
  // xxx_t is the same as xxx<T>::type and which is *cxx-14*
  //
  // decay_t<int const&> // yields int
  // decay_t<int const[4]> // yields int const*
  // void foo();
  // decay_t<decltype(foo)> // yields void(*)()

  template <typename T1, typename T2>
  auto max_04(T1 a, T2 b) -> typename std::decay<decltype(true ? a : b)>::type
  {
    return b < a ? a : b;
  }

  // cxx-error
  // template <typename T1, typename T2>
  //   typename std::decay<decltype(true ? a : b )>::type
  //   max_05(T1 a, T2 b)
  //   {
  //     return b < a ? a : b;
  //   }

  // *cxx-14*
  // However, since C++14 you can simplify the usage of traits like this by
  // appending _t to the trait name and skipping typename and ::type

  // template <typename T1, typename T2>
  //   auto max_05(T1 a, T2 b)
  //   -> std::decay_t<decltype(true ? a : b )>
  //   {
  //     return b < a ? a : b;
  //   }

  // *cxx-type-trait-commontype*
  // CXXSLR-5.4.1 Purpose of Type Traits
  // For example, the expression std::common_type<T1,T2>::type yields int if
  // both arguments are int, long, if one is int and the other is long, or
  // std::string if one is a string and the other is a string literal (type
  // const char*).
  //
  // Again, note that std::common_type<> decays so that the return value can’t
  // become a reference.

  template <typename T1, typename T2>
  typename std::common_type<T1, T2>::type max_06(T1 a, T2 b)
  {
    return b < a ? a : b;
  }

  // "In fact, using true as the condition for operator?: in the declaration is
  // enough:". Does it mean it ALWAYS picks up type of a?

  template <typename T1, typename T2>
  auto which_type(T1 a, T2 b) -> decltype(true ? a : b)
  {
    return a ? a : b;
  }
} // namespace cxx_template_return_type

// ={=========================================================================
TEST(Template, ReturnType)
{
  using namespace cxx_template_return_type;

  {
    vector<int> coll{3, 4, 5, 6};

    EXPECT_THAT(return_element_01(coll.begin(), coll.end()), 3);
    EXPECT_THAT(return_element_02(coll.begin(), coll.end()), 3);
    EXPECT_THAT(return_element_03(coll.begin(), coll.end()), 3);
  }

  {
    max_01(4, 5);

    // cxx.cpp:4622:16: error: no matching function for call to ‘max_01(int,
    // double)’
    //    max_01(4, 7.2);
    //                 ^
    // cxx.cpp:4622:16: note: candidate is:
    // cxx.cpp:4591:7: note: template<class T> T
    // cxx_template_return_type::max_01(const T&, const T&)
    //      T max_01(T const& a, T const& b)
    //        ^
    // cxx.cpp:4591:7: note:   template argument deduction/substitution failed:
    // cxx.cpp:4622:16: note:   deduced conflicting types for parameter ‘const
    // T’ (‘int’ and ‘double’)
    //    max_01(4, 7.2);
    //                 ^
    // max_01(4, 7.2);
  }
  {
    auto result = max_02(4, 7.2);
    EXPECT_THAT(typeid(result).name(), typeid(double).name());
  }

  // WHY NOT the result type is int type?
  // 1.3. C++ Templates The Complete Guide Second Edition
  // but generally produce an intuitively expected result (e.g., if a and b have
  // different arithmetic types, a common arithmetic type is found for the
  // result).

  {
    auto result = max_02(8, 7.2);
    EXPECT_THAT(typeid(result).name(), typeid(double).name());
  }
  {
    auto result = max_03(8, 7.2);
    EXPECT_THAT(typeid(result).name(), typeid(double).name());
  }
  {
    auto result = max_04(8, 7.2);
    EXPECT_THAT(typeid(result).name(), typeid(double).name());
  }

  // {
  //   std::string s{"string"};
  //   auto result = which_type(s, 0);
  //   EXPECT_THAT(typeid(result).name(), typeid(std::string).name());
  // }
}

namespace cxx_template_reference
{
  template <typename T>
  void foo(T value)
  {
    ++value;
  }
} // namespace cxx_template_reference

// ={=========================================================================
TEST(Template, Reference)
{
  using namespace cxx_template_reference;

  int value{10};

  foo(value);
  EXPECT_THAT(value, 10);

  foo(std::ref(value));
  EXPECT_THAT(value, 11);
}

namespace cxx_template_overload
{
  class Mine
  {
  private:
    string name;

  public:
    Mine()
        : name(string("mine"))
    { // cout << "mine class" << endl;
    }

    const string &get() { return name; }
  };

  ostringstream &operator<<(ostringstream &os, Mine mine)
  {
    os << mine.get();
    return os;
  }

  template <typename T>
  string debug_rep(const T &t)
  {
    ostringstream ret;
    ret << t;
    return ret.str();
  }
} // namespace cxx_template_overload

// ={=========================================================================
TEST(Template, Overload)
{
  using namespace cxx_template_overload;

  Mine mine;
  ostringstream os;

  // string str("this is string");
  // cout << debug_rep(str) << endl;

  os << debug_rep(mine) << endl;
  EXPECT_THAT(os.str(), "mine\n");
}

namespace cxx_template_friend
{
  // basics/stack1.hpp

  template <typename T>
  class Stack
  {
  public:
    void push(T const &elem) { elems_.push_back(elem); }

    void pop()
    {
      if (elems_.empty())
        throw std::runtime_error("coll is empty");

      elems_.pop_back();
    }

    T const &top() const
    {
      if (elems_.empty())
        throw std::runtime_error("coll is empty");

      return elems_.back();
    }

    bool empty() const { return elems_.empty(); }

  private:
    std::vector<T> elems_;
  };

  // To show how to define operator<< for template *cxx-overload-operator*
  //
  // C++ Templates The Complete Guide Second Edition
  // 2.4 Friends
  //
  // However, when trying to declare the friend function and define it
  // afterwards, things become more complicated. In fact, we have two options:
  // 1. We can implicitly declare a new function template, which must use a
  // different template parameter, such as U:

  template <typename T>
  ostream &operator<<(ostream &os, Stack<T> const &s)
  {
    cout << "stack's top : " << s.top() << endl;
    return os;
  }

} // namespace cxx_template_friend

// ={=========================================================================
TEST(Template, Friend)
{
  using namespace cxx_template_friend;

  Stack<int> si;

  si.push(1);
  si.push(2);
  si.push(3);
  EXPECT_THAT(si.top(), 3);

  cout << si;
}

// ={=========================================================================
// cxx-template-forward

// from https://modoocode.com/228

namespace templateforward
{
  // here wrapper() pass args to g(). case example is coll.emplace_back() which
  // calls T's ctor with given args.
  //
  // the issue is that emplace_back() must pass right type of args to call right
  // ctor.

  template <typename T>
  void wrapper1(T u)
  {
    g(u);
  }

  class F
  {};

  // overloads
  void g(F &a) { std::cout << "g(lvalue reference)" << std::endl; }
  void g(const F &a) { std::cout << "g(const lvalue reference)" << std::endl; }
  void g(F &&a) { std::cout << "g(rvalue reference)" << std::endl; }
} // namespace templateforward

// [ RUN      ] CxxTemplate.check_forward_1
// - use g       -----
// g(lvalue reference)
// g(const lvalue reference)
// g(rvalue reference)
// - use wrapper -----
// g(lvalue reference)
// g(lvalue reference)
// g(lvalue reference)
// [       OK ] CxxTemplate.check_forward_1 (0 ms)
//
// WHY "g(lvalue reference)" for all cases when use wrapper? Because, if T is
// not reference type when deducing T, ignore "const" so all is deduced as
// "Class F"
//
//
// 1. Ok, what if specify "reference"?
//
// template <typename T>
// void wrapper(T &u)
// {
//   g(u);
// }
//
// then bind error on:
//
// wrapper(F());
//
// since "const F &" -> "F &" in wrapper(T &) is error.
//
//
// 2. Ok, what if have more overloads?
//
// template <typename T>
// void wrapper(T &u)
// {
//   g(u);
// }
//
// template <typename T>
// void wrapper(const T &u)
// {
//   g(u);
// }
//
// wrapper(a);      -> T &      , lvalue reference
// wrapper(ca);     -> const T &, const lvalue reference
// wrapper(F());    -> const T &, const lvalue reference
//
// The problem is that usual reference, "T &" can take, can be deduced to,
// rvalue reference.

// ={=========================================================================
TEST(CxxTemplate, check_forward_1)
{
  using namespace templateforward;

  F a;
  const F ca;

  std::cout << "- use g       -----\n";
  g(a);
  g(ca);
  g(F());

  std::cout << "- use wrapper -----\n";
  wrapper1(a);
  wrapper1(ca);
  wrapper1(F());
}

namespace templateforward
{
  template <typename T>
  void wrapper2(T &&u)
  {
    g(std::forward<T>(u));
  }
} // namespace templateforward

// [ RUN      ] CxxTemplate.check_forward_2
// - use g       -----
// g(lvalue reference)
// g(const lvalue reference)
// g(rvalue reference)
// - use wrapper -----
// g(lvalue reference)
// g(const lvalue reference)
// g(rvalue reference)
// [       OK ] CxxTemplate.check_forward_2 (0 ms)
//
//
// "void wrapper2(T &&u)" is called "universal reference" only in template
// context and accpets both reference
//
// this is different from "void show_value(int&& t);" which only takes rvalue
// reference.
//
// HOW come universal reference work? use "reference collapsing rule"
//
// typedef int& T;
// T& r1;   // int& &;    r1 is int&
// T&& r2;  // int & &&;  r2 is int&

// typedef int&& U;
// U& r3;   // int && &;  r3 is int&
// U&& r4;  // int && &&; r4 is int&&
//
// that is, for simple, think & is 1 and && is 0 and do OR them.
//
// wrapper(F()); T is F.
//
// now, the problem is how to pass args to g(). WHY not use g(u)?
//
// {
//   template <typename T>
//   void wrapper2(T &&u)
//   {
//     g(u);
//   }
// }
//
// "u" is lvalue and have to use std::move() to call g(&&). However, cannot do
// it blindly. How do it only when u is rvalue?
//
// std::forward() do this.
//
// template <class S>
// S&& forward(typename std::remove_reference<S>::type& a) noexcept {
//   return static_cast<S&&>(a);
// }
//
// when S is A&:
//
// A&&& forward(typename std::remove_reference<A&>::type& a) noexcept {
//    return static_cast<A&&&>(a);
// }
//
// reference collapse applies
//
// A& forward(A& a) noexcept { return static_cast<A&>(a); }
//
// when S is A:
//
// A&& forward(A& a) noexcept { return static_cast<A>(a); }
//
// so reutrn rvalue reference.
//
// NOTE: not fully understand but it is enough to say that std::forward return
// rvalue when template T is rvalue.

// ={=========================================================================
TEST(CxxTemplate, check_forward_2)
{
  using namespace templateforward;

  F a;
  const F ca;

  std::cout << "- use g       -----\n";
  g(a);
  g(ca);
  g(F());

  std::cout << "- use wrapper -----\n";
  wrapper2(a);
  wrapper2(ca);
  wrapper2(F());
}

namespace cxx_template
{
  template <typename F, typename T1, typename T2>
  void flip(F f, T1 param1, T2 param2)
  {
    f(param2, param1);
  }

  // use *cxx-rvalue-reference* to preserve template parameter:
  //
  // A function parameter that is an rvalue reference to a template parameter
  // `preserves` constness and lvalue/rvalue property of its corresponding
  // argument.

  template <typename F, typename T1, typename T2>
  void flip_reference(F f, T1 &&param1, T2 &&param2)
  {
    f(param2, param1);
  }

  void f(int value1, int &value2)
  {
    (void)value1;
    ++value2;
  }

  void g(int &&value1, int &value2)
  {
    (void)value1;
    ++value2;
  }

  // Use `std::forward()` to preserve the types of the original arguments.
  // Unlike std::move(), std::forward() 'must' be called with an explicit
  // template argument, *cxx-template-explicit-argument* and returns rvalue
  // reference to that type, T &&.
  //
  // template <typename F, typename T1, typename T2>
  // void flip(F f, T1 &&t1, T2 &&t2)                    // 1
  // {
  //   f(std::forward<T2>(t2), std::forward<T1>(t1));    // 2
  // }
  //
  // To preserve type information is two-step process, 1 and 2.
  //
  // *cxx-ref*
  // What's the difference between std::ref() and std::forward() solution?
  // By std::forward() solution, user of template don't need to anyting.

  template <typename F, typename T1, typename T2>
  void flip_forward(F f, T1 &&param1, T2 &&param2)
  {
    f(std::forward<T2>(param2), std::forward<T1>(param1));
  }

} // namespace cxx_template

// ={=========================================================================
TEST(CxxTemplate, check_forward)
{
  using namespace cxx_template;

  {
    int i = 10;
    int j = 20;

    f(i, j);

    EXPECT_THAT(i, 10);
    EXPECT_THAT(j, 21); // changed
  }

  {
    int i = 10;
    int j = 20;

    flip(f, i, j);

    // both are not changed since flip() uses local copy and f() changes that

    EXPECT_THAT(i, 10);
    EXPECT_THAT(j, 20);
  }

  {
    int i = 10;
    int j = 20;

    flip(f, std::ref(i), j);

    // change since passed reference

    EXPECT_THAT(i, 11); // changed
    EXPECT_THAT(j, 20);
  }

  {
    int i = 10;
    int j = 20;

    flip_reference(f, i, j);

    EXPECT_THAT(i, 11); // changed
    EXPECT_THAT(j, 20);
  }

  // Still flip_reference() solves half of the problem since cannot be used to
  // call a function that has an rvalue parameter.
  //
  // flip_reference() pass `preserved` parameter down to g and which are int.
  // They binds to int&& adn this is error.
  //
  // WHY not reference-collapsing? Because g() is not template.
  //
  // {
  //   int i = 10;
  //   int j = 20;
  //
  //   // cxx.cpp:5315:27:   required from here
  //   // cxx.cpp:5240:19: error: cannot bind ‘int’ lvalue to ‘int&&’
  //   //    f(param2, param1);
  //   //                    ^
  //
  //   flip_reference(g, i, j);
  //
  //   EXPECT_THAT(i, 11);   // changed
  //   EXPECT_THAT(j, 20);
  // }

  {
    int i = 10;
    int j = 20;

    flip_forward(f, i, j);

    EXPECT_THAT(i, 11); // changed
    EXPECT_THAT(j, 20);
  }
}

// *cxx-rvalue-reference* *cxx-forward*
//
// From CPP Challenge, 56. Select algorithm
//
// Write a function that, given a range of values and a projection function,
// transforms each value into a new one and returns a new range with the
// selected values. For instance, if you have a type book that has an id, title,
// and author, and have a range of such book values, it should be possible for
// the function to select only the title of the books. Here is an example of how
// the function should be used:
//
// *cxx-result-of*
//
// std:: result_of <T, Args...>::value
//
// o Yields the return type of the callable T called for Args...
//
// Since C++11, to get the return type you could call: typename
// std::result_of<Callable(Args...)>::type
//
// typename std::decay<typename std::result_of<typename
// std::decay<F>::type&(typename std::vector<T>::const_reference)>::type>::type
//                                            { }
//                    { }
// after all,
// 1. "typename std::decay<F>::type&(vector<T>::const_reference)" gets the
// address of function that takes const reference
// 2. result_of<>::type gets return type
// 3. decay<>::type gets decayed type

namespace cxx_template
{
  struct book
  {
    int id;
    string title;
    string author;
  };

  template <typename T,
            typename F,
            typename R = typename std::decay<
              typename std::result_of<typename std::decay<F>::type &(
                typename std::vector<T>::const_reference)>::type>::type>
  std::vector<R> select(std::vector<T> const &coll, F &&f)
  {
    std::vector<R> result{};
    std::transform(coll.cbegin(),
                   coll.cend(),
                   std::back_inserter(result),
                   std::forward<F>(f));
    return result;
  }

} // namespace cxx_template

// ={=========================================================================
TEST(CxxTemplate, forward_1)
{
  using namespace cxx_template;

  std::vector<book> books{
    {101, "The C++ Programming Language", "Bjarne Stroustrup"},
    {203, "Effective Modern C++", "Scott Meyers"},
    {404, "The Modern C++ Programming Cookbook", "Marius Bancila"}};

  auto titles = select(books, [](book const &b) { return b.title; });

  EXPECT_THAT(titles,
              ElementsAre("The C++ Programming Language",
                          "Effective Modern C++",
                          "The Modern C++ Programming Cookbook"));

  auto authors = select(books, [](book const &b) { return b.author; });

  EXPECT_THAT(
    authors,
    ElementsAre("Bjarne Stroustrup", "Scott Meyers", "Marius Bancila"));

  auto ids = select(books, [](book const &b) { return b.id; });

  EXPECT_THAT(ids, ElementsAre(101, 203, 404));
}

// ={=========================================================================
// cxx-template-variadic

namespace cxx_template_variadic
{
  template <typename T, typename... Args>
  std::pair<int, int> foo(T const &t, Args const &... args)
  {
    (void)t;
    return make_pair(
      // number of template parameters
      sizeof...(Args),
      // number of function parameters
      sizeof...(args));
  }
} // namespace cxx_template_variadic

TEST(CxxTemplate, variadic_sizeof_operator)
{
  using namespace cxx_template_variadic;

  int i         = 0;
  double d      = 3.14;
  std::string s = "how now brown cow";

  // three parameters in the pack
  EXPECT_THAT(foo(i, s, 42, d), make_pair(3, 3));

  // two parameters in the pack
  EXPECT_THAT(foo(s, 42, "hi"), make_pair(2, 2));

  // one parameters in the pack
  EXPECT_THAT(foo(d, s), make_pair(1, 1));

  // empty pack
  EXPECT_THAT(foo("hi"), make_pair(0, 0));
}

// ={=========================================================================
// cxx-template-type-trait cxx-type-trait

namespace cxx_type_trait
{
  template <typename T>
  bool foo(T const &val)
  {
    (void)val;

    // is_pointer<T> yields either a `true_type` ro `false_type`, for which
    // ::value either yields true or false.

    if (std::is_pointer<T>::value)
    {
      // std::cout << "foo() called for a pointer" << std::endl;
      return true;
    }
    else
    {
      return false;
    }
  }

  template <typename T>
  void foo_impl(T value, std::true_type)
  {
    (void)value;
    std::cout << "foo() called for integral type" << std::endl;
  }

  template <typename T>
  void foo_impl(T value, std::false_type)
  {
    (void)value;
    std::cout << "foo() called for floating type" << std::endl;
  }

  template <typename T>
  void foo_overload(T value)
  {
    foo_impl(value, std::is_integral<T>());
  }

} // namespace cxx_type_trait

// ={=========================================================================
// [ RUN      ] Template.TypeTrait
// foo() called for integral type
// foo() called for floating type
// [       OK ] Template.TypeTrait (1 ms)

TEST(CxxTemplate, type_trait)
{
  using namespace cxx_type_trait;

  {
    int *pint;

    EXPECT_THAT(foo(pint), true);
  }

  {
    int ivalue{10};
    double dvalue{10.0};

    foo_overload(ivalue);
    foo_overload(dvalue);
  }
}

namespace cxx_template
{
  std::unordered_map<std::string, int> score1{
    {"andy", 7}, {"al", 9}, {"bill", -3}, {"barbara", 12}};

  // same type X, Y as above
  std::map<std::string, int> score2{
    {"andy", 7}, {"al", 9}, {"bill", -3}, {"barbara", 12}};

  // different types. operator<<() still works
  std::map<std::string, std::string> score3{
    {"andy", "A7"}, {"al", "A9"}, {"bill", "-A3"}, {"barbara", "A12"}};

  template <typename X, typename Y>
  ostream &operator<<(ostream &os, const std::pair<X, Y> &p)
  {
    return os << '{' << p.first << ',' << p.second << '}';
  }
} // namespace cxx_template

// ={=========================================================================
// RUN      ] CxxTemplate.operator_output
// unordered: {bill,-3}, {barbara,12}, {al,9}, {andy,7},
// ordered: {al,9}, {andy,7}, {barbara,12}, {bill,-3},
// [       OK ] CxxTemplate.operator_output (0 ms)

TEST(CxxTemplate, operator_output)
{
  using namespace cxx_template;

  std::cout << "unordered: ";

  for (const auto &x : score1)
    std::cout << x << ", ";

  std::cout << "\nordered: ";

  for (const auto &x : score2)
    std::cout << x << ", ";

  std::cout << "\nordered: ";

  for (const auto &x : score3)
    std::cout << x << ", ";

  std::cout << '\n';
}

// ={=========================================================================
// cxx-const

TEST(CxxConst, check_reference)
{
  // nonconst -> const
  {
    int i{};
    int &ri       = i;
    const int &rc = i; // non-const to const is fine

    ri = 0;

    // cxx.cpp:3312:6: error: assignment of read-only reference ‘rc’
    // rc = 0;

    // to avoid warning
    (void)rc;
  }

  // const -> nonconst
  {
    const int ci{100};

    // const to non-const
    // cxx-const-error cxx-error: invalid initialization of reference of type
    // ‘int&’ from expression of type ‘const int’

    // int &ri = ci;
    (void)ci;

    // int &ri = (int &)ci;   // no error since used cast
  }

  {
    int nci{100};
    const int *pci = &nci;
    EXPECT_THAT(*pci, 100);

    // cxx-error: invalid conversion from ‘const int*’ to ‘int*’ [-fpermissive]
    // int *pi = pci;
  }
}

namespace cxx_const
{
  class Screen
  {
  public:
    Screen()
        : mesg_("screen")
    {}

    string get_message() const { return mesg_; }
    string get_message() { return mesg_; }

  private:
    string mesg_;
  };

  // *cxx-const-error-const* when there's no const version of get_message()
  // since const on arg "print_screen(const Screen &s)"
  //
  // cxx.cpp: In function ‘std::string cxx_const::print_screen(const
  // cxx_const::Screen&)’:
  //
  // cxx.cpp:3359:26: error: passing ‘const cxx_const::Screen’ as
  // ‘this’ argument of ‘std::string
  // cxx_const::Screen::get_message()’ discards qualifiers
  // [-fpermissive]
  //
  //      return s.get_message();

  string print_screen(const Screen &s) { return s.get_message(); }

  template <class T, size_t R, size_t C>
  class array2d
  {
    std::vector<T> arr;

  public:
    array2d()
        : arr(R * C)
    {}
    explicit array2d(std::initializer_list<T> l)
        : arr(l)
    {}

    // *cxx-const-error-const* when there's no const version
    //
    // when use constexpr function
    //
    //  constexpr T& at(size_t const r, size_t const c)
    //  {
    //     return arr.at(r*C + c);
    //  }
    //
    // cxx.cpp: In instantiation of ‘constexpr T&
    // cxx_const::array2d<T, R, C>::at(size_t, size_t) const [with T
    // = int; long unsigned int R = 2ul; long unsigned int C = 3ul; size_t =
    // long unsigned int]’: cxx.cpp:3420:21:   required from ‘void
    // cxx_const::print_array2d(const
    // cxx_const::array2d<T, R, C>&) [with T = int; long unsigned
    // int R = 2ul; long unsigned int C = 3ul]’ cxx.cpp:3439:20:   required from
    // here cxx.cpp:3403:31: error: invalid initialization of reference of type
    // ‘int&’ from expression of type ‘const value_type {aka const int}’
    //           return arr.at(r*C + c);

    // non-const version
    //
    // *cxx-const-error-const* when there's no const version
    //
    // cxx.cpp:3409:21: error: passing ‘const
    // cxx_const::array2d<int, 2ul, 3ul>’ as ‘this’ argument of ‘T&
    // cxx_const::array2d<T, R, C>::at(size_t, size_t) [with T =
    // int; long unsigned int R = 2ul; long unsigned int C = 3ul; size_t = long
    // unsigned int]’ discards qualifiers
    // [-fpermissive]
    //            std::cout << arr.at(i, j) << ' ';
    //
    // T& at(size_t const r, size_t const c)
    // {
    //    return arr.at(r*C + c);
    // }

    // const version covers both const and non-const use
    constexpr T const &at(size_t const r, size_t const c) const
    {
      return arr.at(r * C + c);
    }
  };

  template <class T, size_t R, size_t C>
  void print_array2d(array2d<T, R, C> const &arr)
  {
    for (size_t i = 0; i < R; ++i)
    {
      for (size_t j = 0; j < C; ++j)
      {
        std::cout << arr.at(i, j) << ' ';
      }

      std::cout << std::endl;
    }
  }

} // namespace cxx_const

// ={=========================================================================
TEST(CxxConst, check_memeber_function)
{
  using namespace cxx_const;
  {
    Screen screen;
    print_screen(screen);
  }

  {
    array2d<int, 2, 3> a;
    print_array2d(a);
  }
}

namespace cxx_const
{
  // copy version
  std::string merge_string_1(const std::vector<std::string> &coll)
  {
    std::string ret{};

    for (auto e : coll)
    {
      ret += e;
    }

    return ret;
  }

  // no copy and reference
  std::string merge_string_2(const std::vector<std::string> &coll)
  {
    std::string ret{};

    for (auto &e : coll)
    {
      ret += e;
    }

    return ret;
  }

  // can change? no
  //
  // : error: no matching function for call to ‘std::__cxx11::basic_string<char>::append(const char [2]) const’
  //        e.append("3");
  //
  // std::string merge_string_3(const std::vector<std::string> &coll)
  // {
  //   std::string ret{};
  //
  //   for (auto &e : coll)
  //   {
  //     e.append("3");
  //     ret += e;
  //   }
  //
  //   return ret;
  // }

  // to change, make it non-const
  std::string merge_string_4(std::vector<std::string> &coll)
  {
    std::string ret{};

    for (auto &e : coll)
    {
      e.append("3");
      ret += e;
    }

    return ret;
  }
} // namespace cxx_const

// ={=========================================================================
TEST(CxxConst, range_for)
{
  using namespace cxx_const;

  std::vector<std::string> coll{"const", "range", "for"};

  merge_string_1(coll);
  merge_string_2(coll);
  merge_string_4(coll);
}

// Q: useful case?

namespace cxxconstexpr
{
  class Vector
  {
  private:
    int x_;
    int y_;

  public:
    constexpr Vector(int x, int y)
        : x_(x)
        , y_(y)
    {}

    constexpr int x() const { return x_; }
    constexpr int y() const { return y_; }
  };

  constexpr Vector AddVec(const Vector &v1, const Vector &v2)
  {
    return {v1.x() + v2.x(), v1.y() + v2.y()};
  }

  template <int N>
  struct Foo
  {
    int operator()() { return N; }
  };
} // namespace cxxconstexpr

// ={=========================================================================
TEST(CxxConstExpr, check_constexpr_ctor)
{
  using namespace cxxconstexpr;

  constexpr Vector v1{1, 2};
  constexpr Vector v2{2, 3};

  Foo<v1.x()> f1;
  EXPECT_THAT(f1(), 1);

  Foo<AddVec(v1, v2).x()> f2;
  EXPECT_THAT(f2(), 3);
}

// ={=========================================================================
// cxx-auto

TEST(CxxAuto, auto_const)
{
  // reference type and can change
  {
    std::vector<int> coll1{1, 2, 3, 4, 5};

    for (auto &e : coll1)
      e += 10;

    EXPECT_THAT(coll1, ElementsAre(11, 12, 13, 14, 15));
  }

  // error: assignment of read-only reference ‘e’
  //
  // {
  //   std::vector<int> coll1{1, 2, 3, 4, 5};
  //
  //   for (const auto &e : coll1)
  //     e += 10;
  //
  //   EXPECT_THAT(coll1, ElementsAre(11, 12, 13, 14, 15));
  // }

  // same
  // error: assignment of read-only reference ‘e’
  //
  // {
  //   std::vector<int> coll1{1, 2, 3, 4, 5};
  //
  //   for (auto const &e : coll1)
  //     e += 10;
  //
  //   EXPECT_THAT(coll1, ElementsAre(11, 12, 13, 14, 15));
  // }

  // NOTE: same as "const int" and note that there is no "const reference" but
  // there is only "reference to const". see *cxx-declarator*
  //
  // error: assignment of read-only reference ‘e’
  //
  // {
  //   std::vector<int> coll1{1, 2, 3, 4, 5};
  //
  //   for (const int &e : coll1)
  //     e += 10;
  //
  //   EXPECT_THAT(coll1, ElementsAre(11, 12, 13, 14, 15));
  // }

  // okay
  {
    std::vector<int> coll1{1, 2, 3, 4, 5};
    std::vector<int> coll2;

    for (auto it = coll1.begin(); it != coll1.end(); ++it)
    {
      coll2.push_back(*it);
    }

    EXPECT_THAT(coll2, ElementsAre(1, 2, 3, 4, 5));
  }

  // error: passing ‘const __gnu_cxx::__normal_iterator<const int*, std::vector<int> >’
  //  as ‘this’ argument discards qualifiers [-fpermissive]
  //
  //      for (const auto it = coll1.cbegin(); it != coll1.cend(); ++it)
  //                                                                 ^~
  //
  // {
  //   std::vector<int> coll1{1, 2, 3, 4, 5};
  //   std::vector<int> coll2;
  //
  //   for (const auto it = coll1.cbegin(); it != coll1.cend(); ++it)
  //   {
  //     coll2.push_back(*it);
  //   }
  //
  //   EXPECT_THAT(coll2, ElementsAre(1, 2, 3, 4, 5));
  // }

  // okay
  {
    std::vector<int> coll1{1, 2, 3, 4, 5};
    std::vector<int> coll2;

    for (std::vector<int>::const_iterator it = coll1.cbegin();
         it != coll1.cend();
         ++it)
    {
      coll2.push_back(*it);
    }

    EXPECT_THAT(coll2, ElementsAre(1, 2, 3, 4, 5));
  }

  // same error
  // {
  //   std::vector<int> coll1{1, 2, 3, 4, 5};
  //   std::vector<int> coll2;
  //
  //   for (const std::vector<int>::const_iterator it = coll1.cbegin();
  //        it != coll1.cend();
  //        ++it)
  //   {
  //     coll2.push_back(*it);
  //   }
  //
  //   EXPECT_THAT(coll2, ElementsAre(1, 2, 3, 4, 5));
  // }

  // okay, can change
  {
    std::vector<int> coll1{1, 2, 3, 4, 5};

    for (auto it = coll1.begin(); it != coll1.end(); ++it)
    {
      *it += 10;
    }

    EXPECT_THAT(coll1, ElementsAre(11, 12, 13, 14, 15));
  }

  // const_iterator is like "const T *", so *it is const and cannot change.
  //
  // see *cxx-const-on-iterator*
  //
  // error: assignment of read-only location ‘it.__gnu_cxx::__normal_iterator<_Iterator, _Container>::operator*<const int*, std::vector<int> >()’
  //       *it += 10;
  //              ^~
  //
  // {
  //   std::vector<int> coll1{1, 2, 3, 4, 5};
  //
  //   for (auto it = coll1.cbegin(); it != coll1.cend(); ++it)
  //   {
  //     *it += 10;
  //   }
  //
  //   EXPECT_THAT(coll1, ElementsAre(11, 12, 13, 14, 15));
  // }
}

// ={=========================================================================
// cxx-except

namespace cxx_except
{
  class my_exception : public std::exception
  {
    virtual const char *what() const throw() { return "my exception happened"; }
  };

  my_exception myex;

  void throw_exception() { throw myex; }

  struct Error : std::runtime_error
  {
    Error(const std::string &p)
        : runtime_error(p)
    {}
  };
} // namespace cxx_except

// ={=========================================================================
TEST(CxxException, drived_from_exception)
{
  using namespace cxx_except;

  try
  {
    throw myex;
  } catch (std::exception &e)
  {
    // ? why not error here?
    EXPECT_THAT(e.what(), "my exception happened");
  }

  try
  {
    throw myex;
  } catch (std::exception &e)
  {
    ostringstream os;
    os << "my exception happened";
    EXPECT_THAT(os.str(), "my exception happened");
  }

  try
  {
    throw myex;
  } catch (...)
  {
    ostringstream os;
    os << "my exception happened";
    EXPECT_THAT(os.str(), "my exception happened");
  }

  try
  {
    throw myex;
  } catch (...)
  {}
}

// ={=========================================================================
// see can driver from std::runtime_error
TEST(CxxException, drived_from_stdexcept)
{
  using namespace cxx_except;

  try
  {
    throw Error("this is runtime error");
  } catch (std::exception &e)
  {
    EXPECT_THAT(std::string(e.what()), "this is runtime error");
  }
}

// ={=========================================================================
//  see what happends when catch it but ignore it
TEST(CxxException, see_when_do_catch)
{
  using namespace cxx_except;

  try
  {
    throw_exception();
  } catch (...)
  {}
}

// ={=========================================================================
//  see what happends when do not catch
// [ RUN      ] CxxException.see_when_do_not_catch
// unknown file: Failure
// C++ exception with description "my exception happened" thrown in the test body.
// [  FAILED  ] CxxException.see_when_do_not_catch (0 ms)

TEST(CxxException, DISABLED_see_when_do_not_catch)
{
  using namespace cxx_except;

  throw_exception();
}

namespace cxx_except
{
  class Foo
  {
  public:
    ~Foo() {}
  };

  class FooNoAbort
  {
  public:
    ~FooNoAbort() noexcept(false)
    {
      throw std::runtime_error("noexcept(false) so expects no abort");
    }
  };

  // class FooAbort
  // {
  // public:
  //
  //   // complile warning
  //   // warning: throw will always call terminate() [-Wterminate]
  //   //        throw std::runtime_error("noexcept(true) so expects abort");
  //   //                                                                  ^
  //   // note: in C++11 destructors default to noexcept
  //
  //   ~FooAbort() noexcept(true)
  //   {
  //     throw std::runtime_error("noexcept(true) so expects abort");
  //   }
  // };
} // namespace cxx_except

/*
// ={=========================================================================

CLR 3.1.7 Keyword noexcept

Here, inside noexcept(...), you can specify a boolean condition under which no
exception gets thrown: Specifying noexcept without condition is a short form of
specifying `noexcept(true)` which means exception is not allowed

~TemplateDispatcher()
{}

is same as:

~TemplateDispatcher() noexcept(true)
{}

*/

TEST(CxxException, noexcept_condition_1)
{
  using namespace cxx_except;

  {
    int value{1};

    Foo foo;

    EXPECT_THAT(value, 1);
  }

  // FooNoAbort dtor is allowed to throw so no abort happens
  {
    int value{1};

    try
    {
      FooNoAbort foo;
    } catch (std::exception &e)
    {
      // fails since what() returns "const char*"
      // EXPECT_THAT(e.what(), "noexcept(false) so expects no abort");

      EXPECT_THAT(std::string(e.what()), "noexcept(false) so expects no abort");
    }

    EXPECT_THAT(value, 1);
  }
}

// ={=========================================================================
// "noexcept(true)" but throw so violates promise. abort happens.
//
// [ RUN      ] Exception.Noexcept
// terminate called after throwing an instance of 'std::runtime_error'
//   what():  noexcept(true) so expects abort
// Aborted
//
// DISABLED but still compile it. so comment out
// TEST(CxxException, DISABLED_noexcept_condition_2)
// {
//   using namespace cxx_except;

//   {
//     int value{1};

//     try
//     {
//       FooAbort foo;
//     } catch (exception &e)
//     {
//       std::cout << e.what() << std::endl;
//     }

//     EXPECT_THAT(value, 1);
//   }
// }

// ={=========================================================================
// cxx-printf

TEST(CxxPrintf, printf_format)
{
  // The flag characters
  // The character % is followed by zero or more of the following flags:
  // `#`
  //
  // The value should be converted to an "alternate form".
  //
  // For x and X conversions, a non-zero result has the string "0x" (or "0X" for
  // X conversions) prepended to it.

  // value: 64
  // value: 0x64
  // value: 0x64
  {
    int value{100};
    printf("value: %x\n", value);
    printf("value: %#x\n", value);
    printf("value: 0x%x\n", value);
  }

  // The field width
  // An  optional  decimal  digit  string  (with nonzero first digit) specifying
  // a minimum field width.  If the converted value has fewer characters than
  // the field width, it will be padded with spaces on the left (or right, if
  // the left-adjustment flag has been given).
  //
  // Instead of a decimal digit string one may write "*" or "*m$" (for some
  // decimal integer m) to specify that the field width is given in the next
  // argument, or in the m-th argument, respec‐ tively, which must be of type
  // int.

  // value: {100}
  // value: {       100}
  {
    int value{100};
    printf("value: {%d}\n", value);
    printf("value: {%10d}\n", value);
  }

  // MHEGDebugDebug(eMHEGengRuntime,"OS-OctetStringVariable Append %.*s, %.*s ->
  // %.*s\n",
  //     (OctetStringVariableEntry(pEntry).m_osValue).size,
  //     (OctetStringVariableEntry(pEntry).m_osValue).buf,
  //     osValue.size, osValue.buf,
  //     osNewValue.size, osNewValue.buf);
  //
  // 0: 01
  // 0: 012
  // 0: 0123
  // 0: 01234
  // 0: 012345
  {
    char pmesg[] = "0123456789";

    printf("0: %.*s \n", 2, pmesg);
    printf("0: %.*s \n", 3, pmesg);
    printf("0: %.*s \n", 4, pmesg);
    printf("0: %.*s \n", 5, pmesg);
    printf("0: %.*s \n", 6, pmesg);
  }

  // The precision
  // An  optional precision, in the form of a period ('.') followed by an
  // optional decimal digit string.
  //
  // value: {100.000000}
  // value: {100.0}
  // value: {100.0}
  // value: {100.0}
  // value: {100.0}
  // value: {100.00}
  // value: {100.00}
  {
    float value{100.0};
    printf("value: {%f}\n", value);
    printf("value: {%0.01f}\n", value);
    printf("value: {%.01f}\n", value);
    printf("value: {%0.1f}\n", value);
    printf("value: {%.1f}\n", value);
    printf("value: {%0.2f}\n", value);
    printf("value: {%.2f}\n", value);
  }

  // The format argument of printf can be an expression too.
  {
    // printf((argc>1) ? "%s " : "%s", *++argv );
    int argc = 1;
    printf((argc > 1) ? "|%s|\n" : "%s\n", "argc");
  }

  // print unsigned long value
  //
  // ulong max from limits: 18446744073709551615L
  // ulong max from limits: 18446744073709551615
  {
    // here "L" is normal char
    printf("ulong max from limits: %luL\n", ULONG_MAX);
    printf("ulong max from limits: %lu\n", ULONG_MAX);
  }

  {
    int value{10};
    printf("this is first fmt"
           " this is second fmt %d\n",
           value);
  }

  // cxx-scanf
  // RETURN VALUE
  // On success, these functions return the number of input items successfully
  // matched and assigned; this can be fewer than provided for, or even zero, in
  // the event of an early matching failure.

  // The length modifier "hh"
  {
    uint8_t bytes[3];
    if (!sscanf("1C:A2:B1",
                "%02hhx:%02hhx:%02hhx",
                &bytes[0],
                &bytes[1],
                &bytes[2]))
    {
      EXPECT_THAT(true, false);
    }

    // see that it gets values in reverse order
    EXPECT_THAT(bytes[0], 0x1C);
    EXPECT_THAT(bytes[1], 0xA2);
    EXPECT_THAT(bytes[2], 0xB1);
  }
}

/*
={=========================================================================
cxx-assert

The usage of static_assert is quite easy. static_assert requires an expression
and a string. 

static_assert(expression, string);

The expression has to be a predicate that can be evaluated at compile time.
Predicate means the expression returns true or false. If the expression
evaluates to false you will get at compile time an error message with the string
as message. Of course you get no executable.


You can use static_assert expressions in all parts of your program. Therefore,
It's a good idea to put general requirements on your source code in a separate
header. The result is, the static_assert expression will be automatically
verified at compile time if you include the header. 

note that can use it in global scope.

// static_assert.cpp

#include <string>

static_assert(sizeof(void*) == 4 ,"32-bit adressing is required!");
static_assert(sizeof(void*) >= 8 ,"64-bit adressing is required!");

template < typename T, int Line, int Column >
struct Matrix{
   static_assert(Line >= 0, "Line number must be positive.");
   static_assert(Column >= 0, "Column number must be positive.");
   static_assert( Line + Column > 0, "Line and Column must be greater than 0.");
};

int main() {
  
  static_assert(sizeof(int) == sizeof(long int),"int and long int must be of the same length.");
  
  Matrix<int,10,5> intArray;
  Matrix<std::string,3,4> strArray;
  Matrix<double,0,1> doubleArray;
  Matrix<long long,1,0> longArray;
  
  Matrix<char,0,0> charArray;

}

The program uses static_assert in the global scope (line 5 and 6); it uses it in
the class scope (line 10 - line 12); it uses it in the local scope (line 17).
One of the two assertions in line 5 and 6 must inevitably fall. The assertions
in the class definition guarantees on one hand that the numbers of columns and
lines must be greater than 0 and it guarantees on the other hand that the sum
has to be positive.That's the reason why the template instantiation in line 14
is not valid. On my computer int is smaller than long int. The reverse relation
is guaranteed by the C++ standard.


cxx_ex.cpp: In instantiation of ‘class Vector<short int, 2>’:
cxx_ex.cpp:39:19:   required from here
cxx_ex.cpp:31:2: error: static assertion failed: Vector size is too small!
  static_assert(Size > 3, "Vector size is too small!");
  ^
*/

namespace cxx_assert
{
  template <class T, int Size>
  class Vector
  {
    // Compile time assertion to check if the size of the vector is greater
    // than 3 or not. If any vector is declared whose size is less than 4, the
    // assertion will fail

    // NOTE: no "std::". otherwise, compile error.
    // std::static_assert(Size > 3, "Vector size is too small!");

    static_assert(Size > 3, "Vector size is too small!");

    T m_values[Size];
  };
} // namespace cxx_assert

TEST(CxxAssert, check_static_assert)
{
  using namespace cxx_assert;

  Vector<int, 4> four; // This will work
  (void)four;

  // Vector<short, 2> two; // This will fail
}

// ={=========================================================================
// cxx-numeric cxx-abs

TEST(Numeric, Abs)
{
  EXPECT_THAT(abs(-121), 121);

  {
    // abs() do not work and return input value
    // since -2**31, -2147483648, is min value and if abs it, it is bigger than
    // max, 2**31-1, so overflows and wrong result
    int value = abs(-2147483648);
    EXPECT_THAT(value, -2147483648);
  }

  {
    // abs() works
    long long value = abs(-2147483648);
    EXPECT_THAT(value, 2147483648);
  }
}

// ={=========================================================================
// cxx-typedef

namespace cxx_typedef
{
  // defined here since cannot do in the block since:
  //
  // cxx.cpp:7062:3: error: a template declaration cannot appear at block scope

  template <typename Value>
  using mmap = map<unsigned int, Value>;

} // namespace cxx_typedef

TEST(Typedef, Alias)
{
  {
    // cannot be a const map since operator[] is for non-const.

    map<unsigned int, string> coll{{1, "one"},
                                   {2, "two"},
                                   {3, "three"},
                                   {4, "four"}};

    coll[3] = "threee";
    coll[3] = "threeee";
    coll[3] = "threeeee";
    coll[3] = "threeeeee";

    ASSERT_THAT(coll[3], Eq("threeeeee"));
  }

  {
    typedef map<unsigned int, string> mmap;

    mmap coll{{1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}};

    coll[3] = "threee";
    coll[3] = "threeee";
    coll[3] = "threeeee";
    coll[3] = "threeeeee";

    ASSERT_THAT(coll[3], Eq("threeeeee"));
  }

  // this is cxx-type-alias

  {
    using mmap = map<unsigned int, string>;

    mmap coll{{1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}};

    coll[3] = "threee";
    coll[3] = "threeee";
    coll[3] = "threeeee";
    coll[3] = "threeeeee";

    ASSERT_THAT(coll[3], Eq("threeeeee"));
  }

  // this is *cxx-template-alias*
  // The aliasing mechanism can be used to define a new template by binding some
  // or all template arguments.

  {
    using namespace cxx_typedef;

    mmap<std::string> coll{{1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}};

    coll[3] = "threee";
    coll[3] = "threeee";
    coll[3] = "threeeee";
    coll[3] = "threeeeee";

    ASSERT_THAT(coll[3], Eq("threeeeee"));
  }
}

/* ={=========================================================================
cxx-class
*/

namespace cxx_class
{
  class ClassA
  {
  public:
    ClassA()
        : name_("ClassA")
    {
      std::cout << "This is ClassA" << std::endl;
    }

    void getName() { std::cout << "ClassA's name : " << name_ << std::endl; }

  private:
    std::string name_;
  };

  class ClassB
  {
  public:
    // here see ";" at the end
    ClassB()
        : name_("ClassB")
    {
      std::cout << "This is ClassB" << std::endl;
    };

    void getName() { std::cout << "ClassA's name : " << name_ << std::endl; };

  private:
    std::string name_;
  };

} // namespace cxx_class

// extra semi-colon do not matter
TEST(CxxClass, check_extra_colon_at_end)
{
  using namespace cxx_class;

  ClassA cao;
  cao.getName();

  ClassB cbo;
  cbo.getName();
}

namespace cxx_nested_1
{
  class Outer
  {
  private:
    int outer_x = 100;

  public:
    void test()
    {
      Inner *inner = new Inner();
      inner->display();
    }

  private:
    class Inner
    {

    public:
      void display()
      {
        // works for java
        // have access to enclosing scope? No
        // cxx.cpp:8397:47: error: invalid use of non-static data member
        // ‘cxx_nested::Outer::outer_x’ std::cout << "display: outer_x = " <<
        // outer_x;
        std::cout << "display: outer_x = " << std::endl;
      }
    };
  };
} // namespace cxx_nested_1

TEST(CxxClass, cxx_nested_1)
{
  using namespace cxx_nested_1;

  Outer o;
  o.test();
}

namespace cxx_nested_2
{
  class Outer
  {
  private:
    int outer_x = 100;

  public:
    void test()
    {
      Inner *inner = new Inner();
      inner->display();

      // fails in java as well
      // cxx.cpp:8425:39: error: ‘y’ was not declared in this scope
      //        std::cout << "display: y = " << y << std::endl;
      std::cout << "display: y = " << std::endl;
    }

  private:
    class Inner
    {

    public:
      int y = 10;

    public:
      void display()
      {
        // have access to enclosing scope? No
        // cxx.cpp:8397:47: error: invalid use of non-static data member
        // ‘cxx_nested::Outer::outer_x’ std::cout << "display: outer_x = " <<
        // outer_x;
        std::cout << "display: outer_x = " << std::endl;
      }
    };
  };
} // namespace cxx_nested_2

TEST(CxxClass, cxx_nested_2)
{
  using namespace cxx_nested_2;

  Outer o;
  o.test();
}

/* ={=========================================================================
cxx-class-vptr
*/

namespace cxx_class_vptr
{
  class foo1
  {
    int x;
    int y;
  };

  class foo2
  {
    int x;
    int y;

  public:
    int sum() { return x + y; }
  };

  class foo3
  {
  public:
    int x;
    int y;

  public:
    virtual int sum() { return x + y; }
  };

  class foo4 : public foo3
  {
  public:
    int a;
    int b;

  public:
    virtual int mul() { return a * b; }
  };
} // namespace cxx_class_vptr

// :11992:51: warning: offsetof within non-standard-layout type ‘cxx_class_vptr::foo3’ is undefined [-Winvalid-offsetof]
//    std::cout << "offsetof(foo3, x) : " << offsetof(foo3, x) << std::endl;

#if 0
TEST(CxxClass, check_size_vptr)
{
  using namespace cxx_class_vptr;

  foo1 f1;
  EXPECT_THAT(sizeof(f1), 8);

  foo2 f2;
  EXPECT_THAT(sizeof(f2), 8);

  // has virtual function and have vptr in a object
  foo3 f3;

  // x86_64 GNU/Linux, +8
  EXPECT_THAT(sizeof(f3), 16);

  // offsetof(foo3, x) : 8
  // offsetof(foo3, y) : 12
  std::cout << "offsetof(foo3, x) : " << offsetof(foo3, x) << std::endl;
  std::cout << "offsetof(foo3, y) : " << offsetof(foo3, y) << std::endl;

  // inheritance?
  foo4 f4;

  // x86_64 GNU/Linux, +8(vptr), +8(two members)
  EXPECT_THAT(sizeof(f4), 24);

  // to use `offsetof`, member variable must be public
  // offsetof(foo4, x) : 8
  // offsetof(foo4, y) : 12
  // offsetof(foo4, a) : 16
  // offsetof(foo4, b) : 20
  std::cout << "offsetof(foo4, x) : " << offsetof(foo4, x) << std::endl;
  std::cout << "offsetof(foo4, y) : " << offsetof(foo4, y) << std::endl;
  std::cout << "offsetof(foo4, a) : " << offsetof(foo4, a) << std::endl;
  std::cout << "offsetof(foo4, b) : " << offsetof(foo4, b) << std::endl;

  // multiple inheritance cause to have multiple vptrs.
}
#endif

/* ={=========================================================================
cxx-random

Even though the numbers generated appear to be random, a given generator returns
the same sequence of numbers each time it is run. This is very `helpful` during
testing and on the other hand, programs have to take this fact into account. 

To have different sequence, make the engine and distribution objects `static`
and they will hold their state across calls.

*/

TEST(CxxRandom, check_distribution_1)
{
  std::default_random_engine dre{};

  std::ostringstream oss{};

  {
    std::string expected{
      "16807, 282475249, 1622650073, 984943658, 1144108930, 470211272, "
      "101027544, 1457850878, 1458777923, 2007237709, "};

    // `The engines are function-object` class that define a call operator and
    // returns a random unsigned number. The engine object is similar to the
    // output of rand().

    // use random but always shows the same sequence
    for (size_t i = 0; i < 10; ++i)
      oss << dre() << ", ";

    EXPECT_THAT(oss.str(), expected);
  }

  {
    // clear oss
    oss.str("");

    std::uniform_int_distribution<unsigned> dist1{0, 6};

    for (size_t i = 0; i < 10; ++i)
      oss << dist1(dre) << ", ";

    // std::cout << oss.str() << std::endl;

    EXPECT_THAT(oss.str(), "2, 3, 5, 0, 0, 3, 4, 0, 2, 0, ");
  }

  {
    // clear oss
    oss.str("");

    std::uniform_int_distribution<unsigned> dist1{6, 12};

    for (size_t i = 0; i < 10; ++i)
      oss << dist1(dre) << ", ";

    // std::cout << oss.str() << std::endl;

    EXPECT_THAT(oss.str(), "8, 10, 10, 12, 11, 9, 6, 10, 8, 10, ");
  }
}

namespace cxx_random
{
  class UseEngine
  {
  private:
    static std::default_random_engine dre;
    static std::uniform_int_distribution<size_t> dist;

  public:
    int operator()() { return dist(dre); }
  };

  std::default_random_engine UseEngine::dre{};
  std::uniform_int_distribution<size_t> UseEngine::dist{0, 24};

  // compile error and see CPL 1182
  //
  // class UseEngine2
  // {
  //   private:
  //     std::uniform_int_distribution<>::param_type p;
  //     auto r = std::bind(std::uniform_int_distribution<>{p},
  //                        std::default_random_engine{});
  //
  //   public:
  //     UseEngine2(int lo, int hi)
  //         : p{lo, hi}
  //     {}
  //     int operator()() const { return r(); }
  // };

  class UseRand
  {
  private:
    int max_{};
    int min_{};

  public:
    int operator()() { return min_ + (rand() % (max_ - min_ + 1)); }

    UseRand(int min, int max)
        : max_{max}
        , min_{min}
    {}
  };
} // namespace cxx_random

TEST(CxxRandom, check_distribution_2)
{
  using namespace cxx_random;

  std::ostringstream oss{};

  // always same sequence
  {
    std::vector<size_t> coll1{};

    std::generate_n(std::back_inserter(coll1), 12, UseEngine());

    for (size_t i = 0; i < coll1.size(); ++i)
      oss << coll1[i] << ", ";

    // std::cout << oss.str() << std::endl;

    EXPECT_THAT(oss.str(), "0, 3, 18, 11, 13, 5, 1, 16, 16, 23, 9, 12, ");
  }

  // always same sequence but differnet numbers since dre is static single
  // instance and makes different sequence

  {
    std::vector<size_t> coll1{};

    oss.str("");

    std::generate_n(std::back_inserter(coll1), 12, UseEngine());

    for (size_t i = 0; i < coll1.size(); ++i)
      oss << coll1[i] << ", ";

    // std::cout << oss.str() << std::endl;

    EXPECT_THAT(oss.str(), "20, 0, 1, 13, 16, 0, 9, 1, 10, 17, 14, 23, ");
  }

  {
    std::vector<size_t> coll1{};

    oss.str("");

    std::generate_n(std::back_inserter(coll1), 12, UseRand{0, 23});

    for (size_t i = 0; i < coll1.size(); ++i)
      oss << coll1[i] << ", ";

    // std::cout << oss.str() << std::endl;

    EXPECT_THAT(oss.str(), "7, 22, 9, 19, 17, 7, 10, 12, 9, 13, 2, 19, ");
  }
}

TEST(CxxRandom, check_distribution_3)
{
  // single instance and makes different sequence
  {
    std::default_random_engine dre;

    std::vector<unsigned int> coll1{};

    for (size_t i = 0; i < 18; ++i)
    {
      coll1.push_back(dre());
    }

    std::vector<unsigned int> coll2{};

    for (size_t i = 0; i < 18; ++i)
    {
      coll2.push_back(dre());
    }

    // differnt sequence
    EXPECT_THAT(coll1 == coll2, false);
  }

  // different dre instance and makes the same sequence
  {
    std::vector<unsigned int> coll1{};
    std::vector<unsigned int> coll2{};

    {
      std::default_random_engine dre;

      for (size_t i = 0; i < 18; ++i)
      {
        coll1.push_back(dre());
      }
    }

    {
      std::default_random_engine dre;

      for (size_t i = 0; i < 18; ++i)
      {
        coll2.push_back(dre());
      }
    }

    // same sequence
    EXPECT_THAT(coll1 == coll2, true);
  }
}

#if 0
use seed

The same sequence is helpful during debugging. However, once program is tested
we often want to cause each run to generate different random results.

default_random_engine(C++11) implementation-defined

namespace cxx_code
{
  // bits/random.h
  /**
   * @brief Constructs a %linear_congruential_engine random number
   *        generator engine with seed @p __s.  The default seed value
   *        is 1.
   *
   * @param __s The initial seed value.
   */
  explicit
    linear_congruential_engine(result_type __s = default_seed)
    { seed(__s); }

  /**
   * The classic Minimum Standard rand0 of Lewis, Goodman, and Miller.
   */
  typedef linear_congruential_engine<uint_fast32_t, 16807UL, 0UL, 2147483647UL>
    minstd_rand0;

  typedef minstd_rand0 default_random_engine;
}

Picking a good seed, like most things about generating good random numbers, is
surprisingly hard. The most common approach is to call `time()` function. 

Since that retuns time as the number of seconds, this seed is 'only' useful for
applications that generate the seed at longer intervals. This usually doesn't
work if the program is run repeatedly as part of an automated process; it might
wind up with the same seed several times.

#endif

TEST(CxxRandom, check_distribution_4)
{
  constexpr auto MAX_GENERATION{100000};

  std::default_random_engine dre1{};
  std::default_random_engine dre2{2147483646};

  std::default_random_engine dre3{};
  dre3.seed(32767);

  std::default_random_engine dre4{32767};

  int unmatched{};
  int matched{};

  for (size_t i = 0; i < MAX_GENERATION; ++i)
  {
    // uses different seeds so expects no matches between them
    if (dre1() == dre2())
      ++matched;

    // uses the same seeds so expects matches between them
    if (dre3() != dre4())
      ++unmatched;
  }

  EXPECT_THAT(matched, 0);
  EXPECT_THAT(unmatched, 0);
}

#if 0
CPL 40.7.2

If an implementation is able to offer a truly random number generator, that
source of random numbers is presented as a uniform random number generator
called random_device :

random_device rd {s};

The string s identifies a source of random numbers; implementation-defined;
explicit

HOWEVER, do not have details about "s".


man random

RANDOM(4)

NAME
       random, urandom - kernel random number source devices

SYNOPSIS
#include <linux/random.h>

       int ioctl(fd, RNDrequest, param);

DESCRIPTION
       The  character  special  files  /dev/random and /dev/urandom (present
           since Linux 1.3.30) provide an interface to the kernel's random
       number generator.  The file /dev/random has major device number 1 and
       minor device number 8.  The file /dev/urandom has major device number 1
       and minor device number 9.

https://unix.stackexchange.com/questions/324209/when-to-use-dev-random-vs-dev-urandom
https://www.2uo.de/myths-about-urandom/

#endif

TEST(CxxRandom, check_distribution_5)
{
  {
    // millisecond, 10-3
    constexpr auto RANDWINDOWMS{1800000};

    std::chrono::seconds min = static_cast<std::chrono::seconds>(0);

    // convert ms to sec
    std::chrono::seconds max =
      static_cast<std::chrono::seconds>(RANDWINDOWMS / 1000);

    // `second` type distribution
    std::uniform_int_distribution<std::chrono::seconds::rep> rdist(min.count(),
                                                                   max.count());

    std::random_device rd{"/dev/urandom"};

    for (size_t i = 0; i < 10; ++i)
    {
      // get random seconds
      std::chrono::seconds window = std::chrono::seconds(rdist(rd));

      // std::cout << "random window in seconds : " << window << ", " << window.count()
      //            << std::endl;

      std::cout << "random window in seconds : " << window.count() << std::endl;
    }
  }

  // use std::chrono
  {
    // millisecond, 10-3
    constexpr std::chrono::milliseconds RANDWINDOWMS{1800000};

    std::chrono::seconds min{};

    // convert ms to sec
    std::chrono::seconds max =
      std::chrono::duration_cast<std::chrono::seconds>(RANDWINDOWMS);

    // `second` type distribution
    std::uniform_int_distribution<std::chrono::seconds::rep> rdist(min.count(),
                                                                   max.count());

    std::random_device rd{"/dev/urandom"};

    for (size_t i = 0; i < 10; ++i)
    {
      // get random seconds
      // std::chrono::seconds window = std::chrono::seconds(rdist(rd));
      auto window = rdist(rd);

      // std::cout << "random window in seconds : " << window << ", " << window.count()
      //            << std::endl;

      std::cout << "random window in seconds : " << window << std::endl;
    }
  }
}

// cxx-17
TEST(CxxAny, check_any) {}

/*
={=========================================================================
cxx-namespace

       #include <math.h>

       double sqrt(double x);
       float sqrtf(float x);
       long double sqrtl(long double x);

https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#sf6-use-using-namespace-directives-for-transition-for-foundation-libraries-such-as-std-or-within-a-local-scope-only

*/

TEST(CxxNamespace, check_conflict) {}

/*
={=========================================================================
*/

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
