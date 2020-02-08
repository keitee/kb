//
//  from statemachine.h
//  BleRcuDaemon
//

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <QAtomicInteger>
#include <QDebug>
#include <QEvent>
#include <QList>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QQueue>
#include <QSet>

#include <functional>

class StateMachine : public QObject
{
  Q_OBJECT

public:
  StateMachine(QObject *parent = nullptr);
  virtual ~StateMachine();

protected:
  void customEvent(QEvent *event) override;
  void timerEvent(QTimerEvent *event) override;

public:
  bool addState(int state, const QString &name = QString());
  bool addState(int parentState, int state, const QString &name = QString());

  bool addTransition(int fromState, QEvent::Type eventType, int toState);
//	bool addTransition(int fromState, const QObject *sender, const char *signal,
//	                   int toState);
#ifdef Q_QDOC
  bool addTransition(int fromState,
                     const QObject *sender,
                     PointerToMemberFunction signal,
                     int toState);
#else
  template <typename Func>
  inline bool
  addTransition(int fromState,
                const typename QtPrivate::FunctionPointer<Func>::Object *obj,
                Func signal,
                int toState)
  {
    qint64 id = m_signalIdCounter++;

    std::function<void()> functor =
      std::bind(&StateMachine::onSignalTransition, this, id);

    QMetaObject::Connection c =
      QObject::connect(obj, signal, this, functor, Qt::AutoConnection);
    if (c && !setSignalTransition(fromState, toState, id))
    {
      QObject::disconnect(c);
      return false;
    }

    return c;
  }
#endif // Q_QDOC

  bool setInitialState(int state);
  bool setInitialState(int parentState, int state);

  bool setFinalState(int state);
  bool setFinalState(int parentState, int state);

  bool postEvent(QEvent::Type eventType);
  qint64 postDelayedEvent(QEvent::Type eventType, int delay);
  bool cancelDelayedEvent(qint64 id);
  bool cancelDelayedEvents(QEvent::Type eventType);

  int state() const;
  bool inState(int state) const;
  bool inState(const QSet<int> &states) const;

  bool isRunning() const;

  QtMsgType transistionLogLevel() const;
  const QLoggingCategory *transistionLogCategory() const;
  void setTransistionLogLevel(QtMsgType type,
                              const QLoggingCategory *category = nullptr);

public:
  static const QEvent::Type FinishedEvent = QEvent::StateMachineWrapped;

public slots:
  bool start();
  void stop();

signals:
  void finished();

  void entered(int state);
  void exited(int state);
  void transition(int fromState, int toState);

private:
  bool setSignalTransition(int fromState, int toState, qint64 signalId);
  void onSignalTransition(qint64 signalId);

  int shouldMoveState(QEvent::Type eventType) const;
  void triggerStateMove(int newState);
  void moveToState(int newState);

  QList<int> stateTreeFor(int state, bool bottomUp) const;

  void cleanUpEvents();

  void logTransition(int oldState, int newState) const;

private:
  QtMsgType m_transitionLogLevel;
  QLoggingCategory const *m_transitionLogCategory;

private:
  struct Transition
  {
    int targetState;
    enum
    {
      EventTransition,
      SignalTransition
    } type;
    union
    {
      QEvent::Type eventType;
      qint64 signalId;
    };
  };

  struct State
  {
    int parentState;
    int initialState;
    bool hasChildren;
    bool isFinal;
    QString name;
    QList<Transition> transitions;
  };

  QMap<int, State> m_states;

  int m_currentState;
  int m_initialState;
  int m_finalState;

  bool m_running;

  QAtomicInteger<qint64> m_signalIdCounter;

  bool m_stopPending;
  bool m_withinStateMover;
  QQueue<QEvent::Type> m_localEvents;

private:
  struct DelayedEvent
  {
    int timerId;
    QEvent::Type eventType;
  };

  QMutex m_delayedEventsLock;
  qint64 m_delayedEventIdCounter;
  QMap<qint64, DelayedEvent> m_delayedEvents;
};

#endif // !defined(STATEMACHINE_H)
