#include <iostream>
#include <set>
#include <vector>
#include <memory>
#include <chrono>
#include <limits>
#include <thread>
#include <list>
#include <forward_list>
#include <regex>
#include <boost/lexical_cast.hpp>

#include "gmock/gmock.h"

using namespace std;
using namespace std::placeholders;
using namespace testing;

class foo
{
  public:

    explicit foo() : value_(0) 
    {
      cout << "foo()" << endl;
    }

    explicit foo(int &value) : value_(value) 
    {
      cout << "foo(int)" << endl;
    }

    foo(int &value, int) : value_(value) 
    {
      cout << "foo(int, int)" << endl;
    }

    void print_value()
    {
      cout << "value is " << value_ << endl;
    }

    foo &operator()() { return *this; }

  private:
    int value_;
};


TEST(Cxx, XXX)
{ 
  foo f1;

  f1().print_value();
  f1.print_value();
} 

TEST(Cxx, Ex)
{ 
  int value{10};

  foo f1(value);
  foo f2(value, 30);
} 

namespace cxx_dtor {

  class NoVirtualDtorBase
  {
    public:
      NoVirtualDtorBase() 
      { std::cout << "no virtual ctor: base" << std::endl; }

      ~NoVirtualDtorBase() 
      { std::cout << "no virtual dtor: base" << std::endl; }

    private:
      int base_;
  };

  class DerivedFromNoVirtual : public NoVirtualDtorBase
  {
    public:
      DerivedFromNoVirtual() 
      { std::cout << "no virtual ctor: derived" << std::endl; }

      ~DerivedFromNoVirtual() 
      { std::cout << "no virtual dtor: derived" << std::endl; }

    private:
      int derived_;
  };

  class DerivedDerived : public DerivedFromNoVirtual
  {
    public:
      DerivedDerived() 
      { std::cout << "no virtual ctor: d derived" << std::endl; }

      ~DerivedDerived() 
      { std::cout << "no virtual dtor: d derived" << std::endl; }

    private:
      int derived_;
  };

  class VirtualDtorBase
  {
    public:
      VirtualDtorBase() 
      { std::cout << "virtual ctor: base" << std::endl; }

      virtual ~VirtualDtorBase() 
      { std::cout << "virtual dtor: base" << std::endl; }

    private:
      int value_;
  };

  class DerivedFromVirtual : public VirtualDtorBase
  {
    public:
      DerivedFromVirtual() 
      { std::cout << "virtual ctor: derived" << std::endl; }

      ~DerivedFromVirtual() 
      { std::cout << "virtual dtor: derived" << std::endl; }

    private:
      int derived_;
  };

} // namespace

// TEST(Dtor, NoVirtualDtorProblem)
// int main(int argc, char** argv)
// {
//   using namespace cxx_dtor;

//   {
//     cout << "dtor: " << endl;
//     NoVirtualDtorBase* pbase = new DerivedFromNoVirtual;
//     cout << "dtor: " << endl;
//   }

//   {
//     cout << "dtor: " << endl;
//     VirtualDtorBase* pbase = new DerivedFromVirtual;
//     cout << "dtor: " << endl;
//   }
// }


// ={=========================================================================

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

