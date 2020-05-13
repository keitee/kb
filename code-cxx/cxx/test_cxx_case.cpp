#include <boost/lexical_cast.hpp>
#include <chrono>
#include <condition_variable>
#include <forward_list>
#include <iostream>
#include <limits>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <regex>
#include <set>
#include <thread>
#include <vector>

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

    explicit StrBlob()
        : data_(new std::vector<std::string>())
    {}

    explicit StrBlob(std::initializer_list<std::string> il)
        : data_(new std::vector<std::string>(il))
    {}

    size_type size() const noexcept { return data_->size(); }

    bool empty() const noexcept { return data_->empty(); }

    void push_back(std::string const &elem) { data_->push_back(elem); }

    void pop_back()
    {
      // *cxx-vector* *cxx-undefined* when called on empty vector
      check(0, "coll is empty");

      // *cxx-vector* do not return value
      data_->pop_back();
    }

    std::string &front()
    {
      check(0, "coll is empty");
      return data_->front();
    }

    std::string &back()
    {
      check(0, "coll is empty");
      return data_->back();
    }

  private:
    shared_ptr<std::vector<std::string>> data_;

    // *cxx-except* has nothing to do with return type
    void check(size_type index, string const &message) const
    {
      if (index >= data_->size())
        throw std::out_of_range(message);
    }
  };

} // namespace cxx_case_strblob

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

    explicit StrBlob()
        : data_(new std::vector<std::string>())
    {}

    explicit StrBlob(std::initializer_list<std::string> il)
        : data_(new std::vector<std::string>(il))
    {}

    size_type size() const noexcept { return data_->size(); }

    bool empty() const noexcept { return data_->empty(); }

    void push_back(std::string const &elem) { data_->push_back(elem); }

    void pop_back()
    {
      // *cxx-vector* *cxx-undefined* when called on empty vector
      check(0, "coll is empty");

      // *cxx-vector* do not return value
      data_->pop_back();
    }

    std::string &front()
    {
      check(0, "coll is empty");
      return data_->front();
    }

    std::string &back()
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
    void check(size_type index, string const &message) const
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
    StrBlobIterator(StrBlob &blob, size_type pos = 0)
        : blob_(blob.data_)
        , curr_(pos)
    {}

    // *cxx-operator-postfix* prefix version
    StrBlobIterator &operator++()
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

    StrBlobIterator &operator--()
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

    std::string &operator*() const
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

    bool operator==(StrBlobIterator const &other) const
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

    bool operator!=(StrBlobIterator const &other) const
    {
      return !(*this == other);
    }

  private:
    std::weak_ptr<std::vector<std::string>> blob_;
    size_type curr_{0};

    std::shared_ptr<std::vector<std::string>> check(size_type index,
                                                    string const &message) const
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

  StrBlobIterator StrBlob::begin() { return StrBlobIterator(*this, 0); }

  StrBlobIterator StrBlob::end()
  {
    return StrBlobIterator(*this, data_->size());
  }

} // namespace cxx_case_strblob_iterator

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

    for (auto const &e : sb)
      coll.push_back(e);

    EXPECT_THAT(coll, ElementsAre("one", "two", "three", "four"));
  }

  {
    vector<string> coll;

    StrBlob sb{"one", "two", "three", "four"};

    // since operator*() returns reference
    auto it = sb.begin();
    *it     = "I";

    for (auto const &e : sb)
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

    explicit StrBlob()
        : data_(new std::vector<T>())
    {}

    explicit StrBlob(std::initializer_list<T> il)
        : data_(new std::vector<T>(il))
    {}

    size_type size() const noexcept { return data_->size(); }

    bool empty() const noexcept { return data_->empty(); }

    void push_back(std::string const &elem) { data_->push_back(elem); }

    void pop_back()
    {
      // *cxx-vector* *cxx-undefined* when called on empty vector
      check(0, "coll is empty");

      // *cxx-vector* do not return value
      data_->pop_back();
    }

    T &front()
    {
      check(0, "coll is empty");
      return data_->front();
    }

    T &back()
    {
      check(0, "coll is empty");
      return data_->back();
    }

  private:
    shared_ptr<std::vector<T>> data_;

    void check(size_type index, string const &message) const;
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
  void StrBlob<T>::check(size_type index, string const &message) const
  {
    if (index >= data_->size())
      throw std::out_of_range(message);
  }

} // namespace cxx_case_strblob_template

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
StrVec

CPR 524

StrVec example which is a simplification of the library vector class. Like
vector, supports reallocation. If there is not enough space left, then obtains
new space, moves the existing elements into that space, free the old space, and
adds the new elements.

| 0 | 1 | 2 | 3 | 4 | unconstructed std::string elements |
^                     ^                                  ^
elements              first_free                         cap

*/

namespace cxx_case_strvec
{
  class StrVec
  {
  private:
    using size_type = std::string::size_type;

    // TODO: why static in the text?
    std::allocator<std::string> m_alloc;

    std::string *m_element{nullptr};
    std::string *m_free{nullptr};
    std::string *m_capacity{nullptr};

    // allocate and construct memory copying from [begin, end) and return
    // [begin, end) of the constructed.

    std::pair<std::string *, std::string *>
    alloc_and_copy_(const std::string *begin, const std::string *end)
    {
      // use iterator arithmetic
      auto data = m_alloc.allocate(end - begin);
      return {data, std::uninitialized_copy(begin, end, data)};
    }

    // free [element, cap) only when elemenet is not null
    // move backwards from 'free' to call dtor on [element, free)
    //
    // NOTE: needs destroy()? Yes since std::string is `valuelike` and has own
    // memory for holding characters

    void free_()
    {
      if (m_element)
      {
        for (auto p = m_free; p != m_element; /* empty */)
          m_alloc.destroy(--p);

        m_alloc.deallocate(m_element, m_capacity - m_element);
      }
    }

    // o cxx-move
    // move the data from the old memory to the new std::move() returns rvalue,
    // which cause construct() to use string move ctor.

    // o cxx-gdb
    // seg-fault when use `element` in the loop:
    //
    // for (size_t i = 0; i != size(); ++i)
    //     alloc.construct(dest++, std::move(*element_++));
    //
    // *gdb-debug* bt when use -g
    // Program terminated with signal SIGSEGV, Segmentation fault.
    // #0  0x00007f94aa120113 in std::basic_string<char, std::char_traits<char>, std::allocator<char> >::basic_string(std::string&&) () from /usr/lib/x86_64-linux-gnu/libstdc++.so.6
    // (gdb) bt
    // #0  0x00007f94aa120113 in std::basic_string<char, std::char_traits<char>, std::allocator<char> >::basic_string(std::string&&) () from /usr/lib/x86_64-linux-gnu/libstdc++.so.6
    // #1  0x0000000000401bf7 in __gnu_cxx::new_allocator<std::string>::construct<std::string<std::string> > (this=0x602cd1 <StrVec::alloc>, __p=0xa53000) at /usr/include/c++/4.9/ext/new_allocator.h:120
    // #2  0x0000000000401932 in StrVec::reallocate (this=0x7ffdc9026bc0) at t_ex_strvec.cpp:117
    // #3  0x0000000000401aa5 in StrVec::check_and_alloc (this=0x7ffdc9026bc0) at t_ex_strvec.cpp:147
    // #4  0x000000000040165e in StrVec::push_back (this=0x7ffdc9026bc0, s="two") at t_ex_strvec.cpp:44
    // #5  0x000000000040133e in main () at t_ex_strvec.cpp:198
    //
    // when not use -g
    // (gdb) bt
    // #0  0x00007f3348f5c113 in std::basic_string<char, std::char_traits<char>, std::allocator<char> >::basic_string(std::string&&) () from /usr/lib/x86_64-linux-gnu/libstdc++.so.6
    // #1  0x0000000000401bf7 in void __gnu_cxx::new_allocator<std::string>::construct<std::string, std::string>(std::string*, std::string&&) ()
    // #2  0x0000000000401932 in StrVec::reallocate() ()
    // #3  0x0000000000401aa5 in StrVec::check_and_alloc() ()
    // #4  0x000000000040165e in StrVec::push_back(std::string const&) ()
    // #5  0x000000000040133e in main ()
    //
    // How to debug? See that uses 'construct' and gdb is useful to see
    // what's going on when stepping through. Found out that the loop
    // continues and saw that when add to print i and size().
    //
    // ...
    // i: 16856, size: 18446744073709534761
    // Segmentation fault (core dumped)
    //
    // (gdb) f 2
    // #2  0x0000000000401932 in StrVec::reallocate (this=0x7ffdc9026bc0) at t_ex_strvec.cpp:117
    // 117                     alloc.construct(dest++, std::move(*element_++));
    // (gdb) p i
    // $1 = 16856
    // (gdb) p/u free_-element_
    // $6 = 18446744073709534760
    //
    // Why? Since element_ is member data and keep increasing it, then size()
    // member function would produce negative which turns into big number sicne
    // size() returns size_t, unsigned int.

    void reallocate_()
    {
      auto new_capacity = size() ? size() * 2 : 1;
      auto new_data     = m_alloc.allocate(new_capacity);

      auto dest   = new_data;
      auto source = m_element;

      for (size_type i = 0; i != size(); ++i)
      {
        m_alloc.construct(dest++, std::move(*source++));
      }

      m_element  = new_data;
      m_free     = dest;
      m_capacity = m_element + new_capacity;
    }

    // check if it needs reallocation

    void check_and_alloc_()
    {
      if (size() == capacity())
      {
        reallocate_();
      }
    }

  public:
    StrVec() = default;
    ~StrVec() { free_(); }

    StrVec(std::initializer_list<std::string> il)
    {
      auto coll = alloc_and_copy_(il.begin(), il.end());
      m_element = coll.first;
      m_free = m_capacity = coll.second;
    }

    // copy controls. makes a copy
    StrVec(const StrVec &rhs)
    {
      std::cout << "StrVec(const StrVec &rhs)" << std::endl;
      auto coll = alloc_and_copy_(rhs.begin(), rhs.end());
      m_element = coll.first;
      m_free = m_capacity = coll.second;
    }

    // copy controls. makes a copy and also support cxx-self-assian.
    StrVec &operator=(const StrVec &rhs)
    {
      if (this != &rhs)
      {
        std::cout << "StrVec &operator=(const StrVec &rhs)" << std::endl;
        auto coll = alloc_and_copy_(rhs.begin(), rhs.end());
        free_();
        m_element = coll.first;
        m_free = m_capacity = coll.second;
        return *this;
      }

      return *this;
    }

#ifdef MOVE_SUPPORTED
    // *cxx-move* controls. no need to alloc
    StrVec(StrVec &&rhs) noexcept
        : m_element(rhs.m_element)
        , m_free(rhs.m_free)
        , m_capacity(rhs.m_capacity)
    {
      std::cout << "StrVec(StrVec &&rhs)" << std::endl;
      rhs.m_element = rhs.m_free = rhs.m_capacity = nullptr;
    }

    // move controls. no need to alloc
    StrVec &operator=(StrVec &&rhs) noexcept
    {
      if (this != &rhs)
      {
        std::cout << "StrVec &operator=(StrVec &&rhs)" << std::endl;
        free_();
        m_element  = rhs.m_element;
        m_free     = rhs.m_free;
        m_capacity = rhs.m_capacity;

        rhs.m_element = rhs.m_free = rhs.m_capacity = nullptr;
      }

      return *this;
    }
#endif

  public:
    std::string &operator[](size_type n) { return m_element[n]; }

    size_type size() const { return m_free - m_element; }
    size_type capacity() const { return m_capacity - m_element; }

    std::string *begin() const { return m_element; }
    std::string *end() const { return m_free; }

    const std::string *cbegin() const { return m_element; }
    const std::string *cend() const { return m_free; }

    // *cxx-copy*
    void push_back(const std::string &s)
    {
      check_and_alloc_();

      // TODO: construct a copy of s
      //
      // The first argument to construct must be a pointer to
      // unconstructed space allocated by `allocate()`.
      //
      // The second argument `determine which constructor` to use to
      // construct the object in that space. This is string's copy ctor.

      m_alloc.construct(m_free++, s);
    }

    // *cxx-move-overload*
    //
    // The *cxx-std-move* is to signal to compiler to pick up move operations
    // via resolution. The std::move() doesn't itself do any moving, but merely
    // converts its argument into a so-called rvalue reference.

    void push_back(std::string &&s)
    {
      check_and_alloc_();
      m_alloc.construct(m_free++, std::move(s));
    }

    // *cxx-emplace-back* is covered in chapter 16
    template <typename... Args>
    void emplace_back(Args &&... args)
    {
      check_and_alloc_();
      m_alloc.construct(m_free++, std::forward<Args>(args)...);
    }
  };

  StrVec returnVector()
  {
    StrVec coll;

    coll.push_back("func1");
    coll.push_back("func2");
    coll.push_back("func3");

    // this is where move is necessary
    return coll;
  }
} // namespace cxx_case_strvec

TEST(CxxCase, StringVector)
{
  using namespace cxx_case_strvec;

  {
    StrVec coll{"string1", "string2", "string3"};

    {
      // since T, std::string, supports streams
      ostringstream os;
      for (const auto &e : coll)
        os << e << ",";

      EXPECT_THAT(os.str(), "string1,string2,string3,");
    }

    // operator[]
    {
      EXPECT_THAT(coll[0], "string1");

      coll[0] = "element1";
      coll[1] = "element2";
      coll[2] = "element3";

      // since T, std::string, supports streams
      ostringstream os;
      for (const auto &e : coll)
        os << e << ",";

      EXPECT_THAT(os.str(), "element1,element2,element3,");
    }

    // push_back
    {
      coll.push_back("push1");
      coll.push_back("push2");

      // since T, std::string, supports streams
      ostringstream os;
      for (const auto &e : coll)
        os << e << ",";

      EXPECT_THAT(os.str(), "element1,element2,element3,push1,push2,");
    }

    // copy
    {
      StrVec coll2;
      coll2 = coll;

      // since T, std::string, supports streams
      ostringstream os;
      for (const auto &e : coll)
        os << e << ",";

      EXPECT_THAT(os.str(), "element1,element2,element3,push1,push2,");
      EXPECT_THAT(coll.size(), 5);

      // stringstream-clear
      os = ostringstream("");
      for (const auto &e : coll2)
        os << e << ",";

      EXPECT_THAT(os.str(), "element1,element2,element3,push1,push2,");
      EXPECT_THAT(coll2.size(), 5);
    }

    // move
    {
      StrVec coll2;
      coll2 = std::move(coll);

      EXPECT_THAT(coll2.size(), 5);

#ifdef MOVE_SUPPORTED
      EXPECT_THAT(coll.size(), 0);
#else
      // copy will be used instead
      EXPECT_THAT(coll.size(), 5);
#endif

      ostringstream os;
      for (const auto &e : coll2)
        os << e << ",";

      EXPECT_THAT(os.str(), "element1,element2,element3,push1,push2,");
      EXPECT_THAT(coll2.size(), 5);
    }

    // cxx-emplace-back
    {
      // coll is still around

      coll.emplace_back("string1");
      coll.emplace_back(5, 'c'); // add "ccccc"

      std::string s1("string2");
      std::string s2("string3");
      coll.emplace_back(s1 + s2); // use string move

#ifdef MOVE_SUPPORTED
      EXPECT_THAT(coll.size(), 3);
#else
      EXPECT_THAT(coll.size(), 8);
#endif
      ostringstream os;
      for (const auto &e : coll)
        os << e << ",";

#ifdef MOVE_SUPPORTED
      EXPECT_THAT(os.str(), "string1,ccccc,string2string3,");
#else
      EXPECT_THAT(
        os.str(),
        "element1,element2,element3,push1,push2,string1,ccccc,string2string3,");
#endif
    }

    // cxx-return
    {
      StrVec coll2;
      EXPECT_THAT(coll2.size(), 0);

      // will use cxx-copy-assign or cxx-move-assign? Use cxx-move
      coll2 = returnVector();

      EXPECT_THAT(coll2.size(), 3);
    }
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
    explicit circular_buffer(T const size)
        : data_(size)
    {}

    void clear() noexcept
    {
      head_ = -1;
      size_ = 0;
    }

    bool empty() const noexcept { return size_ == 0; }

    bool full() const noexcept { return size_ >= data_.size(); }

    size_t capacity() const noexcept { return data_.size(); }

    size_t size() const noexcept { return size_; }

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

      head_        = next_pos();
      data_[head_] = item;

      if (size_ < (T)(data_.size()))
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
    typedef circular_buffer_iterator self_type;
    typedef T value_type;
    typedef T &reference;
    typedef T const &const_reference;
    typedef T *pointer;
    typedef std::random_access_iterator_tag iterator_category;
    typedef ptrdiff_t difference_type;

  public:
    explicit circular_buffer_iterator(circular_buffer<T> const &buf,
                                      size_t const pos,
                                      bool const last)
        : buffer_(buf)
        , index_(pos)
        , last_(last)
    {}

    self_type &operator++()
    {
      if (last_)
        throw std::out_of_range(
          "iterator cannot be incremented past the end of range.");

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

    bool operator==(self_type const &other) const
    {
      // assert(compatible(other));

      return &buffer_ == &other.buffer_ && index_ == other.index_ &&
             last_ == other.last_;
    }

    bool operator!=(self_type const &other) const { return !(*this == other); }

    const_reference operator*() const { return buffer_.data_[index_]; }

  private:
    circular_buffer<T> const &buffer_;
    size_t index_;
    bool last_;
  };

  template <typename T>
  std::vector<T> print(circular_buffer<T> &buf)
  {
    std::vector<T> coll{};

    for (auto &e : buf)
      coll.push_back(e);

    return coll;
  }

} // namespace queue_circular_count_iterator

TEST(CxxCase, CircularQueueCountIterator)
{
  using namespace queue_circular_count_iterator;

  {
    circular_buffer<int> cbuf(5); // {0, 0, 0, 0, 0} -> {}

    cbuf.push(1); // {1, 0, 0, 0, 0} -> {1}
    cbuf.push(2); // {1, 2, 0, 0, 0} -> {1, 2}
    cbuf.push(3); // {1, 2, 3, 0, 0} -> {1, 2, 3}

    auto item = cbuf.pop(); // {1, 2, 3, 0, 0} -> {2, 3}
    EXPECT_THAT(item, 1);

    cbuf.push(4); // {1, 2, 3, 4, 0} -> {2, 3, 4}
    cbuf.push(5); // {1, 2, 3, 4, 5} -> {2, 3, 4, 5}

    // see that it overwrites
    cbuf.push(6); // {(6), 2, 3, 4, 5} -> {2, 3, 4, 5, 6}
    cbuf.push(7); // {6, (7), 3, 4, 5} -> {3, 4, 5, 6, 7}
    cbuf.push(8); // {6, 7, (8), 4, 5} -> {4, 5, 6, 7, 8}

    item = cbuf.pop(); // {6, 7, 8, 4, 5} -> {5, 6, 7, 8}
    EXPECT_THAT(item, 4);
    item = cbuf.pop(); // {6, 7, 8, 4, 5} -> {6, 7, 8}
    EXPECT_THAT(item, 5);
    item = cbuf.pop(); // {6, 7, 8, 4, 5} -> {7, 8}
    EXPECT_THAT(item, 6);

    cbuf.pop();   // {6, 7, 8, 4, 5} -> {8}
    cbuf.pop();   // {6, 7, 8, 4, 5} -> {}
    cbuf.push(9); // {6, 7, 8, 9, 5} -> {9}
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
    EXPECT_THAT(print(cbuf), ElementsAre(3, 4, 5, 6, 7));
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
  private:
    std::string book_no_{};

  protected:
    double price_;

  public:
    Quote()
        : book_no_()
        , price_(0.0)
    {}

    Quote(string const &book, double sale_price)
        : book_no_(book)
        , price_(sale_price)
    {}

    virtual ~Quote() {}

    std::string isbn() const { return book_no_; }

    // calculate net price but do not have "discount" concept
    virtual double net_price(size_t count) const { return count * price_; }
  };

  class Discount_Quote : public Quote
  {
  protected:
    size_t quantity_;
    double discount_;

  public:
    // *cxx-ctor*
    // Why need to have constructors in abstract class although cannot define
    // objects of this type directly? Becuase ctors in classes derived from
    // Discount_quote will use the Discount_quote ctor to construct the
    // Disc_quote part of their objects. Default ctor default initialize those
    // members.

    Discount_Quote()
        : quantity_(0)
        , discount_(0.0)
    {}

    Discount_Quote(string const &book,
                   double price,
                   size_t quantity,
                   double discount_percent)
        : Quote(book, price)
        , quantity_(quantity)
        , discount_(discount_percent)
    {}

    // *cxx-dtor*
    // no need to have virtual dtor here since Quote has it already

    // *cxx-abc*
    // okay to have in the middle of inheritance and "discount" concept
    virtual double net_price(size_t count) const = 0;
  };

  class Bulk_Quote : public Discount_Quote
  {
  public:
    Bulk_Quote(string const &book,
               double price,
               size_t quantity,
               double discount_percent)
        : Discount_Quote(book, price, quantity, discount_percent)
    {}

    // cxx-override cxx-const
    // const is one of override conditions

    double net_price(size_t count) const override
    {
      if (count >= quantity_)
        return count * (1 - discount_) * price_;
      else
        return count * price_;
    }
  };

  double print_total(ostream &os, Quote const &item, size_t sold)
  {
    double net_price = item.net_price(sold);

    os << "isbn: " << item.isbn() << ", sold: " << sold
       << ", total due: " << net_price << endl;

    return net_price;
  }
} // namespace case_quote

#if 0
when use it directly than using *gtest-fixture*:

class Basket
{
  public:
    Basket() : items(compare) {}

    void add_item(const shared_ptr<Quote> &item);

    double total_receipt(ostream &os) const;

  private:
    static bool compare(const shared_ptr<Quote> lhs, const shared_ptr<Quote> rhs) 
    { 
      return lhs->isbn() < rhs->isbn(); 
    }

    // using comp = bool (*)(const shared_ptr<Quote> lhs, const shared_ptr<Quote> rhs);
    // multiset<shared_ptr<Quote>, comp> items;

    using comp = bool (const shared_ptr<Quote> lhs, const shared_ptr<Quote> rhs); 

    multiset<shared_ptr<Quote>, comp*> items;
};

void Basket::add_item(const shared_ptr<Quote> &item)
{
  cout << "basket::add_item::copy version" << endl;
  items.insert(item);
}

double Basket::total_receipt(ostream &os) const
{
  for (auto iter = items.cbegin(); 
      iter != items.cend();
      iter = items.upper_bound(*iter))
  {
    os << "isbn : " << (*iter)->isbn()
      << ", sold : " << items.count(*iter)
      << ", total sales: " << (*iter)->net_price( items.count(*iter))
      << endl;
  }
}

int main()
{
   Basket sale;

   // Quote sales which has no discount. 45*3 = 135
   sale.add_item(shared_ptr<Quote>(new Quote("123", 45)));
   sale.add_item(shared_ptr<Quote>(new Quote("123", 45)));
   sale.add_item(make_shared<Quote>("123", 45));

   // minimum 3 and 15% discount. no discount 45*2 = 90
   sale.add_item(shared_ptr<Quote>(new Bulk_quote("345", 45, 3, .15)));
   sale.add_item(shared_ptr<Quote>(new Bulk_quote("345", 45, 3, .15)));

   // Bulk_quote sales which has discount: minimum 3 and 15% discount
   // 35*4*(1-.15) = 119
   sale.add_item(shared_ptr<Quote>(new Bulk_quote("678", 35, 3, .15)));
   sale.add_item(shared_ptr<Quote>(new Bulk_quote("678", 35, 3, .15)));
   sale.add_item(shared_ptr<Quote>(new Bulk_quote("678", 35, 3, .15)));
   sale.add_item(shared_ptr<Quote>(new Bulk_quote("678", 35, 3, .15)));

   // Bulk_quote sales which has discount: minimum 5 and 25% discount
   // 35*6*(1-.25) = 157.5
   sale.add_item(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));
   sale.add_item(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));
   sale.add_item(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));
   sale.add_item(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));
   sale.add_item(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));
   sale.add_item(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));

   sale.display(cout);
   sale.total_receipt(cout);
}
#endif

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

// use fixture and real quotes

class CxxCaseQuoteX : public ::testing::Test
{
protected:
  // *cxx-using* is under *cxx-access-control*

  using Quote = case_quote::Quote;

  CxxCaseQuoteX()
      : items_{compare}
  {}

  virtual ~CxxCaseQuoteX() {}

  virtual void SetUp() {}
  virtual void TearDown() {}

  double total_receipt() const
  {
    double total{};

    for (auto it = items_.cbegin(); it != items_.cend();
         it      = items_.upper_bound(*it))
    {
      // when there are duplicates, skip them since net_price() gets called for
      // them in single call with count(num of books)

      total = (*it)->net_price(items_.count(*it));

      cout << "isbn: " << (*it)->isbn() << ", sold: " << items_.count(*it)
           << ", total sales: " << total << endl;
    }

    return total;
  }

  // why not const &?
  // static bool compare(const std::shared_ptr<Quote> lhs,
  //                     const std::shared_ptr<Quote> rhs)
  // {
  //   return lhs->isbn() < rhs->isbn();
  // }

  static bool compare(const std::shared_ptr<Quote> &lhs,
                      const std::shared_ptr<Quote> &rhs)
  {
    return lhs->isbn() < rhs->isbn();
  }

  using comp = bool(const std::shared_ptr<Quote> &lhs,
                    const std::shared_ptr<Quote> &rhs);

  // NOTE: unique_ptr??
  std::multiset<std::shared_ptr<Quote>, comp *> items_;
};

TEST_F(CxxCaseQuoteX, CheckTotal_1)
{
  // WHY no error without this???
  // using namespace case_quote;

  // Quote("book no", price)
  items_.insert(std::shared_ptr<Quote>(new Quote("123", 45)));
  items_.insert(std::shared_ptr<Quote>(new Quote("123", 45)));
  items_.insert(std::shared_ptr<Quote>(new Quote("123", 45)));

  // Quote sales which has no discount. 45*3 = 135
  EXPECT_THAT(total_receipt(), 135);

  items_.clear();
}

TEST_F(CxxCaseQuoteX, CheckTotal_2)
{
  using namespace case_quote;

  items_.insert(std::shared_ptr<Quote>(new Bulk_Quote("345", 45, 3, .15)));
  items_.insert(std::shared_ptr<Quote>(new Bulk_Quote("345", 45, 3, .15)));

  // minimum 3 to have 15% discount. 2 books so no discount 45*2 = 90
  EXPECT_THAT(total_receipt(), 90);

  items_.clear();
}

TEST_F(CxxCaseQuoteX, CheckTotal_3)
{
  using namespace case_quote;

  items_.insert(std::shared_ptr<Quote>(new Bulk_Quote("678", 35, 3, .15)));
  items_.insert(std::shared_ptr<Quote>(new Bulk_Quote("678", 35, 3, .15)));
  items_.insert(std::shared_ptr<Quote>(new Bulk_Quote("678", 35, 3, .15)));
  items_.insert(std::shared_ptr<Quote>(new Bulk_Quote("678", 35, 3, .15)));

  // Bulk_quote sales which has discount: minimum 3 and 15% discount
  // 35*4*(1-.15) = 119
  EXPECT_EQ(total_receipt(), 119);

  items_.clear();
}

TEST_F(CxxCaseQuoteX, CheckTotal_4)
{
  using namespace case_quote;

  items_.insert(std::shared_ptr<Quote>(new Bulk_Quote("912", 35, 5, .25)));
  items_.insert(std::shared_ptr<Quote>(new Bulk_Quote("912", 35, 5, .25)));
  items_.insert(std::shared_ptr<Quote>(new Bulk_Quote("912", 35, 5, .25)));
  items_.insert(std::shared_ptr<Quote>(new Bulk_Quote("912", 35, 5, .25)));
  items_.insert(std::shared_ptr<Quote>(new Bulk_Quote("912", 35, 5, .25)));
  items_.insert(std::shared_ptr<Quote>(new Bulk_Quote("912", 35, 5, .25)));

  // Bulk_quote sales which has discount: minimum 5 and 25% discount
  // 35*6*(1-.25) = 157.5
  EXPECT_EQ(157.5, total_receipt());

  items_.clear();
}

// To test user(driver class), Basket. not use fixture and mock out quotes.

namespace case_quote
{
  class MockQuote : public Quote
  {
  public:
    MockQuote(string const &book, double sale_price)
        : Quote(book, sale_price)
    {}

    MOCK_CONST_METHOD1(net_price, double(size_t count));
  };

  class MockBulk : public Bulk_Quote
  {
  public:
    MockBulk(string const &book,
             double price,
             size_t quantity,
             double discount_percent)
        : Bulk_Quote(book, price, quantity, discount_percent)
    {}

    MOCK_CONST_METHOD1(net_price, double(size_t count));
  };

  class Basket
  {
  public:
    // *cxx-undefined*
    // Without it, cause core when add_item is called.
    Basket()
        : items_{compare}
    {}

    void add_item(const std::shared_ptr<Quote> &item)
    {
      cout << "basket::add_item(shard_ptr)" << endl;
      items_.insert(item);
    }

    double total_receipt(ostream &os) const
    {
      double total{};

      for (auto iter = items_.cbegin(); iter != items_.cend();
           iter      = items_.upper_bound(*iter))
      {
        total = (*iter)->net_price(items_.count(*iter));
        os << "isbn: " << (*iter)->isbn() << ", sold: " << items_.count(*iter)
           << ", total sales: " << total << endl;
      }

      return total;
    }

  private:
    static bool compare(const std::shared_ptr<Quote> lhs,
                        const std::shared_ptr<Quote> rhs)
    {
      return lhs->isbn() < rhs->isbn();
    }

    using comp = bool(const std::shared_ptr<Quote>,
                      const std::shared_ptr<Quote>);

    std::multiset<std::shared_ptr<Quote>, comp *> items_;
  };

} // namespace case_quote

TEST(CxxCaseQuote, MockQuote)
{
  using namespace case_quote;

  // Quote sales which has no discount, 45*3 = 135
  std::shared_ptr<MockQuote> q1(new MockQuote("123", 45));
  std::shared_ptr<MockQuote> q2(new MockQuote("123", 45));
  std::shared_ptr<MockQuote> q3(new MockQuote("123", 45));

  EXPECT_CALL(*q1, net_price(_)).WillOnce(Return(135));

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

  EXPECT_CALL(*q1, net_price(_)).WillOnce(Return(157.5));

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
    Quote()
        : book_no_()
        , price_(0.0)
    {}

    Quote(string const &book, double sale_price)
        : book_no_(book)
        , price_(sale_price)
    {}

    virtual ~Quote() {}

    std::string isbn() const { return book_no_; }

    // calculate net price but do not have "discount" concept
    virtual double net_price(size_t count) const { return count * price_; }

    // *cxx-clone* *cxx-move* copy version
    // *cxx-reference-qualifier*
    // note: shall have `const` or see *cxx-const-to-nonconst-error*
    // note: if there's no move verison, copy version will be used instead
    virtual Quote *clone() const &
    {
      cout << "quote::clone() &" << endl;
      return new Quote(*this);
    }

    // cxx-clone, move version
    virtual Quote *clone() const &&
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

    Discount_Quote()
        : quantity_(0)
        , discount_(0.0)
    {}
    Discount_Quote(string const &book,
                   double price,
                   size_t quantity,
                   double discount_percent)
        : Quote(book, price)
        , quantity_(quantity)
        , discount_(discount_percent)
    {}

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
    Bulk_Quote(string const &book,
               double price,
               size_t quantity,
               double discount_percent)
        : Discount_Quote(book, price, quantity, discount_percent)
    {}

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
    virtual Bulk_Quote *clone() const & override
    {
      cout << "bulk_quote::clone() &" << endl;
      return new Bulk_Quote(*this);
    }

    virtual Bulk_Quote *clone() const && override
    {
      cout << "bulk_quote::clone() &&" << endl;
      return new Bulk_Quote(std::move(*this));
    }
  };

  double print_total(ostream &os, Quote const &item, size_t sold)
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
    Basket()
        : items_{compare}
    {}

    void add_item(std::shared_ptr<Quote> const &item)
    {
      cout << "basket::add_item(shard_ptr)" << endl;
      items_.insert(item);
    }

    // copy version
    void add_item(Quote const &item)
    {
      cout << "basket::add_item(Quote const&)" << endl;
      items_.insert(std::shared_ptr<Quote>(item.clone()));
    }

    // move version *cxx-dynamic-binding*
    void add_item(Quote &&item)
    {
      cout << "basket::add_item(Quote &&)" << endl;
      items_.insert(std::shared_ptr<Quote>(std::move(item).clone()));
    }

    double total_receipt(ostream &os) const
    {
      double total{};

      for (auto iter = items_.cbegin(); iter != items_.cend();
           iter      = items_.upper_bound(*iter))
      {
        total = (*iter)->net_price(items_.count(*iter));
        os << "isbn: " << (*iter)->isbn() << ", sold: " << items_.count(*iter)
           << ", total sales: " << total << endl;
      }

      return total;
    }

  private:
    static bool compare(const std::shared_ptr<Quote> lhs,
                        const std::shared_ptr<Quote> rhs)
    {
      return lhs->isbn() < rhs->isbn();
    }

    using comp = bool(const std::shared_ptr<Quote>,
                      const std::shared_ptr<Quote>);

    std::multiset<shared_ptr<Quote>, comp *> items_;
  };

} // namespace case_quote_clone

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

/*
={=============================================================================
cxx-queue

*/

// thread safe q from test_ccon
//
// NOTE:
// 1. supports T but still one type at a time

namespace cxx_case_queue
{
  template <typename T>
  class threadsafe_queue
  {
  private:
    std::queue<T> queue_;
    std::mutex m_;
    std::condition_variable cond_;

  public:
    threadsafe_queue() = default;

    // do not support copy
    threadsafe_queue(const threadsafe_queue &) = delete;
    threadsafe_queue &operator=(const threadsafe_queue &) = delete;

    void push(const T &item)
    {
      std::lock_guard<std::mutex> lock(m_);

      queue_.emplace(item);
      cond_.notify_one();
    }

    void wait_and_pop(T &item)
    {
      std::unique_lock<std::mutex> lock(m_);

      cond_.wait(lock, [&] { return !queue_.empty(); });

      item = queue_.front();
      queue_.pop();
    }

    bool empty() const
    {
      std::lock_guard<std::mutex> lock(m_);
      return queue_.empty();
    }
  };

  std::mutex consumed_mtx;
  int consumed_total{};

  void producer(threadsafe_queue<int> &q)
  {
    for (int i = 0; i < 20; ++i)
    {
      q.push(i);
    }
  }

  void consumer(threadsafe_queue<int> &q)
  {
    // You can pass defer_lock to initialize the lock without locking the mutex
    // (yet):
    //
    // note: must use lock here and it cause a rase otherwise.

    std::unique_lock<std::mutex> lock(consumed_mtx, std::defer_lock);

    int value{};

    for (int i = 0; i < 20; ++i)
    {
      q.wait_and_pop(value);
      lock.lock();
      ++consumed_total;
      lock.unlock();
    }
  }
} // namespace cxx_case_queue

TEST(CxxCaseQueue, see_threadsafe_queue)
{
  using namespace cxx_case_queue;

  threadsafe_queue<int> q;

  consumed_total = 0;

  std::vector<std::thread> consumers;
  std::vector<std::thread> producers;

  for (int i = 0; i < 3; ++i)
  {
    consumers.emplace_back(consumer, std::ref(q));
  }

  std::this_thread::sleep_for(chrono::seconds(2));

  for (int i = 0; i < 3; ++i)
  {
    producers.emplace_back(producer, std::ref(q));
  }

  // waits for them to finish
  for (auto &c : consumers)
    c.join();

  for (auto &p : producers)
    p.join();

  // ecah cunsumes 20
  EXPECT_THAT(consumed_total, 60);
}

namespace cxx_case_queue_1
{
  // simple message q
  template <typename T>
  class queue
  {
  private:
    std::mutex m_;
    std::condition_variable cv_;
    std::queue<T> q_;

  public:
    void push(const T &mesg)
    {
      std::lock_guard<std::mutex> lock(m_);

      q_.emplace(mesg);
      cv_.notify_one();
    }

    T wait_and_pop()
    {
      std::unique_lock<std::mutex> lock(m_);

      cv_.wait(lock, [this] { return !q_.empty(); });

      auto mesg = q_.front();
      q_.pop();

      return mesg;
    }
  };
} // namespace cxx_case_queue_1

// event q from bleaudio case
//
// 1. same as threadsafe_queue
//
// 2. use struct which has many members to cover all type of messages
//
// BleAudio::queue<BleAudioMessage> m_q;
// struct BleAudioMessage{}

namespace cxx_case_queue_1
{
  template <typename T>
  class ble_queue
  {
  private:
    std::mutex m_;
    std::condition_variable cv_;
    std::deque<T> q_;

  public:
    void push(const T &item)
    {
      std::lock_guard<std::mutex> lock(m_);
      q_.emplace_back(item);
      cv_.notify_all();
    }

    // uses std::move() and return value
    T wait_and_pop()
    {
      std::unique_lock<std::mutex> lock(m_);
      cv_.wait(lock, [&] { return !q_.empty(); });
      auto item = std::move(q_.front());
      q_.pop_front();
      return item;
    }
  };

  std::mutex consumed_mtx;
  int consumed_total{};

  void producer(ble_queue<int> &q)
  {
    for (int i = 0; i < 20; ++i)
    {
      q.push(i);
    }
  }

  void consumer(ble_queue<int> &q)
  {
    // You can pass defer_lock to initialize the lock without locking the mutex
    // (yet):
    //
    // note: must use lock here and it cause a rase otherwise.

    std::unique_lock<std::mutex> lock(consumed_mtx, std::defer_lock);

    int value{};

    for (int i = 0; i < 20; ++i)
    {
      value = q.wait_and_pop();
      lock.lock();
      (void)value;
      ++consumed_total;
      lock.unlock();
    }
  }
} // namespace cxx_case_queue_1

TEST(CxxCaseQueue, see_ble_queue)
{
  using namespace cxx_case_queue_1;

  ble_queue<int> q;

  consumed_total = 0;

  std::vector<std::thread> consumers;
  std::vector<std::thread> producers;

  for (int i = 0; i < 3; ++i)
  {
    consumers.emplace_back(consumer, std::ref(q));
  }

  std::this_thread::sleep_for(chrono::seconds(2));

  for (int i = 0; i < 3; ++i)
  {
    producers.emplace_back(producer, std::ref(q));
  }

  // waits for them to finish
  for (auto &c : consumers)
    c.join();

  for (auto &p : producers)
    p.join();

  // ecah cunsumes 20
  EXPECT_THAT(consumed_total, 60);
}

// from code-cxx/fsm/test_cxxfsm.cpp

namespace cxx_case_queue_ccon
{
  // polymorphic base. is that only purpose to have this? no.
  struct message_base
  {
    virtual ~message_base() = default;
  };

  template <typename T>
  struct wrapped_message : public message_base
  {
    T message_;

    explicit wrapped_message(const T &message)
        : message_(message)
    {}
  };

  class queue
  {
  private:
    std::mutex m_;
    std::condition_variable cv_;

    // can use unique_ptr instead? it seems no real difference since pop_front()
    // will decrease counter and it will be released when client is done with it
    // which is the same time point when use unique_ptr.

    std::deque<std::shared_ptr<message_base>> q_;

  public:
    // create new type of wrapped_message<T> whenever push() is called
    template <typename T>
    void push(const T &message)
    {
      std::lock_guard<std::mutex> lock(m_);

      q_.emplace_back(std::make_shared<wrapped_message<T>>(message));
      cv_.notify_one();
    }

    std::shared_ptr<message_base> wait_and_pop()
    {
      std::unique_lock<std::mutex> lock(m_);

      cv_.wait(lock, [&] { return !q_.empty(); });

      auto message = std::move(q_.front());

      // *cxx-error* cuase infinite loop on the user when omits this
      q_.pop_front();

      // not needed
      // lock.unlock();

      return message;
    }
  };

  struct simple_message
  {
    std::string name_;
    explicit simple_message()
        : name_("simple_message")
    {}
  };
} // namespace cxx_case_queue_ccon

TEST(CxxCaseQueue, check_ccon_queue_1)
{
  using namespace cxx_case_queue_ccon;

  // to avoid ambiguity
  cxx_case_queue_ccon::queue q;

  // send T. copy T into wrapped_message and save shared_ptr of it
  //
  // NOTE:
  // see use of template function then template class. have to populate a class
  // for every T? it's easy and light to populate template functoin for each T.

  q.push(simple_message());

  // get std::shared_ptr<message_base>
  auto sp = q.wait_and_pop();

  // what's really saved in shared pointer is wrapped_message instance.
  // dynamic_cast() should success since it's in inheritance.
  wrapped_message<simple_message> *message =
    dynamic_cast<wrapped_message<simple_message> *>(sp.get());

  // message->message_ is T which is copied in adn T.name_ to get name
  // std::cout << "message->message_ : " << message->message_.name_ << std::endl;
  EXPECT_THAT(message->message_.name_, "simple_message");
}

// use `reinterpret_cast` instead

TEST(CxxCaseQueue, check_ccon_queue_2)
{
  using namespace cxx_case_queue_ccon;
  // to avoid ambiguity
  cxx_case_queue_ccon::queue q;

  // send T. copy T into wrapped_message and save shared_ptr of it
  //
  // NOTE:
  // see use of template function then template class. have to populate a class
  // for every T? it's easy and light to populate template functoin for each T.

  q.push(simple_message());

  // get std::shared_ptr<message_base>
  auto sp = q.wait_and_pop();

  // what's really saved in shared pointer is wrapped_message instance.
  // dynamic_cast() should success since it's in inheritance.
  wrapped_message<simple_message> *message =
    reinterpret_cast<wrapped_message<simple_message> *>(sp.get());

  // message->message_ is T which is copied in adn T.name_ to get name
  // std::cout << "message->message_ : " << message->message_.name_ << std::endl;
  EXPECT_THAT(message->message_.name_, "simple_message");
}

/*
={=============================================================================
cxx-base64

https://www.base64encode.org/

Base64 encoding schemes are commonly used when there is a need to encode binary
data that needs be stored and transferred over media that are designed to deal
with textual data. This is to ensure that the data remains intact without
modification during transport. Base64 is used commonly in a number of
applications including email via MIME, and storing complex data in XML or JSON.


Details of the Base64 encoding

Base64 is a generic term for a number of similar encoding schemes that encode
binary data by treating it numerically and translating it into a base 64
representation. The Base64 term originates from a specific MIME content transfer
encoding.

Design

`The particular choice of characters` to make up the 64 characters required for
base varies between implementations. 

The general rule is to choose a set of 64 characters that is both part of a
subset common to most encodings, and also printable. This combination leaves the
data unlikely to be modified in transit through systems, such as email, which
were traditionally not 8-bit clean. 

For example, MIME's Base64 implementation uses A-Z, a-z, and 0-9 for the first
62 values, "+" and "/" for the last two.

Other variations, usually derived from Base64, share this property but differ in
the symbols chosen for the last two values; an example is the URL and file name
safe (RFC 4648 / Base64URL) variant, which uses "-" and "_".


Example

A quote snippet from Thomas Hobbes's Leviathan:

"Man is distinguished, not only by his reason, but ..."

represented as an ASCII byte sequence is encoded in MIME's Base64 scheme as follows:

keitee@kit-ubuntu:~/works/coreutils$ echo "Man is distinguished, not only by his reason, but ..." | base64
TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCAuLi4K


In the above quote the encoded value of "Man" is "TWFu". Encoded in ASCII, M, a,
n are stored as the bytes 77, 97, 110, which are 01001101, 01100001, 01101110 in
base 2. These three bytes are joined together in a 24 bit buffer producing
010011010110000101101110. Packs of 6 bits (6 bits have a maximum of 64 different
binary values) are converted into 4 numbers (24 = 4 * 6 bits) which are then
converted to their corresponding values in Base64.

Text content    : M         a         n
ASCII           : 77        97        110
Bit pattern     : 01001101  01100001  01101110
                : 010011 010110 000101 101110

// from python
>>> int("010011", 2)
19
>>> int("010110", 2)
22
>>> int("000101", 2)
5
>>> int("101110", 2)
46

Index           : 19  22  5   46
Base64-encoded  : T   W   F   u

As this example illustrates, Base64 encoding converts 3 uncoded bytes (in this
case, ASCII characters) into 4 encoded ASCII characters.

command line to encode/decode

keitee@kit-ubuntu:~/works/coreutils$ echo "&streamtype=1" | base64
JnN0cmVhbXR5cGU9MQo=

keitee@kit-ubuntu:~/works/coreutils$ echo "JnN0cmVhbXR5cGU9MQo=" | base64 -d
&streamtype=1


https://nachtimwald.com/2017/11/18/base64-encode-and-decode-in-c/

Base64 Encode and Decode in C

Posted on November 18, 2017 ~ John

Introduction

A very popular way to encode binary data is Base64. The basis of this is an
encoding table. As you might expect, there are 64 total characters that go into
the tale. There are multiple implementations of base64 with slight differences.
They are all the same except for the last two characters and line ending
requirements. The first 62 characters are always
“ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789”. PEM and MIME
encoding are the most common and use “+/” as the last two characters. PEM and
MIME may use the same characters but they have different maximum line lengths.
I’m going to implement PEM/MINE but I’m not going to implement new line support.

Properties

Size

In Base64 encoding, 3 binary bytes are represented as 4 characters. This gives
us a 4:3 ratio, meaning there is 33% overhead for base64. To get the number of
base64 characters needed for a given binary data blob, take the length of input
and round up to the nearest multiple of 3. Then, divide by 3 to get the number
of 3 byte blocks. Multiply by 4 to get the number of base64 characters.

The date for each set of 3 binary bytes is spread over 4 characters giving us 6
bits per character. 3 bytes total 24 bits (3*8=24). There 4 characters for the
24 bits (24/4=6). Think of it this way 3/4 (putting 3 bytes into 4) means the
data is split 75% from each byte to span 4 bytes. Thus, 6 bits utilized per
character.


Padding

Base64 works in blocks of 3 bytes. Implementations such as PEM require padding
using = character. Padding keeps the correct number of characters when there are
not enough bytes in the sequence. Ending with a single = means the last binary
sequence contains 2 bytes. Ending with == means it contains 1 byte.

this code is from mongoose.c and examples are:

  unsigned char sha[20];
  char b64_sha[30];
  mg_base64_encode(sha, sizeof(sha), b64_sha);

*/

namespace cxx_case_base64
{
#define BASE64_ENCODE_BODY                                                     \
  static const char *b64 =                                                     \
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";        \
  int i, j, a, b, c;                                                           \
                                                                               \
  for (i = j = 0; i < src_len; i += 3)                                         \
  {                                                                            \
    a = src[i];                                                                \
    b = i + 1 >= src_len ? 0 : src[i + 1];                                     \
    c = i + 2 >= src_len ? 0 : src[i + 2];                                     \
                                                                               \
    BASE64_OUT(b64[a >> 2]);                                                   \
    BASE64_OUT(b64[((a & 3) << 4) | (b >> 4)]);                                \
    if (i + 1 < src_len)                                                       \
    {                                                                          \
      BASE64_OUT(b64[(b & 15) << 2 | (c >> 6)]);                               \
    }                                                                          \
    if (i + 2 < src_len)                                                       \
    {                                                                          \
      BASE64_OUT(b64[c & 63]);                                                 \
    }                                                                          \
  }                                                                            \
                                                                               \
  while (j % 4 != 0)                                                           \
  {                                                                            \
    BASE64_OUT('=');                                                           \
  }                                                                            \
  BASE64_FLUSH()

#define BASE64_OUT(ch)                                                         \
  do                                                                           \
  {                                                                            \
    dst[j++] = (ch);                                                           \
  } while (0)

#define BASE64_FLUSH()                                                         \
  do                                                                           \
  {                                                                            \
    dst[j++] = '\0';                                                           \
  } while (0)

  void cs_base64_encode(const unsigned char *src, int src_len, char *dst)
  {
    BASE64_ENCODE_BODY;
  }

  // o no output buffer size check

  void base64_encode_debug(const unsigned char *src, int src_len, char *dst)
  {
    static const char *b64 =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    int i, j, a, b, c;

    std::cout << "input len is: " << src_len << std::endl;

    for (i = j = 0; i < src_len; i += 3)
    {
      // pick three chars if *possible*
      a = src[i];
      b = i + 1 >= src_len ? 0 : src[i + 1];
      c = i + 2 >= src_len ? 0 : src[i + 2];

      if (a == '\0')
        std::cout << "a is null at i: " << i << std::endl;

      if (b == '\0')
        std::cout << "b is null at i+1: " << i + 1 << std::endl;

      if (c == '\0')
        std::cout << "c is null at i+2: " << i + 2 << std::endl;

      // take the first 6 bits from the first char. convert and save it
      // BASE64_OUT(b64[a >> 2]);
      dst[j++] = (b64[a >> 2]);

      // take 2 bits from the first char and take 4 bits from the second.
      // convert and save it.
      // 3 is 11b
      dst[j++] = (b64[((a & 3) << 4) | (b >> 4)]);

      // since fully used the first char.
      //
      // NOTE: Why check again if the second char is in the valid input range?
      // input includes a null, '\0', and is included in encoding. However, if i
      // goes over the input length, set input char with 0 in the picking char
      // code above. this second check prevent processing those chars so filled
      // with a padding.

      if (i + 1 < src_len)
      {
        // take 4 bits from the second and 2 bits from the third
        dst[j++] = (b64[((b & 15) << 2) | (c >> 6)]);
      }

      if (i + 2 < src_len)
      {
        // take 6 bits from the third
        dst[j++] = (b64[c & 63]);
      }
    } // for end

    // fill a padding char
    while (j % 4 != 0)
    {
      dst[j++] = '=';
    }

    // fill a null
    dst[j++] = '\0';
  }

  void base64_encode(const unsigned char *src, int src_len, char *dst)
  {
    static const char *b64 =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    int i, j, a, b, c;

    for (i = j = 0; i < src_len; i += 3)
    {
      a = src[i];
      b = i + 1 >= src_len ? 0 : src[i + 1];
      c = i + 2 >= src_len ? 0 : src[i + 2];

      dst[j++] = (b64[a >> 2]);
      dst[j++] = (b64[((a & 3) << 4) | (b >> 4)]);

      if (i + 1 < src_len)
      {
        dst[j++] = (b64[((b & 15) << 2) | (c >> 6)]);
      }

      if (i + 2 < src_len)
      {
        dst[j++] = (b64[c & 63]);
      }
    } // for end

    while (j % 4 != 0)
    {
      dst[j++] = '=';
    }

    dst[j++] = '\0';
  }

  // https://nachtimwald.com/2017/11/18/base64-encode-and-decode-in-c/
  const char b64chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  size_t b64_encoded_size(size_t inlen)
  {
    size_t ret;

    ret = inlen;
    if (inlen % 3 != 0)
      ret += 3 - (inlen % 3);
    ret /= 3;
    ret *= 4;

    return ret;
  }

  char *b64_encode_2(const unsigned char *in, size_t len)
  {
    char *out;
    size_t elen;
    size_t i;
    size_t j;
    size_t v;

    if (in == NULL || len == 0)
      return NULL;

    elen      = b64_encoded_size(len);
    out       = (char *)malloc(elen + 1);
    out[elen] = '\0';

    for (i = 0, j = 0; i < len; i += 3, j += 4)
    {
      v = in[i];
      v = i + 1 < len ? v << 8 | in[i + 1] : v << 8;
      v = i + 2 < len ? v << 8 | in[i + 2] : v << 8;

      out[j]     = b64chars[(v >> 18) & 0x3F];
      out[j + 1] = b64chars[(v >> 12) & 0x3F];
      if (i + 1 < len)
      {
        out[j + 2] = b64chars[(v >> 6) & 0x3F];
      }
      else
      {
        out[j + 2] = '=';
      }
      if (i + 2 < len)
      {
        out[j + 3] = b64chars[v & 0x3F];
      }
      else
      {
        out[j + 3] = '=';
      }
    }

    return out;
  }

  // https://github.com/joedf/base64.c/blob/master/base64.c
  unsigned char b64_chr[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  unsigned int b64_encode_3(const unsigned char* in, unsigned int in_len, unsigned char* out) {

    unsigned int i=0, j=0, k=0, s[3];

    for (i=0;i<in_len;i++) {
      s[j++]=*(in+i);
      if (j==3) {
        out[k+0] = b64_chr[ (s[0]&255)>>2 ];
        out[k+1] = b64_chr[ ((s[0]&0x03)<<4)+((s[1]&0xF0)>>4) ];
        out[k+2] = b64_chr[ ((s[1]&0x0F)<<2)+((s[2]&0xC0)>>6) ];
        out[k+3] = b64_chr[ s[2]&0x3F ];
        j=0; k+=4;
      }
    }

    if (j) {
      if (j==1)
        s[1] = 0;
      out[k+0] = b64_chr[ (s[0]&255)>>2 ];
      out[k+1] = b64_chr[ ((s[0]&0x03)<<4)+((s[1]&0xF0)>>4) ];
      if (j==2)
        out[k+2] = b64_chr[ ((s[1]&0x0F)<<2) ];
      else
        out[k+2] = '=';
      out[k+3] = '=';
      k+=4;
    }

    out[k] = '\0';

    return k;
  }

} // namespace cxx_case_base64

// [ RUN      ] CxxCaseBase64.check_decode_1
// output: JnN0cmVhbXR5cGU9MQA=
//
// input len is: 14
// b is null at i+1: 13
// c is null at i+2: 14
// output: JnN0cmVhbXR5cGU9MQA=
//
// input len is: 3
// c is null at i+2: 2
// output: JnMA
//
// input len is: 2
// b is null at i+1: 1
// c is null at i+2: 2
// output: JgA=
//
// input len is: 13
// b is null at i+1: 13
// c is null at i+2: 14
// output: JnN0cmVhbXR5cGU9MQ==
//
// [       OK ] CxxCaseBase64.check_decode_1 (0 ms)

TEST(CxxCaseBase64, check_decode_1)
{
  using namespace cxx_case_base64;

  {
    const unsigned char input[] = "&streamtype=1";
    char output[1000]           = {0};

    cs_base64_encode(input, sizeof(input), output);

    std::cout << "output: " << output << std::endl;
  }

  {
    // src_len is 14 which includes a null
    //                            "012.345.678.901.2"
    const unsigned char input[] = "&streamtype=1";
    char output[1000]           = {0};

    base64_encode(input, sizeof(input), output);

    std::cout << "output: " << output << std::endl;
  }

  // to show it handles a null
  {
    // src_len is 3 which includes a null
    //                            "01"
    const unsigned char input[] = "&s";
    char output[1000]           = {0};

    base64_encode(input, sizeof(input), output);

    std::cout << "output: " << output << std::endl;
  }

  // to show it handles a null
  {
    // src_len is 2 which includes a null
    //                            "0"
    const unsigned char input[] = "&";
    char output[1000]           = {0};

    base64_encode(input, sizeof(input), output);

    std::cout << "output: " << output << std::endl;
  }

  // {
  //   std::string input{"&streamtype=1"};
  //   char output[1000] = {0};
  //
  //   cs_base64_encode(reinterpret_cast<const unsigned char *>(input.data()),
  //                    input.length(),
  //                    output);
  //
  //   std::cout << "output: " << output << std::endl;
  // }
  {
    // src_len is 14 which includes a null since std::string.data()
    //               "012.345.678.901.2"
    std::string input{"&streamtype=1"};
    char output[1000] = {0};

    // NOTE: +1 to include a null to encode
    base64_encode(reinterpret_cast<const unsigned char *>(input.data()),
                  input.length() + 1,
                  output);

    std::cout << "output: " << output << std::endl;
  }
}

// $ echo "ABC123Test Lets Try this' input and see What happens" | base64
// QUJDMTIzVGVzdCBMZXRzIFRyeSB0aGlzJyBpbnB1dCBhbmQgc2VlIFdoYXQgaGFwcGVucwo=
//
// https://www.base64encode.org/
// QUJDMTIzVGVzdCBMZXRzIFRyeSB0aGlzJyBpbnB1dCBhbmQgc2VlIFdoYXQgaGFwcGVucwo=
//
// from base64 man:
// The  data  are  encoded  as  described  for  the base64 alphabet in RFC 4648.

//[ RUN      ] CxxCaseBase64.check_decode_2
// input len is: 53
// b is null at i+1: 52
// c is null at i+2: 53
// output: QUJDMTIzVGVzdCBMZXRzIFRyeSB0aGlzJyBpbnB1dCBhbmQgc2VlIFdoYXQgaGFwcGVucwA=
// output: QUJDMTIzVGVzdCBMZXRzIFRyeSB0aGlzJyBpbnB1dCBhbmQgc2VlIFdoYXQgaGFwcGVucw==
// [       OK ] CxxCaseBase64.check_decode_2 (0 ms)
//
// which one is correct?
//
// https://web.mit.edu/freebsd/head/contrib/wpa/src/utils/base64.c
// Base64 encoding/decoding (RFC1341)
// Copyright (c) 2005-2011, Jouni Malinen <j@w1.fi>
//
// https://www.base64encode.org/
// Split lines into chunks: 
// The encoded data will be a continuous text without any whitespaces, check
// this option if you want to break it up into multiple lines. The applied
// character limit is defined in the MIME (RFC 2045) specification, which states
// that the encoded lines must be no more than 76 characters long. (*)
//
// Perform URL safe encoding: 
// Using standard Base64 in URLs requires encoding of "+", "/" and "="
// characters into their percent-encoded form, which makes the string
// unnecessarily longer. Enable this option to encode into an URL and file name
// friendly Base64 variant (RFC 4648 / Base64URL) where the "+" and "/"
// characters are respectively replaced by "-" and "_", as well as the padding
// "=" signs are omitted.
//
// Have tried couple of codes from googling and all showed the same result as
// decode_1 result. seems there are many RFC or old/new RFC.
//
//
// https://tools.ietf.org/html/rfc4648
//
// This and references in the site shows the correct result as base64.
// http://libb64.sourceforge.net/
// one of the references is base64 in GNU coreutils.

TEST(CxxCaseBase64, check_decode_2)
{
  using namespace cxx_case_base64;

  {
    const unsigned char input[] =
      "ABC123Test Lets Try this' input and see What happens";
    char output[1000] = {0};

    base64_encode(input, sizeof(input), output);

    std::cout << "output: " << output << std::endl;
  }

  {
    const unsigned char input[] =
      "ABC123Test Lets Try this' input and see What happens";

    char *output = b64_encode_2(input, strlen((const char *)input));
    // char *output = b64_encode_2(input, strlen((const char *)input+1));

    std::cout << "output: " << output << std::endl;

    free(output);
  }

  {
    const unsigned char input[] =
      "ABC123Test Lets Try this' input and see What happens";
    char output[1000] = {0};

    b64_encode_3(input, sizeof(input), (unsigned char *)output);

    std::cout << "output: " << output << std::endl;
  }
}

TEST(CxxCaseBase64, check_decode_3)
{
  using namespace cxx_case_base64;

  {
    // src_len is 14 which includes a null since std::string.data()
    //                "0123456789012"
    std::string input{"&streamtype=1"};
    char output[1000] = {0};

    EXPECT_THAT(input.size(), 13);
    EXPECT_THAT(sizeof(output), 1000);

    base64_encode(reinterpret_cast<const unsigned char *>(input.c_str()),
                  input.size() + 1,
                  output);

    //                 "01234567890123456789"
    char converted[] = "JnN0cmVhbXR5cGU9MQA=";
    // std::cout << "output: " << output << std::endl;

    // includes a null
    EXPECT_THAT(sizeof(converted), 21);

    // copy ouput to std::string
    std::string result{output};

    // expected and result are same?
    EXPECT_THAT(memcmp(converted, result.c_str(), sizeof(converted)), 0);

    // not includes a null
    EXPECT_THAT(result.size(), 20);
    EXPECT_THAT(strlen(result.c_str()), 20);
  }
}

TEST(CxxCaseBase64, check_decode_4)
{
  using namespace cxx_case_base64;

  std::string input{"&vgcassetid=F67307ADA1614C35A0BA4DE39F22272&vgctoken=00000040000001A4000000600000001A00000061000000020002000000630000000800B980D500B98675000000700000017A0006B40A223124823D8BB018C766555E7CDC360F8ADE3F1AF7F91B310D0E2F7624828DBFDE2D3D4E59&streamtype=1"};
  std::string expected{"JnZnY2Fzc2V0aWQ9RjY3MzA3QURBMTYxNEMzNUEwQkE0REUzOUYyMjI3MiZ2Z2N0b2tlbj0wMDAwMDA0MDAwMDAwMUE0MDAwMDAwNjAwMDAwMDAxQTAwMDAwMDYxMDAwMDAwMDIwMDAyMDAwMDAwNjMwMDAwMDAwODAwQjk4MEQ1MDBCOTg2NzUwMDAwMDA3MDAwMDAwMTdBMDAwNkI0MEEyMjMxMjQ4MjNEOEJCMDE4Qzc2NjU1NUU3Q0RDMzYwRjhBREUzRjFBRjdGOTFCMzEwRDBFMkY3NjI0ODI4REJGREUyRDNENEU1OSZzdHJlYW10eXBlPTEA"};

  char output[1000] = {0};

  base64_encode(reinterpret_cast<const unsigned char *>(input.c_str()),
      input.size() + 1,
      output);

  // std::cout << "output: " << output << std::endl;

  // copy ouput to std::string
  std::string result{output};

  EXPECT_THAT(result.size(), 332);

  EXPECT_THAT(expected, result);
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
