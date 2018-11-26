#include "gmock/gmock.h"

#include <iostream>
#include <list>
#include <deque>

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;


// ={=========================================================================

/*

15. IPv4 data type                                                        

Write a class that represents an IPv4 address. Implement the functions required
to be able to read and write such addresses from or to the console. The user
should be able to input values in dotted form, such
as 127.0.0.1 or 168.192.0.100. This is also the form in which IPv4 addresses
should be formatted to an output stream.

*/

namespace ipv4_2018_10 {

// 2018.10.15

// usage:
//
// ipv4 address(168, 192, 0, 1);   
// std::cout << address << std::endl;   
// cin >> ip;   

class ipv4
{
  friend ostream &operator<<(ostream &os, ipv4 ip);

  // friend istream &operator>>(istream &is, const ipv4 ip);
  // friend istream &operator>>(istream &is, ipv4 ip);

  friend istream &operator>>(istream &is, ipv4 &ip);

  public:
  ipv4(const int value1=0, const int value2=0, 
      const int value3=0, const int value4=0)
    : value1_(value1), value2_(value2), value3_(value3), value4_(value4)
  {}

  ipv4(const ipv4 &rhs)
    : value1_(rhs.value1_), value2_(rhs.value2_), value3_(rhs.value3_), value4_(rhs.value4_)
  {}

  string return_address()
  {
    return to_string(value1_) + "." + to_string(value2_) + "." 
      + to_string(value3_) + "." + to_string(value4_);
  }

  private:
  int value1_, value2_, value3_, value4_;
};


ostream &operator<<(ostream &os, const ipv4 ip)
{
  os << ip.value1_ << "." << ip.value2_ << "." << ip.value3_ << "." << ip.value4_;
  return os;
}

// this causes infinite loop on cin and eventually seg fault because
// ip's ctor is called on and on.
//
// 15.3.2 Input Operator >>
//
// Note that the second argument is modified. To make this possible, the second
// argument is passed by non-constant reference.
// 
// no seg fault but result is wrong since uses a copy
// istream &operator>>(istream &is, ipv4 ip);
//
// result is right
// istream &operator>>(istream &is, ipv4 &ip);
//
// seg fault when use:
//
// istream &operator>>(istream &is, const ipv4 ip);
//
// see that tempory is made to maintain constness but don't know why it
// causes a infinite ctor.
// 
// run gdb
//
// os >> address;
//
// causes call operator>>() which calls ipv4 ctor since it needs const copy.
//
// operator>>() calls
//
// is >> ip.value1_>> delim;
//
// this calls operator>>() which calls ipv4 ctor since it needs const copy.
//
// operator>>() calls
//
// is >> ip.value1_>> delim;
//
// ... cxx-recursive
// 
// even if operator>>() is called with const copy, it needs another const copy
// when execute "is >> ip.value1_>> delim;"
//
// when use istream &operator>>(istream &is, ipv4 ip)
//
// is >> ip.value1_>> delim;
//
// do not cause to call operator>>() again.
//
// so the key is when operator>>() is called ip is not writable so create one
// and this cause recursive. 


// istream &operator>>(istream &is, const ipv4 ip)
// istream &operator>>(istream &is, ipv4 ip)

istream &operator>>(istream &is, ipv4 &ip)
{
  char delim{};
  is >> ip.value1_>> delim 
    >> ip.value2_ >> delim 
    >> ip.value3_ >> delim
    >> ip.value4_;
  return is;
}

} // namespace

// (gdb) 
// b IPV4_20181015_Test::TestBody()
// b ipv4_2018_10::ipv4::ipv4(int,int,int,int)

TEST(IPV4_U15, 20181015)
{
  using namespace ipv4_2018_10;

  {
    stringstream os;
    ipv4 address(168, 192, 0, 1);   
    os << address;
    EXPECT_THAT(os.fail(), false);
    EXPECT_EQ(os.str(), "168.192.0.1");
  }

  {
    stringstream os;
    ipv4 address;   
    os << address;
    EXPECT_EQ(os.str(), "0.0.0.0");
  }

  {
    stringstream os{"10.20.30.40"};
    ipv4 address;   
    os >> address;
    EXPECT_THAT(os.fail(), false);
    EXPECT_EQ(address.return_address(), "10.20.30.40");
  }
}


namespace ip4_2018_11_14 
{
  // usage:
  //
  // ipv4 address(168, 192, 0, 1);   
  // std::cout << address << std::endl;   
  // cin >> ip;   

  class ipv4
  {
    friend istream &operator>>(istream &is, ipv4 &ip);
    friend ostream &operator<<(ostream &os, const ipv4 &ip);

    public:
      explicit ipv4(int value1 = 0, int value2 = 0, int value3 = 0, int value4 = 0):
        value1_(value1), value2_(value2), value3_(value3), value4_(value4) {}

      string return_address()
      {
        return to_string(value1_) + "." + to_string(value2_) + "." 
          + to_string(value3_) + "." + to_string(value4_);
      }

    private:
      int value1_{}, value2_{}, value3_{}, value4_{};
  };

  // input and output ops
  istream &operator>>(istream &is, ipv4 &ip)
  {
    char delim{};
    is >> ip.value1_ >> delim 
      >> ip.value2_ >> delim 
      >> ip.value3_ >> delim 
      >> ip.value4_;

    return is;
  }

  ostream &operator<<(ostream &os, const ipv4 &ip)
  {
    os << ip.value1_ << "." 
      << ip.value2_ << "." 
      << ip.value3_ << "." 
      << ip.value4_;
    return os;
  }

} // namespace

TEST(IPV4_U15, 20181114)
{
  using namespace ip4_2018_11_14;

  {
    stringstream os;
    ipv4 address(168, 192, 0, 1);   
    os << address;
    EXPECT_THAT(os.fail(), false);
    EXPECT_EQ(os.str(), "168.192.0.1");
  }

  {
    stringstream os;
    ipv4 address;   
    os << address;
    EXPECT_EQ(os.str(), "0.0.0.0");
  }

  {
    stringstream os{"10.20.30.40"};
    ipv4 address;   
    os >> address;
    EXPECT_THAT(os.fail(), false);
    EXPECT_EQ(address.return_address(), "10.20.30.40");
  }
}


// textbook
namespace u15_book
{

class ipv4
{
  public:

    // ctors

    constexpr ipv4() : data_{{0}} {}
    constexpr ipv4(unsigned const char a, unsigned const char b,
        unsigned const char c, unsigned const char d):
      data_{{a,b,c,d}} {}

    constexpr ipv4(unsigned long a):
      data_{{static_cast<unsigned char>((a >> 24) & 0xFF),
        static_cast<unsigned char>((a >> 16) & 0xFF),
        static_cast<unsigned char>((a >> 8) & 0xFF),
        static_cast<unsigned char>((a & 0xFF))}} {}

    // copy controls

    ipv4(const ipv4 &rhs):
      data_(rhs.data_) {}

    ipv4 &operator=(const ipv4 &rhs)
    {
      data_ = rhs.data_;
      return *this;
    }

    // friends

    friend std::ostream &operator<<(std::ostream &os, const ipv4 &a)
    {
      os << static_cast<int>(a.data_[0]) << '.'
        << static_cast<int>(a.data_[1]) << '.'
        << static_cast<int>(a.data_[2]) << '.'
        << static_cast<int>(a.data_[3]);
      return os;
    }

    friend std::istream &operator>>(std::istream &is, ipv4 &a)
    {
      char d1, d2, d3;
      int b1, b2, b3, b4;

      is >> b1 >> d1 >> b2 >> d2 >> b3 >> d3 >> b4;

      if (d1 == '.' && d2 == '.' && d3 == '.')
        a = ipv4(b1, b2, b3, b4);
      else
        is.setstate(std::ios_base::failbit);

      return is;
    }

  private:
    std::array<unsigned char, 4> data_;
};

} // namespace


// TEST(IPV4, Text)
// {
//   // using namespace textbook;
// 
//   textbook::ipv4 address(168, 192, 0, 1);   
//   std::cout << address << std::endl;   
// 
//   textbook::ipv4 ip;   
//   std::cout << ip << std::endl;   
//   std::cin >> ip;   
// 
//   if(!std::cin.fail())      
//     std::cout << ip << std::endl;
// }


// ={=========================================================================

/*

16. Enumerating IPv4 addresses in a range

Write a program that allows the user to input two IPv4 addresses representing a
range and list all the addresses in that range. Extend the structure defined for
the previous problem to implement the requested functionality.

{
   std::cout << "input range: ";
   ipv4 a1, a2;
   std::cin >> a1 >> a2;

   if (a2 > a1)
   {
      for (ipv4 a = a1; a <= a2; a++)
      {
         std::cout << a << std::endl;
      }
   }
   else 
   {
      std::cerr << "invalid range!" << std::endl;
   }
}

To be able to enumerate IPv4 addresses in a given range, it should first be
possible to compare IPv4 values. Therefore, we should implement at least
operator<, but the following listing contains implementation for all comparison
operators: ==, !=, <, >, <=, and >=. Also, in order to increment an IPv4 value,
implementations for both the prefix and postfix operator++ are provided. The
following code is an extension of the IPv4 class from the previous problem:

*/

// unit16
namespace u16_2018_11_19
{
  // usage:
  //
  // ipv4 address(168, 192, 0, 1);   
  // std::cout << address << std::endl;   
  // cin >> ip;   

  class ipv4
  {
    friend istream &operator>>(istream &is, ipv4 &ip);
    friend ostream &operator<<(ostream &os, const ipv4 &ip);

    public:
    explicit ipv4(int value1 = 0, int value2 = 0, int value3 = 0, int value4 = 0):
      value1_(value1), value2_(value2), value3_(value3), value4_(value4) {}

    string return_address()
    {
      return to_string(value1_) + "." + to_string(value2_) + "." 
        + to_string(value3_) + "." + to_string(value4_);
    }

    vector<int> return_list() const
    {
      return vector<int>{value1_, value2_, value3_, value4_};
    }

    // pre and post.
    //
    // Q: How about when value_4 > 255?

    ipv4 &operator++()
    {
      ++value4_;
      return *this;
    }

    // post
    const ipv4 operator++(int)
    {
      const ipv4 old = *this;
      ++(*this);
      return old;
    }

    private:
    int value1_{}, value2_{}, value3_{}, value4_{};
  };

  // input and output ops
  istream &operator>>(istream &is, ipv4 &ip)
  {
    char delim{};
    is >> ip.value1_ >> delim 
      >> ip.value2_ >> delim 
      >> ip.value3_ >> delim 
      >> ip.value4_;

    return is;
  }

  ostream &operator<<(ostream &os, const ipv4 &ip)
  {
    os << ip.value1_ << "." 
      << ip.value2_ << "." 
      << ip.value3_ << "." 
      << ip.value4_;
    return os;
  }

  // o do not need to be friend
  //
  // o could use if or tenary op to make comparison. better way? if ipv4 supports
  // iterator then would be simpler. so have a function to return vector which
  // supports access or iterator

  //  a < b excludes a == b and a > b. 
  //
  //  Also 
  //
  //  ip(value1, value2, value3, value4), 
  //  ip(value1, value2, value3, value4)
  //
  //  ip == ip means equal but not value1 == value1

  bool operator<(const ipv4 &a, const ipv4 &b)
  {
    auto list_a = a.return_list();
    auto list_b = b.return_list();

    // assumes that ipv4 has always four field set
    for (auto ai = list_a.begin(), bi = list_b.begin();
        ai != list_a.end(); ++ai, ++bi)
    {
      if (*ai > *bi)
        return false;
    }

    return (list_a == list_b) ? false : true;
  }

  // cannot use like this since operator<() only supports '<' case and ! makes
  // it supports '==' and '>' which is different to support '>' only.
  //
  // bool operator>(const ipv4 &a, const ipv4 &b)
  // {
  //   return !operator<(a, b);
  // }

  bool operator>(const ipv4 &a, const ipv4 &b)
  {
    auto list_a = a.return_list();
    auto list_b = b.return_list();

    // assumes that ipv4 has always four field set
    for (auto ai = list_a.begin(), bi = list_b.begin();
        ai != list_a.end(); ++ai, ++bi)
    {
      if (*ai < *bi)
        return false;
    }

    return (list_a == list_b) ? false : true;
  }

  bool operator<=(const ipv4 &a, const ipv4 &b)
  {
    auto list_a = a.return_list();
    auto list_b = b.return_list();

    // assumes that ipv4 has always four field set
    for (auto ai = list_a.begin(), bi = list_b.begin();
        ai != list_a.end(); ++ai, ++bi)
    {
      if (*ai > *bi)
        return false;
    }

    return true;
  }

  // cannot use like this since
  //
  // bool operator>=(const ipv4 &a, const ipv4 &b)
  // {
  //   return !operator<=(a, b);
  // }

  bool operator>=(const ipv4 &a, const ipv4 &b)
  {
    auto list_a = a.return_list();
    auto list_b = b.return_list();

    // assumes that ipv4 has always four field set
    for (auto ai = list_a.begin(), bi = list_b.begin();
        ai != list_a.end(); ++ai, ++bi)
    {
      if (*ai < *bi)
        return false;
    }

    return true;
  }

} // namespace


TEST(IPV4_U16, 20181119)
{
  using namespace u16_2018_11_19;

  {
    ipv4 a(10,209,60,1), b(10,209,60,2);

    EXPECT_TRUE(a < b);
    EXPECT_TRUE(b > a);
  }

  {
    ipv4 a(10,209,60,1), b(10,209,60,1);

    EXPECT_FALSE(a < b);
    EXPECT_FALSE(a > b);

    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(a >= b);
  }

  {
    ipv4 a(10,209,60,1);
    a++;
    EXPECT_EQ(a.return_address(), "10.209.60.2");
    ++a;
    EXPECT_EQ(a.return_address(), "10.209.60.3");
  }
}


// unit16
// do not copy the whole source here and repeat the key idea/difference 

namespace u16_book
{

  // copied from namespace ip4_2018_11_14 
  // {
  // usage:
  //
  // ipv4 address(168, 192, 0, 1);   
  // std::cout << address << std::endl;   
  // cin >> ip;   

  class ipv4
  {
    friend istream &operator>>(istream &is, ipv4 &ip);
    friend ostream &operator<<(ostream &os, const ipv4 &ip);

    public:
    explicit ipv4(int value1 = 0, int value2 = 0, int value3 = 0, int value4 = 0):
      value1_(value1), value2_(value2), value3_(value3), value4_(value4) {}

    string return_address()
    {
      return to_string(value1_) + "." + to_string(value2_) + "." 
        + to_string(value3_) + "." + to_string(value4_);
    }

    //
    unsigned long to_long() const
    {
      unsigned long value{};

      // value = (unsigned long)(value1_ << 24) 
      //   | (unsigned long)(value2_ << 16) 
      //   | (unsigned long)(value3_ << 8) 
      //   | (unsigned long)value4_;

      value = static_cast<unsigned long>(value1_ << 24) 
        | static_cast<unsigned long>(value2_ << 16) 
        | static_cast<unsigned long>(value3_ << 8) 
        | static_cast<unsigned long>(value4_);

      // '|' means '+'
      // unsigned long value_sum{};
      // value_sum = (unsigned long)(value1_ << 24) 
      //   + (unsigned long)(value2_ << 16) 
      //   + (unsigned long)(value3_ << 8) 
      //   + (unsigned long)value4_;
      //
      // if (value != value_sum)
      //   cout << "** differ **" << endl;

      return value;
    }

    // pre. the text uses another ctor() instead
    ipv4 &operator++()
    {
      unsigned long value = 1 + (*this).to_long();

      value1_ = static_cast<int>(value >> 24) & 0xFF;
      value2_ = static_cast<int>(value >> 16) & 0xFF;
      value3_ = static_cast<int>(value >> 8) & 0xFF;
      value4_ = static_cast<int>(value) & 0xFF;

      return *this;
    }

    // post
    const ipv4 operator++(int)
    {
      ipv4 old = *this;
      ++(*this);
      return old;
    }

    private:
    int value1_{}, value2_{}, value3_{}, value4_{};
  };

  // input and output ops
  istream &operator>>(istream &is, ipv4 &ip)
  {
    char delim{};
    is >> ip.value1_ >> delim 
      >> ip.value2_ >> delim 
      >> ip.value3_ >> delim 
      >> ip.value4_;

    return is;
  }

  ostream &operator<<(ostream &os, const ipv4 &ip)
  {
    os << ip.value1_ << "." 
      << ip.value2_ << "." 
      << ip.value3_ << "." 
      << ip.value4_;
    return os;
  }
  
  // } // namespace


  // operator <, >
  bool operator<(const ipv4 &a, const ipv4 &b)
  {
    if (a.to_long() < b.to_long())
      return true;

    return false;
  }

  bool operator>(const ipv4 &a, const ipv4 &b)
  {
    return (a.to_long() > b.to_long()) ? true : false;
  }     

  bool operator<=(const ipv4 &a, const ipv4 &b)
  {
    return (a.to_long() <= b.to_long()) ? true : false;
  }     

  bool operator>=(const ipv4 &a, const ipv4 &b)
  {
    return (a.to_long() >= b.to_long()) ? true : false;
  }     
}

TEST(IPV4_U16, Text)
{
  using namespace u16_book;

  {
    ipv4 a(10,209,60,1), b(10,209,60,2);

    EXPECT_TRUE(a < b);
    EXPECT_TRUE(b > a);
  }

  {
    ipv4 a(10,209,60,1), b(10,209,60,1);

    EXPECT_FALSE(a < b);
    EXPECT_FALSE(a > b);

    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(a >= b);
  }

  {
    ipv4 a(10,209,60,1);
    a++;
    EXPECT_EQ(a.return_address(), "10.209.60.2");
    ++a;
    EXPECT_EQ(a.return_address(), "10.209.60.3");
  }
}


// ={=========================================================================

/*
17. Creating a 2D array with basic operations

Write a class template that represents a two-dimensional array container with
methods for element access (at() and data()), capacity querying, iterators,
filling, and swapping. It should be possible to move objects of this type.

{
   // element access
   array2d<int, 2, 3> a {1, 2, 3, 4, 5, 6};
   for (size_t i = 0; i < a.size(1); ++i)
      for (size_t j = 0; j < a.size(2); ++j)
      a(i, j) *= 2;

   // iterating
   std::copy(std::begin(a), std::end(a), 
      std::ostream_iterator<int>(std::cout, " "));
 
   // filling 
   array2d<int, 2, 3> b;
   b.fill(1);

   // swapping
   a.swap(b);

   // moving
   array2d<int, 2, 3> c(std::move(b));
}

*/

// unit17 - when not use stl container as internal data
namespace u17_2018_11_20
{
  template<typename T, int Row, int Col>
  class array2d
  {
    public:
      typedef T value_type;
      typedef value_type *iterator;
      typedef const value_type *const_iterator;

      explicit array2d(std::initializer_list<T> values)
      {
        int row{}, col{};

        for (auto e: values)
        {
          // cxx-modulus
          if (col && ((col % Col) == 0)) 
          {
            ++row; col = 0;
          }

          array_[row][col] = e;

          // cout << "row: " << row << ", col: " << col << " = " <<
          //   array_[row][col] << endl;

          ++col;
        }
      }

      void print_array()
      {
        int row{}, col{};

        for (row = 0; row < Row; ++row)
          for (col = 0; col < Col; ++col)
            cout << array_[row][col] << ", ";
        cout << endl;
      }

      // iterators
      iterator begin() { return (T*) &array_; }
      iterator end() { return (T*)&array_ + Row*Col; }

      const_iterator begin() const { return (T*)&array_; }
      const_iterator end() const { return (T*)&array_ + Row*Col; }

    private:
        T array_[Row][Col];
        T *current_;
  };
} // namespace


TEST(U17, 20181120)
{
  using namespace u17_2018_11_20;

  {
    array2d<int, 2, 3> coll{1,2,3,4,5,6};
    ostringstream os;

    for (auto e : coll)
      os << e << ", ";
    os << endl;

    std::copy(std::begin(coll), std::end(coll),
        ostream_iterator<int>(os, ", "));
    os << endl;

    EXPECT_THAT(os.str(), Eq("1, 2, 3, 4, 5, 6, \n1, 2, 3, 4, 5, 6, \n"));
  }
}


// unit17 - when use stl container as internal data
namespace u17_2018_11_23
{
  template<typename T, int Row, int Col>
    class array2d
    {
      public:

        //  *cxx-remember* *cxx-gtest* shall be public to use ElementsAre()
        typedef T value_type;
        typedef value_type *iterator;
        typedef value_type const* const_iterator;


        // default ctor
        explicit array2d() : array_(Row * Col) {}

        explicit array2d(std::initializer_list<T> values) : array_(values) {}

        // note that how specify array2d arg

        void swap(array2d &rhs)
        {
          array_.swap(rhs.array_);
        }

        // move support??
        //
        // If class do not define copy ctor and assign then compiler 'always'
        // make these operations; defined either as memberwise or deleted.
        // 
        // Move operations are different. The compiler make move operations
        // `only if` a class does 'not' define any copy ctor and assign and
        // `only if` all non-static members can be move constructed and assigned
        // because if a class doesn't have a move operation, `the corresponding
        // copy operation is used` in place of move through function matching. 

        void fill(T const value)
        {
          // algo-fill
          std::fill(begin(), end(), value);
        }

        size_t size(size_t rank)
        {
          if (rank == 1) return Row;
          else if (rank == 2) return Col;

          return 0;
        }

        // operator()
        // why do not need to implement operator*() for iterator to work? since
        // it's pointer.

        T& operator() (size_t const row, size_t const col)
        {
          return array_[row * col + col];
        }

        constexpr const T& operator() (size_t const row, size_t const col) const
        {
          return array_[row * col + col];
        }

        // iterators

        iterator begin() { return array_.data(); }
        iterator end() { return array_.data() + array_.size(); }

        const_iterator begin() const { return array_.data(); }
        const_iterator end() const { return array_.data() + array_.size(); }

      private:
        std::vector<T> array_;
    };

  // note that how specify array2d arg
  template<typename T, size_t Row, size_t Col>
    void print_array2d(array2d<T, Row, Col> const &arr)
    {
      for (int i = 0; i < Row; ++i)
      {
        for (int j = 0; j < Col; ++j)
        {
          std::cout << arr.at(i, j) << ' ';
        }

        std::cout << std::endl;
      }
    }

} // namespace

TEST(U17, 20181123)
{
  using namespace u17_2018_11_23;

  {
    array2d<int, 2, 3> coll{1,2,3,4,5,6};
    ostringstream os;

    for (auto e : coll)
      os << e << ", ";
    os << endl;

    std::copy(std::begin(coll), std::end(coll),
        ostream_iterator<int>(os, ", "));
    os << endl;

    EXPECT_THAT(os.str(), Eq("1, 2, 3, 4, 5, 6, \n1, 2, 3, 4, 5, 6, \n"));
  }

  // fill
  {
    array2d<int, 2, 3> coll;
    coll.fill(1);
    EXPECT_THAT(coll, ElementsAre(1,1,1,1,1,1));
  }

  // operator(x,y)
  {
    array2d<int, 2, 3> coll;

    for (size_t i = 0; i < coll.size(1); ++i)
      for (size_t j = 0; j < coll.size(2); ++j)
        coll(i, j) = 1 + i * 3 + j;

    EXPECT_THAT(coll, ElementsAre(4,2,5,0,6,0));
  }

  // move
  {
    array2d<int, 2, 3> a{10,20,30,40,50,60};
    array2d<int, 2, 3> coll(std::move(a));

    EXPECT_THAT(coll, ElementsAre(10,20,30,40,50,60));
  }

  // swap
  {
    array2d<int, 2, 3> coll1 { 1,2,3,4,5,6 };
    array2d<int, 2, 3> coll2 { 10,20,30,40,50,60 };

    EXPECT_THAT(coll1, ElementsAre(1,2,3,4,5,6));
    EXPECT_THAT(coll2, ElementsAre(10,20,30,40,50,60));

    coll1.swap(coll2);

    EXPECT_THAT(coll1, ElementsAre(10,20,30,40,50,60));
    EXPECT_THAT(coll2, ElementsAre(1,2,3,4,5,6));
  }
}

/*

Note that for element access, we are using operator(), such as in a(i,j), and
not operator[], such as in a[i][j], because only the former can take multiple
arguments (one for the index on each dimension). The latter can only have a
single argument, and in order to enable expressions like a[i][j], it has to
return an intermediate type (one that basically represents a row) that in turn
overloads operator[] to return a single element. 

There are already standard containers that store either fixed or variable-length
sequences of elements. This two-dimensional array class should be just an
`adapter` for such a container. In choosing between std::array and std::vector,
we should consider two things:

The array2d class should have move semantics to be able to move objects It
should be possible to list initialize an object of this type The std::array
container is movable only if the elements it holds are move-constructible and
move-assignable. On the other hand, it cannot be constructed from an
std::initializer_list. (that's not ture)

Therefore, the more viable option remains an std::vector.

Internally, this adapter container can store its data either in a vector of
vectors (each row is a vector<T> with C elements, and the 2D array has R such
elements stored in a vector<vector<T>>) or single vector of RC elements of type
T. In the latter case, the element on row i and column j is found at index i * C
+ j. This approach has a smaller memory footprint, stores all data in a single
contiguous chunk, and is also simpler to implement. For these reasons, it is the
preferred solution.

*/


// ={=========================================================================

/*

18. Minimum function with any number of arguments

Write a function template that can take any number of arguments and returns the
minimum value of them all, using operator < for comparison. 

Write a variant of this function template that can be parameterized with a
binary comparison function to use instead of operator <.

*/

namespace u18_2018_11_23
{
  template <typename T>
    T get_minimum(initializer_list<T> values)
    {
      T running_value = std::numeric_limits<T>::max();

      for (const auto e : values)
      {
        if (e < running_value)
          running_value = e;
      }

      return running_value;
    }

  template <typename T, typename Comp>
    T get_minimum(initializer_list<T> values, Comp comp)
    {
      T running_value = std::numeric_limits<T>::max();

      for (const auto e : values)
      {
        if (comp(e,running_value))
          running_value = e;
      }

      return running_value;
    }
} // namespace

TEST(U18, 20181123)
{
  using namespace u18_2018_11_23;

  EXPECT_THAT(get_minimum({5, 4, 2, 3}), 2);

  EXPECT_THAT(get_minimum({5, 4, 2, 3}, std::less<int>()), 2);
}


#if 0

o the text means variadic and recursive call

o Do not compile

namespace u18_text
{
  template <typename T>
    T minimum(T const a, T const b) { return a < b ? a : b; }

  template <typename T1, typename... T>
    T1 minimum(T1 a, T... args)
    {
      return minimum(a, minimum(args...));
    }

  template <class Compare, typename T>
    T minimumc(Compare comp, T const a, T const b) { return comp(a, b) ? a : b; }

  template <class Compare, typename T1, typename... T>
    T1 minimumc(Compare comp, T1 a, T... args)
    {
      return minimumc(comp, a, minimumc(comp, args...));
    }
} // namespace

TEST(U18, Text)
{
  using namespace u18_text;

  EXPECT_THAT(minimum(5, 4, 2, 3), 2);

  EXPECT_THAT(minimum(less<int>(), 5, 4, 2, 3), 2);

  // EXPECT_THAT(minimum(5, 4, 2, 3, std::less<>()), 2);
}

#endif


// ={=========================================================================

/*

19. Adding a range of values to a container

Write a general-purpose function that can add any number of elements to the end
of a container that has a method push_back(T&& value).

*/

namespace u19_2018_11_23
{
  template <typename T>
  void add_elements(T &coll, initializer_list<typename T::value_type> values)
  {
    for (auto &e : values)
      coll.push_back(e);
  }
} // namespace

TEST(U19, 20181123)
{
  using namespace u19_2018_11_23;

  {
    std::vector<int> coll;
    add_elements(coll, {5, 4, 2, 3});
    EXPECT_THAT(coll, ElementsAre(5, 4, 2, 3));
  }

  {
    std::list<int> coll;
    add_elements(coll, {5, 4, 2, 3});
    EXPECT_THAT(coll, ElementsAre(5, 4, 2, 3));
  }

  {
    std::deque<int> coll;
    add_elements(coll, {5, 4, 2, 3});
    EXPECT_THAT(coll, ElementsAre(5, 4, 2, 3));
  }
}


#if 0

Writing functions with any number of arguments is possible using variadic
function templates. The function should have the container as the first
parameter, followed by a variable number of arguments representing the values to
be added at the back of the container. However, writing such a function template
can be significantly simplified using fold expressions. Such an implementation
is shown here:

fold expression needs:

C++17 features are available as part of "mainline" GCC in the trunk of GCCs
repository and in GCC 5 and later.

namespace u19_text
{
  template<typename C, typename... Args>
    void push_back(C& c, Args&&... args)
    {
      (c.push_back(args), ...);
    }
}

TEST(U19, Text)
{
  using namespace u19_text;

  std::vector<int> v;
  push_back(v, 1, 2, 3, 4);
  std::copy(std::begin(v), std::end(v), 
      std::ostream_iterator<int>(std::cout, " "));

  std::list<int> l;
  push_back(l, 1, 2, 3, 4);
  std::copy(std::begin(l), std::end(l), 
      std::ostream_iterator<int>(std::cout, " "));
}

#endif


// ={=========================================================================

/*

20. Container any, all, none

Write a set of general-purpose functions that enable checking whether any, all,
or none of the specified arguments are present in a given container. These
functions should make it possible to write code as follows:

std::vector<int> v{ 1, 2, 3, 4, 5, 6 };
assert(contains_any(v, 0, 3, 30));

std::array<int, 6> a{ { 1, 2, 3, 4, 5, 6 } };
assert(contains_all(a, 1, 3, 5, 6));

std::list<int> l{ 1, 2, 3, 4, 5, 6 };
assert(!contains_none(l, 0, 6));

*/

namespace u20_2018_11_24
{
  template <typename C>
    bool contains_any(C coll, initializer_list<typename C::value_type> values)
    {
      for (const auto e : values)
        if (coll.end() != find(coll.begin(), coll.end(), e))
          return true;

      return false;
    }

  template <typename C>
    bool contains_all(C coll, initializer_list<typename C::value_type> values)
    {
      for (const auto e : values)
        if (coll.end() == find(coll.begin(), coll.end(), e))
          return false;

      return true;
    }

  template <typename C>
    bool contains_none(C coll, initializer_list<typename C::value_type> values)
    {
      for (const auto e : values)
        if (coll.end() != find(coll.begin(), coll.end(), e))
          return false;

      return true;
    }
} // namespace

TEST(U20, 20081124)
{
  using namespace u20_2018_11_24;

  {
    std::vector<int> coll{ 1, 2, 3, 4, 5, 6 };
    EXPECT_TRUE(contains_any(coll, {0, 3, 30}));
  }

  {
    std::vector<int> coll{ 1, 2, 3, 4, 5, 6 };
    EXPECT_TRUE(contains_all(coll, {1, 3, 5, 6}));
  }

  {
    std::vector<int> coll{ 1, 2, 3, 4, 5, 6 };
    EXPECT_TRUE(!contains_none(coll, {1, 0, 6}));
  }
}

#if 0

The requirement to be able to check the presence or absence of a variable number
of arguments suggests that we should write variadic function templates. However,
   these functions require a helper function, a general-purpose one that checks
   whether an element is found in a container or not and returns a bool to
   indicate success or failure. Since all these functions, which we could call
   contains_all, contains_any, and contains_none, do is apply logical operators
   on the results returned by the helper function, we would use fold expressions
   to simplify the code. Short circuit evaluation is enabled after the expansion
   of the fold expression, which means we are evaluating only the elements that
   lead to a definitive result. So if we are looking for the presence of all 1,
   2, and 3, and 2 is missing, the function will return after looking up value 2
   in the container without checking value 3:

template<class C, class T>
bool contains(C const & c, T const & value)
{
   return std::end(c) != std::find(std::begin(c), std::end(c), value);
}

template<class C, class... T>
bool contains_any(C const & c, T &&... value)
{
   return (... || contains(c, value));
}

template<class C, class... T>
bool contains_all(C const & c, T &&... value)
{
   return (... && contains(c, value));
}

template<class C, class... T>
bool contains_none(C const & c, T &&... value)
{
   return !contains_any(c, std::forward<T>(value)...);
}

#endif


// ={=========================================================================

/*

21. System handle wrapper

Consider an operating system handle, such as a file handle. Write a wrapper that
handles the acquisition and release of the handle, as well as other operations
such as verifying the validity of the handle and moving handle ownership from
one object to another.

*/

namespace u21_2018_11_24
{
  using HANDLE = void *;

#if __GNUC__ && __x86_64__
#define LONG_PTR long long
#else
#define LONG_PTR long
#endif

// win32 specifics
#define DWORD     unsigned long
#define GENERIC_READ           0x80000000L
#define GENERIC_WRITE          0x40000000L
#define CREATE_NEW             1
#define CREATE_ALWAYS          2
#define OPEN_EXISTING          3
#define OPEN_ALWAYS            4
#define TRUNCATE_EXISTING      5
#define FILE_SHARE_READ        1
#define FILE_ATTRIBUTE_NORMAL  0x00000080

#define INVALID_HANDLE  ((HANDLE)(LONG_PTR)-1)

  int CloseHandle(HANDLE hObject) 
  {
    (void) hObject;
    cout << "Handle is closed" << endl;
    return 0;
  }

  int ReadFile(HANDLE,
      void*,
      DWORD,
      DWORD *,
      void *)
  {return 0;}

  HANDLE CreateFile(char const *,
      DWORD,
      DWORD,
      void *,
      DWORD,
      DWORD,
      HANDLE)
  {
    cout << "Handle is created" << endl;
    return INVALID_HANDLE;
  }

  struct null_handle_traits
  {
    using pointer = HANDLE;

    static pointer invalid() noexcept
    { return nullptr; }

    static void close(pointer handle) noexcept
    { CloseHandle(handle); }
  };

  struct invalid_handle_traits
  {
    using pointer = HANDLE;

    // added to avoid error
    explicit invalid_handle_traits(HANDLE handle) : handle_(handle) {}

    static pointer invalid() noexcept
    { return INVALID_HANDLE; }

    static void close(pointer handle) noexcept
    { CloseHandle(handle); }

    pointer handle_;
  };

  template <typename T>
    class unique_handle
    {
      using pointer = typename T::pointer;

      public:
      // no copy support
      unique_handle(const unique_handle &) = delete;
      unique_handle &operator=(const unique_handle &) = delete;

      explicit unique_handle(pointer handle = T::invalid()) noexcept
        : handle_(handle) {}

      // move ctor
      unique_handle(unique_handle &&rhs) noexcept
        : handle_(rhs.release()) {}

      // move assign
      unique_handle &operator=(unique_handle &&rhs) noexcept
      {
        // self assign check
        if (this != &rhs)
          reset(rhs.release());

        return *this;
      }

      ~unique_handle() noexcept
      { T::close(handle_); }

      // bool conversion
      explicit operator bool() const noexcept
      { return handle_ != T::invalid(); }

      pointer get() const noexcept
      { return handle_; }

      pointer release() noexcept
      {
        auto handle = handle_;
        handle_ = T::invalid();
        return handle;
      }

      bool reset(pointer handle = T::invalid()) noexcept
      {
        if (handle_ != handle)
        {
          T::close(handle_);
          handle_ = handle;
        }

        // call bool conversion
        return static_cast<bool>(*this);
      }

      void swap(unique_handle<T> &rhs) noexcept
      { std::swap(handle_, rhs.handle_); }

      private:
      pointer handle_;

    };

  template <typename T>
    void swap(unique_handle<T> &lhs, unique_handle<T> &rhs) noexcept
    {
      lhs.swap(rhs);
    }

  // template <typename T>
  //   bool operator!(const unique_handle<T> &lhs, const unique_handle<T> &rhs) noexcept
  //   {
  //     return lhs.get() != rhs.get();
  //   }

  using invalid_handle = unique_handle<invalid_handle_traits>;

  void function_that_throws()
  {
    cout << "function throws" << endl;
    throw std::runtime_error("an error has occurred");
  }

  // void bad_handle_example()
  // {
  //   bool condition1 = false;
  //   bool condition2 = true;
  //
  //   HANDLE handle = CreateFile("sample.txt",
  //       GENERIC_READ,
  //       FILE_SHARE_READ,
  //       nullptr,
  //       OPEN_EXISTING,
  //       FILE_ATTRIBUTE_NORMAL,
  //       nullptr);
  //   if (handle == INVALID_HANDLE)
  //     return;
  //
  //   if (condition1)
  //   {
  //     CloseHandle(handle);
  //     return;
  //   }
  //
  //   std::vector<char> buffer(1024);
  //   unsigned long bytesRead = 0;
  //   ReadFile(handle, buffer.data(), buffer.size(), &bytesRead, nullptr);
  //
  //   if (condition2)
  //   {
  //     // forgot to close handle, lose handle
  //     return;
  //   }
  //
  //   // lose handle
  //   function_that_throws();
  //
  //   CloseHandle(handle);
  // }


  // handle should be closed no matter what exit path there are

  void good_handle_example()
  {
    bool condition1 = false;
    bool condition2 = true;

    // should not 
    // invalid_handle_traits handle{CreateFile("sample.txt",
    // since we use using typedef

    invalid_handle handle{CreateFile("sample.txt",
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr)};

    function_that_throws();

    if (!handle) return;

    if (condition1) return;

    std::vector<char> buffer(1024);
    unsigned long bytesRead = 0;
    ReadFile(handle.get(),
        buffer.data(),
        buffer.size(),
        &bytesRead,
        nullptr);

    if (condition2) return;
  }

} // namespace


TEST(U21, 20081124)
{
  using namespace u21_2018_11_24;

  // try
  // {
  //   bad_handle_example();
  // }
  // catch (...)
  // {
  //   ostringstream os;
  //   os << "bad handle example happened";
  //   EXPECT_THAT(os.str(), "bad handle example happened");
  // }

  try
  {
    good_handle_example();
  }
  catch (...)
  {
    ostringstream os;
    os << "good handle example happened";
    EXPECT_THAT(os.str(), "good handle example happened");
  }
}


// ={=========================================================================

/*

22. Literals of various temperature scales

Write a small library that enables expressing temperatures in the three most
used scales, Celsius, Fahrenheit, and Kelvin, and converting between them. The
library must enable you to write temperature literals in all these scales, such
as 36.5_deg for Celsius, 97.7_f for Fahrenheit, and 309.65_K for Kelvin; perform
operations with these values; and convert between them.

*/


// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
