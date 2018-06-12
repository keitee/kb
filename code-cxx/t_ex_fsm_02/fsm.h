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

// fsm is a this instance 
//
// fsm_instance_t
//  - current state (make it a instance)
//  - previous state
//  - state list(array) ->  each entry defines
//    - use enum          - state hdl(enum)
//    - use invalid       - eaf pointer
//                        - name
//                        - transition table -> each transition table(array) defines
//                                                - event, state pairs to define
//                                                what input event this state
//                                                cancan have and which state
//                                                this state can go to
//                                                - use loop to run through
//                                                - use invalid marker to mark
//                                                end item.
//
//  - event list
//    - name
//    - void *data
//
// note:
// this fsm uses action function *for state but not event* 
//
// note:
// fsm_create() uses event to use it as argument of action function but not to
// trigger tansation.
//
// The init state which used to create fsm *may* have a transition to self which
// is a loop since it runs eaf and otherwise, fails on check of input event in
// eaf.
//
// note:
// fsm_post_event(instance, event, data) 
//  - undefined events in a state transition table do not have effect, ignored.
//  - get transition table from the current state and loop it to find out next
//  state correspoidng to input event.
//  - update current and previois state
//  - call action function
//    - use eaf from the current state and use event data from event list
//
// note:
// state and event handle enum *may not* have gaps when defined.
//
// * use loop only in transition table to find out inupt event, next state pair.
// * no thread-safe support
// * no check if fsm_post_event is called in eaf. This use cause sync call and
//   nested call. To decoule it, use async such as message mechanism.


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
   char*              name;
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
