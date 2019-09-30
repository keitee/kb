//
//  statemachine.cpp
//  BleRcuDaemon
//
//  © 2017 Sky UK
//

#include "qfsmqt.h"

#include <QCoreApplication>
#include <QTimerEvent>
#include <QThread>
#include <QLoggingCategory>
#include <QStringBuilder>


StateMachine::StateMachine(QObject *parent)
	: QObject(parent)
	, m_transitionLogLevel(QtDebugMsg)
	, m_transitionLogCategory(QLoggingCategory::defaultCategory())
	, m_currentState(-1)
	, m_initialState(-1)
	, m_finalState(-1)
	, m_running(false)
	, m_signalIdCounter(1)
	, m_stopPending(false)
	, m_withinStateMover(false)
	, m_delayedEventIdCounter(1)
{
}

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
	// skip out early if no category or the category has the given message
	// type disabled
	if (Q_UNLIKELY(!m_transitionLogCategory ||
	               !m_transitionLogCategory->isEnabled(m_transitionLogLevel))) {
		return;
	}

	// create the logging message
	QString message;
	if (oldState == newState) {
		message = QString("[%1] re-entering state %2(%3)")
			.arg(objectName())
			.arg(m_states[newState].name)
			.arg(newState);

	} else if (oldState == -1) {
		message = QString("[%1] moving to state %2(%3)")
			.arg(objectName())
			.arg(m_states[newState].name)
			.arg(newState);

	} else {
		message = QString("[%1] moving from state %2(%3) to %4(%5)")
			.arg(objectName())
			.arg(m_states[oldState].name)
			.arg(oldState)
			.arg(m_states[newState].name)
			.arg(newState);
	}

	// log the message
	const QMessageLogger logger(QT_MESSAGELOG_FILE,
	                            QT_MESSAGELOG_LINE,
	                            QT_MESSAGELOG_FUNC,
	                            m_transitionLogCategory->categoryName());
	switch (m_transitionLogLevel) {
		case QtDebugMsg:     logger.debug() << message;      break;
		case QtInfoMsg:      logger.info() << message;       break;
		case QtWarningMsg:   logger.warning() << message;    break;
		case QtCriticalMsg:  logger.critical() << message;   break;
		default:                                             break;
	}
}

QtMsgType StateMachine::transistionLogLevel() const
{
	return m_transitionLogLevel;
}

const QLoggingCategory* StateMachine::transistionLogCategory() const
{
	return m_transitionLogCategory;
}

void StateMachine::setTransistionLogLevel(QtMsgType type, const QLoggingCategory *category)
{
	m_transitionLogLevel = type;

	if (category)
		m_transitionLogCategory = category;
	else
		m_transitionLogCategory = QLoggingCategory::defaultCategory();
}

void StateMachine::cleanUpEvents()
{
	// clear all the queued events
	m_localEvents.clear();

	// clean up any delayed events with lock held
	QMutexLocker locker(&m_delayedEventsLock);

	QMap<qint64, DelayedEvent>::iterator it = m_delayedEvents.begin();
	for (; it != m_delayedEvents.end(); ++it) {

		// kill the timer (safe to do in destructor ?)
		if (it->timerId >= 0)
			killTimer(it->timerId);
	}

	m_delayedEvents.clear();
}

QList<int> StateMachine::stateTreeFor(int state, bool bottomUp) const
{
  QList<int> tree;

  // to speed this up we don't do any checks on the state values, we assume
  // this is done when the states are added
  do {

    if (bottomUp)
      tree.append(state);
    else
      tree.prepend(state);

    state = m_states[state].parentState;

  } while (state >= 0);

  return tree;
}

void StateMachine::moveToState(int newState)
{
  // *transition to self*
  // if the new state is equal to the current state then this is not an error
  // and just means we have to issue the exited, transistion and entered
  // signals for the state

  qWarning("KT: moveToState: {");

  if (newState == m_currentState) {

    logTransition(m_currentState, newState);

    emit exited(m_currentState);

    // want to catch transition
    emit transition(m_currentState, m_currentState);

    emit entered(m_currentState);

    // the rest of the processing we can skip

  } else {

    // lookup the new state to check if we should be moving to an initial state
    QMap<int, State>::const_iterator it = m_states.find(newState);

    // if the state has one or more children then it's a super state and
    // we should be moving to the initial state
    if (it->hasChildren) {

      // sanity check we have an initial state
      if (Q_UNLIKELY(it->initialState == -1)) {
        qWarning("try to move to super state %s(%d) but no initial state set",
            it->name.toLatin1().constData(), newState);
        return;
      }

      // set the new state to be the initial state of the super state
      newState = it->initialState;
    }

    // *call eafs*
    int oldState = m_currentState;
    m_currentState = newState;

    logTransition(oldState, newState);

    // get the set of states we're currently in (includes parents)
    //
    // (make a list of states from the given state up to its top parents)
    //
    // new: (new's parent, new) for enterd order
    // old: (old, old's parent) for exited order

    QList<int> newStates = stateTreeFor(newState, false);
    QList<int> oldStates = stateTreeFor(oldState, true);

    // emit the exit signal for any states we left
    for (const int &_oldState : oldStates) {
      if (!newStates.contains(_oldState))
        emit exited(_oldState);
    }

    // emit a transition signal
    emit transition(oldState, m_currentState);

    // emit the entry signal for any states we've now entered
    for (const int &_newState : newStates) {
      if (!oldStates.contains(_newState))
        emit entered(_newState);
    }
  }

  // check if the new state is a final state of a super state, in which case
  // post a FinishedEvent to the message loop
  if (m_states[newState].isFinal) {
    postEvent(FinishedEvent);
  }

  // check if the new state is a final state for the state machine and if so
  // stop the state machine
  if ((m_currentState == m_finalState) || m_stopPending) {

    m_running = false;
    cleanUpEvents();

    if (m_currentState == m_finalState)
      emit finished();
    m_currentState = -1;
  }

  qWarning("KT: moveToState: }");
}

void StateMachine::triggerStateMove(int newState)
{
  Q_ASSERT(QObject::thread() == QThread::currentThread());

  m_withinStateMover = true;

  // move to the new state, this will *emit signals* that may result in more
  // events being added to local event queue
  moveToState(newState);

  // then check if we have any other events on the queue, note we can get
  // into an infinite loop here if the code using the statemachine is
  // poorly designed, however that's their fault not mine
  while (m_running && !m_localEvents.isEmpty()) {
    const QEvent::Type eventType = m_localEvents.dequeue();

    // check if this event should trigger a state move and if so
    // move the state once again
    newState = shouldMoveState(eventType);
    if (newState != -1)
      moveToState(newState);
  }

  m_withinStateMover = false;
}

int StateMachine::shouldMoveState(QEvent::Type eventType) const
{
  // check if this event triggers any transactions
  int state = m_currentState;

  do {

    // find the current state and sanity check it is in the map
    QMap<int, State>::const_iterator it = m_states.find(state);
    if (Q_UNLIKELY(it == m_states.end())) {
      qCritical("invalid state %d (this shouldn't happen)", state);
      return -1;
    }

    // iterate through the transitions of this state and see if any trigger
    // on this event
    for (const Transition &transition : it->transitions) {
      if ((transition.type == Transition::EventTransition) &&
          (transition.eventType == eventType)) {

        // some extra sanity checks that the target state is valid
        // on debug builds
#if (AI_BUILD_TYPE == AI_DEBUG)
        if (Q_UNLIKELY(!m_states.contains(transition.targetState))) {
          qCritical("invalid target state %d (this shouldn't happen)",
              transition.targetState);
        } else if (Q_UNLIKELY((m_states[transition.targetState].hasChildren == true) &&
              (m_states[transition.targetState].initialState == -1))) {
          qCritical("trying to move to a super state with no initial state set");
        }
#endif // (AI_BUILD_TYPE == AI_DEBUG)

        // return the state we should be moving to
        return transition.targetState;
      }
    }

    // event is not found in the transition list.
    // if this state had a parent state, then see if that matches the
    // event and therefore should transition
    // (so search up to the parent tree)
    state = it->parentState;

  } while (state != -1);

  return -1;
}

void StateMachine::customEvent(QEvent *event)
{
	if (Q_UNLIKELY(event == nullptr))
		return;

	if (!m_running)
		return;

	// get the event type, that's the only bit we check for the transitions
	QEvent::Type eventType = event->type();

	// check if this event triggers any transactions
	int newState = shouldMoveState(eventType);
	if (newState != -1)
		triggerStateMove(newState);
}

void StateMachine::timerEvent(QTimerEvent *event)
{
	if (Q_UNLIKELY(event == nullptr))
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

void StateMachine::onSignalTransition(qint64 signalId)
{
	if (!m_running)
		return;

	int state = m_currentState;
	do {

		// find the current state and sanity check it is in the map
		QMap<int, State>::const_iterator it = m_states.find(m_currentState);
		if (Q_UNLIKELY(it == m_states.end())) {
			qCritical("invalid state %d (this shouldn't happen)", state);
			return;
		}

		// iterate through the transitions of this state and see if any trigger
		// on this event
		const State &stateStruct = it.value();
		for (const Transition &transition : stateStruct.transitions) {
			if ((transition.type == Transition::SignalTransition) &&
			    (transition.signalId == signalId)) {

				// move to the new state
				triggerStateMove(transition.targetState);
				return;
			}
		}

		// if this state had a parent state, then see if that matches the
		// event and therefore should transition
		state = stateStruct.parentState;

	} while (state != -1);

}

bool StateMachine::addState(int state, const QString &name)
{
  return addState(-1, state, name);
}

bool StateMachine::addState(int parentState, int state, const QString &name)
{
  // can't add states while running (really - we're single threaded, why not?)
  if (Q_UNLIKELY(m_running)) {
    qWarning("can't add states while running");
    return false;
  }

  // check the state is a positive integer
  if (Q_UNLIKELY(state < 0)) {
    qWarning("state's must be positive integers");
    return false;
  }

  // check we don't already have this state
  if (Q_UNLIKELY(m_states.contains(state))) {
    qWarning("already have state %s(%d), not adding again",
        m_states[state].name.toLatin1().constData(), state);
    return false;
  }

  // if a parent was supplied then increment it's child count
  if (parentState != -1) {
    QMap<int, State>::iterator parent = m_states.find(parentState);

    // if a parent was supplied make sure we have that parent state
    if (Q_UNLIKELY(parent == m_states.end())) {
      qWarning("try to add state %s(%d) with missing parent state %d",
          name.toLatin1().constData(), state, parentState);
      return false;
    }

    // increment the number of child states
    parent->hasChildren = true;
  }

  // add the state
  State stateStruct;

  stateStruct.parentState = parentState;
  stateStruct.initialState = -1;
  stateStruct.hasChildren = false;
  stateStruct.isFinal = false;
  stateStruct.name = name;

  m_states.insert(state, std::move(stateStruct));

  return true;
}

bool StateMachine::addTransition(int fromState, QEvent::Type eventType, int toState)
{
  // can't add transitions while running (really - we're single threaded, why not?)
  if (Q_UNLIKELY(m_running)) {
    qWarning("can't add transitions while running");
    return false;
  }

  // sanity check the event type
  if (Q_UNLIKELY(eventType == QEvent::None)) {
    qWarning("eventType is invalid (%d)", int(eventType));
    return false;
  }

  // sanity check we have a 'from' state
  QMap<int, State>::iterator from = m_states.find(fromState);
  if (Q_UNLIKELY(from == m_states.end())) {
    qWarning("missing 'fromState' %d", fromState);
    return false;
  }

  // and we have a 'to' state
  QMap<int, State>::const_iterator to = m_states.find(toState);
  if (Q_UNLIKELY(to == m_states.end())) {
    qWarning("missing 'toState' %d", toState);
    return false;
  }

  // also check if the to state is a super state that it has in initial
  // state set
  if (Q_UNLIKELY((to->hasChildren == true) && (to->initialState == -1))) {
    qWarning("'toState' %s(%d) is a super state with no initial state set",
        to->name.toLatin1().constData(), toState);
    return false;
  }

  // add the transition
  Transition transition;
  bzero(&transition, sizeof(transition));

  transition.targetState = toState;
  transition.type = Transition::EventTransition;
  transition.eventType = eventType;

  from->transitions.append(std::move(transition));

  return true;
}

// -----------------------------------------------------------------------------
/*!
	\internal

	Internal method called from \a addTransition, it adds a \c Transition
	structure with the given \a signalId to the given \a fromState.

 */
bool StateMachine::setSignalTransition(int fromState, int toState, qint64 signalId)
{
	// TODO: this needs to be made thread safe

	// can't add transitions while running (really - we're single threaded, why not?)
	if (Q_UNLIKELY(m_running)) {
		qWarning("can't add transitions while running");
		return false;
	}

	// sanity check we have a 'from' state
	QMap<int, State>::iterator from = m_states.find(fromState);
	if (Q_UNLIKELY(from == m_states.end())) {
		qWarning("missing 'fromState' %d", fromState);
		return false;
	}

	// and we have a 'to' state
	QMap<int, State>::const_iterator to = m_states.find(toState);
	if (Q_UNLIKELY(to == m_states.end())) {
		qWarning("missing 'toState' %d", toState);
		return false;
	}

	// also check if the to state is a super state that it has in initial
	// state set
	if (Q_UNLIKELY(to->hasChildren && (to->initialState == -1))) {
		qWarning("'toState' %s(%d) is a super state with no initial state set",
		         to->name.toLatin1().constData(), toState);
		return false;
	}

	// add the transition
	Transition transition;
	transition.type = Transition::SignalTransition;
	transition.signalId = signalId;

	from->transitions.append(std::move(transition));

	return true;
}

// -----------------------------------------------------------------------------
/*!
	Sets the initial \a state of the state machine, this must be called before
	starting the state machine.

 */
bool StateMachine::setInitialState(int state)
{
  // can't set initial state while running (really - we're single threaded, why not?)
  if (Q_UNLIKELY(m_running)) {
    qWarning("can't set initial state while running");
    return false;
  }

  // sanity check we know about the state
  if (Q_UNLIKELY(!m_states.contains(state))) {
    qWarning("can't set initial state to %d as don't have that state", state);
    return false;
  }

  m_initialState = state;
  return true;
}

// -----------------------------------------------------------------------------
/*!
	Sets the initial \a initialState of the super state \a parentState. This is
	used when a transition has the super state as a target.

	It is not necessary to define an initial state of the a super state, for
	example if a super state is never a target for a transition there is no
	need to call this method.

 */
bool StateMachine::setInitialState(int parentState, int initialState)
{
	// can't set initial state while running (really - we're single threaded, why not?)
	if (Q_UNLIKELY(m_running)) {
		qWarning("can't set initial state while running");
		return false;
	}

	// get the parent state
	QMap<int, State>::iterator parent = m_states.find(parentState);
	if (Q_UNLIKELY(parent == m_states.end())) {
		qWarning("can't find parent state %d", parentState);
		return false;
	}

	// sanity check we know about the given initial state
	QMap<int, State>::const_iterator initial = m_states.find(initialState);
	if (Q_UNLIKELY(initial == m_states.end())) {
		qWarning("can't set initial state to %d as don't have that state",
		         initialState);
		return false;
	}

	// sanity check the given initial state has the same parent
	if (Q_UNLIKELY(initial->parentState != parentState)) {
		qWarning("can't set initial state to %d as parent state doesn't match",
		         initialState);
		return false;
	}

	// check if we already an initial state, this is not fatal but raise a warning
	if (Q_UNLIKELY(parent->initialState != -1)) {
		qWarning("replacing existing initial state %d to %d",
		         parent->initialState, initialState);
	}

	parent->initialState = initialState;
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
	if (Q_UNLIKELY(m_running)) {
		qWarning("can't set final state while running");
		return false;
	}

	// sanity check we know about the state
	if (Q_UNLIKELY(!m_states.contains(state))) {
		qWarning("can't set final state to %d as don't have that state", state);
		return false;
	}

	m_finalState = state;
	return true;
}

// -----------------------------------------------------------------------------
/*!
	Sets the final \a finalState of the super state \a parentState. This is used
	when a transition has the super state as a source and an event of
	type StateMachine::FinishedEvent.

	It is not necessary to define an final state of the a super state, for
	example if a super state is never a source for a transition with a
	StateMachine::FinishedEvent event then there is no need to call this method.

	\sa setFinalState(int)
 */
bool StateMachine::setFinalState(int parentState, int finalState)
{
	// can't set final state while running (really - we're single threaded, why not?)
	if (Q_UNLIKELY(m_running)) {
		qWarning("can't set final state while running");
		return false;
	}

	// get the parent state
	QMap<int, State>::const_iterator parent = m_states.find(parentState);
	if (Q_UNLIKELY(parent == m_states.end())) {
		qWarning("can't find parent state %d", parentState);
		return false;
	}

	// sanity check we know about the given final state
	QMap<int, State>::iterator final = m_states.find(finalState);
	if (Q_UNLIKELY(final == m_states.end())) {
		qWarning("can't set final state to %d as don't have that state",
		         finalState);
		return false;
	}

	// sanity check the given initial state has the same parent
	if (Q_UNLIKELY(final->parentState != parentState)) {
		qWarning("can't set final state to %d as parent state doesn't match",
		         finalState);
		return false;
	}

	final->isFinal = true;
	return true;
}

// -----------------------------------------------------------------------------
/*!
	\threadsafe


 */
void StateMachine::postEvent(QEvent::Type eventType)
{
  if (Q_UNLIKELY(!m_running)) {
    qWarning("cannot post event when the state machine is not running");
    return;
  }

  if (Q_UNLIKELY((eventType != FinishedEvent) &&
        ((eventType < QEvent::User) || (eventType > QEvent::MaxUser)))) {
    qWarning("event type must be in user event range (%d <= %d <= %d)",
        QEvent::User, eventType, QEvent::MaxUser);
    return;
  }

  // QThread *QObject::thread() const
  // Returns the thread in which the object lives.

  // QThread *QThread::currentThread()
  // Returns a pointer to a QThread which manages the currently executing thread.

  if (QThread::currentThread() == QObject::thread()) {

    qWarning("KT: calling thread is the same as ours?");

    // the calling thread is the same as ours so post the event to our
    // local queue if inside a handler, otherwise just process the event
    // immediately
    if (m_withinStateMover) {

      // just for debugging
      if (Q_UNLIKELY(m_localEvents.size() > 1024))
        qWarning("state machine event queue getting large");

      qWarning("KT: queue it up");
      // queue it up
      m_localEvents.enqueue(eventType);

    } else {

      // not being called from within our own state mover so check if
      // this event will trigger the current state to move, if so
      // trigger that
      int newState = shouldMoveState(eventType);

      // check if we should be moving to a new state
      if (newState != -1)
        triggerStateMove(newState);
    }

  } else {

    qWarning("KT: QCoreApplication:postEvent()");

    // being called from a different thread so post the message back to
    // us through the main application event loop
    QCoreApplication::postEvent(this, new QEvent(eventType));
  }
}

// -----------------------------------------------------------------------------
/*!
	\threadsafe


 */
qint64 StateMachine::postDelayedEvent(QEvent::Type eventType, int delay)
{
	if (Q_UNLIKELY(!m_running)) {
		qWarning("cannot post delayed event when the state machine is not running");
		return -1;
	}
	if (Q_UNLIKELY((eventType != FinishedEvent) &&
	               ((eventType < QEvent::User) || (eventType > QEvent::MaxUser)))) {
		qWarning("event type must be in user event range (%d <= %d <= %d)",
		         QEvent::User, eventType, QEvent::MaxUser);
		return -1;
	}
	if (Q_UNLIKELY(delay < 0)) {
		qWarning("delay cannot be negative");
		return -1;
	}

	// create a timer and then pin the event to the timer
	const int timerId = startTimer(delay);
	if (Q_UNLIKELY(timerId < 0)) {
		qWarning("failed to create timer for delayed event");
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

// -----------------------------------------------------------------------------
/*!
	\threadsafe


 */
bool StateMachine::cancelDelayedEvent(qint64 id)
{
	if (Q_UNLIKELY(!m_running)) {
		qWarning("the state machine is not running");
		return false;
	}
	if (Q_UNLIKELY(id < 0)) {
		qWarning("invalid delayed event id");
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

// -----------------------------------------------------------------------------
/*!
	\threadsafe

	Cancels all previously posted delayed event using the \a eventType.  Unlike
	the version that takes an id, this version may cancel more that one delayed
	event.

	Returns \c true if one or more delayed events were cancelled, otherwise
	\c false.

 */
bool StateMachine::cancelDelayedEvents(QEvent::Type eventType)
{
	if (Q_UNLIKELY(!m_running)) {
		qWarning("the state machine is not running");
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
	if (Q_UNLIKELY(!m_running)) {
		qWarning("the state machine is not running");
		return false;
	}

	// we first check the current state and then walk back up the parent
	// states to check for a match
	int state_ = m_currentState;
	do {

		// check for a match to this state
		if (state_ == state)
			return true;

		// find the current state and sanity check it is in the map
		QMap<int, State>::const_iterator it = m_states.find(state_);
		if (Q_UNLIKELY(it == m_states.end())) {
			qCritical("invalid state %d (this shouldn't happen)", state_);
			return false;
		}

		// if this state had a parent state then try that on the next loop
		state_ = it->parentState;

	} while (state_ != -1);

	return false;
}

// -----------------------------------------------------------------------------
/*!
	Checks if the state machine is in any one of the supplied set of \a states.
	The states in the set may be super states


 */
bool StateMachine::inState(const QSet<int> &states) const
{
	if (Q_UNLIKELY(!m_running)) {
		qWarning("the state machine is not running");
		return false;
	}

	// we first check the current state and then walk back up the parent
	// states to check for a match
	int state_ = m_currentState;
	do {

		// check for a match to this state
		if (states.contains(state_))
			return true;

		// find the current state and sanity check it is in the map
		QMap<int, State>::const_iterator it = m_states.find(state_);
		if (Q_UNLIKELY(it == m_states.end())) {
			qCritical("invalid state %d (this shouldn't happen)", state_);
			return false;
		}

		// if this state had a parent state then try that on the next loop
		state_ = it->parentState;

	} while (state_ != -1);

	return false;
}

bool StateMachine::isRunning() const
{
  return m_running;
}

bool StateMachine::start()
{
  if (Q_UNLIKELY(m_running)) {
    qWarning("state machine is already running");
    return false;
  }

  if (Q_UNLIKELY(m_initialState == -1)) {
    qWarning("no initial state set, not starting state machine");
    return false;
  }

  m_stopPending = false;
  m_currentState = m_initialState;
  m_running = true;

  logTransition(-1, m_currentState);

  emit entered(m_currentState);

  return true;
}

void StateMachine::stop()
{
  if (Q_UNLIKELY(!m_running)) {
    qWarning("state machine not running");
    return;
  }

  // if being called from within a callback function then just mark the
  // state-machine as pending stop ... this will clean up everything once
  // all the events queued are processed
  if (m_withinStateMover) {
    m_stopPending = true;

  } else {

    m_currentState = -1;
    m_running = false;

    cleanUpEvents();
  }
}

