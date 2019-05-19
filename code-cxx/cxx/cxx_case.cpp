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


/*
={=============================================================================

StrBlob without StrBlobPtr

CPR 455

o show how to implement a new collection type using library container.
`is-implemented-in-terms-of-model`

o This shows when shared_ptr is useful than own raii since this case don't
need `copy-controls` and use syn versions but not a problem as it has only
one member which is shared_ptr.
 
*/

namespace cxx_case_strblob
{
  class StrBlob
  {
    public:
      typedef std::vector<std::string>::size_type size_type;
      
      explicit StrBlob() : 
        data_(new std::vector<std::string>()) {}

      explicit StrBlob(std::initializer_list<std::string> il) : 
        data_(new std::vector<std::string>(il)) {}

      size_type size() const noexcept
      { return data_->size(); }
      
      bool empty() const noexcept
      { return data_->empty(); }

      void push_back(std::string const& elem)
      { data_->push_back(elem); }

      void pop_back()
      {
        // *cxx-vector* *cxx-undefined* when called on empty vector
        check(0, "coll is empty");

        // *cxx-vector* do not return value
        data_->pop_back();
      }

      std::string& front()
      {
        check(0, "coll is empty");
        return data_->front();
      }

      std::string& back()
      {
        check(0, "coll is empty");
        return data_->back();
      }

    private:
      shared_ptr<std::vector<std::string>> data_;

      // *cxx-except* has nothing to do with return type
      void check(size_type index, string const& message) const
      {
        if (index >= data_->size())
          throw std::out_of_range(message);
      }
  };

} // namespace


TEST(CxxCase, StringBlobNoIteratorSupport)
{ 
  using namespace cxx_case_strblob;

  {
    StrBlob sb{"one", "two", "three", "four"};

    EXPECT_THAT(sb.size(), 4);
    EXPECT_THAT(sb.front(), "one");
    EXPECT_THAT(sb.back(), "four");

    sb.push_back("five");

    EXPECT_THAT(sb.size(), 5);
    EXPECT_THAT(sb.front(), "one");
    EXPECT_THAT(sb.back(), "five");
  }

  {
    StrBlob sb;

    EXPECT_THAT(sb.size(), 0);
    EXPECT_THROW(sb.front(), std::out_of_range);
  }
} 


// *cxx-iter*
// see that iterator implementation
//
// <- new container class ->  <- companion class like iterator ->
//    StrBlob                    StrBlobPtr
//    - shared_ptr               - weak_ptr
//
//                      vector[]
//                      - underlying container

namespace cxx_case_strblob_iterator
{
  class StrBlobIterator;

  class StrBlob
  {
    friend class StrBlobIterator;

    public:
      typedef std::vector<std::string>::size_type size_type;
      
      explicit StrBlob() : 
        data_(new std::vector<std::string>()) {}

      explicit StrBlob(std::initializer_list<std::string> il) : 
        data_(new std::vector<std::string>(il)) {}

      size_type size() const noexcept
      { return data_->size(); }
      
      bool empty() const noexcept
      { return data_->empty(); }

      void push_back(std::string const& elem)
      { data_->push_back(elem); }

      void pop_back()
      {
        // *cxx-vector* *cxx-undefined* when called on empty vector
        check(0, "coll is empty");

        // *cxx-vector* do not return value
        data_->pop_back();
      }

      std::string& front()
      {
        check(0, "coll is empty");
        return data_->front();
      }

      std::string& back()
      {
        check(0, "coll is empty");
        return data_->back();
      }

      // cause *cxx-incomplete-type* so cannot be defined until StrBlobIterator
      // is defined.

      StrBlobIterator begin();

      StrBlobIterator end();

    private:
      shared_ptr<std::vector<std::string>> data_;

      // *cxx-except* has nothing to do with return type
      void check(size_type index, string const& message) const
      {
        if (index >= data_->size())
          throw std::out_of_range(message);
      }
  };


  // originally StrBlobPtr

  class StrBlobIterator
  {
    public:
      typedef std::vector<std::string>::size_type size_type;

      // *cxx-incomplete-type* so move StrBlobIterator after StrBlob
      StrBlobIterator(StrBlob& blob, size_type pos = 0)
        : blob_(blob.data_), curr_(pos)
      {}

      // *cxx-operator-postfix* prefix version
      StrBlobIterator& operator++()
      {
        check(curr_, "increase past the end");
        ++curr_;
        return *this;
      }

      // *cxx-operator-postfix* postfix version
      StrBlobIterator const operator++(int)
      {
        StrBlobIterator it = *this;
        ++*this;
        return it;
      }

      StrBlobIterator& operator--()
      {
        // decrease first and since it is size_type, -1 becomes big
        --curr_;
        check(curr_, "decrease before the begin");
        return *this;
      }


      StrBlobIterator const operator--(int)
      {
        StrBlobIterator it = *this;
        --*this;
        return it;
      }

      // *cxx-overload-operator*
      //
      // 1. this iterator points vector<string>. return vector<string> or
      // string?
      //
      // 2. Although it's const member function since do not change its state,
      // returns a reference and client can change through it.

      std::string& operator*() const 
      {
        auto blob_ptr = check(curr_, "dereference non-existent element");

        // *cxx-precedence* [] is higher
        return (*blob_ptr)[curr_];
      }

      // this is the original code from the book. However, operator->() make
      // sense when underlying item is pointer. In this case, it's not so
      // comment it out.
      //
      // std::string* operator->() const
      // {
      //     // take address of a string reference
      //     return & this->operator*();
      // }

      // *cxx-overload-operator-equal*

      bool operator==(StrBlobIterator const& other) const
      {
        auto lhs_blob_ptr = blob_.lock();

        // *cxx-access* WHY NOT ERROR???
        
        auto rhs_blob_ptr = other.blob_.lock();

        // don't seem to need "curr_ == other.curr_" since it's enough to see
        // shared pointer is not null

        if (lhs_blob_ptr == rhs_blob_ptr)
          return (!lhs_blob_ptr || curr_ == other.curr_);
        else 
          return false;
      }

      bool operator!=(StrBlobIterator const& other) const
      {
        return !(*this == other);
      }

    private:
      std::weak_ptr<std::vector<std::string>> blob_;
      size_type curr_{0};

      std::shared_ptr<std::vector<std::string>> check(size_type index,
          string const& message) const
      {
        auto blob_ptr = blob_.lock();
        if (!blob_ptr)
          throw std::runtime_error("unbounded shared_ptr");

        if (index >= blob_ptr->size())
          throw std::out_of_range(message);

        return blob_ptr;
      }
  };

  // cause *cxx-incomplete-type* so cannot be defined until StrBlobIterator
  // is defined. so define them here

  StrBlobIterator StrBlob::begin()
  {
    return StrBlobIterator(*this, 0);
  }

  StrBlobIterator StrBlob::end()
  {
    return StrBlobIterator(*this, data_->size());
  }

} // namespace

TEST(CxxCase, StringBlobIteratorSupport)
{ 
  using namespace cxx_case_strblob_iterator;

  {
    vector<string> coll;

    StrBlob sb{"one", "two", "three", "four"};

    // for (auto const& e : sb)
    // {
    //   cout <<
    // }
    for (auto it = sb.begin(); it != sb.end(); ++it)
      coll.push_back(*it);

    EXPECT_THAT(coll, ElementsAre("one", "two", "three", "four"));
  }

  {
    vector<string> coll;

    StrBlob sb{"one", "two", "three", "four"};

    for (auto const& e : sb)
      coll.push_back(e);

    EXPECT_THAT(coll, ElementsAre("one", "two", "three", "four"));
  }

  {
    vector<string> coll;

    StrBlob sb{"one", "two", "three", "four"};

    // since operator*() returns reference
    auto it = sb.begin();
    *it = "I";

    for (auto const& e : sb)
      coll.push_back(e);

    EXPECT_THAT(coll, ElementsAre("I", "two", "three", "four"));
  }
} 


namespace cxx_case_strblob_template
{
  template <typename T>
  class StrBlob
  {
    public:

      // was:
      // typedef std::vector<T>::size_type size_type;
      //
      // *cxx-template-type-members* it now needs `typename` keyword since it is
      // a template now.

      typedef typename std::vector<T>::size_type size_type;
      
      explicit StrBlob() : 
        data_(new std::vector<T>()) {}

      explicit StrBlob(std::initializer_list<T> il) : 
        data_(new std::vector<T>(il)) {}

      size_type size() const noexcept
      { return data_->size(); }
      
      bool empty() const noexcept
      { return data_->empty(); }

      void push_back(std::string const& elem)
      { data_->push_back(elem); }

      void pop_back()
      {
        // *cxx-vector* *cxx-undefined* when called on empty vector
        check(0, "coll is empty");

        // *cxx-vector* do not return value
        data_->pop_back();
      }

      T& front()
      {
        check(0, "coll is empty");
        return data_->front();
      }

      T& back()
      {
        check(0, "coll is empty");
        return data_->back();
      }

    private:
      shared_ptr<std::vector<T>> data_;

      void check(size_type index, string const& message) const;
  };

  // *cxx-except* has nothing to do with return type
  //
  // to show how to:
  //
  // <cxx-template-member-function>
  //
  // To write a member function of class template 'outside' of definition,
  // remember that the instantiated type is a new type and must say to which
  // class the member belongs. see Blob`<T>`
  //
  // it is instantiated `only if` it is used and if not, it is not instantiated.

  template <typename T>
    void StrBlob<T>::check(size_type index, string const& message) const
  {
    if (index >= data_->size())
      throw std::out_of_range(message);
  }

} // namespace


TEST(CxxCase, StringBlobTemplate)
{ 
  using namespace cxx_case_strblob_template;

  {
    StrBlob<std::string> sb{"one", "two", "three", "four"};

    EXPECT_THAT(sb.size(), 4);
    EXPECT_THAT(sb.front(), "one");
    EXPECT_THAT(sb.back(), "four");

    sb.push_back("five");

    EXPECT_THAT(sb.size(), 5);
    EXPECT_THAT(sb.front(), "one");
    EXPECT_THAT(sb.back(), "five");
  }

  {
    StrBlob<std::string> sb;

    EXPECT_THAT(sb.size(), 0);
    EXPECT_THROW(sb.front(), std::out_of_range);
  }
} 


/*
={=============================================================================

Problem 46, circular buffer, the modern c++ challenge

1. use size(count) and head only
2. in push, no full check since it overwrites and in pop, it simply
calculates first from head substracing size.
3. no iterator support is needed if not use begin()/end()
 
*/

namespace queue_circular_count_iterator
{
  template <typename T>
    class circular_buffer_iterator;

  template <typename T>
    class circular_buffer
    {
      typedef circular_buffer_iterator<T> const_iterator;
      friend class circular_buffer_iterator<T>;

      public:
      circular_buffer() = delete;
      // explicit circular_buffer(size_t const size) : data_(size) 
      explicit circular_buffer(T const size) : data_(size) 
      {}

      void clear() noexcept 
      { head_ = -1; size_ = 0; }

      bool empty() const noexcept
      { return size_ == 0; }

      bool full() const noexcept
      { return size_ >= data_.size(); }

      size_t capacity() const noexcept
      { return data_.size(); }

      size_t size() const noexcept
      { return size_; }

      const_iterator begin() const
      {
        return const_iterator(*this, first_pos(), empty());
      }

      const_iterator end() const
      {
        return const_iterator(*this, next_pos(), true);
      }

      T pop()
      {
        if (empty())
          throw std::runtime_error("buffer is empty");

        auto pos = first_pos();
        size_--;
        return data_[pos];
      }

      void push(T const item)
      {
        // if (full())
        //   throw std::runtime_error("buffer is full");

        head_ = next_pos();
        data_[head_] = item;

        if (size_ < data_.size())
          size_++;
      }

      private:
      // size_t size_{};
      // size_t head_{-1};
      T size_{};
      T head_{-1};
      std::vector<T> data_;

      // return `head` pos to push
      size_t next_pos() const noexcept
      {
        // *cxx-precedence* *cxx-error*
        // return size_ == 0 ? 0 : (head_ + 1 % data_.size());

        return size_ == 0 ? 0 : ((head_ + 1) % data_.size());
      }

      // return `tail` pos to pop
      size_t first_pos() const noexcept
      {
        return size_ == 0 ? 0 : (head_ + data_.size() - size_ + 1) % data_.size();
      }
    };

  template <typename T>
    class circular_buffer_iterator
    {
      typedef circular_buffer_iterator        self_type;
      typedef T                               value_type;
      typedef T&                              reference;
      typedef T const&                        const_reference;
      typedef T*                              pointer;
      typedef std::random_access_iterator_tag iterator_category;
      typedef ptrdiff_t                       difference_type;

      public:

      explicit circular_buffer_iterator(circular_buffer<T> const& buf, size_t const pos, bool const last) :
        buffer_(buf), index_(pos), last_(last)
      {}

      self_type& operator++()
      {
        if (last_)
          throw std::out_of_range("iterator cannot be incremented past the end of range.");

        index_ = (index_ + 1) % buffer_.data_.size();

        // that is when index_ == `head`
        last_ = (index_ == buffer_.next_pos());

        return *this;
      }

      // *cxx-operator-postfix*
      self_type const operator++(int)
      {
        self_type temp = *this;
        ++*this;
        return temp;
      }

      bool operator==(self_type const& other) const
      {
        // assert(compatible(other));

        return &buffer_ == &other.buffer_
          && index_ == other.index_ 
          && last_ == other.last_;
      }

      bool operator!=(self_type const& other) const
      { return !(*this == other); }

      const_reference operator*() const
      {
        return buffer_.data_[index_];
      }

      private:

      circular_buffer<T> const& buffer_;
      size_t index_;
      bool last_;
    };

  template <typename T>
    std::vector<T> print(circular_buffer<T> & buf)
    {
      std::vector<T> coll{};

      for (auto & e : buf)
        coll.push_back(e);

      return coll;
    }

} // namespace

TEST(CxxCase, CircularQueueCountIterator)
{
  using namespace queue_circular_count_iterator;

  {
    circular_buffer<int> cbuf(5);   // {0, 0, 0, 0, 0} -> {}

    cbuf.push(1);                   // {1, 0, 0, 0, 0} -> {1}
    cbuf.push(2);                   // {1, 2, 0, 0, 0} -> {1, 2}
    cbuf.push(3);                   // {1, 2, 3, 0, 0} -> {1, 2, 3}

    auto item = cbuf.pop();         // {1, 2, 3, 0, 0} -> {2, 3}
    EXPECT_THAT(item, 1);

    cbuf.push(4);                   // {1, 2, 3, 4, 0} -> {2, 3, 4}
    cbuf.push(5);                   // {1, 2, 3, 4, 5} -> {2, 3, 4, 5}

    // see that it overwrites
    cbuf.push(6);                   // {(6), 2, 3, 4, 5} -> {2, 3, 4, 5, 6}
    cbuf.push(7);                   // {6, (7), 3, 4, 5} -> {3, 4, 5, 6, 7}
    cbuf.push(8);                   // {6, 7, (8), 4, 5} -> {4, 5, 6, 7, 8}

    item = cbuf.pop();              // {6, 7, 8, 4, 5} -> {5, 6, 7, 8}
    EXPECT_THAT(item, 4);
    item = cbuf.pop();              // {6, 7, 8, 4, 5} -> {6, 7, 8}
    EXPECT_THAT(item, 5);
    item = cbuf.pop();              // {6, 7, 8, 4, 5} -> {7, 8}
    EXPECT_THAT(item, 6);

    cbuf.pop();                     // {6, 7, 8, 4, 5} -> {8}
    cbuf.pop();                     // {6, 7, 8, 4, 5} -> {}
    cbuf.push(9);                   // {6, 7, 8, 9, 5} -> {9}
  }

  {
    circular_buffer<int> cbuf(5);

    cbuf.push(1);
    cbuf.push(2);
    cbuf.push(3);
    cbuf.push(4);
    cbuf.push(5);
    cbuf.push(6);
    cbuf.push(7);
    EXPECT_THAT(print(cbuf), ElementsAre(3,4,5,6,7));
  }
}


/*
={=============================================================================

CXXPP, Quote

1. use size(count) and head only
2. in push, no full check since it overwrites and in pop, it simply
calculates first from head substracing size.
3. no iterator support is needed if not use begin()/end()
 
*/

// when introduce `discount` concept

namespace case_quote
{
  class Quote
  {
    public:

      Quote() : book_no_(), price_(0.0) {}

      Quote(string const& book, double sale_price) :
        book_no_(book), price_(sale_price) {}
      
      virtual ~Quote() {}

      std::string isbn() const { return book_no_; }

      // calculate net price but do not have "discount" concept
      virtual double net_price(size_t count) const { return count * price_; }

    private:
      std::string book_no_;
    protected:
      double price_;
  };

  class Discount_Quote : public Quote
  {
    public:

      // *cxx-ctor*
      // Why need to have constructors in abstract class although cannot
      // define objects of this type directly? Becuase ctors in classes
      // derived from Disc_quote will use the Disc_quote ctor to construct
      // the Disc_quote part of their objects. Default ctor default
      // initialize those members.

      Discount_Quote() : quantity_(0), discount_(0.0) {}
      Discount_Quote(string const& book, double price, 
          size_t quantity, double discount_percent) :
        Quote(book, price), quantity_(quantity), discount_(discount_percent) {}

      // *cxx-dtor*
      // no need to have virtual dtor here since Quote has it already
  
      // *cxx-abc*
      // okay to have in the middle of inheritance and "discount" concept
      virtual double net_price(size_t count) const = 0;

    protected:
      size_t quantity_;
      double discount_;
  };

  class Bulk_Quote : public Discount_Quote
  {
    public:
      Bulk_Quote(string const& book, double price, 
          size_t quantity, double discount_percent) :
        Discount_Quote(book, price, quantity, discount_percent) {}

      // cxx-override cxx-const
      // const is one of override condition

      virtual double net_price(size_t count) const override
      {
        if (count >= quantity_)
          return count * (1 - discount_) * price_;
        else
          return count * price_;
      }
  };

  double print_total(ostream& os, Quote const& item, size_t sold)
  {
    double net_price = item.net_price(sold);

    os << "isbn: " << item.isbn() << ", sold: " << sold 
      << ", total due: " << net_price << endl;

    return net_price;
  }

} // namespace


// To use *gtest-fixture* as a driver than using use user class directly:
//
// class Basket
// {
//     public:
//         Basket() : items(compare) {}
// 
//         void add_item(const shared_ptr<Quote> &item);
// 
//         double total_receipt(ostream &os) const;
// 
//     private:
//         static bool compare(const shared_ptr<Quote> lhs, const shared_ptr<Quote> rhs)
//         { return lhs->isbn() < rhs->isbn(); }
// 
//         // using comp = bool (*)(const shared_ptr<Quote> lhs, const shared_ptr<Quote> rhs);
//         // multiset<shared_ptr<Quote>, comp> items;
// 
//         using comp = bool (const shared_ptr<Quote> lhs, const shared_ptr<Quote> rhs);
//         multiset<shared_ptr<Quote>, comp*> items;
// };
// 
// void Basket::add_item(const shared_ptr<Quote> &item)
// {
//     cout << "basket::add_item::copy version" << endl;
//     items.insert(item);
// }
// 
// double Basket::total_receipt(ostream &os) const
// {
//     for (auto iter = items.cbegin(); iter != items.cend();
//             iter = items.upper_bound(*iter))
//     {
//         os << "isbn : " << (*iter)->isbn() 
//             << ", sold : " << items.count(*iter)
//             << ", total sales: " << (*iter)->net_price( items.count(*iter)) 
//             << endl;
//     }
// }
//
// int main()
// {
//     Basket sale;
// 
//     // Quote sales which has no discount. 45*3 = 135
//     sale.add_item(shared_ptr<Quote>(new Quote("123", 45)));
//     sale.add_item(shared_ptr<Quote>(new Quote("123", 45)));
//     sale.add_item(make_shared<Quote>("123", 45));
// 
//     // minimum 3 and 15% discount. no discount 45*2 = 90
//     sale.add_item(shared_ptr<Quote>(new Bulk_quote("345", 45, 3, .15)));
//     sale.add_item(shared_ptr<Quote>(new Bulk_quote("345", 45, 3, .15)));
// 
//     // Bulk_quote sales which has discount: minimum 3 and 15% discount
//     // 35*4*(1-.15) = 119
//     sale.add_item(shared_ptr<Quote>(new Bulk_quote("678", 35, 3, .15)));
//     sale.add_item(shared_ptr<Quote>(new Bulk_quote("678", 35, 3, .15)));
//     sale.add_item(shared_ptr<Quote>(new Bulk_quote("678", 35, 3, .15)));
//     sale.add_item(shared_ptr<Quote>(new Bulk_quote("678", 35, 3, .15)));
// 
//     // Bulk_quote sales which has discount: minimum 5 and 25% discount
//     // 35*6*(1-.25) = 157.5
//     sale.add_item(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));
//     sale.add_item(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));
//     sale.add_item(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));
//     sale.add_item(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));
//     sale.add_item(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));
//     sale.add_item(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));
// 
//     sale.display(cout);
//     sale.total_receipt(cout);
// }

// *gmock-error*
// must use different name on fixture and other tests
//
// [ RUN      ] CxxCaseQuote.MockQuote
// /home/kyoupark/works/googletest/googletest/src/gtest.cc:2300: Failure
// Failed
// All tests in the same test case must use the same test fixture
// class, so mixing TEST_F and TEST in the same test case is
// illegal.  In test case CxxCaseQuote,
// test CheckTotal_1 is defined using TEST_F but
// test MockQuote is defined using TEST.  You probably
// want to change the TEST to TEST_F or move it to another test
// case.
// [  FAILED  ] CxxCaseQuote.MockQuote (3 ms)


class CxxCaseQuoteX : public ::testing::Test
{
  protected:

    // *cxx-using* is under *cxx-access-control*

    using Quote = case_quote::Quote;

    CxxCaseQuoteX() : items_{compare} {}
    virtual ~CxxCaseQuoteX() {}

    virtual void SetUp() {}
    virtual void TearDown() {}

    double total_receipt() const
    {
      double total{};

      // when there are duplicates, skip them since net_price() gets called for
      // them in single call

      for (auto it = items_.cbegin(); it != items_.cend();
          it = items_.upper_bound(*it))
      {
        total = (*it)->net_price(items_.count(*it));

        cout << "isbn: " << (*it)->isbn()
          << ", sold: " << items_.count(*it)
          << ", total sales: " << total << endl;
      }

      return total;
    }

    static bool compare(std::shared_ptr<Quote> const lhs, std::shared_ptr<Quote> const rhs)
    {
      return lhs->isbn() < rhs->isbn();
    }

    using comp = 
      bool(std::shared_ptr<Quote> const lhs, std::shared_ptr<Quote> const rhs);

    std::multiset<std::shared_ptr<Quote>, comp*> items_;
};

TEST_F(CxxCaseQuoteX, CheckTotal_1)
{
  items_.insert(shared_ptr<Quote>(new Quote("123", 45)));
  items_.insert(shared_ptr<Quote>(new Quote("123", 45)));
  items_.insert(shared_ptr<Quote>(new Quote("123", 45)));

  // Quote sales which has no discount. 45*3 = 135
  EXPECT_THAT(total_receipt(), 135);

  items_.clear();
}

TEST_F(CxxCaseQuoteX, CheckTotal_2)
{
  using namespace case_quote;

  items_.insert(shared_ptr<Quote>(new Bulk_Quote("345", 45, 3, .15)));
  items_.insert(shared_ptr<Quote>(new Bulk_Quote("345", 45, 3, .15)));

  // minimum 3 to have 15% discount. 2 and no discount 45*2 = 90
  EXPECT_THAT(total_receipt(), 90);

  items_.clear();
}

TEST_F(CxxCaseQuoteX, CheckTotal_3)
{
  using namespace case_quote;

  items_.insert(shared_ptr<Quote>(new Bulk_Quote("678", 35, 3, .15)));
  items_.insert(shared_ptr<Quote>(new Bulk_Quote("678", 35, 3, .15)));
  items_.insert(shared_ptr<Quote>(new Bulk_Quote("678", 35, 3, .15)));
  items_.insert(shared_ptr<Quote>(new Bulk_Quote("678", 35, 3, .15)));

  // Bulk_quote sales which has discount: minimum 3 and 15% discount
  // 35*4*(1-.15) = 119
  EXPECT_EQ(total_receipt(), 119);

  items_.clear();
}

TEST_F(CxxCaseQuoteX, CheckTotal_4)
{
  using namespace case_quote;

  items_.insert(shared_ptr<Quote>(new Bulk_Quote("912", 35, 5, .25)));
  items_.insert(shared_ptr<Quote>(new Bulk_Quote("912", 35, 5, .25)));
  items_.insert(shared_ptr<Quote>(new Bulk_Quote("912", 35, 5, .25)));
  items_.insert(shared_ptr<Quote>(new Bulk_Quote("912", 35, 5, .25)));
  items_.insert(shared_ptr<Quote>(new Bulk_Quote("912", 35, 5, .25)));
  items_.insert(shared_ptr<Quote>(new Bulk_Quote("912", 35, 5, .25)));

  // Bulk_quote sales which has discount: minimum 5 and 25% discount
  // 35*6*(1-.25) = 157.5
  EXPECT_EQ(157.5, total_receipt());

  items_.clear();
}


// To test user, driver class, Basket.

namespace case_quote
{
  class MockQuote : public Quote
  {
    public:
      MockQuote(string const& book, double sale_price)
        : Quote(book, sale_price)
      {}

      MOCK_CONST_METHOD1(net_price, double (size_t count));
  };

  class MockBulk : public Bulk_Quote
  {
    public:
      MockBulk(string const& book, double price, 
          size_t quantity, double discount_percent)
        : Bulk_Quote(book, price, quantity, discount_percent)
      {}

      MOCK_CONST_METHOD1(net_price, double (size_t count));
  };

  class Basket
  {
    public:
      // *cxx-undefined*
      // Without it, cause core when add_item is called.
      Basket() : items_{compare}
      {}

      void add_item(std::shared_ptr<Quote> const& item)
      {
        cout << "basket::add_item(shard_ptr)" << endl;
        items_.insert(item);
      }

      double total_receipt(ostream& os) const
      {
        double total{};

        for (auto iter = items_.cbegin(); iter != items_.cend();
            iter = items_.upper_bound(*iter))
        {
          total = (*iter)->net_price(items_.count(*iter));
          os << "isbn: " << (*iter)->isbn()
            << ", sold: " << items_.count(*iter)
            << ", total sales: " << total << endl;
        }

        return total;
      }

    private:
      static bool compare(const std::shared_ptr<Quote> lhs,
          const std::shared_ptr<Quote> rhs)
      { return lhs->isbn() < rhs->isbn(); }

      using comp = bool(const std::shared_ptr<Quote>,
          const std::shared_ptr<Quote>);

      std::multiset<shared_ptr<Quote>, comp*> items_;
  };

} // namespace


TEST(CxxCaseQuote, MockQuote)
{
  using namespace case_quote;

  // Quote sales which has no discount, 45*3 = 135
  std::shared_ptr<MockQuote> q1(new MockQuote("123", 45));
  std::shared_ptr<MockQuote> q2(new MockQuote("123", 45));
  std::shared_ptr<MockQuote> q3(new MockQuote("123", 45));

  EXPECT_CALL(*q1, net_price(_))
    .WillOnce(Return(135));

  // when expects that net_price() gets called on q2 and q3 since net_price()
  // will be called once.
  //
  // .cpp:169: Failure
  // Actual function call count doesn't match EXPECT_CALL(*q3, net_price(_))...
  //          Expected: to be called once
  //            Actual: never called - unsatisfied and active
  // t_ex_basket.cpp:166: Failure
  // Actual function call count doesn't match EXPECT_CALL(*q2, net_price(_))...
  //          Expected: to be called once
  //            Actual: never called - unsatisfied and active
  // [  FAILED  ] BasketTest.checkTotal1 (2 ms)
  //
  // EXPECT_CALL(*q2, net_price(_))
  //     .WillOnce(Return(135));
  //
  // EXPECT_CALL(*q3, net_price(_))
  //     .WillOnce(Return(135));
 
  // so change "Times" or comment them out
  //
  // EXPECT_CALL(*q2, net_price(_))
  //   .Times(AnyNumber())
  //   .WillOnce(Return(135));
  //
  // EXPECT_CALL(*q3, net_price(_))
  //   .Times(AnyNumber())
  //   .WillOnce(Return(135));

  Basket sale;

  sale.add_item(q1);
  sale.add_item(q2);
  sale.add_item(q3);

  EXPECT_EQ(sale.total_receipt(cout), 135); 
}

TEST(CxxCaseQuote, MockBulk)
{
  using namespace case_quote;

  // Bulk_quote sales which has discount: minimum 5 and 25% discount
  // 35*6*(1-.25) = 157.5
  shared_ptr<MockBulk> q1(new MockBulk("912", 35, 5, .25));
  shared_ptr<MockBulk> q2(new MockBulk("912", 35, 5, .25));
  shared_ptr<MockBulk> q3(new MockBulk("912", 35, 5, .25));
  shared_ptr<MockBulk> q4(new MockBulk("912", 35, 5, .25));
  shared_ptr<MockBulk> q5(new MockBulk("912", 35, 5, .25));

  EXPECT_CALL(*q1, net_price(_))
    .WillOnce(Return(157.5));

  Basket sale;

  sale.add_item(q1);
  sale.add_item(q2);
  sale.add_item(q3);
  sale.add_item(q4);
  sale.add_item(q5);

  EXPECT_EQ(sale.total_receipt(cout), 157.5);
}


namespace case_quote_clone
{
  class Quote
  {
    public:

      Quote() : book_no_(), price_(0.0) {}

      Quote(string const& book, double sale_price) :
        book_no_(book), price_(sale_price) {}
      
      virtual ~Quote() {}

      std::string isbn() const { return book_no_; }

      // calculate net price but do not have "discount" concept
      virtual double net_price(size_t count) const { return count * price_; }

      // *cxx-clone* *cxx-move* copy version
      // *cxx-reference-qualifier*
      // note: shall have `const or see *cxx-const-to-nonconst-error*
      // note: if there's no move verison, copy version will be used instead
      virtual Quote* clone() const &
      {
        cout << "quote::clone() &" << endl;
        return new Quote(*this);
      }

      // cxx-clone, move version
      virtual Quote* clone() const &&
      {
        cout << "quote::clone() &&" << endl;
        return new Quote(std::move(*this));
      }

    private:
      std::string book_no_;
    protected:
      double price_;
  };

  class Discount_Quote : public Quote
  {
    public:

      // *cxx-ctor*
      // Why need to have constructors in abstract class although cannot
      // define objects of this type directly? Becuase ctors in classes
      // derived from Disc_quote will use the Disc_quote ctor to construct
      // the Disc_quote part of their objects. Default ctor default
      // initialize those members.

      Discount_Quote() : quantity_(0), discount_(0.0) {}
      Discount_Quote(string const& book, double price, 
          size_t quantity, double discount_percent) :
        Quote(book, price), quantity_(quantity), discount_(discount_percent) {}

      // *cxx-dtor*
      // no need to have virtual dtor here since Quote has it already
  
      // *cxx-abc*
      // okay to have in the middle of inheritance and "discount" concept
      virtual double net_price(size_t count) const = 0;

    protected:
      size_t quantity_;
      double discount_;
  };

  class Bulk_Quote : public Discount_Quote
  {
    public:
      Bulk_Quote(string const& book, double price, 
          size_t quantity, double discount_percent) :
        Discount_Quote(book, price, quantity, discount_percent) {}

      // cxx-override cxx-const
      // const is one of override condition

      virtual double net_price(size_t count) const override
      {
        if (count >= quantity_)
          return count * (1 - discount_) * price_;
        else
          return count * price_;
      }

      // cxx-clone
      // cxx-error virtual Bulk_Quote* clone() const override &
      // cxx-const
      // if don't have const, no error but no override. so use override keyword
      virtual Bulk_Quote* clone() const& override
      {
        cout << "bulk_quote::clone() &" << endl;
        return new Bulk_Quote(*this);
      }

      virtual Bulk_Quote* clone() const&& override
      {
        cout << "bulk_quote::clone() &&" << endl;
        return new Bulk_Quote(std::move(*this));
      }
  };

  double print_total(ostream& os, Quote const& item, size_t sold)
  {
    double net_price = item.net_price(sold);

    os << "isbn: " << item.isbn() << ", sold: " << sold 
      << ", total due: " << net_price << endl;

    return net_price;
  }


  class Basket
  {
    public:
      // *cxx-undefined*
      // Without it, cause core when add_item is called.
      Basket() : items_{compare}
      {}

      void add_item(std::shared_ptr<Quote> const& item)
      {
        cout << "basket::add_item(shard_ptr)" << endl;
        items_.insert(item);
      }

      // copy version
      void add_item(Quote const& item)
      {
        cout << "basket::add_item(Quote const&)" << endl;
        items_.insert(std::shared_ptr<Quote>(
              item.clone()));
      }

      // move version *cxx-dynamic-binding*
      void add_item(Quote &&item)
      {
        cout << "basket::add_item(Quote &&)" << endl;
        items_.insert(std::shared_ptr<Quote>(
              std::move(item).clone()));
      }

      double total_receipt(ostream& os) const
      {
        double total{};

        for (auto iter = items_.cbegin(); iter != items_.cend();
            iter = items_.upper_bound(*iter))
        {
          total = (*iter)->net_price(items_.count(*iter));
          os << "isbn: " << (*iter)->isbn()
            << ", sold: " << items_.count(*iter)
            << ", total sales: " << total << endl;
        }

        return total;
      }

    private:
      static bool compare(const std::shared_ptr<Quote> lhs,
          const std::shared_ptr<Quote> rhs)
      { return lhs->isbn() < rhs->isbn(); }

      using comp = bool(const std::shared_ptr<Quote>,
          const std::shared_ptr<Quote>);

      std::multiset<shared_ptr<Quote>, comp*> items_;
  };

} // namespace

// [ RUN      ] CxxCaseQuote.Clone
// basket::add_item(Quote &&)
// quote::clone() &&
// basket::add_item(Quote &&)
// quote::clone() &&
// basket::add_item(Quote &&)
// quote::clone() &&
// basket::add_item(Quote &&)
// bulk_quote::clone() &&
// basket::add_item(Quote &&)
// bulk_quote::clone() &&
// isbn: 123, sold: 3, total sales: 135
// isbn: 345, sold: 2, total sales: 90
// [       OK ] CxxCaseQuote.Clone (2 ms)

TEST(CxxCaseQuote, Clone)
{
  using namespace case_quote_clone;

  Basket sale;

  // Quote sales which has no discount. 45*3 = 135
  sale.add_item(Quote("123", 45));
  sale.add_item(Quote("123", 45));
  sale.add_item(Quote("123", 45));

  // minimum 3 and 15% discount. no discount 45*2 = 90
  sale.add_item(Bulk_Quote("345", 45, 3, .15));
  sale.add_item(Bulk_Quote("345", 45, 3, .15));

  sale.total_receipt(cout);
}


// ={=========================================================================
int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

