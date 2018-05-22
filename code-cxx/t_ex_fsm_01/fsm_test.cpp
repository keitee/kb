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
#include "events.h"

using namespace messaging;


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

static void runner(VRM_FSM_INSTANCE_HANDLE fsm)
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
    std::cout << "fsm_thread: got a message" << std::endl;

    if(msg == CONST_EXCEPTION_TO_END)
      break;

    VRM_FSM_Input(fsm, (uint16_t)msg);
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
static std::string account;
static std::string pin;

// external -> card_inserted

static void CardInserted(void* data)
{
  (void)data;

  std::cout << "atm::CardInserted:" << std::endl;
  std::cout << "atm::CardInserted:init account and pin" << std::endl;
}

// external -> digit_pressed

static void DigitPressed(void *data)
{
  (void)data;

  unsigned const pin_length = 4;
  pin += "1";
  std::cout << "atm::DigitPressed: " << pin << std::endl;

  if (pin.length() == pin_length)
  {
    send_message((uint16_t)E_ATM_INPUT_DIGIT_DONE);
  }
}

static void VerifyPin(void *data)
{
  (void)data;
  std::cout << "atm::VerifyPin:" << std::endl;
  send_message((uint16_t)E_ATM_INPUT_PIN_VERIFIED);
}

// external -> pin_verified
static void WaitForAction(void *data)
{
  (void)data;
  std::cout << "atm::WaitForAction:" << std::endl;
}

// external -> withdraw_pressed
static void WithdrawPressed(void *data)
{
  (void)data;
  std::cout << "atm::WithdrawPressed:" << std::endl;
  send_message((uint16_t)E_ATM_INPUT_WITHDRAW_OKAY);
}

// external -> withdraw_ok
static void DoneProcessing(void *data)
{
  (void)data;
  std::cout << "atm::DoneProcessing:" << std::endl;
  send_message((uint16_t)E_ATM_INPUT_END);
}

static void FsmOnTransition(void *data, const VRM_FSM_ENTRY *entry)
{
  std::cout << "FsmOnTransition: " 
    << " current state: " << (uint16_t) entry->state
    << " input event  : " << (uint16_t) entry->input
    << " new state    : " << (uint16_t) entry->new_state
    << std::endl;
}

static VRM_FSM_ENTRY AtmFsm[] = {

  VRM_FSM_E(
      E_ATM_STATE_WAITING_FOR_CARD,   // current state
      E_ATM_INPUT_CARD_INSERTED,      // input event
      E_ATM_STATE_GETTING_PIN,        // new state
      (VRM_FSM_ACTION)CardInserted    // action for new state
      ),

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
      (VRM_FSM_ACTION)nullptr
      ),
};


TEST(AtmFsm, RunAtmFsm)
{
  SYSTEM_STATUS	            stat = VRM_FSM_OK;
  VRM_FSM_DEFINITION_INFO   fsm_info;
  VRM_FSM_DEFINITION_HANDLE fsm_definition_handle{};
  VRM_FSM_INSTANCE_HANDLE   fsm_instance_handle{};
  VRM_FSM_INIT	            fsm_init;

  fsm_info.entries      = AtmFsm;
  fsm_info.num_entries  = (uint16_t)NELEMENTS(AtmFsm);

  stat = VRM_FSM_CreateDefinition(&fsm_info, &fsm_definition_handle);

  /* initialize fsm init data */ 
  fsm_init.fsm_definition_handle  = fsm_definition_handle;
  fsm_init.callback               = (VRM_FSM_TRANS_CB)FsmOnTransition;
  fsm_init.init_state             = (uint16_t)E_ATM_STATE_WAITING_FOR_CARD;
  fsm_init.data                   = (void*) nullptr;

  stat = VRM_FSM_CreateInstance(&fsm_init, &fsm_instance_handle);

  VRM_FSM_SetName(fsm_instance_handle, "atm");

  std::thread fsm_thread(runner, fsm_instance_handle);

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

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
