#ifndef MESSAGE_H_
#define MESSAGE_H_

//
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
//
// most of the time you’ll be wanting to handle a message.
//
// It’s a template, and the message type isn’t deducible, so you must
// specify which message type to handle and pass in a function (or
// callable object) to handle it. 
//
// handle() itself passes the queue, the current dispatcher object, and
// the handler function to a new instance of the TemplateDispatcher class
// template, to handle messages of the specified type.
// 
// Your message processing stops when you’ve successfully handled a
// message, so that you can wait for a different set of messages next
// time. If you do get a match for the specified message type, the
// supplied function is called rather than throwing an exception (although
// the handler function may throw an exception itself). 
//
// If you don’t get a match, you chain to the previous dispatcher. In the
// first instance, this will be a dispatcher, but
//  
// if you chain calls to handle() to allow multiple types of messages to
// be handled, this may be a prior instantiation of TemplateDispatcher<>,
// which will in turn chain to the previous handler if the message doesn’t
// match. 
//
// Because "any of the handlers might throw an exception" (including the
// dispatcher’s default handler for close_queue messages), the destructor
// must once again be declared noexcept(false) f.  
//
// This simple framework allows you to push any type of message on the
// queue and then selectively match against messages you can handle on the
// receiving end. It also allows you to pass around a reference to the
// queue for pushing messages on, while keeping the receiving end private.
//
// note:
// So in every state function, member function, build a chain of temporary
// of TemplateDispatcher and this chain is a list of messages that this
// state can handle. messages that are not handled are swallowed. 


#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

namespace messaging
{

  struct message_base
  {
    // shall know why `virtual` is necessary
    virtual ~message_base() {}
  };

  template<typename T>
    struct wrapped_message: message_base
  {
    explicit wrapped_message(T const& contents):
      contents_(contents)
    {}

    T contents_;
  };


  class queue
  {
    public:
      template<typename T>
        void push(T const& message)
        {
          std::lock_guard<std::mutex> lock(m_);

          // <point> accepts `any` type of message
          //
          // input message type is normal struct and is not under inheritance of
          // message_base since wrapper_message wraps it as T

          mq_.push(std::make_shared<wrapped_message<T>>(message));

          // why since notify_one() also works?
          // cv_.notify_all();
          cv_.notify_one();
        }

      std::shared_ptr<message_base> wait_and_pop()
      {
        std::unique_lock<std::mutex> lock(m_);

        cv_.wait(lock, [this](){return !mq_.empty();});

        auto message = mq_.front();
        mq_.pop();

        return message;
      }

    private:
      std::mutex m_;
      std::condition_variable cv_;
      std::queue<std::shared_ptr<message_base>> mq_;
  };

  // sepcial event to end fsm
  class close_queue
  {
  };


  // <point> chaining
  //
  // this action function to event builds chains of dispacher object:
  //
  // void wait_for_action()
  // {
  //   incoming_.wait() // which returns dispatcher() and calls handle() on it
  //     .handle<item_1_purchased>
  //     (
  //      [&](item_1_purchased const& message)
  //      {
  //      }
  //     )  // which returns TDispatcher()
  //     .handle<item_2_purchased>
  //     (
  //      [&](item_2_purchased const& message)
  //      {
  //      }
  //     )
  //     .handle<item_3_purchased>
  //     (
  //      [&](item_3_purchased const& message)
  //      {
  //      }
  //     );
  // }
  // 
  // three unnamed temporary objects:
  //
  // dispatcher                           // object 1, item_1_purchased  
  //  .TDispatcher<item_1_purchased>      // object 2, item_1_purchased
  //  .TDispatcher<item_2_purchased>      // object 3, item_2_purchased
  //  .TDispatcher<item_3_purchased>      // object 4, item_3_purchased
  // 
  // dtor gets called from 3, 2, 1 and wait() happens one of chains which is
  // determined by chained_ flag; inner most object will have it false which
  // menas this will call wait and the rest of chain will have it true.
  //
  // dispatch goes back from inner to outer to see if incomping message(event)
  // matches up with object whcih has a message type.
  //
  //
  // <point> dtor
  //
  // the work is done here in dtor which is to wait and dispatch a message.
  //
  //
  // <point> dispatch chain
  //
  // run through the chain to find a match and if there is, call the callable
  // and return true and search ends. If not, input message has no effect and
  // keeps on waiting. 
  //
  //
  // <point> close_queue exception
  //
  // eiter raise `close_queue` exception to end or say that a message it not
  // handled by returning false in dispatch() 
  //
  // if incoming message is `close_queue` then raise exception and this is
  // why `all` dtor of dispatchers is defined as `noexcept(false)` since it can
  // be raised at any point of fsm and otherwise see exception mismatch and will
  // be aborted.

  template<typename PreviousDispatcher,
    typename Message,
    typename Function>
      class TemplateDispatcher
      {
        public:
          // cxx-no-copy-support
          TemplateDispatcher(TemplateDispatcher const&) = delete;
          TemplateDispatcher& operator=(TemplateDispatcher const&) = delete;

          // it seems not necessary. 
          // why own move ctor when compiler will make one?

          TemplateDispatcher(TemplateDispatcher&& other):
            q_(other.q_), prev_(other.prev_), 
            f_(std::move(other.f_)), chained_(other.chained_)
            {
              std:: cout << "TemplateDispatcher::move" << std::endl;
              other.chained_ = true;
            }

          template<typename OtherDispatcher, typename OtherMessage, typename OtherFunction>
            friend class TemplateDispatcher;

          // "prev_->chained_" requires acccess to private since
          // "PreviousDispatcher" is different type. Hence needs friend
          // relationship.

          TemplateDispatcher(queue* q, PreviousDispatcher* prev, Function&& f):
            q_(q), prev_(prev), f_(std::forward<Function>(f)), chained_(false)
            {
              prev_->chained_ = true;
            }

          template<typename OtherMessage, typename OtherFunction>
            TemplateDispatcher<TemplateDispatcher, OtherMessage, OtherFunction>
            handle(OtherFunction&& f)
            {
              // std:: cout << "TemplateDispatcher::handle" << std::endl;
              return 
                TemplateDispatcher<TemplateDispatcher, OtherMessage, OtherFunction>(
                    q_, this, std::forward<OtherFunction>(f));
            }

          // ~TemplateDispatcher()
          ~TemplateDispatcher() noexcept(false)
          {
            if (!chained_)
            {
              // std:: cout << "TemplateDispatcher::~TemplateDispatcher::wait" << std::endl;
              wait_and_dispatch();
            }
          }

        private:
          queue* q_;
          PreviousDispatcher* prev_;
          Function f_;
          bool chained_;

          bool dispatch(std::shared_ptr<message_base> const& message)
          {
            // {
            //   wrapped_message<Message>* wrapper = 
            //     dynamic_cast<wrapped_message<Message>*>(message.get());
            //   if (wrapper == nullptr)
            //     std::cout << "wrapper is null" << std::endl;
            //   else
            //   {
            //     std:: cout << "TemplateDispatcher::dispatch: " 
            //       << (wrapper->contents_).get_name() << std::endl;
            //   }

            //   wrapped_message<close_queue>* xxx = 
            //     dynamic_cast<wrapped_message<close_queue>*>(message.get());
            //   if (xxx == nullptr)
            //     std::cout << "xxx is null" << std::endl;
            //   else 
            //     std::cout << "xxx is close" << std::endl;

            // }

            if (wrapped_message<Message>* wrapper = 
                dynamic_cast<wrapped_message<Message>*>(message.get()))
            {
              f_(wrapper->contents_);
              return true;
            }
            else
            {
              return prev_->dispatch(message);
            }
          }

          void wait_and_dispatch()
          {
            for (;;)
            {
              auto message = q_->wait_and_pop();
              if (dispatch(message))
                break;
            }
          }

      }; // TemplateDispatcher


  class dispatcher
  {
    public:
      // no copy support
      dispatcher(dispatcher const&) = delete;
      dispatcher& operator=(dispatcher const&) = delete;

      // ctors
      explicit dispatcher(queue* q) : q_(q), chained_(false) {}

      dispatcher(dispatcher&& other):
        q_(other.q_), chained_(other.chained_)
        {
          std:: cout << "dispatcher::move" << std::endl;
          other.chained_ = true;
        }

      ~dispatcher() noexcept(false)
      {
        if (!chained_)
        {
          // std:: cout << "dispatcher::~dispatcher::wait" << std::endl;
          wait_and_dispatch();
        }
      }

      template<typename Message, typename Function>
        TemplateDispatcher<dispatcher, Message, Function>
        handle(Function&& f)
        {
          // std:: cout << "dispatcher::handle" << std::endl;
          return
            TemplateDispatcher<dispatcher, Message, Function>(
                q_, this, std::forward<Function>(f));
        }

    private:
      queue* q_;
      bool chained_;

      template<typename Dispatcher, typename Message, typename Function>
        friend class TemplateDispatcher;

      bool dispatch(std::shared_ptr<message_base> const& message)
      {
        if (dynamic_cast<wrapped_message<close_queue>*>(message.get()))
        {
          // std:: cout << "dispatcher::dispatch::throw" << std::endl;
          // `close_queue` is not exception
          throw close_queue();
        }

        return false;
      }

      void wait_and_dispatch()
      {
        for (;;)
        {
          // std:: cout << "dispatcher::wait_and_dispatch" << std::endl;
          auto message = q_->wait_and_pop();
          dispatch(message);
        }
      }

  };  // dispatcher


  // sender do not own q and copying of sender just copies the pointer to the
  // queue.
  //
  // o sender is a wrapper to queue pointer and uses implicit type conversion 
  //   from receiver, cxx-conversion-op

  class sender
  {
    public: 
      explicit sender(queue* q): q_(q) {}
      sender(): q_(nullptr) {}

      template<typename T>
        void send(T const& message)
        {
          if (q_)
            q_->push(message);
        }

    private:
      queue* q_;

  };  // sender

  class receiver
  {
    public:
      // cxx-conversion-op which converts receiver to sender
      operator sender()
      {
        return sender(&q_);
      }

      dispatcher wait()
      {
        return dispatcher(&q_);
      }

    private:
      queue q_;

  };  // receiver

} // namespace


#endif // MESSAGE_H_
