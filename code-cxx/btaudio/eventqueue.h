/*
 *
*/

#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include <deque>
#include <mutex>
#include <condition_variable>

namespace BleAudio
{
  /*
  class Message
  {
    public:
      virtual ~Message() {}
  }; 

  template <typename T>
    class WrappedMessage : public Message
  {
    public:
      explicit WrappedMessage(T const &message) : message_(message) {}

    private:
      T message_;
  };

  class queue
  {
    public:
      template <typename T>
        void push(T const &message)
        {
          std::lock_guard<std::mutex> lock(m_);

          q_.push_back(std::make_shared<WrappedMessage<T>>(message));
          cv_.notify_all();
        }

      std::shared_ptr<Message> wait_and_pop()
      {
        std::unique_lock<std::mutex> lock(m_);

        cv_.wait(lock, [&]{ return !q_.empty();});
        auto message = std::move(q_.front());
        q_.pop_front();
        lock.unlock();

        return message;
      }

    private:
      std::mutex m_;
      std::condition_variable cv_;
      std::deque<std::shared_ptr<Message>> q_;
  };
  */

  template <typename T>
    class queue
    {
      public:
        void push(T const &message)
        {
          std::lock_guard<std::mutex> lock(m_);

          q_.push_back(message);
          cv_.notify_all();
        }

        T wait_and_pop()
        {
          std::unique_lock<std::mutex> lock(m_);

          cv_.wait(lock, [&]{ return !q_.empty();});
          auto message = std::move(q_.front());
          q_.pop_front();
          lock.unlock();

          return message;
        }

      private:
        std::mutex m_;
        std::condition_variable cv_;
        std::deque<T> q_;
    };
} // namesapce

#endif // EVENTQUEUE_H
