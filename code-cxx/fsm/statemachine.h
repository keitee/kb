//
//  statemachine.h
//

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <algorithm>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <sstream>

class StateMachine
{
public:
  StateMachine();
  virtual ~StateMachine();

public:
  void setName(std::string const &name);
  bool addState(int state, std::string const &name = std::string());
  bool
  addState(int parentState, int state, std::string const &name = std::string());

  bool addTransition(int fromState, int event, int toState);

  bool setInitialState(int state);
  bool setFinalState(int state);

  bool postEvent(int event);

  int getState() const;
  bool inState(int state) const;
  bool inState(std::set<int> const &states) const;

  bool isRunning() const;

  bool transistionLogLevel() const;
  void setTransistionLogLevel(bool enable = false);

  bool connect(std::function<void(int)> entered,
               std::function<void(int)> exited);

public:
  bool start();
  void stop();

private:
  std::string name() const;
  int shouldMoveState(int event) const;
  void triggerStateMove(int newState);
  void moveToState(int newState);

  std::list<int> stateTreeFor(int state, bool bottomUp) const;

  void cleanUpEvents();

  void logTransition(int oldState, int newState) const;

private:
  bool m_transitionLogLevel;

private:
  struct Transition
  {
    int targetState;

    enum
    {
      EventTransition,
      SignalTransition
    } type;

    int event;

    Transition()
        : targetState(-1)
        , type(EventTransition)
        , event(-1)
    {}
  };

  struct State
  {
    int parentState;
    int initialState;
    bool hasChildren;
    bool isFinal;
    std::string name;
    std::list<Transition> transitions;
  };

  std::map<int, State> m_states;

  int m_currentState;
  int m_initialState;
  int m_finalState;

  bool m_running;

  bool m_stopPending;
  bool m_withinStateMover;

  // to queue up event from postEvent()
  std::queue<int> m_localEvents;

  std::string m_name;

  std::function<void(int)> m_entered;
  std::function<void(int)> m_exited;
};

#endif // !defined(STATEMACHINE_H)
