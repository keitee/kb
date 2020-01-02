#include <algorithm>
#include <deque>
#include <future>
#include <iostream>
#include <memory>
#include <vector>

#include "gmock/gmock.h"

#include "eventloop.h"

using namespace std;
using namespace testing;

// ={=========================================================================

void w1(int &v)
{
  v += 1;
}

// no work since do not run event loop.
TEST(EventLoop, checkNoWorkDone)
{
  int value{};

  EventLoop loop;

  // auto w1 = [&]()
  // {
  //   value += 1;
  // };

  for (int i = 0; i < 10; ++i)
    loop.invokeMethod(std::bind(w1, value));

  EXPECT_THAT(value, 0);
}

// no work
TEST(EventLoop, checkWorkDone)
{
  int value{};

  EventLoop loop;

  auto f1 = std::async(std::launch::async, [&]() {
    std::this_thread::sleep_for(chrono::milliseconds(300));

    std::cout << "post works to event loop" << std::endl;

    for (int i = 0; i < 10; ++i)
      loop.invokeMethod(std::bind(w1, value));

    loop.flush();
    loop.quit(0);
  });

  loop.run();

  EXPECT_THAT(value, 10);

  /*
  // run event loop
  loop.run();

  {
    int value{};
    std::shared_ptr<ThreadedDispatcher> td =
      std::make_shared<ThreadedDispatcher>();

    td->post(std::bind(work1, std::ref(value), 10));

    std::this_thread::sleep_for(chrono::seconds(1));
    EXPECT_THAT(value, 10);
  }
}

  // more work
  {
    int value{};
    std::shared_ptr<ThreadedDispatcher> td =
      std::make_shared<ThreadedDispatcher>();

    td->post(std::bind(work1, std::ref(value), 10));
    td->post(std::bind(work1, std::ref(value), 10));
    td->post(std::bind(work1, std::ref(value), 10));
    td->post(std::bind(work1, std::ref(value), 10));

    std::this_thread::sleep_for(chrono::seconds(1));
    EXPECT_THAT(value, 40);
  }
}
*/
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
