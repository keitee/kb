#include "gmock/gmock.h"

#include <iostream>
#include <list>
#include <deque>
#include <fstream>
#include <thread>
#include <future>
#include <queue>

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;


/*
={=============================================================================
Concurrency

61. Parallel transform algorithm

Write a general-purpose algorithm that applies a given unary function to
transform the elements of a range in parallel. The unary operation used to
transform the range must not invalidate range iterators or modify the elements
of the range. The level of parallelism, that is, the number of execution threads
and the way it is achieved, is an implementation detail.

The general-purpose function std::transform() applies a given function to a
range and stores the result in another (or the same) range. The requirement for
this problem is implementing a parallel version of such a function. 

A general-purpose one would take iterators as arguments to define the first and
one-past-last element of the range. Because the unary function is applied in the
same manner to all the elements of the range, it is fairly simple to parallelize
the operation. For this task, we will be using threads. Since it is not
specified how many threads should be running at the same time, we could use
std::thread::hardware_concurrency(). This function returns a hint for the number
of concurrent threads supported by the implementation.

A parallel version of the algorithm performs better than a sequential
implementation only if the size of the range exceeds a particular threshold,
which may vary with compilation options, platform, or hardware. In the following
implementation that threshold is set to 10,000 elements. As a further exercise,
you could experiment with various thresholds and range sizes to see how the
execution time changes.

The following function, ptransform(), implements the parallel transform
algorithm as requested. It simply calls std::transform() if the range size does
not exceed the defined threshold. Otherwise, it splits the range into several
equal parts, one for each thread, and calls std::transform() on each thread for
a particular subrange. In this case, the function blocks the calling thread
until all the worker threads finish execution:

template <typename RandomAccessIterator, typename F>
void ptransform(RandomAccessIterator begin, RandomAccessIterator end, 
                F&& f)
{
   auto size = std::distance(begin, end);
   if (size <= 10000)
   {
      std::transform(begin, end, begin, std::forward<F>(f));
   }
   else
   {
      std::vector<std::thread> threads;
      int thread_count = std::thread::hardware_concurrency();
      auto first = begin;
      auto last = first;
      size /= thread_count;
      for (int i = 0; i < thread_count; ++i)
      {
         first = last;
         if (i == thread_count - 1) last = end;
         else std::advance(last, size);

         threads.emplace_back([first, last, &f]() {
            std::transform(first, last, first, std::forward<F>(f));
         });
      }

      for (auto & t : threads) t.join();
   }
}

The function palter(), shown as follows, is a helper function that applies
ptransform() to an std::vector and returns another std::vector with the result:

template <typename T, typename F>
std::vector<T> palter(std::vector<T> data, F&& f)
{
   ptransform(std::begin(data), std::end(data),
              std::forward<F>(f));
   return data;
}

The function can be used as follows (a complete example can be found in the
    source code accompanying this book):

int main()
{
   std::vector<int> data(1000000);
   // init data
   auto result = palter(data, [](int const e) {return e * e; });
}

In C++17, a series of standard general-purpose algorithms, including
std::transform(), have overloads that implement a parallel version of the
algorithm that can be executed according to a specified execution policy.

*/

namespace U61_Text
{

  template <typename Iterator, typename F>
    void parallel_transform(Iterator begin, Iterator end, F&& f) 
    {
      auto size = std::distance(begin, end);
      if (size <= 10000)
      {
        std::transform(begin, end, begin, std::forward<F>(f));
      }
      else
      {
        std::vector<std::thread> threads;

        // since it's 1 for VM
        // int thread_count = std::thread::hardware_concurrency();

        int thread_count = 4;

        auto first = begin;
        auto last = first;
        size /= thread_count;

        for (int i = 0; i < thread_count; ++i)
        {
          first = last;

          if (i == thread_count -1)
            last = end;
          else
            std::advance(last, size);

          // *cxx-vector-emplace*
          threads.emplace_back(
              [first, last, &f]()
              {
              std::transform(first, last, first, std::forward<F>(f));
              });
        }

        for (auto &t : threads)
          t.join();
      }
    }

  template <typename T, typename F>
    std::vector<T> palter(std::vector<T> data, F&& f)
    {
      parallel_transform(std::begin(data), std::end(data),
          std::forward<F>(f));
      return data;
    }

} // namespace


TEST(U61, Text)
{
  using namespace U61_Text;

  // initialize all 2. this is different from the text since it will have all 0
  // and the result of parallen_transform is also 0 in that case.

  std::vector<int> data(1000000, 2);

  auto result = palter(data, [](int const e) {return e * e; });

  // initialize all 4
  std::vector<int> expected(1000000, 4);

  EXPECT_THAT(result, expected);
}


/*
={=============================================================================
62. Parallel min and max element algorithms using threads

Implement general-purpose parallel algorithms that find the minimum value and,
respectively, the maximum value in a given range. The parallelism should be
implemented using threads, although the number of concurrent threads is an
implementation detail.lgorithms and Data Structures

{
   std::vector<int> data(count);
   // init data
   auto rmin = pmin(std::begin(data), std::end(data));
   auto rmax = pmax(std::begin(data), std::end(data));
}

*/

// if take the same approach as U61

namespace U62_2019_01_14
{

  template <typename Iterator>
    typename std::iterator_traits<Iterator>::value_type 
    paralle_min_element(Iterator begin, Iterator end)
    {
      using T = typename std::iterator_traits<Iterator>::value_type;

      T result;

      auto size = std::distance(begin, end);

      if (size <= 10000)
      {
        std::min_element(begin, end);
      }
      else
      {
        std::vector<std::thread> threads;

        // for VM
        int thread_count = 4;

        auto first = begin;
        auto last = first;
        size /= thread_count;

        std::vector<T> results(thread_count);

        for (int i = 0; i < thread_count; ++i)
        {
          first = last;

          if (i == thread_count -1)
            last = end;
          else 
            std::advance(last, size);

          // wrong result as each thread may use any depending on vaules when
          // runs since it uses reference
          // threads.emplace_back([&]()

          threads.emplace_back([first, last, i, &results]()
              {
                results[i] = *std::min_element(first, last);
                // cout << "result[" << i << "]: " << results[i]
                // << ", dist: " << std::distance(first, last) << endl;
              });
        }

        for (auto &t : threads)
          t.join();

        // for (auto &e : results)
        //   cout << "r: " << e << endl;

        result = *std::min_element(results.begin(), results.end());
      }

      return result;
    }

} // namespace

TEST(U62, 2019_01_14)
{
  using namespace U62_2019_01_14;

  // initialize all 2. this is different from the text since it will have all 0
  // and the result of parallen_transform is also 0 in that case.

  std::vector<int> data(1000000, 2);

  auto result = paralle_min_element(data.begin(), data.end());

  EXPECT_THAT(result, 2);
}


// Similar to parallel_accumulate and this supports flexibility of having
// function object.

namespace U62_Text
{

  template <typename Iterator, typename F>
    typename std::iterator_traits<Iterator>::value_type
    parallel_process(Iterator begin, Iterator end, F&& f)
    {
      using T = typename std::iterator_traits<Iterator>::value_type;

      auto size = std::distance(begin, end);

      if (size <= 10000)
      {
        // return std::forward<F>(f)(begin, end);
        return f(begin, end);
      }
      else
      {
        std::vector<std::thread> threads;

        // for VM
        int thread_count = 4;

        auto first = begin;
        auto last = first;
        size /= thread_count;

        std::vector<T> results(thread_count);

        for (int i = 0; i < thread_count; ++i)
        {
          first = last;

          if (i == thread_count -1)
            last = end;
          else
            std::advance(last, size);

          threads.emplace_back([first, last, i, &results, f]()
              {
                results[i] = f(first, last);

                // compile error and why need to forward rather then simply
                // calling?
                // results[i] = std::forward<F>(f)(first, last);
              });
        }

        for (auto &t : threads)
          t.join();

        // return std::forward<F>(f)(results.begin(), results.end());
        return f(results.begin(), results.end());
      }
    }

  template <typename Iterator>
    typename std::iterator_traits<Iterator>::value_type
    parallel_min(Iterator begin, Iterator end)
    {
      return parallel_process(begin, end,
          [](Iterator b, Iterator e){return *std::min_element(b, e);});
    }

  template <typename Iterator>
    typename std::iterator_traits<Iterator>::value_type
    parallel_max(Iterator begin, Iterator end)
    {
      return parallel_process(begin, end,
          [](Iterator b, Iterator e){return *std::max_element(b, e);});
    }

  class IntegerSequence
  {
    public:
      explicit IntegerSequence(int value = 0) : value_(value) {}
      int operator()() { return ++value_; }

    private:
      int value_;
  };
} // namespace

TEST(U62, Text)
{
  using namespace U62_Text;

  std::vector<int> coll;
  std::generate_n(back_inserter(coll), 1000000, IntegerSequence(0));

  auto min_result = parallel_min(coll.begin(), coll.end());
  auto max_result = parallel_max(coll.begin(), coll.end());

  EXPECT_THAT(min_result, 1);
  EXPECT_THAT(max_result, 1000000);
}


/*
={=============================================================================
63. Parallel min and max element algorithms using asynchronous functions

Implement general-purpose parallel algorithms that find the minimum value and,
respectively, the maximum value in a given range. The parallelism should be
implemented using asynchronous functions, although the number of concurrent
functions is an implementation detail.

*/

namespace U63_Text
{

  template <typename Iterator, typename F>
    typename std::iterator_traits<Iterator>::value_type
    parallel_process(Iterator begin, Iterator end, F&& f)
    {
      using T = typename std::iterator_traits<Iterator>::value_type;

      auto size = std::distance(begin, end);

      if (size <= 10000)
      {
        // return std::forward<F>(f)(begin, end);
        return f(begin, end);
      }
      else
      {
        // std::vector<std::thread> threads;

        std::vector<std::future<T>> tasks;

        // for VM
        int thread_count = 4;

        auto first = begin;
        auto last = first;
        size /= thread_count;

        std::vector<T> results(thread_count);

        for (int i = 0; i < thread_count; ++i)
        {
          first = last;

          if (i == thread_count -1)
            last = end;
          else
            std::advance(last, size);

          // threads.emplace_back([first, last, i, &results, f]()
          //     {
          //       results[i] = f(first, last);
          //     });

          tasks.emplace_back(std::async(std::launch::async, [first, last, f]()
              {
                return f(first, last);
              }));
        }

        // for (auto &t : threads)
        //   t.join();

        for (auto &t : tasks)
          results.push_back(t.get());

        // return std::forward<F>(f)(results.begin(), results.end());
        return f(results.begin(), results.end());
      }
    }

  template <typename Iterator>
    typename std::iterator_traits<Iterator>::value_type
    parallel_min(Iterator begin, Iterator end)
    {
      return parallel_process(begin, end,
          [](Iterator b, Iterator e){return *std::min_element(b, e);});
    }

  template <typename Iterator>
    typename std::iterator_traits<Iterator>::value_type
    parallel_max(Iterator begin, Iterator end)
    {
      return parallel_process(begin, end,
          [](Iterator b, Iterator e){return *std::max_element(b, e);});
    }

  class IntegerSequence
  {
    public:
      explicit IntegerSequence(int value = 0) : value_(value) {}
      int operator()() { return ++value_; }

    private:
      int value_;
  };
} // namespace

TEST(U63, Text)
{
  using namespace U62_Text;

  std::vector<int> coll;
  std::generate_n(back_inserter(coll), 1000000, IntegerSequence(0));

  auto min_result = parallel_min(coll.begin(), coll.end());
  auto max_result = parallel_max(coll.begin(), coll.end());

  EXPECT_THAT(min_result, 1);
  EXPECT_THAT(max_result, 1000000);
}


/*
={=============================================================================
64. Parallel sort algorithm

Write a parallel version of the sort algorithm as defined for problem 53. Sort
Algorithm, in Chapter 6, Algorithms and Data Structures, which, given a pair of
random access iterators to define its lower and upper bounds, sorts the elements
of the range using the quicksort algorithm. The function should use the
comparison operators for comparing the elements of the range. The level of
parallelism and the way to achieve it is an implementation detail.

*/


/*
={=============================================================================
65. Thread-safe logging to the console

Write a class that enables components running in different threads to safely
print log messages to the console by synchronizing access to the standard output
stream to guarantee the integrity of the output. This logging component should
have a method called log() with a string argument representing the message to be
printed to the console.

*/

namespace U65_Text
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
        std::lock_guard<std::mutex> lock(mt);
        std::cout << "LOG: " << message << std::endl;
      }

    protected:
      logger() {}

    private:
      std::mutex mt;
  };
} // namespace


/*
={=============================================================================
66. Customer service system

Write a program that simulates the way customers are served in an office. The
office has three desks where customers can be served at the same time. Customers
can enter the office at any time. They take a ticket with a service number from
a ticketing machine and wait until their number is next for service at one of
the desks. Customers are served in the order they entered the office, or more
precisely, in the order given by their ticket. Every time a service desk
finishes serving a customer, the next customer in order is served. The
simulation should stop after a particular number of customers have been issued
tickets and served.

*/

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
        std::lock_guard<std::mutex> lock(mt);
        std::cout << "LOG: " << message << std::endl;
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

TEST(U66, Text)
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


/*
={=============================================================================
Design Pattern

67. Validating passwords

Write a program that validates password strength based on predefined rules,
which may then be selected in various combinations. At a minimum, every password
must meet a minimum length requirement. In addition, other rules could be
enforced, such as the presence of at least one symbol, digit, uppercase and
lowercase letter, and so on.

The problem described here is a typical case for the decorator pattern. This
design pattern allows adding behavior to an object without affecting other
objects of the same type. This is achieved by wrapping an object within another
object. Multiple decorators could be stacked on top of each other, each time
adding new functionality. In our case, the functionality would be validating
that a given password meets a particular requirement.

The following class diagram describes the pattern for validating passwords:

*/


namespace U67_Text
{

} // namespace


// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
