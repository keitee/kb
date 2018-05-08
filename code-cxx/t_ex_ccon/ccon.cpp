#include <iostream>
#include <thread>
#include <future>
#include <list>
#include <mutex>
#include <queue>
#include <stack>
#include <exception>

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

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
