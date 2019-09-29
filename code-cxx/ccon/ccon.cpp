#include <iostream>
#include <thread>
#include <future>
#include <list>
#include <mutex>
#include <queue>
#include <stack>
#include <exception>
#include <random>

#include <boost/thread/shared_mutex.hpp>
// #include <boost/thread/thread.hpp>

#include <sys/eventfd.h>
#include <unistd.h>

#include "gmock/gmock.h"
#include "slog.h"

using namespace std;
using namespace std::placeholders;
using namespace testing;


// CXXSLR 18.1 The High-Level Interface: async() and Futures
//
// To visualize what happens, we simulate the complex processings in func1() and
// func2() by calling doSomething(),

int doSomething (char c)
{
  // *cxx-random*
  default_random_engine dre(c);
  uniform_int_distribution<int> id(10, 1000);

  for(int i=0; i<10; ++i)
  {
    this_thread::sleep_for(chrono::milliseconds( id(dre) ));
    cout.put(c).flush();
  }
  cout.put('\n');

  return c;
}


// ={=========================================================================
// cxx-thread

namespace cxx_thread {

  void hello(ostringstream &os)
  {
    os << "Hello Concurrent World";
  }

  void hello_and_thread_id(ostringstream &os)
  {
    os << "Hello Concurrent World";
    cout << "thread id: " << std::this_thread::get_id() << endl;
  }
} // namespace

TEST(CConThread, Hello)
{
  using namespace cxx_thread;

  ostringstream os;
  std::thread t([&]{hello(os);});
  t.join();
  EXPECT_THAT(os.str(), "Hello Concurrent World");
}

// thread id: 139805217429248

TEST(CConThread, Id)
{
  using namespace cxx_thread;

  ostringstream os;
  std::thread t([&]{hello_and_thread_id(os);});
  t.join();
  EXPECT_THAT(os.str(), "Hello Concurrent World");
}

// +..+..+...+..+.+++++
// done

TEST(CConThread, Lambda)
{
  std::thread t1([]{doSomething('.');});
  std::thread t2([]{doSomething('+');});
  t1.join();
  t2.join();
}

// true if the thread object identifies an active thread of execution, false
// otherwise

TEST(CConThread, Joinable)
{
  std::thread t([]{doSomething('+');});

  std::cout << "thread is still running? " << std::boolalpha 
    << t.joinable() << std::endl;
  
  // DO NOT WORK as hoped since while() do not ends. Proabaly, it makes a thread
  // keep alive. 
  //
  // while (t.joinable())
  // {
  //   std::cout << "thread is still running" << std::endl;
  //   std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  // }

  t.join();

  std::cout << "thread is still running? " << std::boolalpha 
    << t.joinable() << std::endl;
}


namespace cxx_thread {

  void thread_name_1(ostringstream &os)
  {
    // get thread name
    char name[16];
    pthread_getname_np(pthread_self(), name, sizeof(name));
    os << name;
  }

  void thread_name_2(ostringstream &os)
  {
    // set thread name and get it to see if that works
    //
    // SYNOPSIS
    //        #define _GNU_SOURCE             /* See feature_test_macros(7) */
    //        #include <pthread.h>
    //        int pthread_setname_np(pthread_t thread, const char *name);
    //        int pthread_getname_np(pthread_t thread,
    //                              char *name, size_t len);

    char name[16];
    pthread_setname_np(pthread_self(), "CCON_THREAD");
    pthread_getname_np(pthread_self(), name, sizeof(name));
    os << name;
  }
} // namespace


TEST(CConThread, Name)
{
  using namespace cxx_thread;

  {
    ostringstream os;
    std::thread t([&]{thread_name_1(os);});
    t.join();
    // this output file name
    EXPECT_THAT(os.str(), "ccon_out");
  }

  {
    ostringstream os;
    std::thread t([&]{thread_name_2(os);});
    t.join();
    EXPECT_THAT(os.str(), "CCON_THREAD");
  }
}


namespace cxx_thread {

  void update_data(std::string &data)
  {
    data = "updated data";
  }

  void use_reference(int &value)
  {
    value += 200;
  }

  void use_value(int value)
  {
    value += 200;
  }

} // namespace


// show how arg and return value from thread are used

TEST(CConThread, ArgumentAndReturn)
{
  using namespace cxx_thread;

  // value
  {
    int value{1};

    std::thread t(use_value, value); 
    t.join();

    EXPECT_THAT(value, 1);
  }

  // cause error:
  //
  // {
  //   int value{1};

  //   // In file included from /usr/include/c++/4.9/thread:39:0,
  //   //                  from ccon.cpp:2:
  //   // /usr/include/c++/4.9/functional: In instantiation of ‘struct std::_Bind_simple<void (*(int))(int&)>’:
  //   // /usr/include/c++/4.9/thread:140:47:   required from ‘std::thread::thread(_Callable&&, _Args&& ...) [with _Callable = void (&)(int&); _Args = {int&}]’
  //   // ccon.cpp:418:36:   required from here
  //   // /usr/include/c++/4.9/functional:1665:61: error: no type named ‘type’ in ‘class std::result_of<void (*(int))(int&)>’
  //   //        typedef typename result_of<_Callable(_Args...)>::type result_type;
  //   //                                                              ^
  //   // /usr/include/c++/4.9/functional:1695:9: error: no type named ‘type’ in ‘class std::result_of<void (*(int))(int&)>’
  //   //          _M_invoke(_Index_tuple<_Indices...>)
  //   //          ^
  //   // makefile:58: recipe for target 'ccon.o' failed
  //   // make: *** [ccon.o] Error 1

  //   std::thread t(use_reference, value);  
  //   t.join();

  //   EXPECT_THAT(value, 201);
  // }
  // 
  // TODO:
  // to understand this error, have to understand cxx-bind in 
  // /usr/include/c++/4.9/functional since cxx-thread uses __bind_simple()

  {
    int value{1};

    // *cxx-ref*
    std::thread t(use_reference, std::ref(value));  
    t.join();

    EXPECT_THAT(value, 201);
  }

  {
    int value{1};

    std::thread t([&]{use_reference(value);}); 
    t.join();

    EXPECT_THAT(value, 201);
  }

  // Likewise, have to use std::ref() to compile and to work
  {
    std::string data{"data"};
    std::thread t(update_data, std::ref(data));
    t.join();

    // data is not updated
    EXPECT_THAT(data, string("updated data"));
  }
}


namespace cxx_thread {

  class Foo
  {
    public:
      Foo(int value = 10) : value_(value) {}
      void update_value() { value_ += 10; };
      int get_value() { return value_; }

    private:
      int value_;
  };

} // namespace


TEST(CConThread, MemberFunction)
{
  using namespace cxx_thread;

  Foo foo;

  // not
  // std::thread t(&foo::update_value, &foo);

  std::thread t(&Foo::update_value, &foo);
  t.join();
  EXPECT_THAT(foo.get_value(), 20);
}


namespace cxx_thread 
{
  std::mutex priority_m;

  void f_get_prority(int num)
  {
    std::this_thread::sleep_for(std::chrono::seconds(1));

    sched_param sch{};
    int policy;

    // pthread_t pthread_self(void);
    //
    // int pthread_setschedparam(pthread_t thread, int policy,
    //                                 const struct sched_param *param);
    // int pthread_getschedparam(pthread_t thread, int *policy,
    //                                 struct sched_param *param);

    pthread_getschedparam(pthread_self(), &policy, &sch);

    std::lock_guard<std::mutex> lock(priority_m);
    LOG_MSG("thread %d is running at priority %d", num, sch.sched_priority);
  }
} // namespace

TEST(CConThread, Priority1)
{
  using namespace cxx_thread;

  std::thread t1(f_get_prority, 1);
  std::thread t2(f_get_prority, 2);

  sched_param sch{};
  int policy;

  // int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param);
  //
  // https://en.cppreference.com/w/cpp/thread/thread/native_handle
  // std::thread::native_handle
  // native_handle_type native_handle();
  // (since C++11)
  // Returns the implementation defined underlying thread handle.

  pthread_getschedparam(t1.native_handle(), &policy, &sch);
  LOG_MSG("current priority %d", sch.sched_priority);

  // have to run it as `root`. otherwise, gets error:
  // LOG| F:ccon.cpp C:virtual void CConThread_Priority_Test::TestBody() L:00342 
  //  [EPERM Operation not permitted] failed to pthread_setschedparam

  sch.sched_priority = 20;
  if (pthread_setschedparam(t1.native_handle(), SCHED_FIFO, &sch))
  {
    LOG_MSG("failed to pthread_setschedparam");
  }

  t1.join();
  t2.join();
}

TEST(CConThread, Priority2)
{
  using namespace cxx_thread;

  int policy;

  // int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param);
  //
  // https://en.cppreference.com/w/cpp/thread/thread/native_handle
  // std::thread::native_handle
  // native_handle_type native_handle();
  // (since C++11)
  // Returns the implementation defined underlying thread handle.

  // int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);
  // int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy);
  //
  // 380:47: error: invalid conversion from 
  // ‘std::thread::native_handle_type {aka long unsigned int}’ to 
  // ‘const pthread_attr_t*’ [-fpermissive]
  //
  // pthread_attr_getschedpolicy(t1.native_handle(), &policy);

  // pthread_getattr_np() is a non-standard GNU extension that retrieves the
  // attributes of the thread specified in its first argument
  //
  //        #define _GNU_SOURCE             See feature_test_macros(7)
  //        #include <pthread.h>
  // 
  //        int pthread_getattr_np(pthread_t thread, pthread_attr_t *attr);
  // 
  //        Compile and link with -pthread.
  // 
  // DESCRIPTION
  //        The  pthread_getattr_np()  function  initializes  the  thread
  //        attributes  object referred to by attr so that it contains actual
  //        attribute values describing the running thread thread.

  pthread_attr_t gattr;
  if (pthread_getattr_np(pthread_self(), &gattr))
  {
    LOG_MSG("failed to pthread_getattr_np");
  }

  pthread_attr_getschedpolicy(&gattr, &policy);

  // #define SCHED_NORMAL 0
  EXPECT_THAT(policy, SCHED_OTHER);

  pthread_attr_setschedpolicy(&gattr, SCHED_RR);
  pthread_attr_getschedpolicy(&gattr, &policy);

  // #define SCHED_RR     2
  EXPECT_THAT(policy, SCHED_RR);
}


// CCON listing_2.8

namespace cxx_thread {

  class IntegerSequence
  {
    public:
      IntegerSequence(int value) : value_(value) {}

      int operator()()
      { return ++value_; }

    private:
      int value_;
  };

  // cxx-fobj to use with cxx-thread

  template <typename Iterator, typename T>
    struct accumulate_block
    {
      void operator()(Iterator first, Iterator last, T& result)
      {
        // result = std::accumulate(first, last, 0);
        result = std::accumulate(first, last, result);
      }
    };

  template <typename Iterator, typename T>
    T parallel_accumulate(Iterator first, Iterator last, T init)
    {
      unsigned long const length = std::distance(first, last);

      if (!length)
        return init;

      // minimum block size per thread
      unsigned long const min_per_thread = 25;
      unsigned long const max_thread =
        (length + min_per_thread -1)/min_per_thread;

      unsigned long const hardware_threads = 
        std::thread::hardware_concurrency();

      // cout << "hardware_threads: " << hardware_threads << endl; 

      // when runs on VM, hardware_threads is 1 and it make num_threads is 1
      // unsigned long const num_threads =
      //   std::min(hardware_threads != 0 ? hardware_threads : 2, max_thread);

      unsigned long const num_threads =
        std::min(hardware_threads > 1 ? hardware_threads : 4, max_thread);

      // block size per thread
      unsigned long const block_size = length / num_threads;

      // to launch one fewer than num_threads since you have main thread
      std::vector<T> results(num_threads);
      std::vector<std::thread> threads(num_threads -1);

      Iterator block_start = first;

      // // input distance
      // cout << "num_threads: " << num_threads << " distance: " 
      //   << std::distance(block_start, last) << endl;

      for (unsigned long i = 0; i < (num_threads -1); ++i)
      {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);

        // cout << "num_threads: " << num_threads << " distance: " 
        //   << std::distance(block_start, block_end) << endl;

        threads[i] = std::thread(
            accumulate_block<Iterator, T>(),
            block_start,
            block_end,
            std::ref(results[i])
            );
        block_start = block_end;
      }

      // final block for main thread
      accumulate_block<Iterator, T>()(
          block_start, last, results[num_threads-1]
          );

      std::for_each(threads.begin(), threads.end(),
          std::mem_fn(&std::thread::join));

      return std::accumulate(results.begin(), results.end(), init);
    }

} // namespace

TEST(CConThread, ParallelAccumulate)
{
  using namespace cxx_thread;

  std::vector<int> coll;

  std::generate_n(back_inserter(coll), 10000, IntegerSequence(0));

  auto coll_sum = parallel_accumulate(coll.begin(), coll.end(), 0);


  std::vector<int> result;

  // since IntegerSequence starts from value 1.
  for (int i = 1; i <= 10000; ++i)
    result.push_back(i);

  auto result_sum = std::accumulate(result.begin(), result.end(), 0);

  EXPECT_THAT(coll_sum, result_sum);
}


// ={=========================================================================
// cxx-async

namespace cxx_async {

  int sum{0};

  int find_the_answer()
  {
    this_thread::sleep_for(chrono::milliseconds(100));
    return sum + 90;
  }

  void do_other_stuff()
  {
    this_thread::sleep_for(chrono::seconds(20));
    sum = 10;
  }

  // as with cxx-thread
  class Foo
  {
    public:
      Foo(int value = 10) : value_(value) {}
      void update_value() { value_ += 10; };
      int get_value() { return value_; }

    private:
      int value_;
  };

} // namespace


TEST(CConAsync, MemberFunction)
{
  using namespace cxx_async;

  {
    Foo foo;

    auto result = std::async(&Foo::update_value, &foo);
    // since it's not run yet
    EXPECT_THAT(foo.get_value(), 10);

    result.get();

    // run and expect the update
    EXPECT_THAT(foo.get_value(), 20);
  }
}


// expect that do_other_stuff() finishes first

TEST(CConAsync, Async)
{
  using namespace cxx_async;

  std::future<int> the_answer = std::async(find_the_answer);
  do_other_stuff();
  EXPECT_THAT(the_answer.get(), 100);
}


// Note, finally, that the object passed to async() may be any type of a
// callable object:
//
// A call of async() does not guarantee that the passed functionality gets
// started and finished.
// 
// It is necessary to ensure that sooner or later, the passed functionality gets
// called. Note that I wrote that async() `tries` to start the passed
// functionality. If this didn’t happen we need the future object to force a
// start when we need the result or want to ensure that the functionality was
// performed. Thus, you need the future object even if you are not interested in
// the outcome of a functionality started in the background.
//
// Accordingly, two kinds of outputs are possible for this program. 


// cxx-async with no option
//
// If async() couldn’t start func1(), it will run after func2(), when get() gets
// called, so that the program will have the following output:
//
// [ RUN      ] CConAsync.Default
// ++++++++++
// ..........
// [       OK ] CConAsync.Default (7064 ms)
//
// this is `sequential`

TEST(CConAsync, Default)
{
  future<int> result1(
      std::async([]{return doSomething('.');}));

  int result2 = doSomething('+');

  // . is 46 and + is 43 since doSomething() returns the input char
  int result = result1.get() + result2;

  EXPECT_THAT(result, 89);
}


// Using Launch Policies
//
// You can force async() to not defer the passed functionality, by explicitly
// passing a launch policy directing async() that it should definitely start the
// passed functionality asynchronously the moment it is called:
//
// cxx-async with async option
//
// If async() could successfully start func1(), the output might be something
// like the following:
//
// [ RUN      ] CConAsync.LaunchPolicy
// +..+..+...+..+.
// +++++
// [       OK ] CConAsync.LaunchPolicy (3874 ms)

TEST(CConAsync, LaunchPolicy)
{
  future<int> result1(
      std::async(launch::async, []{return doSomething('.');}));

  int result2 = doSomething('+');

  int result = result1.get() + result2;

  EXPECT_THAT(result, 89);
}


// So, based on this first example, we can define a general way to make a
// program faster: You can modify the program so that it might benefit from
// parallelization, if the underlying platform supports it, but still works as
// before on single-threaded environments.
//
// Note also that you have to ensure that you ask for the result of a
// functionality started with async() no earlier than necessary. For example,
// the following “optimization” is probably not what you want:
//
// std::future<int> result1(std::async(func1));
// int result = func2() + result1.get(); // might call func2() after func1() ends
//
// Because the evaluation order on the right side of the second statement is
// unspecified, result1.get() might be called before func2() so that you have
// sequential processing again.


// `If you don’t assign the result of std::async(std::launch::async,...)`
// anywhere, the caller will block until the passed functionality has finished,
// which would mean that this is nothing but a synchronous call.

TEST(CConAsync, NotUseResultAndSequential)
{
  // future<int> result1(std::async(launch::async, []{return doSomething('.');}));
  std::async(launch::async, []{return doSomething('.');});

  doSomething('+');
}


// starting 2 operations synchronously
// 
// ..........
// ++++++++++
// 
// done
// starting 2 operations asynchronously
// 
// deffered
// -..
// 
// +++++
// 
// done

TEST(CConAsync, Status)
{
  {
    cout << "starting 2 operations synchronously" << endl;

    // start two loops in the background printing characters . or +
    auto f1 = async([]{ doSomething('.'); });
    auto f2 = async([]{ doSomething('+'); });

    // if at least one of the background tasks is running
    if (f1.wait_for(chrono::seconds(0)) != future_status::deferred ||
        f2.wait_for(chrono::seconds(0)) != future_status::deferred) 
    {
      cout << "\ndeffered" << endl;

      // poll until at least one of the loops finished
      while (f1.wait_for(chrono::seconds(0)) != future_status::ready &&
          f2.wait_for(chrono::seconds(0)) != future_status::ready) 
      {
        //...;
        cout << "yield,";
        this_thread::yield();  // hint to reschedule to the next thread
      }
    }
    cout.put('\n').flush();

    // wait for all loops to be finished and process any exception
    try {
      f1.get();
      f2.get();
    }
    catch (const exception& e) {
      cout << "\nEXCEPTION: " << e.what() << endl;
    }
    cout << "\ndone" << endl;
  }

  {
    string const waits{"\\|/-"};
    int i{};

    cout << "starting 2 operations asynchronously" << endl;

    // start two loops in the background printing characters . or +
    auto f1 = async(std::launch::async, []{ doSomething('.'); });
    auto f2 = async(std::launch::async, []{ doSomething('+'); });

    // if at least one of the background tasks is running
    if (f1.wait_for(chrono::seconds(0)) != future_status::deferred ||
        f2.wait_for(chrono::seconds(0)) != future_status::deferred) 
    {
      cout << "\ndeffered" << endl;

      // poll until at least one of the loops finished
      while (f1.wait_for(chrono::seconds(0)) != future_status::ready &&
          f2.wait_for(chrono::seconds(0)) != future_status::ready) 
      {
        //...;
        cout << flush << "\r" << waits[i%4];
        ++i;

        this_thread::yield();  // hint to reschedule to the next thread
      }
    }
    cout.put('\n').flush();

    // wait for all loops to be finished and process any exception
    try {
      f1.get();
      f2.get();
    }
    catch (const exception& e) {
      cout << "\nEXCEPTION: " << e.what() << endl;
    }
    cout << "\ndone" << endl;
  }
}


// ={=========================================================================
// cxx-lock cxx-mutex

// CXXSLR 18.5 A First Complete Example for Using a Mutex and a Lock

namespace cxx_sync
{
  std::mutex print_mutex;

  size_t i{};

  std::string print(std::string const& s)
  {
    std::lock_guard<std::mutex> l(print_mutex);
    std::string result{};

    for (i = 0; i < s.size(); ++i)
    {
      this_thread::sleep_for(chrono::milliseconds(20));
    }

    result = "waited for " + to_string(i*20) + "ms and " + s;
    return result;
  }

  std::string print_no_lock(std::string const& s)
  {
    // std::lock_guard<std::mutex> l(print_mutex);

    std::string result{};

    for (i = 0; i < s.size(); ++i)
    {
      this_thread::sleep_for(chrono::milliseconds(20));
    }

    result = "waited for " + to_string(i*20) + "ms and " + s;
    return result;
  }
} // cxx_sync


// note that this shows `deadlock` becuase calls lock() twice already before
// running a thread which calls unlock().
//
// TEST(CCon, Sync_MultipleLockAndUnlock)
// {
//   std::mutex print_mutex;
// 
//   std::mutex m;
//   bool result{false};
// 
//   m.lock();
//   m.lock();
// 
//   std::thread t([&]{m.unlock(); std::this_thread::sleep_for(std::chrono::seconds(2));});
//   std::cout << "set" << std::endl;
//   result = true;
//   m.unlock();
//   t.join();
// 
//   EXPECT_THAT(result, true);
// }


// LPI 30.1.7 Mutex Types
//
// "On Linux, both of these operations succeed for this mutex type and a
// PTHREAD_MUTEX_DEFAULT mutex behaves like a PTHREAD_MUTEX_NORMAL mutex.

TEST(CConSync, MultipleLockAndUnlock)
{
  std::mutex m;
  bool result{false};

  m.lock();

  std::thread t([&]{std::this_thread::sleep_for(std::chrono::seconds(2)); m.unlock(); });

  m.lock();
  std::cout << "set" << std::endl;
  result = true;

  // note that this works without calling second unlock() since *cxx-raii*
  // m.unlock();

  t.join();

  EXPECT_THAT(result, true);
}

TEST(CConSync, LockGuard)
{
  using namespace cxx_sync;

  {
    auto f1 = std::async(std::launch::async,
        print, "Hello from a first thread");

    auto f2 = std::async(std::launch::async,
        print, "Hello from a second thread");

    EXPECT_THAT(f1.get(), 
        "waited for 500ms and Hello from a first thread");

    EXPECT_THAT(f2.get(), 
        "waited for 520ms and Hello from a second thread");
  }

  // sometimes pass or sometimes fail
  // {
  //   auto f1 = std::async(std::launch::async,
  //       print_no_lock, "Hello from a first thread");

  //   auto f2 = std::async(std::launch::async,
  //       print_no_lock, "Hello from a second thread");

  //   EXPECT_THAT(f1.get(), 
  //       Ne("waited for 500ms and Hello from a first thread"));

  //   EXPECT_THAT(f2.get(), 
  //       Ne("waited for 520ms and Hello from a second thread"));
  // }
}

// output is always that f2 comes first:
//
// f2 writes -WRITE
// f1 writes READ

TEST(CConSync, Eventfd)
{
  std::string result{};

  int efd = eventfd(0, EFD_CLOEXEC);
  if (efd < 0)
  {
    std::cout << "failed to create eventFd" << std::endl;
    ASSERT_THAT(true, false);
  }

  // eventfd
  // 
  // read(2)
  //
  // Each successful read(2) returns an 8-byte integer. A read(2) will fail with
  // the error EINVAL if the size of the supplied buffer is less than 8 bytes.
  //
  //  The semantics of read(2) depend on whether the eventfd counter currently
  //  has a nonzero value and whether the EFD_SEMAPHORE flag was specified when
  //  creating the eventfd file descriptor:
  //
  //  * If EFD_SEMAPHORE was not specified and the eventfd counter has a nonzero
  //  value, then a read(2) returns 8 bytes containing that value, and the
  //  counter's value is "reset to zero."
  //
  //  *  If the eventfd counter is zero at the time of the call to read(2), then
  //  the call either *blocks until* the counter becomes nonzero (at which time,
  //  the read(2) proceeds as described  above)  or  fails  with  the  error
  //  EAGAIN if the file descriptor has been made nonblocking.


  auto f1 = std::async(std::launch::async, [&] 
      {
        uint64_t read_value{};
        if (sizeof(uint64_t) != TEMP_FAILURE_RETRY(read(efd, &read_value, sizeof(uint64_t))))
        {
          std::cout << "errno: " << errno << ", failed on read(eventfd)" << std:: endl;
        }

        result = "READ";
        std::cout << "f1 writes " << result << std::endl;

        return true;
      });

  auto f2 = std::async(std::launch::async, [&] 
      {
        this_thread::sleep_for(chrono::milliseconds(3000));

        uint64_t write_value{100};
        if (sizeof(uint64_t) != TEMP_FAILURE_RETRY(write(efd, &write_value, sizeof(uint64_t))))
        {
          std::cout << "errno: " << errno << ", failed on write(eventfd)" << std:: endl;
        }

        // note: WHY result is not "READ-WRITE"?
        result += "-WRITE";
        std::cout << "f2 writes " << result << std::endl;

        return true;
      });

  f1.get();
  f2.get();

  // EXPECT_THAT(result, "READ-WRITE");
}


// ={=========================================================================
// cxx-condition cxx-queue

namespace cxx_condition
{
  struct ConditionWait
  {
    std::queue<int> q;
    std::mutex m;
    std::condition_variable cv;
    bool thread_running{true};
  };

  void push_items_and_notify(ConditionWait &cw)
  {
    // expect nothing happens since there is no item in the q since q is empty
    // but thread_running is true so wait() do not exit
    {
      // wait to make sure consumer is ready
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));

      // signal
      cw.cv.notify_one();
    }

    // push some and signal
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));

      // okay, put some items
      for (int i = 0; i < 5; i++)
        cw.q.push(i);

      // signal again
      cw.cv.notify_one();
    }

    // push some and signal to end
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));

      // okay, put some items
      for (int i = 0; i < 5; i++)
        cw.q.push(i);

      cw.cv.notify_one();

      cw.thread_running = false;
    }
  }
} // namesapce

// to see how wait() works

TEST(CConCondition, Wait)
{
  using namespace cxx_condition;

  struct ConditionWait cw;

  std::vector<int> coll{};

  std::thread t([&]{push_items_and_notify(cw);});

  std::unique_lock<std::mutex> lock(cw.m);

  while (cw.thread_running)
  {
    // return from wait when 
    // 1. notified, and q is not empty or 
    // 2. notified, q is empty, and thread_running is false

    cw.cv.wait(lock, [&]{ return !cw.q.empty() || !cw.thread_running; });
    if (!cw.q.empty())
    {
      auto item = cw.q.front(); cw.q.pop();
      // std::cout << "poped : " << item << std::endl;
      coll.push_back(item);
    }
    else
    {
      std::cout << "woke up but q is empty" << std::endl;
      coll.push_back(1000);
    }
  }

  t.join();

  EXPECT_THAT(cw.q.size(), 4);
  EXPECT_THAT(coll, ElementsAre(0,1,2,3,4,0));
}


// interface:
//
// class threadsafe_queue 
// {
//  void push(T &);
//  T pop();
// };

namespace cxx_condition
{
  template <typename T>
    class threadsafe_queue 
    {
      public:
        explicit threadsafe_queue() noexcept {}
        threadsafe_queue(threadsafe_queue const&) = delete;
        threadsafe_queue& operator=(threadsafe_queue const&) = delete;

        void push(T const& item)
        {
          std::lock_guard<std::mutex> lock(m_);

          queue_.push(item);
          cond_.notify_one();
        }

        void wait_and_pop(T& value) 
        {
          // to use with cxx-condition
          std::unique_lock<std::mutex> lock(m_);

          cond_.wait(lock, [&]{return !queue_.empty();});

          value = queue_.front();
          queue_.pop();
        }

        bool empty() const
        {
          std::lock_guard<std::mutex> lock(m_);
          return queue_.empty();
        }

      private:
        std::queue<T> queue_;
        mutable std::mutex m_;
        std::condition_variable cond_;
    };

  std::mutex consumed_mtx;
  int consumed_total{};

  void producer(threadsafe_queue<int>& q)
  {
    for (int i = 0; i < 20; ++i)
    {
      q.push(i);
    }
  }

  void consumer(threadsafe_queue<int>& q)
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

  // make consumer can consume more than 20 if it can.
  void consumer_error(threadsafe_queue<int>& q)
  {
    std::unique_lock<std::mutex> lock(consumed_mtx, std::defer_lock);
  
    int value{};
  
    while (true)
    {
      if (consumed_total >= 20*3)
        break;
  
      q.wait_and_pop(value);
  
      lock.lock();
      ++consumed_total;
      lock.unlock();
    }
  }

} // namesapce

// similar to linux-sync-cond-lpi-example
// 3 producers and consumers which produces and consumes 20 items each

TEST(CConCondition, ProducerAndConsumer)
{
  using namespace cxx_condition;

  threadsafe_queue<int> q;

  consumed_total = 0;

  std::vector<std::thread> consumers;
  std::vector<std::thread> producers;
  
  for (int i = 0; i < 3; ++i)
  {
    consumers.emplace_back(consumer, std::ref(q));
  }

  this_thread::sleep_for(chrono::seconds(2));

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

// *cxx-error-race* sometimes works and sometimes not
// hang is caused since consumers waiting and not finished. there are more
// consumers and many do not wake up from wait since there will be no more
// notify once they are used up by other consumer.
//
// not easy to see when 5 consumer and see even when 3 consumers

TEST(DISABLED_CConCondition, ProducerAndConsumerHangError)
// TEST(CConCondition, ProducerAndConsumerHangError)
{
  using namespace cxx_condition;
 
  threadsafe_queue<int> q;

  consumed_total = 0;
 
  std::thread c1(consumer_error, std::ref(q));
  std::thread c2(consumer_error, std::ref(q));
  std::thread c3(consumer_error, std::ref(q));
  // std::thread c4(consumer_error, std::ref(q));
  // std::thread c5(consumer_error, std::ref(q));
 
  this_thread::sleep_for(chrono::seconds(2));
 
  std::thread p1(&producer, std::ref(q));
  std::thread p2(&producer, std::ref(q));
  std::thread p3(&producer, std::ref(q));
 
  c1.join(); c2.join(); c3.join(); 
  // c4.join(); c5.join();
  p1.join(); p2.join(); p3.join();
 
  EXPECT_THAT(consumed_total, 60);
}

namespace cxx_condition_notify_all
{
  // do notify_all() makes difference?

  template <typename T>
    class threadsafe_queue
    {
      public:
        explicit threadsafe_queue() noexcept {}
        threadsafe_queue(threadsafe_queue const&) = delete;
        threadsafe_queue& operator=(threadsafe_queue const&) = delete;

        void push(T const& item)
        {
          std::lock_guard<std::mutex> lock(m_);

          queue_.push(item);
          // cond_.notify_one();
          cond_.notify_all();
        }

        void wait_and_pop(T& value) 
        {
          // to use with cxx-condition
          std::unique_lock<std::mutex> lock(m_);

          cond_.wait(lock, [&]{return !queue_.empty();});

          value = queue_.front();
          queue_.pop();
        }

        bool empty() const
        {
          std::lock_guard<std::mutex> lock(m_);
          return queue_.empty();
        }

      private:
        std::queue<T> queue_;
        mutable std::mutex m_;
        std::condition_variable cond_;
    };

  std::mutex consumed_mtx;
  int consumed_total{};

  void producer(threadsafe_queue<int>& q)
  {
    for (int i = 0; i < 20; ++i)
    {
      q.push(i);
    }
  }

  void consumer(threadsafe_queue<int>& q)
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
} // namespace

TEST(CConCondition, ProducerAndConsumerUseNotifyAllQueue)
{
  using namespace cxx_condition_notify_all;

  threadsafe_queue<int> q;

  consumed_total = 0;

  std::vector<std::thread> consumers;
  std::vector<std::thread> producers;
  
  for (int i = 0; i < 3; ++i)
  {
    consumers.emplace_back(consumer, std::ref(q));
  }

  this_thread::sleep_for(chrono::seconds(2));

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


// 18.6.3 Using Condition Variables to Implement a Queue for Multiple Threads
// Suffers from the same as Error1 as more consumers with no ways to finish all
// consumers.

namespace cxx_condition_error
{
  std::queue<int> queue;
  std::mutex queueMutex;
  std::condition_variable queueCondVar;

  void provider (int val)
  {
    // push different values (val til val+5 with timeouts of val milliseconds
    // into the queue

    for (int i=0; i<6; ++i) {
      {
        std::lock_guard<std::mutex> lg(queueMutex);
        queue.push(val+i);
      } // release lock
      queueCondVar.notify_one();
      std::this_thread::sleep_for(std::chrono::milliseconds(val));
    }
  }
  void consumer (int num)
  {
    // pop values if available (num identifies the consumer)
    while (true) {
      int val;
      {
        std::unique_lock<std::mutex> ul(queueMutex);
        queueCondVar.wait(ul,[]{ return !queue.empty(); });
        val = queue.front();
        queue.pop();
      } // release lock
      std::cout << "consumer " << num << ": " << val << std::endl;
    }
  }
} // namespace

TEST(DISABLED_CConCondition, ProducerAndConsumerError2)
{
  using namespace cxx_condition_error;

  // start three providers for values 100+, 300+, and 500+
  auto p1 = std::async(std::launch::async,provider,100);
  auto p2 = std::async(std::launch::async,provider,300);
  auto p3 = std::async(std::launch::async,provider,500);

  // start two consumers printing the values
  auto c1 = std::async(std::launch::async,consumer,1);
  auto c2 = std::async(std::launch::async,consumer,2);
}


// when 
//
// void consumer(locked_queue<int>* q);
//
// TEST(CConCondition, NotCopyable)
// {
//   using namespace cxx_condition;
// 
//   locked_queue<int> q;
// 
//   // *cxx-error*
//   // ccon_ex.cpp:89:30:   required from here
//   // /usr/include/c++/4.9/tuple:142:42: error: use of deleted function ‘cxx_condition::locked_queue<int>::locked_queue(cxx_condition::locked_queue<int>&&)’
//   //   : _M_head_impl(std::forward<_UHead>(__h)) { }
//   //                                           ^
//   // ccon_ex.cpp:31:11: note: ‘cxx_condition::locked_queue<int>::locked_queue(cxx_condition::locked_queue<int>&&)’ is implicitly deleted because the default definition would be ill-formed:
//   //      class locked_queue {
//   //            ^
//   // ccon_ex.cpp:31:11: error: use of deleted function ‘std::mutex::mutex(const std::mutex&)’
//   // In file included from /usr/include/c++/4.9/future:39:0,
//   //                  from ccon_ex.cpp:3:
//   // /usr/include/c++/4.9/mutex:129:5: note: declared here
//   //      mutex(const mutex&) = delete;
//   //      ^
//   // ...
// 
//   std::thread c1(&consumer, q);
// }


TEST(CConCondition, WaitFor)
{
  std::mutex m;
  std::unique_lock<std::mutex> lock(m);
  std::condition_variable cond;
  std::queue<int> q;
  
  EXPECT_THAT(q.empty(), true);

  cond.wait_for(lock, std::chrono::seconds(1),
      [&q](){ return !q.empty(); });

  // okay, it's done.
  EXPECT_THAT(true, true);
}


// from CPP challenge which solves the issue when there are multiple consumers.
//
// 66. Customer service system
// 
// Write a program that simulates the way customers are served in an office. The
// office has three desks where customers can be served at the same time. Customers
// can enter the office at any time. They take a ticket with a service number from
// a ticketing machine and wait until their number is next for service at one of
// the desks. Customers are served in the order they entered the office, or more
// precisely, in the order given by their ticket. Every time a service desk
// finishes serving a customer, the next customer in order is served. The
// simulation should stop after a particular number of customers have been issued
// tickets and served.

namespace U66_Text
{
  class logger
  {
    public:
      static logger& instance()
      {
        static logger lg;
        return lg;
      }

      logger(logger const&) = delete;
      logger& operator=(logger const&) = delete;

      void log(std::string message)
      {
        (void) message;
        // std::lock_guard<std::mutex> lock(mt);
        // std::cout << "LOG: " << message << std::endl;
      }

    protected:
      logger() {}

    private:
      std::mutex mt;
  };

  class ticketing_machine
  {
    public:
      ticketing_machine(int const start)
        : first_ticket(start), last_ticket(start) {}

      int next() { return last_ticket++; }
      int last() const { return last_ticket -1; }
      void reset() { last_ticket = first_ticket; }

    private:
      int first_ticket;
      int last_ticket;
  };

  class customer
  {
    friend bool operator<(customer const& l, customer const& r);

    public:
    customer(int const no) 
      : number(no) {}

    int ticket_number() const noexcept { return number; }

    private:
    int number;
  };

  bool operator<(customer const& l, customer const& r)
  { 
    return l.number > r.number;
  }

} // namespace

TEST(CConCondition, U66_Text)
{
  using namespace U66_Text;

  std::priority_queue<customer> customers;
  bool office_open{true};
  std::mutex mt;
  std::condition_variable cv;
  int consumed{};

  // consumers

  std::vector<std::thread> desks;

  for (int i = 1; i <= 3; ++i)
  {
    desks.emplace_back([i, &office_open, &mt, &cv, &customers, &consumed]()
        {
          // *cxx-randowm*
          std::default_random_engine dre;
          std::uniform_int_distribution<unsigned> udist(2000, 3000);

          logger::instance().log("desk " + std::to_string(i) + " open");

          while (office_open || !customers.empty())
          {
            std::unique_lock<std::mutex> locker(mt);

            cv.wait_for(locker, std::chrono::seconds(1),
                [&customers](){ return !customers.empty(); });

            if (!customers.empty())
            {
              // not front()?
              auto const c = customers.top();
              customers.pop();

              logger::instance().log("[-] desk " + std::to_string(i) 
                  + " handling customer " + std::to_string(c.ticket_number())
                  + " queue size: " + std::to_string(customers.size())
                  );

              ++consumed;

              locker.unlock();

              // okay without this:
              // cv.notify_one();

              std::this_thread::sleep_for(std::chrono::milliseconds(udist(dre)));

              // logger::instance().log("[ ] desk " 
              //     + std::to_string(i) + " done with customer " 
              //     + std::to_string(c.ticket_number()));
            }
          }

          logger::instance().log("desk " + std::to_string(i) + " closed");

        });
  } // end for


  // single producer

  std::thread store([&]()
      {
        ticketing_machine tm(100);

        // *cxx-randowm*
        std::default_random_engine dre;
        std::uniform_int_distribution<unsigned> udist(200, 300);

        for (int i = 1; i <= 25; ++i)
        {
          customer c(tm.next());
          customers.push(c);

          logger::instance().log(
              "[+] new customer with ticket " + std::to_string(c.ticket_number())
              + "[=] queue size : " + std::to_string(customers.size())
              );

          cv.notify_one();

          std::this_thread::sleep_for(std::chrono::milliseconds(udist(dre)));
        }

        office_open = false;
      });

  store.join();

  for (auto& desk : desks)
    desk.join();

  EXPECT_THAT(consumed, 25);
}


// ={=========================================================================
// cxx-race cxx-stack-threadsafe-stack

// Since lock() do not cover the whole use of `start` variable, while condition
// is not guranteed to work as expected.

namespace cxx_ccon
{
  void do_something(std::string const name, int& start, int& sum, std::mutex& m)
  {
    (void)name;

    while (start < 10)
    {
      std::lock_guard<std::mutex> lock(m);
      this_thread::sleep_for(chrono::milliseconds(30));
      // cout << name << ", start: " << start << ", sum: " << sum << endl;
      sum += start;
      ++start;
    }
  }

} // namespace

TEST(CConRace, NotProtectWhole)
{
  using namespace cxx_ccon;

  {
    std::mutex mx;
    int sum{0};
    int start{1};

    std::thread t1([&]{ do_something("t1", start, sum, mx); });
    std::thread t2([&]{ do_something("t2", start, sum, mx); });
    std::thread t3([&]{ do_something("t3", start, sum, mx); });

    t1.join(); t2.join(); t3.join();

    EXPECT_THAT(sum, Ne(45));
  }

  {
    std::mutex mx;
    int sum{0};
    int start{1};

    std::thread t1([&start, &sum, &mx]{ do_something("t1", start, sum, mx); });
    std::thread t2([&start, &sum, &mx]{ do_something("t2", start, sum, mx); });
    std::thread t3([&start, &sum, &mx]{ do_something("t3", start, sum, mx); });

    t1.join(); t2.join(); t3.join();

    EXPECT_THAT(sum, Ne(45));
  }

  {
    std::mutex mx;
    int sum{0};
    int start{1};

    // OK
    std::thread t1([&]{ 
        do_something("t1", std::ref(start), std::ref(sum), std::ref(mx)); });
    std::thread t2([&]{ 
        do_something("t2", std::ref(start), std::ref(sum), std::ref(mx)); });
    std::thread t3([&]{ 
        do_something("t3", std::ref(start), std::ref(sum), std::ref(mx)); });

    t1.join(); t2.join(); t3.join();

    EXPECT_THAT(sum, Ne(45));
  }
}


namespace cxx_ccon
{
  struct empty_stack : std::exception
  {
    const char *what() const noexcept
    {
      return "empty_stack exception";
    }
  };

  template<typename T>
    class threadsafe_stack
    {
      public:
        threadsafe_stack() {}
        threadsafe_stack(const threadsafe_stack &other)
        {
          std::lock_guard<mutex> lock(other.m);
          data = other.data;
        }

        threadsafe_stack &operator=(const threadsafe_stack &) = delete;

        void push(T value)
        {
          std::lock_guard<mutex> lock(m);
          data.push(value);
        }

        // option3, if make_shared() throes, data is not modified.
        std::shared_ptr<T> pop()
        {
          std::lock_guard<mutex> lock(m);
          if (data.empty())
            throw empty_stack();

          std::shared_ptr<T> const res(std::make_shared<T>(data.top()));
          data.pop();
          return res;
        }

        // option1, use reference
        void pop(T &value)
        {
          std::lock_guard<mutex> lock(m);
          if (data.empty())
            throw empty_stack();

          value = data.top();
          data.pop();
        }

        bool empty() const
        {
          std::lock_guard<mutex> lock(m);
          return data.empty();
        }

      private:
        std::stack<T> data;

        // *cxx-mutable*
        mutable std::mutex m;
    };
} // namesapce

TEST(CConRace, ThreadSafeStack)
{
  using namespace cxx_ccon;

  threadsafe_stack<int> tss;

  // tss.push(1);
  // tss.push(2);
  // tss.push(3);
  // tss.push(4);

  tss.push(5);
  auto sp = tss.pop();

  EXPECT_THAT(*sp, 5);

  // this raises an exception
  if (tss.empty())
  {
    int x;
    EXPECT_THROW(tss.pop(x), empty_stack);
  }
}


// ={=========================================================================
// cxx-future

TEST(CConFuture, SetPromise)
{

  // (not true)
  // If you destroy the std::promise without setting a value, an exception is
  // stored instead.

  // this blocks forever
  // {
  //   std::promise<int> prom;
  //   auto f = prom.get_future();
  //   auto value = f.get();
  // }

  {
    std::promise<int> prom;
    auto f = prom.get_future();
    prom.set_value(10);
    auto value = f.get();
    EXPECT_THAT(value, 10);
  }


  // there are 'no' copy operations for a promise. A set function throws
  // future_error if a value or exception is already set.
  // C++ exception with description 
  // "std::future_error: Promise already satisfied" thrown in the test body.

  {
    std::promise<int> prom;
    auto f = prom.get_future();
    prom.set_value(10);
    EXPECT_THROW(prom.set_value(10), std::exception);
  }
}


namespace cxx_future
{
  int ff(int i)
  {
    if (i) return i;
    throw runtime_error("called as ff(0)");
  }
} // namespace

// The 'point' is that the packaged_task version works exactly like the version
// using 'ordinary' function calls even when the calls of the task (here ff) and
// the calls of the get()s are in 'different' 'threads'. We can concentrate on
// specifying the tasks 'rather' than thinking about threads and locks.
// 
// We can move the future, the packaged_task, or both around. Eventually, the
// packaged_task is invoked and its task deposits its result in the future
// without having to know either which thread executed it or which thread will
// receive the result. This is simple and general.
// 
// The packaged_tasks are actually easier for the server to use than ordinary
// functions because the handling of their exceptions has been taken care of.

TEST(CConFuture, PackagedTaskPoint)
{
  using namespace cxx_future;

  {
    std::packaged_task<int(int)> pt1{ff};
    std::packaged_task<int(int)> pt2{ff};

    // call pt
    pt1(1);
    pt2(0);   // this would cause exception

    // get futures
    auto f1 = pt1.get_future();
    EXPECT_THAT(f1.get(), 1);

    auto f2 = pt2.get_future();
    EXPECT_THROW(f2.get(), runtime_error);
  }
}


// cxx-sort-quick
// CCon, 4.4.1 Functional programming with futures
// Listing 4.12 A sequential implementation of Quicksort
// use the first than the middle

template<typename T>
std::list<T> sequential_quick_sort(std::list<T> input)
{
  if(input.empty())
    return input;

  std::list<T> result;

  // move the first of input to the first of result
  result.splice(result.begin(), input, input.begin());
  const T &pivot = *result.begin();

  // divide input into; 
  // one which are < pivot, pivot, >= pivot.

  auto divide_point = std::partition(input.begin(), input.end(),
      [&](const T &t)
      {
      return t < pivot;
      });

  // make two list; lower_part and input(higher_part)
  std::list<T> lower_part;
  lower_part.splice(lower_part.end(), input, input.begin(), divide_point);

  auto new_lower(sequential_quick_sort(std::move(lower_part)));
  auto new_higher(sequential_quick_sort(std::move(input)));

  // make coll: < p, p, p <=
  result.splice(result.begin(), new_lower);
  result.splice(result.end(), new_higher);
  return result;
}

TEST(CConFuture, SequentialQuickSort)
{
  std::list<int> input{30, 2, 31, 5, 33, 6, 12, 10, 13, 15, 17, 29, 6};

  auto result = sequential_quick_sort(input);
  EXPECT_THAT(result,
      ElementsAreArray({2,5,6,6,10,12,13,15,17,29,30,31,33}));
}

template<typename T>
std::list<T> parallel_quick_sort(std::list<T> input)
{
  if(input.empty())
    return input;

  std::list<T> result;
  // move the first of input to the first of result
  result.splice(result.begin(), input, input.begin());
  const T &pivot = *result.begin();

  // divide input into two; one which are < pivot and the other which are >= pivot.
  auto divide_point = std::partition(input.begin(), input.end(),
      [&](const T &t)
      {
      return t < pivot;
      });

  // make two list; lower_part and input(higher_part)
  std::list<T> lower_part;
  lower_part.splice(lower_part.end(), input, input.begin(), divide_point);

  // only one more since can use main thread
  std::future<std::list<T>> new_lower(
      std::async(parallel_quick_sort<T>, std::move(lower_part))
      );
  auto new_higher(parallel_quick_sort(std::move(input)));

  result.splice(result.begin(), new_lower.get());
  result.splice(result.end(), new_higher);
  return result;
}

TEST(CConFuture, ParallelQuickSort)
{
  std::list<int> input{30, 2, 31, 5, 33, 6, 12, 10, 13, 15, 17, 29, 6};

  auto result = parallel_quick_sort(input);
  EXPECT_THAT(result,
      ElementsAreArray({2,5,6,6,10,12,13,15,17,29,30,31,33}));
}


// ={=========================================================================
// cxx-threadsafe-lookup-table, listing_6.11.cpp

// * each bucket is protected by shared mutex
//
// * If you again use a mutex that supports multiple readers or a single writer,
// you increase the opportunities for concurrency N-fold, where N is the number
// of buckets.

template<typename Key, typename Value, typename Hash = std::hash<Key>>
class threadsafe_lookup_table
{
  public:
    threadsafe_lookup_table(
        unsigned num_buckets = 19, Hash const &hasher = Hash()):
        buckets_(num_buckets), hasher_(hasher)
    {
      for (size_t i = 0; i < num_buckets; ++i)
      {
        buckets_[i].reset(new bucket_type);
      }
    }

    // off copy controls
    threadsafe_lookup_table(const threadsafe_lookup_table &other) = delete;
    threadsafe_lookup_table &operator=(const threadsafe_lookup_table &other) = delete;

    Value value_for(const Key &key, const Value &default_value = Value()) const
    {
      return get_bucket(key).value_for(key, default_value);
    }

    void add_or_update_mapping(const Key &key, const Value &value)
    {
      get_bucket(key).add_or_update_mapping(key, value);
    }

    void remove_mapping(const Key &key)
    {
      get_bucket(key).remove_mapping(key);
    }

  private:
    class bucket_type
    {
      public:
#ifdef SUPPORT_CONST
        Value value_for(const Key &key, const Value &default_value) const
#else
        Value value_for(const Key &key, const Value &default_value)
#endif
        {
          // read on shared mutex
          boost::shared_lock<boost::shared_mutex> lock(m);

          auto found_entry = find_entry_for(key);
          return found_entry == data.end() ? default_value : found_entry->second;
        }

        void add_or_update_mapping(const Key &key, const Value &value)
        {
          // write on shared mutex
          boost::unique_lock<boost::shared_mutex> lock(m);

          auto found_entry = find_entry_for(key);

#ifdef SUPPORT_CONST
          if (found_entry == data.end())
            data.push_back(bucket_value(key, value));
          // else
          //   found_entry->second = value;
#else // SUPPORT_CONST
          if (found_entry == data.end())
            data.push_back(bucket_value(key, value));
          else
            found_entry->second = value;
#endif
        }

        void remove_mapping(const Key &key, const Value &value)
        {
          // write on shared mutex
          boost::unique_lock<boost::shared_mutex> lock(m);

          auto found_entry = find_entry_for(key);
          if (found_entry != data.end())
            data.erase(found_entry);
        }

      private:
        using bucket_value = std::pair<Key, Value>;
        std::list<bucket_value> data;
        mutable boost::shared_mutex m;

#ifdef SUPPORT_CONST
        // okay
        using bucket_iterator = typename std::list<bucket_value>::const_iterator;

        bucket_iterator find_entry_for(const Key &key) const
        {
          return std::find_if(data.begin(), data.end(),
              // [&](const bucket_value &item) {
              // [&](const bucket_value &item) {
              [&](const bucket_value &item) {
              return item.first == key;
              });
        }
#else
        using bucket_iterator = typename std::list<bucket_value>::iterator;

        bucket_iterator find_entry_for(const Key &key)
        {
          return std::find_if(data.begin(), data.end(),
              [&](const bucket_value &item) {
              return item.first == key;
              });
        }
#endif

    };

    std::vector<std::unique_ptr<bucket_type>> buckets_;
    Hash hasher_;

    bucket_type &get_bucket(const Key &key) const
    {
      std::size_t bucket_index = hasher_(key) % buckets_.size();
      return *buckets_[bucket_index];
    }
};

// [ RUN      ] CconThreadTest.UseThreadSafeLookupTable
// kit:vector(n)
// searched value : 300
// [       OK ] CconThreadTest.UseThreadSafeLookupTable (0 ms)

TEST(CconThreadTest, UseThreadSafeLookupTable)
{
  threadsafe_lookup_table<string, int> tslt;

  tslt.add_or_update_mapping("one", 100);
  tslt.add_or_update_mapping("two", 200);
  tslt.add_or_update_mapping("three", 300);
  tslt.add_or_update_mapping("four", 400);

  cout << "searched value : " << tslt.value_for("three") << endl;
}


// ={=========================================================================
// cxx-atomic
// CXXCCON, Listing 5.4 Sequential consistency implies a total ordering

namespace cxx_atomic
{
  namespace use_atomic
  {
    std::atomic<bool> x, y;
    std::atomic<int> z;

    void write_x()
    {
      // 1
      x.store(true, std::memory_order_seq_cst);
    }

    void write_y()
    {
      // 2
      y.store(true, std::memory_order_seq_cst);
    }

    void read_x_then_y()
    {
      while(!x.load(std::memory_order_seq_cst))
        ;

      // 3
      if(y.load(std::memory_order_seq_cst))
        ++z;
    }

    void read_y_then_x()
    {
      while(!y.load(std::memory_order_seq_cst))
        ;

      // 4
      if(x.load(std::memory_order_seq_cst))
        ++z;
    }
  } // namespace

  namespace not_use_atomic
  {
    bool x, y;
    int z;

    void write_x()
    {
      // x.store(true, std::memory_order_seq_cst);
      x = true;
    }

    void write_y()
    {
      // y.store(true, std::memory_order_seq_cst);
      y = true;
    }

    void read_x_then_y()
    {
      while(!x)
        ;

      if(y) ++z;
    }

    void read_y_then_x()
    {
      while(!y)
        ;

      if(x) ++z;
    }
  } // namespace

} // namespace

// Both loads can return true, leading to z being 2, but under no circumstances
// can z be zero.
//
// Not sure if this example is good to study atomic and to represent the point
// since both shows 1 or 2 output for z when runs over and over.

// [ RUN      ] CConAtomic.Atomic
// z: 2
// [       OK ] CConAtomic.Atomic (1 ms)
// 
// [ RUN      ] CConAtomic.Atomic
// z: 1
// [       OK ] CConAtomic.Atomic (0 ms)


TEST(CConAtomic, Atomic)
{
  using namespace cxx_atomic::use_atomic;

  x = false;
  y = false;
  z = 0;

  // either the store to x or the store to y must happen first, even though it’s
  // not specified which.

  std::thread a(write_x);
  std::thread b(write_y);
  std::thread c(read_x_then_y);
  std::thread d(read_y_then_x);

  a.join(); b.join(); c.join(); d.join();

  cout << "z: " << z << endl; 

  assert(z.load()!=0);
}

TEST(CConAtomic, NoAtomic)
{
  using namespace cxx_atomic::not_use_atomic;

  x = false;
  y = false;
  z = 0;

  // either the store to x or the store to y must happen first, even though it’s
  // not specified which.

  std::thread a(write_x);
  std::thread b(write_y);
  std::thread c(read_x_then_y);
  std::thread d(read_y_then_x);

  a.join();
  b.join();
  c.join();
  d.join();

  cout << "z: " << z << endl; 

  assert(z!=0);
}


// https://baptiste-wicht.com/posts/2012/07/c11-concurrency-tutorial-part-4-atomic-type.html
//
// 2. Not much difference in time to run for Atomic and LockCounter

namespace cxx_atomic
{
  struct Counter {

    Counter() : value(0) {}

    int value;

    void increment(){
      ++value;
    }

    void decrement(){
      --value;
    }

    int get(){
      return value;
    }
  };

  void increment_counter(Counter& counter)
  {
    for (int i = 0; i < 30; ++i)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      counter.increment();
    }
  }

  struct AtomicCounter {

    AtomicCounter() : value(0) {}

    // *cxx-atomic*
    // std::atomic<int> value;
    // same as:
    std::atomic_int value;

    void increment(){
      ++value;
    }

    void decrement(){
      --value;
    }

    // *cxx-atomic* Q: show the same result with/without load(). same??
    int get(){
      // return value.load();
      return value;
    }
  };

  void increment_atomic_counter(AtomicCounter& counter)
  {
    for (int i = 0; i < 30; ++i)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      counter.increment();
    }
  }

  struct LockCounter {

    std::mutex m_;

    LockCounter() : value(0) {}

    std::atomic<int> value;

    void increment(){
      std::lock_guard<std::mutex> lock(m_);
      ++value;
    }

    void decrement(){
      std::lock_guard<std::mutex> lock(m_);
      --value;
    }

    int get(){
      std::lock_guard<std::mutex> lock(m_);
      return value.load();
    }
  };

  void increment_lock_counter(LockCounter& counter)
  {
    for (int i = 0; i < 30; ++i)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      counter.increment();
    }
  }

} // namespace


TEST(CConAtomic, CounterIsAtomic)
{
  // atomic<bool> may not be lock-free depending on implementation/platform.
  std::atomic<bool> abool;
  EXPECT_THAT(abool.is_lock_free(), true);

  std::atomic<int> aint;
  EXPECT_THAT(aint.is_lock_free(), true);

  std::atomic_int value;
  EXPECT_THAT(value.is_lock_free(), true);
}


TEST(CConAtomic, CounterRace)
{
  using namespace cxx_atomic;

  Counter counter;

  // increase 30
  std::thread a(increment_counter, std::ref(counter));
  std::thread b(increment_counter, std::ref(counter));
  std::thread c(increment_counter, std::ref(counter));
  std::thread d(increment_counter, std::ref(counter));

  a.join(); b.join(); c.join(); d.join();
  
  cout << "counter value: " << counter.get() << endl;
  // EXPECT_THAT(counter.get(), 120);
}

TEST(CConAtomic, CounterAtomic)
{
  using namespace cxx_atomic;

  AtomicCounter counter;

  std::thread a(increment_atomic_counter, std::ref(counter));
  std::thread b(increment_atomic_counter, std::ref(counter));
  std::thread c(increment_atomic_counter, std::ref(counter));
  std::thread d(increment_atomic_counter, std::ref(counter));

  a.join(); b.join(); c.join(); d.join();
  
  cout << "counter value: " << counter.get() << endl;
  ASSERT_THAT(counter.get(), 120);
}

TEST(CConAtomic, CounterLock)
{
  using namespace cxx_atomic;

  LockCounter counter;

  std::thread a(increment_lock_counter, std::ref(counter));
  std::thread b(increment_lock_counter, std::ref(counter));
  std::thread c(increment_lock_counter, std::ref(counter));
  std::thread d(increment_lock_counter, std::ref(counter));

  a.join(); b.join(); c.join(); d.join();
  
  cout << "counter value: " << counter.get() << endl;
  ASSERT_THAT(counter.get(), 120);
}


// ={=========================================================================

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
