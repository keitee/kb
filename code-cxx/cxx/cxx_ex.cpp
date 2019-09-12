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
            //
            // the original comment is:
            // if addObserver was called 3 times for one object, you need
            // to call remove 3 times too.

            break;
          }
        }

        // when observer calls this to remove and notifier is running then
        // cannot remove it now. hold it for now until notifier finishes
        

        // that is it waits until notifying is done, `notifying_`  is false and
        // waitee_count_ is num of users.

        if (notifying_)
        {
          waitee_count_++;

          do
          {

            // cxx-condition-variable-wait
            // wait()
            // blocks the current thread until the condition variable is woken
            // up 

            cv_.wait(lock);
          } while (notifying_);

          waitee_count_--;
        }
      }

    protected:

    private:
      std::mutex m_;
      std::condition_variable cv_;

      std::deque<std::weak_ptr<T>> observers_;
      std::shared_ptr<IDispatcher> dispatcher_;

      uint32_t waitee_count_{0};
      bool notifying_{false};

      void notify_impl(std::function<void(std::shared_ptr<T> const &)> f)
      {
        std::unique_lock<std::mutex> lock(m_);

        if (!dispatcher_)
        {
          throw std::logic_error("must set a dispatcher before you produce events.");
        }

        // don't want to lock adding new observers while calling observers so
        // make a copy instead. In the unlikely event that there are expired
        // observers, remove expired observers by copying 
        // only if use_count() > 0

        decltype(observers_) observers_copy;

      }
  };
} // namespace


// ={=========================================================================

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

