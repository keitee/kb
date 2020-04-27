#include <chrono>
#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

#include "gmock/gmock.h"

// for static fsm
#include "sfsm.h"

// for dynamic fsm
#include "dfsm.h"

namespace
{
  // simple message q
  template <typename T>
  class queue
  {
  private:
    std::mutex m_;
    std::condition_variable cv_;
    std::queue<T> q_;

  public:
    void push(const T &mesg)
    {
      std::lock_guard<std::mutex> lock(m_);

      q_.emplace(mesg);
      cv_.notify_one();
    }

    T wait_and_pop()
    {
      std::unique_lock<std::mutex> lock(m_);

      cv_.wait(lock, [this] { return !q_.empty(); });

      auto mesg = q_.front();
      q_.pop();

      return mesg;
    }
  };

  queue<uint32_t> q;

  const uint32_t CONST_EXCEPTION_TO_END{0xFFFF};

  void send_message(uint32_t message) { q.push(message); }

  // forward message to fsm
  void fsm_static_runner(fsm_instance_t *fsm)
  {
    // std::cout << "fsm_thread: wait for a message" << std::endl;
    // auto msg = q.wait_and_pop();
    // std::cout << "fsm_thread: got a message" << std::endl;
    // VRM_FSM_Input(fsm, (uint16_t)E_ATM_INPUT_CARD_INSERTED);
    // std::cout << "fsm_thread: end" << std::endl;

    for (;;)
    {
      auto msg = q.wait_and_pop();
      std::cout << "fsm static: got a <" << msg << ">" << std::endl;

      if (msg == CONST_EXCEPTION_TO_END)
        break;

      fsm_post_event(fsm, msg, nullptr);
    }

    std::cout << "fsm static: end" << std::endl;
  }

  // why different runner? since static and dynamic has different interface to use
  void fsm_dynamic_runner(VRM_FSM_INSTANCE_HANDLE fsm)
  {
    // std::cout << "fsm_thread: wait for a message" << std::endl;
    // auto msg = q.wait_and_pop();
    // std::cout << "fsm_thread: got a message" << std::endl;
    // VRM_FSM_Input(fsm, (uint16_t)E_ATM_INPUT_CARD_INSERTED);
    // std::cout << "fsm_thread: end" << std::endl;

    for (;;)
    {
      auto msg = q.wait_and_pop();
      std::cout << "fsm dynamic: got a <" << msg << ">" << std::endl;

      if (msg == CONST_EXCEPTION_TO_END)
        break;

      VRM_FSM_Input(fsm, (uint16_t)msg);
    }

    std::cout << "fsm dynamic: end" << std::endl;
  }
} // namespace

// ={=========================================================================
// cxx-fsm-static

namespace fsm_static
{
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

  // fsm instance

  static fsm_instance_t atm_fsm = {
    "atm", atm_state_list, atm_event_list, NULL, NULL};

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

[ RUN      ]
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
[       OK ] (3003 ms)
*/

TEST(CxxFsm, check_static)
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

  std::thread t(fsm_static_runner, &atm_fsm);

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

  t.join();
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

TEST(CxxFsm, check_dynamic)
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

  std::thread t(fsm_dynamic_runner, fsm_instance_handle);

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

  t.join();

  if (fsm_instance_handle != VRM_FSM_ILLEGAL_INSTANCE_HANDLE)
    VRM_FSM_DestroyInstance(fsm_instance_handle);

  if (fsm_definition_handle != VRM_FSM_ILLEGAL_DEFINITION)
    VRM_FSM_DestroyDefinition(fsm_definition_handle);
}

/*
// ={=========================================================================

cxx-fsm-ccon

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
   
o Your message processing "stops" when you’ve successfully handled a message, so
  that you can wait for a different set of messages "next time."

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

(since this uses dtor to start and dtor is noexcept(true) which means no
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

o use "chained_" flag to decide if dtor of current inner-most temporary node in
  the chain shall run wait() and process message.

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

o chain of resposibility fsm needs to have the same TD for all states in the
  super. 

  ?? can use the same function than duplicating lambda.

In sum, super state is diagram technique and can be implemented in many ways.

*/

namespace fsm_ccon
{
  //--------------------------------------------------------------------------
  // message queue

  // polymorphic base. is that only purpose to have this? no.
  struct message_base
  {
    virtual ~message_base() = default;
  };

  template <typename T>
  struct wrapped_message : public message_base
  {
    T message_;

    explicit wrapped_message(const T &message)
        : message_(message)
    {}
  };

  class queue
  {
  private:
    std::mutex m_;
    std::condition_variable cv_;

    // can use unique_ptr instead? it seems no real difference since pop_front()
    // will decrease counter and it will be released when client is done with it
    // which is the same time point when use unique_ptr.

    std::deque<std::shared_ptr<message_base>> q_;

  public:
    // create new type of wrapped_message<T> whenever push() is called
    template <typename T>
    void push(const T &message)
    {
      std::lock_guard<std::mutex> lock(m_);

      q_.emplace_back(std::make_shared<wrapped_message<T>>(message));
      cv_.notify_one();
    }

    std::shared_ptr<message_base> wait_and_pop()
    {
      std::unique_lock<std::mutex> lock(m_);

      cv_.wait(lock, [&] { return !q_.empty(); });

      auto message = std::move(q_.front());

      // *cxx-error* cuase infinite loop on the user when omits this
      q_.pop_front();

      // not needed
      // lock.unlock();

      return message;
    }
  };

#if 0
  NOT OK. compile error. since "wrapped_message1" is a template but not a
  concrete and std::deque in queue1 clase needs a type to create.

  Shall we make queue1 a "template"? NO since lose the benefit of having
  template function, post().

  So need a concrete base class which is "message_base"

  //--------------------------------------------------------------------------
  // message queue which do not use inheritance

  template <typename T>
  struct wrapped_message1
  {
    T message_;

    explicit wrapped_message1(const T &message)
        : message_(message)
    {}
  };

  class queue1
  {
  private:
    std::mutex m_;
    std::condition_variable cv_;

    // can use unique_ptr instead?
    std::deque<std::shared_ptr<wrapped_message1>> q_;

  public:
    // create new type of wrapped_message<T> whenever push() is called
    template <typename T>
    void push(const T &message)
    {
      std::lock_guard<std::mutex> lock(m_);

      q_.emplace_back(std::make_shared<wrapped_message1<T>>(message));
      cv_.notify_one();
    }

    std::shared_ptr<wrapped_message1> wait_and_pop()
    {
      std::unique_lock<std::mutex> lock(m_);

      cv_.wait(lock, [&] { return !q_.empty(); });

      auto message = std::move(q_.front());

      // *cxx-error* cuase infinite loop on the user when omits this
      q_.pop_front();

      // not needed
      // lock.unlock();

      return message;
    }
  };
#endif

  //--------------------------------------------------------------------------
  // template dispatcher

  // as see cxx-rvo, cxx-rvo is not always guranteed and move context is needed
  // in this case
#define MOVE_IS_NECESSARY

#define FRIEND_IS_NECESSARY
  // #define DISPATCH_DEBUG

  template <typename Dispatcher, typename Message, typename Function>
  class TemplateDispatcher
  {
  private:
    // use pointer
    queue *q_;
    Dispatcher *prev_;
    Function f_;
    bool chained_;

#ifdef DISPATCH_DEBUG

    // NOTE:
    //
    // name_ is only for debugging purpose since other messages do not have
    // `name` member and without this, it will cause compile errors.
    //
    // also,
    // this meant to be debugging purpose. but when defines it but not used in
    // std::cout in the code.
    //
    // see compile errors when use "money_inserted" message type:
    //
    // /home/keitee/git/kb/code-cxx/fsm/test_cxxfsm.cpp: In instantiation of
    // ‘fsm_ccon::TemplateDispatcher<Dispatcher, Message, Function>::TemplateDispatcher(fsm_ccon::queue*, Dispatcher*, Function&&)
    //   [with Dispatcher = fsm_ccon::dispatcher; Message = fsm_atm_ex::money_inserted;
    //   Function = fsm_atm_ex::simple::wait_for_money()::<lambda(const fsm_atm_ex::money_inserted&)>]’:
    //
    // /home/keitee/git/kb/code-cxx/fsm/test_cxxfsm.cpp:1176:14:   required from
    // ‘fsm_ccon::TemplateDispatcher<fsm_ccon::dispatcher, Message, Function>
    // fsm_ccon::dispatcher::handle(Function&&)
    //   [with Message = fsm_atm_ex::money_inserted;
    //   Function = fsm_atm_ex::simple::wait_for_money()::<lambda(const fsm_atm_ex::money_inserted&)>]’
    //
    // /home/keitee/git/kb/code-cxx/fsm/test_cxxfsm.cpp:1817:10:   required from here
    //
    // /home/keitee/git/kb/code-cxx/fsm/test_cxxfsm.cpp:1047:25: error: no matching function for call to ‘fsm_atm_ex::money_inserted::money_inserted()’
    //          , chained_(false)
    //                          ^
    // /home/keitee/git/kb/code-cxx/fsm/test_cxxfsm.cpp:1721:14: note: candidate: fsm_atm_ex::money_inserted::money_inserted(const unsigned int&)
    //      explicit money_inserted(unsigned const &amount)
    //               ^~~~~~~~~~~~~~
    // /home/keitee/git/kb/code-cxx/fsm/test_cxxfsm.cpp:1721:14: note:   candidate expects 1 argument, 0 provided
    // /home/keitee/git/kb/code-cxx/fsm/test_cxxfsm.cpp:1718:10: note: candidate: constexpr fsm_atm_ex::money_inserted::money_inserted(const fsm_atm_ex::money_inserted&)
    //    struct money_inserted
    //           ^~~~~~~~~~~~~~
    // /home/keitee/git/kb/code-cxx/fsm/test_cxxfsm.cpp:1718:10: note:   candidate expects 1 argument, 0 provided
    // /home/keitee/git/kb/code-cxx/fsm/test_cxxfsm.cpp:1718:10: note: candidate: constexpr fsm_atm_ex::money_inserted::money_inserted(fsm_atm_ex::money_inserted&&)
    // /home/keitee/git/kb/code-cxx/fsm/test_cxxfsm.cpp:1718:10: note:   candidate expects 1 argument, 0 provided
    //
    // HOWEVER, two things:
    //
    // 1. the same code works for other messages in "namespace client"
    // 2. errors are gone when not define DISPATCH_DEBUG
    //
    // The problem is:
    //
    // when "Message" type is one which requires args and it will define member
    // m_ here but no arg is given.
    //
    // this is solved when use the default arg for that "Message" type
    //
    // GCC error is not clear

    Message m_;
#endif

  public:
    // no-copy
    TemplateDispatcher(const TemplateDispatcher &) = delete;
    TemplateDispatcher &operator=(const TemplateDispatcher &) = delete;

#ifdef MOVE_IS_NECESSARY
    // move-ctor
    TemplateDispatcher(TemplateDispatcher &&other)
        : q_(other.q_)
        , prev_(other.prev_)
        , f_(std::move(other.f_))
        , chained_(other.chained_)
    {
      // std::cout << "TD: move: set chained_ true: " << m_.name_ << std::endl;
      other.chained_ = true;
    }
#endif

#ifdef FRIEND_IS_NECESSARY
    template <typename D, typename M, typename F>
    friend class TemplateDispatcher;
#endif

    // ctor
    TemplateDispatcher(queue *q, Dispatcher *prev, Function &&f)
        : q_(q)
        , prev_(prev)
        , f_(std::forward<Function>(f))
        , chained_(false)
    {
      // std::cout << "TD: ctor: set chained_ true: " << m_.name_ << std::endl;

      // *cxx-error* if there is no friendship set
      prev_->chained_ = true;
    }

    // dtor
    ~TemplateDispatcher() noexcept(false)
    {
      if (!chained_)
      {
        // std::cout << "TD: ~TD: chained_ false: " << m_.name_ << std::endl;
        wait_and_dispatch();
      }

      // std::cout << "TD: ~TD: " << m_.name_ << std::endl;
    }

    template <typename OtherMessage, typename OtherFunction>
    TemplateDispatcher<TemplateDispatcher, OtherMessage, OtherFunction>
    handle(OtherFunction &&f)
    {
      return TemplateDispatcher<TemplateDispatcher,
                                OtherMessage,
                                OtherFunction>(q_,
                                               this,
                                               std::forward<OtherFunction>(f));
    }

  private:
    // Message is T and if T do not match with a message type from queue then
    // dynamic_cast fails and delegate it to the previous TD in the chain.

    bool dispatch_(const std::shared_ptr<message_base> &message)
    {
      if (wrapped_message<Message> *wrapper =
            dynamic_cast<wrapped_message<Message> *>(message.get()))
      {
        // std::cout << "TD: dispatch_: " << m_.name_
        //           << ": call f: " << wrapper->message_.name_ << std::endl;

        f_(wrapper->message_);
        return true;
      }
      else
      {
        // std::cout << "TD: dispatch_: " << m_.name_
        //           << ": pass to prev: " << std::endl;

        return prev_->dispatch_(message);
      }
    }

    void wait_and_dispatch()
    {
      for (;;)
      {
        // std::cout << "TD: wait_dispatch_: " << m_.name_ << " { " << std::endl;

        auto message = q_->wait_and_pop();

        if (dispatch_(message))
          break;
      }

      // std::cout << "TD: wait_dispatch_: " << m_.name_ << " } " << std::endl;
    }
  };

  //--------------------------------------------------------------------------
  // message to end
  class close_queue
  {};

  //--------------------------------------------------------------------------
  // dispatcher

  class dispatcher
  {
  private:
    queue *q_;
    bool chained_;

  public:
    // no-copy
    dispatcher(const dispatcher &) = delete;
    dispatcher &operator=(const dispatcher &) = delete;

#ifdef MOVE_IS_NECESSARY
    // move-ctor
    dispatcher(dispatcher &&other)
        : q_(other.q_)
        , chained_(other.chained_)
    {
      other.chained_ = true;
    }
#endif

    // ctor
    explicit dispatcher(queue *q)
        : q_(q)
        , chained_(false)
    {
      std::cout << "disp: ctor: " << std::endl;
    }

    // dtor
    ~dispatcher() noexcept(false)
    {
      if (!chained_)
      {
        wait_and_dispatch_();
      }

      std::cout << "disp: ~disp: end: " << std::endl;
    }

    // o muse use `dispatcher` in TemplateDispatcher<dispatcher,...> since it'll
    // be type of the previous of TD
    //
    // o use template-default-argument since cannot deduce "Message" from call

    template <typename Message, typename Function>
    TemplateDispatcher<dispatcher, Message, Function> handle(Function &&f)
    {
      return TemplateDispatcher<dispatcher, Message, Function>(
        q_,
        this,
        std::forward<Function>(f));
    }

#ifdef FRIEND_IS_NECESSARY
    // *cxx-template-friend*
    template <typename D, typename M, typename F>
    friend class TemplateDispatcher;
#endif

  private:
    bool dispatch_(const std::shared_ptr<message_base> &message)
    {
      if (dynamic_cast<wrapped_message<close_queue> *>(message.get()))
      {
        std::cout << "disp: dispatch_: throw" << std::endl;

        throw close_queue();
      }

      std::cout << "disp: dispatch_: false" << std::endl;

      return false;
    }

    void wait_and_dispatch_()
    {
      std::cout << "disp: wait_dispatch_: { " << std::endl;

      for (;;)
      {
        auto message = q_->wait_and_pop();
        dispatch_(message);
      }

      std::cout << "disp: wait_dispatch_: } " << std::endl;
    }
  };

  //--------------------------------------------------------------------------
  // sender and receiver

  class sender
  {
  private:
    queue *q_;

  public:
    // not allow this ctor
    sender() = delete;

    explicit sender(queue *q)
        : q_(q)
    {}

    template <typename T>
    void send(const T &message)
    {
      if (q_)
        q_->push(message);
    }
  };

  // o receiver owns a queue
  class receiver
  {
  private:
    queue q_;

  public:
    // *cxx-conversion-op* which converts receiver to sender
    operator sender() { return sender(&q_); }

    // *cxx-error* compile error when there is no move-ctor of dispatcher
    // #ifdef MOVE_IS_NECESSARY
    dispatcher wait() { return dispatcher(&q_); }
  };

  //--------------------------------------------------------------------------
  // messages

  namespace client
  {
    struct simple_message
    {
      std::string name_;
      explicit simple_message()
          : name_("simple_message")
      {}
    };

    struct cancel_pressed
    {
      std::string name_;
      cancel_pressed()
          : name_("cancel_pressed")
      {}
    };

    struct pin_verified
    {
      std::string name_;
      pin_verified()
          : name_("pin_verified")
      {}
    };

    struct pin_incorrect
    {
      std::string name_;
      pin_incorrect()
          : name_("pin_incorrect")
      {}
    };

    struct withdraw_ok
    {
      std::string name_;
      withdraw_ok()
          : name_("withdraw_ok")
      {}
    };
  } // namespace client
} // namespace fsm_ccon

// show a queue that supports *any* type

TEST(CxxFsm, check_message_queue)
{
  using namespace fsm_ccon;
  using namespace fsm_ccon::client;

  // to avoid ambiguity
  fsm_ccon::queue q;

  // send T. copy T into wrapped_message and save shared_ptr of it
  //
  // NOTE:
  // see use of template function then template class. have to populate a class
  // for every T? it's easy and light to populate template functoin for each T.

  q.push(simple_message());

  // get std::shared_ptr<message_base>
  auto sp = q.wait_and_pop();

  // what's really saved in shared pointer is wrapped_message instance.
  // dynamic_cast() should success since it's in inheritance.
  wrapped_message<simple_message> *message =
    dynamic_cast<wrapped_message<simple_message> *>(sp.get());

  // message->message_ is T which is copied in adn T.name_ to get name
  // std::cout << "message->message_ : " << message->message_.name_ << std::endl;
  EXPECT_THAT(message->message_.name_, "simple_message");
}

// use `reinterpret_cast` instead

TEST(CxxFsm, check_message_queue1)
{
  using namespace fsm_ccon;
  using namespace fsm_ccon::client;

  // to avoid ambiguity
  fsm_ccon::queue q;

  // send T. copy T into wrapped_message and save shared_ptr of it
  //
  // NOTE:
  // see use of template function then template class. have to populate a class
  // for every T? it's easy and light to populate template functoin for each T.

  q.push(simple_message());

  // get std::shared_ptr<message_base>
  auto sp = q.wait_and_pop();

  // what's really saved in shared pointer is wrapped_message instance.
  // dynamic_cast() should success since it's in inheritance.
  wrapped_message<simple_message> *message =
    reinterpret_cast<wrapped_message<simple_message> *>(sp.get());

  // message->message_ is T which is copied in adn T.name_ to get name
  // std::cout << "message->message_ : " << message->message_.name_ << std::endl;
  EXPECT_THAT(message->message_.name_, "simple_message");
}

#if 0
// can we not use "message_base"?

TEST(CxxFsm, check_message_queue1)
{
  using namespace fsm_ccon;
  using namespace fsm_ccon::client;

  // to avoid ambiguity
  fsm_ccon::queue q;

  // send T. copy T into wrapped_message and save shared_ptr of it
  //
  // NOTE:
  // see use of template function then template class. have to populate a class
  // for every T? it's easy and light to populate template functoin for each T.

  q.push(simple_message());

  // get std::shared_ptr<message_base>
  auto sp = q.wait_and_pop();

  // message->message_ is T which is copied in adn T.name_ to get name
  EXPECT_THAT(sp->message_.name_, "simple_message");
}
#endif

// when "pin_incorrect" is sent which is handled.
//
// [ RUN      ] CxxFsm.check_single_message_chain
// disp: ctor:
// TD: ctor: set chained_ true: pin_verified
// TD: ctor: set chained_ true: pin_incorrect
// TD: ctor: set chained_ true: cancel_pressed
// TD: ~TD: chained_ false: cancel_pressed
// TD: wait_dispatch_: cancel_pressed {
// sender thread: send message...
// TD: dispatch_: cancel_pressed: pass to prev:
// TD: dispatch_: pin_incorrect: call f: pin_incorrect
// atm::do pin_incorrect
// TD: wait_dispatch_: cancel_pressed }
// TD: ~TD: cancel_pressed
// TD: ~TD: pin_incorrect
// TD: ~TD: pin_verified
// disp: ~disp: end:

namespace fsm_ccon
{
  namespace client
  {
    void sender1(sender &sender)
    {
      std::this_thread::sleep_for(std::chrono::seconds(2));

      std::cout << "sender thread: send message..." << std::endl;

      sender.send(pin_incorrect());
    }

    void sender2(sender &sender)
    {
      std::this_thread::sleep_for(std::chrono::seconds(2));

      std::cout << "sender thread: send message..." << std::endl;

      sender.send(simple_message());
    }
  } // namespace client
} // namespace fsm_ccon

TEST(CxxFsm, check_single_message_chain_1)
{
  using namespace fsm_ccon;
  using namespace fsm_ccon::client;

  receiver in;

  // *cxx-conversion-op* which converts receiver to sender
  sender out = in;

  std::thread sender(sender1, std::ref(out));

  // set EAF in a STATE
  try
  {
    {
      in.wait()
        .handle<pin_verified>([&](const pin_verified &msg) {
          (void)msg;
          std::cout << "atm::verify_pin::pin_verified" << std::endl;
          std::cout << "state = &atm::wait_for_action" << std::endl;
          ;
        })
        .handle<pin_incorrect>([&](const pin_incorrect &msg) {
          std::cout << "atm::do pin_incorrect" << std::endl;
          EXPECT_THAT(msg.name_, "pin_incorrect");
        })
        .handle<cancel_pressed>([&](const cancel_pressed &msg) {
          (void)msg;
          std::cout << "atm::verify_pin::cancel_pressed" << std::endl;
          std::cout << "state = &atm::done_processing" << std::endl;
        });

      // note:
      // build next chain and if not, ends here since there is no further event
      // and waits.
      //
      // std::cout << "press.." << std::endl;
      // std::this_thread::sleep_for(std::chrono::seconds(1));
      //
      // incoming_.wait()
      //   .handle<pin_verified>
      //   (
      //    [&](const pin_verified &msg)
      //    {
      //    (void)msg;
      //    std::cout << "atm::verify_pin::pin_verified" << std::endl;
      //    std::cout << "state = &atm::wait_for_action" << std::endl;;
      //    }
      //   );
    }
  } catch (close_queue const &)
  {
    std::cout << "got close_queue message" << std::endl;
  }

  sender.join();
}

// when not gets expected message since sender2 sends `simple_message`
// so waits forever
//
// [ RUN      ] CxxFsm.check_single_message_chain_2
// disp: ctor:
// TD: ctor: set chained_ true: pin_verified
// TD: ctor: set chained_ true: pin_incorrect
// TD: ctor: set chained_ true: cancel_pressed
// TD: ~TD: chained_ false: cancel_pressed
// TD: wait_dispatch_: cancel_pressed {
// sender thread: send message...
// TD: dispatch_: cancel_pressed: pass to prev:
// TD: dispatch_: pin_incorrect: pass to prev:
// TD: dispatch_: pin_verified: pass to prev:
// disp: dispatch_: false
// TD: wait_dispatch_: cancel_pressed {
// ^C

TEST(CxxFsm, DISABLED_check_single_message_chain_2)
{
  using namespace fsm_ccon;
  using namespace fsm_ccon::client;

  receiver in;

  // *cxx-conversion-op* which converts receiver to sender
  sender out = in;

  std::thread sender(sender2, std::ref(out));

  try
  {
    {
      in.wait()
        .handle<pin_verified>([](const pin_verified &msg) {
          (void)msg;
          std::cout << "atm::verify_pin::pin_verified" << std::endl;
          std::cout << "state = &atm::wait_for_action" << std::endl;
          ;
        })
        .handle<pin_incorrect>([](const pin_incorrect &msg) {
          std::cout << "atm::do pin_incorrect" << std::endl;
          EXPECT_THAT(msg.name_, "pin_incorrect");
        })
        .handle<cancel_pressed>([](const cancel_pressed &msg) {
          (void)msg;
          std::cout << "atm::verify_pin::cancel_pressed" << std::endl;
          std::cout << "state = &atm::done_processing" << std::endl;
        });
    }
  } catch (close_queue const &)
  {
    std::cout << "got close_queue message" << std::endl;
  }

  sender.join();
}

namespace fsm_atm_ex
{
  // define events/messages
  struct money_inserted
  {
    unsigned amount_;
    std::string name_;
    explicit money_inserted(unsigned const &amount = 100)
        : amount_(amount)
        , name_{"money_inserted"}
    {}
    std::string get_name() { return name_; }
  };

  struct item_1_purchased
  {
    unsigned price_;
    std::string name_;
    explicit item_1_purchased()
        : price_(10)
        , name_{"item_1_purchased"}
    {}
    std::string get_name() { return name_; }
  };

  struct item_2_purchased
  {
    unsigned price_;
    std::string name_;
    explicit item_2_purchased()
        : price_(20)
        , name_{"item_2_purchased"}
    {}
    std::string get_name() { return name_; }
  };

  struct item_3_purchased
  {
    unsigned price_;
    std::string name_;
    explicit item_3_purchased()
        : price_(30)
        , name_{"item_3_purchased"}
    {}
    std::string get_name() { return name_; }
  };

  // define eafs of fsm

  class simple
  {
  public:
    simple() = default;

    simple(simple const &) = delete;
    simple &operator=(simple const &) = delete;

    // a sender is a wrapper to a message queue, a receiver owns it. You
    // can obtain a sender that references the queue by using the implicit
    // conversion.
    //
    // this is an interface to expose q to other class and used to set up
    // connection each other
    //
    // atm machine(bank.get_sender(), intarface_hardware.get_sender());
    //
    // atm machine keeps this sender copies to send messages to them.

    fsm_ccon::sender get_sender() { return incoming_; }

    // send a message to self
    void done()
    {
      std::cout << "simple fsm: send done" << std::endl;
      get_sender().send(fsm_ccon::close_queue());
    }

    void run()
    {
      // init state
      state = &simple::wait_for_money;

      std::cout << "starts simple fsm" << std::endl;

      try
      {
        for (;;)
        {
          (this->*state)();
        }
      } catch (fsm_ccon::close_queue const &)
      {
        std::cout << "ends simple fsm" << std::endl;
      }
    }

    // fsm states and each state defines EAF and a transition to next state

    void wait_for_money()
    {
      std::cout << "simple fsm: wait_for_money and waits..." << std::endl;

      incoming_.wait().handle<money_inserted>(
        [&](money_inserted const &message) {
          amount_ = message.amount_;
          std::cout << "simple fsm: money_inserted: amount left: " << amount_
                    << std::endl;

          // set next state
          state = &simple::wait_for_action;
        });
    }

    void wait_for_action()
    {
      std::cout << "simple fsm: wait_for_action and waits..." << std::endl;

      incoming_.wait()
        .handle<item_1_purchased>([&](item_1_purchased const &message) {
          amount_ -= message.price_;

          std::cout << "fsm: wait_for_action: item 1 purchased:"
                    << " amounts left: " << amount_ << std::endl;

          state = &simple::done_processing;
        })
        .handle<item_2_purchased>([&](item_2_purchased const &message) {
          amount_ -= message.price_;

          std::cout << "fsm: wait_for_action: item 2 purchased:"
                    << " amounts left: " << amount_ << std::endl;

          state = &simple::done_processing;
        })
        .handle<item_3_purchased>([&](item_3_purchased const &message) {
          amount_ -= message.price_;

          std::cout << "fsm: wait_for_action: item 3 purchased:"
                    << " amounts left: " << amount_ << std::endl;

          state = &simple::done_processing;
        });
    }

    void done_processing()
    {
      std::cout << "simple fsm: done_processing and no waits..." << std::endl;

      state = &simple::wait_for_money;
    }

  private:
    fsm_ccon::receiver incoming_;

    // state function pointer
    void (simple::*state)();
    unsigned amount_;
  };

} // namespace fsm_atm_ex

TEST(CxxFsm, check_simple_fsm1)
{
  using namespace fsm_ccon;
  using namespace fsm_atm_ex;

  simple fsm;

  // make thread which runs fsm
  std::thread fsm_thread(&simple::run, &fsm);

  // make sender to send a message

  sender fsm_queue(fsm.get_sender());

  fsm_queue.send(money_inserted(100));

  std::this_thread::sleep_for(std::chrono::seconds(10));

  fsm.done();
  fsm_thread.join();
}

TEST(CxxFsm, check_simple_fsm2)
{
  using namespace fsm_ccon;
  using namespace fsm_atm_ex;

  simple fsm;

  // make thread which runs fsm
  std::thread fsm_thread(&simple::run, &fsm);

  // make sender to send a message

  sender fsm_queue(fsm.get_sender());

  fsm_queue.send(money_inserted(100));
  std::this_thread::sleep_for(std::chrono::seconds(2));

  fsm_queue.send(item_2_purchased());
  std::this_thread::sleep_for(std::chrono::seconds(2));

  fsm.done();
  fsm_thread.join();
}

// from listing_c.9.cpp

namespace fsm_case
{
  struct withdraw
  {
    std::string account;
    unsigned amount;
    mutable fsm_ccon::sender atm_queue;
    withdraw(std::string const &account_,
             unsigned amount_,
             fsm_ccon::sender atm_queue_)
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
    mutable fsm_ccon::sender atm_queue;
    verify_pin(std::string const &account_,
               std::string const &pin_,
               fsm_ccon::sender atm_queue_)
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
    mutable fsm_ccon::sender atm_queue;
    get_balance(std::string const &account_, fsm_ccon::sender atm_queue_)
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
    fsm_ccon::receiver incoming;
    std::mutex iom;

  public:
    void done() { get_sender().send(fsm_ccon::close_queue()); }

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
      } catch (fsm_ccon::close_queue &)
      {}
    }
    fsm_ccon::sender get_sender() { return incoming; }
  };

  class bank_machine
  {
    fsm_ccon::receiver incoming;
    unsigned balance_;

  public:
    bank_machine()
        : balance_(199)
    {}
    void done() { get_sender().send(fsm_ccon::close_queue()); }

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
      } catch (fsm_ccon::close_queue const &)
      {}
    }
    fsm_ccon::sender get_sender() { return incoming; }
  };

  class atm
  {
  private:
    fsm_ccon::receiver incoming_;
    fsm_ccon::sender bank_;
    fsm_ccon::sender interface_hardware_;

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
    atm(fsm_ccon::sender bank, fsm_ccon::sender interface_hardware)
        : bank_(bank)
        , interface_hardware_(interface_hardware)
    {}

    atm(const atm &) = delete;
    atm &operator=(const atm &) = delete;

    // send a message to self
    void done() { get_sender().send(fsm_ccon::close_queue()); }

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
      } catch (fsm_ccon::close_queue const &)
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
    fsm_ccon::sender get_sender() { return incoming_; }
  };
} // namespace fsm_case

// note:
// the example in the text uses three threads and three queue:
//
// std::thread bank_thread(&bank_machine::run,&bank);
// std::thread if_thread(&interface_machine::run,&interface_hardware);
// std::thread atm_thread(&atm::run,&machine);

TEST(CxxFsm, check_case_cancel_pin)
{
  using namespace fsm_case;

  bank_machine bank;
  interface_machine interface_hardware;
  atm machine(bank.get_sender(), interface_hardware.get_sender());

  std::thread atm_thread(&atm::run, &machine);

  fsm_ccon::sender atmqueue(machine.get_sender());

  atmqueue.send(card_inserted("JW1234"));
  atmqueue.send(digit_pressed('3'));
  atmqueue.send(digit_pressed('3'));
  atmqueue.send(digit_pressed('0'));
  atmqueue.send(cancel_pressed());

  std::this_thread::sleep_for(std::chrono::seconds(1));
  machine.done();
  atm_thread.join();
}

TEST(CxxFsm, check_case_do_withdraw)
{
  using namespace fsm_case;

  bank_machine bank;
  interface_machine interface_hardware;
  atm machine(bank.get_sender(), interface_hardware.get_sender());

  std::thread atm_thread(&atm::run, &machine);

  fsm_ccon::sender atmqueue(machine.get_sender());

  atmqueue.send(card_inserted("JW1234"));
  atmqueue.send(digit_pressed('3'));
  atmqueue.send(digit_pressed('3'));
  atmqueue.send(digit_pressed('0'));
  atmqueue.send(digit_pressed('1'));

  // message from bank fsm
  atmqueue.send(pin_verified());

  atmqueue.send(withdraw_pressed(50));
  atmqueue.send(withdraw_ok());

  std::this_thread::sleep_for(std::chrono::seconds(1));
  machine.done();
  atm_thread.join();
}

// NOTE: without main(), it still builds and run

#if 0
// ={=========================================================================

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
#endif
