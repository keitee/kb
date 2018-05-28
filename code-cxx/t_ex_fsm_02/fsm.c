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
 * FSM.C
 *
 * $Id: fsm.c,v 1.1.1.1 2004/08/31 01:54:44 ktpark Exp $
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
 * $Log: fsm.c,v $
 * Revision 1.1.1.1  2004/08/31 01:54:44  ktpark
 * (STB) 초기작업
 *
 * Revision 1.1.1.3  2003/06/20 10:18:41  ktpark
 * (revised fragmentation)
 * -. revised with new fragmentation handling scheme. Main features are:
 *    + redesigned for assembling data on each channel, i.e. channel base
 *    + redesigned with statechart and coded using fsm
 *    + added statistics functionality
 *    + moved some chores to pump task
 *    + added cvt handling
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
 *================================================================================
 */

/* 
 * F O R N O N P C E N V 
 * =================================================================================
 */  
#if !defined(__cplusplus)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#else
#include <iostream>
#endif

#include "fsm.h"
     
// if it is a valid state defined in this fsm

bool is_valid_state(fsm_instance_t* fsm, fsm_state_hdl state_enum)
{
  // loop in the state list
  for (uint32_t i = 0; fsm->state_list[i] != FSM_INVALID_STATE; ++i) 
  {
    if(i == state_enum) 
      return TRUE;
  }

  std::cout << "FSM: " << fsm->name 
    << ": is_valid_state: unsupported state handle(" << state_enum << ")" 
    << std::endl;

  // WATCH(WATCH_ALL|WATCH_LEVEL_ALWAYS, "FSM: %s: is_valid_state: unsupported state handle(%d) in this fsm.\n",
  //     fsm->name, state );

  return FALSE;
}

      
/* ----------------------------------------------------------------------------------
 ** FUNCTION = get_vaild_state
 *
 ** OVERVIEW
 *    find given state in the state list of the fsm object
 *   
 ** SYNTAX
 *	  fsm_state_t* 
 *     get_vaild_state( fsm_instance_t* fsm, fsm_state_hdl state_hdl )
 * 
 ** PROTOTYPE IN
 *    fsm.h
 *                                                                    
 ** PARAMETERS
 *  @ {fsm} is a pointer to given fsm object.
 * 	 
 *  @ {state_hdl} is an handle of the finding state.
 * 
 ** DESCRIPTION 
 *    The function get_vaild_state is used to search a valid state in the
 *    state list.
 * 
 ** RETURN VALUE
 *    fsm_state_t* that is valid and found is returned
 *    FSM_INVALID_STATE
 *  
 ** SEE ALSO
 *
 * ----------------------------------------------------------------------------------
 */      
fsm_state_t* get_vaild_state(fsm_instance_t* fsm, fsm_state_hdl state_enum)
{
  // note: not use this since assume that enums *do not* have any gaps when
  // defined.
  //
  // uint16 idx=0;
  //
  // while( fsm->state_list[idx] != FSM_INVALID_STATE ) 
  // {
  // 	if( fsm->state_list[idx]->hdl == state_hdl )
  // 		return fsm->state_list[idx];
  //
  // 	idx++;
  // }

  // loop in the state list
  for (uint32_t i = 0; fsm->state_list[i] != FSM_INVALID_STATE; ++i) 
  {
    if(i == state_enum) 
      return fsm->state_list[i];
  }

  std::cout << "FSM: " << fsm->name 
    << ": is_valid_state: unsupported state handle(" << state_enum << ")" 
    << std::endl;

  // WATCH(WATCH_ALL|WATCH_LEVEL_ALWAYS, "FSM: %s: get state: unsupported state handle(%d).\n", 
  // 	fsm->name, state_hdl );

  return FSM_INVALID_STATE;
}


/* ----------------------------------------------------------------------------------
 ** FUNCTION = call_state_actions
 *
 ** OVERVIEW
 *    execute an enter action of the state of the fsm object
 *   
 ** SYNTAX
 *	  fsm_status_t 
 *     call_state_actions( fsm_instance_t* fsm, fsm_event_hdl event )
 * 
 ** PROTOTYPE IN
 *    fsm.h
 *                                                                    
 ** PARAMETERS
 *  @ {fsm} is a pointer to given fsm object.
 * 	 
 *  @ {event} is an event causing state change.
 * 
 ** DESCRIPTION 
 *    The function call_state_actions is used to call enter action of the state.
 * 
 ** RETURN VALUE
 *    FSM_OK
 *    FSM_ERR_NO_ENTER_ACTION
 *  
 ** SEE ALSO
 *
 * ----------------------------------------------------------------------------------
 */      
void call_state_actions( fsm_instance_t* fsm, fsm_event_hdl event, void* event_data )
{
  /* in case there is enter function */
  if( fsm->current_state->enter_action ) 
  {
    fsm->event_list[event].data = (void*) event_data;

    fsm->current_state->enter_action((fsm_instance_t*)fsm, 
        (fsm_event_t*)&(fsm->event_list[event]) );
  }
}


/* ----------------------------------------------------------------------------------
 ** FUNCTION = fsm_create
 *
 ** OVERVIEW
 *    create and init fsm with related structures
 *   
 ** SYNTAX
 *	  fsm_status_t 
 *     fsm_create( fsm_instance_t* fsm, fsm_state_hdl init_state, fsm_event_hdl init_event )
 * 
 ** PROTOTYPE IN
 *    fsm.h
 *                                                                    
 ** PARAMETERS
 *  @ {fsm} is a pointer to given fsm object.
 * 	 
 *  @ {init_state} is start state of the given fsm object.
 * 
 *  @ {init_event} is init event of the start state.
 * 
 ** DESCRIPTION 
 *    The function fsm_create is called by a client in order to create fsm object
 *    and should be called before calling any fsm interfaces. 
 * 
 ** RETURN VALUE
 *    FSM_OK
 *    FSM_ERR_CREATE
 *  
 ** SEE ALSO
 *
 * ----------------------------------------------------------------------------------
 */      
fsm_status_t fsm_create( fsm_instance_t* fsm, fsm_state_hdl init_state, fsm_event_hdl init_event, void* init_data )
{
  fsm_status_t stat = FALSE;

  if(is_valid_state(fsm, init_state))
  {  
    fsm->previous_state = fsm->current_state = fsm->state_list[init_state];

    // call eaf and do not change state of fsm 
    call_state_actions( fsm, init_event, init_data );

    stat = TRUE;
  }

  return stat;
}


/* ----------------------------------------------------------------------------------
 ** FUNCTION = fsm_post_event
 *
 ** OVERVIEW
 *    post an event to fsm object
 *   
 ** SYNTAX
 *	  fsm_status_t 
 *     fsm_post_event( fsm_instance_t* fsm, fsm_event_hdl post_event ) 
 * 
 ** PROTOTYPE IN
 *    fsm.h
 *                                                                    
 ** PARAMETERS
 *  @ {fsm} is a pointer to given fsm object.
 * 	 
 *  @ {post_event} is an event to post to the given fsm object.
 * 
 ** DESCRIPTION 
 *    The function fsm_post_event is used to post an event to specified fsm object.
 * 
 ** RETURN VALUE
 *    FSM_OK
 *    FSM_ERR_UNSUPPORTED_EVENT
 *  
 ** SEE ALSO
 *
 * ----------------------------------------------------------------------------------
 */      
fsm_status_t fsm_post_event( fsm_instance_t* fsm, fsm_event_hdl post_event, void* event_data )
// fsm_status_t fsm_post_event( fsm_instance_t* fsm, fsm_event_hdl post_event )
{
  uint32_t i = 0;
  fsm_state_t* next_state = nullptr;

  // look in transition table of a state
  while ( fsm->current_state->transition_tbl[i].event_hdl != FSM_INVALID_EVENT_HDL ) 
  {
    // found match event
    if( fsm->current_state->transition_tbl[i].event_hdl == post_event ) 
    {
      // find valid state
      next_state = get_vaild_state( fsm, fsm->current_state->transition_tbl[i].state_hdl );

      // save state and make state transition
      fsm->previous_state = fsm->current_state;
      fsm->current_state = next_state;

      // use current state to call the right eaf
      call_state_actions( fsm, post_event, event_data );
      return TRUE; // FSM_OK;
    }

    ++i;
  }

  std::cout << "FSM: " << fsm->name 
    << ": post_event: unsupportred event(" <<  fsm->event_list[post_event].name
    << ") in state(" << fsm->current_state->name << ")"
    << std::endl;

  // WATCH(WATCH_ALL|WATCH_LEVEL_ALWAYS, "FSM: %s: post: unsupported event(%s) in state(%s).",
  // 	fsm->name, fsm->event_list[post_event].name, fsm->current_state->name );

  return FALSE; // FSM_ERR_UNSUPPORTED_EVENT;
}


/* ----------------------------------------------------------------------------------
 ** FUNCTION = fsm_get_current_state_hdl
 *
 ** OVERVIEW
 *    get a handle of current state of fsm object
 *   
 ** SYNTAX
 *	  fsm_state_hdl 
 *     fsm_get_current_state_hdl( fsm_instance_t* fsm )
 * 
 ** PROTOTYPE IN
 *    fsm.h
 *                                                                    
 ** PARAMETERS
 *  @ {fsm} is a pointer to given fsm object.
 * 	 
 ** DESCRIPTION 
 *    The utility function is used to get a handle of current state.
 * 
 ** RETURN VALUE
 *    a handle of current state
 *  
 ** SEE ALSO
 *
 * ----------------------------------------------------------------------------------
 */      
fsm_state_hdl fsm_get_current_state_hdl( fsm_instance_t* fsm )
{
   return fsm->current_state->hdl;
}

/* ----------------------------------------------------------------------------------
 ** FUNCTION = fsm_get_current_state_hdl
 *
 ** OVERVIEW
 *    get a handle of current state of fsm object
 *   
 ** SYNTAX
 *	  fsm_state_hdl 
 *     fsm_get_current_state_hdl( fsm_instance_t* fsm )
 * 
 ** PROTOTYPE IN
 *    fsm.h
 *                                                                    
 ** PARAMETERS
 *  @ {fsm} is a pointer to given fsm object.
 * 	 
 ** DESCRIPTION 
 *    The utility function is used to get a handle of current state.
 * 
 ** RETURN VALUE
 *    a handle of current state
 *  
 ** SEE ALSO
 *
 * ----------------------------------------------------------------------------------
 */      
fsm_state_hdl fsm_get_previous_state_hdl( fsm_instance_t* fsm )
{
   return fsm->previous_state->hdl;
}


/* ----------------------------------------------------------------------------------
 ** FUNCTION = fsm_get_current_state_name
 *
 ** OVERVIEW
 *    get state name of fsm object
 *   
 ** SYNTAX
 *	  char* 
 *     fsm_get_current_state_name( fsm_instance_t* fsm )
 * 
 ** PROTOTYPE IN
 *    fsm.h
 *                                                                    
 ** PARAMETERS
 *  @ {fsm} is a pointer to given fsm object.
 * 	 
 ** DESCRIPTION 
 *    The utility function is used to get a name of current state.
 * 
 ** RETURN VALUE
 *    a name of current state
 *  
 ** SEE ALSO
 *
 * ----------------------------------------------------------------------------------
 */      
char *fsm_get_current_state_name(fsm_instance_t* fsm)
{
   return fsm->current_state->name;
}

char *fsm_get_event_name(fsm_instance_t* fsm, fsm_event_hdl event_enum)
{
   return fsm->event_list[event_enum].name;
}


#if 0

// seems unnecessary since transition is already checked when event is posted
// so comment out

bool fsm_is_valid_incoming_event( fsm_instance_t* fsm, fsm_event_hdl event )
{
  uint16			idx=0;

  /* look for match event until it reaches end of transition table */
  while ( fsm->previous_state->transition_tbl[idx].event_hdl != FSM_INVALID_EVENT_HDL ) 
  {
    /* found match event */
    if( fsm->previous_state->transition_tbl[idx].event_hdl == event ) 
      return TRUE;

    idx++;
  }

  std::cout << "FSM: " << fsm->name 
    << ": is_valid_incoming_event: unsupportred event(" <<  fsm->event_list[post_event].name
    << ") from prev state(" << fsm->previous_state->name << ")"
    << std::endl;

  // WATCH(WATCH_ALL|WATCH_LEVEL_ALWAYS, "FSM: %s: post: unsupported event(%s) from prev state(%s).",
  //     fsm->name, fsm->event_list[event].name, fsm->previous_state->name );

  // error: can not find a valid event
  return FALSE;
}
#endif

