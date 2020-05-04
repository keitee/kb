#include <chrono>
#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include "gmock/gmock.h"

#include "qfsm.h"

using namespace testing;

/*
To build:
-------------------------------------------------------------------------------
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

      // build the transitions       
      // From State	              ->    Event                  ->  To State

      m_stateMachine.addTransition(
        ServiceUnavailableState,       ServiceAvailableEvent,      AdapterUnavailableState);

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


To run:
-------------------------------------------------------------------------------

// define custom Qt events

static const QEvent::Type e1  = static_cast<QEvent::Type>(QEvent::User + 100);

// post this custom event

m_stateMachine.postEvent() or;
m_stateMachine.postDelayed();


To use the final state: stop() or setFinalState() to restart fsm again
-------------------------------------------------------------------------------

1. setFinalState() is used to set m_finalState and when transition happens, it's
checked if new state is the final state. If so run codes that stop() do; make
running false, call cleanUpEvents(), and make currentState -1

2. or use enter/exit signal of the final state


To start the state machine again:
-------------------------------------------------------------------------------

1. use start() of a state macnine since it resets state machine's state.

2. use transition of a state machine which can go from the final and to the init
state and then starts again.


Each state has a list of transitions and addTransition() add a transition entry
to `from` state:

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


Super(Nested) state:
-------------------------------------------------------------------------------

1. all exit and enter gets called along the tree path for both direction; exit
or enter gets called for super or state on the path.

2. cannot move into super and cannot add transition to super. However, can set
the init state to super and this can be fixed for future?

NOTE super state is for logical state and for handling enter/exit but not state
to move into.


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

// void StateMachineTest_handleSuperStateNotSameParent_Test::TestBody();
// (gdb) b StateMachineTest_handleSuperStateNotSameParent_Test::TestBody()
// void StateMachineTest_handleSuperStateSameParent_Test::TestBody();

// ={=========================================================================

enum
{
  e1,
  e2,
  e3,
  e4,
  e5,
  e6
};

class StateMachineTest : public ::testing::Test
{
public:
  void SetUp() {}

  void TearDown() {}

protected:
};

// check if initial state gets called when starts fsm
TEST_F(StateMachineTest, transitionToInitialState)
{
  enum
  {
    initialState
  };

  StateMachine fsm;
  fsm.setName("machine");

  EXPECT_TRUE(fsm.addState(initialState));
  EXPECT_TRUE(fsm.setInitialState(initialState));

  std::map<int, unsigned> enters;
  std::map<int, unsigned> exits;

  // since it is map, will be added automatically
  auto entered = [&](int state) {
    enters[state]++;
    // std::cout << "entered state: " << state << std::endl;
  };

  auto exited = [&](int state) {
    exits[state]++;
    // std::cout << "exited state: " << state << std::endl;
  };

  // register functions
  EXPECT_TRUE(fsm.connect(entered, exited));

  EXPECT_TRUE(fsm.start());

  EXPECT_TRUE(fsm.isRunning());

  // return current state
  EXPECT_EQ(fsm.state(), int(initialState));

  fsm.stop();

  EXPECT_FALSE(fsm.isRunning());

  // only one transition which means one `entered` called
  EXPECT_THAT(enters[initialState], 1);
  EXPECT_THAT(exits.size(), 0);
}

TEST_F(StateMachineTest, startWithNoInitialState)
{
  enum
  {
    initialState
  };

  StateMachine fsm;
  fsm.setName("machine");

  EXPECT_TRUE(fsm.addState(initialState));

  // misses out fsm.setInitialState(initialState);
  // so `init state` is not set

  std::map<int, unsigned> enters;
  std::map<int, unsigned> exits;

  auto entered = [&](int state) {
    enters[state]++;
    // std::cout << "entered state: " << state << std::endl;
  };

  auto exited = [&](int state) {
    exits[state]++;
    // std::cout << "exited state: " << state << std::endl;
  };

  EXPECT_TRUE(fsm.connect(entered, exited));

  // start() return false when there's no initial state set. so isRunning() is
  // false as well.

  EXPECT_FALSE(fsm.start());
  EXPECT_FALSE(fsm.isRunning());

  EXPECT_THAT(enters.size(), 0);
  EXPECT_THAT(exits.size(), 0);
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

  StateMachine fsm;
  fsm.setName("machine");

  EXPECT_TRUE(fsm.addState(s1));
  EXPECT_TRUE(fsm.addState(s2));
  EXPECT_TRUE(fsm.addState(s3));
  EXPECT_TRUE(fsm.setInitialState(s1));

  // add an already existing state so return false
  EXPECT_FALSE(fsm.addState(s1));

  // bool addState(int parentState, int state, const QString &name = QString());
  // parent state is valid but chile state, s1, is added already. so fails.
  EXPECT_FALSE(fsm.addState(s2, s1));

  // parent is invalid since it's not added before. Only add new state when
  // parent is already added. so fails
  EXPECT_FALSE(fsm.addState(invalid1, invalid2));

  // add transition to/from invalid state. All states should be valid before
  // adiing transition. all fails
  EXPECT_FALSE(fsm.addTransition(s1, 1, invalid1));
  EXPECT_FALSE(fsm.addTransition(invalid1, 1, invalid2));
  EXPECT_FALSE(fsm.addTransition(invalid1, 1, s2));

  // add transition with invalid event type
  EXPECT_FALSE(fsm.addTransition(s1, -1, s2));
}

// check return from postEvent() since do no checking on return of postEvent()
// and it can go silent without transition. So changed postEvent() to return
// bool valure more correctly and client should check it

TEST_F(StateMachineTest, postEventError)
{
  // state
  enum
  {
    s1,
    s2,
    s3,
    s4
  };

  StateMachine fsm;
  fsm.setName("machine");

  EXPECT_TRUE(fsm.addState(s1));
  EXPECT_TRUE(fsm.addState(s2));
  EXPECT_TRUE(fsm.addState(s3));
  EXPECT_TRUE(fsm.addState(s4));
  EXPECT_TRUE(fsm.setInitialState(s1));

  EXPECT_TRUE(fsm.addTransition(s1, 1, s2));
  EXPECT_TRUE(fsm.addTransition(s2, 1, s3));
  EXPECT_TRUE(fsm.addTransition(s3, 1, s1));
  EXPECT_TRUE(fsm.addTransition(s3, 2, s4));

  std::map<int, unsigned> enters;
  std::map<int, unsigned> exits;

  auto entered = [&](int state) { enters[state]++; };

  auto exited = [&](int state) { exits[state]++; };

  EXPECT_THAT(fsm.connect(entered, exited), true);

  EXPECT_THAT(fsm.start(), true);
  EXPECT_THAT(fsm.isRunning(), true);

  EXPECT_THAT(enters.size(), 1);
  EXPECT_THAT(exits.size(), 0);

  fsm.postEvent(1);
  fsm.postEvent(1);

  // check current state
  EXPECT_THAT(fsm.state(), s3);

  EXPECT_THAT(enters.size(), 3);
  EXPECT_THAT(exits.size(), 2);

  fsm.postEvent(1);
  // check current state
  EXPECT_THAT(fsm.state(), s1);

  // expect no state change since event 2 is not in the transition.
  // LOG| F:qfsm.cpp C:int StateMachine::shouldMoveState(int) const L:00293 :
  // not found event 2 transition from current state 0
  EXPECT_THAT(fsm.postEvent(2), false);
  EXPECT_THAT(fsm.state(), s1);
}

// starts fsm again. stop() or use final state.
TEST_F(StateMachineTest, startFsmAgain1)
{
  // state
  enum
  {
    s1,
    s2,
    s3
  };

  StateMachine fsm;
  fsm.setName("machine");

  EXPECT_TRUE(fsm.addState(s1));
  EXPECT_TRUE(fsm.addState(s2));
  EXPECT_TRUE(fsm.addState(s3));
  EXPECT_TRUE(fsm.setInitialState(s1));

  EXPECT_TRUE(fsm.addTransition(s1, 1, s2));
  EXPECT_TRUE(fsm.addTransition(s2, 1, s3));

  std::map<int, unsigned> enters;
  std::map<int, unsigned> exits;

  auto entered = [&](int state) { enters[state]++; };

  auto exited = [&](int state) { exits[state]++; };

  EXPECT_THAT(fsm.connect(entered, exited), true);

  EXPECT_THAT(fsm.start(), true);
  EXPECT_THAT(fsm.isRunning(), true);

  EXPECT_THAT(enters.size(), 1);
  EXPECT_THAT(exits.size(), 0);

  fsm.postEvent(1);
  fsm.postEvent(1);

  EXPECT_THAT(enters.size(), 3);
  EXPECT_THAT(exits.size(), 2);

  // ok, try to start fsm again.
  fsm.stop();
  enters.clear();
  exits.clear();

  EXPECT_THAT(fsm.isRunning(), false);
  EXPECT_THAT(fsm.start(), true);
  EXPECT_THAT(fsm.isRunning(), true);

  EXPECT_THAT(enters.size(), 1);
  EXPECT_THAT(exits.size(), 0);

  fsm.postEvent(1);
  fsm.postEvent(1);

  EXPECT_THAT(enters.size(), 3);
  EXPECT_THAT(exits.size(), 2);
}

TEST_F(StateMachineTest, startFsmAgain2)
{
  // state
  enum
  {
    s1,
    s2,
    s3
  };

  StateMachine fsm;
  fsm.setName("machine");

  EXPECT_TRUE(fsm.addState(s1));
  EXPECT_TRUE(fsm.addState(s2));
  EXPECT_TRUE(fsm.addState(s3));
  EXPECT_TRUE(fsm.setInitialState(s1));
  EXPECT_TRUE(fsm.setFinalState(s3));

  EXPECT_TRUE(fsm.addTransition(s1, 1, s2));
  EXPECT_TRUE(fsm.addTransition(s2, 1, s3));

  std::map<int, unsigned> enters;
  std::map<int, unsigned> exits;

  auto entered = [&](int state) { enters[state]++; };

  auto exited = [&](int state) { exits[state]++; };

  EXPECT_THAT(fsm.connect(entered, exited), true);

  EXPECT_THAT(fsm.start(), true);
  EXPECT_THAT(fsm.isRunning(), true);

  EXPECT_THAT(enters.size(), 1);
  EXPECT_THAT(exits.size(), 0);

  fsm.postEvent(1);
  fsm.postEvent(1);

  EXPECT_THAT(enters.size(), 3);
  EXPECT_THAT(exits.size(), 2);

  // ok, try to start fsm again. Since used setFinalState(), no need to stop().
  // fsm.stop();
  enters.clear();
  exits.clear();

  EXPECT_THAT(fsm.isRunning(), false);
  EXPECT_THAT(fsm.start(), true);
  EXPECT_THAT(fsm.isRunning(), true);

  EXPECT_THAT(enters.size(), 1);
  EXPECT_THAT(exits.size(), 0);

  fsm.postEvent(1);
  fsm.postEvent(1);

  EXPECT_THAT(enters.size(), 3);
  EXPECT_THAT(exits.size(), 2);
}

// whenever creates FooFsm, it creates fsm and threads with runs on it. So want
// to see it deletes thread and fsm when done and can create it again.
TEST_F(StateMachineTest, useFsmWithThread)
{
  class FooFsm
  {
  private:
    StateMachine m_fsm;
    std::deque<int> m_q;
    std::condition_variable m_cv;
    std::mutex m_m;
    std::thread m_t;
    bool m_running;

    std::map<int, unsigned> enters;
    std::map<int, unsigned> exits;

  public:
    // state
    enum
    {
      s1,
      s2,
      s3
    };

  public:
    explicit FooFsm()
        : m_running(true)
        , m_t(std::thread(&FooFsm::run_, this))
    {
      m_fsm.setName("FooFsm");
      m_fsm.addState(s1);
      m_fsm.addState(s2);
      m_fsm.addState(s3);

      m_fsm.setInitialState(s1);
      m_fsm.setFinalState(s3);

      m_fsm.addTransition(s1, 1, s2);
      m_fsm.addTransition(s2, 1, s3);

      auto entered = [&](int state) {
        enters[state]++;
        // std::cout << "enters (" << state << ")" << std::endl;
      };

      auto exited = [&](int state) {
        exits[state]++;
        // std::cout << "exits (" << state << ")" << std::endl;
      };

      m_fsm.connect(entered, exited);

      m_fsm.start();
      m_fsm.isRunning();
    }

    ~FooFsm()
    {
      // it is still running
      if (m_running)
      {
        std::unique_lock<std::mutex> lock(m_m);
        m_running = false;
        lock.unlock();

        m_cv.notify_one();
        m_t.join();
        // std::cout << "thread joined" << std::endl;
      }
    }

    void post(int event)
    {
      std::lock_guard<std::mutex> lock(m_m);

      if (m_running)
      {
        m_q.push_back(event);
        m_cv.notify_one();
      }
    }

    size_t enter_size() { return enters.size(); }

    size_t exit_size() { return exits.size(); }

  private:
    void run_()
    {
      std::unique_lock<std::mutex> lock(m_m);

      while (m_running)
      {
        // wakes up when there is item in a queue or m_running changes
        m_cv.wait(lock, [&] { return !m_q.empty() || !m_running; });

        if (!m_q.empty())
        {
          // get an item(event)
          auto event = m_q.front();
          m_q.pop_front();

          // std::cout << "post evnet(" << event << ")" << std::endl;

          m_fsm.postEvent(event);
        }
      } // while

      // std::cout << "thread ends" << std::endl;
    }
  };

  {
    FooFsm foo;
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    foo.post(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    foo.post(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    EXPECT_THAT(foo.enter_size(), 3);
    EXPECT_THAT(foo.exit_size(), 2);
  }

  {
    FooFsm foo;
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    foo.post(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    foo.post(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    EXPECT_THAT(foo.enter_size(), 3);
    EXPECT_THAT(foo.exit_size(), 2);
  }
}

// start the state fsm but stops right away since it's stopped in `enter`
// eaf. TODO: what want to see here?

TEST_F(StateMachineTest, stopWithinSlot)
{
  enum
  {
    initialState,
  };

  StateMachine fsm;
  fsm.setName("machine");

  EXPECT_TRUE(fsm.addState(initialState));
  EXPECT_TRUE(fsm.setInitialState(initialState));

  std::map<int, unsigned> enters;
  std::map<int, unsigned> exits;

  std::function<void(int)> lambda = [&](int state) {
    enters[state]++;
    fsm.stop();
  };

  auto exited = [&](int state) {
    exits[state]++;
    // std::cout << "exited state: " << state << std::endl;
  };

  EXPECT_TRUE(fsm.connect(lambda, exited));

  fsm.start();

  EXPECT_FALSE(fsm.isRunning());

  EXPECT_THAT(enters[initialState], 1);
  EXPECT_THAT(exits.size(), 0);
}

// N/A since qfsm do not support the deplayed post
// TEST_F(StateMachineTest, stopWithinDelayedSlot)

// loop transition

TEST_F(StateMachineTest, receiveEntryExitTransitionSignalsOnLoopback)
{
  enum
  {
    s1,
    s2
  };

  StateMachine fsm;

  EXPECT_TRUE(fsm.addState(s1, "s1"));
  EXPECT_TRUE(fsm.addState(s2, "s2"));

  EXPECT_TRUE(fsm.setInitialState(s1));

  EXPECT_TRUE(fsm.addTransition(s1, e1, s2));
  EXPECT_TRUE(fsm.addTransition(s2, e2, s2)); // loop transition

  std::map<int, unsigned> enters;
  std::map<int, unsigned> exits;

  auto entered = [&](int state) {
    enters[state]++;
    std::cout << "entered state: " << state << std::endl;
  };

  auto exited = [&](int state) {
    exits[state]++;
    std::cout << "exited state: " << state << std::endl;
  };

  EXPECT_TRUE(fsm.connect(entered, exited));

  fsm.start(); // s1 enter

  fsm.postEvent(e1); // s1 -> s2; s2 enter, s1 exit
  fsm.postEvent(e2); // s2 <- s2; s2 enter, s2 exit

  fsm.stop();

  EXPECT_FALSE(fsm.isRunning());

  EXPECT_EQ(enters[s1], 1U);
  EXPECT_EQ(exits[s1], 1U);

  // even transiiton to self calls enter/exit calls.
  EXPECT_EQ(enters[s2], 2U);
  EXPECT_EQ(exits[s2], 1U);
}

// NOTE: super state
//
// can move to state with in super state and will get entered called for all
// states on the path in order.
//
// when transit from (outer) state to (inner) state, entered gets called for all
// states along that transition path. In this example, transit from 0 to 5 and
// entered gets called for 0,1,3,5.

TEST_F(StateMachineTest, super_enterNestedStateOrder)
{
  enum
  {
    initialState, // 0
    s1,           // 1
    s1_1,         // 2
    s1_2,         // 3
    s1_2_1,       // 4
    s1_2_2,       // 5
    s2
  };

  StateMachine fsm;
  fsm.setName("machine");

  // init
  //
  //  s1 (1)
  //
  // |s1_2 (3)             |  s1_1
  // |                     |
  // |[s1_2_1   s1_2_2(5)] |
  //
  // bool addState(int parentState, int state, const QString &name = QString());

  EXPECT_TRUE(fsm.addState(initialState));
  EXPECT_TRUE(fsm.addState(s1));
  EXPECT_TRUE(fsm.addState(s1, s1_1)); // s1 is parent
  EXPECT_TRUE(fsm.addState(s1, s1_2));
  EXPECT_TRUE(fsm.addState(s1_2, s1_2_1)); // s1_2 is parent
  EXPECT_TRUE(fsm.addState(s1_2, s1_2_2));
  EXPECT_TRUE(fsm.addState(s2));

  EXPECT_TRUE(fsm.setInitialState(initialState));

  EXPECT_TRUE(fsm.addTransition(initialState, e1, s1_2_2));

  // { common code
  std::vector<int> enters;
  std::vector<int> exits;

  // since not use QSignalSpy from Qt
  auto entered = [&](int state) { enters.push_back(state); };
  auto exited  = [&](int state) { exits.push_back(state); };

  EXPECT_TRUE(fsm.connect(entered, exited));
  // }

  fsm.start();

  EXPECT_TRUE(fsm.isRunning());
  EXPECT_EQ(enters.size(), 1);

  // Removes all items from the list in order not to count it from init state
  enters.clear();

  // since e1 pass through 3 states from init state which is not in (old) state
  // set. from init (no super) to s1_2_2 (no super)

  fsm.postEvent(e1);
  EXPECT_EQ(enters.size(), 3);

  // check the order of `entered` signals
  EXPECT_THAT(enters, ElementsAre(1, 3, 5));
}

TEST_F(StateMachineTest, super_exitNestedStateOrder)
{
  enum
  {
    initialState, // 0
    s1,           // 1
    s1_1,         // 2
    s1_2,         // 3
    s1_2_1,       // 4
    s1_2_2,       // 5
    s2            // 6
  };

  StateMachine fsm;
  fsm.setName("machine");

  // enum
  // {
  //   initialState, // 0
  //   s1,           // 1
  //    s1_1,        // 2
  //    s1_2,        // 3
  //      s1_2_1,    // 4
  //      s1_2_2,    // 5
  //   s2            // 6
  // };
  //
  // bool addState(int parentState, int state, const QString &name = QString());

  EXPECT_TRUE(fsm.addState(initialState));
  EXPECT_TRUE(fsm.addState(s1));
  EXPECT_TRUE(fsm.addState(s1, s1_1)); // s1 is parent
  EXPECT_TRUE(fsm.addState(s1, s1_2));
  EXPECT_TRUE(fsm.addState(s1_2, s1_2_1)); // s1_2 is parent
  EXPECT_TRUE(fsm.addState(s1_2, s1_2_2));
  EXPECT_TRUE(fsm.addState(s2));

  EXPECT_TRUE(fsm.setInitialState(s1_2_1));

  EXPECT_TRUE(fsm.addTransition(s1_2_1, e1, s2));

  // { common code
  std::vector<int> enters;
  std::vector<int> exits;

  // since not use QSignalSpy from Qt
  auto entered = [&](int state) { enters.push_back(state); };
  auto exited  = [&](int state) { exits.push_back(state); };

  EXPECT_TRUE(fsm.connect(entered, exited));
  // }

  fsm.start();

  EXPECT_TRUE(fsm.isRunning());
  EXPECT_EQ(enters.size(), 1);

  // since e1 pass through 3 states from init state which is not in (old) state
  // set. from init (no super) to s1_2_2 (no super)

  fsm.postEvent(e1);
  EXPECT_EQ(exits.size(), 3);

  // check the order of `entered` signals
  EXPECT_THAT(exits, ElementsAre(4, 3, 1));
}

TEST_F(StateMachineTest, super_noTransitionToSuper)
{
  enum
  {
    s1,
    s1_1,
    s1_2,
    s1_2_1,
    s1_2_2,
    s2
  };

  StateMachine fsm;
  fsm.setName("machine");

  EXPECT_TRUE(fsm.addState(s1));
  EXPECT_TRUE(fsm.addState(s1, s1_1));
  EXPECT_TRUE(fsm.addState(s1, s1_2));
  EXPECT_TRUE(fsm.addState(s1_2, s1_2_1));
  EXPECT_TRUE(fsm.addState(s1_2, s1_2_2));
  EXPECT_TRUE(fsm.addState(s2));

  EXPECT_TRUE(fsm.setInitialState(s1_2_1));

  EXPECT_TRUE(fsm.addTransition(s1, e2, s1_2_1));

  // not allowed to move to super state
  EXPECT_FALSE(fsm.addTransition(s1_2_1, e2, s1));
}

TEST_F(StateMachineTest, handleFinishedState1)
{
  enum
  {
    s1,
    s1_1,
    s1_2,
    s1_2_1,
    s1_2_2,
    s2
  };

  StateMachine fsm;
  fsm.setName("machine");

  // {
  //   s1,
  //    s1_1,
  //    s1_2,
  //      s1_2_1,
  //      s1_2_2,
  //   s2
  // };

  EXPECT_TRUE(fsm.addState(s1));
  EXPECT_TRUE(fsm.addState(s1, s1_1));
  EXPECT_TRUE(fsm.addState(s1, s1_2));
  EXPECT_TRUE(fsm.addState(s1_2, s1_2_1));
  EXPECT_TRUE(fsm.addState(s1_2, s1_2_2));
  EXPECT_TRUE(fsm.addState(s2));

  EXPECT_TRUE(fsm.setInitialState(s1));

  EXPECT_TRUE(fsm.setFinalState(s1_2_2));

  EXPECT_TRUE(fsm.addTransition(s1, e1, s1_2_2));
  EXPECT_TRUE(fsm.addTransition(s1_2_2, e2, s2));

  // { common code
  std::vector<int> enters;
  std::vector<int> exits;

  // since not use QSignalSpy from Qt
  auto entered = [&](int state) { enters.push_back(state); };
  auto exited  = [&](int state) { exits.push_back(state); };

  EXPECT_TRUE(fsm.connect(entered, exited));
  // }

  fsm.start();

  EXPECT_THAT(true, fsm.isRunning());
  EXPECT_THAT(enters.size(), 1);

  // Removes all items from the list.
  enters.clear();

  fsm.postEvent(e1);

  // 2 states along the path
  EXPECT_THAT(enters.size(), 2);

  // reached the final state and get `finished` and isRunning() is false
  // NOTE: since qfsm do not support `finished` callback
  // EXPECT_EQ(finisheddSpy.count(), 1);
  EXPECT_THAT(false, fsm.isRunning());

  // current state is -1 since it reached to the finished
  EXPECT_THAT(fsm.state(), -1);

  // NOTE:
  // start() makes fsm running again and set current state with init state.
  fsm.start();

  // so now true.
  EXPECT_THAT(true, fsm.postEvent(e2));

  // current state is s2
  EXPECT_THAT(fsm.state(), s1);
}

TEST_F(StateMachineTest, handleFinishedState2)
{
  enum
  {
    s1,
    s1_1,
    s1_2,
    s1_2_1,
    s1_2_2,
    s2
  };

  StateMachine fsm;
  fsm.setName("machine");

  // {
  //   s1,
  //    s1_1,
  //    s1_2,
  //      s1_2_1,
  //      s1_2_2,
  //   s2
  // };

  EXPECT_TRUE(fsm.addState(s1));
  EXPECT_TRUE(fsm.addState(s1, s1_1));
  EXPECT_TRUE(fsm.addState(s1, s1_2));
  EXPECT_TRUE(fsm.addState(s1_2, s1_2_1));
  EXPECT_TRUE(fsm.addState(s1_2, s1_2_2));
  EXPECT_TRUE(fsm.addState(s2));

  EXPECT_TRUE(fsm.setInitialState(s1));

  EXPECT_TRUE(fsm.setFinalState(s1_2_2));

  EXPECT_TRUE(fsm.addTransition(s1, e1, s1_2_2));
  EXPECT_TRUE(fsm.addTransition(s1_2_2, e2, s2));

  // { common code
  std::vector<int> enters;
  std::vector<int> exits;

  // since not use QSignalSpy from Qt
  auto entered = [&](int state) { enters.push_back(state); };
  auto exited  = [&](int state) { exits.push_back(state); };

  EXPECT_TRUE(fsm.connect(entered, exited));
  // }

  fsm.start();

  EXPECT_THAT(true, fsm.isRunning());
  EXPECT_THAT(enters.size(), 1);

  // Removes all items from the list.
  enters.clear();

  fsm.postEvent(e1);

  // 2 states along the path
  EXPECT_THAT(enters.size(), 2);

  // reached the final state and get `finished` and isRunning() is false
  // NOTE: since qfsm do not support `finished` callback
  // EXPECT_EQ(finisheddSpy.count(), 1);
  EXPECT_THAT(false, fsm.isRunning());

  // current state is -1 since it reached to the finished
  EXPECT_THAT(fsm.state(), -1);

  // NOTE:
  // start() makes fsm running again and set current state with init state.
  fsm.start();

  // so now true.
  EXPECT_THAT(true, fsm.postEvent(e2));

  // current state is s2
  EXPECT_THAT(fsm.state(), s1);
}

// use blercumanager fsm for an example using super state

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

  enum Event
  {
    ServiceRetryEvent,
    ServiceAvailableEvent,
    ServiceUnavailableEvent,
    AdapterRetryAttachEvent,
    AdapterAvailableEvent,
    AdapterUnavailableEvent,
    AdapterRetryPowerOnEvent,
    AdapterPoweredOnEvent,
    AdapterPoweredOffEvent,
    ShutdownEvent
  };

  StateMachine fsm;
  fsm.setName("machine");

  // add all the states
  fsm.addState(ServiceUnavailableState, "ServiceUnavailableState");
  fsm.addState(ServiceAvailableSuperState, "ServiceAvailableSuperState");

  fsm.addState(ServiceAvailableSuperState,
               AdapterUnavailableState,
               "AdapterUnavailableState");

  fsm.addState(ServiceAvailableSuperState,
               AdapterAvailableSuperState,
               "AdapterAvailableSuperState");

  fsm.addState(AdapterAvailableSuperState,
               AdapterPoweredOffState,
               "AdapterPoweredOffState");

  fsm.addState(AdapterAvailableSuperState,
               AdapterPoweredOnState,
               "AdapterPoweredOnState");

  fsm.addState(ShutdownState, "ShutdownState");

  // add the transitions       From State	              ->    Event                  ->  To State
  fsm.addTransition(ServiceUnavailableState,
                    ServiceAvailableEvent,
                    AdapterUnavailableState);

  fsm.addTransition(ServiceUnavailableState,
                    ServiceRetryEvent,
                    ServiceUnavailableState);

  fsm.addTransition(ServiceAvailableSuperState,
                    ServiceUnavailableEvent,
                    ServiceUnavailableState);

  fsm.addTransition(ServiceAvailableSuperState, ShutdownEvent, ShutdownState);

  fsm.addTransition(AdapterUnavailableState,
                    AdapterAvailableEvent,
                    AdapterPoweredOffState);

  fsm.addTransition(AdapterUnavailableState,
                    AdapterRetryAttachEvent,
                    AdapterUnavailableState);

  fsm.addTransition(AdapterAvailableSuperState,
                    AdapterUnavailableEvent,
                    AdapterUnavailableState);

  fsm.addTransition(AdapterPoweredOffState,
                    AdapterPoweredOnEvent,
                    AdapterPoweredOnState);

  fsm.addTransition(AdapterPoweredOffState,
                    AdapterRetryPowerOnEvent,
                    AdapterPoweredOffState);

  fsm.addTransition(AdapterPoweredOnState,
                    AdapterPoweredOffEvent,
                    AdapterPoweredOffState);

  // {
  std::vector<int> enters;
  std::vector<int> exits;

  auto entered = [&](int state) {
    // std::cout << "entered state: " << state << std::endl;
    enters.push_back(state);
  };

  auto exited = [&](int state) {
    // std::cout << "exited state: " << state << std::endl;
    exits.push_back(state);
  };

  EXPECT_TRUE(fsm.connect(entered, exited));
  // }

  // NOTE that can set any state as `init state`
  fsm.setInitialState(AdapterPoweredOffState);

  fsm.start();

  // Removes all items from the list.
  enters.clear();

  // trigger from AdapterPoweredOffState to ServiceUnavailableState
  //
  // there's no no transition defined to handle ServiceUnavailableEvent from
  // AdapterPoweredOffState but it is handled since there is super state that
  // defines that transition.
  //
  // fsm.addTransition(ServiceAvailableSuperState,
  //  ServiceUnavailableEvent,
  //  ServiceUnavailableState);
  //
  // so `exited` called along the path to exit that supre state and `entered`
  // called to enter target state.
  //
  // exited state 4, AdapterPoweredOffState
  // exited state 3, AdapterAvailableSuperState
  // exited state 1, ServiceAvailableSuperState NOTE see that it's called
  // entered state 0
  //
  // NOTE
  // that ServiceUnavailableState is not called since it is not on the tree path

  // post a event to state which don't have same parent and output is:
  // exited state 4
  // exited state 3
  // exited state 1
  // entered state 0
  fsm.postEvent(ServiceUnavailableEvent);

  EXPECT_EQ(enters.size(), 1);
  EXPECT_EQ(exits.size(), 3);

  // check the order of `entered` signals
  EXPECT_THAT(exits, ElementsAre(4, 3, 1));
}

// that is, do not allow transition to super state and which also means no
// transition from the super as well since it cannot be state to stay.

TEST_F(StateMachineTest, NoAddTransitionToSuper_0)
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

  StateMachine fsm;
  fsm.setName("machine");

  // add all the states
  fsm.addState(S1, ("S1"));
  fsm.addState(S1, S2, ("S2"));
  fsm.addState(S2, S3, ("S3"));
  fsm.addState(S3, S4, ("S4"));
  fsm.addState(S4, S5, ("S5"));
  fsm.addState(S5, S6, ("S6"));

  // add the transitions       From State->Event->To State
  //
  // LOG| F:qfsm.cpp C:bool StateMachine::addTransition(int, int, int) L:00425 : 'toState' S2(1) is a super state with no initial state set
  // LOG| F:qfsm.cpp C:bool StateMachine::addTransition(int, int, int) L:00425 : 'toState' S3(2) is a super state with no initial state set
  // LOG| F:qfsm.cpp C:bool StateMachine::addTransition(int, int, int) L:00425 : 'toState' S4(3) is a super state with no initial state set
  // LOG| F:qfsm.cpp C:bool StateMachine::addTransition(int, int, int) L:00425 : 'toState' S5(4) is a super state with no initial state set
  // LOG| F:qfsm.cpp C:bool StateMachine::addTransition(int, int, int) L:00425 : 'toState' S2(1) is a super state with no initial state set

  // cannot move to super, internally, add transition list on super, and failed
  EXPECT_FALSE(fsm.addTransition(S1, e1, S2));
  EXPECT_FALSE(fsm.addTransition(S2, e2, S3));
  EXPECT_FALSE(fsm.addTransition(S3, e3, S4));
  EXPECT_FALSE(fsm.addTransition(S4, e4, S5));

  // true since s6 is not super state
  EXPECT_TRUE(fsm.addTransition(S5, e5, S6));

  // false since s2 is super
  EXPECT_FALSE(fsm.addTransition(S6, e6, S2));
}

// WHAT's the difference??
//
// // Do not allow to add transion to super state.
//
// TEST_F(StateMachineTest, NoAddTransitionToSuper_1)
// {
//   enum
//   {
//     initialState, // 0
//     s1,           // 1
//     s1_1,         // 2
//     s1_2,         // 3
//     s1_2_1,       // 4
//     s1_2_2,       // 5
//     s2
//   };

//   StateMachine machine;
//   machine.setName("machine");

//   EXPECT_TRUE(machine.addState(initialState));
//   EXPECT_TRUE(machine.addState(s1));
//   EXPECT_TRUE(machine.addState(s1, s1_1));
//   EXPECT_TRUE(machine.addState(s1, s1_2));
//   EXPECT_TRUE(machine.addState(s1_2, s1_2_1));
//   EXPECT_TRUE(machine.addState(s1_2, s1_2_2));
//   EXPECT_TRUE(machine.addState(s2));

//   EXPECT_TRUE(machine.setInitialState(initialState));

//   // EXPECT_TRUE(machine.addTransition(initialState, e1, s1_2_2));

//   // LOG| F:qfsm.cpp C:bool StateMachine::addTransition(int, int, int) L:00428 :
//   //  'toState' (3) is a super state with no initial state set
//   EXPECT_FALSE(machine.addTransition(initialState, e1, s1_2));
// }

// since not able to build transition on super state, no transition happens to
// super.

TEST_F(StateMachineTest, NoTransitionToSuper)
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

  enum Event
  {
    ServiceRetryEvent,
    ServiceAvailableEvent,
    ServiceUnavailableEvent,
    AdapterRetryAttachEvent,
    AdapterAvailableEvent,
    AdapterUnavailableEvent,
    AdapterRetryPowerOnEvent,
    AdapterPoweredOnEvent,
    AdapterPoweredOffEvent,
    ShutdownEvent
  };

  StateMachine fsm;
  fsm.setName("machine");

  // add all the states
  fsm.addState(ServiceUnavailableState, "ServiceUnavailableState");
  fsm.addState(ServiceAvailableSuperState, "ServiceAvailableSuperState");

  fsm.addState(ServiceAvailableSuperState,
               AdapterUnavailableState,
               "AdapterUnavailableState");
  fsm.addState(ServiceAvailableSuperState,
               AdapterAvailableSuperState,
               "AdapterAvailableSuperState");

  fsm.addState(AdapterAvailableSuperState,
               AdapterPoweredOffState,
               "AdapterPoweredOffState");
  fsm.addState(AdapterAvailableSuperState,
               AdapterPoweredOnState,
               "AdapterPoweredOnState");

  fsm.addState(ShutdownState, "ShutdownState");

  // add the transitions       From State	              ->    Event                  ->  To State
  fsm.addTransition(ServiceUnavailableState,
                    ServiceAvailableEvent,
                    AdapterUnavailableState);
  fsm.addTransition(ServiceUnavailableState,
                    ServiceRetryEvent,
                    ServiceUnavailableState);
  fsm.addTransition(ServiceAvailableSuperState,
                    ServiceUnavailableEvent,
                    ServiceUnavailableState);
  fsm.addTransition(ServiceAvailableSuperState, ShutdownEvent, ShutdownState);

  fsm.addTransition(AdapterUnavailableState,
                    AdapterAvailableEvent,
                    AdapterPoweredOffState);
  fsm.addTransition(AdapterUnavailableState,
                    AdapterRetryAttachEvent,
                    AdapterUnavailableState);
  fsm.addTransition(AdapterAvailableSuperState,
                    AdapterUnavailableEvent,
                    AdapterUnavailableState);

  fsm.addTransition(AdapterPoweredOffState,
                    AdapterPoweredOnEvent,
                    AdapterPoweredOnState);
  fsm.addTransition(AdapterPoweredOffState,
                    AdapterRetryPowerOnEvent,
                    AdapterPoweredOffState);
  fsm.addTransition(AdapterPoweredOnState,
                    AdapterPoweredOffEvent,
                    AdapterPoweredOffState);

  // {
  std::vector<int> enters;
  std::vector<int> exits;

  auto entered = [&](int state) {
    std::cout << "entered state: " << state << std::endl;
    enters.push_back(state);
  };

  auto exited = [&](int state) {
    std::cout << "exited state: " << state << std::endl;
    exits.push_back(state);
  };

  EXPECT_TRUE(fsm.connect(entered, exited));
  // }

  // set the initial state
  fsm.setInitialState(AdapterPoweredOffState);
  fsm.start();

  // Removes all items from the list.
  enters.clear();

  // TODO: postEvent() do not return error. may be necessary to add?
  // LOG| F:qfsm.cpp C:int StateMachine::shouldMoveState(int) const L:00279 :
  //  not found target state from current state -1
  fsm.postEvent(ServiceAvailableSuperState);

  // since no transition is made, nothing there
  EXPECT_EQ(enters.size(), 0);
  EXPECT_EQ(exits.size(), 0);
}

int main(int argc, char **argv)
{
  // Since Google Mock depends on Google Test, InitGoogleMock() is
  // also responsible for initializing Google Test.  Therefore there's
  // no need for calling testing::InitGoogleTest() separately.
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
