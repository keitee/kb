#ifndef EVENTS_H_
#define EVENTS_H_

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


#include "message.h"

// ={=========================================================================
// message types

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

typedef enum
{
  E_ATM_STATE_WAITING_FOR_CARD,   // current state
  E_ATM_STATE_GETTING_PIN,
  E_ATM_STATE_VERIFY_PIN,
  E_ATM_STATE_WAIT_FOR_ACTION,
  E_ATM_STATE_PROCESS_WITHDRAW,
  E_ATM_STATE_DONE_PROCESSING
} E_ATM_STATE;

#endif // EVENTS_H_
