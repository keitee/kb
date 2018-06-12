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
 * RG100, HMS
 *--------------------------------------------------------------------------------
 * {FILENAME}
 *--------------------------------------------------------------------------------
 * SMS.C
 *
 * $Id: sms.c,v 1.1.1.1 2004/08/31 01:54:44 ktpark Exp $
 *--------------------------------------------------------------------------------
 * {DESCRIPTION}
 *--------------------------------------------------------------------------------
 * This file contains implementations of SMS
 *--------------------------------------------------------------------------------
 * {INITIAL AUTHOR}
 *--------------------------------------------------------------------------------
 * Kyoung Taek, Park, Humax. Contacts at ktpark@humaxdigital.com
 *--------------------------------------------------------------------------------
 * {ANKNOWLEDGMENTS}
 *--------------------------------------------------------------------------------
 * 
 *--------------------------------------------------------------------------------
 * {REVISION HISTORY}
 *--------------------------------------------------------------------------------
 * $Log: sms.c,v $
 * Revision 1.1.1.1  2004/08/31 01:54:44  ktpark
 * (STB) 초기작업
 *
 *================================================================================
 */

/* =================================================================================
 * I N C L U D E S
 * =================================================================================
 */    
#ifdef WIN32 
#include <windows.h>
#include <Mmsystem.h>
#endif

#include <stdio.h>
#include "watch.h"
#include "fsm.h"
#include "sms.h"


/* numeric keys from mw_keys.h
#define KEY_0						0x00
#define KEY_1						0x01
#define KEY_2						0x02
#define KEY_3						0x03
#define KEY_4						0x04
#define KEY_5						0x05
#define KEY_6						0x06
#define KEY_7						0x07
#define KEY_8						0x08
#define KEY_9						0x09
#define KEY_SHARP             0x13
#define KEY_ASTERISK          0x14
*/


void start_timer();
void stop_timer();
#ifdef WIN32
void CALLBACK timer_proc( UINT id, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2 );
#else
void timer_proc( unsigned long timerId, void *pdata);
#endif

UINT8 get_sms_code(UINT8 key, UINT8 count);
UINT8 get_sms_type(UINT8 key);


/* =================================================================================
 * G L O B A L S
 * =================================================================================
 */  
#define SMS_KEY_INVALID	(-1)
 
enum
{
	SMS_TYPE_INVALID	= 0,
	SMS_TYPE_ONE		= 1,
	SMS_TYPE_TWO		= 2,
	SMS_TYPE_THREE		= 3,
	SMS_TYPE_FOUR		= 4,
	SMS_TYPE_FIVE		= 5
};


/* ----------------------------------------------------------------------------------
 * TIMEOUT_VALUE
 * ----------------------------------------------------------------------------------
 * timer value in ms
 */
#define	TIMEOUT_VALUE	2000


#ifdef WIN32
/* USE MM TIMER */
static UINT		g_mmtimer_res	= 0;
static UINT		g_mmtimer_id	= 0;
#else
static unsigned long g_mmtimer_id = 0;
#endif

//static UINT8				g_sms_type		= SMS_TYPE_INVALID;
// FIX. mw key value can be 0x00, so init value of sms key was
// changed not to overlap with mw key range
static INT8					g_sms_key		= SMS_KEY_INVALID;
static UINT8				g_sms_count		= SMS_TYPE_INVALID;

static SMS_KEY_INFO*		g_key_info_table	= NULL;
static SMS_CALLBACK		g_handler			= NULL;


/* =================================================================================
 * S T A T E - E V E N T - D E F N I T I O N S
 * =================================================================================
 */  

/* ----------------------------------------------------------------------------------
 * SMS_STATE
 * ----------------------------------------------------------------------------------
 * state definitions of the sms
 */
typedef enum {
	ST_SMS_WAIT,
	ST_SMS_ONE,
	ST_SMS_TWO,
	ST_SMS_THREE,
	ST_SMS_FOUR,
	ST_SMS_FIVE,
	ST_SMS_TIMEOUT,
	ST_SMS_RESTART
} sms_state_t;


/* ----------------------------------------------------------------------------------
 * SMS_EVENT
 * ----------------------------------------------------------------------------------
 * event definitions of the sms
 */
typedef enum {
	EV_SMS_NEXT,
	EV_SMS_TIMEOUT,
	EV_SMS_DONE,
	EV_SMS_DIFFERENT,
	EV_SMS_WAIT,
	EV_SMS_BACK
} sms_event_t;


/* =================================================================================
 * T R A N S I T I O N - T A B L E - D E F N I T I O N S
 * =================================================================================
 */  

/* ----------------------------------------------------------------------------------
 * [WAIT**] state 
 * ----------------------------------------------------------------------------------
 */
static fsm_transition_tbl_t wait_state_trans_tbl[] = {
   { EV_SMS_WAIT, ST_SMS_WAIT },
   { EV_SMS_NEXT, ST_SMS_ONE },
   { EV_SMS_TIMEOUT, ST_SMS_TIMEOUT },
   { FSM_INVALID_EVENT_HDL, FSM_INVALID_STATE_HDL }
};

/* ----------------------------------------------------------------------------------
 * [ONE] state 
 * ----------------------------------------------------------------------------------
 */
static fsm_transition_tbl_t one_state_trans_tbl[] = {
	{ EV_SMS_WAIT, ST_SMS_WAIT },
	{ EV_SMS_BACK, ST_SMS_WAIT },
	{ EV_SMS_TIMEOUT, ST_SMS_TIMEOUT },
	{ EV_SMS_NEXT, ST_SMS_TWO },
	{ FSM_INVALID_EVENT_HDL, FSM_INVALID_STATE_HDL }
};

/* ----------------------------------------------------------------------------------
 * [TWO] state 
 * ----------------------------------------------------------------------------------
 */
static fsm_transition_tbl_t two_state_trans_tbl[] = {
	{ EV_SMS_WAIT, ST_SMS_WAIT },
	{ EV_SMS_BACK, ST_SMS_WAIT },
	{ EV_SMS_TIMEOUT, ST_SMS_TIMEOUT },
	{ EV_SMS_NEXT, ST_SMS_THREE },
	{ EV_SMS_DIFFERENT, ST_SMS_RESTART },
	{ FSM_INVALID_EVENT_HDL, FSM_INVALID_STATE_HDL }
};

/* ----------------------------------------------------------------------------------
 * [THREE] state 
 * ----------------------------------------------------------------------------------
 */
static fsm_transition_tbl_t three_state_trans_tbl[] = {
	{ EV_SMS_WAIT, ST_SMS_WAIT },
	{ EV_SMS_BACK, ST_SMS_WAIT },
	{ EV_SMS_TIMEOUT, ST_SMS_TIMEOUT },
	{ EV_SMS_NEXT, ST_SMS_FOUR },
	{ EV_SMS_DIFFERENT, ST_SMS_RESTART },
	{ FSM_INVALID_EVENT_HDL, FSM_INVALID_STATE_HDL }
};

/* ----------------------------------------------------------------------------------
 * [FOUR] state 
 * ----------------------------------------------------------------------------------
 */
static fsm_transition_tbl_t four_state_trans_tbl[] = {
	{ EV_SMS_WAIT, ST_SMS_WAIT },
	{ EV_SMS_BACK, ST_SMS_WAIT },
	{ EV_SMS_TIMEOUT, ST_SMS_TIMEOUT },
	{ EV_SMS_NEXT, ST_SMS_FIVE },
	{ EV_SMS_DIFFERENT, ST_SMS_RESTART },
	{ FSM_INVALID_EVENT_HDL, FSM_INVALID_STATE_HDL }
};

/* ----------------------------------------------------------------------------------
 * [FIVE] state 
 * ----------------------------------------------------------------------------------
 */
static fsm_transition_tbl_t five_state_trans_tbl[] = {
	{ EV_SMS_WAIT, ST_SMS_WAIT },
	{ EV_SMS_BACK, ST_SMS_WAIT },
	{ EV_SMS_TIMEOUT, ST_SMS_TIMEOUT },
	{ EV_SMS_NEXT, ST_SMS_ONE },
	{ EV_SMS_DIFFERENT, ST_SMS_RESTART },
	{ FSM_INVALID_EVENT_HDL, FSM_INVALID_STATE_HDL }
};

/* ----------------------------------------------------------------------------------
 * [TIMEOUT] state 
 * ----------------------------------------------------------------------------------
 */
static fsm_transition_tbl_t timeout_state_trans_tbl[] = {
	{ EV_SMS_DONE, ST_SMS_WAIT },
	{ FSM_INVALID_EVENT_HDL, FSM_INVALID_STATE_HDL }
};

/* ----------------------------------------------------------------------------------
 * [RESTART] state 
 * ----------------------------------------------------------------------------------
 */
static fsm_transition_tbl_t restart_state_trans_tbl[] = {
	{ EV_SMS_NEXT, ST_SMS_ONE },
	{ FSM_INVALID_EVENT_HDL, FSM_INVALID_STATE_HDL }
};

/* =================================================================================
 * S T A T E - D E F N I T I O N S
 * =================================================================================
 */  
/* ----------------------------------------------------------------------------------
 * [WAIT**] state 
 * ----------------------------------------------------------------------------------
 */
static fsm_state_t wait_state = {
   ST_SMS_WAIT,
   "sms wait st",
   (fsm_action_func_t)NULL,
   wait_state_trans_tbl
};

/* ----------------------------------------------------------------------------------
 * [ONE] state 
 * ----------------------------------------------------------------------------------
 */
static fsm_state_t one_state = {
   ST_SMS_ONE,
   "sms one st",
   (fsm_action_func_t)NULL,
   one_state_trans_tbl
};

/* ----------------------------------------------------------------------------------
 * [TWO] state 
 * ----------------------------------------------------------------------------------
 */
static fsm_state_t two_state = {
   ST_SMS_TWO,
   "sms two st",
   (fsm_action_func_t)NULL,
   two_state_trans_tbl
};

/* ----------------------------------------------------------------------------------
 * [THREE] state 
 * ----------------------------------------------------------------------------------
 */
static fsm_state_t three_state = {
   ST_SMS_THREE,
   "sms three st",
   (fsm_action_func_t)NULL,
   three_state_trans_tbl
};

/* ----------------------------------------------------------------------------------
 * [FOUR] state 
 * ----------------------------------------------------------------------------------
 */
static fsm_state_t four_state = {
   ST_SMS_FOUR,
   "sms four st",
   (fsm_action_func_t)NULL,
   four_state_trans_tbl
};

/* ----------------------------------------------------------------------------------
 * [FIVE] state 
 * ----------------------------------------------------------------------------------
 */
static fsm_state_t five_state = {
   ST_SMS_FIVE,
   "sms five st",
   (fsm_action_func_t)NULL,
   five_state_trans_tbl
};

/* ----------------------------------------------------------------------------------
 * [TIMEOUT] state 
 * ----------------------------------------------------------------------------------
 */
static fsm_state_t timeout_state = {
   ST_SMS_TIMEOUT,
   "sms timeout st",
   (fsm_action_func_t)NULL,
   timeout_state_trans_tbl
};

/* ----------------------------------------------------------------------------------
 * [RESTART] state 
 * ----------------------------------------------------------------------------------
 */
static fsm_state_t restart_state = {
   ST_SMS_RESTART,
   "sms restart st",
   (fsm_action_func_t)NULL,
   restart_state_trans_tbl
};


/* =================================================================================
 * S T A T E - L I S T S
 * =================================================================================
 */  
static fsm_state_t* sms_state_list[] = {
	&wait_state,
	&one_state,
	&two_state,
	&three_state,
	&four_state,
	&five_state,
	&timeout_state,
	&restart_state,
	FSM_INVALID_STATE
};


/* =================================================================================
 * E V E N T - L I S T S
 * =================================================================================
 */  
static fsm_event_t sms_event_list[] = {
	{ EV_SMS_NEXT, "sms next ev", NULL },
	{ EV_SMS_TIMEOUT, "sms timeout ev", NULL },
	{ EV_SMS_DONE, "sms done ev", NULL },
	{ EV_SMS_DIFFERENT, "sms diff ev", NULL },
	{ EV_SMS_WAIT, "sms wait ev", NULL },
	{ EV_SMS_BACK, "sms back ev", NULL }
};


/* =================================================================================
 * F S M - I N S T A N C E
 * =================================================================================
 */  
static fsm_instance_t sms_fsm = {
	"sms", sms_state_list, sms_event_list, NULL, NULL
};


/* =================================================================================
 * E A F S
 * =================================================================================
 */

/* ----------------------------------------------------------------------------------
 ** FUNCTION :: WAIT_EAF
 * ----------------------------------------------------------------------------------
 ** OVERVIEW
 *    eaf
 *   
 ** SYNTAX
 *	  void 
 *     wait_eaf( fsm_instance_t* fsm, fsm_event_t* event)
 * 
 ** PROTOTYPE IN
 *    fsm.h
 *                                                                    
 ** PARAMETERS
 *  @ {fsm} is a pointer to given fsm object.
 *  @ {event} is a pointer to given event object.
 * 	 
 ** DESCRIPTION 
 *    - initialize sub layer and register control callback
 * 
 ** RETURN VALUE
 *  
 ** SEE ALSO
 *    the definitions of a sms fsm
 * ----------------------------------------------------------------------------------
 */      
static void wait_eaf( fsm_instance_t* fsm, fsm_event_t* event)
{  
	// state check
	if( !fsm_is_valid_incoming_event( fsm, event->hdl ) )
	{
		WATCH( SMS_ERR, "wait_eaf: invalid state change." );
		return;
	}

	WATCH( SMS_L2, "wait_eaf: event(%s).", event->name );

	// to prevent repeated waiting eaf from wait state
	if( fsm_get_previous_state_hdl(fsm) == ST_SMS_WAIT)
		return;

	WATCH( SMS_L3, "wait_eaf: g_sms_count(%d)", g_sms_count );

	if( event->hdl == EV_SMS_BACK )
	{
		WATCH( SMS_L3, "wait_eaf: back:" );
	}
	else
	{
		WATCH( SMS_L3, "wait_eaf: not back: clear key/count/tmr" );

		g_sms_count = 0;
		g_sms_key	= -1;
	}

#ifdef WIN32
	if(g_mmtimer_id)
#else
   // FIX. in case of vk timer in stb, expired timer should not be canceled.
	if((event->hdl != EV_SMS_DONE) && g_mmtimer_id)
#endif
	{
		WATCH( SMS_L3, "wait_eaf: stop tmr" );
		stop_timer();
	}
} // wait_eaf

/* ----------------------------------------------------------------------------------
 ** FUNCTION :: ONE_EAF
 * ----------------------------------------------------------------------------------
 ** OVERVIEW
 *    eaf
 *   
 ** SYNTAX
 *	  void 
 *     one_eaf( fsm_instance_t* fsm, fsm_event_t* event)
 * 
 ** PROTOTYPE IN
 *    fsm.h
 *                                                                    
 ** PARAMETERS
 *  @ {fsm} is a pointer to given fsm object.
 *  @ {event} is a pointer to given event object.
 * 	 
 ** DESCRIPTION 
 *    - initialize sub layer and register control callback
 * 
 ** RETURN VALUE
 *  
 ** SEE ALSO
 *    the definitions of a sms fsm
 * ----------------------------------------------------------------------------------
 */      
static void one_eaf( fsm_instance_t* fsm, fsm_event_t* event)
{  
	UINT8	type 	= 0;
	UINT8	key 	= 0;

	// state check
	if( !fsm_is_valid_incoming_event( fsm, event->hdl ) )
	{
		WATCH( SMS_ERR, "one_eaf: invalid state change." );
		return;
	}
	
	key = *((UINT8*)event->data);
	WATCH( SMS_L2, "one_eaf: event(%s).", event->name );
	WATCH( SMS_L3, "one_eaf: key(0x%X). g_sms_count(%d)", key, g_sms_count );
   
   // FIX. when a different key was entered from the BACK event, it should call
   // different callback
   if( (g_sms_key !=SMS_KEY_INVALID) && (g_sms_key != key) )
   {
      WATCH( SMS_L3, "one_eaf: diff key(0x%X) g_sms_key(0x%X).", key, g_sms_key );
   
      if(g_handler)
         g_handler(CALLBACK_DIFFERENT_KEY);
   }

	// save key
	g_sms_key 	= key;

	// inc count
	g_sms_count = SMS_TYPE_ONE;

	// check type
	if( g_sms_count == get_sms_type(key) )
	{
		WATCH( SMS_L3, "one_eaf: same type with g_sms_count(%d).", g_sms_count );
		fsm_post_event( &sms_fsm, EV_SMS_BACK, &g_sms_key );
	}

	WATCH( SMS_L3, "one_eaf: stop/start tmr" );
	if(g_mmtimer_id) { stop_timer(); }
	start_timer();

} // one_eaf

/* ----------------------------------------------------------------------------------
 ** FUNCTION :: TWO_EAF
 * ----------------------------------------------------------------------------------
 ** OVERVIEW
 *    eaf
 *   
 ** SYNTAX
 *	  void 
 *     two_eaf( fsm_instance_t* fsm, fsm_event_t* event)
 * 
 ** PROTOTYPE IN
 *    fsm.h
 *                                                                    
 ** PARAMETERS
 *  @ {fsm} is a pointer to given fsm object.
 *  @ {event} is a pointer to given event object.
 * 	 
 ** DESCRIPTION 
 *    - initialize sub layer and register control callback
 * 
 ** RETURN VALUE
 *  
 ** SEE ALSO
 *    the definitions of a sms fsm
 * ----------------------------------------------------------------------------------
 */      
static void two_eaf( fsm_instance_t* fsm, fsm_event_t* event)
{  
	UINT8	key 	= 0;

	// state check
	if( !fsm_is_valid_incoming_event( fsm, event->hdl ) )
	{
		WATCH( SMS_ERR, "two_eaf: invalid state change." );
		return;
	}

	key = *((UINT8*)event->data);

	WATCH( SMS_L2, "two_eaf: event(%s).", event->name );
	WATCH( SMS_L3, "two_eaf: key(0x%X). g_sms_count(%d)", key, g_sms_count );

	if( g_sms_key != key )
	{
		WATCH( SMS_L3, "two_eaf: diff key(0x%X) g_sms_key(0x%X).", key, g_sms_key );

		// update sms_key for restart
		g_sms_key = key;

		fsm_post_event( &sms_fsm, EV_SMS_DIFFERENT, NULL );
		return;
	}

	// inc count
	g_sms_count = SMS_TYPE_TWO;

	// check type
	if( g_sms_count == get_sms_type(key) )
	{
		WATCH( SMS_L3, "two_eaf: same type with g_sms_count(%d).", g_sms_count );
		fsm_post_event( &sms_fsm, EV_SMS_BACK, &g_sms_key );
	}

	WATCH( SMS_L3, "two_eaf: stop/start tmr" );
	if(g_mmtimer_id) { stop_timer(); }
	start_timer();

} // two_eaf

/* ----------------------------------------------------------------------------------
 ** FUNCTION :: THREE_EAF
 * ----------------------------------------------------------------------------------
 ** OVERVIEW
 *    eaf
 *   
 ** SYNTAX
 *	  void 
 *     three_eaf( fsm_instance_t* fsm, fsm_event_t* event)
 * 
 ** PROTOTYPE IN
 *    fsm.h
 *                                                                    
 ** PARAMETERS
 *  @ {fsm} is a pointer to given fsm object.
 *  @ {event} is a pointer to given event object.
 * 	 
 ** DESCRIPTION 
 *    - initialize sub layer and register control callback
 * 
 ** RETURN VALUE
 *  
 ** SEE ALSO
 *    the definitions of a sms fsm
 * ----------------------------------------------------------------------------------
 */      
static void three_eaf( fsm_instance_t* fsm, fsm_event_t* event)
{  
	UINT8	key 	= 0;

	// state check
	if( !fsm_is_valid_incoming_event( fsm, event->hdl ) )
	{
		WATCH( SMS_ERR, "three_eaf: invalid state change." );
		return;
	}

	key = *((UINT8*)event->data);

	WATCH( SMS_L2, "three_eaf: event(%s).", event->name );
	WATCH( SMS_L3, "three_eaf: key(0x%X). g_sms_count(%d)", key, g_sms_count );

	if( g_sms_key != key )
	{
		WATCH( SMS_L3, "three_eaf: diff key(0x%X) g_sms_key(0x%X).", key, g_sms_key );

		// update sms_key for restart
		g_sms_key = key;

		fsm_post_event( &sms_fsm, EV_SMS_DIFFERENT, NULL );
		return;
	}

	// inc count
	g_sms_count = SMS_TYPE_THREE;

	// check type
	if( g_sms_count == get_sms_type(key) )
	{
		WATCH( SMS_L3, "three_eaf: same type with g_sms_count(%d).", g_sms_count );
		fsm_post_event( &sms_fsm, EV_SMS_BACK, &g_sms_key );
	}

	WATCH( SMS_L3, "three_eaf: stop/start tmr" );
	if(g_mmtimer_id) { stop_timer(); }
	start_timer();

} // three_eaf

/* ----------------------------------------------------------------------------------
 ** FUNCTION :: FOUR_EAF
 * ----------------------------------------------------------------------------------
 ** OVERVIEW
 *    eaf
 *   
 ** SYNTAX
 *	  void 
 *     four_eaf( fsm_instance_t* fsm, fsm_event_t* event)
 * 
 ** PROTOTYPE IN
 *    fsm.h
 *                                                                    
 ** PARAMETERS
 *  @ {fsm} is a pointer to given fsm object.
 *  @ {event} is a pointer to given event object.
 * 	 
 ** DESCRIPTION 
 *    - initialize sub layer and register control callback
 * 
 ** RETURN VALUE
 *  
 ** SEE ALSO
 *    the definitions of a sms fsm
 * ----------------------------------------------------------------------------------
 */      
static void four_eaf( fsm_instance_t* fsm, fsm_event_t* event)
{  
	UINT8	key 	= 0;

   // state check
   if( !fsm_is_valid_incoming_event( fsm, event->hdl ) )
   {
      WATCH( SMS_ERR, "four_eaf: invalid state change." );
      return;
   }

	key = *((UINT8*)event->data);

	WATCH( SMS_L2, "four_eaf: event(%s).", event->name );
	WATCH( SMS_L3, "four_eaf: key(0x%X). g_sms_count(%d)", key, g_sms_count );

	if( g_sms_key != key )
	{
		WATCH( SMS_L3, "four_eaf: diff key(0x%X) g_sms_key(0x%X).", key, g_sms_key );

		// update sms_key for restart
		g_sms_key = key;

		fsm_post_event( &sms_fsm, EV_SMS_DIFFERENT, NULL );
		return;
	}

	// inc count
	g_sms_count = SMS_TYPE_FOUR;

	// check type
	if( g_sms_count == get_sms_type(key) )
	{
		WATCH( SMS_L3, "four_eaf: same type with g_sms_count(%d).", g_sms_count );
		fsm_post_event( &sms_fsm, EV_SMS_BACK, &g_sms_key );
	}

	WATCH( SMS_L3, "four_eaf: stop/start tmr" );
	if(g_mmtimer_id) { stop_timer(); }
	start_timer();

} // four_eaf

/* ----------------------------------------------------------------------------------
 ** FUNCTION :: FIVE_EAF
 * ----------------------------------------------------------------------------------
 ** OVERVIEW
 *    eaf
 *   
 ** SYNTAX
 *	  void 
 *     five_eaf( fsm_instance_t* fsm, fsm_event_t* event)
 * 
 ** PROTOTYPE IN
 *    fsm.h
 *                                                                    
 ** PARAMETERS
 *  @ {fsm} is a pointer to given fsm object.
 *  @ {event} is a pointer to given event object.
 * 	 
 ** DESCRIPTION 
 *    - initialize sub layer and register control callback
 * 
 ** RETURN VALUE
 *  
 ** SEE ALSO
 *    the definitions of a sms fsm
 * ----------------------------------------------------------------------------------
 */      
static void five_eaf( fsm_instance_t* fsm, fsm_event_t* event)
{  
	UINT8	key 	= 0;

   // state check
   if( !fsm_is_valid_incoming_event( fsm, event->hdl ) )
   {
      WATCH( SMS_ERR, "five_eaf: invalid state change." );
      return;
   }

	key = *((UINT8*)event->data);

	WATCH( SMS_L2, "five_eaf: event(%s).", event->name );
	WATCH( SMS_L3, "five_eaf: key(0x%X). g_sms_count(%d)", key, g_sms_count );

	if( g_sms_key != key )
	{
		WATCH( SMS_L3, "five_eaf: diff key(0x%X) g_sms_key(0x%X).", key, g_sms_key );

		// update sms_key for restart
		g_sms_key = key;

		fsm_post_event( &sms_fsm, EV_SMS_DIFFERENT, NULL );
		return;
	}

	// inc count
	g_sms_count = SMS_TYPE_FIVE;

	// check type
	if( g_sms_count == get_sms_type(key) )
	{
		WATCH( SMS_L3, "five_eaf: same type with g_sms_count(%d).", g_sms_count );
		fsm_post_event( &sms_fsm, EV_SMS_BACK, &g_sms_key );
	}

	WATCH( SMS_L3, "five_eaf: stop/start tmr" );
	if(g_mmtimer_id) { stop_timer(); }
	start_timer();

} // five_eaf

/* ----------------------------------------------------------------------------------
 ** FUNCTION :: TIMEOUT_EAF
 * ----------------------------------------------------------------------------------
 ** OVERVIEW
 *    eaf
 *   
 ** SYNTAX
 *	  void 
 *     timeout_eaf( fsm_instance_t* fsm, fsm_event_t* event)
 * 
 ** PROTOTYPE IN
 *    fsm.h
 *                                                                    
 ** PARAMETERS
 *  @ {fsm} is a pointer to given fsm object.
 *  @ {event} is a pointer to given event object.
 * 	 
 ** DESCRIPTION 
 *    - initialize sub layer and register control callback
 * 
 ** RETURN VALUE
 *  
 ** SEE ALSO
 *    the definitions of a sms fsm
 * ----------------------------------------------------------------------------------
 */      
static void timeout_eaf( fsm_instance_t* fsm, fsm_event_t* event)
{  
   // state check
   if( !fsm_is_valid_incoming_event( fsm, event->hdl ) )
   {
      WATCH( SMS_ERR, "timeout_eaf: invalid state change." );
      return;
   }

   WATCH( SMS_L2, "timeout_eaf: event(%s).", event->name );

	if(g_handler)
		g_handler(CALLBACK_TIMEOUT);

	fsm_post_event( &sms_fsm, EV_SMS_DONE, NULL );

} // timeout_eaf

/* ----------------------------------------------------------------------------------
 ** FUNCTION :: RESTART_EAF
 * ----------------------------------------------------------------------------------
 ** OVERVIEW
 *    eaf
 *   
 ** SYNTAX
 *	  void 
 *     restart_eaf( fsm_instance_t* fsm, fsm_event_t* event)
 * 
 ** PROTOTYPE IN
 *    fsm.h
 *                                                                    
 ** PARAMETERS
 *  @ {fsm} is a pointer to given fsm object.
 *  @ {event} is a pointer to given event object.
 * 	 
 ** DESCRIPTION 
 *    - initialize sub layer and register control callback
 * 
 ** RETURN VALUE
 *  
 ** SEE ALSO
 *    the definitions of a sms fsm
 * ----------------------------------------------------------------------------------
 */      
static void restart_eaf( fsm_instance_t* fsm, fsm_event_t* event)
{  
	// state check
	if( !fsm_is_valid_incoming_event( fsm, event->hdl ) )
	{
		WATCH( SMS_ERR, "restart_eaf: invalid state change." );
		return;
	}

	WATCH( SMS_L2, "restart_eaf: event(%s).", event->name );

	WATCH( SMS_L3, "restart_eaf: stop tmr" );
	if(g_mmtimer_id) { stop_timer(); }

	g_sms_count = SMS_TYPE_INVALID;

	if(g_handler)
		g_handler(CALLBACK_DIFFERENT_KEY);

	WATCH( SMS_L3, "restart_eaf: saved diff key(0x%X).", g_sms_key );

	fsm_post_event( &sms_fsm, EV_SMS_NEXT, &g_sms_key );

} // restart_eaf



/* =================================================================================
 * I N T E R N A L - I N T E R F A C E S
 * =================================================================================
 */  
UINT8 get_sms_type(UINT8 key)
{
	UINT32 idx = 0;

	for( idx = 0; idx < SMS_MAX_KEY; idx++ )
	{
		if( key == g_key_info_table[idx].key )
		{
			WATCH( SMS_L3, "get_sms_type: ret table(idx(%d), type(%d))", idx, g_key_info_table[idx].type );
			return g_key_info_table[idx].type; 
		}
	}

	WATCH( SMS_ERR, "get_sms_type: NOT FOUND: key(0x%X).", key );
	return SMS_TYPE_INVALID;
}


UINT8 get_sms_code(UINT8 key, UINT8 count)
{
	UINT32 idx = 0;

	for( idx = 0; idx < SMS_MAX_KEY; idx++ )
	{
		if( key == g_key_info_table[idx].key )
		{
			if( count > g_key_info_table[idx].type )
			{
				WATCH( SMS_ERR, "get_sms_code: mis type: key(0x%X), type(%d), count(%d)."
					, key, g_key_info_table[idx].type, count );
				return 0;
			}

			WATCH( SMS_L3, "get_sms_code: ret code(0x%X) for key(0x%X)", g_key_info_table[idx].codes[count-1], key );
			return g_key_info_table[idx].codes[count-1]; 
		}
	}

	WATCH( SMS_ERR, "get_sms_code: NOT FOUND: key(0x%X).", key );
	return SMS_TYPE_INVALID;
}

#ifdef WIN32
/*
 *		TIMER FOR WIN32
 */
void CALLBACK timer_proc( UINT id, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2 )
{
	WATCH( SMS_L3, "timer_proc: timer expired");

	fsm_post_event( &sms_fsm, EV_SMS_TIMEOUT, NULL );
}

void start_timer()
{
	TIMECAPS	tc;
	MMRESULT	result = 0;

	g_mmtimer_res	= 0;
	g_mmtimer_id	= 0;

	if (TIMERR_NOERROR == timeGetDevCaps(&tc,sizeof(TIMECAPS)))
	{
		g_mmtimer_res = min(max(tc.wPeriodMin, 10 ),tc.wPeriodMax);
		timeBeginPeriod(g_mmtimer_res);
	}

	result = timeSetEvent( TIMEOUT_VALUE, g_mmtimer_res, timer_proc, (DWORD)NULL, TIME_ONESHOT );
	if(result)
	{
		g_mmtimer_id = (UINT)result;
	}
}

void stop_timer()
{
	MMRESULT	result;

	if(!g_mmtimer_id)
	{
		WATCH_ERR( SMS_ERR, "stop_timer: timer id is null." );
		return;
	}

	result = timeKillEvent(g_mmtimer_id);
	if (TIMERR_NOERROR == result)
		g_mmtimer_id = 0;

	if (0 != g_mmtimer_res)
	{
		timeEndPeriod(g_mmtimer_res);
		g_mmtimer_res = 0;
	}
}

#else
/*
 *		TIMER FOR NEO
 */
void	timer_proc( unsigned long timerId, void *pdata)
{
	WATCH( SMS_L3, "timer_proc: timer expired");
	fsm_post_event( &sms_fsm, EV_SMS_TIMEOUT, NULL );	
}

void start_timer()
{
	if(g_mmtimer_id)
	{
		WATCH_ERR( SMS_ERR, "start_timer: timer id is not null. so stop" );
		VK_TIMER_Cancel(g_mmtimer_id);
	}
   
	VK_TIMER_EventAfter(TIMEOUT_VALUE, timer_proc, (void*)NULL, 0, &g_mmtimer_id);
	WATCH( SMS_L3, "start_timer: g_mmtimer_id(%d)", g_mmtimer_id );   
}

void stop_timer()
{
	if(!g_mmtimer_id)
	{
		WATCH_ERR( SMS_ERR, "stop_timer: timer id is null." );
		return;
	}

	WATCH( SMS_L3, "stop_timer: g_mmtimer_id(%d)", g_mmtimer_id);	
	VK_TIMER_Cancel(g_mmtimer_id);
	
	g_mmtimer_id = 0;
}

#endif


/* =================================================================================
 * E X T E R N A L - I N T E R F A C E S
 * =================================================================================
 */  
SMS_STATUS sms_open(SMS_KEY_INFO* table, SMS_CALLBACK handler )
{
	fsm_status_t	stat	= FALSE;

	if( !(table) || !(handler) )
		return SMS_STAT_ERR_ARG;

	g_key_info_table	= table;
	g_handler			= handler;

   WATCH_SET_LEVEL(WATCH_SMS|WATCH_LEVEL_6);
   
	wait_state.enter_action			= (fsm_action_func_t)wait_eaf;
	one_state.enter_action			= (fsm_action_func_t)one_eaf;
	two_state.enter_action			= (fsm_action_func_t)two_eaf;
	three_state.enter_action		= (fsm_action_func_t)three_eaf;
	four_state.enter_action			= (fsm_action_func_t)four_eaf;
	five_state.enter_action			= (fsm_action_func_t)five_eaf;
	timeout_state.enter_action		= (fsm_action_func_t)timeout_eaf;
	restart_state.enter_action		= (fsm_action_func_t)restart_eaf;

   stat = fsm_create( &sms_fsm, ST_SMS_WAIT, EV_SMS_WAIT, (void*) NULL );
   if(stat)
   {
      WATCH( SMS_L3, "sms_init: sms fsm was created." );
   }      
   else
   {
      WATCH_ERR( SMS_ERR, "sms_init: sms fms was not created." );
      return SMS_STAT_ERR;
   }

   return SMS_STAT_OK;
}

SMS_STATUS sms_close()
{
	fsm_post_event( &sms_fsm, EV_SMS_WAIT, NULL );
	return SMS_STAT_OK;
}

SMS_STATUS sms_get_code( UINT8 key, UINT8* pcode )
{
	UINT8 code = 0;
	
	fsm_post_event( &sms_fsm, EV_SMS_NEXT, (void*)&key );

	WATCH( SMS_L3, "sms_get_code: g_sms_count(0x%d)", g_sms_count );

	if(g_sms_count)
	{
		code = get_sms_code( key, g_sms_count );
	}

	WATCH( SMS_L3, "sms_get_code: arg key(0x%X), ret code(0x%X)", key, code );
	*pcode = code;
	return SMS_STAT_OK;
}
