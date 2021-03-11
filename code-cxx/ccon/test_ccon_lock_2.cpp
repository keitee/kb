#include <future>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

namespace
{
  // use std::mutex
  std::mutex m{};

  std::string print_use_lock_guard(std::string const &s)
  {
    size_t i{};

    std::lock_guard<std::mutex> l(m);

    std::string result{};

    std::string name;

    name = (std::string::npos != s.find("first")) ? "first" : "second";

    for (i = 0; i < s.size(); ++i)
    {
      std::cout << "name: " << name << " sleeps for 1 sec\n";

      // this can make two threads switching? no
      //
      // std::this_thread::yield();
      //
      // The exact behavior of this function depends on the implementation,
      // in particular on the mechanics of the OS scheduler in use and the
      // state of the system. For example, a first-in-first-out realtime
      // scheduler (SCHED_FIFO in Linux) would suspend the current thread
      // and put it on the back of the queue of the same-priority threads
      // that are ready to run (and if there are no other threads at the
      // same priority, yield has no effect).
      //
      // NOTE: in this experiment, no effect.

      this_thread::sleep_for(chrono::seconds(1));
    }

    result = "waited for " + to_string(i * 20) + "ms and " + s;
    return result;
  }
} // namespace

// ={=========================================================================

int main(int argc, char **argv)
{
  {
    auto f1 = std::async(std::launch::async,
                         print_use_lock_guard,
                         "Hello from a first thread");

    auto f2 = std::async(std::launch::async,
                         print_use_lock_guard,
                         "Hello from a second thread");

    // waits for two threads to end
    std::cout << f1.get() << "\n";
    std::cout << f2.get() << "\n";
  }

  return (EXIT_SUCCESS);
}
