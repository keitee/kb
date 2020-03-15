#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "work.h"
#include <atomic>
#include <chrono>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>

// ThreadPool, which creates worker threads to handle the work queue

// code/c9/6/ThreadPool.h
//
// supports only one work

#if 0
class ThreadPool {
public:
   virtual ~ThreadPool() {
      if (workThread_)
         workThread_->join();
   }

   void start() {
      workThread_ = std::make_shared<std::thread>(&ThreadPool::worker, this);
   }
   // ...
   bool hasWork() {
      return !workQueue_.empty();
   }

   void add(Work work) {
      workQueue_.push_front(work); 
      // std::cout << "add: " << workQueue_.size() << std::endl;
   }

   Work pullWork() {
      auto work = workQueue_.back();
      workQueue_.pop_back();
      // std::cout << "pullWork" << std::endl;
      return work;
   }

private:
   void worker() {

      // when no works to do, do busy looping
      while (!hasWork())
        ;
      
      pullWork().execute();
   }

   std::deque<Work> workQueue_;
   std::shared_ptr<std::thread> workThread_;
};


// To support multiple works

class ThreadPool {
public:
    virtual ~ThreadPool() {

        // <problem-2> hang
        // Unfortunately...no, fortunately, our lame implementation hangs the
        // test run every time. Consistent failure when dealing with threads is
        // a great step toward a solution. A bit of analysis suggests that:
        //
        // once the test completes, the ThreadPool destructor sets the done_ flag
        // to true and then attempts to join the thread. The thread can’t
        // complete because it’s stuck in the while loop that waits for work to
        // be available.
        //
        // Introducing done_ to support multiple work cause racy because as
        // said in the book:
        //
        // NOTE: every time? However, it works for me when run. To make the hang
        // happen, make some delay here so that done_ is set after thread enters
        // busy loop and this hang is reproduced.
        // 
        // std::this_thread::sleep_for(std::chrono::milliseconds(10));

        done_ = true;

        if (workThread_)
            workThread_->join();
    }

   // ...
   void start() {
      workThread_ = std::make_shared<std::thread>(&ThreadPool::worker, this);
   }

   bool hasWork() {
      return !workQueue_.empty();
   }

   void add(Work work) {
      workQueue_.push_front(work); 
   }

   Work pullWork() {
      auto work = workQueue_.back();
      workQueue_.pop_back();
      return work;
   }

private:

   // <problem-2>
   // void worker() {
   //    while (!done_) {
   //       while (!hasWork())
   //          ;
   //       pullWork().execute();
   //    }
   // }

   void worker() {
      while (!done_) {
         while (!done_ && !hasWork())
            ;
         pullWork().execute();
      }
   }

   std::atomic<bool> done_{false};
   std::deque<Work> workQueue_;
   std::shared_ptr<std::thread> workThread_;
};


// <problem-3>
// Our tests no longer hang and pass on their first run. Once again, however,
// they fail intermittently. "We need to hit things harder" from the test so that
// it fails every time. A quick attempt at bumping up the number of work items
// added to the loop doesn’t appear to make much difference. 
//
// NOTE fail intermittently? For me, ran 1000 times and no luck to see this
// reproduced.
//
// "We need to hit things harder"? So far, "1 client and 1 thread in a pool"
//
// Our test needs to add work items from multiple threads created by the test
// itself. More client threads!

#endif

#if 0
// support mutiple worker threads and mutex on IFs

class ThreadPool
{
    public:
        ~ThreadPool()
        {
            stop();
        }

        void stop()
        {
            done_ = true;
            for (auto &t : threads_) t.join();
        }

        void start(unsigned int numberOfThreads=1)
        {
            for (unsigned int i{0}; i < numberOfThreads; ++i)
                threads_.push_back(std::thread(&ThreadPool::worker, this));
        }

        bool hasWork()
        {
            std::lock_guard<std::mutex> block(mutex_);
            return !workq_.empty();
        }

        virtual void add(Work work)
        {
            std::lock_guard<std::mutex> block(mutex_);
            workq_.push_front(work);
        }

        Work pullWork()
        {
            std::lock_guard<std::mutex> block(mutex_);

            // <problem-5>
            //
            // Our test fails consistently. Given our suspicion around the
            // worker function, we add a line of code to handle the case where
            // work is no longer available (in other words, where another thread
            // picked it up).
            
            if (workq_.empty()) return Work{};

            auto work = workq_.back();
            workq_.pop_back();
            return work;
        }

    private:
        std::deque<Work> workq_;
        std::vector<std::thread> threads_;
        std::atomic<bool> done_{false};
        std::mutex mutex_;

        void worker()
        {
            // supports multiple works to process
            while (!done_)
            {
                // when no works to do, do busy looping
                while (!done_ && !hasWork())
                    ;

                // if (done_) break;

                pullWork().execute();
            }

            // std::cout << "worker ends" << std::endl;
        }
};

#endif

class ThreadPool
{
private:
  std::deque<Work> q_;
  std::vector<std::thread> threads_;

  // *cxx-atomic*
  std::atomic<bool> done_{false};
  std::mutex m_;

  void worker_()
  {
    while (!done_)
    {
      // no works to do and do busy loop
      while (!done_ && !hasWork())
        ;

      pullWork().execute();
    }
  }

public:
  // no ctor

  ~ThreadPool() { stop(); }

  void start(unsigned int poolSize = 1)
  {
    for (unsigned int i{0}; i < poolSize; ++i)
      threads_.emplace_back(std::thread(&ThreadPool::worker_, this));
  }

  void stop()
  {
    done_ = true;

    // cannot use "const auto" since join() is non-const
    for (auto &t : threads_)
      t.join();
  }

  bool hasWork()
  {
    std::lock_guard<std::mutex> lock(m_);
    return !q_.empty();
  }

  void add(const Work &work)
  {
    std::lock_guard<std::mutex> lock(m_);
    q_.emplace_front(work);
  }

  Work pullWork()
  {
    std::lock_guard<std::mutex> lock(m_);

    if (q_.empty())
      return Work{};

    auto work = q_.back();
    q_.pop_back();
    return work;
  }
};

#endif // THREADPOOL_H
