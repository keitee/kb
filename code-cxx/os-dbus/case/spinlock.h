//
// spinlock.h
//

#ifndef K_SPINLOCK_H
#define K_SPINLOCK_H

#include <atomic>

namespace COMMON
{
  // summary from the comment:
  // a mutex is overkill. It has he same API as std::mutex so can be swapped
  // out since it implements the C++ BasicLockable and Lockable requirements.

  class SpinLock
  {
    private:
      std::atomic_flag locked_;

    public:
      SpinLock() : locked_(ATOMIC_FLAG_INIT) {}

      void lock()
      {
        while (locked_.test_and_set(std::memory_order_acquire))
          ;
      }

      void unlock()
      {
        locked_.clear(std::memory_order_release);
      }

      bool try_lock()
      {
        return locked_.test_and_set(std::memory_order_acquire);
      }
  };
} // namespace

#endif // K_SPINLOCK_H

