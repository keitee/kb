#include <future>
#include <iostream>
#include <mutex>
#include <thread>

/*
two apps to see differences between two as to locking to find out more about
on this comment from spinlock.

a mutex is overkill. It has he same API as std::mutex so can be swapped out
since it implements the C++ BasicLockable and Lockable requirements.

-rwxr-xr-x 1 keitee keitee 655K Feb 28 10:38 test_ccon_lock_2*
-rwxr-xr-x 1 keitee keitee 654K Feb 28 10:38 test_ccon_lock_1*

to check CPU usage

ps -p 10258 -o %cpu,%mem,cmd

test_ccon_lock_2 shows no usage:

$ watch -n 1 ps -p `pgrep test_ccon_lock` -o %cpu,%mem,cmd
%CPU %MEM CMD
 0.0  0.0 ./test_ccon_lock_2


no difference between two as to tool-strace since both seems to show that they
both uses futex.

sudo strace -p `pgrep test_ccon_lock`
strace: Process 29720 attached
futex(0x7f31a75ba9d0, FUTEX_WAIT, 29721, NULL
) = 0
futex(0x55d8c4f81ea8, FUTEX_WAKE_PRIVATE, 2147483647) = 0
write(1, "waited for 500ms and Hello from "..., 47) = 47
futex(0x7f31a6db99d0, FUTEX_WAIT, 29722, NULL) = 0
futex(0x55d8c4f82098, FUTEX_WAKE_PRIVATE, 2147483647) = 0
write(1, "waited for 520ms and Hello from "..., 48) = 48
exit_group(0)                           = ?
+++ exited with 0 +++


sudo strace -p `pgrep test_ccon_lock`
strace: Process 29939 attached
futex(0x7fca1baf19d0, FUTEX_WAIT, 29940, NULL) = 0
futex(0x5642aeec6ea8, FUTEX_WAKE_PRIVATE, 2147483647) = 0
write(1, "waited for 500ms and Hello from "..., 47) = 47
futex(0x7fca1b2f09d0, FUTEX_WAIT, 29941, NULL) = 0
futex(0x5642aeec7098, FUTEX_WAKE_PRIVATE, 2147483647) = 0
write(1, "waited for 520ms and Hello from "..., 48) = 48
exit_group(0)                           = ?

*/

using namespace std;

namespace
{
  // cxx-atomic

  class SpinLock
  {
  private:
    std::atomic_flag lock_;

  public:
    SpinLock()
        : lock_{ATOMIC_FLAG_INIT}
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
    size_t i{};

    // both works
    std::lock_guard<SpinLock> l(spin_lock);
    // std::unique_lock<SpinLock> l(spin_lock);

    std::string result{};

    std::string name =
      (std::string::npos != s.find("first")) ? "first" : "second";

    for (i = 0; i < s.size(); ++i)
    {
      std::cout << "name: " << name << " sleeps for 1 sec\n";

      // this can make two threads switching? no
      std::this_thread::yield();

      this_thread::sleep_for(chrono::seconds(1));
    }

    result = "waited for " + to_string(i * 20) + "ms and " + s;
    return result;
  }
} // namespace

// ={=========================================================================

int main(int argc, char **argv)
{
  // use SpinLock
  {
    auto f1 = std::async(std::launch::async,
                         print_use_spin_lock,
                         "Hello from a first thread");

    auto f2 = std::async(std::launch::async,
                         print_use_spin_lock,
                         "Hello from a second thread");

    // waits for two threads to end
    std::cout << f1.get() << "\n";
    std::cout << f2.get() << "\n";
  }

  return (EXIT_SUCCESS);
}
