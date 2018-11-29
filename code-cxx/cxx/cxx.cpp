#include <iostream>
#include <set>
#include <vector>
#include <memory>
#include <chrono>
#include <limits>
#include <thread>
#include <list>
#include <regex>
#include <boost/cast.hpp>

#include "gmock/gmock.h"

using namespace std;
using namespace std::placeholders;
using namespace testing;


// ={=========================================================================
// cxx-size cxx-sizeof

// x86_64/VM
// size of (int) is                 : 4
// size of (unsigned int) is        : 4
// size of (long) is                : 8
// size of (unsigned long) is       : 8
// size of (long int) is            : 8
// size of (unsigned int) is        : 8
// size of (long long) is           : 8
// size of (unsigned long long) is  : 8
// size of (* int) is               : 8
// size of (* unsigned int) is      : 8

TEST(Size, SizeOfTypes)
{
  cout << "size of (int) is                 : " << sizeof (int) << endl;
  cout << "size of (unsigned int) is        : " << sizeof (unsigned int) << endl;

  cout << "size of (long) is                : " << sizeof (long ) << endl;
  cout << "size of (unsigned long) is       : " << sizeof (unsigned long) << endl;

  cout << "size of (long int) is            : " << sizeof (long int) << endl;
  cout << "size of (unsigned long int) is   : " << sizeof (unsigned long int) << endl;

  cout << "size of (long long) is           : " << sizeof (long long) << endl;
  cout << "size of (unsigned long long) is  : " << sizeof (unsigned long long) << endl;

  int *pint;
  unsigned int *puint;

  cout << "size of (* int) is               : " << sizeof pint << endl;
  cout << "size of (* unsigned int) is      : " << sizeof puint << endl;
}

namespace use_sizeof {

struct nlist {
  struct nlist *next;
  char *name;
  char *defn;
};

} // namespace

TEST(Size, Arrays)
{
  using namespace use_sizeof;

  // array, sizeof, strlen and valid index:
  // 
  //   0   1   2   3   4   5 
  //   D   S   M   :   !   \0
  // 
  // valid array index     [0, 6) [0, size(), sizeof()) or [0, 5] [0, size()-1]
  // valid cstring index   [0, 5) [0, strlen()) or [0, 4] [0, strlen()-1]

  char *s1 = "this is first message";
  char s2[] = "this is first message";

  // is a pointer
  EXPECT_EQ(sizeof(s1), 8);
  EXPECT_EQ(sizeof s1, 8);

  // is object
  EXPECT_EQ(sizeof(*s1), 1);

  // is array
  EXPECT_EQ(sizeof(s2), 22);
  EXPECT_EQ(sizeof(s2)/sizeof(s2[0]), 22);

  // strlen
  EXPECT_EQ(strlen(s2), 21);

  string s{s1};
  EXPECT_EQ(s.size(), 21);


  char coll1[100];
  EXPECT_EQ(sizeof(coll1), 100);

  char coll2[] = {1,2,3,4,5,6,7};
  EXPECT_EQ(sizeof(coll2), 7);

  struct nlist *plist;
  EXPECT_EQ(sizeof(plist), 8);
  // 8 or else?
  // Here a pointer points to a 'type' and this is a struct in this case.
  EXPECT_EQ(sizeof(*plist), 24);
}


// ={=========================================================================
// cxx-pair

// {10, X}
// {9, IX}
// {5, V}
// typeid : St16initializer_listIKSt4pairIiPKcEE
//
// pair 1 is bigger
// pair 4 is bigger

TEST(Pair, UsePairType)
{
  // gcc 6.3.0 emits error:
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

  const auto pair_map = {
    make_pair(10, "X"),
    make_pair(9, "IX"),
    make_pair(5, "V")
  };
  
  for(const auto &e : pair_map)
    cout << "{" << e.first << ", " << e.second << "}" << endl;
  
  cout << "typeid : " << typeid(pair_map).name() << endl;
  
  auto pair1 = make_pair(10, 10);
  auto pair2 = make_pair(10, 9);
  
  auto pair3 = make_pair( 9, 11);
  auto pair4 = make_pair(11, 9);
  
  if(pair1 < pair2)
    cout << "pair 2 is bigger" << endl;
  else
    cout << "pair 1 is bigger" << endl;
  
  if(pair3 < pair4)
    cout << "pair 4 is bigger" << endl;
  else
    cout << "pair 3 is bigger" << endl;
}


// cxx-pair-reference cxx-std-ref
//
// val pair {11, 21}
// i and j  {10, 20}      // not changed
// ref pair {11, 21}
// i and j  {11, 21}      // changed

TEST(CxxPair, UsePairWithReference)
{
  int i = 10;
  int j = 20;

  auto val = make_pair(i, j);

  ++val.first;
  ++val.second;

  cout << "val pair {" << val.first << ", " << val.second << "}" << endl;
  cout << "i and j  {" << i << ", " << j << "}" << endl;

  auto ref = make_pair(std::ref(i), std::ref(j));

  ++ref.first;
  ++ref.second;

  cout << "ref pair {" << ref.first << ", " << ref.second << "}" << endl;
  cout << "i and j  {" << i << ", " << j << "}" << endl;
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

  const auto pair_init_01{
    make_pair(10, "X"),
    make_pair(9, "IX"),
    make_pair(5, "V")
  };

  const auto pair_init_02 = {
    make_pair(10, "X"),
    make_pair(9, "IX"),
    make_pair(5, "V")
  };

  // vector that has pairs

  std::vector<std::pair<int,string>> pair_init_03{
    make_pair(10, "X"),
    make_pair(9, "IX"),
    make_pair(5, "V")
  };

  std::vector<std::pair<int,string>> pair_init_04{
    {10, "X"},
    {9, "IX"},
    {5, "V"}
  };

  // vector that has tuples

  std::vector<std::tuple<int, string, int>> tuple_init_01{
    make_tuple(10, "X", 1),
    make_tuple(9, "IX", 2),
    make_tuple(5, "V", 3)
  };

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


// ={=========================================================================
// cxx-tuple

// tup1: 41 6.3 nico 
// tup2: 22 44 two 
// tup1: 41 44 nico 
// tup1 is bigger than tup2
// tup1: 22 44 two 

TEST(Tuple, UseTupleType)
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
  cout << get<2>(tup2) << " " << endl;;

  get<1>(tup1) = get<1>(tup2);

  cout << "tup1: ";
  cout << get<0>(tup1) << " ";
  cout << get<1>(tup1) << " ";
  cout << get<2>(tup1) << " " << endl;;

  if( tup1 > tup2 )
  {
    cout << "tup1 is bigger than tup2" << endl;
    tup1 = tup2;
  }

  cout << "tup1: ";
  cout << get<0>(tup1) << " ";
  cout << get<1>(tup1) << " ";
  cout << get<2>(tup1) << " " << endl;;
}


// tup: 41 6.3 nico 
// tup: 41 6.3 nico 
// tup: 41 6.3 nico 
// tup: 22 44 two 

TEST(Tuple, UseTupleTie)
{
  tuple<int, float, string> tup1{41, 6.3, "nico"};
  int i;
  float f;
  string s;

  tie(i, f, s) = tup1;

  cout << "tup: ";
  cout << get<0>(tup1) << " ";
  cout << get<1>(tup1) << " ";
  cout << get<2>(tup1) << " " << endl;

  cout << "tup: ";
  cout << i << " ";
  cout << f << " ";
  cout << s << " " << endl;

  i = 45;
  f = 7.3;
  s = "nico mom";

  cout << "tup: ";
  cout << get<0>(tup1) << " ";
  cout << get<1>(tup1) << " ";
  cout << get<2>(tup1) << " " << endl;

  tie(i, f, s) = make_tuple(22, 44, "two");

  cout << "tup: ";
  cout << i << " ";
  cout << f << " ";
  cout << s << " " << endl;
}


// ={=========================================================================
// cxx-std-forward
template <typename F, typename T1, typename T2>
void flip(F f, T1 param1, T2 param2)
{
  f(param2, param1);
}

template <typename F, typename T1, typename T2>
void flip_forward(F f, T1 &&param1, T2 &&param2)
{
  f(std::forward<T2>(param2), std::forward<T1>(param1));
}

void f(int value1, int &value2)
{
  cout << "f(" << value1 << ", " << ++value2 << ")" << endl;
}

// f(20, 11)
// withoug ref: i and j  {10, 20}
// f(20, 11)
// with    ref: i and j  {11, 20}
// f(20, 11)
// wit forward: i and j  {11, 20}

TEST(CxxTemplate, UseForward)
{
  int i = 10;
  int j = 20;

  flip(f, i, j);
  cout << "withoug ref: i and j  {" << i << ", " << j << "}" << endl;

  flip(f, std::ref(i), j);
  cout << "with    ref: i and j  {" << i << ", " << j << "}" << endl;

  i = 10;
  j = 20;

  flip_forward(f, i, j);
  cout << "wit forward: i and j  {" << i << ", " << j << "}" << endl;
}


// ={=========================================================================
// cxx-ctor

// o `If defines any other ctor for the class, the compiler do not generates`
//   default ctor. So if a class requires control to init an object in one case,
//   then the class is likely to require control in all cases.

class ConstructionNoDefault
{
  public:
    ConstructionNoDefault(const std::string &name) 
    {
      (void)name;
    }

  private:
    int value_;
};

// cause compile error
// cxx.cpp: In constructor ‘ConstructionWitNoCtorInitList::ConstructionWitNoCtorInitList()’:
// cxx.cpp:450:37: error: no matching function for call to ‘ConstructionNoDefault::ConstructionNoDefault()’
//      ConstructionWitNoCtorInitList() {}

// class ConstructionWitNoCtorInitList
// {
//   public:
//     ConstructionWitNoCtorInitList() {}

//   private:
//     ConstructionNoDefault member;
// };

// TEST(Ctor, CtorInitList)
// {
//   ConstructionWitNoCtorInitList cwo;
// }


// okay

class ConstructionWitCtorInitList
{
  public:
    ConstructionWitCtorInitList()
      : member("construction with init list") {}
 
  private:
    ConstructionNoDefault member;
};

TEST(Ctor, CtorInitList)
{
  ConstructionWitCtorInitList cw;
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
      Foo() : mesg_() {} 

      // mesg_ is updated only here
      Foo(const string &mesg) : mesg_(mesg) 
      {
        os_ << mesg_ << " and converting ctor";
      }

      Foo(const Foo &foo)
      {
        mesg_ = foo.mesg_;
        os_ << mesg_ << " and copy ctor";
      }

      string return_mesg()
      {
        return os_.str();
      }

    private:
      ostringstream os_;
      string mesg_;
  };
} // namespace

TEST(Ctor, CtorInitForms)
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

  // copy error
  // // cxx.cpp:533:16: error: conversion from ‘const char [14]’ to non-scalar type ‘ctor_init::Foo’ requested
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
      Foo() : mesg_() {} 

      Foo(const string &mesg) : mesg_(mesg) 
      {
        os_ << mesg_ << " and string converting ctor";
      }

      Foo(const Foo &foo)
      {
        mesg_ = foo.mesg_;
        os_ << mesg_ << " and copy ctor";
      }

      // added
      Foo(const char *mesg) : mesg_(mesg)
      {
        os_ << mesg_ << " and char converting ctor";
      }

      string return_mesg()
      {
        return os_.str();
      }

    private:
      ostringstream os_;
      string mesg_;
  };
} // namespace

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
      Foo() : mesg_() {} 

      explicit Foo(const string &mesg) : mesg_(mesg) 
      {
        os_ << mesg_ << " and string converting ctor";
      }

      explicit Foo(const Foo &foo)
      {
        mesg_ = foo.mesg_;
        os_ << mesg_ << " and copy ctor";
      }

      // added
      explicit Foo(const char *mesg) : mesg_(mesg)
      {
        os_ << mesg_ << " and char converting ctor";
      }

      string return_mesg()
      {
        return os_.str();
      }

    private:
      ostringstream os_;
      string mesg_;
  };
} // namespace

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
  // // cxx.cpp:533:16: error: conversion from ‘const char [14]’ to non-scalar type ‘ctor_init::Foo’ requested
  // //      Foo foo2 = "use copy init";
  // //                 ^
  // {
  //   Foo foo2 = "use copy init";
  //   EXPECT_THAT(foo2.return_mesg(), "use copy init and copy ctor");
  // }
}


// ={=========================================================================
// cxx-ctor-initialize-list

namespace ctor_init_list
{
  class Foo
  {
    public:
      Foo() : mesg_() {} 

      // mesg_ is updated only here
      Foo(const string &mesg) : mesg_(mesg) 
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

      string return_mesg()
      {
        return os_.str();
      }

    private:
      ostringstream os_;
      string mesg_;
  };
} // namespace

TEST(Ctor, CtorInitFromList)
{
  using namespace ctor_init_list;

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
}


// ={=========================================================================
// cxx-copy-control

namespace copy_control {

class CopyControlBaseUsePrivate {
  public:
    CopyControlBaseUsePrivate() = default;
    ~CopyControlBaseUsePrivate() = default;

  private:
    int value_;
    CopyControlBaseUsePrivate(const CopyControlBaseUsePrivate& base) 
      { (void)base; cout << "copy-ctor: base" << endl; }   // @11
    CopyControlBaseUsePrivate& operator=(const CopyControlBaseUsePrivate& base) 
      { (void)base; cout << "copy-assign: base" << endl; return *this; }
};

class CopyControlDerivedUsePrivate : public CopyControlBaseUsePrivate
{
  public:
    void getShout() { cout << "derived get shout" << endl; };
};


class CopyControlBaseUseDelete {
  public:
    CopyControlBaseUseDelete() = default;
    ~CopyControlBaseUseDelete() = default;

  public:
    CopyControlBaseUseDelete(const CopyControlBaseUseDelete& base) = delete;
    CopyControlBaseUseDelete& operator=(const CopyControlBaseUseDelete& base) = delete;

  private:
    int value_;
};

class CopyControlDerivedUseDelete : public CopyControlBaseUseDelete
{
  public:
    void getShout() { cout << "derived get shout" << endl; };
};

} // namespace

// TEST(CxxCopyControl, UsePrivateAndDelete)
// {
//   CopyControlBaseUsePrivate b1, b2;
// 
//   // cxx.cpp:141:5: error: ‘CopyControlBaseUsePrivate::CopyControlBaseUsePrivate(const CopyControlBaseUsePrivate&)’ is private
//   //      CopyControlBaseUsePrivate(const CopyControlBaseUsePrivate& base) { cout << "copy-ctor: base" << endl; }   // @11
//   //      ^
//   // cxx.cpp:174:34: error: within this context
//   //    CopyControlBaseUsePrivate b3(b1);
// 
//   CopyControlBaseUsePrivate b3(b1);
// 
//   CopyControlDerivedUsePrivate d1, d2;
// 
//   // note: 
//   // since base copy ctor is private and derived copy ctor is deleted.
//   //
//   // cxx.cpp:177:37: error: use of deleted function ‘CopyControlDerivedUsePrivate::CopyControlDerivedUsePrivate(const CopyControlDerivedUsePrivate&)’
//   //    CopyControlDerivedUsePrivate d3(d1);
//   //                                      ^
//   // cxx.cpp:145:7: note: ‘CopyControlDerivedUsePrivate::CopyControlDerivedUsePrivate(const CopyControlDerivedUsePrivate&)’ is implicitly deleted because the default definition would be ill-formed:
//   //  class CopyControlDerivedUsePrivate : public CopyControlBaseUsePrivate
//   //        ^
//   // cxx.cpp:141:5: error: ‘CopyControlBaseUsePrivate::CopyControlBaseUsePrivate(const CopyControlBaseUsePrivate&)’ is private
//   //      CopyControlBaseUsePrivate(const CopyControlBaseUsePrivate& base) { cout << "copy-ctor: base" << endl; }   // @11
//   //      ^
//   // cxx.cpp:145:7: error: within this context
//   //  class CopyControlDerivedUsePrivate : public CopyControlBaseUsePrivate
//   //        ^
//   
//   CopyControlDerivedUsePrivate d3(d1);
// 
//   // cxx.cpp:141:5: error: ‘CopyControlBaseUsePrivate::CopyControlBaseUsePrivate(const CopyControlBaseUsePrivate&)’ is private
//   //      CopyControlBaseUsePrivate(const CopyControlBaseUsePrivate& base) { cout << "copy-ctor: base" << endl; }   // @11
//   //      ^
//   // cxx.cpp:167:34: error: within this context
//   //    CopyControlBaseUsePrivate b3(b1);
//   //                                   ^
// 
//   CopyControlBaseUseDelete b4, b5;
// 
//   // note:
//   // whether they are public or private do not make difference.
//   //
//   // when are private and deleted
//   //
//   // cxx.cpp:170:33: error: within this context
//   //    CopyControlBaseUseDelete b6(b4);
//   //                                  ^
//   // cxx.cpp:170:33: error: use of deleted function ‘CopyControlBaseUseDelete::CopyControlBaseUseDelete(const CopyControlBaseUseDelete&)’
//   // cxx.cpp:151:5: note: declared here
//   //      CopyControlBaseUseDelete(const CopyControlBaseUseDelete& base) = delete;
//   //      ^
//   
//   // when are public and deleted
//   //
//   // cxx.cpp:181:33: error: use of deleted function ‘CopyControlBaseUseDelete::CopyControlBaseUseDelete(const CopyControlBaseUseDelete&)’
//   //    CopyControlBaseUseDelete b6(b4);
//   //                                  ^
//   // cxx.cpp:151:5: note: declared here
//   //      CopyControlBaseUseDelete(const CopyControlBaseUseDelete& base) = delete;
//   //      ^
// 
//   CopyControlBaseUseDelete b6(b4);
// 
//   CopyControlDerivedUseDelete d4, d5;
// 
//   // cxx.cpp:210:36: error: use of deleted function ‘CopyControlDerivedUseDelete::CopyControlDerivedUseDelete(const CopyControlDerivedUseDelete&)’
//   //    CopyControlDerivedUseDelete d6(d4);
//   //                                     ^
//   // cxx.cpp:165:7: note: ‘CopyControlDerivedUseDelete::CopyControlDerivedUseDelete(const CopyControlDerivedUseDelete&)’ is implicitly deleted because the default definition would be ill-formed:
//   //  class CopyControlDerivedUseDelete : public CopyControlBaseUseDelete
//   //        ^
//   // cxx.cpp:165:7: error: use of deleted function ‘CopyControlBaseUseDelete::CopyControlBaseUseDelete(const CopyControlBaseUseDelete&)’
//   // cxx.cpp:158:5: note: declared here
//   //      CopyControlBaseUseDelete(const CopyControlBaseUseDelete& base) = delete;
//   //      ^
// 
//   CopyControlDerivedUseDelete d6(d4);
// }


// ={=========================================================================
// cxx-copy-control-assign

// cxx.cpp: In member function ‘virtual void CopyControl_AssignWithVoidReturn_Test::TestBody()’:
// cxx.cpp:527:6: error: use of deleted function ‘void copy_control_assign::CopyControlBaseUseDelete::operator=(const copy_control_assign::CopyControlBaseUseDelete&)’
//    d2 = d1;
//       ^
// namespace copy_control_assign {
// 
// class CopyControlBaseUseDelete {
//   public:
//     CopyControlBaseUseDelete(int value = 0) : value_(value) {}
//     ~CopyControlBaseUseDelete() = default;
// 
//   public:
//     CopyControlBaseUseDelete(const CopyControlBaseUseDelete& base) = delete;
// 
//     void operator=(const CopyControlBaseUseDelete& base) = delete;
// 
//   public:
//     int value_;
// };
// 
// } // namespace
// 
// TEST(CopyControl, AssignWithVoidReturn)
// {
//   using namespace copy_control_assign;
// 
//   CopyControlBaseUseDelete d1(100);
//   CopyControlBaseUseDelete d2;
// 
//   d2 = d1;
// }


// cxx.cpp: In member function ‘virtual void CopyControl_AssignWithVoidReturn_Test::TestBody()’:
// cxx.cpp:542:10: error: ‘void copy_control_assign_use_private::CopyControlBaseUseDelete::operator=(const copy_control_assign_use_private::CopyControlBaseUseDelete&)’ is private
//      void operator=(const CopyControlBaseUseDelete& base);
//           ^
// cxx.cpp:557:6: error: within this context
//    d2 = d1;
//       ^
// namespace copy_control_assign_use_private {
// 
// class CopyControlBaseUseDelete {
//   public:
//     CopyControlBaseUseDelete(int value = 0) : value_(value) {}
//     ~CopyControlBaseUseDelete() = default;
// 
//   private:
//     CopyControlBaseUseDelete(const CopyControlBaseUseDelete& base);
//     void operator=(const CopyControlBaseUseDelete& base);
// 
//   public:
//     int value_;
// };
// 
// } // namespace
// 
// TEST(CopyControl, AssignWithVoidReturn)
// {
//   using namespace copy_control_assign_use_private;
// 
//   CopyControlBaseUseDelete d1(100);
//   CopyControlBaseUseDelete d2;
// 
//   d2 = d1;
// }


// ={=========================================================================
// cxx-enum

enum class EnumFlags :char { SPORT=1, KIDS=2, MUSIC=4 };

constexpr EnumFlags operator| (EnumFlags lhs, EnumFlags rhs)
{
    // C++PL 220
    // explicit converison is nessary since enum class does not support implicit
    // conversions.
    return static_cast<EnumFlags>(static_cast<char>(lhs)|static_cast<char>(rhs));
}

constexpr EnumFlags operator& (EnumFlags lhs, EnumFlags rhs)
{
    return static_cast<EnumFlags>(static_cast<char>(lhs)&static_cast<char>(rhs));
}

class ScopedEnum {
    public:
        int checkFlags(EnumFlags flag)
        {
            int result{};

            switch(flag)
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

                // warning: case value ‘5’ not in enumerated type ‘EnumFlags’ [-Wswitch]
                case EnumFlags::SPORT|EnumFlags::MUSIC:
                    cout << "has sport and music flag" << endl;
                    result = 3;
                    break;

                default:
                    cout << "has unknown flag" << endl;
                    result = 100;
            }

            return result;
        }
};


TEST(CxxFeaturesTest, UseScopedEnum)
{
    ScopedEnum scoped;

    EXPECT_EQ(0, scoped.checkFlags(EnumFlags::SPORT));
    EXPECT_EQ(1, scoped.checkFlags(EnumFlags::KIDS));
    EXPECT_EQ(2, scoped.checkFlags(EnumFlags::MUSIC));
    EXPECT_EQ(3, scoped.checkFlags(EnumFlags::SPORT|EnumFlags::MUSIC));
    EXPECT_EQ(100, scoped.checkFlags(EnumFlags::SPORT|EnumFlags::KIDS));
    // EXPECT_EQ(100, scoped.checkFlags(200));
}


// ={=========================================================================
// cxx-enum, unscoped enum, enum hack

enum color { red, yellow, green };          // unscoped
enum class peppers { red, yellow, green };  // scoped

TEST(CxxFeaturesTest, UseEnumHack)
{
    int value_1 = color::yellow;

    // error: cannot convert ‘peppers’ to ‘int’ in initialization
    // int value_2 = peppers::red;

    color set_1 = yellow;

    // error: invalid conversion from ‘int’ to ‘color’ [-fpermissive]
    // color set_2 = 2;
    
    EXPECT_EQ(value_1, set_1);
}


// ={=========================================================================
// cxx-rvo

struct Snitch {   // Note: All methods have side effects
    Snitch(int value): value_(value) { cout << "c'tor" << endl; }
    ~Snitch() { cout << "d'tor" << endl; }

    Snitch(const Snitch&) { cout << "copy c'tor" << endl; }
    Snitch(Snitch&&) { cout << "move c'tor" << endl; }

    Snitch& operator=(const Snitch&) {
        cout << "copy assignment" << endl;
        return *this;
    }

    Snitch& operator=(Snitch&&) {
        cout << "move assignment" << endl;
        return *this;
    }

    int getValue() const { return value_;}

    private:
        int value_{0};
};

Snitch ExampleRVO() {

  Snitch sn(100);

  cout << "in example rvo: " << sn.getValue() << endl;

  return sn;
}

TEST(CxxFeaturesTest, UseRVO)
{
    cout << "----------" << endl;
    Snitch snitch = ExampleRVO();
    cout << "----------" << endl;
}


vector<Snitch> ReturnVector() {
    // vector<Snitch> ivec(1000000000, 1);
    // vector(n, elem); creates n elements
    vector<Snitch> ivec(10, Snitch(200));
    cout << "size of vector: " << ivec.size() << endl;
  return ivec;
}

TEST(CxxFeaturesTest, UseRVOReturnBigVector)
{
    cout << "----------" << endl;
    vector<Snitch> ivec = ReturnVector();
    cout << "----------" << endl;
}

void foo(Snitch s) {
    cout << "snitch value is: " << s.getValue() << endl;
}

TEST(CxxFeaturesTest, UseCopyElison)
{
    cout << "----------" << endl;
    foo(Snitch(200));
    cout << "----------" << endl;
}

Snitch createSnitch() {
    return Snitch(200);
}

TEST(CxxFeaturesTest, UseAssignment)
{
    cout << "----------" << endl;
    Snitch s = createSnitch();
    cout << "----------" << endl;
    s = createSnitch();
    cout << "----------" << endl;
}


// ={=========================================================================
// cxx-reference-const

struct Snitch_X {   // Note: All methods have side effects
    Snitch_X(int value): value_(value) { cout << "c'tor" << endl; }
    ~Snitch_X() { cout << "d'tor" << endl; }

    Snitch_X(Snitch_X const&) { cout << "copy c'tor" << endl; }
    Snitch_X(Snitch_X&&) { cout << "move c'tor" << endl; }

    Snitch_X& operator=(Snitch_X const&) {
        cout << "copy assignment" << endl;
        return *this;
    }

    Snitch_X& operator=(Snitch_X&&) {
        cout << "move assignment" << endl;
        return *this;
    }

    int getValue() const { return value_;}

    private:
        int value_{0};
};

Snitch_X ExampleRVO_X() {

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
// cxx-reference-callby

namespace use_reference_callby
{
  void swap_by_value(int x, int y)
  {
    int temp;
    temp = x;
    x = y;
    y = temp;
  }

  void swap_by_pointer(int *x, int *y)
  {
    int temp;
    temp = *x;
    *x = *y;
    *y = temp;
  }

  void swap_by_reference(int &x, int &y)
  {
    int temp;
    temp = x;
    x = y;
    y = temp;
  }

  void fill_buffer_by_pointer(char *buffer)
  {
    sprintf(buffer, "is filled by pointer %d\n", 10);
  }

  // cxx.cpp: In function ‘void use_reference_callby::fill_buffer_by_reference(char&)’:
  // cxx.cpp:826:52: error: invalid conversion from ‘char’ to ‘char*’ [-fpermissive]
  //      sprintf(buffer, "is filled by pointer %d\n", 10);
  //                                                     ^

  // void fill_buffer_by_reference(char &buffer)
  // {
  //   sprintf(buffer, "is filled by pointer %d\n", 10);
  // }

} // namespace

TEST(Reference, CallBy)
{
  using namespace use_reference_callby;

  // no swap
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

  // swap
  // note: no need to pass its address
  {
    int x = 10, y = 20;
    swap_by_reference(x, y);
    EXPECT_EQ(x, 20);
    EXPECT_EQ(y, 10);
  }
}

TEST(Reference, OnCFunctions)
{
  using namespace use_reference_callby;

  {
    char buffer[100];
    fill_buffer_by_pointer(buffer);
    cout << buffer << endl;
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
    AUsePrivateMemberThroughReference() : val_(10) {}

    int &returnReference()
    {
      return val_;
    }

    // cxx.cpp: In member function ‘virtual void Reference_UsePrivateMemberThroughReference_Test::TestBody()’:
    // cxx.cpp:800:9: error: ‘int AUsePrivateMemberThroughReference::val_’ is private
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

    void printValue() const
    {
      std::cout << "value: " << val_ << std::endl;
    }
};


TEST(Reference, AccessOnReference)
{
    AUsePrivateMemberThroughReference firstObjectWithValue10;

    firstObjectWithValue10.printValue();
    int &ref = firstObjectWithValue10.returnReference();
    ref = 100;
    firstObjectWithValue10.printValue();

    // error
    // firstObjectWithValue10.printValue();
    // AUsePrivateMemberThroughReference &ref = firstObjectWithValue10.returnReference();
    // ref.val_ = 100;
    // firstObjectWithValue10.printValue();

    AUsePrivateMemberThroughReference secondObjectWithValue10;
    secondObjectWithValue10.changePrivate(firstObjectWithValue10);
    firstObjectWithValue10.printValue();
}


// ={=========================================================================
// cxx-time

// The local date and time is: Tue Jun 12 12:49:12 2018
// The local date and time is: Tue Jun 12 12:49:12 2018
// The UTC date and time is: Tue Jun 12 11:49:12 2018

TEST(Time, UseConventionalWay)
{
  // time_t now = time(0);
  auto now = time(0);

  cout << "The local date and time is: " << ctime(&now) << endl;

  // tm *localtm = localtime(&now);
  auto localtm = localtime(&now);
  cout << "The local date and time is: " << asctime(localtm) << endl;

  // tm *gmtm = gmtime(&now);
  auto gmtm = gmtime(&now);
  if (gmtm != nullptr)
  {
    cout << "The UTC date and time is: " << asctime(gmtm) << endl;
  }
}

// The local date and time is: Tue Jun 12 14:42:18 2018
// The local date and time is: Tue Jun 12 14:42:28 2018

TEST(DISABLED_Time, UseConventionalWayToShowPitfall)
{
  // tm *localtm = localtime(&now);
  time_t now = time(0);
  auto localtm = localtime(&now);
  cout << "The local date and time is: " << asctime(localtm) << endl;

  std::this_thread::sleep_for(chrono::seconds(10));

  now = time(0);
  localtm = localtime(&now);
  cout << "The local date and time is: " << asctime(localtm) << endl;
}


// ={=========================================================================
// cxx-time-crono
//
// A typical example is code that segments a duration into different units. For
// example, the following code segments a duration of milliseconds into the
// corresponding hours, minutes, seconds, and milliseconds
//
// raw: [2 of 3600/1]::[0 of 60/1]::[55 of 1/1]::[42 of 1/1000]
//      02::00::55::42

template <typename V, typename R>
ostream &operator<<(ostream &os, const chrono::duration<V,R> &d)
{
    os << "[" << d.count() << " of " << R::num << "/" << R::den << "]";
    return os;
}

TEST(Time, UseCronoDurationCast)
{
    chrono::milliseconds ms(7255042);

    chrono::hours   hh = 
      chrono::duration_cast<chrono::hours>(ms);
    chrono::minutes mm = 
      chrono::duration_cast<chrono::minutes>(ms % chrono::hours(1));
    chrono::seconds ss = 
      chrono::duration_cast<chrono::seconds>(ms % chrono::minutes(1));
    chrono::milliseconds msec = 
      chrono::duration_cast<chrono::milliseconds>(ms % chrono::seconds(1));

    cout << "raw: " << hh << "::" << mm << "::"
        << ss << "::" << msec << endl;
    cout << "     " << setfill('0') << setw(2) << hh.count() << "::" 
        << setw(2) << mm.count() << "::"
        << setw(2) << ss.count() << "::"
        << setw(2) << msec.count() << endl;
}

// ={=========================================================================
// cxx-time-crono-clock

// the following function prints the properties of a clock
// C represents clock
template <typename C>
void print_clock_data()
{
    using namespace std;

    cout << "- precision: ";

    // clock::period 
    // Yields the type of the unit type (equivalent to clock::duration::period)
    
    typedef typename C::period P;

    // /usr/include/c++/4.9/ratio
    // typedef ratio<                     1000, 1> kilo;
    
    if( ratio_less_equal<P, std::milli>::value )
    {
        // This class template alias generates a ratio type that is the
        // multiplication of the ratio types R1 and R2.
        // 
        // The resulting type is the same as if ratio_multiply was defined as:
        //
        // template <typename R1, typename R2> 
        // using ratio_multiply = std::ratio < R1::num * R2::num, R1::den * R2::den >;

        typedef typename ratio_multiply<P, std::kilo>::type TT;
        cout << fixed << double(TT::num)/TT::den << " milliseconds" << endl;
    } 
    else
    {
        cout << fixed << double(P::num)/P::den << " seconds" << endl;
    }

    // clock::is_steady 
    // Yields true if the clock is steady
    
    cout << "- is ready: " << boolalpha << C::is_steady << endl;
}

// TN: this is different from CLR result 
//
// system_clock:
// - precision: 0.000001 milliseconds
// - is ready: false
// high_resolution_clock:
// - precision: 0.000001 milliseconds
// - is ready: false
// steady_clock:
// - precision: 0.000001 milliseconds
// - is ready: true

TEST(Time, ShowCronoClockDetails)
{
    cout << "system_clock: " << endl;
    print_clock_data<std::chrono::system_clock>();

    cout << "high_resolution_clock: " << endl;
    print_clock_data<std::chrono::high_resolution_clock>();

    cout << "steady_clock: " << endl;
    print_clock_data<std::chrono::steady_clock>();
}


// ={=========================================================================
// cxx-time-crono-timepoint

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
    ts.resize(ts.size()-1);
    return ts;
}

// epoch: Thu Jan  1 01:00:00 1970
// now  : Thu Apr 12 10:52:00 2018
// min  : Tue Sep 21 00:11:29 1677
// max  : Sat Apr 12 00:47:16 2262

// Note that it’s 1 o’clock rather than midnight. This may look a bit
// surprising, but remember that the conversion to the calendar time with
// ctime() inside asString() takes the time zone into account.  Thus, the UNIX
// epoch used here  which, again, is not always guaranteed to be the epoch of
// the system time  started at 00:00 in Greenwich, UK. In my time zone, Germany,
// it was 1 a.m. at that moment, so in my time zone the epoch started at 1 a.m.
// on January 1, 1970. Accordingly, if you start this program, your output is
// probably different, according to your time zone, even if your system uses the
// same epoch in its system clock.

// epoch: Thu Jan  1 00:00:00 1970
// now  : Thu Apr 12 10:01:32 2018
// min  : Tue Sep 21 00:12:44 1677
// max  : Fri Apr 11 23:47:16 2262

TEST(Time, PrintCronoTimepoint)
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


TEST(Time, UserTimeFacet)
{
  auto now = chrono::system_clock::now();
  time_t t = chrono::system_clock::to_time_t(now);
  tm *tm_now = localtime(&t);

  locale loc;
  const time_put<char> &tp = use_facet<time_put<char>>(loc);

  // %x Locale’s preferred date representation Jul 12 1998
  // 06/12/18
  tp.put(cout , cout, ' ', tm_now, 'x');
  cout << endl;

  // use format string
  // Tuesday 06/12/18 04PM
  string fmt = "%A %x %I%p\n";
  tp.put(cout, cout, ' ', tm_now,
      fmt.c_str(), fmt.c_str()+fmt.size());
}


// ={=========================================================================
// cxx-static

class FooStatic {
    private:
        static const size_t MAX_CODE_LENGTH{4};         // *TN* no define
        static const std::string DIGIT_NOT_FOUND;

        // cause an error
        // static const std::string DIGIT_NOT_FOUND{"*"};
    public:
        FooStatic() {}
};

const std::string FooStatic::DIGIT_NOT_FOUND{"*"};

TEST(Static, DefineStaticOutside)
{
    FooStatic foo;
}

// c++ cookbook, 8.4 Automatically Adding New Class Instances to a Container

class StaticClass
{
  protected:
    int value_{};
    size_t id_{};

    string name_{};
    static list<StaticClass*> instances_;
    static size_t track_id_;

  public:
    StaticClass(int value, string name =  "static class")
      : value_(value), name_(name)
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

list<StaticClass*> StaticClass::instances_;
size_t StaticClass::track_id_ = 0;

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

TEST(Static, TrackClassInstances)
{
  StaticClass sc1(1, "instance 1");
  StaticClass sc2(10, "instance 2");
  StaticClass sc3(100, "instance 3");
  StaticClass::ShowList();
}

// ShowList: 0

TEST(Static, TrackClassInstancesWhenNothingCreated)
{
  StaticClass::ShowList();
}


// ={=========================================================================
// cxx-lambda

// string callback()
// {
//     std::string value{"this is a callback"};
//     return value;
// }

TEST(CxxFeaturesTest, UseLambda)
{
    auto callback = [](){
        std::string value{"this is a callback"};
        return value;
    };

    std::string result = callback();

    cout << "result: " << result << endl;
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
// cxx-function-adaptor

template <typename T>
void PRINT_PERSON_ELEMENTS(T &coll, const string &mesg = "")
{
    cout << mesg << endl;

    for(const auto &e : coll)
        e.PrintName();

    cout << endl;
}
class Person {
    public:
    Person(const string &name) : name_(name) {}
    void PrintName() const { cout << "+" << name_ << endl;}
    void PrintNameWithPrefix(const string &prefix) const { cout << prefix << name_ << endl; }
    void SetName(const string &name) { name_ = name;}
    private:
    string name_;
};

TEST(CxxFeaturesTest, UseFunctionAdaptor)
{
    vector<Person> coll={Person("one"), Person("two"), Person("thr")};
    PRINT_PERSON_ELEMENTS(coll, "init: ");

    cout << "bind: " << endl;
    for_each(coll.begin(), coll.end(),
        bind(&Person::PrintName, _1));
    cout << endl;

    cout << "bind: " << endl;
    for_each(coll.begin(), coll.end(),
        bind(&Person::PrintNameWithPrefix, _1, "*"));
    cout << endl;

    cout << "bind: " << endl;
    for_each(coll.begin(), coll.end(),
        mem_fn(&Person::PrintName));      
    cout << endl;        

    for_each(coll.begin(), coll.end(),
        bind(&Person::SetName, _1, "paul"));      
    PRINT_PERSON_ELEMENTS(coll, "modi: "); 
}


// ={=========================================================================
// cxx-smart-ptr cxx-sp

// shared_ptr<int> clone(int p) {
//   return new int(p);
// }

TEST(SharedPointerShared, Ctors)
{
  {
    shared_ptr<string> pNico(new string("nico"));           // OK
  }
  {
    shared_ptr<string> pNico{new string("nico")};           // OK
  }
  {
    shared_ptr<string> pNico = make_shared<string>("nico"); // OK
  }

  // // NO since requires copy init but shared ptr's ctor are explicit 
  // {
  //   shared_ptr<string> pNico = new string("nico");
  // }

  // // NO
  // {
  //   shared_ptr<int> p (new int(42));
  //   // since ctor is explicit
  //   // cxx.cpp:1739:5: error: no match for ‘operator=’ (operand types are ‘std::shared_ptr<int>’ and ‘int*’)
  //   p = new int(1024);                      
  // }

  // points to '9999999999'
  shared_ptr<string> p4 = make_shared<string>(10, '9');

  // points to empty vector<string>
  auto p6 = make_shared<vector<string>>();
}

TEST(SharedPointerShared, Copy)
{
  auto p = make_shared<int>(42);

  // p.use++
  auto q(p);

  // all prints 2
  EXPECT_THAT(p.use_count(), 2);
  EXPECT_THAT(q.use_count(), 2);

  auto r = make_shared<int>(52);

  // q.use++ and r.use--. destroies a object which r pointed. 
  r = q;

  // all prints 3
  EXPECT_THAT(p.use_count(), 3);
  EXPECT_THAT(q.use_count(), 3);
  EXPECT_THAT(r.use_count(), 3);
}


// :10:27: error: use of deleted function ‘std::unique_ptr<_Tp,
//   _Dp>::unique_ptr(const std::unique_ptr<_Tp, _Dp>&) [with _Tp =
//   std::basic_string<char>; _Dp = std::default_delete<std::basic_string<char> >;
//   std::unique_ptr<_Tp, _Dp> = std::unique_ptr<std::basic_string<char> >]’
// 
// :12:8: error: use of deleted function ‘std::unique_ptr<_Tp, _Dp>&
// std::unique_ptr<_Tp, _Dp>::operator=(const std::unique_ptr<_Tp, _Dp>&) [with _Tp
// = std::basic_string<char>; _Dp = std::default_delete<std::basic_string<char> >;
// std::unique_ptr<_Tp, _Dp> = std::unique_ptr<std::basic_string<char> >]’

// TEST(SharedPointer, UniqueDoNotAllowCopy)
// {
//   unique_ptr<std::string> p1(new std::string("nico"));
//   unique_ptr<std::string> p2(p1);
//   unique_ptr<std::string> p3;
//   p3 = p2;
// }

// TEST(SharedPointer, UniqueDoNotAllowInitCopyForm)
// {
//   // cxx.cpp:1696:36: error: conversion from ‘std::string* {aka
//   // std::basic_string<char>*}’ to non-scalar type
//   // ‘std::unique_ptr<std::basic_string<char> >’ requested
//   //
//   // unique_ptr<std::string> p1 = new string;
//
//   unique_ptr<std::string> p2(new string);
// }

TEST(SharedPointerUnique, OperatorBool)
{
  unique_ptr<int> up{new int(100)};

  EXPECT_TRUE(up);
  EXPECT_THAT(*up, 100);
}

namespace cxx_sp_shared
{
  class Foo 
  {
    private:
      int id;

    public:
      Foo(int val = 1):id(val) { cout << "Foo ctor(" << id << ")" << endl; }
      ~Foo() { cout << "Foo dtor(" << id << ")" << endl; }
  };
} // namespace

// [ RUN      ] CxxFeaturesTest.UseUniquePtrMove
// Foo ctor(1)
// Foo ctor(2)
// Foo ctor(3)
// Foo ctor(4)
// p3 is not null
// Foo dtor(2)
// Foo dtor(1)
// p3 is null
// Foo dtor(4)
// Foo dtor(3)
// [       OK ] CxxFeaturesTest.UseUniquePtrMove (1 ms)

TEST(SharedPointerUnique, UniqueAndMove)
{
  using namespace cxx_sp_shared;

  unique_ptr<Foo> p1(new Foo(1));
  unique_ptr<Foo> p2(new Foo(2));
  unique_ptr<Foo> p3(new Foo(3));
  unique_ptr<Foo> p4(new Foo(4));

  // if (p3)
  //   cout << "p3 is not null" << endl;
  // else
  //   cout << "p3 is null" << endl;

  EXPECT_TRUE(p3);

  p2 = std::move(p3);     // p1->F1   , p2->F3, p3->null
  p3 = std::move(p1);     // p1->null , p2->F3, p3->F1
  p3 = std::move(p1);     // p1->null , p2->F3, p3->null

  EXPECT_FALSE(p3);
 
  // if (p3)
  //   cout << "p3 is not null" << endl;
  // else
  //   cout << "p3 is null" << endl;
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
} // namespace

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

TEST(SharedPointerUnique, UniqueSinkSource)
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
      ClassA(string mesg = {}) : mesg_(mesg) {}
      
      void print_mesg()
      {
        cout << "ClassA: " << mesg_ << endl;
      }

    private:
      string mesg_;
  };

  class DebugDeleteClassA
  {
    public:
      DebugDeleteClassA(ostream &os = cerr) : os_(os) {}

      void operator() (ClassA* p)
      {
        os_ << "deleting " << typeid(p).name() << ", p = " << p << endl;
        delete p;
      }

    private:
      // *cxx-reference-member*
      ostream &os_;
  };


  class DebugDelete
  {
    public:
      DebugDelete(ostream &os = cerr) : os_(os) {}

      // *cxx-template-member*
      template <typename T>
      void operator() (T* p)
      {
        os_ << "deleting " << typeid(p).name() << ", p = " << p << endl;
        delete p;
      }

    private:
      // *cxx-reference-member*
      ostream &os_;
  };

  void delete_mesg(string *str)
  {
    cout << "deleting " << *str << endl;
    delete str;
  }

} // namespace

TEST(SharedPointer, Deleter)
{
  using namespace cxx_sp_delete;

  {
    unique_ptr<ClassA, DebugDeleteClassA> up(new ClassA());
    up->print_mesg();
  }

  {
    unique_ptr<ClassA, DebugDeleteClassA> up(new ClassA(), DebugDeleteClassA());
    up->print_mesg();
  }

  {
    unique_ptr<ClassA, DebugDelete> up(new ClassA(), DebugDelete());
    up->print_mesg();
  }

  {
    shared_ptr<string> sp1(new string("nico"), delete_mesg);
    shared_ptr<string> sp2(new string("jutta"), delete_mesg);
  }

  {
    // *cxx-lambda*
    shared_ptr<string> sp1(new string("nico"), 
        [] (string *str)
        {
          cout << "deleting " << *str << endl;
          delete str;
        });
  }
}

// name: Jutta ,name: Jutta ,name: Nico ,name: Jutta ,name: Nico ,
// name: Jutta ,name: Jutta ,name: Nicolai ,name: Jutta ,name: Nicolai ,
// deleting Nicolai
// deleting Jutta

TEST(SharedPointer, DeleteTime)
{
  using namespace cxx_sp_delete;

  shared_ptr<string> pnico(new string("nico"), delete_mesg);
  shared_ptr<string> pjutta(new string("jutta"), delete_mesg);

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

  for (auto e: coll)
    cout << "name: " << *e << " ,";
  cout << endl;

  // overwrite name
  *pnico = "Nicolai";

  // print again to see changes
  for (auto e: coll)
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

TEST(SharedPointerUnique, DeleteReleaseReset)
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
  class Foo 
  {
    private:
      int id;
    public:
      Foo(int val):id(val) { cout << "Foo ctor(" << id << ")" << endl; }
      ~Foo() { cout << "Foo dtor(" << id << ")" << endl; }
  };
} // namespace

// Again, shows use_count() is not reliable but does match when object is
// deleted.
//
// Foo ctor(1)
// p1.use_count: 2
// p2.use_count: 2
// p1.use_count: 0
// p2.use_count: 1
// p1.use_count: 0
// p2.use_count: 1
// p1.use_count: 0
// p2.use_count: 1
// -----------
// p1.use_count: 0
// p2.use_count: 1
// Foo dtor(1)
// p1.use_count: 0
// p2.use_count: 0
// p1.use_count: 0
// p2.use_count: 0
// p1.use_count: 0
// p2.use_count: 0
// end of main

TEST(SharedPointer, UseCount)
{
  using namespace cxx_sp_use_count;

  shared_ptr<Foo> p1(new Foo(1));
  shared_ptr<Foo> p2(p1);

  cout << "p1.use_count: " << p1.use_count() << endl;
  cout << "p2.use_count: " << p2.use_count() << endl;

  p1 = nullptr;
  cout << "p1.use_count: " << p1.use_count() << endl;
  cout << "p2.use_count: " << p2.use_count() << endl;

  p1 = nullptr;
  cout << "p1.use_count: " << p1.use_count() << endl;
  cout << "p2.use_count: " << p2.use_count() << endl;

  p1 = nullptr;
  cout << "p1.use_count: " << p1.use_count() << endl;
  cout << "p2.use_count: " << p2.use_count() << endl;

  cout << "-----------" << endl;

  cout << "p1.use_count: " << p1.use_count() << endl;
  cout << "p2.use_count: " << p2.use_count() << endl;

  p2 = nullptr;
  cout << "p1.use_count: " << p1.use_count() << endl;
  cout << "p2.use_count: " << p2.use_count() << endl;

  p2 = nullptr;
  cout << "p1.use_count: " << p1.use_count() << endl;
  cout << "p2.use_count: " << p2.use_count() << endl;

  p2 = nullptr;
  cout << "p1.use_count: " << p1.use_count() << endl;
  cout << "p2.use_count: " << p2.use_count() << endl;

  cout << "end of main" << endl;
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
    // ‘void cxx_sp_unique_own_version::pointer_int_trait::deleter(cxx_sp_unique_own_version::pointer_int_trait::pointer)’ 
    // without object
    //
    //           T::deleter(p_);

    static void deleter(pointer p)
    {
      delete p;
    }
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
          : p_(p) {}

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
          p_ = nullptr;
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
        void swap(unique_own &other) const noexcept
        {
          std::swap(p_, other.p_);
        }

        // *cxx-overload-oparator-bool*
        // as with unique_ptr, must have const to avoid const error
        explicit operator bool() const noexcept
        {
          return p_ != nullptr;
        }

        // *cxx-overload-oparator-dereference*
        typename T::value_type& operator*() const noexcept
        { return *p_; }

      private:
        pointer p_;
    };

  template<typename T>

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

} // namespace

TEST(SharedPointerOwn, Unique)
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

    p2 = std::move(p3);     // p1->F1   , p2->F3, p3->null
    p3 = std::move(p1);     // p1->null , p2->F3, p3->F1
    p3 = std::move(p1);     // p1->null , p2->F3, p3->null

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
        explicit shared_own(T * p = nullptr) noexcept
          : p_(p), pcount_(new size_t(1))
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
          p_ = other.p_;
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

            p_ = other.p_;
            pcount_ = other.pcount_;
            ++*pcount_;
          }

          return *this;
        }

        // *cxx-overload-operators*
        T& operator *() const noexcept
        {
          return *p_;
        }

        // *cxx-overload-operators*
        T* operator ->() const noexcept
        {
          return p_;
        }

        explicit operator bool() const noexcept
        {
          return p_ != nullptr;
        }

        size_t use_count()
        { return *pcount_; }

        // swap
        void swap(shared_own &other) noexcept
        {
          std::swap(p_, other.p_);
          std::swap(pcount_, other.pcount_);
        }
        
        // reset
        // use *cxx-tempprary* to delete lhs.
        void reset(T* p = nullptr) noexcept
        {
          shared_own<T>(p).swap(*this);
        }

        //
        T* get() noexcept
        {
          return p_;
        }

      private:
        T *p_;              // pointee
        size_t *pcount_;
    };

  template<typename T>
    void swap(shared_own<T> &lhs, shared_own<T> &rhs)
    {
      lhs.swap(rhs);
    }

} // namespace

TEST(SharedPointerOwn, Shared)
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

TEST(Bool, CheckBoolDefault)
{
  bool value{};
  EXPECT_EQ(value, false);
}

namespace __cxx_check {

void CheckFailed(const char *file, int line, const char *cond,
                          unsigned int v1, unsigned int v2) {
  printf("Sanitizer CHECK failed: %s:%d %s (%lld, %lld)\n", file, line, cond,
                                                            v1, v2);
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

#define CHECK_IMPL(c1, op, c2) \
  do { \
    unsigned int v1 = (unsigned int)(c1); \
    unsigned int v2 = (unsigned int)(c2); \
    if (!(v1 op v2)) \
      CheckFailed(__FILE__, __LINE__, \
        "(" #c1 ") " #op " (" #c2 ")", v1, v2); \
  } while (false)

#define CHECK(a)       CHECK_IMPL((a), !=, 0)
#define CHECK_EQ(a, b) CHECK_IMPL((a), ==, (b))
#define CHECK_NE(a, b) CHECK_IMPL((a), !=, (b))
#define CHECK_LT(a, b) CHECK_IMPL((a), <,  (b))
#define CHECK_LE(a, b) CHECK_IMPL((a), <=, (b))
#define CHECK_GT(a, b) CHECK_IMPL((a), >,  (b))
#define CHECK_GE(a, b) CHECK_IMPL((a), >=, (b))

} // namespace

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
  // Sanitizer CHECK failed: cxx.cpp:1322 ((0 && "Address is not in memory and not in shadow?")) != (0) (0, 0)
  // Sanitizer CHECK failed: cxx.cpp:1325 ((100 != 100)) != (0) (0, 0)
  //
  // CHECK("unable to unmap" && 0);

  CHECK(0 && "Address is not in memory and not in shadow?");
  CHECK("Address is not in memory and not in shadow?" && 0);

  CHECK(100 != 101);
  CHECK(100 != 100);
}


// ={=========================================================================
// cxx-stdio

TEST(Stdio, Input)
{
  {
    int i{}; double d{}; string s{};

    // show the same result when use cin. To emulate input:
    stringstream iss("10\n4.0\nThis is a text\n");
    iss >> i; iss >> d; iss >> s;

    EXPECT_EQ(i, 10); EXPECT_EQ(d, 4.0); EXPECT_EQ(s, "This");
  }

  {
    int i{}; double d{}; string s{};

    // show the same result when use cin. To emulate input:
    stringstream iss("10 4.0 This is a text");
    iss >> i; iss >> d; iss >> s;

    EXPECT_EQ(i, 10); EXPECT_EQ(d, 4.0); EXPECT_EQ(s, "This");
  }

  {
    int i{}; double d{}; string s{};
    stringstream iss("10\n4.0\nThis is a text\n");
    iss >> i; iss >> d;
    iss.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(iss, s);

    EXPECT_EQ(i, 10); EXPECT_EQ(d, 4.0); EXPECT_EQ(s, "This is a text");
  }

  {
    int i{}; double d{}; string s{};
    int i1, i2, i3, i4;
    stringstream iss("10\n4.0\n1 2 3 4\n");
    iss >> i; iss >> d;
    iss >> i1; iss >> i2; iss >> i3; iss >> i4;

    vector<int> coll{i1, i2, i3, i4};

    EXPECT_EQ(i, 10);
    EXPECT_EQ(d, 4.0);
    EXPECT_THAT(coll, ElementsAre(1,2,3,4));
  }
}

TEST(Stdio, GetLine)
{
  {
    int i{}; double d{}; string s{};
    // why s is null? 
    // "If you read token by token, the newline character is not a special
    // character. In this case, the tokens might contain a newline character."

    stringstream iss("10\n4.0\nThis is a text\n");
    iss >> i; iss >> d;
    getline(iss, s);

    EXPECT_EQ(i, 10); EXPECT_EQ(d, 4.0); EXPECT_EQ(s, "");
  }

  // so use stream-maniplulator ws to remove whitespaces
  {
    int i{}; double d{}; string s{};
    stringstream iss("10\n4.0\nThis is a text\n");
    iss >> i; iss >> d;
    getline(iss >> ws, s);

    EXPECT_EQ(i, 10); EXPECT_EQ(d, 4.0); EXPECT_EQ(s, "This is a text");
  }

  {
    string s{};
    stringstream iss("one|two|three");
    vector<string> coll{};

    while (getline(iss , s, '|'))
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


TEST(Stdio, SpecialTypes)
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
  EXPECT_THAT(coll1, ElementsAre(1,0,0,0));
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
  EXPECT_THAT(coll2, ElementsAre(1,0,0,0));
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
  EXPECT_THAT(coll3, ElementsAre(1,2,3,4));
  EXPECT_THAT(iss8.good(), true);
}


TEST(Stdio, Manipulators)
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

  cout << setiosflags(ios::showpos | ios::uppercase) 
    << "value: " << value << ", value: " << -value << endl;

  // address           perms offset  dev   inode       pathname
  // 00400000-00452000 r-xp 00000000 08:02 173521      /usr/bin/dbus-daemon

  // cout.width(8);
  cout.setf(std::ios::left);
  cout << setw(20) << "| start" << 
    setw(20) << "| end" << setw(20) << "| perms" << setw(20) << "| offset" 
    << setw(40) << "| pathname" << endl;

  cout.setf(std::ios::right);
  cout << setw(20) << "| start" << 
    setw(20) << "| end" << setw(20) << "| perms" << setw(20) << "| offset" 
    << setw(40) << "| pathname" << endl;

  cout << setw(20) << right << "| start" << 
    setw(20) << "| end" << setw(20) << "| perms" << setw(20) << "| offset" 
    << setw(40) << "| pathname" << endl;
}


// ={=========================================================================
// cxx-rtti

class RttiBase 
{
  public:
    // to make it polymorphic
    ~RttiBase() {}

  private:
    int id_;
};

class RttiDerived : public RttiBase 
{
  private:
    int value_;
};

TEST(Rtti, UseTypeid)
{
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

TEST(Rtti, UseDynamicCast)
{
  RttiDerived d;

  auto result = dynamic_cast<RttiBase*>(&d);
  EXPECT_TRUE(result != NULL);

  if (RttiBase *bp = dynamic_cast<RttiBase*>(&d))
  {
    (void)bp;
    cout << "derived is a subclass of base" << endl;
  }
  else
    cout << "derived is NOT a subclass of base" << endl;
}


// ={=========================================================================
// cxx-regex

// 14.1 The Regex Match and Search Interface

TEST(Regex, UseMatch)
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

  found = regex_match ("XML tag: <tag>value</tag>",
      regex("<(.*)>.*</\\1>")); // note: fails to match
  EXPECT_EQ(found, false);

  found = regex_search ("XML tag: <tag>value</tag>",
      regex("<(.*)>.*</\\1>")); // matches
  EXPECT_EQ(found, true);

  found = regex_match ("XML tag: <tag>value</tag>",
      regex(".*<(.*)>.*</\\1>.*")); // matches
  EXPECT_EQ(found, true);

  found = regex_search ("XML tag: <tag>value</tag>",
      regex(".*<(.*)>.*</\\1>.*")); // matches
  EXPECT_EQ(found, true);
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

TEST(Bit, MaxNegagiveIsSpecial)
{
  // get max negative, ???_MIN
  int int_min = (~((unsigned int)0) >> 1)+1;

  bitset<32> bitset_int_min(int_min);
  EXPECT_EQ(bitset_int_min.to_string(), "10000000000000000000000000000000");

  // what'd happen when negate ???_MIN?
  int negate_min = -int_min;
  bitset<32> bitset_negate_min(negate_min);
  EXPECT_EQ(bitset_negate_min.to_string(), "10000000000000000000000000000000");
}

TEST(Bit, Tricks)
{
  // get bits which has [5, 0]th bis on. e.g. 0001.1111
  // in this case, do not need to specify unsigned.

  int value{};

  const int POS_TO_TURN_ON=5;
  value =  ~(~0 << POS_TO_TURN_ON);
  EXPECT_THAT(value, 0x1F);
}


TEST(Bit, RightShift)
{
  {
    // fails
    // unsigned int int_max = (~((int)0)) >> 1;
    // int int_max = (~((int)0)) >> 1;
    // 
    // unsigned int val = ((~0) >> 1);
    
    // okays
    // unsigned int val = (((unsigned)~0) >> 1);
    // unsigned int val = ((unsigned)~0 >> 1);    // okay, since cast is higher
    //
    // int int_max = (~((unsigned int)0)) >> 1;
    // unsigned int int_max = (~((unsigned int)0)) >> 1;

    unsigned int uint_max = ~((unsigned int)0);
    unsigned int uint_max_two = (1U << 32) -1;

    int int_max = uint_max >> 1;
    int int_min = int_max + 1;

    bitset<32> bitsetx{int_max};
    EXPECT_EQ(bitsetx.to_string(), "01111111111111111111111111111111");

    EXPECT_EQ(uint_max, numeric_limits<unsigned int>::max());
    EXPECT_EQ(uint_max_two, numeric_limits<unsigned int>::max());

    EXPECT_EQ(int_max, numeric_limits<int>::max());
    EXPECT_EQ(int_min, numeric_limits<int>::min());
  }

  // why is that different?
  {
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

    // input3 works since the result is `independant` but assigned to unsigned.
    // input1 version is shorter version of this.

    unsigned int input1 = ~((unsigned int)0)>>1;
    unsigned int input2 = (~(0)>>1);
    unsigned int input3 = ~0;
    input3 >>=1;

    std::bitset<32> bset1{input1};
    EXPECT_EQ(bset1.to_string(), "01111111111111111111111111111111");

    // this is wrong
    std::bitset<32> bset2{input2};
    EXPECT_EQ(bset2.to_string(), "11111111111111111111111111111111");

    std::bitset<32> bset3{input3};
    EXPECT_EQ(bset3.to_string(), "01111111111111111111111111111111");
  }
}

// Programming Pearl, C 01, Q 02
// How would you implement bit vectors using bitwise logical operations?
//
// C and old C++ programs usually use type long for arrays of bits and
// manipulate them with the bit operators, such as &, |, and ~.

namespace bit_vectors 
{
  const unsigned int BITSPERWORD = 32;
  const unsigned int SHIFT = 5;
  const unsigned int MASK = 0x1F;
  const unsigned int SIZE = 60;

  // bit vector to represent a array of bits. Why +1? Since 0-31 bits falls to
  // array[0] and 32-63 falls to array[1], and so on. SIZE is num of bits to
  // represent and BITSPERWORD is num of bits of a word(int). So there should be
  // one array at least for 0-31 bits. 
  
  int a[1 + SIZE/BITSPERWORD];

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

    a[pos >> SHIFT] |= ( 1 << (pos & MASK));
  }

  void clear_bit(int pos)
  {
    a[pos >> SHIFT] &= ~( 1 << (pos & MASK));
  }

  bool test_bit(int pos)
  {
    return (a[pos >> SHIFT] & ( 1 << (pos & MASK))) ? true : false;
  }
} // namespace

TEST(Bit, BitVectors)
{
  using namespace bit_vectors;

  auto array_size = sizeof(a)/sizeof(a[0]);

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


TEST(Bit, BitSet)
{
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

    unsigned short short12 = short11 >> 1;  // 512
    bitset<16> bitset12{short12};
    EXPECT_EQ(bitset12.to_string(), "0000001000000000");

    unsigned short short13 = short11 >> 10;  // 1
    bitset<16> bitset13{short13};
    EXPECT_EQ(bitset13.to_string(), "0000000000000001");

    unsigned short short14 = short11 >> 11;  // 0
    bitset<16> bitset14{short14};
    EXPECT_EQ(bitset14.to_string(), "0000000000000000");
  }
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
  int bigrand()
  {
    return RAND_MAX*rand() + rand();
  }
} // namespace

TEST(Bit, Overflow)
{
  using namespace bit_overflow;

  for (int i = 0; i < 10; ++i)
    cout << (bigrand() % 100) << endl;
}


// ={=========================================================================
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


typedef bool (*UPDATEFUNC)(int);

bool UpdateProgress(int percent)
{
  cout << flush << "\r" << percent << "% complete...";
  return true;
}

TEST(FunctionPointer, X)
{
  UPDATEFUNC f = UpdateProgress;

  for (long l = 0; l < 100000000; ++l)
  {
    if (l % 1000000 == 0)
      f(l / 1000000);

    for (long x = 0; l < 1000000; ++l)
      x = x; 

    // this_thread::sleep_for(std::chrono::milliseconds{10});
  }

  cout << endl;
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
    cout << 
      " fah: " << setw(3) << setfill(' ') << fah << 
      " cel: " << setw(3) << setfill(' ') << cel << endl;
    cel = 5 * (fah-32) / 9;
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
    cout << 
      " fah: " << setw(3) << setfill(' ') << fah << 
      " cel: " << setw(3) << setfill(' ') << cel << endl;

    // cel = 5 * (fah-32) / 9;
    // cel = 5/9 * (fah-32);        // not okay
    // cel = 5/9 * (fah-32.0);      // not okay
    
    cel = 5/9.0 * (fah-32);      // okay
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

TEST(IntegerDivision, Precentage)
{
  int total{600}, perce{};
  vector<int> values{10,40,100,200,250,356,450,600};

  for (auto value : values)
  {
    perce = total / value * 100;
    cout << 
      " value: " << setw(3) << setfill(' ') << value << 
      " perce: " << setw(3) << setfill(' ') << perce << endl;
  }

  for (auto value : values)
  {
    perce = (value * 100)/total;
    cout << 
      " value: " << setw(3) << setfill(' ') << value << 
      " perce: " << setw(3) << setfill(' ') << perce << endl;
  }

  for (auto value : values)
  {
    perce = (value/total)*100;
    cout << 
      " value: " << setw(3) << setfill(' ') << value << 
      " perce: " << setw(3) << setfill(' ') << perce << endl;
  }
}


// ={=========================================================================
// cxx-shift

TEST(Shift, Precentage)
{
  int value = 9;

  cout << 
    "division: " << (value/2) << endl <<  
    "shift   : " << (value>>1) << endl;

  float result = value/2.0;
  cout << 
    "ceil  : " << ceil(result) << endl <<
    "floor : " << floor(result) << endl;
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
    int compare(const T& a, const T& b)
    {
      if (a < b) return -1;
      if (b < a) return 1;
      return 0;
    }
} // namespace

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

      if (c1 == c2) break;
      if (c1 != c2) return c1 < c2 ? -1 : 1;
    }

    return 0;
  }

  template <unsigned N, unsigned M>
    int compare(const char (&p1) [N], const char (&p2) [M])
    {
      return internal_strcmp(p1, p2);
    }

  template <>
    int compare(const char* const &p1, const char *const &p2)
    {
      return internal_strcmp(p1, p2);
    }

  template <typename T, int size>
    class FileBuf
    {
      public:
        int get_size()
        {
          return sizeof(array_)/sizeof(T); 
        }

      private:
        T array_[size];
    };

  template<int MIN, int MAX>
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

} // namespace

TEST(Template, Specialisation)
{
  using namespace cxx_template;

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
  // `This shows how function-object is useful` *cpp-functor*
  // default template argument, the 'type' of callable 
  // default function argument, F()

  template <typename T, typename F = less<T>>
    int compare(const T& a, const T& b, F f = F())
    {
      if (f(a, b)) return -1;
      if (f(b, a)) return 1;
      return 0;
    }
} // namespace

TEST(Template, FunctionWithDefaultArg)
{
  using namespace cxx_template_default;

  EXPECT_THAT(compare(1, 2), -1);
  EXPECT_THAT(compare(2, 1), 1);
  EXPECT_THAT(compare(2, 2), 0);

  vector<int> coll1{1, 2, 3}, coll2{1, 2, 4};
  EXPECT_THAT(compare(coll1, coll2), -1);
  EXPECT_THAT(compare(coll2, coll1), 1);
}

namespace cxx_template_member 
{
  class DebugDelete
  {
    public:
      DebugDelete(ostream &os = cerr) : os_(os) {}

      template <typename T>
      void operator() (T *p) const
      {
        os_ << "deleting " << typeid(p).name() << ", p = " << p << endl;
        delete p;
      }

    private:

      // *cxx-reference-member
      ostream &os_;
  };

} // namespace

TEST(Template, MemberTemplate)
{
  using namespace cxx_template_member; 

  // output:
  // deleting Pd, p = 0x9be7008
  // deleting Pi, p = 0x9be7018

  {
    double *pd = new double();
    int *pi = new int();
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
  template <typename T>
    typename std::iterator_traits<T>::value_type &
    return_element_01(T first, T last)
    {
      (void)last;
      return *first;
    }

  template <typename T>
    auto return_element_02(T first, T last) -> typename std::iterator_traits<T>::reference
    {
      (void)last;
      return *first;
    }

  template <typename T>
    auto return_element_03(T first, T last) -> decltype(*first)
    {
      (void)last;
      return *first;
    }
} // namespace


// : error: ‘first’ was not declared in this scope
// template <typename Iterator>
// decltype(*first) return_element_04(Iterator first, Iterator last)
// {
//   return *first;
// }

TEST(Template, TypeTraitsIterator)
{
  using namespace cxx_template_return_type;

  vector<int> coll{3,4,5,6};

  EXPECT_THAT(return_element_01(coll.begin(), coll.end()), 3);
  EXPECT_THAT(return_element_02(coll.begin(), coll.end()), 3);
  EXPECT_THAT(return_element_03(coll.begin(), coll.end()), 3);
}


// ={=========================================================================
// cxx-const

TEST(Const, NoViaConstReference)
{
  int i{};
  int &ri = i;
  const int &rc = i;  // non-const to const is fine

  ri = 0;

  // cxx.cpp:3312:6: error: assignment of read-only reference ‘rc’
  // rc = 0;
  (void) rc;
}

TEST(Const, NoConstToNonConst)
{
  {
    const int ci{100};

    // const to non-const
    // cxx-const-error cxx-error: invalid initialization of reference of type
    // ‘int&’ from expression of type ‘const int’

    // int &ri = ci;
    (void) ci;

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
      Screen() : mesg_("screen") {}

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

  string print_screen(const Screen &s)
  {
    return s.get_message();
  }

  template <class T, size_t R, size_t C>
    class array2d
    {
      std::vector<T> arr;

      public:
      array2d() :arr(R*C) {}
      explicit array2d(std::initializer_list<T> l):arr(l) {}

      // *cxx-const-error-const* when there's no const version
      //
      // when use constexpr function
      //  
      //  constexpr T& at(size_t const r, size_t const c)
      //  {
      //     return arr.at(r*C + c);
      //  }
      //
      // cxx.cpp: In instantiation of ‘constexpr T& const_member_function::array2d<T, R, C>::at(size_t, size_t) const [with T = int; long unsigned int R = 2ul; long unsigned int C = 3ul; size_t = long unsigned int]’:
      // cxx.cpp:3420:21:   required from ‘void const_member_function::print_array2d(const const_member_function::array2d<T, R, C>&) [with T = int; long unsigned int R = 2ul; long unsigned int C = 3ul]’
      // cxx.cpp:3439:20:   required from here
      // cxx.cpp:3403:31: error: invalid initialization of reference of type ‘int&’ from expression of type ‘const value_type {aka const int}’
      //           return arr.at(r*C + c);

      // non-const version
      //
      // *cxx-const-error-const* when there's no const version
      //
      // cxx.cpp:3409:21: error: passing ‘const const_member_function::array2d<int, 2ul, 3ul>’ as ‘this’ argument of ‘T& const_member_function::array2d<T, R, C>::at(size_t, size_t) [with T = int; long unsigned int R = 2ul; long unsigned int C = 3ul; size_t = long unsigned int]’ discards qualifiers [-fpermissive]
      //            std::cout << arr.at(i, j) << ' ';
      //
      // T& at(size_t const r, size_t const c)
      // {
      //    return arr.at(r*C + c);
      // }

      // const version covers both const and non-const use
      constexpr T const & at(size_t const r, size_t const c) const 
      {
        return arr.at(r*C + c);
      }
    };

  template <class T, size_t R, size_t C>
    void print_array2d(array2d<T, R, C> const & arr)
    {
      for (int i = 0; i < R; ++i)
      {
        for (int j = 0; j < C; ++j)
        {
          std::cout << arr.at(i, j) << ' ';
        }

        std::cout << std::endl;
      }
    }

} // namespace

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
// cxx-except

namespace cxx_except 
{
  class my_exception : public exception
  {
    virtual const char *what() const throw()
    {
      return "my exception happened";
    }
  };

  my_exception myex;

} // namespace


TEST(Exception, OwnException)
{
  using namespace cxx_except;

  try
  {
    throw myex;
  }
  catch (exception &e)
  {
    EXPECT_THAT(e.what(), "my exception happened");
  }

  try
  {
    throw myex;
  }
  catch (exception &e)
  {
    ostringstream os;
    os << "my exception happened";
    EXPECT_THAT(os.str(), "my exception happened");
  }

  try
  {
    throw myex;
  }
  catch (...)
  {
    ostringstream os;
    os << "my exception happened";
    EXPECT_THAT(os.str(), "my exception happened");
  }

  try
  {
    throw myex;
  }
  catch (...)
  {
  }
}


// ={=========================================================================
// cxx-cpp, macro

// 0: 01
// 0: 012
// 0: 0123
// 0: 01234
// 0: 012345

TEST(Cpp, Precision)
{
  // The format argument of printf can be an expression too.
  {
    // printf((argc>1) ? "%s " : "%s", *++argv );
    int argc = 1;
    printf((argc>1) ? "|%s|\n" : "%s\n", "argc");
  }

  // print unsigned long value
  //
  // ulong max from limits: 18446744073709551615L
  // ulong max from limits: 18446744073709551615
  {
    // here "L" is normal char
    printf("ulong max from limits: %luL\n", ULONG_MAX );
    printf("ulong max from limits: %lu\n", ULONG_MAX );
  }

  {
    char *pmesg = "0123456789";

    printf("0: %.*s \n", 2, pmesg );
    printf("0: %.*s \n", 3, pmesg );
    printf("0: %.*s \n", 4, pmesg );
    printf("0: %.*s \n", 5, pmesg );
    printf("0: %.*s \n", 6, pmesg );
  }

  {
    // MHEGDebugDebug(eMHEGengRuntime,"OS-OctetStringVariable Append %.*s, %.*s -> %.*s\n",
    //     (OctetStringVariableEntry(pEntry).m_osValue).size,
    //     (OctetStringVariableEntry(pEntry).m_osValue).buf,
    //     osValue.size, osValue.buf,
    //     osNewValue.size, osNewValue.buf);
  }
}


#define dprint(expr)    printf(#expr " = %g\n", expr )
#define dprint_string(expr)    string coll(#expr " = %g\n")
#define dprint_string_1(expr)    string coll(#expr)

#define xstr(s) str(s)
#define str(s) #s
#define foo 4

TEST(Cpp, Stringification)
{
  {
    double x = 10.0;
    double y = 3.0;

    // printf( "x/y = %g\n", x/y );
    // x/y = 3.33333
    dprint(x/y);
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
    // is not "4" because not macro-expanded
    string coll1{str (foo)};
    EXPECT_THAT(coll1, "foo");

    // s is stringified when it is used in str, so it is not macro-expanded
    // first. But s is an ordinary argument to xstr, so it is completely
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
  // PDM_DISK_SPINDOWN_LOG( g_pdm_diag_segment_id, ("device %s now in power saving mode", physDev->shDeviceName));
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
  // #define DIAG_LOG_ERROR(id, msg)		DIAG_LOG_F2B_WRITE((id), DIAG_CMN_TRACE_ERROR, msg)
  //
  // How?

  {
#define PDM_DISK_SPINDOWN_LOG           DIAG_LOG_ERROR
#define DIAG_LOG_ERROR(id, msg)         printf msg

    // #define DIAG_LOG_ERROR(id, msg)         printf msg
    // printf "(\"device %s now in power saving mode\", \"physDev->shDeviceName\")";
    //
    // #define DIAG_LOG_ERROR(id, msg)         printf msg
    // printf ("device %s now in power saving mode", "physDev->shDeviceName");

    // PDM_DISK_SPINDOWN_LOG( g_pdm_diag_segment_id, ("device %s now in power saving mode", "physDev->shDeviceName"));
    PDM_DISK_SPINDOWN_LOG( g_pdm_diag_segment_id, ("device %s now in power saving mode\n", "physDev->shDeviceName"));
  }
}


// ={=========================================================================

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

