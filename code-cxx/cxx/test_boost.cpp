#include <boost/lexical_cast.hpp>
#include <boost/variant.hpp>
#include <chrono>
#include <condition_variable>
#include <forward_list>
#include <iostream>
#include <limits>
#include <list>
#include <memory>
#include <mutex>
#include <regex>
#include <set>
#include <thread>
#include <vector>

#include <sys/prctl.h>

#include "gmock/gmock.h"

using namespace std;
using namespace std::placeholders;
using namespace testing;

// (gdb) b
// PatternObserver_sendNotificationWithCallerDispatcherButRaiseException_Test::TestBody()

/*
={=============================================================================
cxx-boost-variant


*/

// boost::variant is defined in boost/variant.hpp. Because boost::variant is a
// *template*, at least one parameter must be specified.
//
// One or more template parameters *specify the supported types*. In
// Example 24.1, v can store values of type double, char, or std::string.

TEST(CxxBoost, setValues)
{
  boost::variant<double, char, std::string> v;
  v = 3.14;
  v = 'A';
  v = "Boost";

  // error: call of overloaded ‘initialize(void*,
  // boost::move_detail::remove_reference<int&>::type)’ is ambiguous v = 100;

  // EXPECT_THAT(true, true);
}

// To display the stored values of v, use the free-standing function
// boost::get()
//
// boost::get() expects one of the valid types for the corresponding variable as
// a template parameter.
//
// Specifying an invalid type will result in a run-time error because validation
// of types does not take place at compile time.
//

TEST(CxxBoost, getValues)
{
  boost::variant<double, char, std::string> v;

  v = 3.14;
  EXPECT_THAT(3.14, boost::get<double>(v));

  v = 'A';
  EXPECT_THAT('A', boost::get<char>(v));

  v = "Boost";
  EXPECT_THAT("Boost", boost::get<std::string>(v));

  try
  {
    EXPECT_THAT(3.14, boost::get<double>(v));
  } catch (exception &e)
  {
    std::ostringstream os;
    os << e.what();
    EXPECT_THAT(os.str(), "boost::bad_get: failed value get using boost::get");
  }
}

// Variables of type boost::variant can be written to streams such as the
// standard output stream, bypassing the hazard of run-time errors

TEST(CxxBoost, printValues)
{
  boost::variant<double, char, std::string> v;

  std::ostringstream os;
  v = 3.14;
  // std::cout << v << std::endl;
  os << v;
  EXPECT_THAT(os.str(), "3.14");

  // clear sstream
  os = std::ostringstream("");

  v = 'A';
  os << v;
  EXPECT_THAT(os.str(), "A");

  // clear sstream
  os = std::ostringstream("");

  v = "Boost";
  os << v;
  EXPECT_THAT(os.str(), "Boost");
}


/*

For type-safe access, Boost.Variant provides a function called
boost::apply_visitor().

As its first parameter, boost::apply_visitor() expects an object of a class
derived from boost::static_visitor. This class must overload operator() for
every type used by the boost::variant variable it acts on. Consequently, the
operator is overloaded three times in Example 24.4 because v supports the types
double, char, and std::string.

boost::static_visitor is a template. The type of the return value of operator()
must be specified as a template parameter. 

*If the operator does not have a return value*, a template parameter is not
required, as seen in the example.

The second parameter passed to boost::apply_visitor() is a boost::variant
variable.

boost::apply_visitor() automatically calls the operator() for the first
parameter that matches *the type of the value currently stored* in the second
parameter. This means that the sample program uses different overloaded
operators every time boost::apply_visitor() is invoked – first the one for
double, followed by the one for char, and finally the one for std::string.

The advantage of boost::apply_visitor() is not only that the correct operator is
called automatically. In addition, boost::apply_visitor() ensures that
overloaded operators have been provided for every type supported by
boost::variant variables. If one of the three overloaded operators had not been
defined, the code could *not be compiled*

*/

namespace cxx_boost
{
  // have to provice all operator() for all types supported for variant type
  // even if it's not actually used. Otherwise, compile errors

  struct output : public boost::static_visitor<>
  {
    void operator()(double d) const { std::cout << d << std::endl; }
    void operator()(char c) const { std::cout << c << std::endl; }
    void operator()(std::string s) const { std::cout << s << std::endl; }
  };
} // namespace cxx_boost

TEST(CxxBoost, SafeAccess)
{
  using namespace cxx_boost;

  boost::variant<double, char, std::string> v;

  v = 3.14;
  boost::apply_visitor(output{}, v);

  v = 'A';
  boost::apply_visitor(output{}, v);

  v = "Boost";
  boost::apply_visitor(output{}, v);
}

// ={=========================================================================

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
