#include <chrono>
#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include "gmock/gmock.h"

#include "qtfsm.h"

#include <QCoreApplication>
#include <QDebug>
#include <QList>
#include <QLoggingCategory>
#include <QSignalSpy>
#include <QString>
#include <QTimer>

using namespace testing;

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

  StateMachine fsm;

  // NOTE: setObjectName comes from Qt but not StateMachine
  fsm.setObjectName("machine");

  EXPECT_TRUE(fsm.addState(initialState));
  EXPECT_TRUE(fsm.setInitialState(initialState));

  // https://doc.qt.io/qt-5/qsignalspy.html#details

  QSignalSpy enteredSpy(&fsm, &StateMachine::entered);
  QSignalSpy exitedSpy(&fsm, &StateMachine::exited);

  // start() only emits `entered`
  EXPECT_TRUE(fsm.start());

  EXPECT_EQ(enteredSpy.count(), 1);
  EXPECT_TRUE(fsm.isRunning());
  EXPECT_EQ(fsm.state(), int(initialState));

  fsm.stop();

  EXPECT_EQ(exitedSpy.count(), 0);
  EXPECT_FALSE(fsm.isRunning());
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

  //  s1                          s2              init
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

TEST_F(StateMachineTest, handleFinishedState)
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

  StateMachine machine;
  machine.setObjectName("machine");

  // {
  //   s1,
  //    s1_1,
  //    s1_2,
  //      s1_2_1,
  //      s1_2_2,
  //   s2
  // };

  EXPECT_TRUE(machine.addState(s1));
  EXPECT_TRUE(machine.addState(s1, s1_1));
  EXPECT_TRUE(machine.addState(s1, s1_2));
  EXPECT_TRUE(machine.addState(s1_2, s1_2_1));
  EXPECT_TRUE(machine.addState(s1_2, s1_2_2));
  EXPECT_TRUE(machine.addState(s2));

  EXPECT_TRUE(machine.setInitialState(s1));
  EXPECT_TRUE(machine.setFinalState(s1));

  EXPECT_TRUE(machine.addTransition(s1, e1, s1_2_2));

  QSignalSpy enteredSpy(&machine, &StateMachine::entered);

  machine.start();

  EXPECT_TRUE(machine.isRunning());
  EXPECT_EQ(enteredSpy.count(), 1);

  // Removes all items from the list.
  enteredSpy.clear();

  // since e1 pass through 3 states from init state which is not in (old) state
  // set.

  machine.postEvent(e1);
  EXPECT_EQ(enteredSpy.count(), 2);

  // NOTE: cause core
  // EXPECT_EQ(enteredSpy[0].first().toInt(), int(s1));     // 1

  // check the order of `entered` signals
  EXPECT_EQ(enteredSpy[0].first().toInt(), int(s1_2));   // 3
  EXPECT_EQ(enteredSpy[1].first().toInt(), int(s1_2_2)); // 5
}

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
