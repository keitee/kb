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

// o ListEntry can be any type.
// o No remove() to remove entry at random position since it's expensive
// operation as with other contiguous implementation; contiguous stack.

namespace algo_list_contiguous
{
  struct ListEntry
  {
    explicit ListEntry(int row = 0, int col = 0) noexcept 
      : row_(row), col_(col) {}

    int row_{};
    int col_{};
  };

  class List
  {
    public:
      void create() { count_ = 0; }
      void clear() { count_ = 0; }
      bool empty() { return count_ == 0 ? true : false; }
      bool full() { return count_ == MAX_LIST ? true : false; }
      int size() { return count_; }

      void push(ListEntry const& entry)
      {
        if (full())
          throw std::runtime_error("list is full");

        coll_[count_++] = entry;
      }

      std::vector<ListEntry> snap() noexcept
      {
        std::vector<ListEntry> result{};

        for (size_t i = 0; i < count_; count_++)
          result.push_back(coll_[i]);

        return result;
      }

    private:
      static int const MAX_LIST{5};
      size_t count_{};
      ListEntry coll_[MAX_LIST];
  };
} // namespace

TEST(AlgoList, ContiguousSimple)
{
  using namespace algo_list_contiguous;

  auto values{
      ListEntry(1,2), 
      ListEntry(2,3), 
      ListEntry(3,4), 
      ListEntry(4,5), 
      ListEntry(5,6)
  };

  List coll;
  coll.create();

  for (auto &e : values)
    coll.push(e);

  EXPECT_THAT(coll.size(), 5);
  EXPECT_THROW(coll.push(ListEntry(6,7)), runtime_error); 
}


namespace algo_list_linked
{
  // when Node and Entry are different
  //
  // struct ListNode 
  // {
  //   ListNode() : key_(0), next_(nullptr) {}
  //   ListEntry key_;
  //   ListNode *next_;
  // };

  struct ListEntry
  {
    explicit ListEntry(int row = 0, int col = 0) noexcept 
      : row_(row), col_(col), next_(nullptr) {}

    int row_{};
    int col_{};

    ListEntry *next_;
  };

  // when use ListEntry *head

  class List
  {
    public:
      List() : count_(0), head_(nullptr) {}

      bool empty() { return count_ == 0 ? true : false; }

      // no support of max size
      bool full() { return false; }

      size_t size() { return count_; }

      // works
      // void push(ListEntry const& entry)
      // {
      //   // when add the first
      //   if (head_ == nullptr)
      //   {
      //     head_ = new ListEntry(entry);
      //     count_++;
      //   }
      //   else
      //   {
      //     ListEntry *end{};

      //     // find the end *algo-list-find-end*

      //     for (end = head_; end->next_; end = end->next_)
      //       ;

      //     end->next_ = new ListEntry(entry);
      //     count_++;
      //   }
      // }

      // better version
      void push(ListEntry const& entry)
      {
        ListEntry *end{};

        // find the end *algo-list-find-end*

        for (end = head_; end && end->next_; end = end->next_)
          ;

        if (end == nullptr)
          head_ = new ListEntry(entry);
        else
          end->next_ = new ListEntry(entry);

        count_++;
      }

      void clear_old()
      {
        ListEntry *prev{nullptr};
        ListEntry *curr{nullptr};

        for (curr = head_; curr;)
        {
          if (prev)
          {
            free(prev);
            count_--;
          }
          prev = curr;
          curr = curr->next_;
        }

        if (prev)
        {
          free(prev);
          count_--;
        }

        head_ = nullptr;
        assert(count_ == 0);
      }

      // better version
      void clear()
      {
        ListEntry *prev{nullptr};
        ListEntry *curr{nullptr};

        // has different form from *also-list-find-end*

        for (curr = head_; curr;)
        {
          prev = curr;
          curr = curr->next_;

          free(prev);
          count_--;
        }

        head_ = nullptr;
        assert(count_ == 0);
      }

      void snap() noexcept
      {
        ListEntry *curr{nullptr};

        std::vector<ListEntry> result{};

        for (curr = head_; curr; curr = curr->next_)
        {
          cout << "{" << curr->row_ << ", " << curr->col_ << "}" << endl;
        }
      }

    private:
      size_t count_;
      ListEntry *head_;
  };

  // when use ListEntry head

  class List_02
  {
    public:
      List_02() : count_(0) {}

      bool empty() { return count_ == 0 ? true : false; }

      // no support of max size
      bool full() { return false; }

      size_t size() { return count_; }

      void push(ListEntry const& entry)
      {
        ListEntry *end{};

        // find the end
        for (end = &head_; end->next_; end = end->next_)
          ;

        end->next_ = new ListEntry(entry);
        count_++;
      }

      // better version
      void clear()
      {
        ListEntry *prev{nullptr};
        ListEntry *curr{nullptr};

        for (curr = head_.next_; curr;)
        {
          prev = curr;
          curr = curr->next_;

          free(prev);
          count_--;
        }

        head_.next_ = nullptr;
        assert(count_ == 0);
      }

      void snap() noexcept
      {
        ListEntry *curr{nullptr};

        std::vector<ListEntry> result{};

        for (curr = head_.next_; curr; curr = curr->next_)
        {
          cout << "{" << curr->row_ << ", " << curr->col_ << "}" << endl;
        }
      }

    private:
      size_t count_;

      ListEntry head_;
  };
} // namespace

TEST(AlgoList, LinkedSimple)
{
  using namespace algo_list_linked;

  {
    auto values{
      ListEntry(1,2), 
        ListEntry(2,3), 
        ListEntry(3,4), 
        ListEntry(4,5), 
        ListEntry(5,6)
    };

    List coll;

    for (auto &e : values)
      coll.push(e);

    EXPECT_THAT(coll.size(), 5);

    // now do not expect exception since there's no max
    // EXPECT_THROW(coll.push(ListEntry(6,7)), runtime_error); 

    coll.push(ListEntry(6,7));
    EXPECT_THAT(coll.size(), 6);

    coll.snap();

    coll.clear();
    EXPECT_THAT(coll.size(), 0);
  }

  {
    auto values{
      ListEntry(1,2), 
        ListEntry(2,3), 
        ListEntry(3,4), 
        ListEntry(4,5), 
        ListEntry(5,6)
    };

    List_02 coll;

    for (auto &e : values)
      coll.push(e);

    EXPECT_THAT(coll.size(), 5);

    // now do not expect exception since there's no max
    // EXPECT_THROW(coll.push(ListEntry(6,7)), runtime_error); 

    coll.push(ListEntry(6,7));
    EXPECT_THAT(coll.size(), 6);

    coll.snap();

    coll.clear();
    EXPECT_THAT(coll.size(), 0);
  }
}

int main(int argc, char** argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
