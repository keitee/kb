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

namespace queue_circular_vacant
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
    { return head_ == tail_; }

    bool full() const
    { return ((head_ + 1) % MAX_SIZE) == tail_; }

    int size() const 
    { return (head_ - tail_ + MAX_SIZE) % MAX_SIZE; }

    void push(int value) 
    {
      if (full())
        return;

      head_ = (head_ + 1) % MAX_SIZE;
      coll_[head_] = value;
    }

    int pop() 
    {
      if (empty())
        return -1;

      tail_ = (tail_ + 1) % MAX_SIZE;
      return coll_[tail_];
    }

    std::vector<int> snap()
    {
      std::vector<int> coll;

      // care about start value and <= condition. However, cannot use comparison
      // on head and tail since it warps around after all.
      //
      // for (int i = tail_ + 1; i <= head_; i = (i + 1) % MAX_SIZE)
      //     coll.push_back(coll_[i]);

      int start = tail_ + 1;

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
    std::vector<int> coll_; // {MAX_SIZE, 0};
}; 

} // namespace

TEST(Queue, CircularVacant)
{
  using namespace queue_circular_vacant;

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
  cq.push(402);
  cq.push(403);
  cq.push(404);

  // since it is vacant version
  EXPECT_THAT(cq.size(), 9);
  EXPECT_THAT(cq.full(), true);

  EXPECT_THAT(cq.pop(), 15);
  EXPECT_THAT(cq.pop(), 100);
  EXPECT_THAT(cq.pop(), 101);

  // since it is vacant version
  EXPECT_THAT(cq.size(), 6);
  EXPECT_THAT(cq.snap(), ElementsAre(102, 300, 301, 302, 400, 401));

  EXPECT_THAT(cq.full(), false);
}


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
        return;

      head_ = head_ % MAX_SIZE;
      coll_[head_++] = value;
      count_++;
    }

    int pop() 
    {
      int value{};

      if (empty())
        return -1;

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
  cq.push(402);
  cq.push(403);
  cq.push(404);

  EXPECT_THAT(cq.size(), 10);
  EXPECT_THAT(cq.full(), true);

  EXPECT_THAT(cq.pop(), 15);
  EXPECT_THAT(cq.pop(), 100);
  EXPECT_THAT(cq.pop(), 101);

  EXPECT_THAT(cq.size(), 7);
  EXPECT_THAT(cq.snap(), ElementsAre(102, 300, 301, 302, 400, 401, 402));

  EXPECT_THAT(cq.full(), false);
}

int main(int argc, char** argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
