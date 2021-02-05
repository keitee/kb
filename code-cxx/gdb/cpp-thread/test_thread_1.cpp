#include "slog.h"
#include <mutex>
#include <thread>

namespace
{
  std::mutex m_mutex;

  void function2()
  {
    LOG_MSG("function2: trying to lock...");

    std::lock_guard<std::mutex> lock(m_mutex);

    LOG_MSG("function2: have got a lock...");

    std::this_thread::sleep_for(std::chrono::seconds(10));
  }

  void function1()
  {
    std::lock_guard<std::mutex> lock(m_mutex);

    for (int i = 0; i < 20; ++i)
    {
      LOG_MSG("function1: looping in the thread {%d}...", i);
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    function2();
  }
}

// cause deadlock with non-detached thread

int main()
{
  LOG_MSG("main: start a thread...");

  std::thread t([&] { function1(); });

  std::this_thread::sleep_for(std::chrono::seconds(5));

  LOG_MSG("main: join a thread...");

  t.join();

  LOG_MSG("main: ends...");
}
