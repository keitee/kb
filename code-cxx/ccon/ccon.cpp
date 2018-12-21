#include <iostream>
#include <thread>
#include <future>
#include <list>
#include <mutex>
#include <queue>
#include <stack>
#include <exception>

#include <boost/thread/shared_mutex.hpp>

#include "gmock/gmock.h"

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

void hello(ostringstream &os)
{
  os << "Hello Concurrent World";
}

TEST(CConThread, OneAndFunction)
{
  ostringstream os;
  std::thread t([&]{hello(os);});
  t.join();
  EXPECT_THAT(os.str(), "Hello Concurrent World");
}

// +..+..+...+..+.+++++
// done

TEST(CConThread, TwoAndLambda)
{
  std::thread t1([]{doSomething('.');});
  std::thread t2([]{doSomething('+');});
  t1.join();
  t2.join();
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

} // namespace

// expect that do_other_stuff() finishes first

TEST(CConAsync, Async)
{
  using namespace cxx_async;

  std::future<int> the_answer = std::async(find_the_answer);
  do_other_stuff();
  EXPECT_THAT(the_answer.get(), 100);
}

namespace cxx_aync_return {

  void use_reference(int &value)
  {
    value += 200;
  }

  void use_value(int value)
  {
    value += 200;
  }

} // namespace

// show how return value from thread via reference

TEST(CConAsync, ReturnViaReference)
{
  using namespace cxx_aync_return;

  // value
  {
    int value{1};

    std::thread t(use_value, value); 
    t.join();

    EXPECT_THAT(value, 1);
  }

  {
    int value{1};

    // In file included from /usr/include/c++/4.9/thread:39:0,
    //                  from ccon.cpp:2:
    // /usr/include/c++/4.9/functional: In instantiation of ‘struct std::_Bind_simple<void (*(int))(int&)>’:
    // /usr/include/c++/4.9/thread:140:47:   required from ‘std::thread::thread(_Callable&&, _Args&& ...) [with _Callable = void (&)(int&); _Args = {int&}]’
    // ccon.cpp:418:36:   required from here
    // /usr/include/c++/4.9/functional:1665:61: error: no type named ‘type’ in ‘class std::result_of<void (*(int))(int&)>’
    //        typedef typename result_of<_Callable(_Args...)>::type result_type;
    //                                                              ^
    // /usr/include/c++/4.9/functional:1695:9: error: no type named ‘type’ in ‘class std::result_of<void (*(int))(int&)>’
    //          _M_invoke(_Index_tuple<_Indices...>)
    //          ^
    // makefile:58: recipe for target 'ccon.o' failed
    // make: *** [ccon.o] Error 1
    //
    // std::thread t(UseReference, value);  

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
// starting func1() in background and func2() in foreground:
// ++++++++++..........
// result of func1()+func2(): 89


TEST(CConAsync, WithDefault)
{
  future<int> result1(std::async([]{return doSomething('.');}));

  int result2 = doSomething('+');

  int result = result1.get() + result2;

  EXPECT_THAT(result, 89);
}


// cxx-async with async option
//
// If async() could successfully start func1(), the output might be something
// like the following:
//
// starting func1() in background and func2() in foreground:
// +..+..+...+..+.+++++
// result of func1()+func2(): 89


// Using Launch Policies

// You can force async() to not defer the passed functionality, by explicitly
// passing a launch policy directing async() that it should definitely start the
// passed functionality asynchronously the moment it is called:

TEST(CConAsync, WithAsync)
{
  future<int> result1(std::async(launch::async, []{return doSomething('.');}));

  int result2 = doSomething('+');

  int result = result1.get() + result2;

  EXPECT_THAT(result, 89);
}

// So, based on this first example, we can define a general way to make a
// program faster: You can modify the program so that it might benefit from
// parallelization, if the underlying platform supports it, but still works as
// before on single-threaded environments.


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


// If you don’t assign the result of std::async(std::launch::async,...)
// anywhere, the caller will block until the passed functionality has finished,
// which would mean that this is nothing but a synchronous call.
// 
// ..........  
// ++++++++++

TEST(CConAsync, WithAsyncButSequential)
{
  // future<int> result1(std::async(launch::async, []{return doSomething('.');}));
  std::async(launch::async, []{return doSomething('.');});

  doSomething('+');
}


// ={=========================================================================
// cxx-async with async option

// starting 2 operations asynchronously
// 
// ..........++++++++++
// done

TEST(CConAsync, UseAsyncX)
{
  cout << "starting 2 operations asynchronously" << endl;

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
      cout << "\nyield" << endl;
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


// ={=========================================================================
// cxx-lock-mutex cxx-mutex

// CXXSLR 18.5 A First Complete Example for Using a Mutex and a Lock

// see the effect of lock
//
// [ RUN      ] CConMutex.LockGuard
// Hello from a main thread
// Hello from a second thread
// Hello from a first thread
// [       OK ] CConMutex.LockGuard (1520 ms)
// [ RUN      ] CConMutex.NoLockGuard
// HHHeeellllllooo   fffrrrooommm   aaa   msfaeiicrnos nttd h trthehrared
// eaad
// d
// [       OK ] CConMutex.NoLockGuard (528 ms)

namespace cxx_mutex
{
  std::mutex print_mutex;

  void print(std::string const& s)
  {
    std::lock_guard<std::mutex> l(print_mutex);

    for (char c : s)
    {
      this_thread::sleep_for(chrono::milliseconds(20));
      std::cout.put(c);
    }
    std::cout << std::endl;
  }
} // cxx_mutex

TEST(CConLock, LockGuard)
{
  using namespace cxx_mutex;

  auto f1 = std::async(std::launch::async,
      print, "Hello from a first thread");

  auto f2 = std::async(std::launch::async,
      print, "Hello from a second thread");

  print("Hello from a main thread");
}

namespace cxx_mutex_no_lock
{
  std::mutex print_mutex;

  void print(std::string const& s)
  {
    // std::lock_guard<std::mutex> l(print_mutex);

    for (char c : s)
    {
      this_thread::sleep_for(chrono::milliseconds(20));
      std::cout.put(c);
    }
    std::cout << std::endl;
  }
} // cxx_mutex

TEST(CConLock, NoLockGuard)
{
  using namespace cxx_mutex_no_lock;

  auto f1 = std::async(std::launch::async,
      print, "Hello from a first thread");

  auto f2 = std::async(std::launch::async,
      print, "Hello from a second thread");

  print("Hello from a main thread");
}

// found that sometimes sum is 45 and not sure why?

namespace cxx_lock
{
  template <typename T>
    class locked_queue
    {
      public:
        // push an item into the end of the queue
        void push(T const& item)
        {
          std::lock_guard<std::mutex> lock(mutex_);
          queue_.push(item);
        }

        size_t size() const noexcept
        {
          return queue_.size();
        }

      private:
        std::queue<T> queue_;
        std::mutex mutex_;
    };

  void do_something(int& start, int& sum, std::mutex& m)
  {
    while (start < 10)
    {
      std::lock_guard<std::mutex> lock(m);
      // this_thread::sleep_for(chrono::milliseconds(dist(engine)));
      cout << ", start: " << start << ", sum: " << sum << endl;
      // cout << msg << ", start: " << start << ", sum: " << sum << endl;
      sum += start;
      ++start;
    }
  }

} // namespace

TEST(CConLock, X)
{
  using namespace cxx_lock;

  locked_queue<int> lq;

  std::mutex mutex_;
  int sum{0};
  int start{1};

  // OK
  // std::thread t1([&]{ do_something("t1", start, sum, mutex_); });
  // std::thread t2([&]{ do_something("t2", start, sum, mutex_); });
  // std::thread t3([&]{ do_something("t3", start, sum, mutex_); });

  // FAIL or OK
  std::thread t1([&]{ do_something(start, sum, mutex_); });
  std::thread t2([&]{ do_something(start, sum, mutex_); });
  std::thread t3([&]{ do_something(start, sum, mutex_); });

  // OK
  // std::thread t1([&]{ do_something(std::ref(start), std::ref(sum), std::ref(mutex_)); });
  // std::thread t2([&]{ do_something(std::ref(start), std::ref(sum), std::ref(mutex_)); });
  // std::thread t3([&]{ do_something(std::ref(start), std::ref(sum), std::ref(mutex_)); });

  t1.join();
  t2.join();
  t3.join();

  EXPECT_THAT(sum, 45);
}


// ={=========================================================================
// cxx-lockedqueue

template <typename T>
class locked_queue
{
  public:
    // push an item into the end of the queue
    void push(T const &item)
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_queue.push(item);
      // queue went from empty to one, notify a thread
      if(m_queue.size() == 1)
      {
        m_notEmptyCondition.notify_one();
      }
    }

    // pop the oldest item out of the queue
    // blocks until there are item to be popped
    T pop()
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      while(m_queue.empty() == true)
        m_notEmptyCondition.wait(lock);

      T item = m_queue.front();
      m_queue.pop();
      return item;
    }

  private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_notEmptyCondition;
};


// producer that pushes two items to a queue
void producer(locked_queue<int> *q)
{
  std::cout << "starts producer" << std::endl;
  q->push(1);
  q->push(2);
  std::cout << "ends producer" << std::endl;
}

// consumer that pops an item from the queue
void consumer(locked_queue<int> *q)
{
  std::cout << "starts consumer" << std::endl;
  int i = q->pop();
  std::cout << "ends consumer: " << i << std::endl;
}

TEST(CConThread, LockedQueue)
{
  locked_queue<int> lq;

  // set up two consumers to wait for items

  std::cout << "main: starts consumers" << std::endl;
  std::thread c1(&consumer, &lq);
  std::thread c2(&consumer, &lq);

  // sleep for a second to ensure c1 and c2 are ready
  // not the best code, but good enough for a test case
  this_thread::sleep_for(chrono::seconds(2));

  // create the producer to push in two items

  std::thread p(&producer, &lq);

  c1.join();
  c2.join();
  p.join();

  std::cout << "main: ends" << std::endl;
}


// ={=========================================================================
// cxx-threadsafe-stack

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

    std::shared_ptr<T> pop()
    {
      std::lock_guard<mutex> lock(m);
      if (data.empty())
        throw empty_stack();

      std::shared_ptr<T> const res(std::make_shared<T>(data.top()));
      data.pop();
      return res;
    }

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
    mutable std::mutex m;
};

// [ RUN      ] CconThreadTest.UseThreadSafeStack
// returned item is 5
// unknown file: Failure
// C++ exception with description "empty_stack exception" thrown in the test body.
// [  FAILED  ] CconThreadTest.UseThreadSafeStack (0 ms)

TEST(CconThreadTest, UseThreadSafeStack)
{
  threadsafe_stack<int> tss;

  // tss.push(1);
  // tss.push(2);
  // tss.push(3);
  // tss.push(4);
  
  tss.push(5);
  auto sp = tss.pop();

  cout << "returned item is " << *sp << endl;

  // this raises an exception
  if (tss.empty())
  {
    int x;
    tss.pop(x);
  }
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
// cxx-quicksort
// Listing 4.12 A sequential implementation of Quicksort

template<typename T>
std::list<T> sequential_quick_sort(std::list<T> input)
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

    auto new_lower(sequential_quick_sort(std::move(lower_part)));
    auto new_higher(sequential_quick_sort(std::move(input)));

    result.splice(result.end(), new_higher);
    result.splice(result.begin(), new_lower);
    return result;
}

TEST(CconThreadTest, UseSequentialQuickSort)
{
    // result {12, 19, 22, 26, 29, 33, 35 };
    // std::list<int> input{26, 33, 35, 29, 19, 12, 22};
    std::list<int> input{30, 2, 31, 5, 33, 6, 12, 10, 13, 15, 17, 29, 6};

    auto result = sequential_quick_sort(input);
    copy(result.begin(), result.end(), ostream_iterator<int>(cout, " "));
    cout << endl;
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

    std::future<std::list<T>> new_lower(
        std::async(parallel_quick_sort<T>, std::move(lower_part))
        );
    auto new_higher(parallel_quick_sort(std::move(input)));

    result.splice(result.end(), new_higher);
    result.splice(result.begin(), new_lower.get());
    return result;
}

TEST(CconThreadTest, UseParallelQuickSort)
{
    // result {12, 19, 22, 26, 29, 33, 35 };
    // std::list<int> input{26, 33, 35, 29, 19, 12, 22};

    std::list<int> input{30, 2, 31, 5, 33, 6, 12, 10, 13, 15, 17, 29, 6};

    auto result = parallel_quick_sort(input);
    copy(result.begin(), result.end(), ostream_iterator<int>(cout, " "));
    cout << endl;
}


// ={=========================================================================
// cxx-atomic
// Listing 5.4 Sequential consistency implies a total ordering

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

// Both loads can return true, leading to z being 2, but under no circumstances
// can z be zero.

// [ RUN      ] CconThreadTest.UseSequentialConsistency
// z: 2
// [       OK ] CconThreadTest.UseSequentialConsistency (8 ms)

TEST(CconThreadTest, UseSequentialConsistency)
{
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


// ={=========================================================================

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
