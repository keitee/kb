#include <chrono>
#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include "gmock/gmock.h"

#include "dfsm.h"
#include "sfsm.h"

using namespace testing;

// (gdb) b AlgoList_Divide_Test::TestBody()

// ={=========================================================================
// cxx-fsm-queue
// - make it simple, set T as int.

namespace fsm
{
  // simple message q
  template <typename T>
  class queue
  {
  public:
    void push(const T &message)
    {
      std::lock_guard<std::mutex> lock(m);

      mq.push(message);
      cv.notify_all();
    }

    T wait_and_pop()
    {
      std::unique_lock<std::mutex> lock(m);

      cv.wait(lock, [this] { return !mq.empty(); });
      auto message = mq.front();
      mq.pop();
      return message;
    }

  private:
    std::mutex m;
    std::condition_variable cv;
    std::queue<T> mq;
  };

} // namespace fsm

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

    if (msg == CONST_EXCEPTION_TO_END)
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

    if (msg == CONST_EXCEPTION_TO_END)
      break;

    VRM_FSM_Input(fsm, (uint16_t)msg);
  }

  std::cout << "fsm_thread: end" << std::endl;
}

// ={=========================================================================
// cxx-fsm-static

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
    E_ATM_STATE_WAITING_FOR_CARD, // current state
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
    {E_ATM_INPUT_CARD_INSERTED, E_ATM_STATE_CARD_INSERTED},
    {FSM_INVALID_EVENT_HDL, FSM_INVALID_STATE_HDL}};

  static fsm_transition_tbl_t card_inserted_tbl[] = {
    {E_ATM_INPUT_DIGIT_PRESSED, E_ATM_STATE_GETTING_PIN},
    {FSM_INVALID_EVENT_HDL, FSM_INVALID_STATE_HDL}};

  static fsm_transition_tbl_t getting_pin_tbl[] = {
    {E_ATM_INPUT_DIGIT_PRESSED, E_ATM_STATE_GETTING_PIN},
    {E_ATM_INPUT_DIGIT_DONE, E_ATM_STATE_VERIFY_PIN},
    {FSM_INVALID_EVENT_HDL, FSM_INVALID_STATE_HDL}};

  static fsm_transition_tbl_t verify_pin_tbl[] = {
    {E_ATM_INPUT_PIN_VERIFIED, E_ATM_STATE_WAIT_FOR_ACTION},
    {FSM_INVALID_EVENT_HDL, FSM_INVALID_STATE_HDL}};

  static fsm_transition_tbl_t wait_for_action_tbl[] = {
    {E_ATM_INPUT_WITHDRAW_PRESSED, E_ATM_STATE_PROCESS_WITHDRAW},
    {FSM_INVALID_EVENT_HDL, FSM_INVALID_STATE_HDL}};

  static fsm_transition_tbl_t process_withdraw_tbl[] = {
    {E_ATM_INPUT_WITHDRAW_OKAY, E_ATM_STATE_DONE_PROCESSING},
    {FSM_INVALID_EVENT_HDL, FSM_INVALID_STATE_HDL}};

  static fsm_transition_tbl_t done_processing_tbl[] = {
    {E_ATM_INPUT_END, E_ATM_STATE_GETTING_PIN},
    {FSM_INVALID_EVENT_HDL, FSM_INVALID_STATE_HDL}};

  // ={=========================================================================
  // fsm state and event list

  static fsm_state_t wait_for_card_state = {E_ATM_STATE_WAITING_FOR_CARD,
                                            (char *)"atm wait for card state",
                                            (fsm_action_func_t)NULL,
                                            wait_for_card_tbl};

  static fsm_state_t card_inserted_state = {E_ATM_STATE_CARD_INSERTED,
                                            (char *)"atm card inserted state",
                                            (fsm_action_func_t)NULL,
                                            card_inserted_tbl};

  static fsm_state_t getting_pin_state = {E_ATM_STATE_GETTING_PIN,
                                          (char *)"atm getting pin state",
                                          (fsm_action_func_t)NULL,
                                          getting_pin_tbl};

  static fsm_state_t verify_pin_state = {E_ATM_STATE_VERIFY_PIN,
                                         (char *)"atm verify pin state",
                                         (fsm_action_func_t)NULL,
                                         verify_pin_tbl};

  static fsm_state_t wait_for_action_state = {
    E_ATM_STATE_WAIT_FOR_ACTION,
    (char *)"atm wait for action state",
    (fsm_action_func_t)NULL,
    wait_for_action_tbl};

  static fsm_state_t process_withdraw_state = {
    E_ATM_STATE_PROCESS_WITHDRAW,
    (char *)"atm process withdraw state",
    (fsm_action_func_t)NULL,
    process_withdraw_tbl};

  static fsm_state_t done_processing_state = {E_ATM_STATE_DONE_PROCESSING,
                                              (char *)"atm done process state",
                                              (fsm_action_func_t)NULL,
                                              done_processing_tbl};

  static fsm_state_t *atm_state_list[] = {&wait_for_card_state,
                                          &card_inserted_state,
                                          &getting_pin_state,
                                          &verify_pin_state,
                                          &wait_for_action_state,
                                          &process_withdraw_state,
                                          &done_processing_state};

  static fsm_event_t atm_event_list[] = {
    {E_ATM_INPUT_WAIT_FOR_CARD, (char *)"want for card event", nullptr},
    {E_ATM_INPUT_CARD_INSERTED, (char *)"card inserted event", nullptr},
    {E_ATM_INPUT_DIGIT_PRESSED, (char *)"digit pressed event", nullptr},
    {E_ATM_INPUT_DIGIT_DONE, (char *)"digit done event", nullptr},
    {E_ATM_INPUT_PIN_VERIFIED, (char *)"pin verified event", nullptr},
    {E_ATM_INPUT_WITHDRAW_PRESSED, (char *)"withdraw pressed event", nullptr},
    {E_ATM_INPUT_WITHDRAW_OKAY, (char *)"withdraw okay event", nullptr},
    {E_ATM_INPUT_END, (char *)"input endevent", nullptr}};

  // ={=========================================================================
  // fsm instance

  static fsm_instance_t atm_fsm = {
    "atm", atm_state_list, atm_event_list, NULL, NULL};

  // ={=========================================================================
  // (state) action functions, eafs

  static std::string account;
  static std::string pin;

  // start -> wait for a card
  static void WaitForCard(fsm_instance_t *fsm, fsm_event_t *event)
  {
    (void)fsm;
    (void)event;

    std::cout << "--------------------------" << std::endl;
    std::cout << "event: <" << fsm_get_event_name(fsm, event->hdl) << ">"
              << ", current state: <" << fsm_get_current_state_name(fsm) << ">"
              << std::endl;

    std::cout << "atm::WaitForCard:" << std::endl;
  }

  // external -> card_inserted
  static void CardInserted(fsm_instance_t *fsm, fsm_event_t *event)
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
              << ", current state: <" << fsm_get_current_state_name(fsm) << ">"
              << std::endl;

    std::cout << "atm::CardInserted:" << std::endl;
    std::cout << "atm::CardInserted:init account and pin" << std::endl;
  }

  static void DigitPressed(fsm_instance_t *fsm, fsm_event_t *event)
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

  static void VerifyPin(fsm_instance_t *fsm, fsm_event_t *event)
  {
    (void)fsm;
    (void)event;
    std::cout << "--------------------------" << std::endl;
    std::cout << "atm::VerifyPin:" << std::endl;
    send_message((uint16_t)E_ATM_INPUT_PIN_VERIFIED);
  }

  // external -> pin_verified
  static void WaitForAction(fsm_instance_t *fsm, fsm_event_t *event)
  {
    (void)fsm;
    (void)event;
    std::cout << "--------------------------" << std::endl;
    std::cout << "atm::WaitForAction:" << std::endl;
  }

  // external -> withdraw_pressed
  static void WithdrawPressed(fsm_instance_t *fsm, fsm_event_t *event)
  {
    (void)fsm;
    (void)event;
    std::cout << "--------------------------" << std::endl;
    std::cout << "atm::WithdrawPressed:" << std::endl;
    send_message((uint16_t)E_ATM_INPUT_WITHDRAW_OKAY);
  }

  // external -> withdraw_ok
  static void DoneProcessing(fsm_instance_t *fsm, fsm_event_t *event)
  {
    (void)fsm;
    (void)event;
    std::cout << "--------------------------" << std::endl;
    std::cout << "atm::DoneProcessing:" << std::endl;
    send_message((uint16_t)E_ATM_INPUT_END);
  }

} // namespace fsm_static

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

TEST(Fsm, AtmStatic)
{
  using namespace fsm_static;

  std::cout << "fsm atm:: static" << std::endl;

  // set AFs
  wait_for_card_state.enter_action    = WaitForCard;
  card_inserted_state.enter_action    = CardInserted;
  getting_pin_state.enter_action      = DigitPressed;
  verify_pin_state.enter_action       = VerifyPin;
  wait_for_action_state.enter_action  = WaitForAction;
  process_withdraw_state.enter_action = WithdrawPressed;
  done_processing_state.enter_action  = DoneProcessing;

  // careful care for init state and event
  fsm_create(&atm_fsm,
             E_ATM_STATE_WAITING_FOR_CARD,
             E_ATM_INPUT_WAIT_FOR_CARD,
             nullptr);

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
// cxx-fsm-dynamic

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
    E_ATM_STATE_WAITING_FOR_CARD, // current state
    E_ATM_STATE_CARD_INSERTED,
    E_ATM_STATE_GETTING_PIN, // digit pressed
    E_ATM_STATE_VERIFY_PIN,
    E_ATM_STATE_WAIT_FOR_ACTION,
    E_ATM_STATE_PROCESS_WITHDRAW,
    E_ATM_STATE_DONE_PROCESSING
  } E_ATM_STATE;

  // eafs

  static std::string account;
  static std::string pin;

  // external -> card_inserted

  static void CardInserted(void *data)
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
              << " current state: " << (uint16_t)entry->state
              << " input event  : " << (uint16_t)entry->input
              << " -> new state    : " << (uint16_t)entry->new_state
              << std::endl;
  }

  // transition table

  static VRM_FSM_ENTRY AtmFsm[] = {

    VRM_FSM_E(E_ATM_STATE_WAITING_FOR_CARD, // current state
              E_ATM_INPUT_CARD_INSERTED,    // input event
              E_ATM_STATE_CARD_INSERTED,    // new state
              (VRM_FSM_ACTION)CardInserted  // action for new state
              ),

    VRM_FSM_E(E_ATM_STATE_CARD_INSERTED,
              E_ATM_INPUT_DIGIT_PRESSED,
              E_ATM_STATE_GETTING_PIN,
              (VRM_FSM_ACTION)DigitPressed),

    // need to define two entry here for differnt event from same state. For sfsm,
    // two entry in the transition table.

    VRM_FSM_E(E_ATM_STATE_GETTING_PIN,
              E_ATM_INPUT_DIGIT_PRESSED,
              E_ATM_STATE_GETTING_PIN,
              (VRM_FSM_ACTION)DigitPressed),

    VRM_FSM_E(E_ATM_STATE_GETTING_PIN,
              E_ATM_INPUT_DIGIT_DONE,
              E_ATM_STATE_VERIFY_PIN,
              (VRM_FSM_ACTION)VerifyPin),

    VRM_FSM_E(E_ATM_STATE_VERIFY_PIN,
              E_ATM_INPUT_PIN_VERIFIED,
              E_ATM_STATE_WAIT_FOR_ACTION,
              (VRM_FSM_ACTION)WaitForAction),

    VRM_FSM_E(E_ATM_STATE_WAIT_FOR_ACTION,
              E_ATM_INPUT_WITHDRAW_PRESSED,
              E_ATM_STATE_PROCESS_WITHDRAW,
              (VRM_FSM_ACTION)WithdrawPressed),

    VRM_FSM_E(E_ATM_STATE_PROCESS_WITHDRAW,
              E_ATM_INPUT_WITHDRAW_OKAY,
              E_ATM_STATE_DONE_PROCESSING,
              (VRM_FSM_ACTION)DoneProcessing),

    VRM_FSM_E(E_ATM_STATE_DONE_PROCESSING,
              E_ATM_INPUT_END,
              E_ATM_STATE_GETTING_PIN,
              (VRM_FSM_ACTION)DigitPressed),
  };

} // namespace fsm_dynamic

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

TEST(Fsm, AtmDynamic)
{
  using namespace fsm_dynamic;

  SYSTEM_STATUS stat = VRM_FSM_OK;

  // builds definition from definition info
  VRM_FSM_DEFINITION_INFO fsm_definition_info;
  VRM_FSM_DEFINITION_HANDLE fsm_definition_handle{};

  fsm_definition_info.entries     = AtmFsm;
  fsm_definition_info.num_entries = (uint16_t)NELEMENTS(AtmFsm);

  stat = VRM_FSM_CreateDefinition(&fsm_definition_info, &fsm_definition_handle);

  // build instance from init
  VRM_FSM_INSTANCE_HANDLE fsm_instance_handle{};
  VRM_FSM_INIT fsm_init;

  // initialize fsm init data
  fsm_init.fsm_definition_handle = fsm_definition_handle;
  fsm_init.callback              = (VRM_FSM_TRANS_CB)FsmOnTransition;
  fsm_init.init_state            = (uint16_t)E_ATM_STATE_WAITING_FOR_CARD;
  fsm_init.data                  = (void *)nullptr;

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
// cxx-fsm-cxx

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

key 3: chain *cxx_pattern_chain_of_responsibility* *cxx_pattern_decorator*

It’s a template, and the message type isn’t deducible, so you must specify which
message type to handle and pass in a function (or callable object) to handle it. 
  
handle() itself passes the queue, the current dispatcher object, and the handler
function to a new instance of the TemplateDispatcher class template, to handle
messages of the specified type.
   
o Your message processing stops when you’ve successfully handled a message, so
  that you can wait for a different set of messages next time. 

o If you do get a match for the specified message type, the `supplied function`
  is called rather than throwing an exception (although the handler function may
  throw an exception itself). *eaf*
  
If you don’t get a match, you chain to the previous dispatcher. In the first
instance, this will be a dispatcher, but

if you chain calls to handle() to allow multiple types of messages to be
handled, this may be a prior instantiation of TemplateDispatcher<>, which
will in turn chain to the previous handler if the message doesn’t match. 

Because any of the handlers might throw an exception (including the
dispatcher’s default handler for close_queue messages), the destructor must
once again be declared noexcept(false) f.  

(since this used dtor to start and dtor is noexcept(true) which means no
exception is expected. To off this, noexcept(false))

This simple framework allows you to push any type of message on the queue and
then selectively match against messages you can handle on the receiving end.
It also allows you to pass around a reference to the queue for pushing
messages on, while keeping the receiving end private.

note: So in every state function, member function, build a chain of temporary
of TemplateDispatcher and this chain is a list of messages that this state
can handle. messages that are not handled by the chain has no effect and
ignored.
  
   receiver.wait() returns disp temporary 
      | .handle<msg1>(f) return TD temporary, chain(true) 
        | .handle<msg2>(f) return TD temporary, chain(true) 
          | .handle<msg3>(f) return TD temporary, chain(false) 
            | dtor runs

  disp -> TD(msg1) -> TD(msg2) -> TD(msg3)  : creation
  disp <- TD(msg1) <- TD(msg2) <- TD(msg3)  : chain
                               <- check if it can handle a message
  
o use chain of temporaries which represent a state and evevnts for that state. 

o use "chained_" flag to decide if dtor of current temporary node in the chain
  shall run wait() and process message.

o if found a match from a chain, end loop and chain is destroyed.  

o if not foudn a match due to either no event or having event that are not
  expected in this chain, it do not end loop and so wait for next event.


Super State support

To support the super state:

o static fsm add the same transition to the table for all state in the super
  state group so that fsm can transit from any state to the destination.
  duplicate table entries.

o dynamic fsm do the same but done it programatically by setting the table. no
  duplicate and do simply setting value.

o chain of resposibility fsm needs to have the same lambda(duplicate) for all
  states in the group. can use the same function than duplicating lambda.

In sum, super state is diagram technique and can be implemented in many ways.

*/

TEST(Fsm, SimpleFsm1)
{
  using namespace cxx_fsm;

  simple fsm;

  // make thread which runs fsm
  std::thread fsm_thread(&simple::run, &fsm);

  // make sender to send a message

  messaging::sender fsm_queue(fsm.get_sender());

  fsm_queue.send(money_inserted(100));

  std::this_thread::sleep_for(std::chrono::seconds(10));

  fsm.done();
  fsm_thread.join();
}

TEST(Fsm, SimpleFsm2)
{
  using namespace cxx_fsm;

  simple fsm;

  // make thread which runs fsm
  std::thread fsm_thread(&simple::run, &fsm);

  // make sender to send a message

  messaging::sender fsm_queue(fsm.get_sender());

  fsm_queue.send(money_inserted(100));
  std::this_thread::sleep_for(std::chrono::seconds(2));

  fsm_queue.send(item_2_purchased());
  std::this_thread::sleep_for(std::chrono::seconds(2));

  fsm.done();
  fsm_thread.join();
}

// only for debugging purpose since other messages do not have `name` member and
// without this, it will cause compile errors.

#if !(defined DISPATCH_DEBUG)

// from listing_c.9.cpp

namespace messaging
{
  struct withdraw
  {
    std::string account;
    unsigned amount;
    mutable messaging::sender atm_queue;
    withdraw(std::string const &account_,
             unsigned amount_,
             messaging::sender atm_queue_)
        : account(account_)
        , amount(amount_)
        , atm_queue(atm_queue_)
    {}
  };
  struct withdraw_ok
  {};
  struct withdraw_denied
  {};
  struct cancel_withdrawal
  {
    std::string account;
    unsigned amount;
    cancel_withdrawal(std::string const &account_, unsigned amount_)
        : account(account_)
        , amount(amount_)
    {}
  };
  struct withdrawal_processed
  {
    std::string account;
    unsigned amount;
    withdrawal_processed(std::string const &account_, unsigned amount_)
        : account(account_)
        , amount(amount_)
    {}
  };
  struct card_inserted
  {
    std::string account;
    explicit card_inserted(std::string const &account_)
        : account(account_)
    {}
  };
  struct digit_pressed
  {
    char digit;
    explicit digit_pressed(char digit_)
        : digit(digit_)
    {}
  };
  struct clear_last_pressed
  {};
  struct eject_card
  {};
  struct withdraw_pressed
  {
    unsigned amount;
    explicit withdraw_pressed(unsigned amount_)
        : amount(amount_)
    {}
  };
  struct cancel_pressed
  {};
  struct issue_money
  {
    unsigned amount;
    issue_money(unsigned amount_)
        : amount(amount_)
    {}
  };
  struct verify_pin
  {
    std::string account;
    std::string pin;
    mutable messaging::sender atm_queue;
    verify_pin(std::string const &account_,
               std::string const &pin_,
               messaging::sender atm_queue_)
        : account(account_)
        , pin(pin_)
        , atm_queue(atm_queue_)
    {}
  };
  struct pin_verified
  {};
  struct pin_incorrect
  {};
  struct display_enter_pin
  {};
  struct display_enter_card
  {};
  struct display_insufficient_funds
  {};
  struct display_withdrawal_cancelled
  {};
  struct display_pin_incorrect_message
  {};
  struct display_withdrawal_options
  {};
  struct get_balance
  {
    std::string account;
    mutable messaging::sender atm_queue;
    get_balance(std::string const &account_, messaging::sender atm_queue_)
        : account(account_)
        , atm_queue(atm_queue_)
    {}
  };
  struct balance
  {
    unsigned amount;
    explicit balance(unsigned amount_)
        : amount(amount_)
    {}
  };
  struct display_balance
  {
    unsigned amount;
    explicit display_balance(unsigned amount_)
        : amount(amount_)
    {}
  };
  struct balance_pressed
  {};

  class interface_machine
  {
    messaging::receiver incoming;
    std::mutex iom;

  public:
    void done() { get_sender().send(messaging::close_queue()); }
    void run()
    {
      try
      {
        for (;;)
        {
          incoming.wait()
            .handle<issue_money>([&](issue_money const &msg) {
              (void)msg;
              {
                std::lock_guard<std::mutex> lk(iom);
                std::cout << "Issuing " << msg.amount << std::endl;
              }
            })
            .handle<display_insufficient_funds>(
              [&](display_insufficient_funds const &msg) {
                (void)msg;
                {
                  std::lock_guard<std::mutex> lk(iom);
                  std::cout << "Insufficient funds" << std::endl;
                }
              })
            .handle<display_enter_pin>([&](display_enter_pin const &msg) {
              (void)msg;
              {
                std::lock_guard<std::mutex> lk(iom);
                std::cout << "Please enter your PIN (0-9)" << std::endl;
              }
            })
            .handle<display_enter_card>([&](display_enter_card const &msg) {
              (void)msg;
              {
                std::lock_guard<std::mutex> lk(iom);
                std::cout << "Please enter your card (I)" << std::endl;
              }
            })
            .handle<display_balance>([&](display_balance const &msg) {
              (void)msg;
              {
                std::lock_guard<std::mutex> lk(iom);
                std::cout << "The balance of your account is " << msg.amount
                          << std::endl;
              }
            })
            .handle<display_withdrawal_options>(
              [&](display_withdrawal_options const &msg) {
                (void)msg;
                {
                  std::lock_guard<std::mutex> lk(iom);
                  std::cout << "Withdraw 50? (w)" << std::endl;
                  std::cout << "Display Balance? (b)" << std::endl;
                  std::cout << "Cancel? (c)" << std::endl;
                }
              })
            .handle<display_withdrawal_cancelled>(
              [&](display_withdrawal_cancelled const &msg) {
                (void)msg;
                {
                  std::lock_guard<std::mutex> lk(iom);
                  std::cout << "Withdrawal cancelled" << std::endl;
                }
              })
            .handle<display_pin_incorrect_message>(
              [&](display_pin_incorrect_message const &msg) {
                (void)msg;
                {
                  std::lock_guard<std::mutex> lk(iom);
                  std::cout << "PIN incorrect" << std::endl;
                }
              })
            .handle<eject_card>([&](eject_card const &msg) {
              (void)msg;
              {
                std::lock_guard<std::mutex> lk(iom);
                std::cout << "Ejecting card" << std::endl;
              }
            });
        }
      } catch (messaging::close_queue &)
      {}
    }
    messaging::sender get_sender() { return incoming; }
  };

  class bank_machine
  {
    messaging::receiver incoming;
    unsigned balance_;

  public:
    bank_machine()
        : balance_(199)
    {}
    void done() { get_sender().send(messaging::close_queue()); }
    void run()
    {
      try
      {
        for (;;)
        {
          incoming.wait()
            .handle<verify_pin>([&](verify_pin const &msg) {
              if (msg.pin == "1937")
              {
                msg.atm_queue.send(pin_verified());
              }
              else
              {
                msg.atm_queue.send(pin_incorrect());
              }
            })
            .handle<withdraw>([&](withdraw const &msg) {
              if (balance_ >= msg.amount)
              {
                msg.atm_queue.send(withdraw_ok());
                balance_ -= msg.amount;
              }
              else
              {
                msg.atm_queue.send(withdraw_denied());
              }
            })
            .handle<get_balance>([&](get_balance const &msg) {
              msg.atm_queue.send(balance(balance_));
            })
            .handle<withdrawal_processed>(
              [&](withdrawal_processed const &msg) { (void)msg; })
            .handle<cancel_withdrawal>(
              [&](cancel_withdrawal const &msg) { (void)msg; });
        }
      } catch (messaging::close_queue const &)
      {}
    }
    messaging::sender get_sender() { return incoming; }
  };

  class atm
  {
  private:
    messaging::receiver incoming_;
    messaging::sender bank_;
    messaging::sender interface_hardware_;

    // *cxx-member-function-pointer*
    void (atm::*state)();

    std::string account_;
    std::string pin_;
    unsigned withdraw_amount_;

    // function name is *state*
    void waiting_for_card()
    {
      // interface_hardware.send(display_enter_card());
      //
      // state waitinf_for_card -> event card_inserted -> state getting_pin
      //                              -> eaf, lambda

      incoming_.wait().handle<card_inserted>([&](const card_inserted &msg) {
        std::cout << "atm::waiting_for_card::card_inserted" << std::endl;
        account_ = msg.account;
        pin_     = "";
        // interface_hardware_.send(display_enter_pin());

        // note: set next state
        state = &atm::getting_pin;
      });
    }

    void getting_pin()
    {
      incoming_.wait()
        .handle<digit_pressed>([&](const digit_pressed &msg) {
          (void)msg;
          unsigned const pin_length = 4;
          pin_ += msg.digit;
          std::cout << "atm::getting_pin::digit_pressed: " << pin_ << std::endl;
          if (pin_.length() == pin_length)
          {
            // bank.send(verify_pin(account, pin, incoming));
            state = &atm::verifying_pin;
          }
        })
        .handle<clear_last_pressed>([&](const clear_last_pressed &msg) {
          (void)msg;
          std::cout << "atm::getting_pin::clear_last_pressed" << std::endl;
          if (!pin_.empty())
          {
            pin_.pop_back();
          }
        })
        .handle<cancel_pressed>([&](const cancel_pressed &msg) {
          (void)msg;
          std::cout << "atm::getting_pin::cancel_pressed" << std::endl;
          state = &atm::done_processing;
        });
    }

    // expect pin_* message from back fsm.
    void verifying_pin()
    {
      incoming_.wait()
        .handle<pin_verified>([&](const pin_verified &msg) {
          (void)msg;
          std::cout << "atm::verify_pin::pin_verified" << std::endl;
          state = &atm::wait_for_action;
        })
        .handle<pin_incorrect>([&](const pin_incorrect &msg) {
          (void)msg;
          std::cout << "atm::verify_pin::pin_incorrect" << std::endl;
          // interface_hardware_.send(display_pin_incorrect_message());
          state = &atm::done_processing;
        })
        .handle<cancel_pressed>([&](const cancel_pressed &msg) {
          (void)msg;
          std::cout << "atm::verify_pin::cancel_pressed" << std::endl;
          state = &atm::done_processing;
        });
    }

    void wait_for_action()
    {
      // interface_hardware_.send(display_withdrawal_options());
      incoming_.wait()
        .handle<withdraw_pressed>([&](const withdraw_pressed &msg) {
          withdraw_amount_ = msg.amount;
          std::cout << "atm::wait_for_action::withdraw_pressed: "
                    << withdraw_amount_ << std::endl;
          // bank_.send(withdraw(account_, msg.account, incoming_));
          state = &atm::process_withdrawal;
        })
        .handle<balance_pressed>([&](const balance_pressed &msg) {
          (void)msg;
          std::cout << "atm::wait_for_action::balance_pressed" << std::endl;
          // bank_.send(get_balance(account_, incoming_));
          state = &atm::process_balance;
        })
        .handle<cancel_pressed>([&](const cancel_pressed &msg) {
          (void)msg;
          std::cout << "atm::wait_for_action::cancel_pressed" << std::endl;
          state = &atm::done_processing;
        });
    }

    void process_balance()
    {
      incoming_.wait()
        .handle<balance>([&](const balance &msg) {
          (void)msg;
          std::cout << "atm::process_balance::balance" << std::endl;
          interface_hardware_.send(display_balance(msg.amount));
          state = &atm::wait_for_action;
        })
        .handle<cancel_pressed>([&](const cancel_pressed &msg) {
          (void)msg;
          std::cout << "atm::process_balance::cancel_pressed" << std::endl;
          state = &atm::done_processing;
        });
    }

    void process_withdrawal()
    {
      incoming_.wait()
        .handle<withdraw_ok>([&](const withdraw_ok &msg) {
          (void)msg;
          std::cout << "atm::process_withdrawal::withdraw_ok" << std::endl;
          // interface_hardware_.send(issue_money(withdrawal_amount));
          // bank_.send(withdrawal_processed(account,withdrawal_amount));
          state = &atm::done_processing;
        })
        .handle<withdraw_denied>([&](const withdraw_denied &msg) {
          (void)msg;
          std::cout << "atm::process_withdrawal::withdraw_denied" << std::endl;
          // interface_hardware_.send(display_insufficient_funds());
          state = &atm::done_processing;
        })
        .handle<cancel_pressed>([&](const cancel_pressed &msg) {
          (void)msg;
          std::cout << "atm::process_withdrawal::cancel_pressed" << std::endl;
          // bank_.send(cancel_withdrawal(account,withdrawal_amount));
          // interface_hardware_.send(display_withdrawal_cancelled());
          state = &atm::done_processing;
        });
    }

    void done_processing()
    {
      // interface_hardware_.send(eject_card());
      state = &atm::getting_pin;
    }

  public:
    atm(messaging::sender bank, messaging::sender interface_hardware)
        : bank_(bank)
        , interface_hardware_(interface_hardware)
    {}

    atm(const atm &) = delete;
    atm &operator=(const atm &) = delete;

    // send a message to self
    void done() { get_sender().send(messaging::close_queue()); }

    void run()
    {
      // note:
      // set init state
      state = &atm::waiting_for_card;

      std::cout << "atm::run::for" << std::endl;

      try
      {
        for (;;)
        {
          (this->*state)();
        }
      } catch (messaging::close_queue const &)
      {
        std::cout << "atm::run::end" << std::endl;
        // note: what would happen when catch exception but do nothing?
      }
    }

    // Whereas a sender just references a message queue, a receiver owns it. You
    // can obtain a sender that references the queue by using the implicit
    // conversion.
    //
    // note: this is an interface to expose q to other class and used to set up
    // connection each other
    //
    // atm machine(bank.get_sender(), intarface_hardware.get_sender());
    //
    // atm machine keeps this sender copies to send messages to them.

    // note: cxx-conversion-op which convert receiver to sender
    messaging::sender get_sender() { return incoming_; }
  };
} // namespace messaging


#endif // #if !(defined DISPATCH_DEBUG)

// ={=========================================================================

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
