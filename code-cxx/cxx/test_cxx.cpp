#include <boost/lexical_cast.hpp>
#include <chrono>
#include <cstdarg>
#include <forward_list>
#include <future>
#include <iostream>
#include <limits>
#include <list>
#include <memory>
#include <regex>
#include <set>
#include <thread>
#include <vector>

#include "gmock/gmock.h"

using namespace std;
using namespace std::placeholders;
using namespace testing;

/*

56:TEST(CxxType, showSizeOfTypes)
101:TEST(CxxType, Null)
123:TEST(Size, Arrays)
175:TEST(Arith, Comparison)
189:TEST(CxxStatement, switch)
304:TEST(Cxx, If)
335:TEST(Pair, MakePair)
404:TEST(Pair, PackReference)
438:TEST(Pair, Initialisation)
526:TEST(Pair, Comparison)
562:TEST(Tuple, MakeTuple)
600:TEST(Tuple, Tie)
701:TEST(CxxCtor, Private)
744:TEST(CxxCtor, NoDefault)
786:TEST(CxxCtor, Parameters)
828:TEST(CxxCtor, CtorCallsCtor)
876:TEST(CxxCtor, CtorDefaultArgAndInClassInit)
975:TEST(Ctor, CtorInitForms)
1055:TEST(Ctor, CtorInitFromMore)
1101:TEST(Ctor, CtorInitFromExplicit)
1220:TEST(Initialise, List)
1329:TEST(Ctor, Access)
1427:TEST(Dtor, NoVirtualDtorProblem)
1538:TEST(Dtor, AbstractBaseClassNoCompileError)
1575:TEST(Dtor, AbstractBaseClassNeedVirtualDtor)
1649:TEST(CxxCopyControl, PrivateAndDelete)
1728:TEST(Enum, ScopedAndUnscoped)
1841:TEST(Enum, InClass)
1958:TEST(Enum, ScopeAndType)
2030:TEST(RVO, OnSingle)
2037:TEST(RVO, OnVector)
2044:TEST(RVO, OnArg)
2051:TEST(RVO, OnAssignment)
2103:TEST(Reference, UseConstReference)
2140:TEST(Reference, Swap)
2202:TEST(Reference, CString)
2275:TEST(Reference, AccessOnReference)
2298:TEST(Ratio, Ratio)
2318:TEST(Time, SystemCall)
2358:TEST(Time, SleepFor)
2394:TEST(CxxTime, DurationCast)
2480:TEST(Time, CronoClockDetails)
2508:TEST(Time, SteadyClock)
2579:TEST(Time, Timepoint)
2599:TEST(Time, TimePointArithmetic)
2645:TEST(Time, Facet)
2722:TEST(Time, ElapsedTime)
2957:TEST(Time, Snapper)
3190:TEST(CxxFeaturesTest, UseIsspace)
3201:// Q: If run it standalone, it runs slower than on in GTEST. WHY?
3251:TEST(CxxFunctionObject, FunctionPointer)
3289:TEST(CxxFunctionObject, FunctionPointerVoidCast)
3336:TEST(CxxFunctionObject, Function)
3438:TEST(CxxFunctionObject, Bind)
3626:TEST(CxxFunctionObject, BindAndFunction)
3757:TEST(CxxFunctionObject, MemFn)
3898:TEST(CxxFunctionObject, Case)
3923:TEST(CxxFunctionObject, Pointer)
3984:TEST(CxxFunctionObject, LambdaCaptureAndReturn)
4165:TEST(CxxFunctionObject, LambdaBiggies)
4193:TEST(CxxFunctionObject, LambdaCompare)
4257:TEST(CxxSmartPointerShared, construct)
4296:TEST(CxxSmartPointerShared, copy)
4324:TEST(CxxSmartPointer, reset)
4407:TEST(CxxSmartPointerShared, example1)
4450:TEST(CxxSmartPointer, bool)
4503:TEST(SharedPointer, UniqueDoNotSupportCopy)
4516:TEST(SharedPointer, UniqueDoNotSupportCopyInitCopyForm)
4527:TEST(CxxSmartPointerUnique, construct)
4579:TEST(CxxSmartPointerUnique, moveAssign)
4668:TEST(SmartPointer, UniqueSinkSource)
4757:TEST(CxxSmartPointer, Deleter)
4849:TEST(SmartPointer, DeleteTime)
4915:TEST(SmartPointer, UniqueDeleteReleaseReset)
4973:TEST(SmartPointer, UseCount)
5018:TEST(SmartPointer, WeakNotInReferenceCount)
5282:TEST(SmartPointer, WeakCyclicReference)
5366:TEST(SmartPointer, WeakCyclicReferenceSolution)
5476:TEST(SmartPointer, WeakResourceManagerSolution)
5682:TEST(SmartPointer, OwnUnique)
5864:TEST(SmartPointer, OwnShared)
5918:// TEST(CxxFeaturesTest, DISABLED_UseRangeForOnInteger)
5934:TEST(CxxFeaturesTest, UseHashOnString)
5994:TEST(CxxBool, LogicalNot)
6015:TEST(Bool, CheckBoolDefault)
6070:TEST(Bool, CheckUsage)
6101:TEST(Stdio, Input)
6171:TEST(Stdio, GetLine)
6245:TEST(Stdio, SpecialTypes)
6309:TEST(Stdio, Manipulators)
6343:TEST(Stdio, ManipulatorsFloat)
6435:TEST(Override, Condition_1)
6497:TEST(Override, Condition_2)
6550:TEST(Override, Condition_3)
6604:// TEST(Override, Condition_4)
6690:TEST(Override, PureVirtual)
6744:TEST(Rtti, UseTypeid)
6762:TEST(Rtti, DynamicCast)
6829:TEST(Regex, Match)
6876:TEST(Regex, MatchFound)
6904:TEST(Regex, MatchResult)
7017:TEST(Bit, BitSetCtor)
7058:TEST(Bit, SizeConsideration)
7088:TEST(Bit, RightShift)
7208:TEST(Bit, MaxNegagiveIsSpecial)
7271:TEST(Bit, BitVectors)
7306:TEST(Bit, BitSet)
7384:TEST(Bit, Overflow)
7395:TEST(MinMax, Order)
7424:TEST(IntegerDivision, FahrenheitCelsius)
7457:TEST(IntegerDivision, FahrenheitCelsius_Float)
7506:TEST(IntegerDivision, Percentage)
7535:TEST(IntegerDivision, Shift)
7603:// TEST(Operator, MinMax)
7616:TEST(Operator, PrefixPostfix)
7646:TEST(Template, Function)
7721:TEST(Template, Specialisation)
7789:TEST(CxxTemplate, DefaultTypeArgument)
7854:TEST(Template, MemberTemplate)
8022:TEST(Template, ReturnType)
8093:TEST(Template, Reference)
8137:TEST(Template, Overload)
8202:TEST(Template, Friend)
8277:TEST(Template, Forward)
8415:TEST(Template, ForwardEx)
8459:TEST(Template, VariadicSizeofOperator)
8531:TEST(Template, TypeTrait)
8553:TEST(Const, NoViaConstReference)
8566:TEST(Const, NoConstToNonConst)
8694:TEST(Const, ForMemberFunction)
8722:TEST(Exception, OwnException)
8785:TEST(Exception, Noexcept)
8839:TEST(TypeConversion, Double)
8861:TEST(CxxPrintf, Formats)
8924:TEST(CxxCpp, Stringification)
9125:TEST(CxxCpp, VariableArgs)
9153:TEST(CxxCpp, VariableArgsMacro)
9164:TEST(CxxCpp, useDefined)
9238:TEST(Assert, StaticAssert)
9249:TEST(Numeric, Abs)
9282:TEST(Typedef, Alias)
9385:TEST(Class, ColonDoesMatter)
9431:TEST(Class, Nested_1)
9478:TEST(Class, Nested_2)

*/

/*
// ={=========================================================================
//
[ RUN      ] CxxMemoryModel.allocator
sizeof p : 8
sizeof *p: 32
coll{string1,string2,string3,}
coll{string1,string2,string3,}
[       OK ] CxxMemoryModel.allocator (0 ms)

*/

TEST(CxxMemoryModel, memory_model_allocator)
{
  // do allocation only
  {
    std::allocator<std::string> alloc;
    auto const p = alloc.allocate(10);
    std::cout << "sizeof p : " << sizeof(p) << std::endl;
    std::cout << "sizeof *p: " << sizeof(*p) << std::endl;
    alloc.deallocate(p, 10);
  }

  // do allocation and construction
  {
    std::allocator<std::string> alloc;

    // auto const data = alloc.allocate(10);
    // NOTE: compile error when use `const`
    auto data = alloc.allocate(10);

    // back it up.
    // NOTE: shall use `coll` to deallocate and access. crashes otherwise.
    auto const coll = data;

    // calls ctor. From *cxx-11*, supports many ctors of the 'type'.
    //
    // The first argument to construct() must be a pointer to unconstructed
    // space allocated by allocate(). The second and subsequent argument
    // determine ctor to use.

    alloc.construct(data++, "string1");
    alloc.construct(data++, "string2");
    alloc.construct(data++, "string3");

    std::cout << "coll{";

    for (int i = 0; i < 3; i++)
      std::cout << coll[i] << ",";

    std::cout << "}" << std::endl;

    // alloc.destroy(p) calls dtor

    alloc.destroy(--data);
    alloc.destroy(--data);
    alloc.destroy(data);

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

    auto result = std::uninitialized_copy(coll.cbegin(), coll.cend(), data);

    std::cout << "coll{";

    for (int i = 0; i < 3; i++)
      std::cout << data[i] << ",";

    std::cout << "}" << std::endl;

    alloc.deallocate(data, 10);
  }
}

// ={=========================================================================
// cxx-size cxx-sizeof

// LP64
// __x86_64__
// size of (int) is                : 4
// size of (unsigned int) is       : 4
// size of (long) is               : 8
// size of (unsigned long) is      : 8
// size of (long int) is           : 8
// size of (unsigned long int) is  : 8
// size of (long long) is          : 8
// size of (unsigned long long) is : 8
// size of (* int) is              : 8
// size of (* unsigned int) is     : 8
// size of (size_t) is             : 8
// size of (uint32_t) is           : 4
// size of (uint64_t) is           : 8
//
//
// LP32
// __x86_32__
// size of (int) is                : 4
// size of (unsigned int) is       : 4
// size of (long) is               : 4
// size of (unsigned long) is      : 4
// size of (long int) is           : 4
// size of (unsigned long int) is  : 4
// size of (long long) is          : 8
// size of (unsigned long long) is : 8
// size of (* int) is              : 4
// size of (* unsigned int) is     : 4
// size of (size_t) is             : 4
// size of (uint32_t) is           : 4
// size of (uint64_t) is           : 8

TEST(CxxType, type_size)
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

  std::cout << "size of (int) is                : " << sizeof(int) << std::endl;
  std::cout << "size of (unsigned int) is       : " << sizeof(unsigned int)
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

// ={=========================================================================

TEST(CxxType, type_null)
{
  const char *p1 = "";
  const char *p2 = nullptr;

  EXPECT_NE(p1, p2);
}

namespace use_sizeof
{
  struct nlist
  {
    struct nlist *next;
    char *name;
    char *defn;
  };
} // namespace use_sizeof

// ={=========================================================================
// cxx-cast cxx-conversion

TEST(CxxType, type_cast)
{
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

    float value2 = float(value0);

    EXPECT_TRUE((value2 / value1) == 1.0);
    EXPECT_TRUE((value2 / value1) == 1);
  }
}

// ={=========================================================================
// cxx-variant

namespace cxx_variant
{
  // custom variant type

  struct Argument
  {
    enum class Type
    {
      Bool,
      Int,
      UInt,
      Double,
      String
    } m_type;

    // NOTE: cannot use in-class init for union

    union Base
    {
      bool bool_;
      int i_;
      unsigned ui_;
      double real_;

      // cxx-error: initializations for multiple members of
      // since it's cxx-union
      // explicit Base(bool b)
      //     : bool_(b)
      //     , i_(0)
      //     , ui_(0)
      //     , real_(0.0f)
      // {}

      // default ctor is necessary since `string` type needs it and otherwise
      // it's error
      Base()
          : real_(0.0f)
      {}

      explicit Base(bool b)
          : bool_(b)
      {}

      explicit Base(int i)
          : i_(i)
      {}

      explicit Base(unsigned ui)
          : ui_(ui)
      {}

      explicit Base(double real)
          : real_(real)
      {}
    } m_base;

    std::string m_string;

    explicit Argument(bool value)
        : m_type(Type::Bool)
        , m_base(value)
    {}

    explicit Argument(int value)
        : m_type(Type::Int)
        , m_base(value)
    {}

    explicit Argument(unsigned value)
        : m_type(Type::UInt)
        , m_base(value)
    {}

    explicit Argument(double value)
        : m_type(Type::Double)
        , m_base(value)
    {}

    explicit Argument(std::string value)
        : m_type(Type::String)
        , m_string(value)
    {}

    // getters
    explicit operator bool() const
    {
      if (m_type != Type::Bool)
      {
        std::cout << "type is not bool" << std::endl;
        return false;
      }

      return m_base.bool_;
    }

    explicit operator int() const
    {
      if (m_type != Type::Int)
      {
        std::cout << "type is not int" << std::endl;
        return INT32_MAX;
      }

      return m_base.i_;
    }

    explicit operator unsigned() const
    {
      if (m_type != Type::UInt)
      {
        std::cout << "type is not unsigned int" << std::endl;
        return UINT32_MAX;
      }

      return m_base.ui_;
    }

    explicit operator double() const
    {
      if (m_type != Type::Double)
      {
        std::cout << "type is not double" << std::endl;
        return std::nan("");
      }

      return m_base.real_;
    }

    explicit operator std::string() const
    {
      if (m_type != Type::String)
      {
        std::cout << "type is not string" << std::endl;
        return std::string("");
      }

      return m_string;
    }
  };
} // namespace cxx_variant

TEST(CxxType, show_variant)
{
  using namespace cxx_variant;

  // error since it do not have default ctor
  // Argument arg1;

  {
    Argument arg1(true);
    EXPECT_THAT((bool)arg1, true);
  }

  {
    Argument arg1(false);
    EXPECT_THAT((bool)arg1, false);
  }

  {
    Argument arg1(3301);
    EXPECT_THAT((int)arg1, 3301);
  }

  {
    Argument arg1(3301.0);
    EXPECT_THAT((double)arg1, 3301.0);
  }

  {
    // since ctor is explicit
    Argument arg1(std::string("string variant"));
    EXPECT_THAT((std::string)arg1, std::string("string variant"));
  }
}

namespace cxx_variant
{
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
    };

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

    // void visit(Visitor &&visitor) const
    // this is original code and which means it uses a temporary like:
    //
    // apply the visitor to all entries in the map, this will populate the
    // dictionary in the reply message
    // variantMap.visit(VariantVisitor(msg));
    //
    // instead, use msg and populate variant map contents into it. visitor
    // struct can have a pointer to real object and populate maps into it.
    //
    // struct VariantVisitor : ASVariantMap::Visitor
    // {
    //  sd_bus_message * const reply;
    //  ...
    // }

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
} // namespace cxx_variant

// [ RUN      ] CxxType.type_variantMap
// {key1:b}
// {key2:b}
// {key3:i}
// {key4:i}
// {key5:b}
// {key6:b}
// [       OK ] CxxType.type_variantMap (0 ms)

TEST(CxxType, show_variant_map)
{
  using namespace cxx_variant;

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

  // see cxx-string
  // vmap.insert("key5", "variant1");
  // vmap.insert("key6", "variant2");
  //
  // will have:
  //
  // {key5:b}
  // {key6:b}

  vmap.insert("key5", std::string("variant1"));
  vmap.insert("key6", std::string("variant2"));

  vmap.visit(visitor);

  EXPECT_THAT(visitor.coll.size(), 6);

  for (const auto e : visitor.coll)
  {
    cout << e << endl;
  }
}

// ={=========================================================================
// cxx-array

TEST(CxxSize, sizeof)
{
  using namespace use_sizeof;

  // array, sizeof, strlen and valid index:
  //
  //   0   1   2   3   4   5
  //   D   S   M   :   !   \0
  //
  // valid array index     [0, 6) [0, size(), sizeof()) or [0, 5] [0, size()-1]
  // valid cstring index   [0, 5) [0, strlen()) or [0, 4] [0, strlen()-1]

  {
    // cxx.cpp:77:14: warning: deprecated conversion from string constant to
    // ‘char*’ [-Wwrite-strings]
    // char *s1 = "this is first message"; is a pointer
  }

  char s2[]   = "this is first message";
  int arr[20] = {33};

  EXPECT_EQ(sizeof(s2), 22);
  EXPECT_EQ(sizeof s2, 22);

  // "*s2" is object
  EXPECT_EQ(sizeof(*s2), 1);

  // array size
  {
    EXPECT_EQ(sizeof(s2), 22);
    EXPECT_EQ(sizeof(s2) / sizeof(s2[0]), 22);

    EXPECT_EQ(sizeof(arr), 20 * 4);
    EXPECT_EQ(sizeof(arr) / sizeof(arr[0]), 20);
  }

  // strlen do not count '\n'
  EXPECT_EQ(strlen(s2), 21);

  std::string s{s2};
  EXPECT_EQ(s.size(), 21);

  char coll1[100];
  EXPECT_EQ(sizeof(coll1), 100);

  char coll2[] = {1, 2, 3, 4, 5, 6, 7};
  EXPECT_EQ(sizeof(coll2), 7);

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
}

// ={=========================================================================
// cxx-pointer

TEST(CxxPointer, array)
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
  enum class VALUES{E1, E2, E3, E4};
}

TEST(CxxSwitch, show_switch)
{
  {
    ostringstream os;
    VALUES value;

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
// cxx-if

namespace cxx_if
{
  bool f1(std::string &s)
  {
    s += "f1 return true,";
    return true;
  }
  bool f2(std::string &s)
  {
    s += "f2 return true,";
    return true;
  }
  bool f3(std::string &s)
  {
    s += "f3 return true";
    return true;
  }
} // namespace cxx_if

// see that a sequence of calls are called if all returns true
TEST(Cxx, If)
{
  using namespace cxx_if;

  std::string ret{};

  if (!f1(ret))
    std::cout << "f1 return false" << std::endl;
  else if (!f2(ret))
    std::cout << "f2 return false" << std::endl;
  else if (!f3(ret))
    std::cout << "f3 return false" << std::endl;

  EXPECT_THAT(ret, "f1 return true,f2 return true,f3 return true");
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

TEST(Pair, Comparison)
{
  {
    auto p1 = make_pair(1, 2);
    auto p2 = make_pair(3, 2);

    EXPECT_THAT(p1 > p2, false);
    EXPECT_THAT(p1 < p2, true);
  }

  {
    auto p1 = make_pair(1, 2);
    auto p2 = make_pair(1, 3);

    EXPECT_THAT(p1 > p2, false);
    EXPECT_THAT(p1 < p2, true);
  }

  {
    auto p1 = make_pair(13, 2);
    auto p2 = make_pair(13, 4);

    EXPECT_THAT(p1 > p2, false);
    EXPECT_THAT(p1 < p2, true);
  }
}

// ={=========================================================================
// cxx-tuple

// tup1: 41 6.3 nico
// tup2: 22 44 two
// tup1: 41 44 nico
// tup1 is bigger than tup2
// tup1: 22 44 two

TEST(Tuple, MakeTuple)
{
  tuple<int, float, string> tup1{41, 6.3, "nico"};

  cout << "tup1: ";
  cout << get<0>(tup1) << " ";
  cout << get<1>(tup1) << " ";
  cout << get<2>(tup1) << " " << endl;

  auto tup2 = make_tuple(22, 44, "two");

  cout << "tup2: ";
  cout << get<0>(tup2) << " ";
  cout << get<1>(tup2) << " ";
  cout << get<2>(tup2) << " " << endl;
  ;

  get<1>(tup1) = get<1>(tup2);

  cout << "tup1: ";
  cout << get<0>(tup1) << " ";
  cout << get<1>(tup1) << " ";
  cout << get<2>(tup1) << " " << endl;
  ;

  if (tup1 > tup2)
  {
    cout << "tup1 is bigger than tup2" << endl;
    tup1 = tup2;
  }

  cout << "tup1: ";
  cout << get<0>(tup1) << " ";
  cout << get<1>(tup1) << " ";
  cout << get<2>(tup1) << " " << endl;
  ;
}

TEST(Tuple, Tie)
{
  {
    tuple<int, float, string> tup1{41, 6.3, "nico"};
    int i;
    float f;
    string s;

    auto tup = make_tuple(std::ref(i), std::ref(f), std::ref(s));

    // if prints out tup here before assigning value, then do not get tup1's
    // value and see undefined value instead. referece?

    tup = tup1;

    // shows the same

    EXPECT_THAT(get<0>(tup), 41);
    EXPECT_THAT(get<1>(tup), 6.3);
    EXPECT_THAT(get<2>(tup), "nico");

    EXPECT_THAT(i, 41);
    EXPECT_THAT(f, 6.3);
    EXPECT_THAT(s, "nico");

    // changes both tup and i,f,s

    i = 51;
    f = 7.3;
    s = "nico nico";

    EXPECT_THAT(get<0>(tup), 51);
    EXPECT_THAT(get<1>(tup), 7.3);
    EXPECT_THAT(get<2>(tup), "nico nico");

    EXPECT_THAT(i, 51);
    EXPECT_THAT(f, 7.3);
    EXPECT_THAT(s, "nico nico");
  }

  // cxx-tie do the same
  {
    tuple<int, float, string> tup1{41, 6.3, "nico"};
    int i;
    float f;
    string s;

    auto tup = tie(i, f, s);
    tup      = tup1;

    // shows the same

    EXPECT_THAT(get<0>(tup), 41);
    EXPECT_THAT(get<1>(tup), 6.3);
    EXPECT_THAT(get<2>(tup), "nico");

    EXPECT_THAT(i, 41);
    EXPECT_THAT(f, 6.3);
    EXPECT_THAT(s, "nico");

    // changes both tup and i,f,s

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

  // `If defines any other ctor for the class, the compiler do not generates`
  // default ctor. So if a class requires control to init an object in one case,
  // then the class is likely to require control in all cases.

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

// to see that default ctor is necessary

// cause compile error
// cxx.cpp: In constructor
// ‘ConstructionWitNoCtorInitList::ConstructionWitNoCtorInitList()’:
// cxx.cpp:450:37: error: no matching function for call to
// ‘ConstructionNoDefault::ConstructionNoDefault()’
//      ConstructionWitNoCtorInitList() {}

TEST(CxxCtor, NoDefault)
{
  // {
  // using namespace cxx_ctor;
  // Base1 o;
  // }

  {
    using namespace cxx_ctor;
    Base1 o("another ctor");
  }
}

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

  foo f1(value);
  foo f2(value, 30);
}

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

TEST(CxxCtor, CtorCallsCtor)
{
  using namespace cxx_ctor;

  CtorCallsCtor oo("cxx-oo");
  EXPECT_THAT(oo.get_value(), 10);
  EXPECT_THAT(oo.get_name(), "cxx-oo");
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

TEST(CxxCtor, CtorDefaultArgAndInClassInit)
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

  // so have to define all ctors if want to use it like one using default
  // arguments
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

  // now works when changes ctor
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

namespace ctor_init
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
      os_ << mesg_ << " and converting ctor";
    }

    // copy-ctor
    Foo(const Foo &foo)
    {
      mesg_ = foo.mesg_;
      os_ << mesg_ << " and copy ctor";
    }

    // copy-assign
    Foo &operator=(Foo const &foo)
    {
      mesg_ = foo.mesg_;
      os_ << mesg_ << " and copy assign";
      return *this;
    }

    string return_mesg() { return os_.str(); }

  private:
    ostringstream os_;
    string mesg_;
  };
} // namespace ctor_init

TEST(CxxCtor, show_init_forms)
{
  using namespace ctor_init;

  // direct
  {
    // conversion from char * to string before calling ctor
    Foo foo1("use direct init");
    EXPECT_THAT(foo1.return_mesg(), "use direct init and converting ctor");
  }

  // copy for foo2
  {
    // conversion from char * to string before calling ctor
    Foo foo1("use direct init");
    Foo foo2(foo1);
    EXPECT_THAT(foo2.return_mesg(), "use direct init and copy ctor");
  }

  // copy
  {
    Foo foo1("use copy init");
    Foo foo2 = foo1;
    EXPECT_THAT(foo2.return_mesg(), "use copy init and copy ctor");
  }

  // copy, brace init
  {
    Foo foo1{"use copy init"};
    Foo foo2 = foo1;
    EXPECT_THAT(foo2.return_mesg(), "use copy init and copy ctor");
  }

  // WHY?
  {
    Foo foo2 = Foo{"use copy init"};
    EXPECT_THAT(foo2.return_mesg(), "use copy init and converting ctor");
  }

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

namespace ctor_init_converting_ctor
{
  class Foo
  {
  public:
    Foo()
        : mesg_()
    {}

    Foo(const string &mesg)
        : mesg_(mesg)
    {
      os_ << mesg_ << " and string converting ctor";
    }

    Foo(const Foo &foo)
    {
      mesg_ = foo.mesg_;
      os_ << mesg_ << " and copy ctor";
    }

    // added
    Foo(const char *mesg)
        : mesg_(mesg)
    {
      os_ << mesg_ << " and char converting ctor";
    }

    string return_mesg() { return os_.str(); }

  private:
    ostringstream os_;
    string mesg_;
  };
} // namespace ctor_init_converting_ctor

TEST(Ctor, CtorInitFromMore)
{
  using namespace ctor_init_converting_ctor;

  {
    Foo foo2 = "use copy init";
    EXPECT_THAT(foo2.return_mesg(), "use copy init and char converting ctor");
  }
}

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
      os_ << mesg_ << " and string converting ctor";
    }

    explicit Foo(const Foo &foo)
    {
      mesg_ = foo.mesg_;
      os_ << mesg_ << " and copy ctor";
    }

    // added
    explicit Foo(const char *mesg)
        : mesg_(mesg)
    {
      os_ << mesg_ << " and char converting ctor";
    }

    string return_mesg() { return os_.str(); }

  private:
    ostringstream os_;
    string mesg_;
  };
} // namespace ctor_init_explicit

TEST(Ctor, CtorInitFromExplicit)
{
  using namespace ctor_init_explicit;

  // copy
  {
    // conversion from char * to string before calling ctor
    Foo foo1("use direct init");
    Foo foo2(foo1);
    EXPECT_THAT(foo2.return_mesg(), "use direct init and copy ctor");
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

// ={=========================================================================
// cxx-init-list

namespace cxx_init_list
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
      os_ << mesg_ << " and converting ctor";
    }

    Foo(const Foo &foo)
    {
      mesg_ = foo.mesg_;
      os_ << mesg_ << " and copy ctor";
    }

    Foo(std::initializer_list<std::string> values)
    {
      for (auto &e : values)
        os_ << e << ", ";
    }

    string return_mesg() { return os_.str(); }

  private:
    ostringstream os_;
    string mesg_;
  };

  // to show that it can be used in a function and return vector than print out
  // to be used in test

  // *cxx-const* *cxx-temporary* *cxx-reference*

  std::vector<string>
  error_message_1(std::string const &message,
                  std::initializer_list<std::string> const &ls)
  {
    std::vector<string> coll{};

    coll.push_back(message);

    for (auto const &e : ls)
      coll.push_back(e);

    return coll;
  }

  // const is must to use temporary and if not, non-const compile error

  std::vector<int> error_message_2(std::initializer_list<int> const &ls)
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

} // namespace cxx_init_list

TEST(Initialise, List)
{
  using namespace cxx_init_list;

  // copy, brace init
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

namespace cxx_enum
{
  enum color
  {
    red,
    yellow,
    green
  }; // unscoped
  // enum { red, yellow, green } color;       // same as above so cause error

  enum class peppers
  {
    yellow,
    red,
    green
  }; // scoped
} // namespace cxx_enum

TEST(Enum, ScopedAndUnscoped)
{
  using namespace cxx_enum;

  {
    // explicitly use unscoped enumerators and implicitly use unscoped

    int value_1 = color::yellow;
    int value_2 = yellow;
    EXPECT_THAT(value_1, value_2);
  }

  {
    // {
    //   enum color { red, yellow, green };          // unscoped
    //   enum {red, yellow, green};                  // unscoped and unnamed
    // }
    //
    // enum {red, yellow, green}; causes errors with enum color {}:
    //
    // cxx.cpp:1497:9: error: redeclaration of ‘red’
    //    enum {red, yellow, green};                  // unscoped and unnamed
    //          ^~~
    // cxx.cpp:1495:16: note: previous declaration ‘cxx_enum::color red’
    //    enum color { red, yellow, green };          // unscoped
    //                 ^~~
    //
    // {
    //   enum color { red, yellow, green };        // unscoped
    //   enum stoplight { red, yellow, green };    // unscoped
    // } // namespace
    //
    // so can use enumerators of unscoped enum and this allows *cxx-enum-hack*

    int value_1 = yellow;
    int coll[green];
    EXPECT_THAT(8, sizeof(coll));

    color value_2 = yellow;

    EXPECT_THAT(value_1, value_2);
  }

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
  }

  {
    std::cout << "yellow value " << yellow << std::endl;

    // this also confirms that cxx-enum-class do not support cxx-enum-hack
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

TEST(Enum, InClass)
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

TEST(Enum, ScopeAndType)
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

  vector<Snitch> ReturnVector()
  {
    // vector<Snitch> ivec(1000000000, 1);
    // vector(n, elem); creates n elements
    vector<Snitch> ivec(10, Snitch(200));
    cout << "in ReturnVector: size of vector: " << ivec.size() << endl;
    return ivec;
  }

  Snitch createSnitch() { return Snitch(200); }

  void foo(Snitch s) { cout << "snitch value is: " << s.getValue() << endl; }

} // namespace cxx_rvo

TEST(RVO, OnSingle)
{
  using namespace cxx_rvo;

  Snitch snitch = ExampleRVO();
}

TEST(RVO, OnVector)
{
  using namespace cxx_rvo;

  vector<Snitch> ivec = ReturnVector();
}

TEST(RVO, OnArg)
{
  using namespace cxx_rvo;

  foo(Snitch(200));
}

TEST(RVO, OnAssignment)
{
  using namespace cxx_rvo;

  Snitch s = createSnitch();
  cout << "----------" << endl;
  s = createSnitch();
}

// ={=========================================================================
// cxx-reference-const

struct Snitch_X
{ // Note: All methods have side effects
  Snitch_X(int value)
      : value_(value)
  {
    cout << "c'tor" << endl;
  }
  ~Snitch_X() { cout << "d'tor" << endl; }

  Snitch_X(Snitch_X const &) { cout << "copy c'tor" << endl; }
  Snitch_X(Snitch_X &&) { cout << "move c'tor" << endl; }

  Snitch_X &operator=(Snitch_X const &)
  {
    cout << "copy assignment" << endl;
    return *this;
  }

  Snitch_X &operator=(Snitch_X &&)
  {
    cout << "move assignment" << endl;
    return *this;
  }

  int getValue() const { return value_; }

private:
  int value_{0};
};

Snitch_X ExampleRVO_X()
{

  Snitch_X sn(100);

  cout << "in example rvo: " << sn.getValue() << endl;

  return sn;
}

TEST(Reference, UseConstReference)
{
  cout << "----------" << endl;
  Snitch_X snitch = ExampleRVO_X();
  cout << "----------" << endl;
}

// ={=========================================================================
// cxx-swap

namespace cxx_swap
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
} // namespace cxx_swap

TEST(CxxSwap, swap)
{
  using namespace cxx_swap;

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

namespace cxx_reference
{
  void func_01(char *buffer)
  {
    sprintf(buffer, "is filled by pointer %d\n", 10);
  }

  void func_02(char *&buffer)
  {
    sprintf(buffer, "is filled by pointer %d\n", 10);
  }

  // cxx.cpp: In function ‘void
  // use_reference_callby::fill_buffer_by_reference(char&)’: cxx.cpp:826:52:
  // error: invalid conversion from ‘char’ to ‘char*’ [-fpermissive]
  //      sprintf(buffer, "is filled by pointer %d\n", 10);
  //                                                     ^

  // void fill_buffer_by_reference(char &buffer)
  // {
  //   sprintf(buffer, "is filled by pointer %d\n", 10);
  // }

} // namespace cxx_reference

TEST(Reference, CString)
{
  using namespace cxx_reference;

  {
    char buffer[100];
    func_01(buffer);

    // *cxx-array* the point is that name of array is "rvalue of char *".
    //
    // error: invalid initialization of non-const reference of type ‘char*&’
    // from an rvalue of type ‘char*’
    //      func_02(buffer);
  }

  {
    char *buffer = new char[100];

    // error: invalid initialization of reference of type ‘char&’ from
    // expression of type ‘char*’
    func_02(buffer);
    func_01(buffer);
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
// cxx-ratio

TEST(Ratio, Ratio)
{
  using FiveThirds = std::ratio<5, 3>;

  // Numerator and denominator
  EXPECT_THAT(FiveThirds::num, 5);
  EXPECT_THAT(FiveThirds::den, 3);

  // Numerator and denominator
  EXPECT_THAT(std::nano::num, 1);
  EXPECT_THAT(std::nano::den, 1000000000LL);
}

// ={=========================================================================
// cxx-time

// The local date and time is: Tue Jun 12 12:49:12 2018
// The local date and time is: Tue Jun 12 12:49:12 2018
// The UTC date and time is: Tue Jun 12 11:49:12 2018

TEST(Time, SystemCall)
{
  // #include <time.h>
  // time_t time(time_t *timep);
  // time_t now = time(0);
  //
  // time() system call returns the number of seconds since the Epoch. i.e., the
  // same value that gettimeofday() returns in the tv_sec field of its tv
  // argument.

  auto now = time(0);

  // The ctime() function provides a simple method of converting a time_t value
  // `into printable form.` The ctime() function automatically accounts for
  // local timezone and DST settings when performing the conversion.
  //
  // Returns pointer to statically allocated string `terminated by newline` and
  // \0 on success, or NULL on error

  cout << "The local date and time is: " << ctime(&now);

  // The gmtime() and localtime() functions convert a time_t value into a
  // so-called brokendown time. The broken-down time is placed in a statically
  // allocated structure whose address is returned as the function result.

  // tm *localtm = localtime(&now);
  auto localtm = localtime(&now);
  cout << "The local date and time is: " << asctime(localtm);

  // tm *gmtm = gmtime(&now);
  auto gmtm = gmtime(&now);
  if (gmtm != nullptr)
  {
    cout << "The UTC date and time is: " << asctime(gmtm);
  }
}

// The local date and time is: Tue Jun 12 14:42:18 2018
// The local date and time is: Tue Jun 12 14:42:28 2018

TEST(Time, SleepFor)
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

// cxx-time-duration-cast
//
// A typical example is code that segments a duration into different units. For
// example, the following code segments a duration of milliseconds into the
// corresponding hours, minutes, seconds, and milliseconds
//
// raw: [2 of 3600/1]::[0 of 60/1]::[55 of 1/1]::[42 of 1/1000]
//      02::00::55::42

namespace cxx_time_duration_case
{
  template <typename Unit, typename Ratio>
  ostream &operator<<(ostream &os, const chrono::duration<Unit, Ratio> &d)
  {
    os << "[" << d.count() << " of " << Ratio::num << "/" << Ratio::den << "]";
    return os;
  }
} // namespace cxx_time_duration_case

TEST(CxxTime, DurationCast)
{
  using namespace cxx_time_duration_case;

  chrono::milliseconds ms(7255042);

  chrono::hours hh = chrono::duration_cast<chrono::hours>(ms);

  // take remians which means take hours out
  chrono::minutes mm =
    chrono::duration_cast<chrono::minutes>(ms % chrono::hours(1));

  chrono::seconds ss =
    chrono::duration_cast<chrono::seconds>(ms % chrono::minutes(1));

  chrono::milliseconds msec =
    chrono::duration_cast<chrono::milliseconds>(ms % chrono::seconds(1));

  chrono::nanoseconds nsec1 =
    chrono::duration_cast<chrono::nanoseconds>(ms % chrono::seconds(1));

  uint32_t nsec2 = ((ms % chrono::seconds(1)).count() * 1000000L);

  ostringstream os;

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

TEST(Time, CronoClockDetails)
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

TEST(Time, SteadyClock)
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

// cxx-time-timepoint

std::string as_string(const std::chrono::system_clock::time_point &tp)
{
  // static convenience function
  // Note also that this convenience function probably will work only for
  // system_clocks, the only clocks that provide an interface for conversions
  // to and from time_t.

  std::time_t time = std::chrono::system_clock::to_time_t(tp);

  // std::string ts = std::ctime(&time);
  std::string ts = std::asctime(gmtime(&time));

  // remove trailing newline
  ts.resize(ts.size() - 1);
  return ts;
}

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
// min  : Tue Sep 21 00:12:44 1677  // this is bigger and different from the
// book max  : Fri Apr 11 23:47:16 2262  // this is bigger and different from
// the book

TEST(Time, Timepoint)
{
  // print the epoch of this clock

  // is equivalent to:
  // std::chrono::time_point<std::chrono::system_clock> tp
  std::chrono::system_clock::time_point tp;

  cout << "epoch: " << as_string(tp) << endl;

  tp = std::chrono::system_clock::now();
  cout << "now  : " << as_string(tp) << endl;

  tp = std::chrono::system_clock::time_point::min();
  cout << "min  : " << as_string(tp) << endl;

  tp = std::chrono::system_clock::time_point::max();
  cout << "max  : " << as_string(tp) << endl;
}

TEST(Time, TimePointArithmetic)
{
  ostringstream os;

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

TEST(Time, ElapsedTime)
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

// [ RUN      ] Static.UnderInheritance
// Foo::createInstance()
// Bar::createInstance()
// Foo::createInstance()
// Bar::createInstance()
// Foo::createInstance()
// [       OK ] Static.UnderInheritance (0 ms)

TEST(CxxStatic, static_in_inheritance)
{
  using namespace cxx_static;

  Foo::createInstance();
  Bar::createInstance();

  Foo foo;
  foo.createInstance();

  Bar bar;
  bar.createInstance();

  // see foo's version since it is not virtual
  Foo *p = new Bar;
  p->createInstance();
  delete p;
}

namespace cxx_static
{
  class FooStatic2
  {
  public:
    FooStatic2() {}

    static int createInstance()
    {
      static int m_count = 0;

      m_count++;

      // std::cout << "FooStatic2::createInstance(): count: " << m_count
      //           << std::endl;

      return m_count;
    }
  };
} // namespace cxx_static

// see that static variable is initialised once when it is created.

TEST(CxxStatic, static_initialised_once)
{
  using namespace cxx_static;

  EXPECT_THAT(1, FooStatic2::createInstance());
  EXPECT_THAT(2, FooStatic2::createInstance());
  EXPECT_THAT(3, FooStatic2::createInstance());
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
// cxx-function-object

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

namespace cxx_function
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

} // namespace cxx_function

// on member function.
//
// NOTE on syntax since "has to specify the target". For example, update_10()
// takes no argument but has to say:
//
// std::function<void(Foo &)> op = &Foo::update_10;
//                    ^^^^^

TEST(CxxCallable, std_function)
{
  using namespace cxx_function;

  // expect that nullptr if not set
  {
    Foo foo = Foo(100);
    std::function<void(Foo &)> op;

    if (op == nullptr)
      EXPECT_THAT(true, true);
    else
      EXPECT_THAT(true, false);
  }

  // when use std::function, have to speficy `target`
  {
    Foo foo                       = Foo(100);
    std::function<void(Foo &)> op = &Foo::update_10;

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

    op(foo);

    EXPECT_THAT(foo.get_value(), 110);
  }

  // the signatuer to std::function specify whether the argument is copy or
  // reference
  {
    // use reference and copy
    {
      Foo foo{100};

      std::function<void(Foo &)> fo1 = &Foo::update_20;
      fo1(foo);
      EXPECT_THAT(foo.get_value(), 120);

      // if use reference, should be 140
      std::function<void(Foo)> fo2 = &Foo::update_20;
      fo2(foo);
      EXPECT_THAT(foo.get_value(), 120);
    }

    // use reference
    {
      vector<Foo> coll = {Foo(1), Foo(2), Foo(3)};
      vector<size_t> result{};

      std::function<void(Foo &)> op = &Foo::update_20;

      for_each(coll.begin(), coll.end(), op);

      // to get result out and algo-transform requires unary predicate
      transform(coll.begin(), coll.end(), back_inserter(result), print_value);

      EXPECT_THAT(result, ElementsAre(21, 22, 23));
    }

    // use copy so not updated
    {
      vector<Foo> coll = {Foo(1), Foo(2), Foo(3)};
      vector<size_t> result{};

      std::function<void(Foo)> op = &Foo::update_20;

      for_each(coll.begin(), coll.end(), op);

      // to get result out and cxx-transform requires unary predicate
      transform(coll.begin(), coll.end(), back_inserter(result), print_value);

      EXPECT_THAT(result, ElementsAre(1, 2, 3));
    }
  }
}

// *cxx-bind*

namespace cxx_function
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
} // namespace cxx_function

TEST(CxxCallable, std_bind)
{
  using namespace cxx_function;

  // use library defined callable
  {
    std::plus<int> fo;
    EXPECT_THAT(fo(10, 20), 30);
  }

  // cxx-error: no matching function for call to ‘std::plus<int>::plus(int, int)’
  // {
  //   EXPECT_THAT(std::plus<int>(10, 20), 30);
  // }

  // cxx-min, template function
  {
    EXPECT_THAT(std::min(10, 20), 10);
  }

  // expect true if size >= 6
  {
    auto fo = std::bind(check_size, std::placeholders::_1, 6);

    EXPECT_THAT(fo("hello"), false);
    EXPECT_THAT(fo("hello!"), true);
  }

  // show when bind() composites
  {
    std::vector<std::string> coll{"bind", "do", "wonders"};

    int size{6};

    auto found1 =
      std::find_if(coll.cbegin(), coll.cend(), [size](std::string const &e) {
        return e.size() >= size;
      });

    auto found2 =
      std::find_if(coll.cbegin(), coll.cend(), [](std::string const &e) {
        return e.size() >= 6;
      });

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
    set<int, greater<int>> coll1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    deque<int> coll2;

    // initialised
    EXPECT_THAT(coll1, ElementsAre(9, 8, 7, 6, 5, 4, 3, 2, 1));

    // *algo-transform*
    transform(coll1.cbegin(),
              coll1.cend(),                     // source
              back_inserter(coll2),             // destination
              bind(multiplies<int>(), _1, 10)); // unary operation

    // transformed
    EXPECT_THAT(coll2, ElementsAre(90, 80, 70, 60, 50, 40, 30, 20, 10));

    // algo-replace-if
    replace_if(coll2.begin(),
               coll2.end(),                   // range
               bind(equal_to<int>(), _1, 70), // criterion
               42);                           // new value

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

    coll2.erase(remove_if(coll2.begin(),
                          coll2.end(),
                          bind(logical_and<bool>(),
                               bind(greater_equal<int>(), _1, 50),
                               bind(less_equal<int>(), _1, 80))),
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

// on member functions
TEST(CxxCallable, std_bind_and_function)
{
  using namespace cxx_function;

  // when use std::function<>
  {
    Foo foo{100};
    std::function<void(Foo &)> fo = &Foo::update_10;

    fo(foo);

    EXPECT_THAT(foo.get_value(), 110);
  }

  // when use cxx-bind
  {
    Foo foo{100};
    auto fo = std::bind(&Foo::update_10, _1);

    // see that pass `foo` object to op
    fo(foo);

    EXPECT_THAT(foo.get_value(), 110);
  }

  // when use cxx-bind and object
  {
    Foo foo{100};
    auto fo = std::bind(&Foo::update_10, &foo);

    // see that no target object
    fo();

    EXPECT_THAT(foo.get_value(), 110);
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

  // cxx-bind use reference
  // shows that bind() internally uses copy of passed arguments. NO. it is
  // becuase when bind() makes function object it binds to copy or reference.
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
  {
    vector<Foo> coll = {Foo(1), Foo(2), Foo(3)};
    vector<size_t> result{};

    auto fo = std::bind(&Foo::update_10, _1);

    {
      for_each(coll.begin(), coll.end(), fo);

      // to get result out and algo-transform requires unary predicate
      transform(coll.begin(), coll.end(), back_inserter(result), print_value);

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

  // cxx-function use reference
  {
    vector<Foo> coll = {Foo(1), Foo(2), Foo(3)};
    vector<size_t> result{};

    // note:
    // void update_10() but why <void(Foo &)>? becuse of *cxx-this* and this
    // `specify` target object

    std::function<void(Foo &)> op = &Foo::update_10;

    {
      for_each(coll.begin(), coll.end(), op);

      // to get result out and algo-transform requires unary predicate
      transform(coll.begin(), coll.end(), back_inserter(result), print_value);

      EXPECT_THAT(result, ElementsAre(11, 12, 13));
    }
  }
}

// Use `cxx-mem-fn` to let the compiler deduce the member's type and to
// generate a callable object. The callable generated by `mem_fn` can be
// called on either an object or a pointer.
//
// mem_fn() needs target but not flexible as std::bind().
//
// std::bind(&foo::closeConsole, this, std::placeholders::_1);
//
// the point is std::bind() is more flexible.

TEST(CxxCallable, std_memfn)
{
  using namespace cxx_function;

  // *cxx-error* since no default ctor for std::mem_fn()
  // {
  //   auto op = std::mem_fn();
  // }

  // still need target
  {
    Foo foo = Foo(100);
    auto op = std::mem_fn(&Foo::update_10);

    op(foo);

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

  // use reference
  {
    vector<Foo> coll = {Foo(1), Foo(2), Foo(3)};
    vector<size_t> result{};

    auto op = std::mem_fn(&Foo::update_10);

    for_each(coll.begin(), coll.end(), op);

    // to get result out and algo-transform requires unary predicate
    transform(coll.begin(), coll.end(), back_inserter(result), print_value);

    EXPECT_THAT(result, ElementsAre(11, 12, 13));
  }
}

namespace cxx_function
{
  class EventLoopPrivate
  {
  public:
    bool invoke(std::function<void()> &&f) const
    {
      if (f)
        f();
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

    // NOTE:
    // std::move() is required when forward calls to another class
    bool invokeImpl(std::function<void()> &&f) const
    {
      // ok:
      // if (f)
      //   f();

      // error: cannot bind ‘std::function<void()>’ lvalue
      // to ‘std::function<void()>&&’
      // m_private->invoke(f);

      // error: cannot bind ‘std::function<void()>’ lvalue
      // to ‘std::function<void()>&&’
      // m_private.invoke(f);

      // ok:
      m_private.invoke(std::move(f));
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
} // namespace cxx_function

TEST(CxxFunctionObject, Case)
{
  using namespace cxx_function;

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

TEST(CxxFunctionObject, Pointer)
{
  using namespace cxx_function;

  // use pointer
  {
    Foo foo = Foo(100);

    // error
    // void (*op)(void);

    // see how to define pointer to member function
    void (Foo::*op)(void);

    // `Unlike ordinary function pointer, no automatic conversion` between a
    // member funtion and a pointer to that function.
    // must explicitly use address-of operator
    // error
    // op = Foo::update_10;

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
    // cxx_function::Foo::update_10()’ cout << "f2 member address " <<
    // Foo::update_10 <<
  }
}

TEST(CxxCallable, lambda_CaptureAndReturn)
{
  {
    auto func = []() {
      std::string value{"this is a callback"};
      return value;
    };

    EXPECT_THAT(func(), "this is a callback");
  }

  // deduced return is integer
  {
    auto func = [] { return 42; };

    EXPECT_THAT(func(), 42);
  }

  // deduced return is double
  {
    auto func = [] { return 42.0; };

    EXPECT_THAT(func(), 42.0);
  }

  // no deduction
  {
    auto func = []() -> double { return 42; };

    EXPECT_THAT(func(), 42.0);
  }

  // CXXSLR-3.1.10 Lambdas
  // [=] means that the outer scope is passed to the lambda by value. Thus, you
  // can `read` but not modify all data that was readable where the lambda was
  // defined.

  {
    int x = 42;
    int y = 42;

    auto func = [x, &y] {
      // std::cout << "x: " << x << std::endl;
      // std::cout << "y: " << y << std::endl;

      ++y;

      // *cxx-error*
      // cxx.cpp:2646:9: error: increment of read-only variable ‘x’
      //        ++x;
      // ++x;
    };

    x = 77;
    func();
    func();

    // when do capture value? the question is wrong since
    // EXPECT_THAT(x, 42); is true inside of lambda

    EXPECT_THAT(x, 77);
    EXPECT_THAT(y, 44);
  }

  // To have a mixture of passing by value and passing by reference, you can
  // declare the lambda as mutable. In that case, objects are passed by value,
  // but inside the function object defined by the lambda, you have write access
  // to the passed value.
  // Ok but `x` do not change so what's the point???

  {
    int x = 42;
    int y = 42;

    auto func = [x, &y]() mutable {
      // std::cout << "x: " << x << std::endl;
      // std::cout << "y: " << y << std::endl;

      ++y;

      // no *cxx-error*
      ++x;
    };

    func();
    func();

    // not 44
    EXPECT_THAT(x, 42);
    EXPECT_THAT(y, 44);
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
// Therefore, the closure type of a capture-less lambda that takes only one
// void* parameter and returns void* could be imagined as:
//
// copied from test_ccon.cpp
namespace
{

  // int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void
  // *(*start_routine) (void *), void *arg);

  static void *thread_func(void *arg)
  {
    const char *s = (char *)arg;
    // cout << "thread id: " << std::this_thread::get_id() << endl;
    printf("%s, sleeps for 10s, getpid=%ld\n", s, getpid());
    sleep(10);

    // return the length of input message
    return (void *)strlen(s);
  }
} // namespace

// shows the same pid but not tid
TEST(CxxCallable, lambda_c_interface)
{
  // static function works
  {
    char coll[] = "Hello world";
    pthread_t t;
    void *res;
    int s;

    EXPECT_THAT(strlen(coll), 11);

    // this works since it's static
    s = pthread_create(&t, NULL, thread_func, (void *)coll);
    if (s != 0)
    {
      printf("pthread_create failed");
      exit(EXIT_FAILURE);
    }

    printf("main pid = %ld\n", getpid());

    s = pthread_join(t, &res);
    if (s != 0)
    {
      printf("pthread_join failed");
      exit(EXIT_FAILURE);
    }

    // get return
    EXPECT_THAT((long)res, 11);
  }

  // use lambda
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
        printf("%s, sleeps for 10s, getpid=%ld\n", s, getpid());
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

    printf("main pid = %ld\n", getpid());

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
    explicit SmartFoo2()
    {
      std::cout << "SmartFoo2()" << std::endl;
      m_name = "smart foo";
    }

    explicit SmartFoo2(const std::string name)
        : m_name(name)
    {
      std::cout << "SmartFoo2(std::string)" << std::endl;
    }

  public:
    ~SmartFoo2() = default;
  };
} // namespace cxx_smart_pointer

TEST(CxxSmartPointer, sp_construct)
{
  using namespace cxx_smart_pointer;

  {
    std::shared_ptr<std::string> pNico(new std::string("nico")); // OK
  }

  {
    std::shared_ptr<std::string> pNico{new std::string("nico")}; // OK
  }

  {
    shared_ptr<string> pNico = std::make_shared<string>("nico"); // OK
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
  // ‘cxx_smart_pointer::SmartFoo::SmartFoo()’ is private within this context
  // SmartFoo foo;
  //
  // new_allocator.h:120:4:
  // error: ‘cxx_smart_pointer::SmartFoo::SmartFoo()’ is private within this context
  // std::shared_ptr<SmartFoo> sp = std::make_shared<SmartFoo>();
  // {
  //   std::shared_ptr<SmartFoo1> foo = std::make_shared<SmartFoo1>(); // OK
  // }

  // cases that have public ctors
  {
    std::shared_ptr<SmartFoo2> foo = std::make_shared<SmartFoo2>(); // OK
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

TEST(CxxSmartPointer, sp_shared_copy)
{
  auto p = make_shared<int>(42);

  // p.use++
  auto q(p);

  // all prints 2
  EXPECT_THAT(p.use_count(), 2);
  EXPECT_THAT(q.use_count(), 2);

  EXPECT_THAT(*p, 42);
  EXPECT_THAT(*q, 42);

  auto r = make_shared<int>(52);

  // q.use++ and r.use--. destroies a object which r pointed.
  r = q;

  // all prints 3
  EXPECT_THAT(p.use_count(), 3);
  EXPECT_THAT(q.use_count(), 3);
  EXPECT_THAT(r.use_count(), 3);

  EXPECT_THAT(*r, 42);
}

// common for shared and unique
TEST(CxxSmartPointer, sp_reset)
{
  // 1. sp, shared structure, and referenced object are *separate* entity.
  //
  // 2. although shared count is 2, q.use_count() return 0 since cxx-sp-code
  // returns 0 when sp is empty(null)
  //
  // EXPECT_THAT(q.use_count(), 0);

  {
    auto p = make_shared<int>(42);

    // use++
    auto q(p);
    auto r(p);

    EXPECT_THAT(p.use_count(), 3);
    EXPECT_THAT(q.use_count(), 3);

    q.reset();

    EXPECT_THAT(p.use_count(), 2);

    // shows q.reset() do independantly
    EXPECT_THAT(q.use_count(), 0);

    EXPECT_THAT(r.use_count(), 2);
  }

  {
    auto p = make_shared<int>(42);

    // use++
    auto q(p);
    auto r(p);

    EXPECT_THAT(p.use_count(), 3);
    EXPECT_THAT(q.use_count(), 3);

    // same as cxx-sp-reset()
    q = nullptr;

    EXPECT_THAT(p.use_count(), 2);
    EXPECT_THAT(q.use_count(), 0);
    EXPECT_THAT(r.use_count(), 2);
  }

  {
    auto p = make_shared<int>(42);

    // use++
    auto q(p);
    auto r(p);

    EXPECT_THAT(p.use_count(), 3);
    EXPECT_THAT(q.use_count(), 3);

    // multiple reset() are fine
    q.reset();
    q.reset();
    q.reset();
    q.reset();

    EXPECT_THAT(p.use_count(), 2);
    EXPECT_THAT(q.use_count(), 0);
    EXPECT_THAT(r.use_count(), 2);
  }

  {
    unique_ptr<int> up{new int(100)};

    EXPECT_TRUE(up);
    EXPECT_THAT(*up, 100);

    up.reset();

    // now, up is nullptr
    EXPECT_FALSE(up);
  }
}

// CXXSLR 5.2 Smart Pointers

TEST(CxxSmartPointer, sp_shared_example1)
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

// common
TEST(CxxSmartPointer, sp_bool)
{
  {
    auto p = make_shared<int>(42);

    // use++
    auto q(p);
    auto r(p);

    EXPECT_THAT(p.use_count(), 3);
    EXPECT_THAT(q.use_count(), 3);

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
    unique_ptr<int> up{new int(100)};

    EXPECT_TRUE(up);

    // cxx-error on gmock matcher
    // EXPECT_THAT(up, nullptr);
    EXPECT_THAT(!!up, true);
    EXPECT_THAT(up != nullptr, true);

    EXPECT_THAT(*up, 100);
  }
}

// ={=========================================================================
// cxx-smart-ptr cxx-up

/*

:10:27: error: use of deleted function ‘std::unique_ptr<_Tp,
  _Dp>::unique_ptr(const std::unique_ptr<_Tp, _Dp>&) [with _Tp =
  std::basic_string<char>; _Dp = std::default_delete<std::basic_string<char> >;
  std::unique_ptr<_Tp, _Dp> = std::unique_ptr<std::basic_string<char> >]’

:12:8: error: use of deleted function ‘std::unique_ptr<_Tp, _Dp>&
std::unique_ptr<_Tp, _Dp>::operator=(const std::unique_ptr<_Tp, _Dp>&) [with _Tp
= std::basic_string<char>; _Dp = std::default_delete<std::basic_string<char> >;
std::unique_ptr<_Tp, _Dp> = std::unique_ptr<std::basic_string<char> >]’

TEST(SharedPointer, UniqueDoNotSupportCopy)
{
  unique_ptr<std::string> p1(new std::string("nico"));

  // *cxx-error*
  unique_ptr<std::string> p2(p1);

  unique_ptr<std::string> p3;

  // *cxx-error*
  p3 = p2;
}

TEST(SharedPointer, UniqueDoNotSupportCopyInitCopyForm)
{
  // cxx.cpp:1696:36: error: conversion from ‘std::string* {aka
  // std::basic_string<char>*}’ to non-scalar type
  // ‘std::unique_ptr<std::basic_string<char> >’ requested

  unique_ptr<std::string> p1 = new string;
}

*/

namespace cxx_sp_shared
{
  int dtor_count{};

  class Foo
  {
  private:
    int id;

  public:
    Foo(int val = 1)
        : id(val)
    {
      // cout << "Foo ctor(" << id << ")" << endl;
    }

    ~Foo()
    {
      dtor_count++;
      // cout << "Foo dtor(" << id << ")" << endl;
    }
  };
} // namespace cxx_sp_shared

TEST(CxxSmartPointer, sp_unique_moveAssign)
{
  // Foo ctor(1)
  // Foo ctor(2)
  // Foo dtor(1)
  // Foo ctor(3)
  // Foo dtor(2)
  // Foo dtor(3)

  {
    using namespace cxx_sp_shared;

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

    p2 = std::move(p3); // p1->F1   , p2->F3, p3->null
    p3 = std::move(p1); // p1->null , p2->F3, p3->F1
    p3 = std::move(p1); // p1->null , p2->F3, p3->null

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

TEST(CxxSmartPointer, sp_unique_SinkSource)
{
  using namespace cxx_sp_shared;

  cout << "call source()" << endl;
  unique_ptr<Foo> up = source();

  cout << "main: owns " << up.get() << endl;

  sink(move(up));

  cout << "main: ends" << endl;
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

TEST(CxxSmartPointer, sp_Deleter)
{
  using namespace cxx_sp_delete;

  // unique shares pointers uses type of deleter
  {
    std::unique_ptr<ClassA, DebugDeleteClassA> up(
      new ClassA("unique with deleter"));

    EXPECT_THAT(up->getMessage(), "unique with deleter");
  }

  // same as the above and another unique_ptr ctor
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
    std::shared_ptr<string> sp1(new std::string("nico on function"),
                                function_delete);
    std::shared_ptr<string> sp2(new std::string("jutta on function"),
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

TEST(CxxBool, LogicalNot)
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
}

TEST(Bool, CheckBoolDefault)
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

TEST(Bool, CheckUsage)
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

// ={=========================================================================
// cxx-stdio

TEST(CxxIO, stdio_input)
{
  {
    int i{};
    double d{};
    string s{};

    // show the same result when use cin. To emulate input:
    stringstream iss("10\n4.0\nThis is a text\n");
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

    // show the same result when use cin. To emulate input:
    stringstream iss("10 4.0 This is a text");
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
    stringstream iss("10\n4.0\nThis is a text\n");
    iss >> i;
    iss >> d;
    iss.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(iss, s);

    EXPECT_EQ(i, 10);
    EXPECT_EQ(d, 4.0);
    EXPECT_EQ(s, "This is a text");
  }

  {
    int i{};
    double d{};
    string s{};
    int i1, i2, i3, i4;
    stringstream iss("10\n4.0\n1 2 3 4\n");
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

TEST(CxxIO, stdio_getline)
{
  {
    int i{};
    double d{};
    string s{};
    // why s is null?
    // "If you read token by token, the newline character is not a special
    // character. In this case, the tokens might contain a newline character."

    stringstream iss("10\n4.0\nThis is a text\n");
    iss >> i;
    iss >> d;
    getline(iss, s);

    EXPECT_EQ(i, 10);
    EXPECT_EQ(d, 4.0);
    EXPECT_EQ(s, "");
  }

  // so use stream-maniplulator ws to remove whitespaces
  {
    int i{};
    double d{};
    string s{};
    stringstream iss("10\n4.0\nThis is a text\n");
    iss >> i;
    iss >> d;
    getline(iss >> ws, s);

    EXPECT_EQ(i, 10);
    EXPECT_EQ(d, 4.0);
    EXPECT_EQ(s, "This is a text");
  }

  {
    string s{};
    stringstream iss("one|two|three");
    vector<string> coll{};

    while (getline(iss, s, '|'))
      coll.push_back(s);

    EXPECT_THAT(coll, ElementsAre("one", "two", "three"));
  }

  // why loop once?
  {
    char buf[100];
    int bufsize = 100;

    stringstream iss("one|two|three");
    vector<string> coll{};

    while (iss.get(buf, bufsize, '|'))
      coll.push_back(buf);

    EXPECT_THAT(coll, ElementsAre("one"));
  }

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

TEST(CxxIO, stdio_special_types)
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

TEST(CxxIO, stdio_manipulators)
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

TEST(CxxIO, stdio_manipulators_float)
{
  double value = 8.809030;
  ostringstream os;
  os << std::fixed << std::setprecision(2) << value;
  EXPECT_THAT(os.str(), "8.81");
}

#if 0
TEST(CxxIO, fstream_wildcard)
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
// cxx-move

namespace cxx_move
{
  // one which uses copy version
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
      // still need to move on std::string
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

  // one which uses move version
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

TEST(CxxMove, move_binding)
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
  }
}

// [ RUN      ] CxxMove.move_signal1
// Move1(const Move &rhs)
// Move1(const Move &rhs)
// Move1(const Move &rhs)
// [       OK ] CxxMove.move_signal1 (1 ms)
// [ RUN      ] CxxMove.move_signal11
// Move1 &operator=(const Move &rhs)
// Move1 &operator=(const Move &rhs)
// [       OK ] CxxMove.move_signal11 (0 ms)

// uses copy
TEST(CxxMove, move_signal1)
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

TEST(CxxMove, move_signal11)
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
TEST(CxxMove, move_signal2)
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

TEST(CxxMove, move_signal22)
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

// Ok, uses Move2 which has real move controls but here uses copy
TEST(CxxMove, move_signal3)
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

// MOVE!
TEST(CxxMove, move_signal4)
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

// `std::move() doesn't itself do any moving`, but merely signal function
// matching or resolution and trigger cxx-overload
TEST(CxxMove, move_signal5)
{
  using namespace cxx_move;

  {
    Move1 m;

    const std::string cname("cmove");
    const int cvalue{10};

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

  // which will be called? no copy or move controls are called
  //
  // *cxx-reference-binding* Can bind `rvalue` to `const-lvalue-reference`
  // assume that the moved is const and that means nothing will be changed so
  // why bother to copy of it? so no copy or move.
  {
    Move1 m1{"m1", 10};
    Move1 m2{"m2", 20};

    // bind `rvalue(moved-from)` to `const-lvalue-reference(moved-to)`
    // void set_move1(const Move1 &m);
    set_move(std::move(m1));
    set_move(m2);

    // void set_move2(Move1 &&m);
    set_move(std::move(m1));

    // cxx-error: cannot bind ‘cxx_move::Move1’ lvalue to ‘cxx_move::Move1&&’
    // set_move2(m2);
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
        : Foo(name), m_middle_name(middle)
      {}

      ~FooFoo() {}

      std::string name() override { return m_name + ":" + m_middle_name; }
  };
} // namespace cxx_sliced_off

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

    Foo(const Foo&) = delete;
    Foo& operator=(const Foo&) = delete;

    virtual std::string name() { return m_name; }
  };

  class FooFoo : public Foo
  {
    private:
      std::string m_middle_name{};

    public:
      FooFoo(const std::string &name, const std::string &middle)
        : Foo(name), m_middle_name(middle)
      {}

      ~FooFoo() {}

      std::string name() override { return m_name + ":" + m_middle_name; }
  };
} // namespace cxx_sliced_off

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

namespace cxx_override
{
  namespace no_override
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
  } // namespace no_override
} // namespace cxx_override

TEST(Override, Condition_1)
{
  using namespace cxx_override::no_override;

  // No override since it do not meet *cxx-override-condition* Hence no vtable
  // update and base version called.
  {
    Derived derived;
    Base *pbase = &derived;
    EXPECT_THAT(pbase->get_value(), 10);
  }

  // cxx.cpp: In member function ‘virtual void
  // Override_Condition_Test::TestBody()’: cxx.cpp:4285:34: error: no matching
  // function for call to ‘cxx_override::no_override::Derived::get_value()’
  //      EXPECT_THAT(pbase->get_value(), 20);
  //
  // Effectively defines a new function in the derived, inner scope, name found
  // and stops lookup. so hide the name in the base in *cxx-name-lookup*
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

TEST(Override, Condition_2)
{
  using namespace cxx_override::no_virtual;

  // No override happens since there is no virtual used.
  {
    Derived derived;
    Base *pbase = &derived;
    EXPECT_THAT(pbase->get_value(), 10);
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

TEST(Override, Condition_3)
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

TEST(Override, PureVirtual)
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

// ={=========================================================================
// cxx-regex

// 14.1 The Regex Match and Search Interface

TEST(CxxRegex, Match)
{
  // find XML/HTML tagged value (using fefault syntax)
  regex reg1("<.*>.*</.*>");
  bool found = regex_match("<tag>value</tag>", reg1);
  EXPECT_EQ(found, true);

  // find XML/HTML tagged value (tags before and after the value must match)
  // R"(<(.*)>.*</\1>)" // equivalent to: "<(.*)>.*</\\1>"
  regex reg2("<(.*)>.*</\\1>");
  found = regex_match("<tag>value</tag>", reg2);
  EXPECT_EQ(found, true);

  // find XML/HTML tagged value (using grep syntax)
  regex reg3("<\\(.*\\)>.*</\\1>", regex_constants::grep);
  found = regex_match("<tag>value</tag>", reg2);
  EXPECT_EQ(found, true);

  // use C string as reg expression (needs explicit cast to regex)
  found = regex_match("<tag>value</tag>", regex("<.*>.*</.*>"));
  EXPECT_EQ(found, true);

  // regex_match() versus regex_search():
  //
  // regex_match() checks whether the *whole* character sequence matches a
  // regular expression.
  //
  // regex_search() checks whether the character sequence *partially* matches a
  // regular expression.

  found = regex_match("XML tag: <tag>value</tag>",
                      regex("<(.*)>.*</\\1>")); // note: fails to match
  EXPECT_EQ(found, false);

  found = regex_search("XML tag: <tag>value</tag>",
                       regex("<(.*)>.*</\\1>")); // matches
  EXPECT_EQ(found, true);

  found = regex_match("XML tag: <tag>value</tag>",
                      regex(".*<(.*)>.*</\\1>.*")); // matches
  EXPECT_EQ(found, true);

  found = regex_search("XML tag: <tag>value</tag>",
                       regex(".*<(.*)>.*</\\1>.*")); // matches
  EXPECT_EQ(found, true);
}

TEST(CxxRegex, MatchFound)
{
  string data{"XML tag: <tag-name>the value</tag-name>."};

  {
    regex rx(R"(<([\w-]+)>(.*)<(\/[\w-]+)>)");

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

// CXXSLR-14.2 Dealing with Subexpressions

TEST(CxxRegex, MatchResult)
{
  string data{"XML tag: <tag-name>the value</tag-name>."};

  // ok
  regex rx(R"(<([\w-]+)>(.*)<\/([\w-]+)>)");

  // ok
  // regex rx("<(.*)>(.*)</(\\1)>");

  // for returned details of the match
  std::smatch m;

  auto found = regex_search(data, m, rx);
  EXPECT_TRUE(found);

  if (found)
  {
    // Regex: <(.*)>(.*)</(\1)>
    // XML tag: <tag-name>the value</tag-name>.
    //           | m[1] | |  m[2] |  | m[3] | |
    // | prefix|             m[0]            | suffix
    //
    // In general, the match_results object contains:
    //
    //  o A sub_match object m[0] for all the matched characters
    //
    //  o A prefix(), a sub_match object that represents all characters before
    //  the first matched character
    //
    //  o A suffix(), a sub_match object that represents all characters after
    //  the last matched character

    EXPECT_THAT(m.empty(), false);

    // size() yields the number of sub_match objects (including m[0]).

    EXPECT_THAT(m.size(), 4);

    // member function str() to yield the matched string as a whole (calling
    // str() or str(0)) or the nth matched substring (calling str(n)), which is
    // empty if no matched substring exists (thus, passing an n greater than
    // size() is valid)
    // that is m[0]

    EXPECT_THAT(m.str(), "<tag-name>the value</tag-name>");

    // member function length() to yield the length of the matched string as a
    // whole (calling length() or length(0)) or the length of the nth matched
    // substring (calling length(n)), which is 0 if no matched substring exists
    // (thus, passing an n greater than size() is valid)

    EXPECT_THAT(m.length(), 30);

    // member function position() to yield the position of the matched string as
    // a whole (calling position() or position(0)) or the position of the nth
    // matched substring (calling length(n))

    EXPECT_THAT(m.position(), 9);
    EXPECT_THAT(m.prefix().str(), "XML tag: ");
    EXPECT_THAT(m.suffix().str(), ".");

    EXPECT_THAT(m[1].str(), "tag-name");
    EXPECT_THAT(m.str(1), "tag-name");
    EXPECT_THAT(m.position(1), 10);

    EXPECT_THAT(m[2].str(), "the value");
    EXPECT_THAT(m.str(2), "the value");
    EXPECT_THAT(m.position(2), 19);

    EXPECT_THAT(m[3].str(), "tag-name");
    EXPECT_THAT(m.str(3), "tag-name");
    EXPECT_THAT(m.position(3), 30);

    // use iterator

    ostringstream os;

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

// ={=========================================================================
// cxx-bit cxx-numeric-limit
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

TEST(CxxBit, bitset)
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
    bitset<32> coll{value};
    EXPECT_EQ(coll.to_string(), "00000000000000000000010000000000");
  }

  // note: can use variable to set size of bitset.
  {
    const int x = 40;
    bitset<x> coll(1U);
    EXPECT_THAT(coll.size(), 40);
  }

  {
    bitset<32> bitvec(1U);
    EXPECT_EQ(bitvec.to_string(), "00000000000000000000000000000001");

    EXPECT_EQ(bitvec.any(), true);
    EXPECT_EQ(bitvec.none(), false);
    EXPECT_EQ(bitvec.all(), false);
    EXPECT_EQ(bitvec.count(), 1);
    EXPECT_EQ(bitvec.size(), 32);

    bitvec.flip();
    EXPECT_EQ(bitvec.count(), 31);
    bitvec.reset();
    EXPECT_EQ(bitvec.count(), 0);
    bitvec.set();
    EXPECT_EQ(bitvec.count(), 32);

    bitset<16> bitvec2("01011001011");
    EXPECT_EQ(bitvec2.to_string(), "0000001011001011");
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
    bitset<16> bitset11{short11};
    EXPECT_EQ(bitset11.to_string(), "0000010000000000");

    unsigned short short12 = short11 >> 1; // 512
    bitset<16> bitset12{short12};
    EXPECT_EQ(bitset12.to_string(), "0000001000000000");

    unsigned short short13 = short11 >> 10; // 1
    bitset<16> bitset13{short13};
    EXPECT_EQ(bitset13.to_string(), "0000000000000001");

    unsigned short short14 = short11 >> 11; // 0
    bitset<16> bitset14{short14};
    EXPECT_EQ(bitset14.to_string(), "0000000000000000");
  }
}

// bool vector < bitset and bit array < bool array

TEST(CxxBit, bit_SizeConsideration)
{
  const int size{1000};

  vector<bool> coll_bool_vector(size, 1);
  bitset<size> coll_bitset(1U);
  int coll_bit_array[size / 32 + 1];
  bool coll_bool_array[size];

  EXPECT_THAT(coll_bool_vector.size(), size);

#if __GNUC__ && __x86_64__
  EXPECT_THAT(sizeof(coll_bool_vector), 72); // 64 bits
#else
  EXPECT_THAT(sizeof(coll_bool_vector), 1); // 32 bits, need to update
#endif

  EXPECT_THAT(coll_bitset.size(), size);
#if __GNUC__ && __x86_64__
  EXPECT_THAT(sizeof(coll_bitset), 128); // 64 bits
#else
  EXPECT_THAT(sizeof(coll_bitset), 1);      // 32 bits
#endif

  // 1000/32 = 31, 31+1 = 32, 32*4 = 128
  EXPECT_THAT(sizeof(coll_bit_array), 128);

  EXPECT_THAT(sizeof(coll_bool_array), size);
}

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

// Programming Pearl, Column 01, Q 02
// How would you implement bit vectors using bitwise logical operations?
//
// C and old C++ programs usually use type long for arrays of bits and
// manipulate them with the bit operators, such as &, |, and ~.

namespace bit_set_array
{
  const unsigned int BITSPERWORD = 32;
  const unsigned int SHIFT       = 5;
  const unsigned int MASK        = 0x1F;
  const unsigned int SIZE        = 60;

  // bit vector to represent a array of bits. Why +1? Since 0-31 bits falls to
  // array[0] and 32-63 falls to array[1], and so on. SIZE is num of bits to
  // represent and BITSPERWORD is num of bits of a word(int). So there should be
  // one array at least for 0-31 bits.

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
} // namespace bit_set_array

TEST(CxxBit, bit_BitVectors)
{
  using namespace bit_set_array;

  auto array_size = sizeof(a) / sizeof(a[0]);

  EXPECT_THAT(array_size, 2);

  set_bit(35);
  set_bit(45);
  EXPECT_EQ(test_bit(45), true);

  // 45        35
  // 10000000001000 | 0
  //
  // for (int i = 0; i < (int)array_size; ++i)
  // {
  //   cout << ":" << hex << (a[i]);
  // }
  // cout << endl;

  clear_bit(45);
  EXPECT_EQ(test_bit(35), true);
  EXPECT_EQ(test_bit(45), false);

  // 35
  // 1000 | 0

  // for (int i = 0; i < (int)array_size; ++i)
  // {
  //   cout << ":" << hex << (a[i]);
  // }
  // cout << endl;
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

// ={=========================================================================
// cxx-max

TEST(MinMax, Order)
{
  auto value1 = max(10, 20);
  auto value2 = max(20, 10);

  EXPECT_TRUE(value1 == value2);
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

TEST(IntegerDivision, FahrenheitCelsius)
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

TEST(IntegerDivision, FahrenheitCelsius_Float)
{
  float fah{}, cel{};
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

// value:  10 perce: 6000
// value:  40 perce: 1500
// value: 100 perce: 600
// value: 200 perce: 300
// value: 250 perce: 200
// value: 356 perce: 100
// value: 450 perce: 100
// value: 600 perce: 100
//
// value:  10 perce:   1
// value:  40 perce:   6
// value: 100 perce:  16
// value: 200 perce:  33
// value: 250 perce:  41
// value: 356 perce:  59
// value: 450 perce:  75
// value: 600 perce: 100
//
// value:  10 perce:   0
// value:  40 perce:   0
// value: 100 perce:   0
// value: 200 perce:   0
// value: 250 perce:   0
// value: 356 perce:   0
// value: 450 perce:   0
// value: 600 perce: 100

TEST(IntegerDivision, Percentage)
{
  int total{600}, perce{};
  vector<int> values{10, 40, 100, 200, 250, 356, 450, 600};

  for (auto value : values)
  {
    perce = total / value * 100;
    cout << " value: " << setw(3) << setfill(' ') << value
         << " perce: " << setw(3) << setfill(' ') << perce << endl;
  }

  for (auto value : values)
  {
    perce = (value * 100) / total;
    cout << " value: " << setw(3) << setfill(' ') << value
         << " perce: " << setw(3) << setfill(' ') << perce << endl;
  }

  for (auto value : values)
  {
    perce = (value / total) * 100;
    cout << " value: " << setw(3) << setfill(' ') << value
         << " perce: " << setw(3) << setfill(' ') << perce << endl;
  }
}

// cxx-shift cxx-floor cxx-ceil

TEST(IntegerDivision, Shift)
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

TEST(Template, Function)
{
  using namespace cxx_template;

  EXPECT_THAT(compare(1, 2), -1);
  EXPECT_THAT(compare(2, 1), 1);
  EXPECT_THAT(compare(2, 2), 0);

  vector<int> coll1{1, 2, 3}, coll2{1, 2, 4};
  EXPECT_THAT(compare(coll1, coll2), -1);
  EXPECT_THAT(compare(coll2, coll1), 1);
}

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

  // *cxx-array*
  template <unsigned N, unsigned M>
  int compare(const char (&p1)[N], const char (&p2)[M])
  {
    return internal_strcmp(p1, p2);
  }

  template <>
  int compare(const char *const &p1, const char *const &p2)
  {
    return internal_strcmp(p1, p2);
  }

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

} // namespace cxx_template

TEST(Template, Specialisation)
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
    EXPECT_THAT(compare("mon", "hi"), 1);
    EXPECT_THAT(compare("hi", "mon"), -1);

    // cxx.cpp:2829:33: error: call of overloaded ‘compare(const char [3], const
    // char [3])’ is ambiguous

    // EXPECT_THAT(compare("hi", "hi"), 0);

    const char *p1 = "hi";
    const char *p2 = "mon";
    EXPECT_THAT(compare(p1, p2), -1);
  }

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
}

namespace cxx_template_default
{
  // `This shows how function-object is useful` *cxx-functor*
  // default template argument, the 'type' of callable
  // default function argument, F()

  template <typename T, typename F = less<T>>
  int compare(const T &a, const T &b, F f = F())
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

TEST(CxxTemplate, DefaultTypeArgument)
{
  using namespace cxx_template_default;

  {
    // 1 < 2
    EXPECT_THAT(compare(1, 2), -1);
    // 2 > 1
    EXPECT_THAT(compare(2, 1), 1);
    // 2 == 2
    EXPECT_THAT(compare(2, 2), 0);
  }

  {
    // 1 < 2
    EXPECT_THAT(compare(1, 2, greater<int>()), 1);
    // 2 > 1
    EXPECT_THAT(compare(2, 1, greater<int>()), -1);
    // 2 == 2
    EXPECT_THAT(compare(2, 2, greater<int>()), 0);
  }

  {
    vector<int> coll1{1, 2, 3}, coll2{1, 2, 4};
    EXPECT_THAT(compare(coll1, coll2), -1);
    EXPECT_THAT(compare(coll2, coll1), 1);
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

namespace cxx_template_member
{
  class DebugDelete
  {
  public:
    DebugDelete(ostream &os = cerr)
        : os_(os)
    {}

    template <typename T>
    void operator()(T *p) const
    {
      os_ << "deleting " << typeid(p).name() << ", p = " << p << endl;
      delete p;
    }

  private:
    // *cxx-reference-member
    ostream &os_;
  };

} // namespace cxx_template_member

TEST(Template, MemberTemplate)
{
  using namespace cxx_template_member;

  // output:
  // deleting Pd, p = 0x9be7008
  // deleting Pi, p = 0x9be7018

  {
    double *pd = new double();
    int *pi    = new int();
    DebugDelete d;
    d(pd);
    d(pi);
  }

  // output:
  // deleting PSs, p = 0x1d5e830
  // deleting Pi, p = 0x1d5e850

  {
    unique_ptr<int, DebugDelete> pi(new int, DebugDelete());
    unique_ptr<string, DebugDelete> ps(new string, DebugDelete());
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

TEST(Template, Forward)
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

TEST(Template, ForwardEx)
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

TEST(Template, VariadicSizeofOperator)
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

// [ RUN      ] Template.TypeTrait
// foo() called for integral type
// foo() called for floating type
// [       OK ] Template.TypeTrait (1 ms)

TEST(Template, TypeTrait)
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

// ={=========================================================================
// cxx-const

TEST(Const, NoViaConstReference)
{
  int i{};
  int &ri       = i;
  const int &rc = i; // non-const to const is fine

  ri = 0;

  // cxx.cpp:3312:6: error: assignment of read-only reference ‘rc’
  // rc = 0;
  (void)rc;
}

TEST(Const, NoConstToNonConst)
{
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

namespace const_member_function
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
  //
  // cxx.cpp: In function ‘std::string const_member_function::print_screen(const
  // const_member_function::Screen&)’:
  //
  // cxx.cpp:3359:26: error: passing ‘const const_member_function::Screen’ as
  // ‘this’ argument of ‘std::string
  // const_member_function::Screen::get_message()’ discards qualifiers
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
    // const_member_function::array2d<T, R, C>::at(size_t, size_t) const [with T
    // = int; long unsigned int R = 2ul; long unsigned int C = 3ul; size_t =
    // long unsigned int]’: cxx.cpp:3420:21:   required from ‘void
    // const_member_function::print_array2d(const
    // const_member_function::array2d<T, R, C>&) [with T = int; long unsigned
    // int R = 2ul; long unsigned int C = 3ul]’ cxx.cpp:3439:20:   required from
    // here cxx.cpp:3403:31: error: invalid initialization of reference of type
    // ‘int&’ from expression of type ‘const value_type {aka const int}’
    //           return arr.at(r*C + c);

    // non-const version
    //
    // *cxx-const-error-const* when there's no const version
    //
    // cxx.cpp:3409:21: error: passing ‘const
    // const_member_function::array2d<int, 2ul, 3ul>’ as ‘this’ argument of ‘T&
    // const_member_function::array2d<T, R, C>::at(size_t, size_t) [with T =
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

} // namespace const_member_function

TEST(Const, ForMemberFunction)
{
  using namespace const_member_function;
  {
    Screen screen;
    print_screen(screen);
  }

  {
    array2d<int, 2, 3> a;
    print_array2d(a);
  }
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
  class my_exception : public exception
  {
    virtual const char *what() const throw() { return "my exception happened"; }
  };

  my_exception myex;

} // namespace cxx_except

TEST(Exception, OwnException)
{
  using namespace cxx_except;

  try
  {
    throw myex;
  } catch (exception &e)
  {
    EXPECT_THAT(e.what(), "my exception happened");
  }

  try
  {
    throw myex;
  } catch (exception &e)
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

  class FooAbort
  {
  public:
    ~FooAbort() { throw std::runtime_error("noexcept(true) so expects abort"); }
  };
} // namespace cxx_except

TEST(Exception, Noexcept)
{
  using namespace cxx_except;

  {
    int value{1};

    Foo foo;

    EXPECT_THAT(value, 1);
  }

  {
    int value{1};

    try
    {
      FooNoAbort foo;
    } catch (exception &e)
    {
      // why fails???
      // EXPECT_THAT(e.what(), "noexcept(false) so expects no abort");

      std::ostringstream os;
      os << e.what();
      EXPECT_THAT(os.str(), "noexcept(false) so expects no abort");
    }

    EXPECT_THAT(value, 1);
  }

  // [ RUN      ] Exception.Noexcept
  // terminate called after throwing an instance of 'std::runtime_error'
  //   what():  noexcept(true) so expects abort
  // Aborted

  {
    int value{1};

    try
    {
      FooAbort foo;
    } catch (exception &e)
    {
      std::cout << e.what() << std::endl;
    }

    EXPECT_THAT(value, 1);
  }
}

// ={=========================================================================
// cxx-type-conversion

TEST(TypeConversion, Double)
{
  // double value: 0
  // double value: 0
  // double value is not < or > 0
  {
    double value{0};
    printf("double value: %g\n", value);
    printf("double value: %u\n", value);

    if (value < 0)
      printf("double value is < 0\n");
    else if (value > 0)
      printf("double value is > 0\n");
    else
      printf("double value is not < or > 0\n");
  }
}

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

// ={=========================================================================
// cxx-cpp, macro

namespace cxx_cpp
{
#define dprint(expr) printf(#expr " = %g\n", expr)
#define dprint_string(expr) string coll(#expr " = %g\n")
#define dprint_string_1(expr) string coll(#expr)

#define xstr(s) str(s)
#define str(s) #s
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

    string coll1{str(foo)};
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

    string coll2{xstr(foo)};
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

namespace cxx_cpp
{
#define CHECK_CXX_CPP_DEFINED 1
  // #define CHECK_CXX_CPP_DEFINED 0
} // namespace cxx_cpp

TEST(CxxCpp, check_conditional)
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

  // that is, expect to run EXPECT_THAT(true, true) regardless of if macro is
  // defined.
  //
  // if use
  // #define CHECK_CXX_CPP_DEFINED
  //
  // then see error:
  // :8855:27: error: operator '||' has no left operand
  // #if CHECK_CXX_CPP_DEFINED || 1
  //
  //  control:
  //  -DCHECK_CXX_CPP_DEFINED=0

#if CHECK_CXX_CPP_DEFINED || 1
  EXPECT_THAT(true, true);
#else
  EXPECT_THAT(false, true);
#endif

  // fails when #define CHECK_CXX_CPP_DEFINED 0
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

// ={=========================================================================
// cxx-assert

// cxx_ex.cpp: In instantiation of ‘class Vector<short int, 2>’:
// cxx_ex.cpp:39:19:   required from here
// cxx_ex.cpp:31:2: error: static assertion failed: Vector size is too small!
//   static_assert(Size > 3, "Vector size is too small!");
//   ^
//
// CPP code to demonstrate
// static assertion using static_assert

#include <iostream>
using namespace std;

template <class T, int Size>
class Vector
{
  // Compile time assertion to check if
  // the size of the vector is greater than
  // 3 or not. If any vector is declared whose
  // size is less than 4, the assertion will fail
  static_assert(Size > 3, "Vector size is too small!");

  T m_values[Size];
};

TEST(Assert, StaticAssert)
{
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

// ={=========================================================================
// cxx-class

// so semi-colon(;) at end DO NOT matter

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

TEST(Class, ColonDoesMatter)
{
  using namespace cxx_class;

  ClassA cao;
  cao.getName();

  ClassB cbo;
  cbo.getName();
}

// ={=========================================================================
// cxx-class-nested

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

TEST(Class, Nested_1)
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

TEST(Class, Nested_2)
{
  using namespace cxx_nested_2;

  Outer o;
  o.test();
}

// ={=========================================================================

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
