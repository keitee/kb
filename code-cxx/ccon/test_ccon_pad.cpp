#include <condition_variable>
#include <exception>
#include <future>
#include <iostream>
#include <list>
#include <mutex>
#include <queue>
#include <stack>
#include <thread>

// #include <string.h>
// #include <pthread.h>
// #include <sys/syscall.h>
// #include <unistd.h>

#include "gmock/gmock.h"

using namespace std;
using namespace std::placeholders;
using namespace testing;

namespace cxx_ex
{
} // namespace cxx_ex

// ={=========================================================================
// cxx-atomic
TEST(CConAtomic, check_lock_free_types)
{
  using namespace cxx_ex;

  // atomic_flag do not have is_lock_free() since it's the only type that
  // lock free is guranteeded.
  {
    std::atomic_flag flag{ATOMIC_FLAG_INIT};

    // flag.is_lock_free();
  }

  // bool type?
  {
    std::atomic<bool> flag{};

    auto free = flag.is_lock_free();

    EXPECT_THAT(free, true);
  }

  // int?
  {
    std::atomic<int> flag{};

    auto free = flag.is_lock_free();

    EXPECT_THAT(free, true);
  }

  // long?
  {
    std::atomic<long> flag{};

    auto free = flag.is_lock_free();

    EXPECT_THAT(free, true);
  }
}

namespace cxx_atomic
{
  // Listing 5.1 Implementation of a spinlock mutex using std::atomic_flag
  // note that there is no try_lock() in the listing 5.1
  class SpinLock
  {
  private:
    std::atomic_flag lock_;

  public:
    SpinLock()
        : lock_(ATOMIC_FLAG_INIT)
    {}

    void lock()
    {
      while (lock_.test_and_set(std::memory_order_acquire))
        ;
    }

    void unlock() { lock_.clear(std::memory_order_release); }

    bool try_lock() { return lock_.test_and_set(std::memory_order_acquire); }
  };

  // use SpinLock

  SpinLock spin_lock{};

  std::string print_use_spin_lock(std::string const &s)
  {
    // both works
    std::lock_guard<SpinLock> l(spin_lock);
    // std::unique_lock<SpinLock> l(spin_lock);

    std::string result{};

    size_t i{};

    std::string name =
      (std::string::npos != s.find("first")) ? "first" : "second";

    for (i = 0; i < s.size(); ++i)
    {
      // std::cout << "spinlock thread sleeps for 1 sec\n";
      std::cout << "name: " << name << " sleeps for 1 sec\n";
      this_thread::sleep_for(chrono::seconds(1));
    }

    result = name + " waited for " + to_string(i * 20) + "ms and " + s;

    std::cout << result << "\n";

    return result;
  }
} // namespace cxx_atomic

/*
// ={=========================================================================
cxx-atomic-flag

The limited feature set makes std::atomic_flag ideally suited to use as a
spin-lock mutex. Initially the flag is clear and the mutex is unlocked.
To lock the mutex, loop on test_and_set() until the old value is false,
indicating that this thread set the value to true. Unlocking the mutex
is simply a matter of clearing the flag. Such an implementation is shown
in the following listing.

bool test_and_set()

Atomically sets the flag and check whether or not it was set. returns true of
the flag was set at the point of the call, flase if the flag was clear. This
is an atomic "read-modify-write" operation for the memory location comprising
*this.

// /7/bits/atomic_base.h
{
#define ATOMIC_FLAG_INIT { 0 }

  /// atomic_flag
  struct atomic_flag : public __atomic_flag_base
  {
    test_and_set(memory_order __m = memory_order_seq_cst) noexcept
    {
      return __atomic_test_and_set (&_M_i, __m);
    }

    _GLIBCXX_ALWAYS_INLINE bool
    test_and_set(memory_order __m = memory_order_seq_cst) volatile noexcept
    {
      return __atomic_test_and_set (&_M_i, __m);
    }
  }
}

// /7/atomic
{
  inline bool
  atomic_flag_test_and_set(atomic_flag* __a) noexcept
  { return atomic_flag_test_and_set_explicit(__a, memory_order_seq_cst); }

  inline bool
  atomic_flag_test_and_set(volatile atomic_flag* __a) noexcept
  { return atomic_flag_test_and_set_explicit(__a, memory_order_seq_cst); }
}

*/

// ={=========================================================================
TEST(CConAtomic, atomic_flag_and_spin_lock_1)
{
  using namespace cxx_atomic;

  {
    auto f1 = std::async(std::launch::async,
                         print_use_spin_lock,
                         "Hello from a first thread");

    auto f2 = std::async(std::launch::async,
                         print_use_spin_lock,
                         "Hello from a second thread");

    EXPECT_THAT(f1.get(),
                "first waited for 500ms and Hello from a first thread");
    EXPECT_THAT(f2.get(),
                "second waited for 520ms and Hello from a second thread");
  }
}

/*
std::mutex::try_lock

bool try_lock(); (since C++11)

Tries to lock the mutex. Returns immediately. On successful lock acquisition
returns true, otherwise returns false.

*/

// ={=========================================================================
TEST(CConAtomic, atomic_flag_and_spin_lock_2)
{
  using namespace cxx_atomic;

  {
    auto f1 = std::async(std::launch::async,
                         print_use_spin_lock,
                         "Hello from a first thread");

    auto f2 = std::async(std::launch::async, []() {
      for (int i = 0; i < 20; ++i)
      {
        if (spin_lock.try_lock())
          std::cout << "second: try_lock returns true and got a lock"
                    << std::endl;
        else
          std::cout << "second: try_lock returns false and didn't get a lock"
                    << std::endl;
      }
    });

    EXPECT_THAT(f1.get(),
                "first waited for 500ms and Hello from a first thread");
  }
}

/*
// ={=========================================================================
5.2.3 Operations on std::atomic<bool>

The most basic of the atomic integral types is std::atomic<bool>. This is a
more full-featured Boolean flag than std::atomic_flag, as you might expect.
Although it’s still not copy-constructible or copy-assignable, you can
construct it from a nonatomic bool, so it can be initially true or false, and
you can also assign to instances of std::atomic<bool> from a nonatomic bool

o differs from the general convention of returning a reference to the object
it’s assigned to: it returns a bool with the value assigned instead.

This avoids the potential problems that could stem from the usual habit of
such assignment operators returning a reference to the object being assigned
to. In order to get the stored value from such a reference, the code would
have to perform a separate read, thus allowing another thread to modify the
value between the assignment and the read and opening the door for a race
condition.

std::atomic<T>::operator=

(1)	(since C++11)
T operator=( T desired ) noexcept;
T operator=( T desired ) volatile noexcept;

(2)	(since C++11)
atomic& operator=( const atomic& ) = delete;
atomic& operator=( const atomic& ) volatile = delete;

1) Atomically assigns the desired value to the atomic variable. Equivalent to
store(desired). The volatile-qualified version is deprecated if
std::atomic<T>::is_always_lock_free is false. (since C++20)

2) Atomic variables are not CopyAssignable.

*/
TEST(CConAtomic, atomic_bool_is_assignable)
{
  // error: use of deleted function ‘std::atomic_flag& std::atomic_flag::operator=(const std::atomic_flag&)’
  //      flag = true;
  //             ^~~~
  //
  // {
  //   std::atomic_flag flag{ATOMIC_FLAG_INIT};
  //   flag = true;
  //   EXPECT_THAT(flag == true, true);
  // }

  {
    std::atomic<bool> flag{false};

    flag = true;

    EXPECT_THAT(flag == true, true);
  }
}

// ={=========================================================================

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
