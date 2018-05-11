// appendix C A message-passing framework and complete ATM example
//
// Back in chapter 4, I presented an example of sending messages between threads
// using a message-passing framework, using a simple implementation of the code
// in an ATM as an example. What follows is the complete code for this example,
// including the message-passing framework.
//
// Listing C.1 shows the message queue. It stores a list of messages as pointers
// to a base class; the specific message type is handled with a template class
// derived from that base class. Pushing an entry constructs an appropriate
// instance of the wrapper class and stores a pointer to it; popping an entry
// returns that pointer. Because the message_base class doesn’t have any member
// functions, the popping thread will need to cast the pointer to a suitable
// wrapped_message<T> pointer before it can access the stored message.

#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

namespace messaging
{
  ///////////////////////////////////////////////////////////////////
  struct message_base
  {
    virtual ~message_base() {}
  };

  template<typename T>
    struct wrapped_message: message_base
  {
    T contents_;
    explicit wrapped_message(const T &contents):
      contents_(contents)
    {}
  };

  ///////////////////////////////////////////////////////////////////
  class queue
  {
    public:
      template<typename T>
        void push(const T &message)
        {
          std::lock_guard<std::mutex> lock(m);

          mq.push(std::make_shared<wrapped_message<T>>(message));
          cv.notify_all();
        }

      std::shared_ptr<message_base> wait_and_pop()
      {
        std::unique_lock<std::mutex> lock(m);

        cv.wait(lock, [this]{return !mq.empty();});
        auto message = mq.front();
        mq.pop();
        return message;
      }

    private:
      std::mutex m;
      std::condition_variable cv;
      std::queue<std::shared_ptr<message_base>> mq;
  };

#if 0
  ///////////////////////////////////////////////////////////////////
  class sender
  {
    public:
      sender(): q(nullptr) {}
      explicit sender(queue *q) : q_(q) {}

      template<typename T>
        void send(const T& message)
        {
          if(q)
            q->push(message);
        }

    private:
      queue *q_;
  };

  ///////////////////////////////////////////////////////////////////
  class receiver
  {
    public:
      operator sender()
      {
        return sender(&q);
      }

      dispatcher wait()
      {
        return dispatcher(&q);
      }

    private:
      queue *q_;
  };
#endif
}
