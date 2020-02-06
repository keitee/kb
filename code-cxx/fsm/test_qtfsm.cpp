#include <chrono>
#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include "gmock/gmock.h"

#include "qfsmqt.h"

#include <QCoreApplication>
#include <QDebug>
#include <QList>
#include <QLoggingCategory>
#include <QSignalSpy>
#include <QString>
#include <QTimer>

using namespace testing;

// void StateMachineTest_handleSuperStateNotSameParent_Test::TestBody();
// (gdb) b StateMachineTest_handleSuperStateNotSameParent_Test::TestBody()
// void StateMachineTest_handleSuperStateSameParent_Test::TestBody();

/*
 
class UseFsm 
{
  private:

    // define state

    enum State {
      ServiceUnavailableState,
      ServiceAvailableSuperState,
      AdapterUnavailableState,
      AdapterAvailableSuperState,
      AdapterPoweredOffState,
      AdapterPoweredOnState,
      ShutdownState
    };

    StateMachine m_stateMachine;

    initStateManince()
    {
      // build all the states

      m_stateMachine.addState(ServiceUnavailableState, QStringLiteral("ServiceUnavailableState"));

      // build the transitions       From State	              ->    Event                  ->  To State

      m_stateMachine.addTransition(ServiceUnavailableState,       ServiceAvailableEvent,      AdapterUnavailableState);

      // connect to the state entry and exit signals

      QObject::connect(&m_stateMachine, &StateMachine::entered,
        this, &BleRcuManagerImpl::onStateEntry);

      QObject::connect(&m_stateMachine, &StateMachine::exited,
        this, &BleRcuManagerImpl::onStateExit);

      // set the initial state
      m_stateMachine.setInitialState(ServiceUnavailableState);

      m_stateMachine.start();

    };

    void BleRcuManagerImpl::onStateEntry(int state)
    {
      switch (State(state)) {
        case ServiceUnavailableState:
          onEnteredServiceUnavailableState();
        break;
        case AdapterUnavailableState:
          onEnteredAdapterUnavailableState();
        break;
        case AdapterPoweredOffState:
          onEnteredAdapterPoweredOffState();
        break;
        case AdapterPoweredOnState:
          onEnteredAdapterPoweredOnState();
        break;
        default:
        break;
      }
    }
};


To run fsm:

// define custom Qt events

static const QEvent::Type e1  = static_cast<QEvent::Type>(QEvent::User + 100);

// post this custom event

m_stateMachine.postEvent() or;
m_stateMachine.postDelayed();


NOTE:
1. do not use `finished` signal of fsm but m_finalState which is set by
setFinalState() is used to make running false and call cleanUpEvents()


(from) state has
  - transition list{pair<event, new state>}


class StateMachine
{
  struct State {
    int parentState;
    int initialState;
    bool hasChildren;
    bool isFinal;
    QString name;
    QList<Transition> transitions;
  };

  struct Transition {
    int targetState;
    enum { EventTransition, SignalTransition } type;
    union {
      QEvent::Type eventType;
      qint64 signalId;
    };
  };

  QMap<int, State> m_states;

  // add pair<state enum, State> to m_state map. can set if it's parent(super)
  // state or not
  bool StateMachine::addState(int parentState, int state, const QString &name);

  // add `transition` to fromState and transion entry has toState
  bool StateMachine::addTransition(int fromState, QEvent::Type eventType, int toState)


  int m_currentState;

  // search `event` in the transition list of `the current event` and return
  // state enum if found. -1 otherwise.

  int StateMachine::shouldMoveState(QEvent::Type event) const;


  // custom event handler

  void customEvent(QEvent *event)
  {
    // get the event type, that's the only bit we check for the transitions
    QEvent::Type eventType = event->type();

    // check if this event triggers any transactions
    int newState = shouldMoveState(eventType);
    if (newState != -1)
      triggerStateMove(newState);
  }

  void StateMachine::triggerStateMove(int newState)
  {
    m_withinStateMover = true;

    // move to the new state, this will emit signals that may result in more
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
};


*/

// ={=========================================================================

static const QEvent::Type e1  = static_cast<QEvent::Type>(QEvent::User + 100);
static const QEvent::Type e2  = static_cast<QEvent::Type>(QEvent::User + 101);
static const QEvent::Type e3  = static_cast<QEvent::Type>(QEvent::User + 102);
static const QEvent::Type e4  = static_cast<QEvent::Type>(QEvent::User + 103);
static const QEvent::Type e5  = static_cast<QEvent::Type>(QEvent::User + 104);
static const QEvent::Type e6  = static_cast<QEvent::Type>(QEvent::User + 105);
static const QEvent::Type e7  = static_cast<QEvent::Type>(QEvent::User + 106);
static const QEvent::Type e8  = static_cast<QEvent::Type>(QEvent::User + 107);
static const QEvent::Type e9  = static_cast<QEvent::Type>(QEvent::User + 108);
static const QEvent::Type e10 = static_cast<QEvent::Type>(QEvent::User + 109);

// Q: why error?
//
// using ServiceRetryEvent         = e1;
// using ServiceAvailableEvent     = e2;
// using ServiceUnavailableEvent   = e3;
// using AdapterRetryAttachEvent   = e4;
// using AdapterAvailableEvent     = e5;
// using AdapterUnavailableEvent   = e6;
// using AdapterRetryPowerOnEvent  = e7;
// using AdapterPoweredOnEvent     = e8;
// using AdapterPoweredOffEvent    = e9;
// using ShutdownEvent             = e10;

static const QEvent::Type ServiceRetryEvent =
  static_cast<QEvent::Type>(QEvent::User + 100);
static const QEvent::Type ServiceAvailableEvent =
  static_cast<QEvent::Type>(QEvent::User + 101);
static const QEvent::Type ServiceUnavailableEvent =
  static_cast<QEvent::Type>(QEvent::User + 102);
static const QEvent::Type AdapterRetryAttachEvent =
  static_cast<QEvent::Type>(QEvent::User + 103);
static const QEvent::Type AdapterAvailableEvent =
  static_cast<QEvent::Type>(QEvent::User + 104);
static const QEvent::Type AdapterUnavailableEvent =
  static_cast<QEvent::Type>(QEvent::User + 105);
static const QEvent::Type AdapterRetryPowerOnEvent =
  static_cast<QEvent::Type>(QEvent::User + 106);
static const QEvent::Type AdapterPoweredOnEvent =
  static_cast<QEvent::Type>(QEvent::User + 107);
static const QEvent::Type AdapterPoweredOffEvent =
  static_cast<QEvent::Type>(QEvent::User + 108);
static const QEvent::Type ShutdownEvent =
  static_cast<QEvent::Type>(QEvent::User + 109);

class StateMachineTest : public ::testing::Test
{
public:
  void SetUp() {}

  void TearDown() {}

protected:
  void processEvents(int mintime = 0)
  {
    if (mintime <= 0)
    {
      QCoreApplication::processEvents();
    }
    else
    {
      volatile bool done = false;

      std::function<void()> lambda = [&] { done = true; };

      QTimer::singleShot(mintime, lambda);
      while (!done)
      {
        QCoreApplication::processEvents();
      }
    }
  }

protected:
};

TEST_F(StateMachineTest, transitionToInitialState)
{
  enum
  {
    initialState
  };

  StateMachine machine;
  machine.setObjectName("machine");

  EXPECT_TRUE(machine.addState(initialState));
  EXPECT_TRUE(machine.setInitialState(initialState));

  // https://doc.qt.io/qt-5/qsignalspy.html#details

  QSignalSpy enteredSpy(&machine, &StateMachine::entered);
  QSignalSpy exitedSpy(&machine, &StateMachine::exited);

  // start() only emits `entered`
  EXPECT_TRUE(machine.start());

  EXPECT_EQ(enteredSpy.count(), 1);
  EXPECT_TRUE(machine.isRunning());
  EXPECT_EQ(machine.state(), int(initialState));

  machine.stop();

  EXPECT_EQ(exitedSpy.count(), 0);
  EXPECT_FALSE(machine.isRunning());
}

TEST_F(StateMachineTest, startWithNoInitialState)
{
  enum
  {
    initialState
  };

  StateMachine machine;
  machine.setObjectName("machine");

  EXPECT_TRUE(machine.addState(initialState));

  // misses out machine.setInitialState(initialState);
  // so `init state` is not set

  QSignalSpy enteredSpy(&machine, &StateMachine::entered);

  EXPECT_FALSE(machine.start());

  EXPECT_EQ(enteredSpy.count(), 0);
  EXPECT_FALSE(machine.isRunning());
}

TEST_F(StateMachineTest, addingInvalidStateAndTransitions)
{
  enum
  {
    s1,
    s2,
    s3,
    invalid1,
    invalid2
  };

  StateMachine machine;
  machine.setObjectName("machine");

  EXPECT_TRUE(machine.addState(s1));
  EXPECT_TRUE(machine.addState(s2));
  EXPECT_TRUE(machine.addState(s3));
  EXPECT_TRUE(machine.setInitialState(s1));

  // add an already existing state
  EXPECT_FALSE(machine.addState(s1));

  // bool addState(int parentState, int state, const QString &name = QString());
  // `state` should be valid and it is not since it's already added before.
  EXPECT_FALSE(machine.addState(s2, s1));

  // add new state with invalid parent. Only add new state when parent is
  // already added.
  EXPECT_FALSE(machine.addState(invalid1, invalid2));

  // add transition to/from invalid state. All states should be valid before
  // adiing transition
  EXPECT_FALSE(machine.addTransition(s1, QEvent::User, invalid1));
  EXPECT_FALSE(machine.addTransition(invalid1, QEvent::User, invalid2));
  EXPECT_FALSE(machine.addTransition(invalid1, QEvent::User, s2));

  // add transition with invalid event type
  EXPECT_FALSE(machine.addTransition(s1, QEvent::None, s2));
}

TEST_F(StateMachineTest, stopWithinSlot)
{
  enum
  {
    initialState,
  };

  StateMachine machine;
  machine.setObjectName("machine");

  EXPECT_TRUE(machine.addState(initialState));
  EXPECT_TRUE(machine.setInitialState(initialState));

  // create a lambda that calls stop on a state entry since signal is defined
  // as:
  //
  // void entered(int state);
  // void exited(int state);

  std::function<void(int)> lambda = [&](int state) {
    Q_UNUSED(state);
    machine.stop();
  };

  QObject::connect(&machine, &StateMachine::entered, lambda);

  // check we get a started / stopped signal
  QSignalSpy enteredSpy(&machine, &StateMachine::entered);
  QSignalSpy exitedSpy(&machine, &StateMachine::exited);

  // start the state machine
  machine.start();

  EXPECT_EQ(enteredSpy.count(), 1);
  EXPECT_EQ(exitedSpy.count(), 0);
  EXPECT_FALSE(machine.isRunning());
}

// ???

TEST_F(StateMachineTest, stopWithinDelayedSlot)
{
  enum
  {
    s1,
    s2,
  };

  StateMachine machine;
  machine.setObjectName("machine");

  EXPECT_TRUE(machine.addState(s1));
  EXPECT_TRUE(machine.addState(s2));
  EXPECT_TRUE(machine.setInitialState(s1));
  EXPECT_TRUE(machine.addTransition(s1, e1, s2));

  // create a lambda that calls stop on entry to state 's2'
  std::function<void(int)> lambda = [&](int state) {
    if (state == s2)
      machine.stop();
  };

  QObject::connect(&machine, &StateMachine::entered, lambda);

  // check we get a started / stopped signal
  QSignalSpy enteredSpy(&machine, &StateMachine::entered);
  QSignalSpy exitedSpy(&machine, &StateMachine::exited);

  // start the state machine
  machine.start();

  // post a delayed event 'e1'
  machine.postDelayedEvent(e1, 10);
  EXPECT_TRUE(machine.isRunning());

  processEvents(100);

  EXPECT_EQ(enteredSpy.count(), 2);
  EXPECT_EQ(exitedSpy.count(), 1);
  EXPECT_FALSE(machine.isRunning());
}

TEST_F(StateMachineTest, receiveEntryExitTransitionSignalsOnLoopback)
{
  enum
  {
    s1,
    s2
  };

  StateMachine machine;

  EXPECT_TRUE(machine.addState(s1, "s1"));
  EXPECT_TRUE(machine.addState(s2, "s2"));
  EXPECT_TRUE(machine.setInitialState(s1));

  EXPECT_TRUE(machine.addTransition(s1, e1, s2));
  EXPECT_TRUE(machine.addTransition(s2, e2, s2));

  std::map<int, unsigned> entries;
  std::function<void(int)> entryLamda = [&](int state) { entries[state]++; };
  QObject::connect(&machine, &StateMachine::entered, entryLamda);

  std::map<int, unsigned> exits;
  std::function<void(int)> exitLamda = [&](int state) { exits[state]++; };
  QObject::connect(&machine, &StateMachine::exited, exitLamda);

  machine.start();       // s1 enter
  machine.postEvent(e1); // s2 enter, s1 exit
  machine.postEvent(e2); // s2 enter, s2 exit
  machine.stop();

  EXPECT_FALSE(machine.isRunning());

  EXPECT_EQ(entries[s1], 1U);
  EXPECT_EQ(exits[s1], 1U);

  EXPECT_EQ(entries[s2], 2U);
  EXPECT_EQ(exits[s2], 1U);
}

TEST_F(StateMachineTest, enterNestedStateOrder)
{
  // tests that when entering a nested state that we receive state entry
  // notifications for the super states before the final state

  enum
  {
    initialState,
    s1,
    s1_1,
    s1_2,
    s1_2_1,
    s1_2_2,
    s2
  };

  StateMachine machine;
  machine.setObjectName("machine");

  // init
  //
  //  s1
  //
  // |s1_2              |  s1_1
  // |                  |
  // |[s1_2_1   s1_2_2] |
  //
  // bool addState(int parentState, int state, const QString &name = QString());

  EXPECT_TRUE(machine.addState(initialState));
  EXPECT_TRUE(machine.addState(s1));
  EXPECT_TRUE(machine.addState(s1, s1_1));
  EXPECT_TRUE(machine.addState(s1, s1_2));
  EXPECT_TRUE(machine.addState(s1_2, s1_2_1));
  EXPECT_TRUE(machine.addState(s1_2, s1_2_2));
  EXPECT_TRUE(machine.addState(s2));

  EXPECT_TRUE(machine.setInitialState(initialState));

  EXPECT_TRUE(machine.addTransition(initialState, e1, s1_2_2));
  EXPECT_TRUE(machine.addTransition(s1_2_2, e2, s1_2_1));

  QSignalSpy enteredSpy(&machine, &StateMachine::entered);

  machine.start();

  EXPECT_TRUE(machine.isRunning());
  EXPECT_EQ(enteredSpy.count(), 1);

  // Removes all items from the list.
  enteredSpy.clear();

  // since e1 pass through 3 states from init state which is not in (old) state
  // set.

  machine.postEvent(e1);
  EXPECT_EQ(enteredSpy.count(), 3);

  // check the order of `entered` signals
  EXPECT_EQ(enteredSpy[0].first().toInt(), int(s1));     // 1
  EXPECT_EQ(enteredSpy[1].first().toInt(), int(s1_2));   // 3
  EXPECT_EQ(enteredSpy[2].first().toInt(), int(s1_2_2)); // 5
}

TEST_F(StateMachineTest, exitNestedStateOrder)
{
  // tests that when exiting a nested state that we receive state exit
  // notifications for the state back up through the super states

  enum
  {
    s1,
    s1_1,
    s1_2,
    s1_2_1,
    s1_2_2,
    s2
  };

  StateMachine machine;
  machine.setObjectName("machine");

  EXPECT_TRUE(machine.addState(s1));
  EXPECT_TRUE(machine.addState(s1, s1_1));
  EXPECT_TRUE(machine.addState(s1, s1_2));
  EXPECT_TRUE(machine.addState(s1_2, s1_2_1));
  EXPECT_TRUE(machine.addState(s1_2, s1_2_2));
  EXPECT_TRUE(machine.addState(s2));

  EXPECT_TRUE(machine.setInitialState(s1_2_1));

  EXPECT_TRUE(machine.addTransition(s1_2_1, e1, s2));

  QSignalSpy exitedSpy(&machine, &StateMachine::exited);

  machine.start();
  EXPECT_TRUE(machine.isRunning());

  // since s1_2_1 has 3 nested deep which is not in (old) state set, expect 3
  // exited signals

  machine.postEvent(e1);

  EXPECT_EQ(exitedSpy.count(), 3);

  EXPECT_EQ(exitedSpy[0].first().toInt(), int(s1_2_1));
  EXPECT_EQ(exitedSpy[1].first().toInt(), int(s1_2));
  EXPECT_EQ(exitedSpy[2].first().toInt(), int(s1));
}

/*

Super(Nested) states:

shouldMoveState() search for possible new state from transition tables of states
from current up to top parents. this is how event is handled in nested state
which do not have a transition table and is handled by parent(super) state.

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

        // return the state we should be moving to
        return transition.targetState;
      }
    }

    // event is not found in the transition list.
    // if this state had a parent state, then see if that matches the
    // event and therefore should transition
    //
    // (so search up to the parent tree)

    state = it->parentState;

  } while (state != -1);

  return -1;
}


The unusual thing is that all exited/enterede are fired along the transition
path and user of fsm can decice what to handle in exited/entered handler.


1. not allow to transit to super state which has child/children and this makes
  to have `entered` for `toState`. If allows this, would have only `exited`

            new             old
1 - 2 - 3 - 4 - 5 - 6 - 7 - 8

if moves from old to new, then exited called in order:
8, 7, 6, 5

                            old
1 - 2 - 3 - 4 - 5 - 6 - 7 - 8
            new
          - 9

if moves from old to new, then exited called in order:
8, 7, 6, 5, 4

entered called:
9


bool StateMachine::addTransition(int fromState, QEvent::Type eventType, int toState)
{
  // also check if the to state is a super state that it has in initial
  // state set
  if (Q_UNLIKELY((to->hasChildren == true) && (to->initialState == -1))) {
    qWarning("'toState' %s(%d) is a super state with no initial state set",
        to->name.toLatin1().constData(), toState);
    return false;
  }
}


void StateMachine::moveToState(int newState)
{
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

*/

TEST_F(StateMachineTest, handleSuperStateNotSameParent)
{
  enum State
  {
    ServiceUnavailableState,    // 0
    ServiceAvailableSuperState, // 1
    AdapterUnavailableState,    // 2
    AdapterAvailableSuperState, // 3
    AdapterPoweredOffState,     // 4
    AdapterPoweredOnState,      // 5
    ShutdownState               // 6
  };

  StateMachine machine;
  machine.setObjectName("machine");

  // add all the states
  machine.addState(ServiceUnavailableState,
                   QStringLiteral("ServiceUnavailableState"));
  machine.addState(ServiceAvailableSuperState,
                   QStringLiteral("ServiceAvailableSuperState"));

  machine.addState(ServiceAvailableSuperState,
                   AdapterUnavailableState,
                   QStringLiteral("AdapterUnavailableState"));
  machine.addState(ServiceAvailableSuperState,
                   AdapterAvailableSuperState,
                   QStringLiteral("AdapterAvailableSuperState"));

  machine.addState(AdapterAvailableSuperState,
                   AdapterPoweredOffState,
                   QStringLiteral("AdapterPoweredOffState"));
  machine.addState(AdapterAvailableSuperState,
                   AdapterPoweredOnState,
                   QStringLiteral("AdapterPoweredOnState"));

  machine.addState(ShutdownState, QStringLiteral("ShutdownState"));

  // add the transitions       From State	              ->    Event                  ->  To State
  machine.addTransition(ServiceUnavailableState,
                        ServiceAvailableEvent,
                        AdapterUnavailableState);
  machine.addTransition(ServiceUnavailableState,
                        ServiceRetryEvent,
                        ServiceUnavailableState);
  machine.addTransition(ServiceAvailableSuperState,
                        ServiceUnavailableEvent,
                        ServiceUnavailableState);
  machine.addTransition(ServiceAvailableSuperState,
                        ShutdownEvent,
                        ShutdownState);

  machine.addTransition(AdapterUnavailableState,
                        AdapterAvailableEvent,
                        AdapterPoweredOffState);
  machine.addTransition(AdapterUnavailableState,
                        AdapterRetryAttachEvent,
                        AdapterUnavailableState);
  machine.addTransition(AdapterAvailableSuperState,
                        AdapterUnavailableEvent,
                        AdapterUnavailableState);

  machine.addTransition(AdapterPoweredOffState,
                        AdapterPoweredOnEvent,
                        AdapterPoweredOnState);
  machine.addTransition(AdapterPoweredOffState,
                        AdapterRetryPowerOnEvent,
                        AdapterPoweredOffState);
  machine.addTransition(AdapterPoweredOnState,
                        AdapterPoweredOffEvent,
                        AdapterPoweredOffState);

  // to print out
  // // connect to the state entry and exit signals
  // QObject::connect(&machine, &StateMachine::entered,
  //     [](int state)
  //     {
  //       qWarning("entered state %d", state);
  //     });
  //
  // QObject::connect(&machine, &StateMachine::exited,
  //     [](int state)
  //     {
  //       qWarning("exited state %d", state);
  //     });

  QSignalSpy enteredSpy(&machine, &StateMachine::entered);

  // *cxx-error* cause seg fault. typo on `entered` which should be `exited`
  // QSignalSpy exitedSpy(&machine, &StateMachine::entered);
  QSignalSpy exitedSpy(&machine, &StateMachine::exited);

  // set the initial state
  machine.setInitialState(AdapterPoweredOffState);
  machine.start();

  // Removes all items from the list.
  enteredSpy.clear();

  // post a event to state which don't have same parent and output is:
  //
  // exited state 4
  // exited state 3
  // exited state 1
  // entered state 0

  machine.postEvent(ServiceUnavailableEvent);

  EXPECT_EQ(enteredSpy.count(), 1);
  EXPECT_EQ(exitedSpy.count(), 3);

  EXPECT_EQ(exitedSpy[0].first().toInt(), 4);
  EXPECT_EQ(exitedSpy[1].first().toInt(), 3);
  EXPECT_EQ(exitedSpy[2].first().toInt(), 1);
}

TEST_F(StateMachineTest, handleSuperStateSameParent)
{
  enum State
  {
    S1, // 0
    S2, // 1
    S3, // 2
    S4, // 3
    S5, // 4
    S6  // 5
  };

  StateMachine machine;
  machine.setObjectName("machine");

  // add all the states
  machine.addState(S1, QStringLiteral("S1"));
  machine.addState(S1, S2, QStringLiteral("S2"));
  machine.addState(S2, S3, QStringLiteral("S3"));
  machine.addState(S3, S4, QStringLiteral("S4"));
  machine.addState(S4, S5, QStringLiteral("S5"));
  machine.addState(S5, S6, QStringLiteral("S6"));

  // add the transitions       From State->Event->To State
  EXPECT_FALSE(machine.addTransition(S1, e1, S2));
  EXPECT_FALSE(machine.addTransition(S2, e2, S3));
  EXPECT_FALSE(machine.addTransition(S3, e3, S4));
  EXPECT_FALSE(machine.addTransition(S4, e4, S5));
  EXPECT_TRUE(machine.addTransition(S5, e5, S6));
  EXPECT_FALSE(machine.addTransition(S6, e6, S2));
}

// ={=========================================================================

static void GoogleTestRunner(int argc, char **argv)
{
  // Since Google Mock depends on Google Test, InitGoogleMock() is
  // also responsible for initializing Google Test.  Therefore there's
  // no need for calling testing::InitGoogleTest() separately.
  testing::InitGoogleMock(&argc, argv);
  int result = RUN_ALL_TESTS();

  QCoreApplication::exit(result);
}

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  // note: this do not exit when finishes.
  //
  // // Since Google Mock depends on Google Test, InitGoogleMock() is
  // // also responsible for initializing Google Test.  Therefore there's
  // // no need for calling testing::InitGoogleTest() separately.
  // testing::InitGoogleMock(&argc, argv);
  // RUN_ALL_TESTS();

  // This will run the functor from the application event loop.
  QTimer::singleShot(0, &app, std::bind(&GoogleTestRunner, argc, argv));

  return app.exec();
}
