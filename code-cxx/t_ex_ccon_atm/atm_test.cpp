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
#include "atm.h"
#include "message.h"
#include "events.h"

using namespace messaging;

TEST(AtmFsm, CancelPin)
{
  bank_machine bank;
  interface_machine interface_hardware;
  atm machine(bank.get_sender(),interface_hardware.get_sender());

  std::thread atm_thread(&atm::run, &machine);

  sender atmqueue(machine.get_sender());

  atmqueue.send(card_inserted("JW1234"));
  atmqueue.send(digit_pressed('3'));
  atmqueue.send(digit_pressed('3'));
  atmqueue.send(digit_pressed('0'));
  atmqueue.send(cancel_pressed());

  std::this_thread::sleep_for(std::chrono::seconds(1));
  machine.done();
  atm_thread.join();
}


TEST(AtmFsm, DoWithdraw)
{
  bank_machine bank;
  interface_machine interface_hardware;
  atm machine(bank.get_sender(),interface_hardware.get_sender());

  std::thread atm_thread(&atm::run, &machine);

  sender atmqueue(machine.get_sender());

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


// ={=========================================================================

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
