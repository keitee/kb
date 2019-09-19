#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <queue>
#include <future>

#include "gmock/gmock.h"

// to exercise cxx fsm code

using namespace testing;


// ={=========================================================================
/*
appendix C A message-passing framework and complete ATM example

Back in chapter 4, I presented an example of sending messages between threads
using a message-passing framework, using a simple implementation of the code in
an ATM as an example. What follows is the complete code for this example,
including the message-passing framework.

Listing C.1 shows the message queue. It stores a list of messages as pointers to
a base class; the specific message type is handled with a template class derived
from that base class. Pushing an entry constructs an appropriate instance of the
wrapper class and stores a pointer to it; popping an entry returns that pointer.

  o Because the message_base class doesn’t have any member functions, the
    popping thread will need to cast the pointer to a suitable
    wrapped_message<T> pointer before it can access the stored message.

key 1: template member function

  o why template member function push() than template class queue used above?
  
     template class approach allows to create queue class for any type.
     However, if want to support multiple types then have to have queue for
     each type:
  
     queue<int>, queue<double>, ..
  
    so template member function approach deduce type T when called and passes
    it to create wrapped_message. this support any type with signle queue.


key 2: polymorphic and pointer

  o why use message_base?
  
     wait_and_pop() returns shared_ptr but shared_ptr needs the known type but
     this function is normal function. (cannot be template or template member
     function). so use message_base which is fixed type.

  o why use wrapper_message<T>?

    struct message_A : public message_base
    {};

    struct message_B : public message_base
    {};

    or

    struct message_A
    {};

    struct message_B
    {};

points: use of polymorphic base, shared_ptr and template method

*/

namespace cxx_fsm
{
  // this is simple queue which suppors only single T type.

  namespace simple
  {
    template<typename T>
      class queue
      {
        public:
          void push(T const &message)
          {
            std::lock_guard<std::mutex> lock(m_);

            q_.emplace_back(message);
            cv_.notify_all();
          }

          T wait_and_pop()
          {
            std::unique_lock<std::mutex> lock(m_);

            // wait until q it not empty
            cv_.wait(lock, [&]{return !q_.empty();});

            auto message = std::move(q_.front());
            q_.pop_front();
            lock.unlock();

            return message;
          }

        private:
          std::condition_variable cv_;
          std::mutex m_;
          std::deque<T> q_;
      };
  } // namespace


  // this is a generic queue which supports any T type.
  
  namespace multiple
  {
    // like polymorphic base
    struct message_base
    {
      virtual ~message_base() {}
    };

    template<typename T>
      struct wrapped_message : public message_base
    {
      explicit wrapped_message(T const &message)
        : message_(message) {}

      T message_;
    };

    class queue
    {
      public:
        template<typename T>
          void push(T const &message)
          {
            std::lock_guard<std::mutex> lock(m_);

            // create new type of wrapped_message<T> whenever push()
            q_.push(std::make_shared<wrapped_message<T>>(message));
            cv_.notify_all();
          }

        std::shared_ptr<message_base> wait_and_pop()
        {
          std::unique_lock<std::mutex> lock(m_);

          cv_.wait(lock, [&]{ return !q_.empty();});
          auto message = q_.front();

          // *cxx-error* cuase infinite loop on the user when omits this
          q_.pop();
          lock.unlock();
          return message;
        }

      private:
        std::condition_variable cv_;
        std::mutex m_;
        // std::queue<T> q_;
        std::queue<std::shared_ptr<message_base>> q_;
    };
  } // namespace

  void push_items_to_queue(multiple::queue& q)
  {
    q.push(100);
  }


  // say, message is class and has member functions

  class message_1
  {
    public:
      explicit message_1():
        name_("message_1") {}

      std::string getName() { return name_; }

    private:
        std::string name_;
  };

  class message_2
  {
    public:
      explicit message_2():
        name_("message_2") {}

      std::string getName() { return name_; }

    private:
        std::string name_;
  };
} // namespce


// ={=========================================================================
// cxx-fsm-cxx

// send and get single message
TEST(Fsm, CxxUseMessage)
{
  using namespace cxx_fsm;

  // to avoid conflict with std::queue
  multiple::queue mq;

  std::async(std::launch::async, push_items_to_queue, std::ref(mq));

  auto message = mq.wait_and_pop();

  EXPECT_THAT(
      dynamic_cast<multiple::wrapped_message<int>*>(message.get())->message_,
      100);
}

// send and get various type of messages
TEST(Fsm, CxxUseMessageVariousType)
{
  using namespace cxx_fsm;

  multiple::queue mq;
  
  {
    mq.push(100);
    auto message = mq.wait_and_pop();

    EXPECT_THAT(
        dynamic_cast<multiple::wrapped_message<int>*>(message.get())->message_, 
        100);
  }

  {
    mq.push((double)100.0);
    auto message = mq.wait_and_pop();

    EXPECT_THAT(
        dynamic_cast<multiple::wrapped_message<double>*>(message.get())->message_, 
        100.0);
  }

  {
    std::string text{"string type"};
    mq.push(text);
    auto message = mq.wait_and_pop();

    EXPECT_THAT(
        dynamic_cast<multiple::wrapped_message<std::string>*>(message.get())->message_, 
        text);
  }

  {
    mq.push(message_1());
    auto message = mq.wait_and_pop();

    EXPECT_THAT(
        dynamic_cast<multiple::wrapped_message<message_1>*>(message.get())->message_.getName(),
       "message_1");
  }

  {
    mq.push(message_2());
    auto message = mq.wait_and_pop();

    // *cxx-error* have to cast it to right type and seg fault otherwise.
    // ex. wrapped_message<message_1>

    EXPECT_THAT(
        dynamic_cast<multiple::wrapped_message<message_2>*>(message.get())->message_.getName(),
       "message_2");
  }
}


// ={=========================================================================

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
