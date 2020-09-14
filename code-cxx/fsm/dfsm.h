/*
    Copyright (c) NDS Limited 2006

    P R O P R I E T A R Y  & C O N F I D E N T I A L

    The copyright of this code and related documentation together with
    any other associated intellectual property rights are vested in
    NDS Limited and may not be used except in accordance with the terms
    of the licence that you have entered into with NDS Limited.
    Use of this material without an express licence from NDS Limited
    shall be an infringement of copyright and any other intellectual
    property rights that may be incorporated with this material.
*/

/*
*********************************************************************
Revision Properties:
--------------------
$User:$
$Revision:$
$Date:$
$Activity:$
$Comments:$
*********************************************************************
*/

/** 
* VRM - Viewing and Recording Manager
* 
* VRM FSM module Header File. Darwin revision
* 
* Author: Haim Shoham
* first deliver date 11/1/2006
*/

/* It is user's reponsibility to make sure states enumerators are defined with no gaps,
   e.g. 
	 rigth way:  
	   enum STATES { 
	     STATE_A,    --> defaulted to 0	 
		 STATE_B,	 --> defaulted to 1 
		 STATE_C     --> defaulted to 2
	   } MyStates;  
	 wrong way: 
	   enum STATES { 
		  STATE_A = 0,      
		  STATE_B = 2,	 --> gap here - 1 is missing
		  STATE_C = 3
       } MyStates;	        
*/

#ifndef _FSM_H_
#define _FSM_H_

/*****************************************************************************\
  Includes
\*****************************************************************************/

/*****************************************************************************\
  Definitions
\*****************************************************************************/

/* typedef  uint32_t FSM_HANDLE;*/
typedef uint32_t SYSTEM_STATUS;

#define SYSTEM_STATUS_IS_OK(status) (((uint32_t)(status)) == 1U)

#define NELEMENTS(a) (sizeof(a) / sizeof(*(a)))

/* SYSTEM_STATUS possible return values */
// #define VRM_FSM_OK			((SYSTEM_STATUS)VRM_STATUS_OK)
// #define VRM_FSM_FAILURE		((SYSTEM_STATUS)VRM_MAKE_SUBCOMP_ERR_STATUS(VRM_IF_UNDEF, SEVERITY_SEVERE, E_VRM_SUBCOMP_ID_MISC, VRM_STAT_VAL_FAILURE))
#define VRM_FSM_OK ((SYSTEM_STATUS)1)
#define VRM_FSM_FAILURE ((SYSTEM_STATUS)0)

#define VRM_FSM_ILLEGAL_STATE ((uint16_t)0xFFFFu)
#define VRM_FSM_ALL_STATES ((uint16_t)0xFFFE)

#define VRM_VSM_MAX_INSTANCE_NAME_LEN 30u

#define VRM_FSM_NAME_PREFIX_VRM_MONITOR "VRM_MONITOR_"

#define VRM_FSM_ILLEGAL_DEFINITION NULL

/* illegal instace handle value */
#define VRM_FSM_ILLEGAL_INSTANCE_HANDLE NULL

/* fsm definition handle */
typedef void *VRM_FSM_DEFINITION_HANDLE;

/* handle to FSM instance */
typedef void *VRM_FSM_INSTANCE_HANDLE;

/* typedef for action function */
typedef void (*VRM_FSM_ACTION)(void *data);

// Use this macro for make array of VRM_FSM_ENTRY.
#define VRM_FSM_E(w, x, y, z)                                                  \
  {                                                                            \
    (uint16_t) w, (uint16_t)x, (uint16_t)y, z, #w, #x                          \
  }

typedef struct _fsm_entry
{
  uint16_t
    state; // state to which this transition belongs or VRM_FSM_ALL_STATES
  uint16_t input;        // input on which the transition works
  uint16_t new_state;    // new state
  VRM_FSM_ACTION action; // action on this transition
  const char *state_str; // to support trace
  const char *input_str; // to support trace
} VRM_FSM_ENTRY;

// /*
// 	This structure is used to define timeouts. FSM uses it to detect when the state machine
// 	gets "stuck" in a state for longer than it should.
// */
//
// typedef struct _fsm_timeout {
// 	uint16_t state;				/* state at which we wish to start a timeout timer */
// 	uint32_t milli;				/* the number of milli for the timeout */
// 	uint16_t input;				/* the input to generate if the timeout expires */
// } VRM_FSM_TIMEOUT;

// FSM definition info structure
typedef struct _fsm_definition_info
{
  VRM_FSM_ENTRY *entries; // must be static during definition's life time

  uint16_t num_entries;

  // VRM_FSM_TIMEOUT* timeouts; must be static during definition's life time
  // uint16_t num_timeouts;

} VRM_FSM_DEFINITION_INFO;

// callback function that is called after every transition
typedef void (*VRM_FSM_TRANS_CB)(
  void *data, // user's pointed data supplied in VRM_FSM_INIT struct
  VRM_FSM_ENTRY *entry);

// typedef void (*FSM_TIMEOUT_CB)			   /* callback function that is called when timeout */
// (
//     void    *data,						   /* user's pointed data supplied in VRM_FSM_INIT struct */
//     uint16_t input
// );

// ={=========================================================================
// specific FSM instance associated data

typedef struct _fsm_init
{
  // FSM definition to construct instance from
  VRM_FSM_DEFINITION_HANDLE fsm_definition_handle;

  // used by tracing. any user data, that is sent to the user after actions and
  // timeouts
  void *data;

  // initial state to be set for the machine
  uint16_t init_state;

  // called on transition
  VRM_FSM_TRANS_CB callback;

  // called on timeout
  // FSM_TIMEOUT_CB   timeout_callback;

} VRM_FSM_INIT;

/*****************************************************************************\
  Functions prototypes
\*****************************************************************************/

/*===========================================================================*/
SYSTEM_STATUS VRM_FSM_Init(void);

/*!<
  \Description: Initializes the FSM module
  \Returns:
      VRM_FSM_OK - if opeartion succeeded
	  VRM_FSM_FAILURE - if operation failed
  \See also:
	  VRM_FSM_Term
\*===========================================================================*/

/*===========================================================================*/
SYSTEM_STATUS VRM_FSM_Term(void);

/*!<
  \Description: Terminates the FSM module
  \Returns:
      VRM_FSM_OK - if opeartion succeeded
	  VRM_FSM_FAILURE - if operation failed
  \See also:
	  VRM_FSM_Init
\*===========================================================================*/

/*===========================================================================*/

SYSTEM_STATUS
VRM_FSM_CreateDefinition(const VRM_FSM_DEFINITION_INFO *pFSM_definition_info,
                         VRM_FSM_DEFINITION_HANDLE *pHandle);
/*!<
  \Description: Creates a FSM definition
  \Parameters: 
      \in  pFSM_definition_info           definition info structure
	  \out pHandle						  handle to created definition
  \Returns:
      VRM_FSM_OK - if opeartion succeeded
	  VRM_FSM_FAILURE - if operation failed
  \See also:
	  VRM_FSM_DestroyDefinition
\*===========================================================================*/

/*===========================================================================*/
SYSTEM_STATUS
VRM_FSM_DestroyDefinition(VRM_FSM_DEFINITION_HANDLE fsm_def_handle);
/*!<
  \Description:  Destroys a FSM definition
  \Parameters:
      \in  fsm_def_handle                  handle to FSM definition to be destroyed
  \Returns: 
	  VRM_FSM_OK when destroyed. 
	  VRM_FSM_FAILURE if number of instances alive > 0
  \See also:
	  VRM_FSM_CreateDefinition
\*===========================================================================*/

/*===========================================================================*/
SYSTEM_STATUS VRM_FSM_CreateInstance(const VRM_FSM_INIT *pInit,
                                     VRM_FSM_INSTANCE_HANDLE *pHandle);
/*!<
  \Description:  initalizes a new FSM instance.
				
  \Parameters:   
      \in   pInit          initialization data 
	  \out  pHandle        handle to the created instance
  \Returns:
      VRM_FSM_OK - if opeartion succeeded
	  VRM_FSM_FAILURE - if operation failed
  \See also:
	  VRM_FSM_DestroyInstance
  \Remarks:
	-	When there is no more use in the instance, 
		VRM_FSM_DestroyInstance should be call to free allocated resources.
	-   First machine running state is the minimum enum value supplied in pInit-> entries 
		To an alternative initial state use VRM_FSM_SetState
	-	To set a name to the created instance Use VRM_FSM_SetName
\*===========================================================================*/

/*===========================================================================*/
SYSTEM_STATUS VRM_FSM_DestroyInstance(VRM_FSM_INSTANCE_HANDLE fsm_h);
/*!<
  \Description: Destroys a FSM instance, that was previously initialized using VRM_FSM_CreateInstance()
  \Parameters:
      \in fsm_h            FSM instance handle to be destroyed
  \Returns:  
	  VRM_FSM_OK - if opeartion succeeded
	  VRM_FSM_FAILURE - if operation failed
  \See also:	
	  VRM_FSM_CreateInstance
\*===========================================================================*/

/*===========================================================================*/
SYSTEM_STATUS VRM_FSM_GetState(VRM_FSM_INSTANCE_HANDLE fsm_h, uint16_t *pState);
/*!<
  \Description: Retrieve the current state of the FSM
  \Parameters:
      \in  fsm_h         handle to FSM instance
      \out pState        state of FSM instance
  \Returns:
      VRM_FSM_OK - if opeartion succeeded
	  VRM_FSM_FAILURE - when there is error in parameters, or if action function is currently performed
\*===========================================================================*/

/*===========================================================================*/
SYSTEM_STATUS VRM_FSM_SetName(VRM_FSM_INSTANCE_HANDLE fsm_h, const char *name);
/*!<
  \Description:  Sets a name to be associated with FSM instance 
  \Parameters:
      \fsm_               handle to FSM instance 
      \name               string to be set as name.
						  truncated to VRM_VSM_MAX_INSTANCE_NAME_LEN chars if needed
  \Returns: 
      VRM_FSM_OK - if opeartion succeeded
	  VRM_FSM_FAILURE - if there is error in parameters
\*===========================================================================*/

SYSTEM_STATUS VRM_FSM_Input(VRM_FSM_INSTANCE_HANDLE fsm_h, uint16_t input);
// !<
//   \Description: Injects an INPUT into the FSM.
//   \Parameters:
//       \in fsm_h          FSM instance handle
//       \in input          the input that is injected
//   \Returns:
//       VRM_FSM_OK - if opeartion succeeded
// 	  VRM_FSM_FAILURE - if operation failed
//   \See also:
// 	VRM_FSM_SetNextInput
//   \Remarks:
//   -	Caller should not call this function
// 	from within an action function, otherwise input won't be injected and
// 	VRM_FSM_FAILURE is returned.
//
// note:
// fsm->in_action
// check user is not calling this function from within an action function
//
// so supposed to be called in call path of receiveing message from a queue.

SYSTEM_STATUS VRM_FSM_SetNextInput(VRM_FSM_INSTANCE_HANDLE fsm_h,
                                   uint16_t input);
// !<
//   \Description: Injects input to a FSM instance , while action function is in
//   progress.
//
//   \Parameters:
//       \in fsm_h          FSM instance handle
//       \in input          the input that is injected
//
//   \Returns:
//       VRM_FSM_OK - if opeartion succeeded
// 	  VRM_FSM_FAILURE - if operation failed
//
//    \See also:
// 	  VRM_FSM_Input(),VRM_FSM_ClearNextInput()
//    \Remarks:
//
//    // note: not true when see code
//    // -	If the FSM instance is not busy on transition (action function is not
//    // performed) then error is returned.
//
//    -	If the FSM is currently busy on transition the input will be injected
//    immediately after the transition is concluded (i.e. action function returns).
//
//    -	If a previous call to VRM_FSM_SetNextInput has not been processed yet (
//    two calls are made from the same action function) the later call will
//    override the previous one.
//
// note:
// so this function is to be called in eaf and then when this deferred input
// event will be run?

/*===========================================================================*/
SYSTEM_STATUS VRM_FSM_ClearNextInput(VRM_FSM_INSTANCE_HANDLE fsm_h);

/*!<
  \Description: Allows the user to change his mind about previous call to VRM_FSM_SetNextInput
                assuming it has not been processed it. 
				(That is, both VRM_FSM_SetNextInput,VRM_FSM_ClearNextInput should be called from within
				the same action function for the latter to have effect.

  \Parameters:
      \in fsm_h          FSM instance handle
      \in input          the input that is injected
  \Returns:
      VRM_FSM_OK - if opeartion succeeded
	  VRM_FSM_FAILURE - if operation failed  
  \See also: VRM_FSM_SetNextInput()
\*===========================================================================*/

/*===========================================================================*/
SYSTEM_STATUS VRM_FSM_Reset(VRM_FSM_INSTANCE_HANDLE fsm_h);

/*!<
  \Description:	Reset the machine to initial state
				initial state - as sent in FSM_INIT struct on initialization of instace

  \Parameters:
      \in fsm_h          FSM instance handle

  \Returns:
      VRM_FSM_OK - if opeartion succeeded
	  VRM_FSM_FAILURE - if operation failed  
\*===========================================================================*/

/*===========================================================================*/
SYSTEM_STATUS VRM_FSM_SetState(VRM_FSM_INSTANCE_HANDLE fsm_h, uint16_t state);

/*!<
  \Description:	Move the machine to certain state						

  \Parameters:
      \in fsm_h          FSM instance handle
	  \in state - the state to move the instance to

  \Returns:
      VRM_FSM_OK - if opeartion succeeded
	  VRM_FSM_FAILURE - if operation failed  
\*===========================================================================*/

#endif /* _FSM_H_ */
