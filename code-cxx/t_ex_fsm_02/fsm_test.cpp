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
#include <chrono>
#include <condition_variable>
#include <queue>

#include "gtest/gtest.h"
#include "fsm.h"


// using namespace messaging;

///////////////////////////////////////////////////////////////////
// note:
// To make it simple, set T as int.

namespace fsm {
class queue
{
  public:
    template<typename T>
      void push(const T &message)
      {
        std::lock_guard<std::mutex> lock(m);

        // mq.push(std::make_shared<wrapped_message<T>>(message));
        mq.push(message);
        cv.notify_all();
      }

    // std::shared_ptr<message_base> wait_and_pop()
    // {
    //   std::unique_lock<std::mutex> lock(m);
    //
    //   cv.wait(lock, [this]{return !mq.empty();});
    //   auto message = mq.front();
    //   mq.pop();
    //   return message;
    // }

    // TODO: how to set return T?
    // T wait_and_pop()
    uint32_t wait_and_pop()
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
    // std::queue<std::shared_ptr<message_base>> mq;
    std::queue<uint32_t> mq;
};
}

static fsm::queue q;

static const uint32_t CONST_EXCEPTION_TO_END = 0xFFFF;

static void runner(fsm_instance_t *fsm)
{ 
  // std::cout << "fsm_thread: wait for a message" << std::endl;
  // auto msg = q.wait_and_pop();
  // std::cout << "fsm_thread: got a message" << std::endl;
  // VRM_FSM_Input(fsm, (uint16_t)E_ATM_INPUT_CARD_INSERTED);
  // std::cout << "fsm_thread: end" << std::endl;

  for (;;)
  {
    auto msg = q.wait_and_pop();
    std::cout << "fsm_thread: got a <" << msg << ">" << std::endl;

    if(msg == CONST_EXCEPTION_TO_END)
      break;

    fsm_post_event(fsm, msg, nullptr);
  }

  std::cout << "fsm_thread: end" << std::endl;
}

static void get_message()
{
  auto msg = q.wait_and_pop();
}

static void send_message(uint32_t message)
{
  q.push(message);
}


///////////////////////////////////////////////////////////////////
// fsm enums

typedef enum 
{
  E_ATM_INPUT_CARD_INSERTED,
  E_ATM_INPUT_DIGIT_PRESSED,
  E_ATM_INPUT_DIGIT_DONE,
  E_ATM_INPUT_PIN_VERIFIED,
  E_ATM_INPUT_WITHDRAW_PRESSED,
  E_ATM_INPUT_WITHDRAW_OKAY,
  E_ATM_INPUT_END
} E_ATM_INPUT;

typedef enum
{
  E_ATM_STATE_WAITING_FOR_CARD,   // current state
  E_ATM_STATE_CARD_INSERTED,
  E_ATM_STATE_GETTING_PIN,
  E_ATM_STATE_VERIFY_PIN,
  E_ATM_STATE_WAIT_FOR_ACTION,
  E_ATM_STATE_PROCESS_WITHDRAW,
  E_ATM_STATE_DONE_PROCESSING
} E_ATM_STATE;


///////////////////////////////////////////////////////////////////
// fsm state transition tables

static fsm_transition_tbl_t wait_for_card_tbl[] = {
  { E_ATM_INPUT_CARD_INSERTED, E_ATM_STATE_CARD_INSERTED },
  { FSM_INVALID_EVENT_HDL, FSM_INVALID_STATE_HDL }
};

static fsm_transition_tbl_t card_inserted_tbl[] = {
  { E_ATM_INPUT_DIGIT_PRESSED, E_ATM_STATE_GETTING_PIN },
  { FSM_INVALID_EVENT_HDL, FSM_INVALID_STATE_HDL }
};

static fsm_transition_tbl_t getting_pin_tbl[] = {
  { E_ATM_INPUT_DIGIT_PRESSED, E_ATM_STATE_GETTING_PIN },
  { E_ATM_INPUT_DIGIT_DONE, E_ATM_STATE_VERIFY_PIN },
  { FSM_INVALID_EVENT_HDL, FSM_INVALID_STATE_HDL }
};

static fsm_transition_tbl_t verify_pin_tbl[] = {
  { E_ATM_INPUT_PIN_VERIFIED, E_ATM_STATE_WAIT_FOR_ACTION },
  { FSM_INVALID_EVENT_HDL, FSM_INVALID_STATE_HDL }
};

static fsm_transition_tbl_t wait_for_action_tbl[] = {
  { E_ATM_INPUT_WITHDRAW_PRESSED, E_ATM_STATE_PROCESS_WITHDRAW },
  { FSM_INVALID_EVENT_HDL, FSM_INVALID_STATE_HDL }
};

static fsm_transition_tbl_t process_withdraw_tbl[] = {
  { E_ATM_INPUT_WITHDRAW_OKAY, E_ATM_STATE_DONE_PROCESSING },
  { FSM_INVALID_EVENT_HDL, FSM_INVALID_STATE_HDL }
};

static fsm_transition_tbl_t done_processing_tbl[] = {
  { E_ATM_INPUT_END, E_ATM_STATE_GETTING_PIN },
  { FSM_INVALID_EVENT_HDL, FSM_INVALID_STATE_HDL }
};


///////////////////////////////////////////////////////////////////
// fsm states

static fsm_state_t wait_for_card_state = {
   E_ATM_STATE_WAITING_FOR_CARD,
   (char *)"atm wait for card state",
   (fsm_action_func_t)NULL,
   wait_for_card_tbl
};

static fsm_state_t card_inserted_state = {
   E_ATM_STATE_CARD_INSERTED,
   (char *)"atm card inserted state",
   (fsm_action_func_t)NULL,
   card_inserted_tbl
};

static fsm_state_t getting_pin_state = {
   E_ATM_STATE_GETTING_PIN,
   (char *)"atm getting pin state",
   (fsm_action_func_t)NULL,
   getting_pin_tbl
};

static fsm_state_t verify_pin_state = {
   E_ATM_STATE_VERIFY_PIN,
   (char *)"atm verify pin state",
   (fsm_action_func_t)NULL,
   verify_pin_tbl
};

static fsm_state_t wait_for_action_state = {
   E_ATM_STATE_WAIT_FOR_ACTION,
   (char *)"atm wait for action state",
   (fsm_action_func_t)NULL,
   wait_for_action_tbl
};

static fsm_state_t process_withdraw_state = {
   E_ATM_STATE_PROCESS_WITHDRAW,
   (char *)"atm process withdraw state",
   (fsm_action_func_t)NULL,
   process_withdraw_tbl
};

static fsm_state_t done_processing_state = {
   E_ATM_STATE_DONE_PROCESSING,
   (char *)"atm done process state",
   (fsm_action_func_t)NULL,
   done_processing_tbl
};


///////////////////////////////////////////////////////////////////
// fsm states and events list

static fsm_state_t* atm_state_list[] = {
  &wait_for_card_state,
  &card_inserted_state,
  &getting_pin_state,
  &verify_pin_state,
  &wait_for_action_state,
  &process_withdraw_state,
  &done_processing_state
};


static fsm_event_t atm_event_list[] = {
  { E_ATM_INPUT_CARD_INSERTED, (char *)"card inserted event", nullptr},
  { E_ATM_INPUT_DIGIT_PRESSED, (char *)"digit pressed event", nullptr},
  { E_ATM_INPUT_DIGIT_DONE, (char *)"digit done event", nullptr},
  { E_ATM_INPUT_PIN_VERIFIED, (char *)"pin verified event", nullptr},
  { E_ATM_INPUT_WITHDRAW_PRESSED, (char *)"withdraw pressed event", nullptr},
  { E_ATM_INPUT_WITHDRAW_OKAY, (char *)"withdraw okay event", nullptr},
  { E_ATM_INPUT_END, (char *)"input endevent", nullptr}
};


///////////////////////////////////////////////////////////////////
// fsm instance

static fsm_instance_t atm_fsm = {
	"atm", atm_state_list, atm_event_list, NULL, NULL
};


///////////////////////////////////////////////////////////////////
// eafs

static std::string account;
static std::string pin;

// external -> card_inserted
static void CardInserted(fsm_instance_t* fsm, fsm_event_t* event)
{
  (void)fsm;
  (void)event;

  // state check
  // if( !fsm_is_valid_incoming_event( fsm, event->hdl ) )
  // {
  //   std::cout << "invalid event <" 
  //     << event->name << "in " << fsm->current_state->name << std::endl;
  //   return;
  // }

  std::cout << "--------------------------" << std::endl;
  std::cout << "event: <" << fsm_get_event_name(fsm, event->hdl) << ">"
    << " state: <" << fsm_get_current_state_name(fsm) << ">" << std::endl;

  std::cout << "atm::CardInserted:" << std::endl;
  std::cout << "atm::CardInserted:init account and pin" << std::endl;
}

static void DigitPressed(fsm_instance_t* fsm, fsm_event_t* event)
{
  (void)fsm;
  (void)event;

  if (event->hdl != E_ATM_INPUT_END)
  {
    unsigned const pin_length = 4;
    pin += "1";
    std::cout << "--------------------------" << std::endl;
    std::cout << "atm::DigitPressed: " << pin << std::endl;

    if (pin.length() == pin_length)
    {
      send_message((uint16_t)E_ATM_INPUT_DIGIT_DONE);
    }
  }
}

static void VerifyPin(fsm_instance_t* fsm, fsm_event_t* event)
{
  (void)fsm;
  (void)event;
  std::cout << "atm::VerifyPin:" << std::endl;
  send_message((uint16_t)E_ATM_INPUT_PIN_VERIFIED);
}

// external -> pin_verified
static void WaitForAction(fsm_instance_t* fsm, fsm_event_t* event)
{
  (void)fsm;
  (void)event;
  std::cout << "atm::WaitForAction:" << std::endl;
}

// external -> withdraw_pressed
static void WithdrawPressed(fsm_instance_t* fsm, fsm_event_t* event)
{
  (void)fsm;
  (void)event;
  std::cout << "atm::WithdrawPressed:" << std::endl;
  send_message((uint16_t)E_ATM_INPUT_WITHDRAW_OKAY);
}

// external -> withdraw_ok
static void DoneProcessing(fsm_instance_t* fsm, fsm_event_t* event)
{
  (void)fsm;
  (void)event;
  std::cout << "atm::DoneProcessing:" << std::endl;
  send_message((uint16_t)E_ATM_INPUT_END);
}


TEST(AtmStaticFsm, RunAtmFsm)
{
  std::cout << "atm::" << std::endl;

  wait_for_card_state.enter_action = CardInserted;
  getting_pin_state.enter_action = DigitPressed;
  verify_pin_state.enter_action = VerifyPin;
  wait_for_action_state.enter_action = WaitForAction;
  process_withdraw_state.enter_action = WithdrawPressed;
  done_processing_state.enter_action = DoneProcessing;

  fsm_create(&atm_fsm, 
      E_ATM_INPUT_CARD_INSERTED, E_ATM_STATE_WAITING_FOR_CARD, nullptr); 

  std::thread fsm_thread(runner, &atm_fsm);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  send_message((uint16_t)E_ATM_INPUT_CARD_INSERTED);
  
  send_message((uint16_t)E_ATM_INPUT_DIGIT_PRESSED);
  send_message((uint16_t)E_ATM_INPUT_DIGIT_PRESSED);
  send_message((uint16_t)E_ATM_INPUT_DIGIT_PRESSED);
  send_message((uint16_t)E_ATM_INPUT_DIGIT_PRESSED);

  std::this_thread::sleep_for(std::chrono::seconds(1));
  send_message((uint16_t)E_ATM_INPUT_WITHDRAW_PRESSED);

  // to wait for some time to allow fsm to run transitions
  std::this_thread::sleep_for(std::chrono::seconds(1));
  send_message((uint16_t)CONST_EXCEPTION_TO_END);
  fsm_thread.join();
}


// ={=========================================================================

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
