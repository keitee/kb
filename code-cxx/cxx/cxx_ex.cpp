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
#include <mutex>
#include <condition_variable>

#include <sys/prctl.h>

#include "gmock/gmock.h"

using namespace std;
using namespace std::placeholders;
using namespace testing;


TEST(Cxx, ex)
{
  EXPECT_THAT(true, true);
}

namespace cxx_observer_notifier
{
  class Polymorphic
  {
    public:
      virtual ~Polymorphic() {};
  };

  template<typename T>
    class Notifier : virtual public Polymorphic
  {
    public:
      Notifier() {}

      // register
      void add_observer(std::shared_ptr<T> const &o)
      {
        observer_ = o;
      }

    protected:

      // for multiple arguments and it calls notify<T>
      template<typename F, typename... Args>
        void notify(F f, Args&&... args)
        {
          notify(std::bind(f, std::placeholders::_1, std::forward<Args>(args)...));
        }

      // for single argument call, notify(x)
      template<typename F>
        void notify(F f)
        {
          notify_impl_(f);
        }

    private:

      // saved observer
      std::shared_ptr<T> observer_;

      // (std::function<void(std::shared_ptr<T> const &)> f) means that the
      // returned type from bind() will be called shared_ptr<T> which is target
      // object.

      void notify_impl_(std::function<void(std::shared_ptr<T> const &)> f)
      {
        if(observer_)
          f(observer_);

        // std::shared_ptr<T> strong = o.lock();
        // if(strong)
        // {
        //   // void notify_impl(
        //   //  std::function<void(std::shared_ptr<T> const &)> f);
        //   dispatcher_->post(std::bind(f, strong));
        // }
      }
  };

  // A observer(callback) interface
  //
  class StateEvents
  {
    public:
      virtual void stateChanged(int state) = 0;
      virtual void nameChanged(std::string name) = 0;
      virtual void keyAndValueChanged(std::string key, std::string value) = 0;
      virtual void eventOccured() = 0;
  };


  // observer
  //
  template <class T>
    class Observer : public T, virtual public Polymorphic
  {
  };

  class TestObserver : public Observer<StateEvents>
  {
    public:

      // MOCK_METHOD1(stateChanged, void (int));
      // MOCK_METHOD1(nameChanged, void (std::string));
      // MOCK_METHOD2(keyAndValueChanged, void (std::string, std::string));
      // MOCK_METHOD0(eventOccured, void());
      
      void stateChanged(int state)
      {
        std::cout << "called stateChanged(" << state << ")" << std::endl;
      }

      void nameChanged(std::string name)
      {
        std::cout << "called nameChanged(" << name << ")" << std::endl;
      }

      void keyAndValueChanged(std::string key, std::string value)
      {
        std::cout << "called keyAndValueChanged(" << key 
          << ", " << value << ")" << std::endl;
      }

      void eventOccured()
      {
        std::cout << "called eventOccured()" << std::endl;
      }
  };

  // source
  // 
  class Source : public Notifier<StateEvents>
  {
    public:
      void setState(int state)
      {
        // do something

        // calls 
        // template<typename F> 
        //  void notify(F f);
        notify(std::bind(&StateEvents::stateChanged, std::placeholders::_1, state));
      }

      void setName(std::string name)
      {
        // do something

        // calls 
        // template<typename F, typename... Args> 
        //  void notify(F f, Args&&... args);
        notify(&StateEvents::nameChanged, name);
      }

      void setKeyAndValue(std::string key, std::string value)
      {
        // do something

        notify(&StateEvents::keyAndValueChanged, key, value);
      }

      void emitEvent()
      {
        // do something

        notify(&StateEvents::eventOccured);
      }
  };
} // namespace


TEST(PatternObserver, Notifier)
{
  using namespace cxx_observer_notifier;

  Source source;

  auto observer = std::make_shared<TestObserver>();
  
  source.add_observer(observer);
  source.setState(5);
  source.setName("notifier");
  source.setKeyAndValue("key", "value");
}


/*
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

      // for multiple arguments and it calls notify<T>
      template<typename F, typename... Args>
        void notify(F f, Args&&... args)
        {
          notify(std::bind(f, std::placeholders::_1, std::forward<Args>(args)...));
        }

      // for single argument call, notify(x)
      template<typename F>
        void notify(F f)
        {
          notify_impl_(f);
        }
      

    private:
      std::mutex m_;
      std::condition_variable cv_;

      std::deque<std::weak_ptr<T>> observers_;
      std::shared_ptr<IDispatcher> dispatcher_;

      uint32_t waitee_count_{0};
      bool notifying_{false};

      // (std::function<void(std::shared_ptr<T> const &)> f) means that the
      // returned type from bind() will be called shared_ptr<T> which is target
      // object.

      void notify_impl_(std::function<void(std::shared_ptr<T> const &)> f)
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

  //
  //
  class Polymorphic
  {
    public:
      virtual ~Polymorphic() {};
  };

  // A template for observing objects that accept signals defined in T
  //
  template <typename T>
    class Observer : public T, virtual public Polymorphic
  {
  };

  // A observer(callback) interface
  //
  class StateEvents
  {
    public:
      virtual void stateChanged(int state) = 0;
      virtual void nameChanged(std::string name) = 0;
      virtual void keyAndValueChanged(std::string key, std::string value) = 0;
      virtual void eventOccured() = 0;
  };

  class Source : public Notifier<StateEvents>
  {
    public:
      void setState(int state)
      {
        // do something

        // calls 
        // template<typename F> 
        //  void notify(F f);
        notify(std::bind(&StateEvents::stateChanged, std::placeholders::_1, state));
      }

      void setName(std::string name)
      {
        // do something

        // calls 
        // template<typename F, typename... Args> 
        //  void notify(F f, Args&&... args);
        notify(&StateEvents::nameChanged, name);
      }

      void setKeyAndValue(std::string key, std::string value)
      {
        // do something

        notify(&StateEvents::keyAndValueChanged, key, value);
      }

      void emitEvent()
      {
        // do something

        notify(&StateEvents::eventOccured);
      }
  };

} // namespace
*/


namespace cxx_pattern_dispatcher
{
  // Polymorphic.h
  // original comments:
  // Inherit from this from all types that have virtual functions. Doing so
  // ensures that you have virtual destructor and saves you nasty surprises.

  class Polymorphic
  {
    public:
      virtual ~Polymorphic() {};
  };

  // IDispatcher.h
  class IDispatcher : public Polymorphic
  {
    public:
      // post an work item to be executed
      virtual void post(std::function<void ()>) = 0;

      // ensures that any works that was in the queue before the call has been
      // executed
      virtual void sync() = 0;

      // check if it's called from this dispatch thread
      virtual bool invoked_from_this() = 0;
  }; 


  // ThreadedDispatcher.h
  // (see) that use of `public` for interfaces from parent class and ones from
  // this class which shows a clear seperation.

  class ThreadedDispatcher : public IDispatcher
  {
    public:
      // post an work item to be executed
      virtual void post(std::function<void ()>);

      // ensures that any works that was in the queue before the call has been
      // executed
      virtual void sync();

      // check if it's called from this dispatch thread
      virtual bool invoked_from_this();

    public:
      ThreadedDispatcher(std::string const &name = std::string());

      // create dispatcher with supplied priority and name
      ThreadedDispatcher(int priority, std::string const &name = std::string());

      ~ThreadedDispatcher();

      // perform any work remaining in the queue the stop accepting new work.
      void flush();

      // stop accepting new work and dispatcher even if there are works in the
      // queue.
      void stop();

    private:
      bool running_;

      std::thread t_;
      std::mutex m_;
      std::condition_variable cv_;

      std::deque<std::function<void()>> q_;

      void do_work_(std::string const &name, int priority);
      std::function<void()> next_();
  };

  // (see) that ctor calls ctors
  ThreadedDispatcher::ThreadedDispatcher(std::string const &name)
    : ThreadedDispatcher(-1, name)
  {}

  // *cxx-error* : ISO C++ forbids taking the address of an
  // unqualified or parenthesized non-static member function to form a pointer
  // to member function. 
  // Say ‘&cxx_pattern_dispatcher::ThreadedDispatcher::_do_work’ [-fpermissive] ,
  //
  // _t(std::thread(&_do_work, this, name, priority))

  ThreadedDispatcher::ThreadedDispatcher(int priority, std::string const &name)
    : running_(true)
      , t_(std::thread(&ThreadedDispatcher::do_work_, this, name, priority))
  {}

  ThreadedDispatcher::~ThreadedDispatcher()
  {
    if (running_)
      stop();
  }

  void ThreadedDispatcher::post(std::function<void ()> work)
  {
    // (see) this is original code. is it different from the below?
    //
    // std::uniqie_lock<std::mutex> lock(m);
    // if (running)
    // {
    //   q.push_back(work);
    //   lock.unlock();
    //   cv.notify_one();
    // }
    // else
    // {
    //   ...
    // }

    std::lock_guard<std::mutex> lock(m_);

    if(running_)
    {
      q_.push_back(work);
      cv_.notify_one();
    }
    else
    {
      std::cout << "ignoring work because the dispatcher is not running"
        << std::endl;

      // original comment:
      // LOG_WARN("Ignoring work because the dispatcher is not running anymore");
      //
      // can't throw an exception here because if this is executed from
      // destructor, which occurs when work adds more work things go horribly
      // wrong. Instead, ignore work.

    }
  }

  bool ThreadedDispatcher::invoked_from_this()
  {
    return (std::this_thread::get_id() == t_.get_id());
  }

  namespace
  {
    void syncCallback(std::mutex *m, std::condition_variable *cond, bool *fired)
    {
      std::unique_lock<std::mutex> lock(*m);
      *fired = true;
      cond->notify_all();

      // unnecessary
      // lock.unlock();
    }
  } // namespace

  void ThreadedDispatcher::sync()
  {
    std::mutex sm;
    std::condition_variable cv;
    bool fired{false};

    std::unique_lock<std::mutex> qlock(m_);
    if (!running_)
    {
      std::cout << "ignoring sync request because the dispatcher is not running"
        << std::endl;
      return;
    }

    q_.push_back(std::bind(syncCallback, &sm, &cv, &fired));
    qlock.unlock();

    // pushed sync work and make dispatcher run to do all works in the q
    cv_.notify_one();

    // wait for `fired` to become true
    std::unique_lock<std::mutex> slock(sm);

    // same as wait(std::unique_lock<>, predicate);
    while(!fired)
    {
      cv.wait(slock);
    }
  }

  // (see) uses unique_lock to unlock it as soon as it changes necessary state.
  void ThreadedDispatcher::stop()
  {
    std::unique_lock<std::mutex> lock(m_);
    running_ = false;
    lock.unlock();

    cv_.notify_one();
    t_.join();
  }

  // To ensure all the work that is in the queue is done, we lock a mutex. post
  // a job to the queue that unlocks it and stops running further jobs. Then
  // block here until that's done.

  namespace
  {
    void unlockAndSetFlagToFalse(std::mutex& m, bool& flag)
    {
      std::this_thread::sleep_for(std::chrono::seconds(5));
      // std::cout << "flush thread: waits ends" << std::endl;
      m.unlock();

      // TODO ??? really work without this?
      flag = false;

      // (see)
      // original code. without setting flag, still works
      // flag = false;
      //
      // using namespace std;
      // m.unlock();
      // flag = false;
    }
  }

  void ThreadedDispatcher::flush()
  {
    if (running_)
    {
      std::mutex m2;
      m2.lock();
      // *cxx-bind*
      post(std::bind(unlockAndSetFlagToFalse, 
            std::ref(m2), std::ref(this->running_)));
      // blocks here until unlockAndSetFlagToFalse() unlock it since post() will
      // signal and it make signal false as stop() do.
      m2.lock();
      m2.unlock();
      stop();
    }
    else
    {
      std::cout << "ignoring flush request because the dispatcher is not running"
        << std::endl;
    }
  }

  std::function<void()> ThreadedDispatcher::next_()
  {
    // (see) Q: std::move() has an effect here?

    auto work = std::move(q_.front());
    q_.pop_front();
    return work;
  }

  // thread function

  void ThreadedDispatcher::do_work_(std::string const &name, int priority)
  {
    if (prctl(PR_SET_NAME, name.empty() ? "THR_DISPATCH" : name.c_str(), 0, 0, 0) < 0)
    {
      // AI_LOG_SYS_ERROR(errno, "Failed to set thread name");
    }

    // if (priority > 0)
    // {
    //   struct sched_param param;
    //   param.sched_priority = priority;
    //   int err = pthread_setschedparam(pthread_self(), SCHED_RR, &param);
    //   if (err != 0)
    //   {
    //     AI_LOG_SYS_ERROR(err, "Failed to set thread priority to %d", priority);
    //   }
    // }

    std::unique_lock<std::mutex> lock(m_);

    // (see) out of wait() when running_ is false? means when stop() is called
    // runs one work if there is and ends dispatch thread.
    //
    // (see) use of bind
    //
    // the original code:
    //
    // bool ThreadedDispatcher::hasMoreWorkOrWasStopRequested()
    // {
    //     return !q.empty() || !running;
    // }
    //
    // cv.wait(lock, bind(&This::hasMoreWorkOrWasStopRequested, this));

    while (running_)
    {
      cv_.wait(lock, [&]{ return !q_.empty() || !running_;});
      if (!q_.empty())
      {
        auto work = next_();

        // (see) do not block adding work, post(), while running work.
        lock.unlock();
        work();
        lock.lock();
      }
    }
  }

} // namespace


namespace cxx_pattern_dispatcher
{
  void assign1(int &what, int value)
  {
    // std::cout << "assign1 is called, what: " << what << std::endl;
    what += value;
  }
} // namespace

TEST(PatternDispatcher, checkPostedWorkDone)
{
  using namespace cxx_pattern_dispatcher;

  // do one work and uses stop()
  {
    int value{};
    std::shared_ptr<ThreadedDispatcher> td = std::make_shared<ThreadedDispatcher>();
    td->post(std::bind(assign1, std::ref(value), 10));

    std::this_thread::sleep_for(chrono::seconds(1));
    EXPECT_THAT(value, 10);
  }

  // do more work and uses stop()
  {
    int value{};
    std::shared_ptr<ThreadedDispatcher> td = std::make_shared<ThreadedDispatcher>();
    td->post(std::bind(assign1, std::ref(value), 10));
    td->post(std::bind(assign1, std::ref(value), 10));
    td->post(std::bind(assign1, std::ref(value), 10));
    td->post(std::bind(assign1, std::ref(value), 10));

    std::this_thread::sleep_for(chrono::seconds(1));
    EXPECT_THAT(value, 40);
  }
}

TEST(PatternDispatcher, checkFlush)
{
  using namespace cxx_pattern_dispatcher;

  // do one work and uses flush()
  {
    int value{};
    std::shared_ptr<ThreadedDispatcher> td = std::make_shared<ThreadedDispatcher>();
    td->post(std::bind(assign1, std::ref(value), 10));
    td->flush();

    // since flush() used and no need to sleep
    // std::this_thread::sleep_for(chrono::seconds(2));

    EXPECT_THAT(value, 10);
  }
}

namespace cxx_pattern_dispatcher
{
  void check_thread_id(std::thread::id id)
  {
    EXPECT_NE(std::this_thread::get_id(), id);
  }
} // namespace

TEST(PatternDispatcher, checkPostedWorkDoneOnDispatcher)
{
  using namespace cxx_pattern_dispatcher;

  // expect that gtest thread is different from dispatcher thread
  {
    int value{};
    std::shared_ptr<ThreadedDispatcher> td = std::make_shared<ThreadedDispatcher>();
    td->post(std::bind(check_thread_id, std::this_thread::get_id()));

    std::this_thread::sleep_for(chrono::seconds(1));
  }
}

namespace cxx_pattern_dispatcher
{
  void save_sequence(int &value)
  {
    static int sequence{0};
    value = ++sequence;
  }
} // namespace

TEST(PatternDispatcher, checkPostedWorkDoneInOrder)
{
  using namespace cxx_pattern_dispatcher;

  // expect that gtest thread is different from dispatcher thread
  {
    int first{};
    int second{};
    int third{};

    std::shared_ptr<ThreadedDispatcher> td = std::make_shared<ThreadedDispatcher>();
    td->post(std::bind(save_sequence, std::ref(first)));
    td->post(std::bind(save_sequence, std::ref(second)));
    td->post(std::bind(save_sequence, std::ref(third)));

    std::this_thread::sleep_for(chrono::seconds(1));

    EXPECT_THAT(first, 1);
    EXPECT_THAT(second, 2);
    EXPECT_THAT(third, 3);
  }
}

// ={=========================================================================

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

