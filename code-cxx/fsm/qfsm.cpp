//
//  statemachine.cpp
//

#include "qfsm.h"
#include "slog.h"

#define LOG_ERROR LOG_MSG

#define G_UNLIKELY(x) __glibc_unlikely(x)

StateMachine::StateMachine()
    : m_transitionLogLevel(false)
    , m_currentState(-1)
    , m_initialState(-1)
    , m_finalState(-1)
    , m_running(false)
    , m_stopPending(false)
    , m_withinStateMover(false)
    , m_name()
{}

StateMachine::~StateMachine()
{
  cleanUpEvents();
}

// -----------------------------------------------------------------------------
/*!
  \internal

  Creates a log message string using \a oldState and \a newState and sends it
  out the designated log channel and category.

  \see StateMachine::setTransistionLogLevel()
  \see StateMachine::transistionLogLevel()
  \see StateMachine::transistionLogCategory()
 */
void StateMachine::logTransition(int oldState, int newState) const
{
  // create the logging message
  std::ostringstream message;

  if (oldState == newState)
  {
    message << "[" << name() << "] re-entering state "
            << m_states.at(newState).name << "(" << newState << ")";
  }
  else if (oldState == -1)
  {

    message << "[" << name() << "] moving to state "
            << m_states.at(newState).name << "(" << newState << ")";
  }
  else
  {

    message << "[" << name() << "] moving from state "
            << m_states.at(oldState).name << "(" << oldState << ")"
            << " to " << m_states.at(newState).name << "(" << newState << ")";
  }

  if (m_transitionLogLevel)
  {
    std::cout << message.str() << std::endl;
  }
}

bool StateMachine::transistionLogLevel() const
{
  return m_transitionLogLevel;
}

void StateMachine::setTransistionLogLevel(bool enable)
{
  m_transitionLogLevel = enable;
}

void StateMachine::cleanUpEvents()
{
  // clear all the queued events
  while (!m_localEvents.empty())
    m_localEvents.pop();

  // // clean up any delayed events with lock held
  // QMutexLocker locker(&m_delayedEventsLock);

  // QMap<qint64, DelayedEvent>::iterator it = m_delayedEvents.begin();
  // for (; it != m_delayedEvents.end(); ++it) {

  //   // kill the timer (safe to do in destructor ?)
  //   if (it->timerId >= 0)
  //     killTimer(it->timerId);
  // }

  // m_delayedEvents.clear();
}

std::list<int> StateMachine::stateTreeFor(int state, bool bottomUp) const
{
  std::list<int> tree;

  // to speed this up we don't do any checks on the state values, we assume
  // this is done when the states are added
  do
  {

    if (bottomUp)
      tree.push_back(state);
    else
      tree.push_front(state);

    state = m_states.at(state).parentState;

  } while (state >= 0);

  return tree;
}

void StateMachine::moveToState(int newState)
{
  // *transition to self*
  // if the new state is equal to the current state then this is not an error
  // and just means we have to issue the exited, transistion and entered
  // signals for the state

  if (newState == m_currentState)
  {

    logTransition(m_currentState, newState);

    // TODO
    m_exited(m_currentState);

    // want to catch transition
    // emit transition(m_currentState, m_currentState);

    m_entered(m_currentState);

    // the rest of the processing we can skip
  }
  else
  {

    // lookup the new state to check if we should be moving to an initial state
    auto it = m_states.find(newState);

    // if the state has one or more children then it's a super state and
    // we should be moving to the initial state
    if (it->second.hasChildren)
    {

      // sanity check we have an initial state
      if (G_UNLIKELY(it->second.initialState == -1))
      {
        LOG_ERROR("try to move to super state %s(%d) but no initial state set",
                  it->second.name.c_str(),
                  newState);
        return;
      }

      // set the new state to be the initial state of the super state
      LOG_ERROR("initialState (%d) is really used?", it->second.initialState);
      newState = it->second.initialState;
    }

    // *call eafs*
    int oldState   = m_currentState;
    m_currentState = newState;

    logTransition(oldState, newState);

    // get the set of states we're currently in (includes parents)
    //
    // (make a list of states from the given state up to its top parents)
    //
    // new: (new's parent, new) for enterd order
    // old: (old, old's parent) for exited order

    std::list<int> newStates = stateTreeFor(newState, false);
    std::list<int> oldStates = stateTreeFor(oldState, true);

    // emit the exit signal for any states we left
    for (const int &_oldState : oldStates)
    {
      if (std::find(newStates.cbegin(), newStates.cend(), _oldState) ==
          newStates.cend())
      {
        m_exited(_oldState);
      }
    }

    // TODO
    // // emit a transition signal
    // emit transition(oldState, m_currentState);

    // emit the entry signal for any states we've now entered
    for (const int &_newState : newStates)
    {
      if (std::find(oldStates.cbegin(), oldStates.cend(), _newState) ==
          oldStates.cend())
      {
        m_entered(_newState);
      }
    }
  }

  // // check if the new state is a final state of a super state, in which case
  // // post a FinishedEvent to the message loop
  // if (m_states[newState].isFinal) {
  //   postEvent(FinishedEvent);
  // }

  // check if the new state is a final state for the state machine and if so
  // stop the state machine
  if ((m_currentState == m_finalState) || m_stopPending)
  {

    m_running = false;
    cleanUpEvents();

    // TODO
    // if (m_currentState == m_finalState)
    //   emit finished();

    m_currentState = -1;
  }
}

void StateMachine::triggerStateMove(int newState)
{
  m_withinStateMover = true;

  // move to the new state, this will *emit signals* that may result in more
  // events being added to local event queue
  moveToState(newState);

  // then check if we have any other events on the queue, note we can get
  // into an infinite loop here if the code using the statemachine is
  // poorly designed, however that's their fault not mine
  while (m_running && !m_localEvents.empty())
  {
    const int event = m_localEvents.front();
    m_localEvents.pop();

    // check if this event should trigger a state move and if so
    // move the state once again
    newState = shouldMoveState(event);
    if (newState != -1)
      moveToState(newState);
  }

  m_withinStateMover = false;
}

int StateMachine::shouldMoveState(int event) const
{
  // check if this event triggers any transactions
  int state = m_currentState;

  do
  {

    // find the current state and sanity check it is in the map
    auto it = m_states.find(state);
    if (G_UNLIKELY(it == m_states.end()))
    {
      LOG_ERROR("invalid state %d (this shouldn't happen)", state);
      return -1;
    }

    // iterate through the transitions of this state and see if any trigger
    // on this event
    for (const Transition &transition : it->second.transitions)
    {
      if ((transition.type == Transition::EventTransition) &&
          (transition.event == event))
      {

        // return the state we should be moving to
        return transition.targetState;
      }
    }

    // note: this is interesting and it is how handle super state
    // event is not found in the transition list.
    // if this state had a parent state, then see if that matches the
    // event and therefore should transition
    // (so search up to the parent tree)
    state = it->second.parentState;

  } while (state != -1);

  LOG_ERROR("not found target state from current state %d", state);
  return -1;
}

/*

void StateMachine::timerEvent(QTimerEvent *event)
{
  if (G_UNLIKELY(event == nullptr))
    return;

  if (!m_running)
    return;

  const int timerId = event->timerId();

  // take the lock before accessing the delay events map
  QMutexLocker locker(&m_delayedEventsLock);

  // if a timer then use the id to look-up the actual event the was
  // put in the delayed queue
  QMap<qint64, DelayedEvent>::iterator it = m_delayedEvents.begin();
  for (; it != m_delayedEvents.end(); ++it) {

    if (it->timerId == timerId) {

      // if we have a valid delayed event then swap out the event type
      // to the one in the delayed list
      QEvent::Type delayedEventType = it->eventType;

      // free the delayed entry
      m_delayedEvents.erase(it);

      // release the lock so clients can add other delayed events in
      // their state change callbacks
      locker.unlock();

      // kill the timer
      killTimer(timerId);

      // check if this event triggers any transactions
      int newState = shouldMoveState(delayedEventType);
      if (newState != -1)
        triggerStateMove(newState);

      break;
    }
  }
}

*/

// added to set fsm name and which is not supported in qtfsm

std::string StateMachine::name() const
{
  return m_name;
}

void StateMachine::setName(std::string const &name)
{
  m_name = name;
}

// `state` value is enum. build a state with parent state value if there is and
// add it to `state map`

bool StateMachine::addState(int state, std::string const &name)
{
  return addState(-1, state, name);
}

bool StateMachine::addState(int parentState, int state, std::string const &name)
{
  // can't add states while running (really - we're single threaded, why not?)
  if (G_UNLIKELY(m_running))
  {
    LOG_ERROR("can't add states while running");
    return false;
  }

  // check the state is a positive integer
  if (G_UNLIKELY(state < 0))
  {
    LOG_ERROR("state's must be positive integers");
    return false;
  }

  // check we don't already have this state
  if (G_UNLIKELY(m_states.end() != m_states.find(state)))
  {
    LOG_ERROR("already have state %s(%d), not adding again",
              m_states[state].name.c_str(),
              state);
    return false;
  }

  // if a parent was supplied then increment it's child count
  if (parentState != -1)
  {
    auto parent = m_states.find(parentState);

    // if a parent was supplied make sure we have that parent state
    if (G_UNLIKELY(parent == m_states.end()))
    {
      LOG_ERROR("try to add state %s(%d) with missing parent state %d",
                name.c_str(),
                state,
                parentState);
      return false;
    }

    // say it as children
    parent->second.hasChildren = true;
  }

  // add the state
  State stateStruct;

  stateStruct.parentState  = parentState;
  stateStruct.initialState = -1;
  stateStruct.hasChildren  = false;
  stateStruct.isFinal      = false;
  stateStruct.name         = name;

  m_states.insert({state, std::move(stateStruct)});

  return true;
}

// add a transition entry to `fromState` if it is not super state

bool StateMachine::addTransition(int fromState, int event, int toState)
{
  // can't add transitions while running (really - we're single threaded, why not?)
  if (G_UNLIKELY(m_running))
  {
    LOG_ERROR("can't add transitions while running");
    return false;
  }

  // sanity check the event type
  if (G_UNLIKELY(event < 0))
  {
    LOG_ERROR("event is invalid (%d)", int(event));
    return false;
  }

  // sanity check if we have a valid 'from' state
  auto from = m_states.find(fromState);
  if (G_UNLIKELY(from == m_states.end()))
  {
    LOG_ERROR("missing 'fromState' %d", fromState);
    return false;
  }

  // sanity check if we have a valid 'to' state
  auto to = m_states.find(toState);
  if (G_UNLIKELY(to == m_states.end()))
  {
    LOG_ERROR("missing 'toState' %d", toState);
    return false;
  }

  // also check if the to state is a super state that has children and has in
  // initial state -1
  //
  // NOTE:
  // 1. initialState is really used?
  // 2. this is how super(parent) state do not have transition list

  if (G_UNLIKELY((to->second.hasChildren == true) &&
                 (to->second.initialState == -1)))
  {
    LOG_ERROR("'toState' %s(%d) is a super state with no initial state set",
              to->second.name.c_str(),
              toState);
    return false;
  }

  // add the transition
  Transition transition{};

  transition.targetState = toState;
  transition.type        = Transition::EventTransition;
  transition.event       = event;

  from->second.transitions.push_back(std::move(transition));

  return true;
}

/* ={--------------------------------------------------------------------------
 @brief :
  Sets the initial \a state of the state machine, this must be called before
  starting the state machine.
*/

bool StateMachine::setInitialState(int state)
{
  // can't set initial state while running (really - we're single threaded, why not?)
  if (G_UNLIKELY(m_running))
  {
    LOG_ERROR("can't set initial state while running");
    return false;
  }

  // sanity check we know about the state
  if (G_UNLIKELY(m_states.find(state) == m_states.end()))
  {
    LOG_ERROR("can't set initial state to %d as don't have that state", state);
    return false;
  }

  m_initialState = state;
  return true;
}

// -----------------------------------------------------------------------------
/*!
  Sets the final \a state of the state machine, this can't be a super state.
  When the state machine reaches this state it is automatically stopped and
  a finished() signal is emitted.

  It is not necessary to define an final state if the state machine never
  finishes.

  \sa setFinalState(int, int)
 */
bool StateMachine::setFinalState(int state)
{
  // can't set final state while running (really - we're single threaded, why not?)
  if (G_UNLIKELY(m_running))
  {
    LOG_ERROR("can't set final state while running");
    return false;
  }

  // sanity check we know about the state
  if (G_UNLIKELY(m_states.find(state) == m_states.end()))
  {
    LOG_ERROR("can't set final state to %d as don't have that state", state);
    return false;
  }

  m_finalState = state;
  return true;
}

// -----------------------------------------------------------------------------
/*!
 * \threadsafe
 */
void StateMachine::postEvent(int event)
{
  if (G_UNLIKELY(!m_running))
  {
    LOG_ERROR("cannot post event when the state machine is not running");
    return;
  }

  if (G_UNLIKELY(event < (-1)))
  {

    LOG_ERROR("event type must be in valid event range (> -1)");

    // LOG_ERROR("event type must be in user event range (%d <= %d <= %d)",
    //     QEvent::User, event, QEvent::MaxUser);
    return;
  }

  // QThread *QObject::thread() const
  // Returns the thread in which the object lives.

  // QThread *QThread::currentThread()
  // Returns a pointer to a QThread which manages the currently executing thread.

  // if (QThread::currentThread() == QObject::thread())
  {
    // the calling thread is the same as ours so post the event to our
    // local queue if inside a handler, otherwise just process the event
    // immediately
    if (m_withinStateMover)
    {

      // just for debugging
      if (G_UNLIKELY(m_localEvents.size() > 1024))
        LOG_ERROR("state machine event queue getting large");

      // queue it up
      m_localEvents.push(event);
    }
    else
    {

      // not being called from within our own state mover so check if
      // this event will trigger the current state to move, if so
      // trigger that
      int newState = shouldMoveState(event);

      // check if we should be moving to a new state
      if (newState != -1)
        triggerStateMove(newState);
    }
  }
}

// -----------------------------------------------------------------------------
/*!
	\threadsafe

qint64 StateMachine::postDelayedEvent(QEvent::Type eventType, int delay)
{
  if (G_UNLIKELY(!m_running)) {
    LOG_ERROR("cannot post delayed event when the state machine is not running");
    return -1;
  }
  if (G_UNLIKELY((eventType != FinishedEvent) &&
        ((eventType < QEvent::User) || (eventType > QEvent::MaxUser)))) {
    LOG_ERROR("event type must be in user event range (%d <= %d <= %d)",
        QEvent::User, eventType, QEvent::MaxUser);
    return -1;
  }
  if (G_UNLIKELY(delay < 0)) {
    LOG_ERROR("delay cannot be negative");
    return -1;
  }

  // create a timer and then pin the event to the timer
  // int QObject::startTimer(int interval, Qt::TimerType timerType = Qt::CoarseTimer)
  const int timerId = startTimer(delay);
  if (G_UNLIKELY(timerId < 0)) {
    LOG_ERROR("failed to create timer for delayed event");
    return -1;
  }

  // take the lock before accessing the delay events map
  QMutexLocker locker(&m_delayedEventsLock);

  // get a unique id
  qint64 id = m_delayedEventIdCounter++;

  // map the id to the event
  m_delayedEvents.insert(id, { timerId, eventType });
  return id;
}

bool StateMachine::cancelDelayedEvent(qint64 id)
{
  if (G_UNLIKELY(!m_running)) {
    LOG_ERROR("the state machine is not running");
    return false;
  }
  if (G_UNLIKELY(id < 0)) {
    LOG_ERROR("invalid delayed event id");
    return false;
  }

  // take the lock before accessing the delay events map
  QMutexLocker locker(&m_delayedEventsLock);

  // try and find the id in the delayed events map
  QMap<qint64, DelayedEvent>::iterator it = m_delayedEvents.find(id);
  if (it == m_delayedEvents.end())
    return false;

  const int timerId = it->timerId;

  m_delayedEvents.erase(it);

  locker.unlock();

  killTimer(timerId);

  return true;
}

*/

// -----------------------------------------------------------------------------
/*!
	\threadsafe

	Cancels all previously posted delayed event using the \a eventType.  Unlike
	the version that takes an id, this version may cancel more that one delayed
	event.

	Returns \c true if one or more delayed events were cancelled, otherwise
	\c false.

bool StateMachine::cancelDelayedEvents(QEvent::Type eventType)
{
  if (G_UNLIKELY(!m_running)) {
    LOG_ERROR("the state machine is not running");
    return false;
  }

  // take the lock before accessing the delay events map
  QMutexLocker locker(&m_delayedEventsLock);

  // vector of all the timers to kill
  QVector<int> timersToKill(m_delayedEvents.size());

  // try and find the id in the delayed events map
  QMap<qint64, DelayedEvent>::iterator it = m_delayedEvents.begin();
  while (it != m_delayedEvents.end()) {

    if (it->eventType == eventType) {
      timersToKill.append(it->timerId);
      it = m_delayedEvents.erase(it);
    } else {
      ++it;
    }
  }

  locker.unlock();

  // clean-up all the timers
  for (int timerId : timersToKill) {
    killTimer(timerId);
  }

  return !timersToKill.isEmpty();
}

*/

// -----------------------------------------------------------------------------
/*!
  Returns the current (non super) state the state machine is in.
  If the state machine is not currently running then \c -1 is returned.
 */
int StateMachine::state() const
{
  if (!m_running)
    return -1;
  else
    return m_currentState;
}

// -----------------------------------------------------------------------------
/*!
  Checks if the state machine is current in the \a state given.  The \a state
  may may refer to a super state.
 */
bool StateMachine::inState(const int state) const
{
  if (G_UNLIKELY(!m_running))
  {
    LOG_ERROR("the state machine is not running");
    return false;
  }

  // we first check the current state and then walk back up the parent
  // states to check for a match
  int state_ = m_currentState;
  do
  {

    // check for a match to this state
    if (state_ == state)
      return true;

    // find the current state and sanity check it is in the map
    auto it = m_states.find(state_);
    if (G_UNLIKELY(it == m_states.end()))
    {
      LOG_ERROR("invalid state %d (this shouldn't happen)", state_);
      return false;
    }

    // if this state had a parent state then try that on the next loop
    state_ = it->second.parentState;

  } while (state_ != -1);

  return false;
}

// -----------------------------------------------------------------------------
/*!
  Checks if the state machine is in any one of the supplied set of \a states.
  The states in the set may be super states
 */
bool StateMachine::inState(std::set<int> const &states) const
{
  if (G_UNLIKELY(!m_running))
  {
    LOG_ERROR("the state machine is not running");
    return false;
  }

  // we first check the current state and then walk back up the parent
  // states to check for a match
  int state_ = m_currentState;
  do
  {

    // check for a match to this state
    if (states.find(state_) != states.end())
      return true;

    // find the current state and sanity check it is in the map
    auto it = m_states.find(state_);
    if (G_UNLIKELY(it == m_states.end()))
    {
      LOG_ERROR("invalid state %d (this shouldn't happen)", state_);
      return false;
    }

    // if this state had a parent state then try that on the next loop
    state_ = it->second.parentState;

  } while (state_ != -1);

  return false;
}

bool StateMachine::isRunning() const
{
  return m_running;
}

bool StateMachine::connect(std::function<void(int)> entered,
                           std::function<void(int)> exited)
{
  if ((entered == nullptr) || (exited == nullptr))
  {
    LOG_ERROR("state machine is already running");
    return false;
  }

  m_entered = entered;
  m_exited  = exited;
  return true;
}

bool StateMachine::start()
{
  if (G_UNLIKELY(m_running))
  {
    LOG_ERROR("state machine is already running");
    return false;
  }

  if (G_UNLIKELY(m_initialState == -1))
  {
    LOG_ERROR("no initial state set, not starting state machine");
    return false;
  }

  m_stopPending  = false;
  m_currentState = m_initialState;
  m_running      = true;

  logTransition(-1, m_currentState);

  // run initial state
  m_entered(m_currentState);

  return true;
}

void StateMachine::stop()
{
  if (G_UNLIKELY(!m_running))
  {
    LOG_ERROR("state machine not running");
    return;
  }

  // if being called from within a callback function then just mark the
  // state-machine as pending stop ... this will clean up everything once
  // all the events queued are processed
  if (m_withinStateMover)
  {
    m_stopPending = true;
  }
  else
  {
    m_currentState = -1;
    m_running      = false;

    cleanUpEvents();
  }
}
