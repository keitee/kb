#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <queue>
#include <future>

#include "gmock/gmock.h"

#include "qfsm.h"

#include <QList>
#include <QString>
#include <QDebug>
#include <QLoggingCategory>
#include <QCoreApplication>
#include <QSignalSpy>
#include <QTimer>

using namespace testing;

// (gdb) b AlgoList_Divide_Test::TestBody()

/*
 
class UseFsm 
{
  private:
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
      // add all the states
      m_stateMachine.addState(ServiceUnavailableState, QStringLiteral("ServiceUnavailableState"));

      // add the transitions       From State	              ->    Event                  ->  To State
      m_stateMachine.addTransition(ServiceUnavailableState,       ServiceAvailableEvent,      AdapterUnavailableState);

      // connect to the state entry and exit signals
      QObject::connect(&m_stateMachine, &StateMachine::entered,
        this, &BleRcuManagerImpl::onStateEntry);
      QObject::connect(&m_stateMachine, &StateMachine::exited,
        this, &BleRcuManagerImpl::onStateExit);
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
    Q_ASSERT(QObject::thread() == QThread::currentThread());

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


Nested states:

for old state set, only emit exited signal for states which are not in new state
set. smae for new state set.  becuase if do blindly, emit exited(call exit eaf)
for states we're entering.

so as test cases, if 3 nested states, call 3 entered signals.

that is shouldMoveState() search for possible new state from transition tables
of states from current to top parents. this is how event is handled in nested
state which do not have a transition table and is handled by parent(super)
state.

The unusual thing is that all exited/enterede are fired along the transition
path and client can decice what to handle.


*/


// ={=========================================================================

static const QEvent::Type e1 = static_cast<QEvent::Type>(QEvent::User + 100);
static const QEvent::Type e2 = static_cast<QEvent::Type>(QEvent::User + 101);
static const QEvent::Type e3 = static_cast<QEvent::Type>(QEvent::User + 102);
static const QEvent::Type e4 = static_cast<QEvent::Type>(QEvent::User + 103);
static const QEvent::Type e5 = static_cast<QEvent::Type>(QEvent::User + 104);


class StateMachineTest : public ::testing::Test
{
  public:
    void SetUp()
    {
    }

    void TearDown()
    {
    }

  protected:
    void processEvents(int mintime = 0)
    {
      if (mintime <= 0) {
        QCoreApplication::processEvents();

      } else {
        volatile bool done = false;

        std::function<void()> lambda = [&] {
          done = true;
        };

        QTimer::singleShot(mintime, lambda);
        while (!done) {
          QCoreApplication::processEvents();
        }
      }
    }

  protected:
};

TEST_F(StateMachineTest, transitionToInitialState)
{
  enum {
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
  enum {
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
  enum {
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
  enum {
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

  std::function<void(int)> lambda = [&](int state){
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
  enum {
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
  std::function<void(int)> lambda = [&](int state){
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
  enum {
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
  std::function<void(int)> exitLamda = [&](int state) {	exits[state]++; };
  QObject::connect(&machine, &StateMachine::exited, exitLamda);


  machine.start();          // s1 enter
  machine.postEvent(e1);    // s2 enter, s1 exit
  machine.postEvent(e2);    // s2 enter, s2 exit
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

  enum {
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
  EXPECT_EQ(enteredSpy[0].first().toInt(), int(s1));
  EXPECT_EQ(enteredSpy[1].first().toInt(), int(s1_2));
  EXPECT_EQ(enteredSpy[2].first().toInt(), int(s1_2_2));
}


TEST_F(StateMachineTest, exitNestedStateOrder)
{
  // tests that when exiting a nested state that we receive state exit
  // notifications for the state back up through the super states

  enum {
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


TEST_F(StateMachineTest, handleSupreState)
{
  enum State {
    ServiceUnavailableState,
    ServiceAvailableSuperState,
    AdapterUnavailableState,
    AdapterAvailableSuperState,
    AdapterPoweredOffState,
    AdapterPoweredOnState,
    ShutdownState
  };

  // add all the states
  m_stateMachine.addState(ServiceUnavailableState, QStringLiteral("ServiceUnavailableState"));
  m_stateMachine.addState(ServiceAvailableSuperState, QStringLiteral("ServiceAvailableSuperState"));

  m_stateMachine.addState(ServiceAvailableSuperState, AdapterUnavailableState, QStringLiteral("AdapterUnavailableState"));
  m_stateMachine.addState(ServiceAvailableSuperState, AdapterAvailableSuperState, QStringLiteral("AdapterAvailableSuperState"));

  m_stateMachine.addState(AdapterAvailableSuperState, AdapterPoweredOffState, QStringLiteral("AdapterPoweredOffState"));
  m_stateMachine.addState(AdapterAvailableSuperState, AdapterPoweredOnState, QStringLiteral("AdapterPoweredOnState"));

  m_stateMachine.addState(ShutdownState, QStringLiteral("ShutdownState"));


  // add the transitions       From State	              ->    Event                  ->  To State
  m_stateMachine.addTransition(ServiceUnavailableState,       ServiceAvailableEvent,      AdapterUnavailableState);
  m_stateMachine.addTransition(ServiceUnavailableState,       ServiceRetryEvent,          ServiceUnavailableState);
  m_stateMachine.addTransition(ServiceAvailableSuperState,    ServiceUnavailableEvent,    ServiceUnavailableState);
  m_stateMachine.addTransition(ServiceAvailableSuperState,    ShutdownEvent,              ShutdownState);

  m_stateMachine.addTransition(AdapterUnavailableState,       AdapterAvailableEvent,      AdapterPoweredOffState);
  m_stateMachine.addTransition(AdapterUnavailableState,       AdapterRetryAttachEvent,    AdapterUnavailableState);
  m_stateMachine.addTransition(AdapterAvailableSuperState,    AdapterUnavailableEvent,    AdapterUnavailableState);

  m_stateMachine.addTransition(AdapterPoweredOffState,        AdapterPoweredOnEvent,      AdapterPoweredOnState);
  m_stateMachine.addTransition(AdapterPoweredOffState,        AdapterRetryPowerOnEvent,   AdapterPoweredOffState);
  m_stateMachine.addTransition(AdapterPoweredOnState,         AdapterPoweredOffEvent,     AdapterPoweredOffState);


  // connect to the state entry and exit signals
  QObject::connect(&m_stateMachine, &StateMachine::entered,
      this, &BleRcuManagerImpl::onStateEntry);
  QObject::connect(&m_stateMachine, &StateMachine::exited,
      this, &BleRcuManagerImpl::onStateExit);


  // set the initial state
  m_stateMachine.setInitialState(ServiceUnavailableState);
  m_stateMachine.start();
}


// ={=========================================================================

static void GoogleTestRunner(int argc, char** argv)
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
