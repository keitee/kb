#include <mutex>
#include <condition_variable>
#include "gmock/gmock.h"
#include "threadpool.h"

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

// TEST_F(AThreadPool, PullsWorkInAThread)
// {
//     pool.start();
//     condition_variable wasExecuted;
// 
//     unsigned int count{0};
//     unsigned int numberOfWorkItems{3};
// 
//     Work work{
//         [&] {
//             unique_lock<mutex> lock(m);
//             ++count;
//             wasExecuted.notify_all();
//         }
//     };
// 
//     for( unsigned int i{0}; i < numberOfWorkItems; ++i )
//         pool.add(work);
// 
//     unique_lock<mutex> lock(m);
//     ASSERT_TRUE(wasExecuted.wait_for(lock, chrono::milliseconds(100),
//                 [&] {return count == numberOfWorkItems;}));
// }


// We need to hit things harder from the test so that it fails every time. A
// quick attempt at bumping up the number of work items added to the loop
// doesn’t appear to make much difference. Our test needs to add work items from
// multiple threads created by the test itself.
//
// Q: Why does this hit harder? Mean that every TEST_F() creates a thread as
// well?

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

TEST_F(AThreadPoolAddRequest, PullsWorkInAThread)
{
    Work work{ [&] { incrementCountAndNotify(); }};
    unsigned int NumberOfWorkItems{1};

    pool.add(work);
    waitForCountAndFailOnTimeout(NumberOfWorkItems);
}

TEST_F(AThreadPoolAddRequest, ExecutesAllWork) {
   Work work{[&] { incrementCountAndNotify(); }};
   unsigned int NumberOfWorkItems{3};

   for (unsigned int i{0}; i < NumberOfWorkItems; i++)
      pool.add(work);

   waitForCountAndFailOnTimeout(NumberOfWorkItems);
}

// <problem 2>
//
// Our tests aren’t simply failing; they are generating segmentation faults.
// Concurrent modification of the work queue is the likely suspect. 
//
// TN: not seen yet.

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
            ASSERT_TRUE(wasExecuted.wait_for(lock, chrono::milliseconds(100),
                        [&] {return count == expectedCount;}));
        }

        // <problem>
        //
        // The test DispatchesWorkToMultipleThreads demonstrates that client
        // code can now start a specified number of threads. To verify that the
        // ThreadPool indeed processes work in separate threads, we first update
        // our work callback to add a thread if its ID is unique. Our assertion
        // compares the thread count specified to the number of unique threads
        // processed.
    
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
        // HOW??
        //
        // TN: adding a line like below fixes this though. or when increase
        // # of client threads.

        void addThreadIfUnique(const thread::id &id)
        {
            // Q: okay to use like this?
            // unique_lock<mutex> block(m);
            lock_guard<mutex> block(ifm);
            // cout << "tid : " << id << endl;
            // cout << "." << endl;
            worker_threads.insert(id);
        }

        size_t numberOfThreadsProcessed() 
        {
            return worker_threads.size();
        }
};


TEST_F(AThreadPoolWithMultipleThreads, DispatchesWorkToMultipleThreads) {
   unsigned int numberOfWorkerThreads{2};
   pool.start(numberOfWorkerThreads);
   Work work{[&] { 
      addThreadIfUnique(this_thread::get_id());
      incrementCountAndNotify();
   }};

   unsigned int NumberOfWorkItems{500};
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
   EXPECT_THAT(numberOfThreadsProcessed(), Eq(numberOfWorkerThreads));
}

