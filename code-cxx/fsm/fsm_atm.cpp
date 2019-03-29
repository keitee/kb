#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <queue>

#include "gtest/gtest.h"
#include "sfsm.h"
#include "dfsm.h"


// ={=========================================================================
// message q 
// o To make it simple, set T as int.

namespace fsm 
{
  template <typename T>
  class queue
  {
    public:
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

      T wait_and_pop()
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
      std::queue<T> mq;
  };

} // namespace

static fsm::queue<uint32_t> q;

static const uint32_t CONST_EXCEPTION_TO_END{0xFFFF};

// not used
// static void get_message()
// {
//   auto msg = q.wait_and_pop();
// }

static void send_message(uint32_t message)
{
  q.push(message);
}

// forward message to fsm
static void fsm_static_runner(fsm_instance_t *fsm)
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

static void fsm_dynamic_runner(VRM_FSM_INSTANCE_HANDLE fsm)
{ 
  // std::cout << "fsm_thread: wait for a message" << std::endl;
  // auto msg = q.wait_and_pop();
  // std::cout << "fsm_thread: got a message" << std::endl;
  // VRM_FSM_Input(fsm, (uint16_t)E_ATM_INPUT_CARD_INSERTED);
  // std::cout << "fsm_thread: end" << std::endl;

  for (;;)
  {
    std::cout << "fsm_thread: wait for a message" << std::endl;

    auto msg = q.wait_and_pop();
    std::cout << "fsm_thread: got a <" << msg << ">" << std::endl;

    if(msg == CONST_EXCEPTION_TO_END)
      break;

    VRM_FSM_Input(fsm, (uint16_t)msg);
  }

  std::cout << "fsm_thread: end" << std::endl;
}


namespace fsm_static
{
  // ={=========================================================================
  // fsm event and state enums

  typedef enum 
  {
    E_ATM_INPUT_WAIT_FOR_CARD,
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


  // ={=========================================================================
  // fsm state transition table, <event, new state> pair

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


  // ={=========================================================================
  // fsm state and event list

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
    { E_ATM_INPUT_WAIT_FOR_CARD, (char *)"want for card event", nullptr},
    { E_ATM_INPUT_CARD_INSERTED, (char *)"card inserted event", nullptr},
    { E_ATM_INPUT_DIGIT_PRESSED, (char *)"digit pressed event", nullptr},
    { E_ATM_INPUT_DIGIT_DONE, (char *)"digit done event", nullptr},
    { E_ATM_INPUT_PIN_VERIFIED, (char *)"pin verified event", nullptr},
    { E_ATM_INPUT_WITHDRAW_PRESSED, (char *)"withdraw pressed event", nullptr},
    { E_ATM_INPUT_WITHDRAW_OKAY, (char *)"withdraw okay event", nullptr},
    { E_ATM_INPUT_END, (char *)"input endevent", nullptr}
  };


  // ={=========================================================================
  // fsm instance

  static fsm_instance_t atm_fsm = {
    "atm", atm_state_list, atm_event_list, NULL, NULL
  };


  // ={=========================================================================
  // (state) action functions, eafs

  static std::string account;
  static std::string pin;

  // start -> wait for a card
  static void WaitForCard(fsm_instance_t* fsm, fsm_event_t* event)
  {
    (void)fsm;
    (void)event;

    std::cout << "--------------------------" << std::endl;
    std::cout << "event: <" << fsm_get_event_name(fsm, event->hdl) << ">"
      << ", current state: <" << fsm_get_current_state_name(fsm) << ">" << std::endl;

    std::cout << "atm::WaitForCard:" << std::endl;
  }

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
      << ", current state: <" << fsm_get_current_state_name(fsm) << ">" << std::endl;

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
        // post a message to a q and exit
        send_message((uint16_t)E_ATM_INPUT_DIGIT_DONE);
      }
    }
    else
    {
      std::cout << "--------------------------" << std::endl;
      std::cout << "atm::DigitPressed: got ends" << std::endl;
    }
  }

  static void VerifyPin(fsm_instance_t* fsm, fsm_event_t* event)
  {
    (void)fsm;
    (void)event;
    std::cout << "--------------------------" << std::endl;
    std::cout << "atm::VerifyPin:" << std::endl;
    send_message((uint16_t)E_ATM_INPUT_PIN_VERIFIED);
  }

  // external -> pin_verified
  static void WaitForAction(fsm_instance_t* fsm, fsm_event_t* event)
  {
    (void)fsm;
    (void)event;
    std::cout << "--------------------------" << std::endl;
    std::cout << "atm::WaitForAction:" << std::endl;
  }

  // external -> withdraw_pressed
  static void WithdrawPressed(fsm_instance_t* fsm, fsm_event_t* event)
  {
    (void)fsm;
    (void)event;
    std::cout << "--------------------------" << std::endl;
    std::cout << "atm::WithdrawPressed:" << std::endl;
    send_message((uint16_t)E_ATM_INPUT_WITHDRAW_OKAY);
  }

  // external -> withdraw_ok
  static void DoneProcessing(fsm_instance_t* fsm, fsm_event_t* event)
  {
    (void)fsm;
    (void)event;
    std::cout << "--------------------------" << std::endl;
    std::cout << "atm::DoneProcessing:" << std::endl;
    send_message((uint16_t)E_ATM_INPUT_END);
  }

} // namesapce


// ={=========================================================================
// tests 

/*
fsm diagram:

   [init] (as if there is event to start state)

          | wait for a card
   [waiting for cards]

          | inserted
   [card inserted]

          | digit pressed
   [getting pin] loop: digit pressed

          | digit done
   [verify pin]

          | pin verified
   [wait for action]

          | withdraw pressed
   [process withdraw]

          | withdraw okay
   [done processing]
          | ends, back to "getting pin"

[ RUN      ] FsmAtm.Static
fsm atm:: static
--------------------------
event: <want for card event>, current state: <atm wait for card state>
atm::WaitForCard:
fsm_thread: got a <1>
--------------------------
event: <card inserted event>, current state: <atm card inserted state>
atm::CardInserted:
atm::CardInserted:init account and pin
fsm_thread: got a <2>
--------------------------
atm::DigitPressed: 1
fsm_thread: got a <2>
--------------------------
atm::DigitPressed: 11
fsm_thread: got a <2>
--------------------------
atm::DigitPressed: 111
fsm_thread: got a <2>
--------------------------
atm::DigitPressed: 1111
fsm_thread: got a <3>
--------------------------
atm::VerifyPin:
fsm_thread: got a <4>
--------------------------
atm::WaitForAction:
fsm_thread: got a <5>
--------------------------
atm::WithdrawPressed:
fsm_thread: got a <6>
--------------------------
atm::DoneProcessing:
fsm_thread: got a <7>
--------------------------
atm::DigitPressed: got ends
fsm_thread: got a <65535>
fsm_thread: end
[       OK ] FsmAtm.Static (3003 ms)

*/

TEST(FsmAtm, Static)
{
  using namespace fsm_static;

  std::cout << "fsm atm:: static" << std::endl;

  // set AFs
  wait_for_card_state.enter_action = WaitForCard;
  card_inserted_state.enter_action = CardInserted;
  getting_pin_state.enter_action = DigitPressed;
  verify_pin_state.enter_action = VerifyPin;
  wait_for_action_state.enter_action = WaitForAction;
  process_withdraw_state.enter_action = WithdrawPressed;
  done_processing_state.enter_action = DoneProcessing;

  // careful care for init state and event
  fsm_create(&atm_fsm, 
      E_ATM_STATE_WAITING_FOR_CARD, E_ATM_INPUT_WAIT_FOR_CARD,  nullptr); 

  std::thread fsm_thread(fsm_static_runner, &atm_fsm);

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
// dynamic fsm

namespace fsm_dynamic
{
  typedef enum
  {
    E_ATM_INPUT_NULL,
    E_ATM_INPUT_CARD_INSERTED,
    E_ATM_INPUT_DIGIT_PRESSED,
    E_ATM_INPUT_DIGIT_DONE,
    E_ATM_INPUT_PIN_VERIFIED,
    E_ATM_INPUT_WITHDRAW_PRESSED,
    E_ATM_INPUT_WITHDRAW_OKAY,
    E_ATM_INPUT_END
  } E_ATM_INPUT;

  // same as sfsm
  typedef enum
  {
    E_ATM_STATE_WAITING_FOR_CARD,   // current state
    E_ATM_STATE_CARD_INSERTED,
    E_ATM_STATE_GETTING_PIN,        // digit pressed
    E_ATM_STATE_VERIFY_PIN,
    E_ATM_STATE_WAIT_FOR_ACTION,
    E_ATM_STATE_PROCESS_WITHDRAW,
    E_ATM_STATE_DONE_PROCESSING
  } E_ATM_STATE;


// eafs

static std::string account;
static std::string pin;

// external -> card_inserted

static void CardInserted(void* data)
{
  (void)data;
  std::cout << "--------------------------" << std::endl;
  std::cout << "atm::CardInserted:" << std::endl;
  std::cout << "atm::CardInserted:init account and pin" << std::endl;
}

static void DigitPressed(void *data)
{
  (void)data;

  unsigned const pin_length = 4;
  pin += "1";
  std::cout << "--------------------------" << std::endl;
  std::cout << "atm::DigitPressed: " << pin << std::endl;

  if (pin.length() == pin_length)
  {
    send_message((uint16_t)E_ATM_INPUT_DIGIT_DONE);
  }
}

static void VerifyPin(void *data)
{
  (void)data;
  std::cout << "--------------------------" << std::endl;
  std::cout << "atm::VerifyPin:" << std::endl;
  send_message((uint16_t)E_ATM_INPUT_PIN_VERIFIED);
}

// external -> pin_verified
static void WaitForAction(void *data)
{
  (void)data;
  std::cout << "--------------------------" << std::endl;
  std::cout << "atm::WaitForAction:" << std::endl;
}

// external -> withdraw_pressed
static void WithdrawPressed(void *data)
{
  (void)data;
  std::cout << "--------------------------" << std::endl;
  std::cout << "atm::WithdrawPressed:" << std::endl;
  send_message((uint16_t)E_ATM_INPUT_WITHDRAW_OKAY);
}

// external -> withdraw_ok
static void DoneProcessing(void *data)
{
  (void)data;
  std::cout << "--------------------------" << std::endl;
  std::cout << "atm::DoneProcessing:" << std::endl;
  send_message((uint16_t)E_ATM_INPUT_END);
}

static void FsmOnTransition(void *data, const VRM_FSM_ENTRY *entry)
{
  std::cout << "--------------------------" << std::endl;
  std::cout << "FsmOnTransition: " 
    << " current state: " << (uint16_t) entry->state
    << " input event  : " << (uint16_t) entry->input
    << " -> new state    : " << (uint16_t) entry->new_state
    << std::endl;
}


// transition table

static VRM_FSM_ENTRY AtmFsm[] = {

  VRM_FSM_E(
      E_ATM_STATE_WAITING_FOR_CARD,   // current state
      E_ATM_INPUT_CARD_INSERTED,      // input event
      E_ATM_STATE_CARD_INSERTED,      // new state
      (VRM_FSM_ACTION)CardInserted    // action for new state
      ),

  VRM_FSM_E(
      E_ATM_STATE_CARD_INSERTED,
      E_ATM_INPUT_DIGIT_PRESSED,
      E_ATM_STATE_GETTING_PIN,
      (VRM_FSM_ACTION)DigitPressed
      ),

  // need to define two entry here for differnt event from same state. For sfsm,
  // two entry in the transition table.

  VRM_FSM_E(
      E_ATM_STATE_GETTING_PIN,
      E_ATM_INPUT_DIGIT_PRESSED,
      E_ATM_STATE_GETTING_PIN,
      (VRM_FSM_ACTION)DigitPressed
      ),

  VRM_FSM_E(
      E_ATM_STATE_GETTING_PIN,
      E_ATM_INPUT_DIGIT_DONE,
      E_ATM_STATE_VERIFY_PIN,
      (VRM_FSM_ACTION)VerifyPin
      ),

  VRM_FSM_E(
      E_ATM_STATE_VERIFY_PIN,
      E_ATM_INPUT_PIN_VERIFIED,
      E_ATM_STATE_WAIT_FOR_ACTION,
      (VRM_FSM_ACTION)WaitForAction
      ),

  VRM_FSM_E(
      E_ATM_STATE_WAIT_FOR_ACTION,
      E_ATM_INPUT_WITHDRAW_PRESSED,
      E_ATM_STATE_PROCESS_WITHDRAW,
      (VRM_FSM_ACTION)WithdrawPressed
      ),

  VRM_FSM_E(
      E_ATM_STATE_PROCESS_WITHDRAW,
      E_ATM_INPUT_WITHDRAW_OKAY,
      E_ATM_STATE_DONE_PROCESSING,
      (VRM_FSM_ACTION)DoneProcessing
      ),

  VRM_FSM_E(
      E_ATM_STATE_DONE_PROCESSING,
      E_ATM_INPUT_END,
      E_ATM_STATE_GETTING_PIN,
      (VRM_FSM_ACTION)DigitPressed
      ),
};

} // namespace


/*
 same fsm diagram as sfsm.

[ RUN      ] FsmAtm.Dynamic
fsm_definition::min_states: 0
fsm_definition::max_states: 6
fsm_definition::num_inputs: 7
fsm_definition::num_states: 7
fsm_definition::count     : 49
fsm_thread: wait for a message
fsm_thread: got a <1>
VRM-FSM: atm: E_ATM_STATE_WAITING_FOR_CARD -> (E_ATM_INPUT_CARD_INSERTED) -> E_ATM_STATE_CARD_INSERTED
--------------------------
atm::CardInserted:
atm::CardInserted:init account and pin
--------------------------
FsmOnTransition:  current state: 0 input event  : 1 -> new state    : 1
fsm_thread: wait for a message
fsm_thread: got a <2>
VRM-FSM: atm: E_ATM_STATE_CARD_INSERTED -> (E_ATM_INPUT_DIGIT_PRESSED) -> E_ATM_STATE_GETTING_PIN
--------------------------
atm::DigitPressed: 1
--------------------------
FsmOnTransition:  current state: 1 input event  : 2 -> new state    : 2
fsm_thread: wait for a message
fsm_thread: got a <2>
VRM-FSM: atm: E_ATM_STATE_GETTING_PIN -> (E_ATM_INPUT_DIGIT_PRESSED) -> E_ATM_STATE_GETTING_PIN
--------------------------
atm::DigitPressed: 11
--------------------------
FsmOnTransition:  current state: 2 input event  : 2 -> new state    : 2
fsm_thread: wait for a message
fsm_thread: got a <2>
VRM-FSM: atm: E_ATM_STATE_GETTING_PIN -> (E_ATM_INPUT_DIGIT_PRESSED) -> E_ATM_STATE_GETTING_PIN
--------------------------
atm::DigitPressed: 111
--------------------------
FsmOnTransition:  current state: 2 input event  : 2 -> new state    : 2
fsm_thread: wait for a message
fsm_thread: got a <2>
VRM-FSM: atm: E_ATM_STATE_GETTING_PIN -> (E_ATM_INPUT_DIGIT_PRESSED) -> E_ATM_STATE_GETTING_PIN
--------------------------
atm::DigitPressed: 1111
--------------------------
FsmOnTransition:  current state: 2 input event  : 2 -> new state    : 2
fsm_thread: wait for a message
fsm_thread: got a <3>
VRM-FSM: atm: E_ATM_STATE_GETTING_PIN -> (E_ATM_INPUT_DIGIT_DONE) -> E_ATM_STATE_VERIFY_PIN
--------------------------
atm::VerifyPin:
--------------------------
FsmOnTransition:  current state: 2 input event  : 3 -> new state    : 3
fsm_thread: wait for a message
fsm_thread: got a <4>
VRM-FSM: atm: E_ATM_STATE_VERIFY_PIN -> (E_ATM_INPUT_PIN_VERIFIED) -> E_ATM_STATE_WAIT_FOR_ACTION
--------------------------
atm::WaitForAction:
--------------------------
FsmOnTransition:  current state: 3 input event  : 4 -> new state    : 4
fsm_thread: wait for a message
fsm_thread: got a <5>
VRM-FSM: atm: E_ATM_STATE_WAIT_FOR_ACTION -> (E_ATM_INPUT_WITHDRAW_PRESSED) -> E_ATM_STATE_PROCESS_WITHDRAW
--------------------------
atm::WithdrawPressed:
--------------------------
FsmOnTransition:  current state: 4 input event  : 5 -> new state    : 5
fsm_thread: wait for a message
fsm_thread: got a <6>
VRM-FSM: atm: E_ATM_STATE_PROCESS_WITHDRAW -> (E_ATM_INPUT_WITHDRAW_OKAY) -> E_ATM_STATE_DONE_PROCESSING
--------------------------
atm::DoneProcessing:
--------------------------
FsmOnTransition:  current state: 5 input event  : 6 -> new state    : 6
fsm_thread: wait for a message
fsm_thread: got a <7>
VRM-FSM: atm: E_ATM_STATE_DONE_PROCESSING -> (E_ATM_INPUT_END) -> E_ATM_STATE_GETTING_PIN
--------------------------
atm::DigitPressed: 11111
--------------------------
FsmOnTransition:  current state: 6 input event  : 7 -> new state    : 2
fsm_thread: wait for a message
fsm_thread: got a <65535>
fsm_thread: end
[       OK ] FsmAtm.Dynamic (3002 ms)
 
*/

TEST(FsmAtm, Dynamic)
{
  using namespace fsm_dynamic;

  SYSTEM_STATUS	            stat = VRM_FSM_OK;
  VRM_FSM_DEFINITION_INFO   fsm_info;
  VRM_FSM_DEFINITION_HANDLE fsm_definition_handle{};
  VRM_FSM_INSTANCE_HANDLE   fsm_instance_handle{};
  VRM_FSM_INIT	            fsm_init;

  fsm_info.entries      = AtmFsm;
  fsm_info.num_entries  = (uint16_t)NELEMENTS(AtmFsm);

  stat = VRM_FSM_CreateDefinition(&fsm_info, &fsm_definition_handle);

  // initialize fsm init data 
  fsm_init.fsm_definition_handle  = fsm_definition_handle;
  fsm_init.callback               = (VRM_FSM_TRANS_CB)FsmOnTransition;
  fsm_init.init_state             = (uint16_t)E_ATM_STATE_WAITING_FOR_CARD;
  fsm_init.data                   = (void*) nullptr;

  stat = VRM_FSM_CreateInstance(&fsm_init, &fsm_instance_handle);

  VRM_FSM_SetName(fsm_instance_handle, "atm");

  std::thread fsm_thread(fsm_dynamic_runner, fsm_instance_handle);

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

  if (fsm_instance_handle != VRM_FSM_ILLEGAL_INSTANCE_HANDLE)
    VRM_FSM_DestroyInstance(fsm_instance_handle);

  if (fsm_definition_handle != VRM_FSM_ILLEGAL_DEFINITION)
    VRM_FSM_DestroyDefinition(fsm_definition_handle); 
}


// ={=========================================================================
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

namespace messaging
{
  // Q: why use this?

  struct message_base
  {
    virtual ~message_base() {}
  };

  template<typename T>
    struct wrapped_message : public message_base
    {
      explicit wrapped_message(T const &contents) 
        : contents_(contents) {}

      T contents_;
    };

  class queue
  {
    public:
      template<typename T>
        void push(T const &message)
        {
          std::lock_guard<std::mutex> lock(m_);

          mg_.push(std::make_shared<wrapper_message<T>>(message));
          cv_.notify_all();
        }

    private:
      std::mutex m_;
      std::codition_variable cv_;

      // use sp instead
      std::queue<std::shared_ptr<message_base>> mq_;
  };

  class close_queue
  {};

  class sender
  {
    public:
      sender() : pq_(nullptr) {}
      explicit sender(queue *pq) : pq_(pq) {}

      template<type
    private:
      queue *pq_;
  };

  class receiver
  {
    public:
      // *cxx-conversion-op* which convert receiver to sender
      operator sender()
      { return sender(&q_); }

      dispatcher wait()
      { return dispatcher(&q_); }

    private:
      queue q_;
  };

  class interface_machine
  {
    public:
      sender get_sender()
      {
        return incoming_;
      }

      void done()
      {
        get_sender().send(close_queue());
      }

      
    private:
      receiver incoming_;
      std::mutex iom_;
  };

} // namespace


// ={=========================================================================

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
