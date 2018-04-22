#include <mutex>
#include <condition_variable>
#include "gmock/gmock.h"
#include "threadpool.h"

// "Separating threading logic from application logic"

using namespace std;
using namespace testing;

class AThreadPool : public Test
{
    public:
        ThreadPool pool;
        mutex m;
};

TEST_F(AThreadPool, HasNoWorkOnCreation) {
   ASSERT_FALSE(pool.hasWork());
}

TEST_F(AThreadPool, HasWorkAfterAdd) {
   pool.add(Work{});
   ASSERT_TRUE(pool.hasWork());
}

TEST_F(AThreadPool, AnswersWorkAddedOnPull) {
   pool.add(Work{1});
   auto work = pool.pullWork();
   
   ASSERT_THAT(work.id(), Eq(1));
}

TEST_F(AThreadPool, PullsElementsInFIFOOrder) {
   pool.add(Work{1});
   pool.add(Work{2});
   auto work = pool.pullWork();
   
   ASSERT_THAT(work.id(), Eq(1));
}

TEST_F(AThreadPool, HasNoWorkAfterLastElementRemoved) {
   pool.add(Work{});
   pool.pullWork();
   ASSERT_FALSE(pool.hasWork());
}

TEST_F(AThreadPool, HasWorkAfterWorkRemovedButWorkRemains) {
   pool.add(Work{});
   pool.add(Work{});
   pool.pullWork();
   ASSERT_TRUE(pool.hasWork());
}


#if 0

// code/c9/6/ThreadPoolTest.cpp
//
// <problem-1>
// Our test runs successfully...sometimes. We need only glance at our
// deliberately simplistic implementation to know it’s a problem. Were you to
// create a command-line script that repeatedly runs the tests, you would find
// that our new threaded test crashes every once in a while with a segmentation
// fault. We’ll investigate another approach: forcing failure directly from the
// test itself.
//
// NOTE seg fault? For me, ran 1000 times and no luck to see this reproduced.

TEST_F(AThreadPool, PullsWorkInAThread) {
   pool.start();
   condition_variable wasExecuted;
   bool wasWorked{0};
   Work work{[&] { 
      unique_lock<mutex> lock(m); 
      wasWorked = true;
      wasExecuted.notify_all(); 
   }};

   pool.add(work);

   unique_lock<mutex> lock(m);
   ASSERT_TRUE(wasExecuted.wait_for(lock, chrono::milliseconds(100), 
         [&] { return wasWorked; }));
}


// code/c9/7/ThreadPoolTest.cpp
//
// to demonstrate that the worker thread can pull and execute multiple work
// items from the queue.

TEST_F(AThreadPool, ExecutesAllWork) {
   pool.start();
   unsigned int count{0};
   unsigned int NumberOfWorkItems{3};
   condition_variable wasExecuted;
   Work work{[&] { 
      std::unique_lock<std::mutex> lock(m); 
      ++count;
      wasExecuted.notify_all(); 
   }};

   for (unsigned int i{0}; i < NumberOfWorkItems; i++)
      pool.add(work);

   unique_lock<mutex> lock(m);
   ASSERT_TRUE(wasExecuted.wait_for(lock, chrono::milliseconds(100), 
         [&] 
         { 
         // cout << "count: " << count << endl;
         return count == NumberOfWorkItems; 
         }));
}

#endif


// c9/9/ThreadPoolTest.cpp

class AThreadPoolAddRequest : public Test
{
    public:
        ThreadPool pool;
        mutex m;
        condition_variable wasExecuted;
        unsigned int count{0};

        vector<shared_ptr<thread>> threads;

        void SetUp() override
        {
            pool.start();
        }

        void TearDown() override
        {
            for ( auto &t : threads )
                t->join();
        }

        void incrementCountAndNotify()
        {
            unique_lock<mutex> lock(m);
            ++count;
            wasExecuted.notify_all();
        }

        void waitForCountAndFailOnTimeout(unsigned int expectedCount)
        {
            unique_lock<mutex> lock(m);
            ASSERT_TRUE(wasExecuted.wait_for(lock, chrono::milliseconds(100),
                        [&] {return count == expectedCount;}));
        }
};

// refactor tests. 1 client and 1 thread in a pool
TEST_F(AThreadPoolAddRequest, PullsWorkInAThread)
{
    Work work{ [&] { incrementCountAndNotify(); }};
    unsigned int NumberOfWorkItems{1};

    pool.add(work);
    waitForCountAndFailOnTimeout(NumberOfWorkItems);
}

// 1 client and 1 thread in a pool
TEST_F(AThreadPoolAddRequest, ExecutesAllWork) {
   Work work{[&] { incrementCountAndNotify(); }};
   unsigned int NumberOfWorkItems{3};

   for (unsigned int i{0}; i < NumberOfWorkItems; i++)
      pool.add(work);

   waitForCountAndFailOnTimeout(NumberOfWorkItems);
}


// <problem-4>
// Creating Client Threads in the Test
//
// Our tests aren’t simply failing; they are generating segmentation faults.
// Concurrent modification of the work queue is the likely suspect. 
//
// NOTE seg fault? For me, ran 1000 times and no luck to see this reproduced.

// 200 client and 1 thread in a pool
TEST_F(AThreadPoolAddRequest, HoldsUpUnderClientStress) 
{
   Work work{[&] { incrementCountAndNotify(); }};
   unsigned int NumberOfWorkItems{10};
   unsigned int NumberOfThreads{200};

   for (unsigned int i{0}; i < NumberOfThreads; ++i)
   {
       threads.push_back(
               make_shared<thread>(
                   [&] {
                   for (unsigned int i{0}; i < NumberOfWorkItems; ++i)
                     pool.add(work);
                     }
                   ));
   }

   waitForCountAndFailOnTimeout(NumberOfThreads * NumberOfWorkItems);
}


// To solve <problem-4>, introduce mutex on IFs


// Creating Multiple Threads in the ThreadPool
//
// The test DispatchesWorkToMultipleThreads demonstrates that client
// code can now start a specified number of threads. To verify that the
// ThreadPool indeed processes work in separate threads, we first update
// our work callback to add a thread if its ID is unique. Our assertion
// compares the thread count specified to the number of unique threads
// processed.
//        
// <problem-5>
// Can we ever know whether we’ve addressed all concurrency holes? Introduce
// more threads in pool.
//
// The worker function seems to remain the only code with any such potential. In
// worker, we loop until there is work; each time through the loop establishes
// and immediately releases a lock within hasWork. Once there is work, the loop
// exits, and control falls through to the statement pullWork().execute(). What
// if, during this short span, another thread has grabbed work?
//
// Otherwise, one of threads fails as pullWork() calls back() when queue is
// empty and which is undefined.
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
//
// So have to add a check in pullWork().


class AThreadPoolWithMultipleThreads: public Test
{
    public:
        ThreadPool pool;
        mutex m;
        mutex ifm;
        condition_variable wasExecuted;
        unsigned int count{0};

        vector<shared_ptr<thread>> client_threads;
        set<thread::id> worker_threads;

        // removed since to set num of worker thread in the test
        // void SetUp() override
        // {
        //     pool.start();
        // }

        void TearDown() override
        {
            for ( auto &t : client_threads )
                t->join();
        }

        void incrementCountAndNotify()
        {
            unique_lock<mutex> lock(m);
            ++count;
            wasExecuted.notify_all();
        }

        void waitForCountAndFailOnTimeout(unsigned int expectedCount)
        {
            unique_lock<mutex> lock(m);

            // ASSERT_TRUE(wasExecuted.wait_for(lock, chrono::milliseconds(100),
            //            [&] {return count == expectedCount;}));

            wasExecuted.wait_for(lock, chrono::milliseconds(1000),
                        [&] {return count == expectedCount;});
        }

        void addThreadIfUnique(const thread::id &id)
        {
            // Q: okay to use like this?
            unique_lock<mutex> block(ifm);
            // lock_guard<mutex> block(ifm);
            // cout << "tid : " << id << endl;
            // cout << "." << endl;
            worker_threads.insert(id);
        }

        size_t numberOfThreadsProcessed() 
        {
            return worker_threads.size();
        }
};

// 500 work items, 2 workers

TEST_F(AThreadPoolWithMultipleThreads, DispatchesWorkToMultipleThreadsOneClient) {
   // number of worker threads in pool
   unsigned int numberOfThreads{2};
   pool.start(numberOfThreads);
   Work work{[&] { 
      addThreadIfUnique(this_thread::get_id());
      incrementCountAndNotify();
   }};
   unsigned int NumberOfWorkItems{500};

   for (unsigned int i{0}; i < NumberOfWorkItems; i++)
      pool.add(work);

   waitForCountAndFailOnTimeout(NumberOfWorkItems);
   EXPECT_THAT(numberOfThreadsProcessed(), Eq(numberOfThreads));
}

//  500x100 work items, 2 workers

TEST_F(AThreadPoolWithMultipleThreads, DispatchesWorkToMultipleThreadsMultipleClient) {
   unsigned int numberOfWorkerThreads{2};
   pool.start(numberOfWorkerThreads);
   Work work{[&] { 
      addThreadIfUnique(this_thread::get_id());
      incrementCountAndNotify();
   }};

   unsigned int NumberOfWorkItems{500};

   // <problem-8>
   // okay when:
   // unsigned int NumberOfClientThreads{10};
   //
   // void waitForCountAndFailOnTimeout(unsigned int expectedCount)
   // wasExecuted.wait_for(lock, chrono::milliseconds(100),
   //
   // hangs when runs in run.py:
   // unsigned int NumberOfClientThreads{100};
   //
   // void waitForCountAndFailOnTimeout(unsigned int expectedCount)
   // wasExecuted.wait_for(lock, chrono::milliseconds(100),
   //
   // when increase time, passes all.
   // wasExecuted.wait_for(lock, chrono::milliseconds(1000),
   //
   // Q: So more work items and needs more time to process. Okay, then when
   // wait_for() returns, does it hang?
   
   unsigned int NumberOfClientThreads{100};

   for (unsigned int i{0}; i < NumberOfClientThreads; ++i)
   {
       client_threads.push_back(
               make_shared<thread>(
                   [&] {
                   for (unsigned int i{0}; i < NumberOfWorkItems; ++i)
                     pool.add(work);
                     }
                   ));
   }

   waitForCountAndFailOnTimeout(NumberOfClientThreads * NumberOfWorkItems);

   // <problem-7>
   //
   // use std::set since worker threads divides the total works and set
   // removes duplicates.
   //
   // (Unfortunately, this test has the potential to fail on the rare
   // occasion that one of the threads processes all of the work items. The
   // exercise of eliminating this potential for sporadic failure is left
   // to the reader.) 
   //
   // falis on:
   //
   // EXPECT_THAT(numberOfThreadsProcessed(), Eq(numberOfWorkerThreads));
   //
   // HOW to divide works between worker threads??

   EXPECT_THAT(numberOfThreadsProcessed(), Eq(numberOfWorkerThreads));
}
