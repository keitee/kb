#include <iostream>
#include <set>
#include <vector>
#include <memory>
#include <chrono>

#include "gtest/gtest.h"

using namespace std;

// ={=========================================================================
// cxx-pair

TEST(CxxFeaturesTest, UsePairType)
{
    const auto map{
        make_pair(10, "X"),
        make_pair(9, "IX"),
        make_pair(5, "V")
    };

    for(const auto &e : map)
        cout << "{" << e.first << ", " << e.second << "}" << endl;

    cout << "typeid : " << typeid(map).name() << endl;
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
// cxx-crono
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

TEST(CxxFeaturesTest, UseCronoDurationCast)
{
    chrono::milliseconds ms(7255042);

    chrono::hours   hh = chrono::duration_cast<chrono::hours>(ms);
    chrono::minutes mm = chrono::duration_cast<chrono::minutes>(ms % chrono::hours(1));
    chrono::seconds ss = chrono::duration_cast<chrono::seconds>(ms % chrono::minutes(1));
    chrono::milliseconds msec = chrono::duration_cast<chrono::milliseconds>(ms % chrono::seconds(1));

    cout << "raw: " << hh << "::" << mm << "::"
        << ss << "::" << msec << endl;
    cout << "     " << setfill('0') << setw(2) << hh.count() << "::" 
        << setw(2) << mm.count() << "::"
        << setw(2) << ss.count() << "::"
        << setw(2) << msec.count() << endl;
}

// ={=========================================================================
// cxx-crono-clock

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

TEST(CxxFeaturesTest, UseCronoClock)
{
    cout << "system_clock: " << endl;
    print_clock_data<std::chrono::system_clock>();

    cout << "high_resolution_clock: " << endl;
    print_clock_data<std::chrono::high_resolution_clock>();

    cout << "steady_clock: " << endl;
    print_clock_data<std::chrono::steady_clock>();
}


// ={=========================================================================
// cxx-crono-timepoint

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

TEST(CxxFeaturesTest, UseCronoTimepoint)
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

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

