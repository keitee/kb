#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <deque>
#include <thread>
#include <atomic>
#include "work.h"

// ThreadPool, which creates worker threads to handle the work queue

class ThreadPool
{
    public:
        ~ThreadPool()
        {
            stop();
        }

        void stop()
        {
            // <problem 1>
            //
            // Introducing done_ to support multiple work cause racy because as
            // said in the book:
            //
            // once the test completes, the ThreadPool destructor sets the done_
            // flag to true and then attempts to join the thread. The thread
            // can’t complete because it’s stuck in the while loop that waits
            // for work to be available.
            //
            // However, it works for me when run. To make the hang happen, make
            // some delay here like:
            // 
            // std::string s;
            // for(int i = 0; i < 100; ++i)
            //     s = s + "x";
            //
            // so that done_ is get after thread enters busy loop and this hang
            // is reproduced.
            //
            // where:
            //
            // void worker()
            // {
            //     // supports multiple works to process
            //     while (!done_)
            //     {
            //         // when no works to do, do busy looping
            //         while (!hasWork())
            //             ;
            //
            //         pullWork().execute();
            //     }
            // }

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

        void add(Work work)
        {
            std::lock_guard<std::mutex> block(mutex_);
            workq_.push_front(work);
        }

        Work pullWork()
        {
            std::lock_guard<std::mutex> block(mutex_);

            // <problem>
            //
            // Our test fails consistently. Given our suspicion around the
            // worker function, we add a line of code to handle the case where
            // work is no longer available (in other words, where another thread
            // picked it up).
            //
            // The worker function seems to remain the only code with any such
            // potential. In worker, we loop until there is work; each time
            // through the loop establishes and immediately releases a lock
            // within hasWork. Once there is work, the loop exits, and control
            // falls through to the statement pullWork().execute(). What if,
            // during this short span, another thread has grabbed work?
            //
            // Otherwise, one of threads fails as:
            //
            // terminate called after throwing an instance of 'std::bad_function_call'
            //   what():  bad_function_call
            // Aborted (core dumped)
            //
            // (gdb) bt
            // #0  0x00007fd2bc07b067 in __GI_raise (sig=sig@entry=6) at ../nptl/sysdeps/unix/sysv/linux/raise.c:56
            // #1  0x00007fd2bc07c448 in __GI_abort () at abort.c:89
            // #2  0x00007fd2bc968b3d in __gnu_cxx::__verbose_terminate_handler() () from /usr/lib/x86_64-linux-gnu/libstdc++.so.6
            // #3  0x00007fd2bc966bb6 in ?? () from /usr/lib/x86_64-linux-gnu/libstdc++.so.6
            // #4  0x00007fd2bc966c01 in std::terminate() () from /usr/lib/x86_64-linux-gnu/libstdc++.so.6
            // #5  0x00007fd2bc9bea01 in ?? () from /usr/lib/x86_64-linux-gnu/libstdc++.so.6
            // #6  0x00007fd2bcc1b064 in start_thread (arg=0x7fd2bc045700) at pthread_create.c:309
            // #7  0x00007fd2bc12e62d in clone () at ../sysdeps/unix/sysv/linux/x86_64/clone.S:111

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


#endif // THREADPOOL_H
