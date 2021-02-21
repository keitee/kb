#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>

using namespace testing;

/*
// ={=========================================================================

3.1 Object-like Macros

An object-like macro is a simple identifier which will be replaced by a code
fragment. It is called object-like because it looks like a data object in code
that uses it. They are most commonly used to give symbolic names to numeric
constants.

You create macros with the ‘#define’ directive. ‘#define’ is followed by the
name of the macro and then the token sequence it should be an abbreviation for,
     which is variously referred to as the macro’s body, expansion or
     replacement list. For example,

#define BUFFER_SIZE 1024

*/

namespace cxx_cpp
{
  // not defines and fails on "EXPECT_THAT(false, true);"
  // (empty)
  //
  // // defines and value 1
  // #define CHECK_CXX_CPP_DEFINED 1
  //
  // // defines and value 0
  // #define CHECK_CXX_CPP_DEFINED 0
  //
  // // defines
  // #define CHECK_CXX_CPP_DEFINED
  //
  // then see error:
  // :8855:27: error: operator '||' has no left operand
  // #if CHECK_CXX_CPP_DEFINED || 1
  //
} // namespace cxx_cpp

// ={=========================================================================
TEST(CxxCpp, defined)
{
#if defined CHECK_CXX_CPP_DEFINED
  EXPECT_THAT(true, true);
#else
  EXPECT_THAT(false, true);
#endif

#if defined(CHECK_CXX_CPP_DEFINED)
  EXPECT_THAT(true, true);
#else
  EXPECT_THAT(false, true);
#endif

#ifdef CHECK_CXX_CPP_DEFINED
  EXPECT_THAT(true, true);
#else
  EXPECT_THAT(false, true);
#endif

#if CHECK_CXX_CPP_DEFINED || 1
  EXPECT_THAT(true, true);
#else
  EXPECT_THAT(false, true);
#endif

  // fails when #define CHECK_CXX_CPP_DEFINED 0 since it becomes "if 0 || 0"
#if CHECK_CXX_CPP_DEFINED || 0
  EXPECT_THAT(true, true);
#else
  EXPECT_THAT(false, true);
#endif

#if (CXX_CPP_VALUE == GCC) && (CXX_CPP_VALUE2 == ARM)
  std::cout << "GCC AND ARM" << std::endl;
#elif (CXX_CPP_VALUE == VALUE_GLANG) && (CXX_CPP_VALUE2 != ARM)
  std::cout << "GLANG AND MIPS" << std::endl;
#endif
}

/*
// ={=========================================================================

4.2.5 Elif

One common case of nested conditionals is used to check for more than two
possible alternatives. For example, you might have

#if X == 1
...
#else // X != 1
#if X == 2
...
#else  // X != 2
...
#endif // X != 2
#endif // X != 1

Another conditional directive, ‘#elif’, allows this to be abbreviated as
follows:

#if X == 1
...
#elif X == 2
...
#else  // X != 2 and X != 1
...
#endif // X != 2 and X != 1

‘#elif’ stands for “else if”. Like ‘#else’, it goes in the middle of a
conditional group and subdivides it; it does not require a matching ‘#endif’
of its own. Like ‘#if’, the ‘#elif’ directive includes an expression to be
tested.  The text following the ‘#elif’ is processed only if the original
‘#if’-condition failed and the ‘#elif’ condition succeeds.

More than one ‘#elif’ can go in the same conditional group. Then the text
after each ‘#elif’ is processed only if the ‘#elif’ condition succeeds after
the original ‘#if’ and all previous ‘#elif’ directives within it have failed.

‘#else’ is allowed after any number of ‘#elif’ directives, but ‘#elif’ may not
follow ‘#else’.

*/

#define CPPUNIT_NO_STREAM 1
// shows "if defined (CPPUNIT_NO_STREAM)"

// #define CPPUNIT_HAVE_CLASS_STRSTREAM  1
// shows "if defined (CPPUNIT_HAVE_CLASS_STRSTREAM)"

// so, one of them must be defined in order not to see "error"

// ={=========================================================================
TEST(CxxCpp, elseif)
{
#if defined(CPPUNIT_NO_STREAM)
  std::cout << "if defined (CPPUNIT_NO_STREAM)" << std::endl;
#elif CPPUNIT_HAVE_SSTREAM // #if defined( CPPUNIT_NO_STREAM )
  std::cout << "if defined (CPPUNIT_HAVE_SSTREAM)" << std::endl;
#elif CPPUNIT_HAVE_CLASS_STRSTREAM
  std::cout << "if defined (CPPUNIT_HAVE_CLASS_STRSTREAM)" << std::endl;
#else // CPPUNIT_HAVE_CLASS_STRSTREAM
#error Cannot define CppUnit::OStringStream.
#endif // #if defined( CPPUNIT_NO_STREAM )
}

namespace cxx_cpp
{
  // taken from <sys/timerfd.h>
  // Bits to be set in the FLAGS parameter of `timerfd_settime'.
  enum
  {
    TFD_TIMER_ABSTIME = 1 << 0,
#define TFD_TIMER_ABSTIME TFD_TIMER_ABSTIME
    TFD_TIMER_CANCEL_ON_SET = 1 << 1
#define TFD_TIMER_CANCEL_ON_SET TFD_TIMER_CANCEL_ON_SET
  };
} // namespace cxx_cpp

// ={=========================================================================
TEST(CxxCpp, define_and_enum)
{
  using namespace cxx_cpp;

  // can use enum but cannot use like:
  // EXPECT_THAT(TFD_TIMER_ABSTIME, 1);

  if (TFD_TIMER_ABSTIME == 1)
  {
    EXPECT_THAT(true, true);
  }
  else
  {
    EXPECT_THAT(true, false);
  }

#ifdef TFD_TIMER_ABSTIME
  EXPECT_THAT(true, true); // true
#else
  EXPECT_THAT(true, false); // false
#endif
}

#if 0
namespace cxx_cpp
{
#define dprint(expr) printf(#expr " = %g\n", expr)
#define dprint_string(expr) std::string coll(#expr " = %g\n")
#define dprint_string_1(expr) std::string coll(#expr)

#define xstr(s) mstr(s)
#define mstr(s) #s
#define foo 4
} // namespace cxx_cpp

TEST(CxxCpp, check_stringification)
{
  {
    double x = 10.0;
    double y = 3.0;

    // printf( "x/y = %g\n", x/y );
    // x/y = 3.33333
    dprint(x / y);
  }

  {
    dprint_string("string");
    EXPECT_THAT(coll, "\"string\" = %g\n");
  }

  {
    dprint_string_1("string");
    EXPECT_THAT(coll, "\"string\"");
  }

  // If you want to stringify the-result-of-expansion of a macro argument, you
  // have to use two-levels of macros.
  {
    // "foo" is stringified when it is used in "str(foo)" but not macro-expanded
    // first. so not "4"

    std::string coll1{mstr(foo)};
    EXPECT_THAT(coll1, "foo");

    // But "foo" is an ordinary argument to "xstr(foo)", so it is completely
    // macro-expanded before xstr itself is expanded (see Argument Prescan).
    // Therefore, by the time str gets to its argument, it has already been
    // macro-expanded.
    //
    // xstr (foo)
    // ==> xstr (4)
    // ==> str (4)
    // ==> "4"

    std::string coll2{xstr(foo)};
    EXPECT_THAT(coll2, "4");
  }

  // print the name of an [expression] and its value, along with the file name
  // and the line number.
  //
  // ex
  // int some_function() {
  //   int foo;
  //   /* a lot of complicated code goes here */
  //   dumpme(foo, "%d");
  //   /* more complicated code goes here */
  // }
  //
  // cxx.cpp:4157: value=100

  {
#define dumpme(x, fmt) printf("%s:%u: %s=" fmt "\n", __FILE__, __LINE__, #x, x)

    int value{100};
    dumpme(value, "%d");
  }

  // void JPA_CREATE(const char* vname, int value)
  // {
  //   printf("JPA_CREATE: value=%s value=%d...\n", vname, value );
  // }

  //  {
  //#define KT_CREATE(value)	JPA_CREATE( #value, value)
  //
  //    int val = 3;
  //    int is_this_my_own = 4;
  //
  //    KT_CREATE(val);
  //    KT_CREATE(is_this_my_own);
  //  }

  // <case>
  // want to print on console then usuel loggig and macros are defined like:
  //
  // PDM_DISK_SPINDOWN_LOG( g_pdm_diag_segment_id, ("device %s now in power
  // saving mode", physDev->shDeviceName));
  //
  // to
  //
  // printf("device %s now in power saving mode", physDev->shDeviceName);
  //
  // #ifdef CQ1840116
  // #define PDM_DISK_SPINDOWN_LOG           DIAG_LOG_ERROR
  // #else
  // #define PDM_DISK_SPINDOWN_LOG           DIAG_LOG_INFO
  // #endif
  //
  // #define DIAG_LOG_ERROR(id, msg)		DIAG_LOG_F2B_WRITE((id),
  // DIAG_CMN_TRACE_ERROR, msg)
  //
  // How?

  {
#define PDM_DISK_SPINDOWN_LOG DIAG_LOG_ERROR
#define DIAG_LOG_ERROR(id, msg) printf msg

    // #define DIAG_LOG_ERROR(id, msg)         printf msg
    // printf "(\"device %s now in power saving mode\",
    // \"physDev->shDeviceName\")";
    //
    // #define DIAG_LOG_ERROR(id, msg)         printf msg
    // printf ("device %s now in power saving mode", "physDev->shDeviceName");

    // PDM_DISK_SPINDOWN_LOG( g_pdm_diag_segment_id, ("device %s now in power
    // saving mode", "physDev->shDeviceName"));
    PDM_DISK_SPINDOWN_LOG(
      g_pdm_diag_segment_id,
      ("device %s now in power saving mode\n", "physDev->shDeviceName"));
  }
}

namespace cxx_cpp
{
  void argprint(int num_args, ...)
  {
    va_list ap;

    // The argument `last` is the name of the last argument `before` the
    // variable argument list, that is, the last argument of `which the calling
    // function knows the type.`

    va_start(ap, num_args);

    // note that num_arg is valid arg to use

    for (int i = 0; i < num_args; ++i)
    {
      std::cout << "arg " << i << " is " << va_arg(ap, int) << std::endl;
    }

    va_end(ap);
  }

  // The function takes a string of format characters and prints out the
  // argument associated with each format character based on the type.

  void xprint(char const *fmt, ...)
  {
    va_list ap;
    int d;
    char c, *s;

    va_start(ap, fmt);

    while (*fmt)
    {
      switch (*fmt++)
      {
        case 's':
          s = va_arg(ap, char *);
          printf("string %s\n", s);
          break;

        case 'd':
          d = va_arg(ap, int);
          printf("int %d\n", d);
          break;

        case 'c':

          // c = va_arg(ap, char);
          // cxx.cpp:7950:26: warning: ‘char’ is promoted to ‘int’ when passed
          // through ‘...’
          //            c = va_arg(ap, char);
          //                           ^
          // cxx.cpp:7950:26: note: (so you should pass ‘int’ not ‘char’ to
          // ‘va_arg’) cxx.cpp:7950:26: note: if this code is reached, the
          // program will abort
          //
          // need a cast here since va_arg only takes fully promoted types

          c = (char)va_arg(ap, int);
          printf("char %c\n", c);
          break;
      }
    }
    va_end(ap);
  }

} // namespace cxx_cpp

// arg 0 is 10
// arg 1 is 20
// arg 2 is 30
//
// string foo
// int 10
// char a
//
// string foo
// int 10
// int 97

TEST(CxxCpp, check_variable_args)
{
  using namespace cxx_cpp;

  argprint(3, 10, 20, 30);

  xprint("sdc", "foo", 10, 'a');
  xprint("sdd", "foo", 10, 'a');
}

// no napesapce effect but for visual effect
namespace cxx_cpp
{

  // No for GCC 4.6 and 6.x
  //
  // #define eprintf(fmt, ...)  fprintf(stdout, fmt, __VA_ARGS__)
  // cxx.cpp:7996:66: error: expected primary-expression before ‘)’ token
  //  #define eprintf(format, ...)  fprintf(stdout, format, __VA_ARGS__)
  //                                                                   ^

#define eprintf(fmt, ...) fprintf(stdout, fmt, ##__VA_ARGS__)
} // namespace cxx_cpp

// [ RUN      ] Cpp.VaargMacro
// success!
// [       OK ] Cpp.VaargMacro (0 ms)

TEST(CxxCpp, cpp_VariableArgsMacro)
{
  eprintf("success!\n");
}

namespace
{
#define SR1(x)                                                                 \
  {                                                                            \
    std::cout << dec << boolalpha << showbase << x << std::endl;               \
  }

#define VAR(var)                                                               \
  " " #var "="                                                                 \
    << (var) // VAR here are only simple aguments allowed thus 'hex<<x' is not allowed
#define _VAR(x) << "," VAR(x)
#define VAR2(name, var)                                                        \
  " " #name "="                                                                \
    << var // no parenthesis around var - to make it possible to give 'hex<<x' as argument to var
#define _VAR2(name, var) << "," VAR2(name, var)
} // namespace

// [ RUN      ] CxxCpp.cpp_case1
// cpp case example
// cpp case example more to say
// cpp case example value1=10
// cpp case example value1=10
// cpp case example value1=10, value2=10
// cpp case example what=10
// [       OK ] CxxCpp.cpp_case1 (0 ms)

TEST(CxxCpp, cpp_case1)
{
  int value1{10};
  int value2{10};

  SR1("cpp case example");
  SR1("cpp case example"
      << " more to say");
  SR1("cpp case example"
      << " value1=" << value1);
  SR1("cpp case example" VAR(value1));
  SR1("cpp case example" VAR(value1) _VAR(value2));
  SR1("cpp case example" VAR2(what, value1));
}
#endif

// ={=========================================================================

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
