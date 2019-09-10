/*

o Use public structures in header and private structures in source code

o multiple instance shares the same definition.

  fsm instance(FSM_T) inited by init structure
  - current state
  - reference to fsm definition

  fsm definition inited by definition info
  - state x input array of pair<next state, eaf>

  VRM_FSM_ALL_STATES can support `super` state since it allows transition to new
  state for any event. Then how can use some of events or states to define super
  state?  Set VRM_FSM_ALL_STATES and carefully post event


*/


/*****************************************************************************\
  \Include files
\*****************************************************************************/

/* system include files */
#include <iostream>
#include <string>
#include <cstring>

/* project include files */
#include "dfsm.h"


/*****************************************************************************\
  \Definitions
\*****************************************************************************/
#define DIAG_DECLARE_FUNCTION(msg)      ((void)0)
#define DIAG_LOG_ERROR(id, msg)         ((void)0)
#define DIAG_LOG_MIL(id, msg)           ((void)0)
#define DIAG_LOG_INFO(id, msg)          ((void)0)


// State transition. Contains the new state and the action to be taken.

typedef struct fsm_trans {
  uint16_t new_state;     // new state
  VRM_FSM_ACTION action;  // action on this transition
} FSM_TRANS;


#if 0
/* Timeout. Contains the timeout milli and the input to be generated upon timeout. */
typedef struct fsm_timer {
  uint32_t   milli;
  uint16_t input;
} FSM_TIMER;
#endif


// ={=========================================================================
// fsm definition structure

typedef struct _fsm_definition { 

  VRM_FSM_DEFINITION_INFO def_info;

  uint16_t     min_state, max_state, num_states;
  uint16_t     min_input, max_input, num_inputs;

  // transitions table in size of states_num*inputs_num
  FSM_TRANS   *table;

  // timeouts array in size of states_num
  // FSM_TIMER   *timeouts;

  // number of live instances of this FSM definition
  uint16_t instances_count;

} FSM_DEFINITION;


// ={=========================================================================
// fsm instance structure
// FSM internal control structure - per instance.

typedef struct _fsm
{
  // initialization data
  VRM_FSM_INIT  init;

  // name of instance
  char          name[VRM_VSM_MAX_INSTANCE_NAME_LEN];

  // *current state of the FSM*
  uint16_t      state_offset;

  // previous state of the FSM
  uint16_t      pre_state_offset;

  // input value that will be processed
  uint16_t      input_offset;

  // true if input is available, false otherwise 
  bool          input_available;

#if 0
  uint16_t      timeout_input;						/* input to be sent as callback paramter when timeout */
  SYSTEMALARM_ALARM_HANDLE   alarm_handle;			/* alarm used for timeout */
  bool		 alarm_expected;	  /* true after alarm is set, false if alarm is not set, cancled or timed out */	
#endif

  // when action is preformed this flag is true 
  bool          in_action;

} *FSM_T;

 
/*****************************************************************************\
  \Constants
\*****************************************************************************/



/*****************************************************************************\
  \Variables
\*****************************************************************************/

static const char *par_err = "ER_FSM_101: Parameters error";
static const char *mem_err = "ER_FSM_102: unable to allocate/free memory";
static const char *ins_err = "ER_FSM_103: Instances still alive - can't destroy definition";
static const char *act_err = "ER_FSM_104: VRM_FSM_Input was called while executing action function";
static const char *act2_err = "ER_FSM_105: VRM_FSM_SetNextInput was called not from action function";
 

/*****************************************************************************\
  \Functions prototypes
\*****************************************************************************/

static void _fsm_error(const char *msg);      /* reports a fatal error and exits */


/* forward declartions */
static SYSTEM_STATUS FSMRun(VRM_FSM_INSTANCE_HANDLE fsm_h);

static SYSTEM_STATUS FSMSetState(VRM_FSM_INSTANCE_HANDLE fsm_h,
							     uint16_t state);

#if 0
static void FSMTimeout(SYSTEMALARM_ALARM_HANDLE handle,usHandle_t data);
#endif


/*****************************************************************************\
  \Functions bodies
\*****************************************************************************/

/*---------------------------------------------------------------------------*/ 
/* print the string corresponding with a given state */
// note: if see default return string, means not supported new state
static std::string TraceState(const FSM_T   fsm,uint16_t state) { 

  std::string ret{"<End State>"};
  uint32_t i;
  FSM_DEFINITION* def; 

  /* get the FSM definition */
  def = (FSM_DEFINITION*)(fsm->init.fsm_definition_handle);

  for (i = 0u; i < def->def_info.num_entries; i++) {
    if (def->def_info.entries[i].state == state) { 
      ret = def->def_info.entries[i].state_str;
      break;
    }
  }

  return ret;
}

/*---------------------------------------------------------------------------*/
/* print the string corresponding with a given input */
// note: if see default return string, means not supported new input
static std::string TraceInput(const FSM_T   fsm, uint16_t input)
{
  std::string ret{"<End Event>"};
  uint32_t i;
  FSM_DEFINITION* def; 

  /* get the FSM definition */
  def = (FSM_DEFINITION*)(fsm->init.fsm_definition_handle);

  for (i = 0u; i < def->def_info.num_entries; i++) {
    if (def->def_info.entries[i].input == input) {
      ret = def->def_info.entries[i].input_str;
      break;
    }
  }

  return ret;
}

#if 0
/* alarm callback function */
/*---------------------------------------------------------------------------*/
static void FSMTimeout(SYSTEMALARM_ALARM_HANDLE handle,usHandle_t data) { 

	SYSTEM_STATUS stat;
	/* avoid comiler warning, of not using this formal parameter*/
	/*handle = handle; */
		
    FSM_T fsm = (FSM_T)data; /*lint !e923
		this should be fixed later, currently this is protected by assert in the start of VRM_FSM_Init function */

	DIAG_DECLARE_FUNCTION("FSMTimeout"); 

    (fsm->init.timeout_callback)(fsm->init.data, fsm->timeout_input);


	/* wait, in what context this function is called? 
	   if it is a different thread than the VRM's, than there could be a race condition 
	   here*/ 

	fsm->alarm_expected = false;

	/* inject input */    
	stat = VRM_FSM_Input((VRM_FSM_INSTANCE_HANDLE)fsm,fsm->timeout_input); 

#ifdef VRM_DEBUG  
	if (!SYSTEM_STATUS_IS_OK(stat)) { 
		DIAG_LOG_ERROR(VRM_diag_segment_id,("fail to inject FSM input upon timeout. fsm name: %s",fsm->name));
	} 
#endif

	return; 
}
#endif


/*===========================================================================*/
SYSTEM_STATUS VRM_FSM_Init(void) { 

  SYSTEM_STATUS stat = VRM_FSM_OK;
  return stat;
} 


/*===========================================================================*/

SYSTEM_STATUS VRM_FSM_Term(void) { 

  SYSTEM_STATUS stat = VRM_FSM_OK;
  return stat;
} 


// ={=========================================================================
// private function
// change state by seeing current and previous state

static SYSTEM_STATUS 
FSMSetState(VRM_FSM_INSTANCE_HANDLE fsm_h, uint16_t state)
{
  SYSTEM_STATUS stat = VRM_FSM_OK;
  FSM_DEFINITION* def = NULL; 
  FSM_T fsm = (FSM_T)fsm_h;
  uint16_t state_offset = 0u;

#if 0
  SYSTEMALARM_ALARM_INFO info;
  SYSTEM_STATUS alarm_stat;
#endif

  DIAG_DECLARE_FUNCTION("FSMSetState"); 

  // check FSM is not performing an action
  if (fsm->in_action) {
    stat = VRM_FSM_FAILURE; 
  }

  if (SYSTEM_STATUS_IS_OK(stat)) { 
    // get the FSM definition
    def = (FSM_DEFINITION*)(fsm->init.fsm_definition_handle); 

    // calculate the state offset
    state_offset = state - def->min_state; 

    // check validity
    if (state_offset >= def->num_states) {  
      _fsm_error(par_err); 
      stat = VRM_FSM_FAILURE;
    }
  }

  if (SYSTEM_STATUS_IS_OK(stat)) { 
    // set the previous state and new state
    fsm->pre_state_offset = fsm->state_offset;
    fsm->state_offset = state_offset;

#if 0
    /* set the input that is injected when timeout from this state */
    fsm->timeout_input = def->timeouts[state].input;

    /* cancel previous alarm if needed */
    if (true == fsm->alarm_expected) { 
      alarm_stat = SYSTEMALARM_Cancel(fsm->alarm_handle);  

#ifdef VRM_DEBUG 
      if (!SYSTEM_STATUS_IS_OK(alarm_stat)) {
        DIAG_LOG_INFO(VRM_diag_segment_id,("Previous alarm could not be canceled. fsm->name: %s",fsm->name));
      } 
#endif

      fsm->alarm_expected = false;
    } 

    /* do we need to set a new alarm? */ 
    if (0u != def->timeouts[state].milli) {

      /* set the alarm */ 
      info.alarm_data.rel_info.callback = FSMTimeout;
      info.alarm_data.rel_info.user_data = (usWord_t)fsm_h; /*lint !e923:
                                                              this should be fixed later, currently this is protected by assert in the start of VRM_FSM_Init function */
      info.alarm_data.rel_info.is_repeating = false;
      info.alarm_data.rel_info.timeout = def->timeouts[state].milli; 

      info.is_absolute = false;

      fsm->alarm_expected = true;

      /* set the alarm */ 	
      alarm_stat = SYSTEMALARM_Set(&info,&(fsm->alarm_handle)); 

      if (SYSTEMALARM_STATUS_OK != alarm_stat) { 		   
        VRM_DIAG_ASSERT_ZERO("Alarm counld not be set"); 

        fsm->alarm_expected = false;
      }		 
    } 
#endif

  } 	 

  return stat; 
}


// ={=========================================================================
// public function
// set fsm name

SYSTEM_STATUS 
VRM_FSM_SetName(VRM_FSM_INSTANCE_HANDLE fsm_h, const char *name) 
{
  SYSTEM_STATUS stat = VRM_FSM_OK;
  FSM_T fsm = (FSM_T)fsm_h;

  if (fsm == NULL) {
    stat = VRM_FSM_FAILURE;
  }
  else {
    std::string fsm_name(name);
    fsm_name.copy(fsm->name, VRM_VSM_MAX_INSTANCE_NAME_LEN);
  }

  return stat;
}


// ={=========================================================================
// construct `definition` in memory from the definition info and return its
// pointer to the user
//
// defintion  - definition info
//              this is list and each entry defines a mapping 
//              <current state, input> -> <next state, action>
//
//            - transition table
//              dynamically made and which has <new state, action> pair
//
//                              x num of events
//              num of state | <trans> <trans> ... <trans>
//
// o builds dynamically transition table
//
// o supports VRM_FSM_ALL_STATES
// 
// o mapping between <current state, input> -> <next state, action>

SYSTEM_STATUS 
VRM_FSM_CreateDefinition(
    const VRM_FSM_DEFINITION_INFO* pFSM_definition_info,
    VRM_FSM_DEFINITION_HANDLE* pHandle)
{
  uint16_t     i = 0u;
  uint16_t     count = 0u;
  uint16_t     offset;
  uint16_t     state;
  uint16_t     first_state;
  uint16_t     last_state;
  VRM_FSM_ENTRY   *pEntry;
  FSM_TRANS   *pTrans;
  SYSTEM_STATUS stat = VRM_FSM_OK;
  FSM_DEFINITION* fsm_definition = NULL;

  DIAG_DECLARE_FUNCTION("VRM_FSM_CreateDefinition");

  // Check for parameter errors 
  if (pFSM_definition_info->entries == 0) { 
    _fsm_error(par_err); 
    stat = VRM_FSM_FAILURE;
  }

  // allocate space for definition structure 
  if (SYSTEM_STATUS_IS_OK(stat)) {
    fsm_definition = (FSM_DEFINITION*)malloc(sizeof(_fsm_definition));
  }

  // check for allocation error
  if (SYSTEM_STATUS_IS_OK(stat) && (fsm_definition == NULL)) { 
    _fsm_error(mem_err); 
    stat = VRM_FSM_FAILURE;
  }

  // set definition info
  if (SYSTEM_STATUS_IS_OK(stat)) { 

    // copy info
    fsm_definition->def_info.entries = pFSM_definition_info->entries;
    fsm_definition->def_info.num_entries = pFSM_definition_info->num_entries; 

    // fsm_definition->def_info.timeouts = pFSM_definition_info->timeouts; 
    // fsm_definition->def_info.num_timeouts = pFSM_definition_info->num_timeouts;

    // no instances at this time 
    fsm_definition->instances_count = 0u;

    // Scan for the range of states and inputs
    fsm_definition->min_state = (uint16_t) -1;
    fsm_definition->min_input = (uint16_t) -1;
    fsm_definition->max_state = 0u;
    fsm_definition->max_input = 0u;

    // calc min, max num of states, events, and count
    // Each fsm definition(class) has different state and input enum definition
    // and min always starts from 0 and max varies. This loop finds out min and
    // max of state and input for a definition from fsm transition table.

    for (i = 0u; i < pFSM_definition_info->num_entries; i++) {

      if ((pFSM_definition_info->entries[i].state != VRM_FSM_ILLEGAL_STATE) &&
          (pFSM_definition_info->entries[i].state != VRM_FSM_ALL_STATES )   ) {

        if (pFSM_definition_info->entries[i].state < fsm_definition->min_state) {
          fsm_definition->min_state = pFSM_definition_info->entries[i].state;
        }
        if (pFSM_definition_info->entries[i].state > fsm_definition->max_state) { 
          fsm_definition->max_state = pFSM_definition_info->entries[i].state;
        }
        if (pFSM_definition_info->entries[i].new_state < fsm_definition->min_state) {
          fsm_definition->min_state = pFSM_definition_info->entries[i].new_state;
        }
        if (pFSM_definition_info->entries[i].new_state > fsm_definition->max_state) {
          fsm_definition->max_state = pFSM_definition_info->entries[i].new_state;
        }
      }

      if (pFSM_definition_info->entries[i].input < fsm_definition->min_input) { 
        fsm_definition->min_input = pFSM_definition_info->entries[i].input;
      }
      if (pFSM_definition_info->entries[i].input > fsm_definition->max_input) {
        fsm_definition->max_input = pFSM_definition_info->entries[i].input;
      }
    }

    // get the number of state and inputs; for example, 0-3 means 4 entries and plus 1.
    fsm_definition->num_states = (fsm_definition->max_state - fsm_definition->min_state) + 1u;
    fsm_definition->num_inputs = (fsm_definition->max_input - fsm_definition->min_input) + 1u;
    count = fsm_definition->num_states * fsm_definition->num_inputs;

    std::cout << "fsm_definition::min_states: " << fsm_definition->min_state << std::endl;
    std::cout << "fsm_definition::max_states: " << fsm_definition->max_state << std::endl;
    std::cout << "fsm_definition::num_inputs: " << fsm_definition->num_inputs << std::endl;
    std::cout << "fsm_definition::num_states: " << fsm_definition->num_states << std::endl;
    std::cout << "fsm_definition::count     : " << count << std::endl;
  }

  if (SYSTEM_STATUS_IS_OK(stat)) { 

    /* Allocate the states transiiton table */    
    fsm_definition->table = 
      (FSM_TRANS *)malloc((uint32_t)count * (uint32_t)sizeof(FSM_TRANS));

    /* check for allocation error */
    if (fsm_definition->table == 0) {
      /* assert */ 
      _fsm_error(mem_err);
      (void)free(fsm_definition);
      fsm_definition = NULL;
      stat = VRM_FSM_FAILURE;
    }
  }

#if 0
  if (SYSTEM_STATUS_IS_OK(stat)) { 

    /* Allocate a timeouts array, setting all elements to 0 */ 
    fsm_definition->timeouts =
      (struct fsm_timer *)VRM_MEMMAN_API_CallocStaticP(VRM_MEMMAN_GetVrmMemoryPoolHandle(),
          (uint32_t)fsm_definition->num_states, 
          (uint32_t)sizeof(struct fsm_timer));


    /* check for allocation error */ 
    if (fsm_definition->timeouts == 0) { 

      /* free previous allocated resources */ 	   
      (void)VRM_MEMMAN_API_FreeStaticP(VRM_MEMMAN_GetVrmMemoryPoolHandle(),
          (void*)fsm_definition->table); 

      /* free the definition */ 
      (void)MEMMAN_API_FreeElementP(VRM_MEMMAN_GetVrmMemoryPoolHandle(),
          p_definitions_allocator, fsm_definition);			    

      fsm_definition = 0;
      _fsm_error(mem_err); 
      stat = VRM_FSM_FAILURE;
    }
  }
#endif

  // build dynamic state transition table from definition table
  //
  //            inputs
  //            0   1   2   3   4   5   6   7   8   9
  // old states
  // 0          X   0   X   0   X   X   X   X   X   X
  // 1          X   X   0   0   X   X   X   X   X   X 
  // 2          X   X   X   0   X   X   0   X   X   X 
  // 3          X   X   X   0   X   X   X   0   X   X 
  // 4          X   X   X   0   X   X   X   X   0   X 
  //
  // each entry is FSM_TRANS which is pair<new state, eaf> and this table defines
  // from(old state, input) to(new state, action)
  //
  // static state trnsiiton table defines current state and input(event) and
  // this loop set new state and action pair at index which corresponds to
  // item in state transition table. 
  //
  // Where 0 means set and X is not set. 
  //
  // So this trans table defines map between new state, action and current state
  // and input. 
  //
  // For ALL_STATES, set all state for the same input
  //
  //
  //
  // note: why do this? faster than searching through single array state
  // transition table?
  //
  // note: multiple path from one state? supported and set other column for the
  // same state low.

  if (SYSTEM_STATUS_IS_OK(stat)) { 
    /* initialize the entries in the table */
    for (i = 0u; i < count; i++) {
      fsm_definition->table[i].new_state = VRM_FSM_ILLEGAL_STATE;
    }

    // Fill the transition table using the given definition info 
    //
    // o if ALL_STATES then fill all states in column with the same value and
    // that means that any event can make state transition from that state.
    //
    // o otherwise, set specific [state, event] in the table

    for (i = 0u; i < pFSM_definition_info->num_entries; i++) 
    {
      // etnry from definition
      // `pFSM_definition_info->entries;` is input VRM_FSM_ENTRY list

      pEntry = pFSM_definition_info->entries + i;

      if (pEntry->state == VRM_FSM_ALL_STATES) {
        first_state = fsm_definition->min_state;
        last_state = fsm_definition->max_state;
      }
      else {
        first_state = pEntry->state;
        last_state = pEntry->state;
      }

      for (state = first_state; state <= last_state; state++) 
      {
        offset = (fsm_definition->num_inputs * (state - fsm_definition->min_state)) 
          + (pEntry->input - fsm_definition->min_input);

        pTrans = fsm_definition->table + offset;

        /* we keep the states offsets, rather than the states themself */
        pTrans->new_state = pEntry->new_state - fsm_definition->min_state;
        pTrans->action = pEntry->action;
      }
    }

#if 0
    /* Fill the timeouts array */
    for (i = 0u; i < pFSM_definition_info->num_timeouts; i++) {
      state = pFSM_definition_info->timeouts[i].state - fsm_definition->min_state;
      fsm_definition->timeouts[state].input = pFSM_definition_info->timeouts[i].input;
      fsm_definition->timeouts[state].milli = pFSM_definition_info->timeouts[i].milli;
    }    
#endif

    // return definition handle
    *pHandle = (VRM_FSM_DEFINITION_HANDLE)fsm_definition;
  } 

  if (!SYSTEM_STATUS_IS_OK(stat)) {
    *pHandle = VRM_FSM_ILLEGAL_DEFINITION;
  }

  return stat;
}


/*===========================================================================*/
SYSTEM_STATUS VRM_FSM_DestroyDefinition(VRM_FSM_DEFINITION_HANDLE fsm_def_handle) 
{
  SYSTEM_STATUS stat = VRM_FSM_OK; 
  FSM_DEFINITION* def = NULL; 

  DIAG_DECLARE_FUNCTION("VRM_FSM_DestroyDefinition");

  /* sanity checking */
  if (VRM_FSM_ILLEGAL_DEFINITION == fsm_def_handle) {
    _fsm_error(par_err);
    stat = VRM_FSM_FAILURE;
  }

  if (SYSTEM_STATUS_IS_OK(stat)) {
    def = (FSM_DEFINITION*)fsm_def_handle;

    /* do not allow to free space while there are live instances */
    if (def->instances_count > 0u) {
      DIAG_LOG_ERROR(VRM_diag_segment_id,("Cannot destroy Definition, (%d) instances are still attached", def->instances_count));
      _fsm_error(ins_err);
      stat = VRM_FSM_FAILURE;
    }
  }

  /* free allocated space */
  if (SYSTEM_STATUS_IS_OK(stat)) { 

    /* free the states table */ 	   
    (void)free((void*)def->table);

#if 0
    /* free the definition time outs table */ 	   
    (void)free((void*)def->timeouts); 
    DIAG_LOG_MIL(VRM_diag_segment_id,("FSM Defintion(%p) destroyed", def));
#endif 

    /* free the definition */
    (void)free((void*)def);
  } 

  return stat;
} 


// ={=========================================================================
// Once fsm definition is created, fill fsm init structure with:
//  definition, init state, data, callback
//
// Then call this to alloc fsm instance and set inital state

SYSTEM_STATUS 
VRM_FSM_CreateInstance(
    const VRM_FSM_INIT *pInit, 
    VRM_FSM_INSTANCE_HANDLE *pHandle)
{
  FSM_DEFINITION* def;
  SYSTEM_STATUS stat = VRM_FSM_OK;
  FSM_T new_fsm = 0;

  DIAG_DECLARE_FUNCTION("VRM_FSM_CreateInstance");

  // sanity checking
  if ((pInit == NULL) || (pInit->fsm_definition_handle == VRM_FSM_ILLEGAL_DEFINITION)) {
    _fsm_error(par_err);  
    stat = VRM_FSM_FAILURE; 
  }

  // Allocate FSM instance 
  if (SYSTEM_STATUS_IS_OK(stat)) { 

    new_fsm = (FSM_T)malloc(sizeof(_fsm));

    /* check for allocation error */
    if (NULL == new_fsm) {
      _fsm_error(mem_err);
      stat = VRM_FSM_FAILURE; 
    } 
  }

  // initialize the allocated structure
  if (SYSTEM_STATUS_IS_OK(stat)) { 

    // copy the initialization data
    memcpy(&(new_fsm->init), pInit, sizeof(VRM_FSM_INIT));

    new_fsm->input_available = false;

    // get the FSM definition to increase instance count for the used definition
    def = (FSM_DEFINITION*)(new_fsm->init.fsm_definition_handle);

    // this is flag to say fsm is running an action or not to make sure that
    // action is completed before doing next
    new_fsm->in_action = false;

#if 0
    /* alarm is not valid */ 
    new_fsm->alarm_expected = false;
#endif 

    // set the initial state
    stat = FSMSetState((VRM_FSM_INSTANCE_HANDLE)new_fsm,
        (uint16_t)(new_fsm->init.init_state - def->min_state));

    // consider error case
    if (!SYSTEM_STATUS_IS_OK(stat)) {   

      (void)free((void*)new_fsm);

      *pHandle = NULL;

      _fsm_error("Error encountered while trying to set initial state to new FSM instance");
    } 
    else {
      def->instances_count++;
    }
  }

  if (SYSTEM_STATUS_IS_OK(stat)) { 
    // set the handle
    *pHandle = (VRM_FSM_INSTANCE_HANDLE)new_fsm; 
  }

  return stat;  
}


/*===========================================================================*/
// get current state of a fsm instance
SYSTEM_STATUS VRM_FSM_GetState(VRM_FSM_INSTANCE_HANDLE fsm_h,
						   uint16_t *pState)
{
  SYSTEM_STATUS	stat = VRM_FSM_OK;
  FSM_T	fsm	= (FSM_T)fsm_h;
  FSM_DEFINITION* def = NULL;

  if (NULL == fsm_h) { 
    _fsm_error(par_err); 
    stat = VRM_FSM_FAILURE;
  }
  /*
     if (SYSTEM_STATUS_IS_OK(stat))  
     if (fsm->in_action)
     stat = VRM_FSM_FAILURE;
     */
  if (SYSTEM_STATUS_IS_OK(stat)) {
    def = (FSM_DEFINITION*)(fsm->init.fsm_definition_handle);
  }

  if (SYSTEM_STATUS_IS_OK(stat)) {
    *pState = fsm->state_offset + def->min_state;
  }

  return stat;
}


// ={=========================================================================
// send (input) event to fsm instance
//
// VRM_FSM_SetNextInput() vs VRM_FSM_Input() 
//
// o use event from argument when input is not set before or input from
//   VRM_FSM_SetNextInput().
//
// fsm->in_action
//
// check user is not calling this function "from within an action function"
//
// VRM_FSM_SetNextInput() is called in eaf to set next input event to be
// preocessed, say deferred event. Then when is this will be run?
//
// For this VRM fsm, eafs until E_PLAY_INT_STATE_WAIT_PANEL_CONFIG calls
// VRM_FSM_SetNextInput() to set next state and this enables that one async
// message runs through to this state.
//
// // first async message
//
// !ENTER  -VRM_SERVER   		< T:VRM_SRV M:basic_play.c F:PLAY_ReceiveMessage L:1090 > |-> PLAY_ReceiveMessage
// !ENTER  -VRM_SERVER   		< T:VRM_SRV M:basic_play.c F:PlaySendFSMInput L:4547 > |-> PlaySendFSMInput
// !ENTER  -VRM_SERVER   		< T:VRM_SRV M:fsm.c F:VRM_FSM_Input L:810 > |-> VRM_FSM_Input
// 
// !MIL    -VRM_SERVER   		< T:VRM_SRV M:fsm.c F:FSMRun L:922 > VRM-FSM: play:0x100: E_PLAY_INT_STATE_NONE -> (E_PLAY_INPUT_START) -> E_PLAY_INT_STATE_STARTING 
// !MIL    -VRM_SERVER   		< T:VRM_SRV M:fsm.c F:FSMRun L:922 > VRM-FSM: play:0x100: E_PLAY_INT_STATE_STARTING -> (E_PLAY_INPUT_RMF_ACQUIRE_RESOURCE) -> E_PLAY_INT_STATE_ALLOCATING_RESOURCES 
// !MIL    -VRM_SERVER   		< T:VRM_SRV M:fsm.c F:FSMRun L:922 > VRM-FSM: play:0x100: E_PLAY_INT_STATE_ALLOCATING_RESOURCES -> (E_PLAY_INPUT_RMF_RESOURCES_ALLOCATED) -> E_PLAY_INT_STATE_RESOURCES_ALLOCATED 
// !MIL    -VRM_SERVER   		< T:VRM_SRV M:fsm.c F:FSMRun L:922 > VRM-FSM: play:0x100: E_PLAY_INT_STATE_RESOURCES_ALLOCATED -> (E_PLAY_INPUT_PANEL_CONFIGURE) -> E_PLAY_INT_STATE_CONFIGURING 
// !MIL    -VRM_SERVER   		< T:VRM_SRV M:fsm.c F:FSMRun L:922 > VRM-FSM: play:0x100: E_PLAY_INT_STATE_CONFIGURING -> (E_PLAY_INPUT_WAIT_PANEL_CONFIG) -> E_PLAY_INT_STATE_WAIT_PANEL_CONFIG 
// 
// !EXIT   -VRM_SERVER   		< T:VRM_SRV M:fsm.c F:VRM_FSM_Input L:858 > <-| VRM_FSM_Input : return_status = 0xb400000
// !EXIT   -VRM_SERVER   		< T:VRM_SRV M:basic_play.c F:PlaySendFSMInput L:4569 > <-| PlaySendFSMInput : return_status = 0xb400000
// 
// // next async message
//
// !ENTER  -VRM_SERVER   		< T:VRM_SRV M:basic_play.c F:PlaySendFSMInput L:4547 > |-> PlaySendFSMInput
// !ENTER  -VRM_SERVER   		< T:VRM_SRV M:fsm.c F:VRM_FSM_Input L:810 > |-> VRM_FSM_Input
// !MIL    -VRM_SERVER   		< T:VRM_SRV M:fsm.c F:FSMRun L:922 > VRM-FSM: play:0x100: E_PLAY_INT_STATE_WAIT_PANEL_CONFIG -> (E_PLAY_INPUT_MCONN_CONFIGURED) -> E_PLAY_INT_STATE_CONFIGURED 
//
// HOW?
//
// SYSTEM_STATUS VRM_FSM_Input()
// {
//   /* run FSM while there is input available */
//   while (SYSTEM_STATUS_IS_OK(stat) && (true == fsm->input_available)) { 
//     stat = FSMRun(fsm_h); 
//   } 
// 
//   return stat;
// }
//
// This while loops do the trick. Firstly, run the first eaf and this set next
// state and exit. In the second iteration, run that agian until input_available
// is true.
//
// use fsm->in_action and fsm->input_available to manage when the second event
// comes while running action of the first and where while loops is used.
// suppors only one more.
//
// set input_available
//              clear input_available
//                                set in_action               
// |            |                 |
// input()    call run()          call action()
//
//                                  |
//                                  input()
// 
// Q: is it necessary since Input() and Run() are all async? so do not pick up
// the next until finishes the first?

SYSTEM_STATUS 
VRM_FSM_Input(VRM_FSM_INSTANCE_HANDLE fsm_h, uint16_t input)
{
  FSM_DEFINITION* def = NULL;
  FSM_T fsm = (FSM_T)fsm_h; 
  SYSTEM_STATUS stat = VRM_FSM_OK;

  DIAG_DECLARE_FUNCTION("VRM_FSM_Input");

  if (fsm == NULL) { 
    _fsm_error(par_err);
    stat = VRM_FSM_FAILURE;
  }

  // check user is not calling this function from within an action function 
  // if it does, set next state and return
  
  if (SYSTEM_STATUS_IS_OK(stat)) { 
    if (fsm->in_action) {
      _fsm_error(act_err);
      // stat = VRM_FSM_FAILURE;
      // in release mode, we will recover by:
      return VRM_FSM_SetNextInput(fsm_h, input);
    }
  }

  if (SYSTEM_STATUS_IS_OK(stat)) {
    def = (FSM_DEFINITION*)(fsm->init.fsm_definition_handle);
  }


  if (SYSTEM_STATUS_IS_OK(stat)) { 
    if ((input > def->max_input) || (input < def->min_input)) { 
      _fsm_error(par_err);  
      stat = VRM_FSM_FAILURE; 
    }
  } 

  // get event
  if  (SYSTEM_STATUS_IS_OK(stat)) { 
    if (!fsm->input_available) {
      fsm->input_available = true;
      fsm->input_offset = input - def->min_input;		  
    }
    else { 
      // this sould not happen
      stat = VRM_FSM_FAILURE;
      _fsm_error("Internal FSM error");
    }
  }

  // run FSM while there is input available 
  while (SYSTEM_STATUS_IS_OK(stat) && (true == fsm->input_available)) { 
    stat = FSMRun(fsm_h); 
  } 

  return stat;
}


// ={=========================================================================
// save next event while the first is in calling action

SYSTEM_STATUS 
VRM_FSM_SetNextInput(VRM_FSM_INSTANCE_HANDLE fsm_h, uint16_t input) 
{
  FSM_DEFINITION* def = NULL; 
  FSM_T fsm = (FSM_T)fsm_h; 
  SYSTEM_STATUS stat = VRM_FSM_OK;

  DIAG_DECLARE_FUNCTION("VRM_FSM_SetNextInput");

  if (NULL == fsm_h) { 
    _fsm_error(par_err);
    stat = VRM_FSM_FAILURE;
  }

  if (SYSTEM_STATUS_IS_OK(stat)) {
    def = (FSM_DEFINITION*)(fsm->init.fsm_definition_handle);
  }

  if (SYSTEM_STATUS_IS_OK(stat)) { 
    if ((input > def->max_input) || (input < def->min_input)) { 

#ifdef VRM_DEBUG
      DIAG_LOG_ERROR(VRM_diag_segment_id,("Severe error encountered. fsm->name=%s, input=%d, def->max_input=%d, def->min_input=%d",
            fsm->name,
            input,
            def->max_input,
            def->min_input));													
#endif
      _fsm_error(par_err);  
      stat = VRM_FSM_FAILURE; 
    }
  } 


  /* check if not executing action function */
  if (SYSTEM_STATUS_IS_OK(stat)) { 
    if (fsm->in_action == false) { 
      _fsm_error(act2_err);
      /*stat = VRM_FSM_FAILURE;*/

      /* in release mode, we recover by: */
      return VRM_FSM_Input(fsm_h,input);
    } 
    else { 	     
      /* set the input */ 
      fsm->input_available = true;
      fsm->input_offset = input - def->min_input;     
    } 
  }	

  return stat; 
}


#if 0
/* 
NGDEV-32730/MSCQ2866122, reduce VRM_MONITOR state change log, 
as VRM_MONITOR FSM might keep changing between two states during progressive download, such as [REPORT_EVALUATE, PLAYABLE], [REPORT_EVALUATE, NOT_PLAYABLE].
*/
static bool ignore_state_change_log(VRM_FSM_INSTANCE_HANDLE fsm_h, uint16_t new_state)
{
  bool        ignore = false;
  FSM_T       fsm = (FSM_T)fsm_h;

  if(new_state == fsm->pre_state_offset)
  {
    if(strncmp((const char *)fsm->name,VRM_FSM_NAME_PREFIX_VRM_MONITOR, strlen(VRM_FSM_NAME_PREFIX_VRM_MONITOR)) == 0)
    {
      ignore = true;
    }
  }
  return ignore;
}
#endif


// ={=========================================================================
// If gets unsupported event which is not defined for a state in fsm definition
// then they are silently ignored since their new state are mapped to illegal
// state and do nothing.

static SYSTEM_STATUS 
FSMRun(VRM_FSM_INSTANCE_HANDLE fsm_h) 
{
  SYSTEM_STATUS stat = VRM_FSM_OK;
  FSM_TRANS   *pTrans;
  uint16_t     offset;
  FSM_T       fsm = (FSM_T)fsm_h;
  VRM_FSM_ENTRY   cb_entry;
  FSM_DEFINITION* def = NULL;

#if 0
  SYSTEMTIME_TIMEDATE currentTime;                            /* The current time */
  char timeStrCurrent[SYSTEMTIME_MIN_STRING_BUFFER_SIZE] = "";/* String to print the date */
  int32_t timeSize = SYSTEMTIME_MIN_STRING_BUFFER_SIZE;           /* String size to print the date */
#endif 

  DIAG_DECLARE_FUNCTION("FSMRun");	 

  // in any case reset (input) event available flag
  fsm->input_available = false;

  def = (FSM_DEFINITION*)(fsm->init.fsm_definition_handle);

  // calculate offset in states table -> from old state (state_offset) & input
  offset = (fsm->state_offset * def->num_inputs) + fsm->input_offset; 

  // set a pointer to the table's entry
  // note: Unlike sfsm, do not loop to find transition

  pTrans = def->table + offset;

  // if(!ignore_state_change_log(fsm_h, pTrans->new_state))
  {
    std::cout << "VRM-FSM: " 
      << fsm->name 
      << ": " <<  TraceState(fsm, fsm->state_offset)
      << " -> (" << TraceInput(fsm, (uint16_t)(fsm->input_offset + def->min_input))
      << ") -> " << TraceState(fsm, pTrans->new_state)
      << std::endl;
  }

#if 0 /* getting current time is a very heavy operation */
  /* Get the current time */
  stat = SYSTEMTIME_GetTime(&currentTime);

  if (SYSTEM_STATUS_IS_OK(stat)) {
    stat = SYSTEMTIME_DebugString(currentTime, timeStrCurrent, &timeSize);
  }

  DIAG_LOG_INFO(VRM_diag_segment_id,("current time: %s (%d)\n", timeStrCurrent, currentTime));
#endif /* 0 */

  // L.R.
  if ( VRM_FSM_ILLEGAL_STATE == pTrans->new_state ) {	    
    _fsm_error("^__ Attention: Undefined fsm_entry !!!"); 					 
  }

  // Setup the callback if any
  if (fsm->init.callback != NULL) {
    cb_entry.state	= fsm->state_offset + def->min_state;
    cb_entry.input = fsm->input_offset + def->min_input;
    cb_entry.new_state = pTrans->new_state + def->min_state;
  }

  // If there is a real new state, set it
  if (pTrans->new_state != VRM_FSM_ILLEGAL_STATE) {

    // set new state, the state_offset + initialize timeout if needed
    stat = FSMSetState((VRM_FSM_INSTANCE_HANDLE)fsm, pTrans->new_state);

    if (SYSTEM_STATUS_IS_OK(stat)) { 
      fsm->in_action = true;

      // Carry out action
      if (pTrans->action) {
        (*pTrans->action)(fsm->init.data);
      }
      else // (!pTrans->action) 
      {
        std::cout << "VRM-FSM: action is null" << std::endl;
      }

      // Call the callback if any
      if (fsm->init.callback) {
        fsm->init.callback (fsm->init.data, &cb_entry);
      }

      fsm->in_action = false;	
    }	
  }

  return stat; 
}


#if 0
/*===========================================================================*/
SYSTEM_STATUS VRM_FSM_ClearNextInput(VRM_FSM_INSTANCE_HANDLE fsm_h) {
  
  SYSTEM_STATUS stat = VRM_FSM_OK;

  FSM_T fsm = (FSM_T)fsm_h; 

/*  DIAG_DECLARE_FUNCTION("VRM_FSM_ClearNextInput");
*/

  if (NULL == fsm) {    
	_fsm_error(par_err);
	stat = VRM_FSM_FAILURE;
  }

  /* reset input */
  if (SYSTEM_STATUS_IS_OK(stat)) {
    fsm->input_available = false; 
  }

  return stat;
}
#endif


/*===========================================================================*/
SYSTEM_STATUS VRM_FSM_DestroyInstance(VRM_FSM_INSTANCE_HANDLE fsm_h)
{
  SYSTEM_STATUS stat = VRM_FSM_OK;
  FSM_DEFINITION* def = NULL;
  FSM_T fsm = (FSM_T)fsm_h;

  DIAG_DECLARE_FUNCTION("VRM_FSM_DestroyInstance");

  /* Check for parameter errors. */
  if (fsm == NULL) { 
    _fsm_error(par_err); 
    stat = VRM_FSM_FAILURE;
  }

  if (SYSTEM_STATUS_IS_OK(stat)) { 

    /* get the definition */
    def = (FSM_DEFINITION*)(fsm->init.fsm_definition_handle);


    /* decrease instances count of this definition */
    if (def->instances_count > 0u) {
      def->instances_count--;
    }
    else {
      DIAG_LOG_ERROR(VRM_diag_segment_id,
          ("Something bad has happened. FSM instances count is zero (0) when trying to destroy an instance")); 

      stat = VRM_FSM_FAILURE;
    }
  }

  /* free the instance */
  if (SYSTEM_STATUS_IS_OK(stat)) {

    /* free the instance */ 
    (void)free((void*)fsm); 	   	
  }

  return stat;
}


/*===========================================================================*/
// reset a fsm to initial state
SYSTEM_STATUS VRM_FSM_Reset(VRM_FSM_INSTANCE_HANDLE fsm_h) 
{ 
  SYSTEM_STATUS stat = VRM_FSM_OK;
  FSM_T fsm = (FSM_T)fsm_h;

  /* Check for parameter errors. */
  if (fsm == NULL) { 
    _fsm_error(par_err); 
    stat = VRM_FSM_FAILURE;
  }

  /* set the machine to initial state */
  if (SYSTEM_STATUS_IS_OK(stat)) { 
    stat = FSMSetState(fsm_h,fsm->init.init_state);
  }

  return stat;
} 


/*===========================================================================*/
// set a state to a fsm
SYSTEM_STATUS VRM_FSM_SetState(VRM_FSM_INSTANCE_HANDLE fsm_h,uint16_t state) 
{
  SYSTEM_STATUS stat = VRM_FSM_OK;
  FSM_T fsm = (FSM_T)fsm_h;

  /* Check for parameter errors. */
  if (fsm == NULL) { 
    _fsm_error(par_err); 
    stat = VRM_FSM_FAILURE;
  }

  /* set the machine to given state */
  if (SYSTEM_STATUS_IS_OK(stat)) {
    stat = FSMSetState(fsm_h,state);
  }

  return stat;
}


/*---------------------------------------------------------------------------*/
static void _fsm_error(const char *msg)
{	 
  std::cout << "fsm_error: " << msg << std::endl;
}

