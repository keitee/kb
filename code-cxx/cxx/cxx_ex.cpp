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


TEST(Ex, ex)
{
  EXPECT_THAT(true, true);
}

namespace cxx_pattern_dispatcher
{
  template<typename T>
    class Notifier : virtual public Polymorphic
  {
    public:
      Notifier() {}

      void add_observer(std::shared_ptr<T> const &o)
      {
        std::lock_guard<std::mutex> lock(m_);
        observers_.push_back(o);
      }

      void remove_observer(std::shared_ptr<T> const &o)
      {
        std::unique_lock<std::mutex> lock(m_);

        for (size_t i = 0; i < observers_.size(); ++i)
        {
          // get strong shared ptr from weak
          // if stored strong pointer is still valid and which means object is
          // still around and it's safe to use
          if (observers_[i].lock() == o)
          {
            observers_.erase(observers_.begin() + i);

            // this make to erase one. what if there are duplicates??
            break;
          }
        }

        // when observer calls this to remove and notifier is running then
        // cannot remove it now. hold it for now until notifier finishes
        if (notifying_)
        {
          waitee_count_++;

          do
          {
          } while (waitee_count_);
        }
      }

    private:
      std::deque<std::weak_ptr<T>> observers_;
  };
} // namespace

// ={=========================================================================

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

