#ifndef EVENTS_H_
#define EVENTS_H_

// #include "message.h"

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
