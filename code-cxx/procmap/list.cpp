#include "gmock/gmock.h"

#include <iostream>
#include <syscall.h>    // system call symbolic names
#include <unistd.h>
#include <asm/param.h>  // EXEC_PAGESIZE
#include <fcntl.h>      // O_RDONLY, etc
#include <sys/mman.h>   // mmap defines

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;


// ={=========================================================================

/*

*/

// types

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

typedef unsigned  long      uptr;
typedef signed    long      sptr;
typedef unsigned long long  u64;
typedef u64                 OFF_T;

struct ProcSelfMapsBuff 
{
  char *data;
  uptr mmapped_size;
  uptr len;
};

enum FileAccessMode
{
  RdOnly,
  WrOnly,
  RdWr
};


// ={=========================================================================
//
// // sanitizer_common/sanitizer_list.h
// // Intrusive singly-linked list with size(), push_back(), push_front()
// // pop_front(), append_front() and append_back().
// // This class should be a POD (so that it can be put into TLS)
// // and an object with all zero fields should represent a valid empty list.
// // This class does not have a CTOR, so clear() should be called on all
// // non-zero-initialized objects before using.
//
// template<class Item>
// struct IntrusiveList {
//   friend class Iterator;
// 
//   void clear();
//   bool empty() const;
//   uptr size() const;
// 
//   void push_back(Item *x); 
//   void push_front(Item *x);
//   void pop_front();
// 
//   Item *front();
//   Item *back();
// 
//   void append_front(IntrusiveList<Item> *l);
//   void append_back(IntrusiveList<Item> *l);
// 
//   void CheckConsistency();
// 
//   template<class ListTy, class ItemTy>
//   class IteratorBase {
//    public:
//     explicit IteratorBase(ListTy *list)
//         : list_(list), current_(list->first_) { }
//     ItemTy *next() {
//       ItemTy *ret = current_;
//       if (current_) current_ = current_->next;
//       return ret;
//     }
//     bool hasNext() const { return current_ != nullptr; }
//    private:
//     ListTy *list_;
//     ItemTy *current_;
//   };
// 
//   typedef IteratorBase<IntrusiveList<Item>, Item> Iterator;
//   typedef IteratorBase<const IntrusiveList<Item>, const Item> ConstIterator;
// 
// // private, don't use directly.
//   uptr size_;
//   Item *first_;
//   Item *last_;
// };

struct AddressRange
{
  AddressRange *next_;
  uptr beg_;
  uptr end_;
  bool executable_;

  AddressRange(uptr beg, uptr end, bool executable)
    : next_(nullptr), beg_(beg), end_(end), executable_(executable) {}
};


// 1. no ctors
// 2. has _back()
// 3. do not have item structure

template<typename T>
struct IntrusiveList 
{
  friend class Iterator;

  void clear() { first_ = last_ = nullptr; size_ = 0; }
  bool empty() { return first_ == last_; }
  uptr size() { return size_; }

  T *front() { return first_; }
  T *back() { return last_; }

  void push_back(T *item) 
  {
    if (empty())
    {
      item->next_ = nullptr;
      first_ = last_ = item;
      size_ = 1;
    }
    else
    {
      item->next_ = nullptr;
      last_->next_ = item;
      last_ = item;
      ++size_;
    }
  }

  void push_front(T *item)
  {
    if (empty())
    {
      item->next_ = nullptr;
      first_ = last_ = item;
      size_ = 1;
    } 
    else
    {
      item->next_ = first_;
      first_ = item;
      ++size_;
    }
  }

  void pop_front()
  {
    CHECK(!empty());
    first_ = first_->next_;

    // why need this since both becomes nullptr?
    if (!first_)
      last_ = nullptr;

    --size_;
  }

  // "private, don't use directly." but how since it's public?
  uptr size_;
  T *first_;
  T *last_;
};

namespace use_intrusive_list
{
  struct SampleItem
  {
    SampleItem *next_;
    uptr id_;
    string name;
    SampleItem(uptr id) : id_(id) {}
  };
}


TEST(List, Use)
{
  using namespace use_intrusive_list;

  IntrusiveList<SampleItem> coll;

  coll.clear();
  EXPECT_TRUE(coll.empty());

  coll.push_back(new SampleItem(1));
  EXPECT_FALSE(coll.empty());

  cout << "coll size: " << coll.size() << endl;
}


// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
