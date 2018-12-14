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


// StrBlob without StrBlobPtr
//
// CPR 455
//
// * show how to implement a new collection type using library container.
// `is-implemented-in-terms-of-model`
//
// * This shows when shared_ptr is useful than own raii since this case don't
// need `copy-controls` and use syn versions but not a problem as it has only
// one member which is shared_ptr.

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

// from Problem 46, circular buffer, the modern c++ challenge
// 1. use size(count) and head only
// 2. in push, no full check since it overwrites and in pop, it simply
// calculates first from head substracing size.
// 3. no iterator support is needed if not use begin()/end()

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
  explicit circular_buffer(size_t const size) : data_(size) 
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
  size_t size_{};
  size_t head_{-1};
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


// ={=========================================================================

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

