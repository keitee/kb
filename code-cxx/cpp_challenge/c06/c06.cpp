#include "gmock/gmock.h"

#include <iostream>
#include <list>
#include <deque>
#include <fstream>
#include <mutex>
#include <thread>

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;


// ={=========================================================================

/*

Algorithms and Data Structures

45. Priority queue

Write a data structure that represents a priority queue that provides constant
time lookup for the largest element, but has logarithmic time complexity for
adding and removing elements. A queue inserts new elements at the end and
removes elements from the top. By default, the queue should use operator< to
compare elements, but it should be possible for the user to provide a comparison
function object that returns true if the first argument is less than the second.
The implementation must provide at least the following operations:

push() to add a new element
pop() to remove the top element
top() to provide access to the top element
size() to indicate the number of elements in the queue
empty() to indicate whether the queue is empty

A priority queue is an abstract data type whose elements have a priority
attached to them. Instead of working as a first-in-first-out container, a
priority queue makes elements available in the order of their priority. This
data structure is used in algorithms such as Dijkstra's shortest path, Prim's
algorithm, heap sort, the A* search algorithm, in Huffman codes used for data
compression, and others.

A very simple approach to implement a priority queue would be to use an
std::vector as the underlying container of elements and always maintain it
sorted. That means the maximum and minimum elements are always at the two ends.
However, this approach does not provide the most efficient operations.

The most suitable data structure that can be used to implement a priority queue
is a heap. This is a tree-based data structure that satisfies the following
property: if P is a parent node of C, then the key (the value) of P is either
greater than or equal to (in a max heap) or less than or equal to (in a min
heap) the key of C.

The standard library provides several operations for working with heaps:

std::make_heap(): This creates a max heap for the given range, using either
operator< or a user-provided comparison function to order the elements

std::push_heap(): This inserts a new element at the end of the max heap

std::pop_heap(): This removes the first element of the heap (by swapping the
values in the first and last position and making the sub-range [first, last-1) a
max heap)

A priority queue implementation, that uses std::vector to hold data and the
standard functions for heaps, can look as follows:

*/

namespace U45_text
{
  template <typename T, typename Compare = std::less<typename std::vector<T>::value_type>>
    class priority_queue
    {
      using size_type = typename std::vector<T>::size_type;
      using value_type = typename std::vector<T>::value_type;
      using const_reference = typename std::vector<T>::const_reference;
      using reference = typename std::vector<T>::reference;

      public:
        bool empty() const noexcept
        { return coll_.empty(); }

        size_type size() const noexcept 
        { return coll_.size(); }

        void push(value_type const& value)
        {
          coll_.push_back(value);
          std::push_heap(coll_.begin(), coll_.end(), f_);
        }

        void pop()
        {
          std::pop_heap(coll_.begin(), coll_.end(), f_);
          coll_.pop_back();
        }

        const_reference top() const 
        { return coll_.front(); }

      private:
        std::vector<T> coll_;
        Compare f_;
    };
} // namespace


TEST(U45, Text)
{
  using namespace U45_text;

  priority_queue<int> q;
  for (int i : {1, 5, 3, 1, 13, 21, 8})
  {
    q.push(i);
  }

  EXPECT_TRUE(!q.empty());
  EXPECT_TRUE(q.size() == 7);

  vector<int> coll{};

  while (!q.empty())
  {
    coll.push_back(q.top());
    q.pop();
  }

  // the default make max version which makes descending order.
  EXPECT_THAT(coll, ElementsAre(21, 13, 8, 5, 3, 1, 1));
}


/*
 
46. Circular buffer

Create a data structure that represents a circular buffer of a fixed size. A
circular buffer overwrites existing elements when the buffer is being filled
beyond its fixed size. The class you must write should:

Prohibit default construction

Support the creation of objects with a specified size

Allow checking of the buffer capacity and status (empty(), full(), size(),
capacity())

Add a new element, an operation that could potentially overwrite the oldest
element in the buffer

Remove the oldest element from the buffer

Support iteration through its elements

*/

namespace U46_text
{
  // done in cxx-algo
}


/*

47. Double buffer

Write a class that represents a buffer that could be written and read at the
same time without the two operations colliding. A read operation must provide
access to the old data while a write operation is in progress. Newly written
data must be available for reading upon completion of the write operation.

{
   double_buffer<int> buf(10);

   std::thread t([&buf]() {
      for (int i = 1; i < 1000; i += 10)
      {
         int data[] = { i, i + 1, i + 2, i + 3, i + 4, 
                        i + 5, i + 6,i + 7,i + 8,i + 9 };
         buf.write(data, 10);

         using namespace std::chrono_literals;
         std::this_thread::sleep_for(100ms);
       }
   });

   auto start = std::chrono::system_clock::now();
   do
   {
      print_buffer(buf);

      using namespace std::chrono_literals;
      std::this_thread::sleep_for(150ms);
   } while (std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now() - start).count() < 12);

   t.join();
}


The problem described here is a typical double buffering situation. Double
buffering is the most common case of multiple buffering, which is a technique
that allows a reader to see a complete version of the data and not a partially
updated version produced by a writer. This is a common technique – especially in
computer graphics – for avoiding flickering.

In order to implement the requested functionality, the buffer class that we
should write must have two internal buffers: one that contains temporary data
being written, and another one that contains completed (or committed) data. Upon
the completion of a write operation, the content of the temporary buffer is
written in the primary buffer. For the internal buffers, the implementation
below uses std::vector. When the write operation completes, instead of copying
data from one buffer to the other, we just swap the content of the two, which is
a much faster operation. Access to the completed data is provided with either
the read() function, which copies the content of the read buffer into a
designated output, or with direct element access (overloaded operator[]). Access
to the read buffer is synchronized with an std::mutex to make it safe to read
from one thread while another is writing to the buffer:

1. not use unique_lock and use lock_guard instead
2. swap buffers is a key idea
3. see use of cxx-vector
4. see use of cxx-mutable
5. do not impleent all from the text

*/

namespace U47_text
{
  template <typename T>
    class double_buffer
    {
      using value_type = T;
      using reference = T&;
      using const_reference = T const&;
      using pointer =  T*;

      public:
      explicit double_buffer(size_t const size) :
        rdbuf_(size), wrbuf_(size)
      {}

      size_t size() const noexcept 
      { return rdbuf_.size(); }

      void write(T const* ptr, size_t const size)
      {
        // std::unique_lock<std::mutex> lock(mt_);
        std::lock_guard<std::mutex> lock(mt_);
        auto length = std::min(size, wrbuf_.size());
        // *cxx-vector*
        std::copy(ptr, ptr + length, std::begin(wrbuf_));
        wrbuf_.swap(rdbuf_);
      }

      // *cxx-mutable*
      template <typename Output>
      void read(Output it) const
      {
        std::lock_guard<std::mutex> lock(mt_);
        std::copy(rdbuf_.cbegin(), rdbuf_.cend(), it);
      }

      private:
      std::vector<T> rdbuf_;
      std::vector<T> wrbuf_;
      mutable std::mutex mt_;
    };

  template <typename T>
    void print_buffer(double_buffer<T> const& buf)
    {
      buf.read(std::ostream_iterator<T>(std::cout, " "));
      std::cout << std::endl;
    }
} // namespace


TEST(U47, Text)
{
  using namespace U47_text;

  double_buffer<int> buf(10);

  std::thread t([&buf]() {
      for (int i = 1; i < 1000; i += 10)
      {
      int data[] = { i, i + 1, i + 2, i + 3, i + 4, 
      i + 5, i + 6,i + 7,i + 8,i + 9 };
      buf.write(data, 10);

      // using namespace std::chrono_literals;
      std::this_thread::sleep_for(chrono::milliseconds(100));
      }
      });

  auto start = std::chrono::system_clock::now();

  do
  {
    print_buffer(buf);

    // using namespace std::chrono_literals;
    std::this_thread::sleep_for(chrono::milliseconds(150));
  } while (std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now() - start).count() < 12);

  t.join();
}


/*

48. The most frequent element in a range

Write a function that, given a range, returns the most frequent element and the
number of times it appears in the range. If more than one element appears the
same maximum number of times then the function should return all the elements.
For instance, for the range {1,1,3,5,8,13,3,5,8,8,5}, it should return {5, 3}
and {8, 3}.

{
   auto range = std::vector<int>{1,1,3,5,8,13,3,5,8,8,5};
   auto result = find_most_frequent(range);

   for (auto const & e : result)
   {
      std::cout << e.first << " : " << e.second << std::endl;
   }
}

*/

namespace U48_2018_12_25
{
  std::vector<pair<int, int>> find_most_frequent(std::vector<int> const& coll)
  {
    std::vector<pair<int, int>> result{};
    std::vector<int> bit_set(100);

    // first loop to count occurance of the input
    for (auto e : coll)
      ++bit_set[e];

    // second loop to find the most frequent
    int current_element{};
    int current_max{};

    for (size_t i = 0; i < bit_set.size(); ++i)
    {
      if (current_max < bit_set[i])
      {
        // reset return coll
        result.clear();

        // update current set
        current_max = bit_set[i];
        current_element = i;
      }
      else if (current_max == bit_set[i])
      {
        // push current to the return coll
        result.push_back(make_pair(current_element, current_max));

        // update current set
        current_max = bit_set[i];
        current_element = i;
      }
      // else if (current_max > bit_set[i])
      // {
      //   // do nothing
      // }
    }

    // have to push current when is out of loop.
    if (current_element)
    {
      // push current to the return coll
      result.push_back(make_pair(current_element, current_max));
    }

    // // print results
    // for (auto e : result)
    //   cout << "{" << e.first << ", " << e.second << "}" << endl;
    
    return result;
  }
} // namesapce

TEST(U48, 2018_12_25)
{
  using namespace U48_2018_12_25;

  auto range = std::vector<int>{1,1,3,5,8,13,3,5,8,8,5};

  auto result = find_most_frequent(range);

  EXPECT_THAT(result, ElementsAre(make_pair(5, 3), make_pair(8, 3)));
}

namespace U48_Text
{
  template <typename T>
    std::vector<std::pair<T, size_t>> find_most_frequent(std::vector<T> const& range)
    {
      // loop1
      std::map<T, size_t> counts;
      for (auto const& e: range)
        counts[e]++;

      // loop2

      // *cxx-algo-max-element* *cxx-algo-minmax*
      //
      // op is used to compare two elements:
      // op(elem1,elem2)
      // It should return true when the first element is less than the second
      // element.

      auto maxelem = std::max_element(
          std::begin(counts), std::end(counts),
          [](pair<T, size_t> const& e1, pair<T, size_t> const& e2)
          { return e1.second < e2.second; });
      
      // loop3
      std::vector<std::pair<T, size_t>> result;

      std::copy_if(begin(counts), end(counts),
          back_inserter(result),
          // [](auto const& e)
          [=](pair<T, size_t> const& e)
          {
            return e.second == maxelem->second;
          });

      return result;
    }
} // namespace

TEST(U48, Text)
{
  using namespace U48_Text;

  auto range = std::vector<int>{1,1,3,5,8,13,3,5,8,8,5};

  auto result = find_most_frequent(range);

  EXPECT_THAT(result, ElementsAre(make_pair(5, 3), make_pair(8, 3)));
}


/*

49. Text histogram

Write a program that, given a text, determines and prints a histogram with the
frequency of each letter of the alphabet. The frequency is the percentage of the
number of appearances of each letter from the total count of letters. The
program should count only the appearances of letters and ignore digits, signs,
and other possible characters. The frequency must be determined based on the
count of letters and not the text size.

{
   auto result = analyze_text(R"(Lorem ipsum dolor sit amet, consectetur 
      adipiscing elit, sed do eiusmod tempor incididunt ut labore et 
      dolore magna aliqua.)");

   for (auto const & kvp : result)
   {
      std::cout << kvp.first << " : "
                << std::fixed
                << std::setw(5) << std::setfill(' ')
                << std::setprecision(2) << kvp.second << std::endl;
   }
}

*/

namespace U49_2018_12_27
{
  vector<pair<char, float>> analyze_text(string const& input)
  {
    map<char, int> counts{};

    size_t total_count{};

    for (auto e : input)
    {
      if (isalpha(e))
      {
        ++total_count;
        ++counts[e];
      }
    }

    vector<pair<char, float>> result;

    for (auto& e : counts)
    {
      float percent = (e.second * 100.0) / total_count;
      result.push_back(make_pair(e.first, percent));
    }

    // for (auto& e : result)
    //   cout << e.first << ", " << e.second << endl;

    return result;
  }
} // namespace

TEST(U49, 2018_12_27)
{
  using namespace U49_2018_12_27;

  auto result = analyze_text("Lorem ipsum dolor sit amet, consectetur" 
      "adipiscing elit, sed do eiusmod tempor incididunt ut labore et "
      "dolore magna aliqua.");
  //
  // auto result = analyze_text(R"(Lorem ipsum dolor sit amet, consectetur 
   //    adipiscing elit, sed do eiusmod tempor incididunt ut labore et 
   //    dolore magna aliqua.)");

   for (auto const & kvp : result)
   {
      std::cout << kvp.first << " : "
                << std::fixed
                << std::setw(5) << std::setfill(' ')
                << std::setprecision(2) << kvp.second << std::endl;
   }
}


// the text version use the same map for a return
// the text version has chars that are not seen input.

namespace U49_Text
{
  // *cxx-17*
  // std::map<char, double> analyze_text(std::string_view text)
  std::map<char, double> analyze_text(std::string text)
  {
    std::map<char, double> frequencies;

    // init a map
    for (char ch = 'a'; ch <= 'z'; ch++)
      frequencies[ch] = 0;

    for (auto ch : text)
    {
      if (isalpha(ch))
        frequencies[std::tolower(ch)]++;
    }

    // adds up occurances
    // *cxx-const-error* since key is const on map
    //    [](unsigned long long sum, pair<char, double>& e)

    auto total = std::accumulate(frequencies.begin(), frequencies.end(), 0ull,
        [](unsigned long long sum, pair<char const, double>& e)
        { return sum + e.second; });

    // calc percentages
    // 
    // *cxx-const-error* since key is const on map
    //    [=](pair<char, double>&e)

    std::for_each(frequencies.begin(), frequencies.end(),
        [=](pair<char const, double>&e)
        { e.second = (100.0 * e.second) / total; });

    return frequencies;
  }
} // namespace

TEST(U49, Text)
{
  using namespace U49_Text;

  auto result = analyze_text("Lorem ipsum dolor sit amet, consectetur" 
      "adipiscing elit, sed do eiusmod tempor incididunt ut labore et "
      "dolore magna aliqua.");
  //
  // auto result = analyze_text(R"(Lorem ipsum dolor sit amet, consectetur 
   //    adipiscing elit, sed do eiusmod tempor incididunt ut labore et 
   //    dolore magna aliqua.)");

   for (auto const & kvp : result)
   {
      std::cout << kvp.first << " : "
                << std::fixed
                << std::setw(5) << std::setfill(' ')
                << std::setprecision(2) << kvp.second << std::endl;
   }
}


/*

50. Filtering a list of phone numbers

Write a function that, given a list of phone numbers, returns only the numbers
that are from a specified country. The country is indicated by its phone country
code, such as 44 for Great Britain. Phone numbers may start with the country
code, a + followed by the country code, or have no country code. The ones from
this last category must be ignored.

{
   std::vector<std::string> numbers{
      "+40744909080",
      "44 7520 112233",
      "+44 7555 123456",
      "40 7200 123456",
      "7555 123456"
   };

   auto result = filter_numbers(numbers, "44");

   for (auto const & number : result)
   {
      std::cout << number << std::endl;
   }
}

*/

namespace U50_Text
{
  bool starts_with(std::string const& text, std::string const& prefix)
  {
    // *cxx-string-find* returns index of found so if text start with prefix
    // then should return 0.

    return text.find(prefix) == 0;
  }

  std::vector<std::string> filter_numbers(std::vector<std::string> const& numbers,
      std::string const& code)
  {
    std::vector<std::string> result;

    std::copy_if(numbers.begin(), numbers.end(), std::back_inserter(result),
        [=](std::string const& number)
        { return starts_with(number, code) || starts_with(number, "+" + code); }
        );

    return result;
  }
} // namespace

TEST(U50, Text)
{
  using namespace U50_Text;

  std::vector<std::string> numbers{
    "+40744909080",
      "44 7520 112233",
      "+44 7555 123456",
      "40 7200 123456",
      "7555 123456"
  };

  auto result = filter_numbers(numbers, "44");

  ostringstream os;
  for (auto const & number : result)
    os << number << std::endl;
  
  EXPECT_THAT(os.str(), "44 7520 112233\n+44 7555 123456\n");
}


/*

51. Transforming a list of phone numbers

Write a function that, given a list of phone numbers, transforms them so they
all start with a specified phone country code, preceded by the + sign. Any
whitespaces from a phone number should also be removed. The following is a list
of input and output examples:

07555 123456    => +447555123456
07555123456     => +447555123456
+44 7555 123456 => +447555123456
44 7555 123456  => +447555123456
7555 123456     => +447555123456

{
   std::vector<std::string> numbers{
      "07555 123456",
      "07555123456",
      "+44 7555 123456",
      "44 7555 123456",
      "7555 123456"
   };

   normalize_phone_numbers(numbers, "44");

   for (auto const & number : numbers)
   {
      std::cout << number << std::endl;
   }
}

*/

namespace U51_2018_12_28
{
  void 
} // namespace


// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
