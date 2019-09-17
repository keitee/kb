#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <queue>
#include <future>

#include "gmock/gmock.h"

#include "qfsm.h"

using namespace testing;

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
    void SetUp()
    {
    }

    void TearDown()
    {
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

  std::map<int, unsigned> enters;
  std::map<int, unsigned> exits;

  auto entered = [&](int state)
  { 
    enters[state]++;
    // std::cout << "entered state: " << state << std::endl;
  };

  auto exited = [&](int state)
  {
    exits[state]++;
    // std::cout << "exited state: " << state << std::endl;
  };

  EXPECT_TRUE(machine.connect(entered, exited));

  EXPECT_TRUE(machine.start());

  EXPECT_TRUE(machine.isRunning());

  // return current state
  EXPECT_EQ(machine.state(), int(initialState));

  machine.stop();

  EXPECT_FALSE(machine.isRunning());

  EXPECT_THAT(enters[initialState], 1);
  EXPECT_THAT(exits.size(), 0);
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
  
  std::map<int, unsigned> enters;
  std::map<int, unsigned> exits;

  auto entered = [&](int state)
  { 
    enters[state]++;
    // std::cout << "entered state: " << state << std::endl;
  };

  auto exited = [&](int state)
  {
    exits[state]++;
    // std::cout << "exited state: " << state << std::endl;
  };

  EXPECT_TRUE(machine.connect(entered, exited));

  EXPECT_FALSE(machine.start());

  EXPECT_FALSE(machine.isRunning());

  EXPECT_THAT(enters.size(), 0);
  EXPECT_THAT(exits.size(), 0);
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
  EXPECT_FALSE(machine.addTransition(s1, 1, invalid1));
  EXPECT_FALSE(machine.addTransition(invalid1, 1, invalid2));
  EXPECT_FALSE(machine.addTransition(invalid1, 1, s2));

  // add transition with invalid event type
  EXPECT_FALSE(machine.addTransition(s1, -1, s2));
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

  std::map<int, unsigned> enters;
  std::map<int, unsigned> exits;

  std::function<void(int)> lambda = [&](int state){
    enters[state]++;
    machine.stop();
  };

  auto exited = [&](int state)
  {
    exits[state]++;
    // std::cout << "exited state: " << state << std::endl;
  };

  EXPECT_TRUE(machine.connect(lambda, exited));

  // start the state machine
  machine.start();

  EXPECT_FALSE(machine.isRunning());

  EXPECT_THAT(enters[initialState], 1);
  EXPECT_THAT(exits.size(), 0);
}


// N/A since do not support the deplayed post
// TEST_F(StateMachineTest, stopWithinDelayedSlot)


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


  std::map<int, unsigned> enters;
  std::map<int, unsigned> exits;

  auto entered = [&](int state)
  { 
    enters[state]++;
    // std::cout << "entered state: " << state << std::endl;
  };

  auto exited = [&](int state)
  {
    exits[state]++;
    // std::cout << "exited state: " << state << std::endl;
  };

  EXPECT_TRUE(machine.connect(entered, exited));


  machine.start();          // s1 enter
  machine.postEvent(e1);    // s2 enter, s1 exit
  machine.postEvent(e2);    // s2 enter, s2 exit
  machine.stop();

  EXPECT_FALSE(machine.isRunning());

  EXPECT_EQ(enters[s1], 1U);
  EXPECT_EQ(exits[s1], 1U);

  EXPECT_EQ(enters[s2], 2U);
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
  //  s1 (1)
  //
  // |s1_2 (2)             |  s1_1
  // |                     |
  // |[s1_2_1   s1_2_2(5)] |
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


  // {
  std::vector<int> enters;
  std::vector<int> exits;

  auto entered = [&](int state)
  { 
    enters.push_back(state);
  };

  auto exited = [&](int state)
  {
    exits.push_back(state);
  };

  EXPECT_TRUE(machine.connect(entered, exited));
  // }


  machine.start();

  EXPECT_TRUE(machine.isRunning());
  EXPECT_EQ(enters.size(), 1);

  // Removes all items from the list.
  enters.clear();

  // since e1 pass through 3 states from init state which is not in (old) state
  // set.

  machine.postEvent(e1);
  EXPECT_EQ(enters.size(), 3);

  // check the order of `entered` signals
  EXPECT_THAT(enters, ElementsAre(1,3,5)); 
}

TEST_F(StateMachineTest, handleSuperStateNotSameParent)
{
  enum State {
    ServiceUnavailableState,        // 0
    ServiceAvailableSuperState,     // 1
    AdapterUnavailableState,        // 2
    AdapterAvailableSuperState,     // 3
    AdapterPoweredOffState,         // 4
    AdapterPoweredOnState,          // 5
    ShutdownState                   // 6
  };

  enum Event {
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


  StateMachine machine;
  machine.setObjectName("machine");

  // add all the states
  machine.addState(ServiceUnavailableState, "ServiceUnavailableState");
  machine.addState(ServiceAvailableSuperState, "ServiceAvailableSuperState");

  machine.addState(ServiceAvailableSuperState, AdapterUnavailableState, "AdapterUnavailableState");
  machine.addState(ServiceAvailableSuperState, AdapterAvailableSuperState, "AdapterAvailableSuperState");

  machine.addState(AdapterAvailableSuperState, AdapterPoweredOffState, "AdapterPoweredOffState");
  machine.addState(AdapterAvailableSuperState, AdapterPoweredOnState, "AdapterPoweredOnState");

  machine.addState(ShutdownState, "ShutdownState");


  // add the transitions       From State	              ->    Event                  ->  To State
  machine.addTransition(ServiceUnavailableState,       ServiceAvailableEvent,      AdapterUnavailableState);
  machine.addTransition(ServiceUnavailableState,       ServiceRetryEvent,          ServiceUnavailableState);
  machine.addTransition(ServiceAvailableSuperState,    ServiceUnavailableEvent,    ServiceUnavailableState);
  machine.addTransition(ServiceAvailableSuperState,    ShutdownEvent,              ShutdownState);

  machine.addTransition(AdapterUnavailableState,       AdapterAvailableEvent,      AdapterPoweredOffState);
  machine.addTransition(AdapterUnavailableState,       AdapterRetryAttachEvent,    AdapterUnavailableState);
  machine.addTransition(AdapterAvailableSuperState,    AdapterUnavailableEvent,    AdapterUnavailableState);

  machine.addTransition(AdapterPoweredOffState,        AdapterPoweredOnEvent,      AdapterPoweredOnState);
  machine.addTransition(AdapterPoweredOffState,        AdapterRetryPowerOnEvent,   AdapterPoweredOffState);
  machine.addTransition(AdapterPoweredOnState,         AdapterPoweredOffEvent,     AdapterPoweredOffState);


  // {
  std::vector<int> enters;
  std::vector<int> exits;

  auto entered = [&](int state)
  { 
    // std::cout << "entered state: " << state << std::endl;
    enters.push_back(state);
  };

  auto exited = [&](int state)
  {
    // std::cout << "exited state: " << state << std::endl;
    exits.push_back(state);
  };

  EXPECT_TRUE(machine.connect(entered, exited));
  // }


  // set the initial state
  machine.setInitialState(AdapterPoweredOffState);
  machine.start();

  // Removes all items from the list.
  enters.clear();
  
  // post a event to state which don't have same parent and output is:
  //
  // exited state 4
  // exited state 3
  // exited state 1
  // entered state 0

  machine.postEvent(ServiceUnavailableEvent);

  EXPECT_EQ(enters.size(), 1);
  EXPECT_EQ(exits.size(), 3);

  // check the order of `entered` signals
  EXPECT_THAT(exits, ElementsAre(4, 3, 1)); 
}


TEST_F(StateMachineTest, handleSuperStateSameParent)
{
  enum State {
    S1,     // 0
    S2,     // 1
    S3,     // 2
    S4,     // 3
    S5,     // 4
    S6      // 5
  };

  StateMachine machine;
  machine.setObjectName("machine");

  // add all the states
  machine.addState(S1, ("S1"));
  machine.addState(S1, S2, ("S2"));
  machine.addState(S2, S3, ("S3"));
  machine.addState(S3, S4, ("S4"));
  machine.addState(S4, S5, ("S5"));
  machine.addState(S5, S6, ("S6"));


  // add the transitions       From State->Event->To State
  EXPECT_FALSE(machine.addTransition(S1,       e1,      S2));
  EXPECT_FALSE(machine.addTransition(S2,       e2,      S3));
  EXPECT_FALSE(machine.addTransition(S3,       e3,      S4));
  EXPECT_FALSE(machine.addTransition(S4,       e4,      S5));
  EXPECT_TRUE(machine.addTransition(S5,       e5,      S6));
  EXPECT_FALSE(machine.addTransition(S6,       e6,      S2));
}


int main(int argc, char **argv)
{
  // Since Google Mock depends on Google Test, InitGoogleMock() is
  // also responsible for initializing Google Test.  Therefore there's
  // no need for calling testing::InitGoogleTest() separately.
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
