#include <iostream>
#include <thread>
#include <future>
#include <list>
#include <mutex>
#include <queue>
#include <stack>
#include <exception>

#include <boost/thread/shared_mutex.hpp>

#include "gtest/gtest.h"

using namespace std;

int doSomething (char c)
{
    default_random_engine dre(c);
    uniform_int_distribution<int> id(10, 1000);

    for(int i=0; i<10; ++i)
    {
        this_thread::sleep_for(chrono::milliseconds( id(dre) ));
        cout.put(c).flush();
    }

    return c;
}


// ={=========================================================================
// cxx-thread

void hello()
{
    cout << "Hello Concurrent World\n";
}

TEST(CconThreadTest, HelloWorld)
{
    std::thread t(hello);
    t.join();
}

// +..+..+...+..+.+++++
// done

TEST(CconThreadTest, RunTwoThreads)
{
    std::thread t1([]{doSomething('.');});
    std::thread t2([]{doSomething('+');});
    t1.join();
    t2.join();
    cout << "\ndone" << endl;
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

TEST(CconThreadTest, UseLockedQueue)
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
// cxx-async,
//
// ends do_other_stuff
// ends find_the_answer
// the answer is 100

int find_the_answer()
{
    this_thread::sleep_for(chrono::milliseconds(100));
    std::cout << "ends find_the_answer" << std::endl;
    return 100;
}

void do_other_stuff()
{
    this_thread::sleep_for(chrono::seconds(20));
    std::cout << "ends do_other_stuff" << std::endl;
}

TEST(CconThreadTest, UseAsync)
{
    std::future<int> the_answer = std::async( find_the_answer );
    do_other_stuff();
    std::cout << "the answer is " << the_answer.get() << std::endl;
}

void UseReference(int &value)
{
  std::cout << "input   value: " << value << std::endl;
  value += 200;
  std::cout << "changed value: " << value << std::endl;
}

// [ RUN      ] CconThreadTest.CompareThreadAndAsync
// input   value: 1
// changed value: 201
// final value: 201
// [       OK ] CconThreadTest.CompareThreadAndAsync (1 ms)

TEST(CconThreadTest, UseAsyncWithReference)
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

  std::thread t(UseReference, std::ref(value));  
  t.join();
  std::cout << "final value: " << value << std::endl;
}


// ={=========================================================================
// cxx-async with no option

// starting func1() in background and func2() in foreground:
// ++++++++++..........
// result of func1()+func2(): 89


int func1()
{
    return doSomething('.');
}

int func2()
{
    return doSomething('+');
}

TEST(CconThreadTest, UseAsyncWithDefault)
{
    cout << "starting func1() in background"
        << " and func2() in foreground:" << endl;

    future<int> result1(std::async(func1));

    int result2 = func2();

    int result = result1.get() + result2;

    cout << "\nresult of func1()+func2(): " << result << endl;
}


// ={=========================================================================
// cxx-async with async option

// starting func1() in background and func2() in foreground:
// +..+..+...+..+.+++++
// result of func1()+func2(): 89

TEST(CconThreadTest, UseAsyncWithAsync)
{
    cout << "starting func1() in background"
        << " and func2() in foreground:" << endl;

    future<int> result1(std::async(launch::async, func1));

    int result2 = func2();

    int result = result1.get() + result2;

    cout << "\nresult of func1()+func2(): " << result << endl;
}


// ={=========================================================================
// cxx-async with async option

// starting 2 operations asynchronously
// 
// ..........++++++++++
// done

TEST(CconThreadTest, UseAsyncX)
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
