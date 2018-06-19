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
// cxx-pair

// {10, X}
// {9, IX}
// {5, V}
// typeid : St16initializer_listIKSt4pairIiPKcEE
//
// pair 1 is bigger
// pair 4 is bigger

TEST(CxxPair, UsePairType)
{
    const auto map{
        make_pair(10, "X"),
        make_pair(9, "IX"),
        make_pair(5, "V")
    };

    for(const auto &e : map)
        cout << "{" << e.first << ", " << e.second << "}" << endl;

    cout << "typeid : " << typeid(map).name() << endl;

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


// ={=========================================================================
// cxx-pair-reference cxx-std-ref

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
// cxx-copy-control

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
// cxx-tuple

// tup1: 41 6.3 nico 
// tup2: 22 44 two 
// tup1: 41 44 nico 
// tup1 is bigger than tup2
// tup1: 22 44 two 

TEST(CxxFeaturesTest, UseTupleType)
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

TEST(CxxFeaturesTest, UseTupleTie)
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

TEST(CxxFeaturesTest, UseConstReference)
{
    cout << "----------" << endl;
    Snitch_X snitch = ExampleRVO_X();
    cout << "----------" << endl;
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
// cxx-reference-and-access

// value: 10
// value: 100
// value: 200

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
        
        // error
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


TEST(CxxFeaturesTest, UsePrivateMemberThroughReference)
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

TEST(Time, UseConventionalWayToShowPitfall)
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
// cxx-unique-ptr

class Foo 
{
  private:
    int id;
  public:
    Foo(int val = 1):id(val) { cout << "Foo ctor(" << id << ")" << endl; }
    ~Foo() { cout << "Foo dtor(" << id << ")" << endl; }
};

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

TEST(CxxFeaturesTest, UseUniquePtrMove)
{
  unique_ptr<Foo> p1(new Foo(1));
  unique_ptr<Foo> p2(new Foo(2));
  unique_ptr<Foo> p3(new Foo(3));
  unique_ptr<Foo> p4(new Foo(4));

  if (p3)
    cout << "p3 is not null" << endl;
  else
    cout << "p3 is null" << endl;

  p2 = std::move(p3);     // p1->F1   , p2->F3, p3->null
  p3 = std::move(p1);     // p1->null , p2->F3, p3->F1
  p3 = std::move(p1);     // p1->null , p2->F3, p3->null

  if (p3)
    cout << "p3 is not null" << endl;
  else
    cout << "p3 is null" << endl;
}

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

TEST(CxxFeaturesTest, UseUniqueSinkSource)
{
  cout << "call source()" << endl;
  unique_ptr<Foo> up = source();

  cout << "main: owns " << up.get() << endl;

  sink(move(up));

  cout << "main: ends" << endl;
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
// cxx-bool

TEST(Bool, CheckBoolDefault)
{
  bool value{};
  EXPECT_EQ(value, false);
}


// ={=========================================================================
// cxx-stdio

TEST(CxxStdio, UseInput)
{
  int i{};
  double d{};
  string s{};

  // show the same result when use cin. To emulate input:
  // 10
  // 4.0
  // This is a text
  stringstream iss("10\n4.0\nThis is a text\n");
  iss >> i;
  iss >> d;
  iss >> s;

  EXPECT_EQ(i, 10);
  EXPECT_EQ(d, 4.0);
  EXPECT_EQ(s, "This");

  stringstream iss2("10\n4.0\nThis is a text\n");
  iss2 >> i;
  iss2 >> d;
  getline(iss2, s);

  EXPECT_EQ(i, 10);
  EXPECT_EQ(d, 4.0);
  EXPECT_EQ(s, "");

  stringstream iss3("10\n4.0\nThis is a text\n");
  iss3 >> i;
  iss3 >> d;
  getline(iss3 >> ws, s);

  EXPECT_EQ(i, 10);
  EXPECT_EQ(d, 4.0);
  EXPECT_EQ(s, "This is a text");

  // cin >> i;
  // cin >> d;
  // cin.ignore(numeric_limits<streamsize>::max(), '\n');
  // getline(cin, s);
  //
  // cout << i << endl;
  // cout << d << endl;
  // cout << s << endl;

  stringstream iss4("10\n4.0\n1 2 3 4\n");
  int i1, i2, i3, i4;
  iss4 >> i;
  iss4 >> d;
  iss4 >> i1;
  iss4 >> i2;
  iss4 >> i3;
  iss4 >> i4;

  vector<int> coll{i1, i2, i3, i4};
  
  EXPECT_EQ(i, 10);
  EXPECT_EQ(d, 4.0);
  EXPECT_THAT(coll, ElementsAre(1,2,3,4));
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
    cout << "derived is a subclass of base" << endl;
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

TEST(Bit, GetLimts)
{
  // fails
  // unsigned int int_max = (~((int)0)) >> 1;
  // int int_max = (~((int)0)) >> 1;
  
  // okays
  // int int_max = (~((unsigned int)0)) >> 1;
  // unsigned int int_max = (~((unsigned int)0)) >> 1;
  
  unsigned int uint_max = ~((unsigned int)0);
  int int_max = uint_max >> 1;
  int int_min = int_max + 1;
  
  // bitset<32> bitsetx{int_max};
  // cout << bitsetx << endl;

  EXPECT_EQ(uint_max, numeric_limits<unsigned int>::max());
  EXPECT_EQ(int_max, numeric_limits<int>::max());
  EXPECT_EQ(int_min, numeric_limits<int>::min());
}


// ={=========================================================================

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

