#include <iostream>
#include <thread>
#include <future>
#include <list>
#include <mutex>
#include <queue>
#include <stack>
#include <exception>

// #include <boost/thread/shared_mutex.hpp>

#include "gmock/gmock.h"

#include "message.h"

using namespace std;
using namespace testing;


// ={=========================================================================
//

void PushItemsToQueue(messaging::queue &mq)
{
  mq.push(100);
}

TEST(MessageTest, PutAndPopSingleItem)
{
  messaging::queue mq;
  int value = 100;

  std::future<void> result 
    = std::async(launch::async, PushItemsToQueue, std::ref(mq));
  
  auto message = mq.wait_and_pop();

  // don't need to do it since wait_and_pop() do waits
  // result.get();

  ASSERT_THAT(dynamic_cast<messaging::wrapped_message<int>*>(message.get())->contents_,
      Eq(100));
}
