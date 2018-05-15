#ifndef MESSAGE_H_
#define MESSAGE_H_

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


  ///////////////////////////////////////////////////////////////////
  //
      
  class close_queue
  {};

  template<typename PreviousDispatcher,typename Msg,typename Func>
    class TemplateDispatcher
    {
      queue* q_;
      PreviousDispatcher* prev_;
      Func f_;
      bool chained_;

      TemplateDispatcher(TemplateDispatcher const&)=delete;
      TemplateDispatcher& operator=(TemplateDispatcher const&)=delete;

      template<typename Dispatcher,typename OtherMsg,typename OtherFunc>
        friend class TemplateDispatcher;

      void wait_and_dispatch()
      {
        for(;;)
        {
          auto msg=q_->wait_and_pop();
          if(dispatch(msg))
            break;
        }
      }

      bool dispatch(std::shared_ptr<message_base> const& msg)
      {
        if(wrapped_message<Msg>* wrapper=
            dynamic_cast<wrapped_message<Msg>*>(msg.get()))
        {
          f_(wrapper->contents_);
          return true;
        }
        else
        {
          return prev_->dispatch(msg);
        }
      }
      public:
      TemplateDispatcher(TemplateDispatcher&& other):
        q_(other.q_),prev_(other.prev_),f_(std::move(other.f_)),
        chained_(other.chained_)
      {
        other.chained_=true;
      }

      TemplateDispatcher(queue* q_,PreviousDispatcher* prev_,Func&& f_):
        q_(q_),prev_(prev_),f_(std::forward<Func>(f_)),chained_(false)
      {
        prev_->chained_=true;
      }

      template<typename OtherMsg,typename OtherFunc>
        TemplateDispatcher<TemplateDispatcher,OtherMsg,OtherFunc>
        handle(OtherFunc&& of)
        {
          return TemplateDispatcher<
            TemplateDispatcher,OtherMsg,OtherFunc>(
                q_,this,std::forward<OtherFunc>(of));
        }

      ~TemplateDispatcher() noexcept(false)
      {
        if(!chained_)
        {
          wait_and_dispatch();
        }
      }
    };

  class dispatcher
  {
    public:
      dispatcher(dispatcher &&other):
        q_(other.q_), chained_(other.chained_) 
    {
      other.chained_ = true;
    }
      explicit dispatcher(queue *q):
        q_(q), chained_(false) {}


      // most of the time you’ll be wanting to handle a message.
      //
      // It’s a template, and the message type isn’t deducible, so you must
      // specify which message type to handle and pass in a function (or
      // callable object) to handle it. 
      //
      // handle() itself passes the queue, the current dispatcher object, and
      // the handler function to a new instance of the TemplateDispatcher class
      // template, to handle messages of the specified type.

      template<typename Message, typename Func>
        TemplateDispatcher<dispatcher, Message, Func>
        handle(Func &&f)
        {
          return TemplateDispatcher<dispatcher, Message, Func>(
              q_, this, std::forward<Func>(f));
        }

      // the work is done in the destructor. In this case, that work
      // consists of waiting for a message and dispatching it.
      ~dispatcher() noexcept(false)
      {
        if (!chained_)
          wait_and_dispatch();
      }

    private:
      queue *q_;
      bool chained_;

      // note: WHY private?
      dispatcher(const dispatcher &) = delete;
      dispatcher &operator=(const dispatcher &) = delete;

      template<
        typename Dispatcher,
                 typename Msg,
                 typename Func>
                   friend class TemplateDispatcher;

      void wait_and_dispatch()
      {
        for (;;)
        {
          auto msg = q_->wait_and_pop();
          dispatch(msg);
        }
      }

      // it checks whether the message is a close_queue message and throws an
      // exception if it is; otherwise, it returns false to indicate that the
      // message was unhandled. This close_queue exception is why the destructor
      // is marked noexcept(false);
      //
      // note: this exception is used to end run()
      //
      // Q: when is it going to be called to end?
      //
      // so either raise exception to end or say that message is not handled.

      bool dispatch(const std::shared_ptr<message_base> &msg)
      {
        if (dynamic_cast<wrapped_message<close_queue>*>(msg.get()))
        {
          throw close_queue();
        }
        return false;
      }
  };


  ///////////////////////////////////////////////////////////////////
  // Copying instances of sender just copies the pointer to the queue rather
  // than the queue itself.

  class sender
  {
    public:
      sender(): q_(nullptr) {}
      explicit sender(queue *q) : q_(q) {}

      template<typename T>
        void send(const T& message)
        {
          if(q_)
            q_->push(message);
        }

    private:
      queue *q_;
  };

  ///////////////////////////////////////////////////////////////////
  // Receiving messages is a bit more complicated. Not only do you have to wait
  // for a message from the queue, but 
  //
  // you also have to check to see if the type matches any of the message types
  // being waited on and call the appropriate handler function. 
  //
  // Whereas a sender just references a message queue, a receiver owns it.
  
  class receiver
  {
    public:
      // note: cxx-conversion-op which convert receiver to sender
      operator sender()
      {
        return sender(&q_);
      }

      // The complexity of doing the message dispatch starts with a call to
      // wait(). This creates a dispatcher object that references the queue from
      // the receiver. 

      dispatcher wait()
      {
        return dispatcher(&q_);
      }

    private:
      queue q_;
  };

}

#endif // MESSAGE_H_
