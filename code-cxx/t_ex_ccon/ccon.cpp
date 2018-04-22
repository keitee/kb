#include <iostream>
#include <thread>
#include <future>

#include "gtest/gtest.h"

using namespace std;

int doSomething (char c)
{
    default_random_engine dre(c);
    uniform_int_distribution<int> id(10, 1000);

    for(int i=0; i<10; ++i)
    {
        this_thread::sleep_for(chrono::milliseconds( id(dre) ));
        cout.put(c).flush();
    }

    return c;
}


// ={=========================================================================
// cxx-thread

void hello()
{
    cout << "Hello Concurrent World\n";
}

TEST(CconThreadTest, HelloWorld)
{
    std::thread t(hello);
    t.join();
}

// +..+..+...+..+.+++++
// done

TEST(CconThreadTest, RunTwoThreads)
{
    std::thread t1([]{doSomething('.');});
    std::thread t2([]{doSomething('+');});
    t1.join();
    t2.join();
    cout << "\ndone" << endl;
}


// ={=========================================================================
// cxx-async,
//
// ends do_other_stuff
// ends find_the_answer
// the answer is 100

int find_the_answer()
{
    this_thread::sleep_for(chrono::milliseconds(100));
    std::cout << "ends find_the_answer" << std::endl;
    return 100;
}

void do_other_stuff()
{
    this_thread::sleep_for(chrono::seconds(20));
    std::cout << "ends do_other_stuff" << std::endl;
}

TEST(CconThreadTest, UseAsync)
{
    std::future<int> the_answer = std::async( find_the_answer );
    do_other_stuff();
    std::cout << "the answer is " << the_answer.get() << std::endl;
}


// ={=========================================================================
// cxx-async with no option

// starting func1() in background and func2() in foreground:
// ++++++++++..........
// result of func1()+func2(): 89


int func1()
{
    return doSomething('.');
}

int func2()
{
    return doSomething('+');
}

TEST(CconThreadTest, UseAsyncWithDefault)
{
    cout << "starting func1() in background"
        << " and func2() in foreground:" << endl;

    future<int> result1(std::async(func1));

    int result2 = func2();

    int result = result1.get() + result2;

    cout << "\nresult of func1()+func2(): " << result << endl;
}


// ={=========================================================================
// cxx-async with async option

// starting func1() in background and func2() in foreground:
// +..+..+...+..+.+++++
// result of func1()+func2(): 89

TEST(CconThreadTest, UseAsyncWithAsync)
{
    cout << "starting func1() in background"
        << " and func2() in foreground:" << endl;

    future<int> result1(std::async(launch::async, func1));

    int result2 = func2();

    int result = result1.get() + result2;

    cout << "\nresult of func1()+func2(): " << result << endl;
}


// ={=========================================================================
// cxx-async with async option

// starting 2 operations asynchronously
// 
// ..........++++++++++
// done

TEST(CconThreadTest, UseAsyncX)
{
    cout << "starting 2 operations asynchronously" << endl;

    // start two loops in the background printing characters . or +
    auto f1 = async([]{ doSomething('.'); });
    auto f2 = async([]{ doSomething('+'); });

    // if at least one of the background tasks is running
    if (f1.wait_for(chrono::seconds(0)) != future_status::deferred ||
        f2.wait_for(chrono::seconds(0)) != future_status::deferred) 
    {
        cout << "\ndeffered" << endl;

        // poll until at least one of the loops finished
        while (f1.wait_for(chrono::seconds(0)) != future_status::ready &&
               f2.wait_for(chrono::seconds(0)) != future_status::ready) 
        {
            //...;
            cout << "\nyield" << endl;
            this_thread::yield();  // hint to reschedule to the next thread
        }
    }
    cout.put('\n').flush();

    // wait for all loops to be finished and process any exception
    try {
        f1.get();
        f2.get();
    }
    catch (const exception& e) {
        cout << "\nEXCEPTION: " << e.what() << endl;
    }
    cout << "\ndone" << endl;
}

// ={=========================================================================

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
