#include <iostream>
#include <cstring>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <bitset>

#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;

// ={=========================================================================
template <typename T>
void PRINT_ELEMENTS( T& coll, const string optstr="" )
{
    size_t count{};
    cout << optstr;

    for( const auto &elem : coll )
    {
        cout << elem << " ";
        ++count;
    }

    cout << "(" << count << ")" << endl;
}

template <typename T>
void PRINT_M_ELEMENTS( T& coll, const string optstr="" )
{
    size_t count{};
    cout << optstr;

    for( const auto &elem : coll )
    {
        cout << "(" << elem.first << ", " << elem.second << ") ";
        ++count;
    }

    cout << "(" << count << ")" << endl;
}


// ={=========================================================================

namespace queue_circular_count
{

class CircularQueue
{
  public:
    // *cxx-vector-ctor*
    CircularQueue() 
      : coll_(MAX_SIZE, 0)
    {
    }

    bool empty() const 
    { return count_ == 0; }

    bool full() const
    { return count_ >= MAX_SIZE; }

    int size() const 
    { return count_; }

    void push(int value) 
    {
      if (full())
        throw std::runtime_error("queue is full");

      head_ = head_ % MAX_SIZE;
      coll_[head_++] = value;
      count_++;
    }

    int pop() 
    {
      int value{};

      if (empty())
        throw std::runtime_error("queue is empty");

      tail_ = tail_ % MAX_SIZE;
      value = coll_[tail_++];
      count_--;

      return value;
    }

    std::vector<int> snap()
    {
      std::vector<int> coll;

      int start = tail_;

      for (int i = 0; i < size(); ++i)
      {
        coll.push_back(coll_[start]);
        start = (start+1) % MAX_SIZE;
      }

      return coll;
    }

  private:
    const int MAX_SIZE{10};
    int head_{};
    int tail_{};
    int count_{};
    std::vector<int> coll_; // {MAX_SIZE, 0};
}; 

} // namespace

TEST(Queue, CircularCount)
{
  using namespace queue_circular_count;

  CircularQueue cq;

  cq.push(10);
  cq.push(11);
  cq.push(12);

  EXPECT_THAT(cq.size(), 3);
  EXPECT_THAT(cq.snap(), ElementsAre(10, 11, 12));

  cq.push(13);
  cq.push(14);
  cq.push(15);

  EXPECT_THAT(cq.size(), 6);
  EXPECT_THAT(cq.snap(), ElementsAre(10, 11, 12, 13, 14, 15));

  EXPECT_THAT(cq.pop(), 10);
  EXPECT_THAT(cq.pop(), 11);
  EXPECT_THAT(cq.pop(), 12);

  EXPECT_THAT(cq.size(), 3);

  cq.push(100);
  cq.push(101);
  cq.push(102);

  EXPECT_THAT(cq.size(), 6);
  EXPECT_THAT(cq.snap(), ElementsAre(13, 14, 15, 100, 101, 102));

  EXPECT_THAT(cq.pop(), 13);
  EXPECT_THAT(cq.pop(), 14);

  cq.push(300);
  cq.push(301);
  cq.push(302);

  EXPECT_THAT(cq.size(), 7);
  EXPECT_THAT(cq.snap(), ElementsAre(15, 100, 101, 102, 300, 301, 302));

  cq.push(400);
  cq.push(401);
  cq.push(402);   // since it does have one more item than the vacant version
  EXPECT_THROW(cq.push(403), std::runtime_error);
  EXPECT_THROW(cq.push(402), std::runtime_error);

  EXPECT_THAT(cq.size(), 10);
  EXPECT_THAT(cq.full(), true);

  EXPECT_THAT(cq.pop(), 15);
  EXPECT_THAT(cq.pop(), 100);
  EXPECT_THAT(cq.pop(), 101);

  EXPECT_THAT(cq.size(), 7);
  EXPECT_THAT(cq.snap(), ElementsAre(102, 300, 301, 302, 400, 401, 402));

  EXPECT_THAT(cq.full(), false);
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

    cout << "pop: pos: " << pos 
      << ", coll_[]: " << data_[pos] << endl;

    size_--;
    return data_[pos];
  }

  void push(T const item)
  {
    // if (full())
    //   throw std::runtime_error("buffer is full");

    head_ = next_pos();
    data_[head_] = item;

    cout << "push: head_: " << head_ 
      << ", coll_[]: " << data_[head_] << endl;

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


TEST(Queue, CircularCountIterator)
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


int main(int argc, char** argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
