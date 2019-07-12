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

#include "gmock/gmock.h"

// fsm
#include "message.h"

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

TEST(CConThread, StdThread)
{
  using namespace cxx_thread;

  ostringstream os;
  std::thread t([&]{hello(os);});
  t.join();
  EXPECT_THAT(os.str(), "Hello Concurrent World");
}

// thread id: 139805217429248

TEST(CConThread, StdThreadId)
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

namespace cxx_mutex
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

  // void print_no_lock(std::string const& s)
  // {
  //   for (char c : s)
  //   {
  //     this_thread::sleep_for(chrono::milliseconds(20));
  //     std::cout.put(c);
  //   }
  //   std::cout << std::endl;
  // }

} // cxx_mutex


// note that this shows `deadlock` becuase calls lock() twice already before
// running a thread which calls unlock().
//
// TEST(CConLock, MultipleLockAndUnlock)
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

TEST(CConLock, MultipleLockAndUnlock)
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

TEST(CConLock, LockGuard)
{
  using namespace cxx_mutex;

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

  {
    auto f1 = std::async(std::launch::async,
        print_no_lock, "Hello from a first thread");

    auto f2 = std::async(std::launch::async,
        print_no_lock, "Hello from a second thread");

    EXPECT_THAT(f1.get(), 
        Ne("waited for 500ms and Hello from a first thread"));

    EXPECT_THAT(f2.get(), 
        Ne("waited for 520ms and Hello from a second thread"));
  }

  // mangled output when use stdout
  //
  // HHHeeellllllooo   fffrrrooommm   aaa   msfaeiicrnos nttd h trthehrared
  // eaad
  // d
  // 
  // {
  //   using namespace cxx_mutex_no_lock;
  //
  //   auto f1 = std::async(std::launch::async,
  //       print, "Hello from a first thread");
  //
  //   auto f2 = std::async(std::launch::async,
  //       print, "Hello from a second thread");
  //
  //   print("Hello from a main thread");
  // }
}


// ={=========================================================================
// cxx-condition

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
    int value{};

    for (int i = 0; i < 20; ++i)
    {
      q.wait_and_pop(value);
      ++consumed_total;
    }
  }

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

TEST(CConCondition, ProducerAndConsumer)
{
  using namespace cxx_condition;

  threadsafe_queue<int> q;

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

  for (auto &c : consumers)
    c.join();

  for (auto &p : producers)
    p.join();

  EXPECT_THAT(consumed_total, 60);
}

// Error which causes consumers waiting and not finished since there are more
// consumers and many do not wake up from wait since there will be no more
// notify once they are used up by other consumer.

TEST(DISABLED_CConCondition, ProducerAndConsumerError1)
{
  using namespace cxx_condition;
 
  threadsafe_queue<int> q;
 
  std::thread c1(consumer_error, std::ref(q));
  std::thread c2(consumer_error, std::ref(q));
  std::thread c3(consumer_error, std::ref(q));
  std::thread c4(consumer_error, std::ref(q));
  std::thread c5(consumer_error, std::ref(q));
 
  this_thread::sleep_for(chrono::seconds(2));
 
  std::thread p1(&producer, std::ref(q));
  std::thread p2(&producer, std::ref(q));
  std::thread p3(&producer, std::ref(q));
 
  c1.join(); c2.join(); c3.join(); c4.join(); c5.join();
  p1.join(); p2.join(); p3.join();
 
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

  std::string expected{"it's out of wait"};
  EXPECT_THAT(expected, "it's out of wait");
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
// cxx-fsm

namespace cxx_fsm
{
  void push_items_to_queue(messaging::queue& q)
  {
    q.push(100);
  }
}

TEST(CConFsm, Queue)
{
  using namespace cxx_fsm;

  // to avoid conflict with std::queue
  messaging::queue mq;

  std::async(std::launch::async, push_items_to_queue, std::ref(mq));

  auto message = mq.wait_and_pop();

  EXPECT_THAT(dynamic_cast<messaging::wrapped_message<int>*>(message.get())->contents_,
      100);
}

namespace cxx_fsm
{
  // define events/messages

  struct money_inserted
  {
    unsigned amount_;
    explicit money_inserted(unsigned const& amount)
      : amount_(amount) {}
    std::string get_name() { return "money_inserted"; }
  };

  struct item_1_purchased
  {
    unsigned price_;
    explicit item_1_purchased()
      : price_(10) {}
    std::string get_name() { return "item_1_purchased"; }
  };

  struct item_2_purchased
  {
    unsigned price_;
    explicit item_2_purchased()
      : price_(20) {}
    std::string get_name() { return "item_2_purchased"; }
  };

  struct item_3_purchased
  {
    unsigned price_;
    explicit item_3_purchased()
      : price_(30) {}
    std::string get_name() { return "item_3_purchased"; }
  };

  // define eafs of fsm

  class simple
  {
    public:
      simple() {}
      simple(simple const&) = delete;
      simple& operator=(simple const&) = delete;

      // a sender is a wrapper to a message queue, a receiver owns it. You
      // can obtain a sender that references the queue by using the implicit
      // conversion.
      //
      // this is an interface to expose q to other class and used to set up
      // connection each other
      //
      // atm machine(bank.get_sender(), intarface_hardware.get_sender());
      //
      // atm machine keeps this sender copies to send messages to them.

      messaging::sender get_sender()
      { return incoming_; }

      // send a message to self
      void done()
      {
        cout << "simple fsm: send done" << endl;
        get_sender().send(messaging::close_queue());
      }

      void run()
      {
        // init state
        state = &simple::wait_for_money;

        cout << "starts simple fsm" << endl;

        try
        {
          for (;;)
          {
            (this->*state)();
          }
        }
        catch (messaging::close_queue const&)
        {
          cout << "ends simple fsm" << endl;
        }
      }

      // fsm states

      void wait_for_money()
      {
        std::cout << "simple fsm: wait_for_money and waits..." 
          << std::endl; 

        incoming_.wait()
          .handle<money_inserted>
          (
           [&](money_inserted const& message)
           {
              amount_ = message.amount_;
              std::cout << "simple fsm: money_inserted: amount left: " 
              << amount_ << std::endl;

              // set next state
              state = &simple::wait_for_action;
           }
          );
      }

      void wait_for_action()
      {
        std::cout << "simple fsm: wait_for_action and waits..." 
          << std::endl; 

        incoming_.wait()
          .handle<item_1_purchased>
          (
           [&](item_1_purchased const& message)
           {
              amount_ -= message.price_;

              std::cout << "fsm: wait_for_action: item 1 purchased:"
              << " amounts left: " << amount_ << std::endl;

              state = &simple::done_processing;   
           }
          )
          .handle<item_2_purchased>
          (
           [&](item_2_purchased const& message)
           {
              amount_ -= message.price_;

              std::cout << "fsm: wait_for_action: item 2 purchased:"
              << " amounts left: " << amount_ << std::endl;

              state = &simple::done_processing;   
           }
          )
          .handle<item_3_purchased>
          (
           [&](item_3_purchased const& message)
           {
              amount_ -= message.price_;

              std::cout << "fsm: wait_for_action: item 3 purchased:"
              << " amounts left: " << amount_ << std::endl;

              state = &simple::done_processing;   
           }
          );
      }

      void done_processing()
      {
        std::cout << "simple fsm: done_processing and no waits..." 
          << std::endl; 

        state = &simple::wait_for_money;
      }

    private:
      messaging::receiver incoming_;

      // state function pointer
      void (simple::*state)();
      unsigned amount_;
  };

} // namespace

TEST(CConFsm, SimpleFsm1)
{
  using namespace cxx_fsm;

  simple fsm;

  // make thread which runs fsm
  std::thread fsm_thread(&simple::run, &fsm);

  // make sender to send a message

  messaging::sender fsm_queue(fsm.get_sender());

  fsm_queue.send(money_inserted(100));

  std::this_thread::sleep_for(std::chrono::seconds(10));

  fsm.done();
  fsm_thread.join();
}

TEST(CConFsm, SimpleFsm2)
{
  using namespace cxx_fsm;

  simple fsm;

  // make thread which runs fsm
  std::thread fsm_thread(&simple::run, &fsm);

  // make sender to send a message

  messaging::sender fsm_queue(fsm.get_sender());

  fsm_queue.send(money_inserted(100));
  std::this_thread::sleep_for(std::chrono::seconds(2));

  fsm_queue.send(item_2_purchased());
  std::this_thread::sleep_for(std::chrono::seconds(2));

  fsm.done();
  fsm_thread.join();
}

namespace cxx_fsm_atm
{
  // can be a seperate header

  struct withdraw
  {
    std::string account;
    unsigned amount;
    mutable messaging::sender atm_queue;
    withdraw(std::string const& account_,
        unsigned amount_,
        messaging::sender atm_queue_):
      account(account_),amount(amount_),
      atm_queue(atm_queue_)
    {}
  };
  struct withdraw_ok
  {};
  struct withdraw_denied
  {};
  struct cancel_withdrawal
  {
    std::string account;
    unsigned amount;
    cancel_withdrawal(std::string const& account_,
        unsigned amount_):
      account(account_),amount(amount_)
    {}
  };
  struct withdrawal_processed
  {
    std::string account;
    unsigned amount;
    withdrawal_processed(std::string const& account_,
        unsigned amount_):
      account(account_),amount(amount_)
    {}
  };
  struct card_inserted
  {
    std::string account;
    explicit card_inserted(std::string const& account_):
      account(account_)
    {}
  };
  struct digit_pressed
  {
    char digit;
    explicit digit_pressed(char digit_):
      digit(digit_)
    {}
  };
  struct clear_last_pressed
  {};
  struct eject_card
  {};
  struct withdraw_pressed
  {
    unsigned amount;
    explicit withdraw_pressed(unsigned amount_):
      amount(amount_)
    {}
  };
  struct cancel_pressed
  {};
  struct issue_money
  {
    unsigned amount;
    issue_money(unsigned amount_):
      amount(amount_)
    {}
  };
  struct verify_pin
  {
    std::string account;
    std::string pin;
    mutable messaging::sender atm_queue;
    verify_pin(std::string const& account_,std::string const& pin_,
        messaging::sender atm_queue_):
      account(account_),pin(pin_),atm_queue(atm_queue_)
    {}
  };
  struct pin_verified
  {};
  struct pin_incorrect
  {};
  struct display_enter_pin
  {};
  struct display_enter_card
  {};
  struct display_insufficient_funds
  {};
  struct display_withdrawal_cancelled
  {};
  struct display_pin_incorrect_message
  {};
  struct display_withdrawal_options
  {};
  struct get_balance
  {
    std::string account;
    mutable messaging::sender atm_queue;
    get_balance(std::string const& account_,messaging::sender atm_queue_):
      account(account_),atm_queue(atm_queue_)
    {}
  };
  struct balance
  {
    unsigned amount;
    explicit balance(unsigned amount_):
      amount(amount_)
    {}
  };
  struct display_balance
  {
    unsigned amount;
    explicit display_balance(unsigned amount_):
      amount(amount_)
    {}
  };
  struct balance_pressed
  {};


  // listing_c.9.cpp
  class interface_machine
  {
    messaging::receiver incoming;
    std::mutex iom;

    public:
    void done()
    {
      get_sender().send(messaging::close_queue());
    }

    void run()
    {
      try
      {
        for(;;)
        {
          incoming.wait()
            .handle<issue_money>(
                [&](issue_money const& msg)
                {
                (void)msg;
                {
                std::lock_guard<std::mutex> lk(iom);
                std::cout<<"Issuing "
                <<msg.amount<<std::endl;
                }
                }
                )
            .handle<display_insufficient_funds>(
                [&](display_insufficient_funds const& msg)
                {
                (void)msg;
                {
                std::lock_guard<std::mutex> lk(iom);
                std::cout<<"Insufficient funds"<<std::endl;
                }
                }
                )
            .handle<display_enter_pin>(
                [&](display_enter_pin const& msg)
                {
                (void)msg;
                {
                std::lock_guard<std::mutex> lk(iom);
                std::cout
                <<"Please enter your PIN (0-9)"
                <<std::endl;
                }
                }
                )
            .handle<display_enter_card>(
                [&](display_enter_card const& msg)
                {
                (void)msg;
                {
                std::lock_guard<std::mutex> lk(iom);
                std::cout<<"Please enter your card (I)"
                <<std::endl;
                }
                }
                )
            .handle<display_balance>(
                [&](display_balance const& msg)
                {
                (void)msg;
                {
                std::lock_guard<std::mutex> lk(iom);
                std::cout
                <<"The balance of your account is "
                <<msg.amount<<std::endl;
                }
                }
                )
            .handle<display_withdrawal_options>(
                [&](display_withdrawal_options const& msg)
                {
                (void)msg;
                {
                std::lock_guard<std::mutex> lk(iom);
                std::cout<<"Withdraw 50? (w)"<<std::endl;
                std::cout<<"Display Balance? (b)"
                <<std::endl;
                std::cout<<"Cancel? (c)"<<std::endl;
                }
                }
                )
            .handle<display_withdrawal_cancelled>(
                [&](display_withdrawal_cancelled const& msg)
                {
                (void)msg;
                {
                std::lock_guard<std::mutex> lk(iom);
                std::cout<<"Withdrawal cancelled"
                <<std::endl;
                }
                }
                )
            .handle<display_pin_incorrect_message>(
                [&](display_pin_incorrect_message const& msg)
                {
                (void)msg;
                {
                std::lock_guard<std::mutex> lk(iom);
                std::cout<<"PIN incorrect"<<std::endl;
                }
                }
                )
            .handle<eject_card>(
                [&](eject_card const& msg)
                {
                (void)msg;
                {
                std::lock_guard<std::mutex> lk(iom);
                std::cout<<"Ejecting card"<<std::endl;
                }
                }
                );
        }
      }
      catch(messaging::close_queue&)
      {
      }
    }
    messaging::sender get_sender()
    {
      return incoming;
    }
  };


  // listing_c.8.cpp

  class bank_machine
  {
    messaging::receiver incoming;
    unsigned balance_;

    public:
    bank_machine():
      balance_(199)
    {}
    void done()
    {
      get_sender().send(messaging::close_queue());
    }

    void run()
    {
      try
      {
        for(;;)
        {
          incoming.wait()
            .handle<verify_pin>
            (
             [&](verify_pin const& msg)
             {
             if(msg.pin=="1937")
             {
             msg.atm_queue.send(pin_verified());
             }
             else
             {
             msg.atm_queue.send(pin_incorrect());
             }
             }
            )
            .handle<withdraw>
            (
             [&](withdraw const& msg)
             {
             if(balance_>=msg.amount)
             {
             msg.atm_queue.send(withdraw_ok());
             balance_-=msg.amount;
             }
             else
             {
             msg.atm_queue.send(withdraw_denied());
             }
             }
            )
            .handle<get_balance>
            (
             [&](get_balance const& msg)
             {
             msg.atm_queue.send(balance(balance_));
             }
            )
            .handle<withdrawal_processed>
            (
             [&](withdrawal_processed const& msg)
             {
             (void)msg;
             }
            )
            .handle<cancel_withdrawal>
            (
             [&](cancel_withdrawal const& msg)
             {
             (void)msg;
             }
            );
        }
      }
      catch(messaging::close_queue const&)
      {
      }
    }
    messaging::sender get_sender()
    {
      return incoming;
    }
  };


  class atm
  {
    private:
      messaging::receiver incoming_;
      messaging::sender bank_;
      messaging::sender interface_hardware_;

      void (atm::*state)();

      std::string account_;
      std::string pin_;
      unsigned withdraw_amount_;

      void waiting_for_card()
      {
        // interface_hardware.send(display_enter_card());
        //
        // state waitinf_for_card -> event card_inserted -> state getting_pin
        //                              -> eaf, lambda

        incoming_.wait()
          .handle<card_inserted>
          (
           [&](const card_inserted &msg) 
           {
           std::cout << "atm::waiting_for_card::card_inserted" << std::endl;
           account_ = msg.account;
           pin_ = "";
           // interface_hardware_.send(display_enter_pin());
           state = &atm::getting_pin;
           }
          );
      }

      void getting_pin()
      {
        incoming_.wait()
          .handle<digit_pressed>
          (
           [&](const digit_pressed &msg)
           {
           (void)msg;
           unsigned const pin_length = 4;
           pin_ += msg.digit;
           std::cout << "atm::getting_pin::digit_pressed: " << pin_ << std::endl;
           if (pin_.length() == pin_length)
           {
           // bank.send(verify_pin(account, pin, incoming));
           state = &atm::verifying_pin;
           }
           }
          )
          .handle<clear_last_pressed>
          (
           [&](const clear_last_pressed &msg)
           {
           (void)msg;
           std::cout << "atm::getting_pin::clear_last_pressed" << std::endl;
           if (!pin_.empty())
           {
           pin_.pop_back();
           }
           }
          )
          .handle<cancel_pressed>
          (
           [&](const cancel_pressed &msg)
           {
           (void)msg;
           std::cout << "atm::getting_pin::cancel_pressed" << std::endl;
           state = &atm::done_processing;
           }
          );
      }

      // expect pin_* message from back fsm.
      void verifying_pin()
      {
        incoming_.wait()
          .handle<pin_verified>
          (
           [&](const pin_verified &msg)
           {
           (void)msg;
           std::cout << "atm::verify_pin::pin_verified" << std::endl;
           state = &atm::wait_for_action;
           }
          )
          .handle<pin_incorrect>
          (
           [&](const pin_incorrect &msg)
           {
           (void)msg;
           std::cout << "atm::verify_pin::pin_incorrect" << std::endl;
           // interface_hardware_.send(display_pin_incorrect_message());
           state = &atm::done_processing;
           }
          )
          .handle<cancel_pressed>
          (
           [&](const cancel_pressed &msg)
           {
           (void)msg;
           std::cout << "atm::verify_pin::cancel_pressed" << std::endl;
           state = &atm::done_processing;
           }
          );
      }

      void wait_for_action()
      {
        // interface_hardware_.send(display_withdrawal_options());
        incoming_.wait()
          .handle<withdraw_pressed>
          (
           [&](const withdraw_pressed &msg)
           {
           withdraw_amount_ = msg.amount;
           std::cout << "atm::wait_for_action::withdraw_pressed: " << withdraw_amount_ << std::endl;
           // bank_.send(withdraw(account_, msg.account, incoming_));
           state = &atm::process_withdrawal;
           }
          )
          .handle<balance_pressed>
          (
           [&](const balance_pressed &msg)
           {
           (void)msg;
           std::cout << "atm::wait_for_action::balance_pressed" << std::endl;
           // bank_.send(get_balance(account_, incoming_));
           state = &atm::process_balance;
           }
          )
          .handle<cancel_pressed>
          (
           [&](const cancel_pressed &msg)
           {
           (void)msg;
           std::cout << "atm::wait_for_action::cancel_pressed" << std::endl;
           state = &atm::done_processing;
           }
          );
      }

      void process_balance()
      {
        incoming_.wait()
          .handle<balance>
          (
           [&](const balance &msg)
           {
           (void)msg;
           std::cout << "atm::process_balance::balance" << std::endl;
           interface_hardware_.send(display_balance(msg.amount));
           state = &atm::wait_for_action;
           }
          )
          .handle<cancel_pressed>
          (
           [&](const cancel_pressed &msg)
           {
           (void)msg;
           std::cout << "atm::process_balance::cancel_pressed" << std::endl;
           state = &atm::done_processing;
           }
          );
      }

      void process_withdrawal()
      {
        incoming_.wait()
          .handle<withdraw_ok>
          (
           [&](const withdraw_ok &msg)
           {
           (void)msg;
           std::cout << "atm::process_withdrawal::withdraw_ok" << std::endl;
           // interface_hardware_.send(issue_money(withdrawal_amount));
           // bank_.send(withdrawal_processed(account,withdrawal_amount));
           state = &atm::done_processing;
           }
          )
          .handle<withdraw_denied>
          (
           [&](const withdraw_denied &msg)
           {
           (void)msg;
           std::cout << "atm::process_withdrawal::withdraw_denied" << std::endl;
           // interface_hardware_.send(display_insufficient_funds());
           state = &atm::done_processing;
           }
          )
          .handle<cancel_pressed>
          (
           [&](const cancel_pressed &msg)
           {
           (void)msg;
           std::cout << "atm::process_withdrawal::cancel_pressed" << std::endl;
           // bank_.send(cancel_withdrawal(account,withdrawal_amount));
           // interface_hardware_.send(display_withdrawal_cancelled());
           state = &atm::done_processing;
           }
          );
      }

      void done_processing()
      {
        // interface_hardware_.send(eject_card());
        state = &atm::getting_pin;
      }

    public:
      atm(messaging::sender bank, messaging::sender interface_hardware):
        bank_(bank), interface_hardware_(interface_hardware) {}

      atm(const atm &) = delete;
      atm &operator=(const atm &) = delete;

      // send a message to self
      void done()
      {
        get_sender().send(messaging::close_queue());
      }

      void run()
      {
        state = &atm::waiting_for_card;

        std::cout << "atm::run::for" << std::endl;

        try
        {
          for (;;)
          {
            (this->*state)();
          }
        }
        catch(messaging::close_queue const &)
        {
          std::cout << "atm::run::end" << std::endl;
          // note: what would happen when catch exception but do nothing?
        }
      }

      // Whereas a sender just references a message queue, a receiver owns it. You
      // can obtain a sender that references the queue by using the implicit
      // conversion.
      //
      // note: this is an interface to expose q to other class and used to set up
      // connection each other
      //
      // atm machine(bank.get_sender(), intarface_hardware.get_sender());
      //
      // atm machine keeps this sender copies to send messages to them.

      // note: cxx-conversion-op which convert receiver to sender
      messaging::sender get_sender()
      {
        return incoming_;
      }
  };

} // namespace


// [ RUN      ] AtmFsm.CancelPin
// atm::run::for
// atm::waiting_for_card::card_inserted
// atm::getting_pin::digit_pressed: 3
// atm::getting_pin::digit_pressed: 33
// atm::getting_pin::digit_pressed: 330
// atm::getting_pin::cancel_pressed
// [       OK ] AtmFsm.CancelPin (1001 ms)
//
// [ RUN      ] AtmFsm.DoWithdraw
// atm::run::for
// atm::waiting_for_card::card_inserted
// atm::getting_pin::digit_pressed: 3
// atm::getting_pin::digit_pressed: 33
// atm::getting_pin::digit_pressed: 330
// atm::getting_pin::digit_pressed: 3301
// atm::verify_pin::pin_verified
// atm::wait_for_action::withdraw_pressed: 50
// atm::process_withdrawal::withdraw_ok
// [       OK ] AtmFsm.DoWithdraw (1000 ms)

TEST(CConFsm, AtmCancelPin)
{
  using namespace cxx_fsm_atm;

  bank_machine bank;
  interface_machine interface_hardware;
  atm machine(bank.get_sender(),interface_hardware.get_sender());

  std::thread atm_thread(&atm::run, &machine);

  messaging::sender atmqueue(machine.get_sender());

  atmqueue.send(card_inserted("JW1234"));
  atmqueue.send(digit_pressed('3'));
  atmqueue.send(digit_pressed('3'));
  atmqueue.send(digit_pressed('0'));
  atmqueue.send(cancel_pressed());

  std::this_thread::sleep_for(std::chrono::seconds(1));
  machine.done();
  atm_thread.join();
}

TEST(CConFsm, AtmDoWithdraw)
{
  using namespace cxx_fsm_atm;

  bank_machine bank;
  interface_machine interface_hardware;
  atm machine(bank.get_sender(),interface_hardware.get_sender());

  std::thread atm_thread(&atm::run, &machine);

  messaging::sender atmqueue(machine.get_sender());

  atmqueue.send(card_inserted("JW1234"));
  atmqueue.send(digit_pressed('3'));
  atmqueue.send(digit_pressed('3'));
  atmqueue.send(digit_pressed('0'));
  atmqueue.send(digit_pressed('1'));

  // message from bank fsm
  atmqueue.send(pin_verified());

  atmqueue.send(withdraw_pressed(50));
  atmqueue.send(withdraw_ok());

  std::this_thread::sleep_for(std::chrono::seconds(1));
  machine.done();
  atm_thread.join();
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
// Listing 5.4 Sequential consistency implies a total ordering

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

  a.join();
  b.join();
  c.join();
  d.join();

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
// 1. TEST(DISABLED_CConAtomic, NoAtomicCounter)
// only failes when run on real server but not on VM.
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

    std::atomic<int> value;

    void increment(){
      ++value;
    }

    void decrement(){
      --value;
    }

    int get(){
      return value.load();
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

TEST(DISABLED_CConAtomic, NoAtomicCounter)
{
  using namespace cxx_atomic;

  Counter counter;

  std::thread a(increment_counter, std::ref(counter));
  std::thread b(increment_counter, std::ref(counter));
  std::thread c(increment_counter, std::ref(counter));
  std::thread d(increment_counter, std::ref(counter));

  a.join(); b.join(); c.join(); d.join();
  
  cout << "counter value: " << counter.get() << endl;
  ASSERT_THAT(counter.get(), 120);
}

TEST(CConAtomic, AtomicCounter)
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

TEST(CConAtomic, LockCounter)
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


TEST(CConAtomic, AtomicTypes)
{
  // atomic<bool> may not be lock-free depending on implementation/platform.
  std::atomic<bool> abool;
  EXPECT_THAT(abool.is_lock_free(), true);

  std::atomic<int> aint;
  EXPECT_THAT(aint.is_lock_free(), true);
}


// ={=========================================================================

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
