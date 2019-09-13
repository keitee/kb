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

        // In the unlikely event that there are expired observers, remove
        // expired observers by copying only if use_count() > 0
        //
        // since referring shared_ptr which weak_ptr refers to is gone then
        // use_count() of weak_ptr gets decreased.

        decltype(observers_) observers_copy;
        std::copy_if(observers_.cbegin(), observers_.cend(),
            std::back_inserter(observers_copy),
            std::bind(&std::weak_ptr<T>::use_count, std::placeholders::_1));

        if (observers_copy.size() != observers_.size())
          observers_ = observers_copy;

        // okay, starts notifying
        notifying_ = true;

        // don't want to lock adding new observers while calling observers so
        // use a copy instead. 
        
        lock.unlock();

        //----------------- NOTE ----------------------------------------------
        // We maintain vector of strong pointers pointing to observer objects as
        // otherwise bad things can happen. Lets consider, the observer object
        // point backs to the notifier object itself.  That means, there is a
        // circular dependency between the notifier and the observer, but we
        // break that by using a combination of shared and weak pointers.
        // However, imagine, within the notify_impl() method, we gets a shared
        // pointer of observer object out of weak_ptr. After the shared pointer
        // is constructed (bit still in use), now the owner of the observer
        // resets its pointer that is pointing to the observer object. This
        // might result one to one references between the notifier and the
        // observer, i.e., as soon as the observer will be destroyed the
        // notifier will also be destroyed. It means, if now the observer object
        // is destroyed from the notify_imp() method, it will cause the notifier
        // object itself to be destroyed, where the notify_impl can still
        // continue to access its member variable (e.g. dispatcher). This might
        // result an undefined behaviour.
        //---------------------------------------------------------------------

        // Q: WHY need observers_strong vector?
        std::vector<std::shared_ptr<T>> observers_strong;

        for(auto const &o : observers_copy)
        {
          std::shared_ptr<T> strong = o.lock();
          if(strong)
          {
            // void notify_impl(
            //  std::function<void(std::shared_ptr<T> const &)> f);
            dispatcher_->post(std::bind(f, strong));
          }
          observers_strong.push_back(strong);
        }

        // okay, notifying finishes
        lock.lock();

        // TODO ???
        // about to unregister an observer so make sure that there is no work
        // for this observer after that.
        if (dispatcher_ && (waitee_count_ > 0))
        {
          lock.unlock();
          dispatcher_->sync();
          lock.lock();
        }

        notifying_ = false;

        // notify all waited observers on remove_observer()
        if (waitee_count_ > 0)
          cv_.notify_all();

        lock.unlock();
      }
  };
} // namespace


// ={=========================================================================

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

