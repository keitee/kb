/*================================================================================
 * Copyright 2001- by HUMAX Co., Ltd.
 * 
 * R&D Center 
 * Humax Venture Tower 271-2
 * Seohyeon-Dong, Bundang-Gu, Seongnam-Si,
 * Gyeonggi-Do, Korea(ROK)
 *
 * This source code is confidential, proprietary information of HUMAX and 
 * is an unpublished work of authorship protected by the copyright laws of 
 * the Korea(ROK). Unauthorized copying or use of this document or the program
 * contained herein, in original or modified form, is a violation of Korea law.
 *--------------------------------------------------------------------------------
 * {PROJECT}
 *--------------------------------------------------------------------------------
 * LINDEN, PRIMA
 *--------------------------------------------------------------------------------
 * {FILENAME}
 *--------------------------------------------------------------------------------
 * FSM.H
 *--------------------------------------------------------------------------------
 * {DESCRIPTION}
 *--------------------------------------------------------------------------------
 * This file contains the finite state machine (FSM) manager header file, 
 * including constant and data type definitions. The FSM manager offers an 
 * effective control mechanism that can be re-used and shared among many 
 * applications.  
 *
 * !NOTE
 * ------
 * Naming convention is modified and some code is revised. (2003.03, ktpark)
 *
 * This FSM version was minimized and modified to meet OpenTV environment and
 * non threaded model. (2001.08, ktpark)
 *--------------------------------------------------------------------------------
 * {INITIAL AUTHOR}
 *--------------------------------------------------------------------------------
 * Kyoung Taek, Park, Humax. Contacts at ktpark@humaxdigital.com
 *--------------------------------------------------------------------------------
 * {ANKNOWLEDGMENTS}
 *--------------------------------------------------------------------------------
 * Thanks to their initial work and efforts by...
 *
 * 1. COPYRIGHT 1988/96 BY REALTIME PERFORMANCE, INC.
 *                            158 COMMERCIAL STREET,        
 *                            SUNNYVALE, CA 94086           
 *                            (408) 245-6537
 * 2. C. D. Kang
 *--------------------------------------------------------------------------------
 * {REVISION HISTORY}
 *--------------------------------------------------------------------------------
 * Revision 1.1.1.3  2003/07/08 04:30:23  ktpark
 * -. handling increased compiler waring level (initialization and unused vars)
 *
 * Revision 1.1.1.2  2003/05/20 07:22:25  ktpark
 * -. mod for release build
 * -. mod for supporting control info, i.e. signal strength and bit error
 * -. add a ALIVE_MSG_DEBUG_ON define for testing proper cm_alive message flow
 * -. mod for comments
 *
 * Revision 1.1.1.1  2003/05/17 01:08:27  ktpark
 * initial check in
 *
 * Revision 1.1.1.1  2001/08/30 17:52:30  Administrator
 * - (2001.08.30 ktpark) initial release for OTV Paris application at Paris
 *================================================================================
 */

/*

fsm is a this instance 

fsm_instance_t
 - current state (make it a instance)
 - previous state
 - state list(array) ->  each entry defines
   - use enum          - state hdl(enum)
   - use invalid       - eaf pointer
                       - name
                       - transition table -> each transition table(array) defines
                                               - event, state pairs to define
                                               what input event this state
                                               can have and which state
                                               this state can go to
                                               - use loop to run through
                                               - use invalid marker to mark
                                               end item.

 - event list
   - name
   - void *data


o all static and global structures


o state and event enum

1 state and event handle enum *must not* have gaps when defined since it's used
  as an *index* in event and state list

2 use the end marker which is used in the post event loop to end search

3 helper macro

#define FSM_E(x) {x,#x}

static Fsm::fsm_event_t bootEventList[] = {
  FSM_E(BOOT_EVENT_CLOSED),
  FSM_E(BOOT_EVENT_READY),
  ...
}


o each states defines a trnsition table, one to one match

1 the post event loop use it to find next matching state corresponding to
  (incoming) posted event.

2 For common evnets for all state, then need to write transition table for each
  state to have that event. Also for transition to self state. All depends on
  how defines transition table.


o action function

1 this static fsm uses action function *for state but not event* 

2 Questions from the previous note: In this fsm, it runs its eaf when enters a
  state and this is actually state action compared to event action. How to
  handle when error occurs in eaf? Do we need not to make state transition
  depending on a return value of eaf? Currently runs eaf blindly.

  2014.02. This is up to eaf design but not to fsm and that means that a user of
  fsm do all necessary work and call state transition only when there is no
  error to change state.


o init state and event 

fsm_create(fsm, state, event, data);

1 uses init state to set current_state and use init event to call action
  function use it as argument of action function. Since it sets current state
  but not searching it through transition table, do not cause state transition.

2 fsm_create() will run AF without checking if the event is a valid in that
  state.


o post event

fsm_post_event(fsm, event, data); 

1 undefined or missing event in event enum or misssed in a state transition
  table, do nothing and ignored.

2 no use of data in the events list


o other considerations

1 no thread-safe support

2 no check if fsm_post_event is called in eaf. This use cause sync call and
  nested call. To decoule it, use async such as message mechanism.

*/


#ifndef __FSM_H__
#define __FSM_H__


/* =================================================================================
 * C O N S T A N T S - M A C R O S
 * =================================================================================
 */  
 
#ifndef NULL
#define NULL 0
#endif

// to mark the end of list
#define FSM_INVALID_STATE   nullptr
#define FSM_INVALID_EVENT   nullptr

// to mark the end of enums
// #define FSM_INVALID_STATE_HDL ((fsm_state_hdl)0)
#define FSM_INVALID_STATE_HDL ((fsm_state_hdl)-1)
#define FSM_INVALID_EVENT_HDL ((fsm_event_hdl)-1)


/* =================================================================================
 * D A T A T Y P E S
 * =================================================================================
 * enums and typdefs
 */  

#ifndef TRUE
#define TRUE 	1
#define FALSE	0
#endif

/* ----------------------------------------------------------------------------------
 * uint16, sint16
 * ----------------------------------------------------------------------------------
 */
#ifndef uint16
typedef unsigned short uint16;
#endif

#ifndef sint16
typedef signed short sint16;
#endif


/* ----------------------------------------------------------------------------------
 * fsm_status_t
 * ----------------------------------------------------------------------------------
 * return codes from fsm routines

typedef enum 
   FSM_OK,
   FSM_ERR,
   FSM_ERR_CREATE,
   FSM_ERR_NO_ENTER_ACTION,
   FSM_ERR_UNSUPPORTED_EVENT
} fsm_status_t;
*/

typedef bool fsm_status_t;


/* ----------------------------------------------------------------------------------
 * fsm_state_hdl
 * ----------------------------------------------------------------------------------
 */
typedef uint16 fsm_state_hdl;


/* ----------------------------------------------------------------------------------
 * fsm_event_hdl
 * ----------------------------------------------------------------------------------
 * because of FSM_INVALID_EVENT_HDL
 */
typedef sint16 fsm_event_hdl;


/* ----------------------------------------------------------------------------------
 * forward reference 
 * ----------------------------------------------------------------------------------
 */
typedef struct _fsm_instance_t fsm_instance_t;
typedef struct _fsm_event_t fsm_event_t;


/* ----------------------------------------------------------------------------------
 * fsm_action_func_t
 * ----------------------------------------------------------------------------------
 */
typedef void (*fsm_action_func_t)(struct _fsm_instance_t*, struct _fsm_event_t*);


/* ----------------------------------------------------------------------------------
 * fsm_event_t
 * ----------------------------------------------------------------------------------
 * hdl			 : handle
 * name		     : name
 */
typedef struct _fsm_event_t {
  fsm_event_hdl   hdl;
  char*           name;
  void*           data;
} fsm_event_t;


/* ----------------------------------------------------------------------------------
 * fsm_transition_tbl_t
 * ----------------------------------------------------------------------------------
 * event_hdl      : event handle
 * state_hdl      : state handle
 */
typedef struct {
  fsm_event_hdl   event_hdl;
  fsm_state_hdl   state_hdl;
} fsm_transition_tbl_t;


/* ----------------------------------------------------------------------------------
 * fsm_state_t
 * ----------------------------------------------------------------------------------
 * hdl            : state own handle
 * name           : state own name
 * action_func    : enter function of the state
 * transiton_tbl  : transition table of the state
 */
typedef struct {  
  fsm_state_hdl         hdl;
  char*                 name;
  fsm_action_func_t     enter_action;
  fsm_transition_tbl_t* transition_tbl;
} fsm_state_t;


/* ----------------------------------------------------------------------------------
 * fsm_instatnce_t
 * ----------------------------------------------------------------------------------
 * name           : the fsm name
 * state_list     : list of all the states
 * event_list     : list of all the events
 * current_state  : fsm's current state
 */  
typedef struct _fsm_instance_t {
   const char*              name;
   fsm_state_t**      state_list;
   fsm_event_t*       event_list;
   fsm_state_t*       current_state;
   fsm_state_t*       previous_state;
} fsm_instance_t;


/* =================================================================================
 * I N T E R F A C E S
 * =================================================================================
 */  
extern fsm_status_t  fsm_create( fsm_instance_t* fsm, fsm_state_hdl init_state, fsm_event_hdl init_event, void* data );
extern fsm_status_t  fsm_post_event( fsm_instance_t* fsm, fsm_event_hdl post_event, void* data );
extern fsm_state_hdl fsm_get_current_state_hdl( fsm_instance_t* fsm );
extern fsm_state_hdl fsm_get_previous_state_hdl( fsm_instance_t* fsm );
extern char*         fsm_get_current_state_name( fsm_instance_t* fsm );
extern char*         fsm_get_event_name( fsm_instance_t* fsm, fsm_event_hdl event );
extern bool          fsm_is_valid_incoming_event( fsm_instance_t* fsm, fsm_event_hdl event );

#endif /* __FSM_H__ */
