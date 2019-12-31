/*

*/
#include <algorithm>
#include <deque>
#include <iostream>
#include <memory>
#include <vector>

#include <condition_variable>
#include <mutex>
#include <thread>

#include <sys/prctl.h>

#include "gmock/gmock.h"

using namespace std;
using namespace testing;

// ={=========================================================================

namespace cxx_observer_ex_2019_12_25
{
  // use observable vector

  enum class ChangedAction
  {
    add,
    remove,
    clear,
    assign
  };

  struct ChangedNotification
  {
    ChangedAction action;
    size_t index;
  };

  std::string GET_ACTION_NAME(ChangedAction const action)
  {
    switch (action)
    {
      case ChangedAction::add:
        return "add notification";
      case ChangedAction::remove:
        return "remove notification";
      case ChangedAction::clear:
        return "clear notification";
      case ChangedAction::assign:
        return "assign notification";
      default:
        return "unknown notification";
    }
  };

  // observer uses `interface` and let's use ABC and inheritance in observer
  // tree

  class CollectionObserver
  {
  public:
    virtual ~CollectionObserver() {}
    virtual void onChanged(ChangedNotification const &changedNotification) = 0;
  };

  class Observer1 : public CollectionObserver
  {
  public:
    virtual void onChanged(ChangedNotification const &changedNotification)
    {
      std::cout << "Observer1::onChanged("
                << GET_ACTION_NAME(changedNotification.action) << " on index "
                << changedNotification.index << std::endl;
    }
  };

  class Observer2 : public CollectionObserver
  {
  public:
    virtual void onChanged(ChangedNotification const &changedNotification)
    {
      std::cout << "Observer2::onChanged("
                << GET_ACTION_NAME(changedNotification.action) << " on index "
                << changedNotification.index << std::endl;
    }
  };

  // observee, subject

  template <typename T>
  class ObservableVector
  {
    using size_type = typename std::vector<T>::size_type;

  private:
    std::vector<T> data_;
    std::vector<CollectionObserver *> observers_;

  public:
    explicit ObservableVector()
        : data_()
        , observers_()
    {}

    explicit ObservableVector(std::initializer_list<T> list)
        : data_(list)
        , observers_()
    {}

    // copy supports
    // copies data vector but not observers. notify current observers

    // no need to use this
    // ObservableVector<T> &operator=(const ObservableVector<T> &rhs)

    ObservableVector &operator=(const ObservableVector &rhs)
    {
      std::cout << "operator=(&)" << std::endl;

      // self assign
      if (this != &rhs)
      {
        data_ = rhs.data_;

        for (const auto &e : observers_)
        {
          // although it is less likely that e is nullptr
          // if (nullptr != e)
          // so can use

          if (__builtin_expect(nullptr != e, 1))
          {
            // NOTE: see cxx-temporary
            e->onChanged({ChangedAction::assign, data_.size()});
          }
        }
      }

      return *this;
    }

    // no copy ctor
    ObservableVector(const ObservableVector &rhs) = delete;

    // move supports
    ObservableVector &operator=(const ObservableVector &&rhs)
    {
      std::cout << "operator=(&&)" << std::endl;

      if (this != &rhs)
      {
        data_ = std::move(rhs.data_);

        for (const auto &e : observers_)
        {
          if (__builtin_expect(nullptr != e, 1))
          {
            e->onChanged({ChangedAction::assign, data_.size()});
          }
        }
      }

      return *this;
    }

    ObservableVector(const ObservableVector &&rhs) = delete;

    void push_back(const T &value)
    {
      data_.push_back(value);

      for (const auto &e : observers_)
      {
        if (__builtin_expect(nullptr != e, 1))
        {
          // e.onChanged({ChangedAction::add, 1});
          e->onChanged({ChangedAction::add, data_.size()});
        }
      }
    }

    void push_back(const T &&value)
    {
      // data_.push_back(std::move(value));
      // no need since it's already rvalue adn std::vector() will handle
      // move context
      //
      // this makes the move version same as the copy version.

      data_.push_back(value);

      for (const auto &e : observers_)
      {
        if (__builtin_expect(nullptr != e, 1))
        {
          e->onChanged({ChangedAction::add, data_.size()});
        }
      }
    }

    void pop_back()
    {
      data_.pop_back();

      for (const auto &e : observers_)
      {
        if (__builtin_expect(nullptr != e, 1))
        {
          e->onChanged({ChangedAction::remove, data_.size()});
        }
      }
    }

    void clear() noexcept
    {
      data_.clear();

      for (const auto &e : observers_)
      {
        if (__builtin_expect(nullptr != e, 1))
        {
          e->onChanged({ChangedAction::clear, data_.size()});
        }
      }
    }

    size_type size() const noexcept { return data_.size(); }

    bool empty() const noexcept { return data_.empty(); }

    // void add_observer(const CollectionObserver *observer)
    void add_observer(CollectionObserver *observer)
    {
      observers_.push_back(observer);
    }

    // assumes that there are duplicates in observers
    void remove_observer(const CollectionObserver *observer)
    {
      observers_.erase(
        std::remove(observers_.begin(), observers_.end(), observer),
        observers_.end());
    }
  }; // ObservableVector

} // namespace cxx_observer_ex_2019_12_25

// [ RUN      ] Observer.Ex_ObservableVectorContainer
// =========
// Observer1::onChanged(add notification on index 1
// Observer2::onChanged(add notification on index 1
// =========
// Observer1::onChanged(add notification on index 2
// Observer2::onChanged(add notification on index 2
// =========
// Observer1::onChanged(add notification on index 3
// Observer2::onChanged(add notification on index 3
// =========
// Observer1::onChanged(remove notification on index 2
// Observer2::onChanged(remove notification on index 2
// =========
// Observer1::onChanged(clear notification on index 0
// Observer2::onChanged(clear notification on index 0
// =========
// operator=(&)
// =========
// operator=(&&)
// [       OK ] Observer.Ex_ObservableVectorContainer (0 ms)

TEST(Observer, Ex_ObservableVectorContainer)
{
  using namespace cxx_observer_ex_2019_12_25;

  ObservableVector<int> ov1;

  Observer1 o1;
  Observer2 o2;

  int value{5};

  ov1.add_observer(&o1);
  ov1.add_observer(&o2);

  // move
  cout << "=========" << endl;
  ov1.push_back(1);

  // move
  cout << "=========" << endl;
  ov1.push_back(2);

  // copy
  cout << "=========" << endl;
  ov1.push_back(value);

  cout << "=========" << endl;
  ov1.pop_back();

  cout << "=========" << endl;
  ov1.clear();

  ov1.remove_observer(&o1);
  ov1.remove_observer(&o2);

  // no notification since observer is removed.

  ObservableVector<int> ov2{1, 2, 3};

  // copy assign
  cout << "=========" << endl;
  ov1 = ov2;

  // move assign
  cout << "=========" << endl;
  ov1 = ObservableVector<int>{7, 8, 9};
  ov1.push_back(1);
}

/*
// ={=========================================================================

o this pattern is the same as the observer. The observer side uses
  observer/callback/listener interface ABC and concrete observers have to
  implement them.

o The source/subject side inherites Notifier<T> has T* and T is interface ABC
  and T points to observer. Notifier<T> provides notification functinality and a
  subclass, source, inherits to have them. This is main difference to observer
  pattern.

o The both have to use multiple inheritance if want to supports multiple
  callback interfaces.

*/

namespace cxx_notifier_simple_ex_2019_12
{
  // A observer(callback) interface ABC
  class StateEvents
  {
  public:
    virtual void stateChanged(int state)                                = 0;
    virtual void nameChanged(std::string name)                          = 0;
    virtual void keyAndValueChanged(std::string key, std::string value) = 0;
    virtual void eventOccurred()                                        = 0;
  };

  class Polymorphic
  {
  public:
    virtual ~Polymorphic() {}
  };

  // source. supports single observer

  // necessary?
  // template <typename T> class Notifier : virtual public Polymorphic

  template <typename T>
  class Notifier : public Polymorphic
  {
  private:
    std::shared_ptr<T> observer_;

    // implies that `observer_` is shared pointer which is target object and
    // calls its member function.
    void notify_impl_(std::function<void(const std::shared_ptr<T> &)> f)
    {
      if (observer_)
        f(observer_);
    }

  protected:
    template <typename F>
    void notify(F f)
    {
      notify_impl_(f);
    }

    // as `Source::setState()` below shows, user have to use bind() to call.
    // To provide more convenient form to user:
    template <typename F, typename... Args>
    void notify(F f, Args &&... args)
    {
      notify(std::bind(f, std::placeholders::_1, std::forward<Args>(args)...));
    }

  public:
    Notifier() {}

    void addObserver(const std::shared_ptr<T> &observer)
    {
      observer_ = observer;
    }
  };

  class Source : public Notifier<StateEvents>
  {
  public:
    void setState(int state)
    {
      // calls 'template <typename F> void notify(F f)'
      notify(
        std::bind(&StateEvents::stateChanged, std::placeholders::_1, state));
    }

    void setName(std::string name)
    {
      // use `convenient form`
      notify(&StateEvents::nameChanged, name);
    }

    void setKeyAndValue(std::string key, std::string value)
    {
      notify(&StateEvents::keyAndValueChanged, key, value);
    }

    void emitEvent() { notify(&StateEvents::eventOccurred); }
  };

  // observer
  // NOTE:
  // Arguably you could inherit directly from T, however inheriting from
  // Observer<T> is more intention revealing than inheriting from T. There is
  // no extra overhead because of Empty Base Class Optimisation.

  // necessry?
  // template <class T> class Observer : public T, virtual public Polymorphic

  template <typename T>
  class Observer : public T, public Polymorphic
  {};

  class Observer1 : public Observer<StateEvents>
  {
  public:
    // if use gmock
    // MOCK_METHOD1(stateChanged, void (int));
    // MOCK_METHOD1(nameChanged, void (std::string));
    // MOCK_METHOD2(keyAndValueChanged, void (std::string, std::string));
    // MOCK_METHOD0(eventOccured, void());

  public:
    virtual void stateChanged(int state)
    {
      std::cout << "Observer1::stateChanged(" << state << ") called"
                << std::endl;
    }

    virtual void nameChanged(std::string name)
    {
      std::cout << "Observer1::nameChanged(" << name << ") called" << std::endl;
    }

    virtual void keyAndValueChanged(std::string key, std::string value)
    {
      std::cout << "Observer1::keyAndValueChanged(" << key << "," << value
                << ") called" << std::endl;
    }

    virtual void eventOccurred()
    {
      std::cout << "Observer1::eventOccurred() called" << std::endl;
    }
  };

} // namespace cxx_notifier_simple_ex_2019_12

// [ RUN      ] PatternObserver.NotifierSimple
// Observer1::stateChanged(5) called
// Observer1::nameChanged(notifier) called
// Observer1::keyAndValueChanged(name,Observer1) called
// [       OK ] PatternObserver.NotifierSimple (0 ms)

TEST(PatternObserver, NotifierSimple)
{
  using namespace cxx_notifier_simple_ex_2019_12;

  Source source;

  auto observer = std::make_shared<Observer1>();

  source.addObserver(observer);
  source.setState(5);
  source.setName("notifier");
  source.setKeyAndValue("name", "Observer1");
}

/*
// ={=========================================================================
cxx_pattern_observer_notifier_full

*/

namespace cxx_notifier_full_ex_2019_12
{
  // A observer(callback) interface ABC
  class StateEvents
  {
  public:
    virtual void stateChanged(int state)                                = 0;
    virtual void nameChanged(std::string name)                          = 0;
    virtual void keyAndValueChanged(std::string key, std::string value) = 0;
    virtual void eventOccurred()                                        = 0;
  };

  // Inherit from this for all types that have virtual functions. Doing so
  // ensures that you have virtual destructor and saves you nasty surprises.
  class Polymorphic
  {
  public:
    virtual ~Polymorphic() {}
  };

  // IDispatcher.h
  class Dispatcher : public Polymorphic
  {
  public:
    // post an `work` to be run
    virtual void post(std::function<void()>) = 0;

    // ensure that any works in the queue before tha call has been run
    virtual void sync() = 0;

    // check if it's called from this dispatcher thread
    virtual bool invoked_from_this() = 0;
  };

  // source. supports `multiple` observer

  // necessary?
  // template <typename T> class Notifier : virtual public Polymorphic

  template <typename T>
  class Notifier : public Polymorphic
  {
  private:
    std::mutex m_;
    std::condition_variable cv_;
    std::deque<std::weak_ptr<T>> observers_;

    std::shared_ptr<Dispatcher> dispatcher_;
    bool notifying_{false};
    uint32_t waitee_count_{0};

    void notify_impl_(std::function<void(const std::shared_ptr<T> &)> f)
    {
      std::unique_lock<std::mutex> lock(m_);

      if (!dispatcher_)
      {
        throw std::logic_error(
          "must set a dispatcher before you produces events");
      }

      // In the unlikely event where there are expired observers, remove expired
      // observers by copying only if weak_ptr.use_count() > 0 since if
      // corresponding shared pointer is gone, then use_count() of weak pointer
      // gets decreased.

      decltype(observers_) observers_copy;

      std::copy_if(
        observers_.cbegin(),
        observers_.cend(),
        std::back_inserter(observers_copy),
        std::bind(&std::weak_ptr<T>::use_count, std::placeholders::_1));

      // update observers_
      if (observers_copy.size() != observers_.size())
        observers_ = observers_copy;

      // okay, now we have all living observers so starts notifying
      notifying_ = true;

      // do not want to block adding observers while notifying them so use a
      // copy instead.

      lock.unlock();

      //----------------- NOTE ----------------------------------------------
      // We maintain vector of strong pointers pointing to observer objects as
      // otherwise bad things can happen. Lets consider, the observer object
      // point backs to the notifier object itself.
      //
      // NOTE: cannot see where the observer points back to the notifier?
      //
      // That means, there is a circular dependency between the notifier and the
      // observer, but we break that by using a combination of shared and weak
      // pointers.  However, imagine, within the notify_impl() method, we gets a
      // shared pointer of observer object out of weak_ptr. After the shared
      // pointer is constructed (bit still in use), now the owner of the
      // observer resets its pointer that is pointing to the observer object.
      // This might result one to one references between the notifier and the
      // observer, i.e., as soon as the observer will be destroyed the notifier
      // will also be destroyed. It means, if now the observer object is
      // destroyed from the notify_imp() method, it will cause the notifier
      // object itself to be destroyed, where the notify_impl can still continue
      // to access its member variable (e.g. dispatcher). This might result an
      // undefined behaviour.
      //---------------------------------------------------------------------

      // Q: WHY need observers_strong vector? that's NOTE above but not sure?
      std::vector<std::shared_ptr<T>> observers_strong;

      for (auto const &o : observers_copy)
      {
        std::shared_ptr<T> strong = o.lock();
        if (strong)
        {
          // dispatcher expects void()
          dispatcher_->post(std::bind(f, strong));
        }
        observers_strong.push_back(strong);
      }

      // okay, notifying finishes
      lock.lock();

      // about to unregister an observer so make sure that there is no work
      // for this observer after that.
      if (dispatcher_ && (waitee_count_ > 0))
      {
        lock.unlock();
        // run all works in dispatcher queue. so not only for this observer
        // but for all.
        dispatcher_->sync();
        lock.lock();
      }

      notifying_ = false;

      // notify all waited observers on remove_observer()
      if (waitee_count_ > 0)
        cv_.notify_all();

      lock.unlock();
    }

  protected:
    template <typename F>
    void notify(F f)
    {
      notify_impl_(f);
    }

    template <typename F, typename... Args>
    void notify(F f, Args &&... args)
    {
      notify_impl_(
        std::bind(f, std::placeholders::_1, std::forward<Args>(args)...));
    }

  public:
    explicit Notifier() {}

    void addObserver(const std::shared_ptr<T> &observer)
    {
      std::lock_guard<std::mutex> lock(m_);
      observers_.emplace_back(observer);
    }

    void removeObserver(const std::shared_ptr<T> &observer)
    {
      std::unique_lock<std::mutex> lock(m_);

      // if (_BUILD_TYPE == DEBUG)
      if (dispatcher_ && dispatcher_->invoked_from_this())
      {
        throw std::logic_error(
          "potential deadlock since it should not be called from dispatcher");
      }
      // #endif

      // 1. have to use this form using index for searching since have to use
      // weak_ptr.lock().
      // 2. if shared pointer can be made from a weak pointer, it means a
      // object is still around and it is safe to use.

      for (size_t i = 0; i < observers_.size(); ++i)
      {
        if (observers_[i].lock() == observer)
        {
          observers_.erase(observers_.begin() + i);
        }
      }

      // observers can call this while notification from source is pending so
      // to make observers is still around, hold them until notification
      // finishes.
      // TODO: to do that, should it be moved before `deleting it`? that is to
      // move before `for loop` to search?

      if (notifying_)
      {
        waitee_count_++;

        // cxx-condition-variable-wait
        // same as `wait(lock, condition)` and blocks the current thread until
        // the condition variable is woken up.
        while (notifying_)
        {
          cv_.wait(lock);
        }

        waitee_count_--;
      }
    }

    void setDispatcher(const std::shared_ptr<Dispatcher> &dispatcher)
    {
      std::lock_guard<std::mutex> lock(m_);
      dispatcher_ = dispatcher;
    }
  };

  class Source : public Notifier<StateEvents>
  {
  public:
    void setState(int state)
    {
      // calls 'template <typename F> void notify(F f)'
      notify(
        std::bind(&StateEvents::stateChanged, std::placeholders::_1, state));
    }

    void setName(std::string name)
    {
      // use `convenient form`
      notify(&StateEvents::nameChanged, name);
    }

    void setKeyAndValue(std::string key, std::string value)
    {
      notify(&StateEvents::keyAndValueChanged, key, value);
    }

    void emitEvent() { notify(&StateEvents::eventOccurred); }
  };

  // observer
  // NOTE:
  // Arguably you could inherit directly from T, however inheriting from
  // Observer<T> is more intention revealing than inheriting from T. There is
  // no extra overhead because of Empty Base Class Optimisation.

  // necessary?
  // template <class T> class Observer : public T, virtual public Polymorphic

  template <typename T>
  class Observer : public T, public Polymorphic
  {};

  class Observer1 : public Observer<StateEvents>
  {
  public:
    // if use gmock
    MOCK_METHOD1(stateChanged, void(int));
    MOCK_METHOD1(nameChanged, void(std::string));
    MOCK_METHOD2(keyAndValueChanged, void(std::string, std::string));
    MOCK_METHOD0(eventOccurred, void());
  };

  class ObserverWrong : public Polymorphic
  {
  public:
    virtual void stateOccurred()
    {
      std::cout << "stateOccurred()" << std::endl;
    }
  };

  // Common/lib/include/CallerThreadDispatcher.h
  // do not use seperate thread for dispatcher and simply use caller thread as
  // if it's a `real` dispatcher

  class CallerThreadedDispatcher : public Dispatcher
  {
  public:
    virtual void post(std::function<void()> work) final { work(); }
    virtual void sync() final {}
    virtual bool invoked_from_this() final { return false; }
  };

} // namespace cxx_notifier_full_ex_2019_12

// what if use unrelated observer? type error at compile.
//
// test_cxx_ex.cpp:794:23: error: no matching function for call to
// ‘cxx_notifier_full_ex_2019_12::Source::addObserver(
//    std::shared_ptr<cxx_notifier_full_ex_2019_12::ObserverWrong>&)’
//   source.addObserver(o);
#if 0
TEST(PatternObserver, tryUnrelatedObserver)
{
  using namespace cxx_notifier_full_ex_2019_12;

  Source source;
  source.setDispatcher(std::make_shared<CallerThreadedDispatcher>());

  std::shared_ptr<ObserverWrong> o = std::make_shared<ObserverWrong>();
  source.addObserver(o);
}
#endif

#if 0

// can use real dispatcher but since not able to control dispatcher directly
// such as sync(), flush() or wait it to finish, cannot have deterministic
// result. Hence use CallerThreadDispatcher which call a work on calling thread.

TEST(PatternNotifierFull, sendNotificationWithRealDispatcher)
{
  using namespace cxx_notifier_full_ex_2019_12;

  Source noti;
  std::shared_ptr<Observer1> observer = std::make_shared<Observer1>();
  noti.set_dispatcher(std::make_shared<ThreadedDispatcher>());
  noti.add_observer(observer);

  EXPECT_CALL(*observer, stateChanged(5)).Times(1);

  noti.setState(5);
}
#endif

TEST(PatternNotifierFull, sendNotificationWithCallerDispatcher)
{
  using namespace cxx_notifier_full_ex_2019_12;

  Source source;
  source.setDispatcher(std::make_shared<CallerThreadedDispatcher>());

  std::shared_ptr<Observer1> o1 = std::make_shared<Observer1>();
  std::shared_ptr<Observer1> o2 = std::make_shared<Observer1>();
  source.addObserver(o1);
  source.addObserver(o2);

  EXPECT_CALL(*o1, stateChanged(5)).Times(1);
  EXPECT_CALL(*o2, stateChanged(5)).Times(1);
  source.setState(5);
}

TEST(PatternNotifierFull, sendNotificationManyArgs)
{
  using namespace cxx_notifier_full_ex_2019_12;

  Source noti;
  std::shared_ptr<Observer1> observer = std::make_shared<Observer1>();
  noti.setDispatcher(std::make_shared<CallerThreadedDispatcher>());
  noti.addObserver(observer);

  EXPECT_CALL(*observer, keyAndValueChanged("key", "value")).Times(1);

  noti.setKeyAndValue("key", "value");
}

/*
={=========================================================================
cxx_pattern_dispatcher

*/

namespace cxx_dispatcher_ex_2019_12
{
  // Inherit from this for all types that have virtual functions. Doing so
  // ensures that you have virtual destructor and saves you nasty surprises.
  class Polymorphic
  {
  public:
    virtual ~Polymorphic() {}
  };

  // IDispatcher.h
  class Dispatcher : public Polymorphic
  {
  public:
    // post an `work` to be run
    virtual void post(std::function<void()>) = 0;

    // ensure that any works in the queue before tha call has been run
    virtual void sync() = 0;

    // check if it's called from this dispatcher thread
    virtual bool invoked_from_this() = 0;
  };

  // ThreadedDispatcher.h
  class ThreadedDispatcher : public Dispatcher
  {
  private:
    std::thread t_;
    std::mutex m_;
    std::condition_variable cv_;

    std::deque<std::function<void()>> q_;

    bool running_;

    void do_work_(const std::string &name, int priority);
    std::function<void()> next_();

  public:
    ThreadedDispatcher(const std::string &name = std::string());
    ThreadedDispatcher(int priority, const std::string &name = std::string());
    ~ThreadedDispatcher();

    // perform any remaining work in the queue and stop taking new work
    void flush();

    // stop a dispatcher even if there are works in the queue
    void stop();

  public:
    virtual void post(std::function<void()>);
    virtual void sync();
    virtual bool invoked_from_this();
  };

  // *cxx-error* : ISO C++ forbids taking the address of an
  // unqualified or parenthesized non-static member function to form a pointer
  // to member function.
  // Say ‘&cxx_pattern_dispatcher::ThreadedDispatcher::_do_work’ [-fpermissive]
  //
  // _t(std::thread(&_do_work, this, name, priority))

  ThreadedDispatcher::ThreadedDispatcher(int priority, const std::string &name)
      : running_(true)
      , t_(std::thread(&ThreadedDispatcher::do_work_, this, name, priority))
  {}

  // see that ctor calls ctor
  ThreadedDispatcher::ThreadedDispatcher(const std::string &name)
      : ThreadedDispatcher(-1, name)
  {}

  ThreadedDispatcher::~ThreadedDispatcher()
  {
    if (running_)
      stop();
  }

  void ThreadedDispatcher::do_work_(const std::string &name, int priority)
  {
    if (prctl(PR_SET_NAME,
              name.empty() ? "DISPATCHER" : name.c_str(),
              0,
              0,
              0) < 0)
    {
      // LOG_ERR("failed to set thread name");
    }

    // if (priority > 0)
    // {
    //   struct sched_param param;
    //   param.sched_priority = priority;
    //   int err = pthread_setschedparam(pthread_self(), SCHED_RR, &param);
    //   if (err != 0)
    //   {
    //     AI_LOG_SYS_ERROR(err, "Failed to set thread priority to %d",
    //     priority);
    //   }
    // }

    std::unique_lock<std::mutex> lock(m_);

    // 1. exit from wait() when running_ is false which means when stop() is
    // called and there are works in the queue, runs one work and ends dispatch
    // thread.
    //
    // 2. the original code:
    //
    // bool ThreadedDispatcher::hasMoreWorkOrWasStopRequested()
    // { return !q.empty() || !running; }
    //
    // cv.wait(lock, bind(&This::hasMoreWorkOrWasStopRequested, this));

    while (running_)
    {
      // wait until q is not empty or running is true
      cv_.wait(lock, [&] { return !q_.empty() || !running_; });

      // check again for sure
      if (!q_.empty())
      {
        auto work = next_();

        // while calling `work()`, allow uses to post more works
        lock.unlock();
        // this makes `void f()`
        work();
        lock.lock();
      }
    }
  }

  std::function<void()> ThreadedDispatcher::next_()
  {
    // necessary? seems not
    // auto work = std::move(q_.front());

    auto work = q_.front();
    q_.pop_front();
    return work;
  }

  // this is original code. so the difference is to use unique_lock for fine
  // control. does it run faster??
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

  void ThreadedDispatcher::post(std::function<void()> work)
  {
    std::lock_guard<std::mutex> lock(m_);

    if (running_)
    {
      q_.push_back(work);
      cv_.notify_one();
    }
    else
    {
      std::cout << "ignore work as the dispatcher is not running" << std::endl;

      // NOTE: when occurrs?
      // original comment:
      // can't throw an exception here because if this is executed from
      // destructor, which occurs when work adds more work things go horribly
      // wrong. Instead, ignore work.
    }
  }

  bool ThreadedDispatcher::invoked_from_this()
  {
    // see if calling thread is the same as dispatcher t_ thread
    return (std::this_thread::get_id() == t_.get_id());
  }

  namespace
  {
    // 1. the original uses unique_lock and it seems unnecessary
    // 2. when dispatcher calls this, how does it gets all arguments? all
    //    arguments are vaild to use? through std::bind()
    // 3. use of pointer but not reference which is used in flush()

    void syncCallback_(std::mutex *m, std::condition_variable *cv, bool *done)
    {
      // NOTE: using lock_guard is a problem? nope.
      // std::unique_lock<std::mutex> lock(*m);
      std::lock_guard<std::mutex> lock(*m);
      *done = true;
      cv->notify_all();
      // unnecessary
      // lock.unlock();
    }
  } // namespace

  // ensure that any works in the queue before tha call has been run
  void ThreadedDispatcher::sync()
  {
    std::mutex sm;
    std::condition_variable scv;
    bool done{false};

    std::unique_lock<std::mutex> lock(m_);

    if (!running_)
    {
      std::cout << "ignoring sync request because the dispatcher is not running"
                << std::endl;
      return;
    }

    q_.push_back(std::bind(syncCallback_, &sm, &scv, &done));
    lock.unlock();

    // signal dispatcher to run
    cv_.notify_one();

    // wait for `sync work` to be done
    std::unique_lock<std::mutex> sync_lock(sm);

    // same as wait(sync_lock, predicate) *cxx-condition-variable-wait*
    while (!done)
    {
      scv.wait(sync_lock);
    }
  }

  namespace
  {
    // original code
    // void unlockAndSetFlagToFalse(std::mutex& m, bool& flag)
    // {
    //   using namespace std;
    //   m.unlock();
    //   flag = false;
    // }

    void unlockAndSetFlagToFalse_(std::mutex &m, bool &flag)
    {
      // NOTE: okay without this since flush() calls stop() which do set flag
      // false?
      // flag = false;
      m.unlock();
    }
  } // namespace

  // void ThreadedDispatcher::flush()
  // {
  //   if (running_)
  //   {
  //     std::mutex m2;
  //     m2.lock();
  //     // *cxx-bind*
  //     post(std::bind(unlockAndSetFlagToFalse,
  //                    std::ref(m2),
  //                    std::ref(this->running_)));
  //     // blocks here until unlockAndSetFlagToFalse() unlock it since post()
  //     will
  //     // signal and it make signal false as stop() do.
  //     m2.lock();
  //     m2.unlock();
  //     stop();
  //   }
  //   else
  //   {
  //     std::cout
  //       << "ignoring flush request because the dispatcher is not running"
  //       << std::endl;
  //   }
  // }

  void ThreadedDispatcher::flush()
  {
    std::mutex fm;

    if (!running_)
    {
      std::cout << "ignoring sync request because the dispatcher is not running"
                << std::endl;
      return;
    }

    fm.lock();

    post(std::bind(unlockAndSetFlagToFalse_,
                   std::ref(fm),
                   std::ref(this->running_)));

    // unlike sync() which use condition variable, use `double lock` on mutex
    // waits for callback is called
    // NOTE: that is flush() is the same as sync except doing stop().
    fm.lock();
    fm.unlock();
    stop();
  }

  void ThreadedDispatcher::stop()
  {
    std::unique_lock<std::mutex> lock(m_);
    running_ = false;
    lock.unlock();

    cv_.notify_one();
    t_.join();
  }

} // namespace cxx_dispatcher_ex_2019_12

namespace
{
  void work1(int &what, int value) { what += value; }

  class Work
  {
  private:
    int &what_; // see reference
    int value_;

  public:
    Work(int &what, int value)
        : what_(what)
        , value_(value)
    {}

    void assign() { what_ += value_; }
  };
} // namespace

TEST(PatternDispatcher, checkPostedWorkDone)
{
  using namespace cxx_dispatcher_ex_2019_12;

  // single work
  {
    int value{};
    std::shared_ptr<ThreadedDispatcher> td =
      std::make_shared<ThreadedDispatcher>();

    td->post(std::bind(work1, std::ref(value), 10));

    std::this_thread::sleep_for(chrono::seconds(1));
    EXPECT_THAT(value, 10);
  }

  // more work
  {
    int value{};
    std::shared_ptr<ThreadedDispatcher> td =
      std::make_shared<ThreadedDispatcher>();

    td->post(std::bind(work1, std::ref(value), 10));
    td->post(std::bind(work1, std::ref(value), 10));
    td->post(std::bind(work1, std::ref(value), 10));
    td->post(std::bind(work1, std::ref(value), 10));

    std::this_thread::sleep_for(chrono::seconds(1));
    EXPECT_THAT(value, 40);
  }
}

TEST(PatternDispatcher, checkPostedMemberFunctionDone)
{
  using namespace cxx_dispatcher_ex_2019_12;

  // single work
  {
    int value{};
    Work w1(std::ref(value), 10);

    std::shared_ptr<ThreadedDispatcher> td =
      std::make_shared<ThreadedDispatcher>();

    td->post(std::bind(&Work::assign, &w1));

    std::this_thread::sleep_for(chrono::seconds(1));
    EXPECT_THAT(value, 10);
  }
}

TEST(PatternDispatcher, checkFlush)
{
  using namespace cxx_dispatcher_ex_2019_12;

  // do one work and uses flush()
  {
    int value{};
    std::shared_ptr<ThreadedDispatcher> td =
      std::make_shared<ThreadedDispatcher>();

    td->post(std::bind(work1, std::ref(value), 10));
    td->flush();

    // since flush() used so no need to sleep
    // std::this_thread::sleep_for(chrono::seconds(2));

    EXPECT_THAT(value, 10);
  }
}

namespace
{
  void checkThreadId_(std::thread::id id)
  {
    EXPECT_NE(std::this_thread::get_id(), id);
  }
} // namespace

TEST(PatternDispatcher, checkDoneOnDispatcher)
{
  using namespace cxx_dispatcher_ex_2019_12;

  {
    int value{};
    std::shared_ptr<ThreadedDispatcher> td =
      std::make_shared<ThreadedDispatcher>();

    // expect false since calling thread is not dispatcher
    EXPECT_THAT(td->invoked_from_this(), false);

    td->post(std::bind(checkThreadId_,
                       std::this_thread::get_id() // calling thread id
                       ));

    std::this_thread::sleep_for(chrono::seconds(1));
  }
}

namespace
{
  void saveSequence_(int &value)
  {
    static int sequence{33};
    value = ++sequence;
  }
} // namespace

TEST(PatternDispatcher, checkDoneInOrder)
{
  using namespace cxx_dispatcher_ex_2019_12;

  {
    int v1{};
    int v2{};
    int v3{};

    std::shared_ptr<ThreadedDispatcher> td =
      std::make_shared<ThreadedDispatcher>();

    td->post(std::bind(saveSequence_, std::ref(v1)));
    td->post(std::bind(saveSequence_, std::ref(v2)));
    td->post(std::bind(saveSequence_, std::ref(v3)));

    std::this_thread::sleep_for(chrono::seconds(1));

    EXPECT_THAT(v1, 34);
    EXPECT_THAT(v2, 35);
    EXPECT_THAT(v3, 36);
  }
}

// do not see possibilities of deadlock in using stop()
TEST(PatternDispatcher, checkStopDoNotCauseDeadlock)
{
  using namespace cxx_dispatcher_ex_2019_12;

  {
    int v1{};
    int v2{};
    int v3{};

    std::shared_ptr<ThreadedDispatcher> td =
      std::make_shared<ThreadedDispatcher>();

    td->post(std::bind(saveSequence_, std::ref(v1)));
    td->post(std::bind(saveSequence_, std::ref(v2)));
    td->post(std::bind(saveSequence_, std::ref(v3)));

    td->stop();
  }
}

namespace
{
  void doIncrements_(int &value)
  {
    ++value;
    std::this_thread::sleep_for(chrono::milliseconds(10));
  }

  void doDecrements_(int &value)
  {
    {
      --value;
      std::this_thread::sleep_for(chrono::milliseconds(20));
    }
  }
} // namespace

TEST(PatternDispatcher, doLostOfWorks)
{
  using namespace cxx_dispatcher_ex_2019_12;

  // single work
  {
    int value{};
    const int count{100000};

    std::shared_ptr<ThreadedDispatcher> td =
      std::make_shared<ThreadedDispatcher>();

    for (int i = 0; i < count; ++i)
    {
      td->post(std::bind(doIncrements_, std::ref(value)));
    }

    td->flush();

    // expect that the number of work done must be count value.
    EXPECT_THAT(value, count);
  }

  // multiple works
  {
    int value{};
    const int count{100000};

    std::shared_ptr<ThreadedDispatcher> td =
      std::make_shared<ThreadedDispatcher>();

    for (int i = 0; i < count; ++i)
    {
      if ((i % 2) == 0)
        td->post(std::bind(doIncrements_, std::ref(value)));
      else
        td->post(std::bind(doDecrements_, std::ref(value)));
    }

    td->flush();

    // expect that the number of work done must be count value.
    EXPECT_THAT(value, 0);
  }
}

namespace
{
  void notify_(std::mutex &m, std::condition_variable &cv)
  {
    std::lock_guard<std::mutex> lock(m);
    cv.notify_one();
  }
} // namespace

TEST(PatternDispatcher, checkDonwInTimeout)
{
  using namespace cxx_dispatcher_ex_2019_12;

  // since there is no determistic way to check how many works can be done in
  // certain time, do one test in timeout.
  {
    // std::shared_ptr<ThreadedDispatcher> td =
    //   std::make_shared<ThreadedDispatcher>();

    auto td = std::make_shared<ThreadedDispatcher>();

    std::mutex m;
    std::condition_variable cv;
    std::unique_lock<std::mutex> lock(m);

    td->post(std::bind(notify_, std::ref(m), std::ref(cv)));

    // *cxx-condition-variable-wait*
    // check if condition is signaled within timeout value
    EXPECT_THAT(cv.wait_for(lock, std::chrono::seconds(5)),
                std::cv_status::no_timeout);
  }
}

TEST(PatternDispatcher, checkSync)
{
  using namespace cxx_dispatcher_ex_2019_12;

  // single work
  {
    int value{};
    const int count{100000};

    std::shared_ptr<ThreadedDispatcher> td =
      std::make_shared<ThreadedDispatcher>();

    for (int i = 0; i < count; ++i)
    {
      td->post(std::bind(doIncrements_, std::ref(value)));
    }

    td->sync();

    // expect that the number of work done must be count value.
    EXPECT_THAT(value, count);
  }
}

// ={=========================================================================

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
