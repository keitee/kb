#ifndef QT_CLASS_H
#define QT_CLASS_H

#include <mutex>
#include <condition_variable>

#include <QTimer>
#include <QDebug>
#include <QThread>
#include <QEvent>
#include <QCoreApplication>


// ={=========================================================================
// ex from https://doc.qt.io/qt-5/signalsandslots.html
// shows it's object based

class Counter : public QObject
{
  Q_OBJECT

  public:
    Counter() { m_value = 0; }

    int value() const { return m_value; }

  // private slots:
  public slots:
      void setValue(int value);
      void doSomethingLong();

  signals:
    void valueChanged(int newValue);

  private:
    int m_value;
};


// ={=========================================================================

class UseTimer : public QObject
{
  Q_OBJECT

  public:
    UseTimer() 
    { 
      timer_ = QSharedPointer<QTimer>::create(this);
      connect(timer_.data(), SIGNAL(timeout()), this, SLOT(onExpired()));
      timer_->start(2000);
      value_ = 0; 
    }

    int getValue() const { return value_; }

  // private slots:
  public slots:
    void onExpired()
    {
      // qDebug() << "UseTimer: timer expired";
      emit timerExpired(1);
    }

  signals:
    void timerExpired(int value);

  private:
    int value_;
    QSharedPointer<QTimer> timer_;
};


class UseTimerFromQObject : public QObject
{
  Q_OBJECT

  public:
    UseTimerFromQObject(std::condition_variable &cv)
      : cv_(cv)
    { 
      int timerid{};

      // Starts a timer and returns a timer identifier, or returns zero if it
      // could not start a timer.

      timerid = startTimer(50);
      if (timerid <= 0)
        qDebug() << "failed to create timer(50)";
      else
        qDebug() << "timer id: " << timerid;

      timerid = startTimer(1000);
      if (timerid <= 0)
        qDebug() << "failed to create timer(1000)";
      else
        qDebug() << "timer id: " << timerid;

      timerid = startTimer(2000);
      if (timerid <= 0)
        qDebug() << "failed to create timer(2000)";
      else
        qDebug() << "timer id: " << timerid;

      value_ = 0; 
    }

    int getValue() const { return value_; }

  protected:
    // void QObject::timerEvent(QTimerEvent *event)
    void timerEvent(QTimerEvent *event)
    {
      qDebug() << "timer id: " << event->timerId();

      if (value_ > 10)
        cv_.notify_one();
    }

  private:
    std::condition_variable &cv_;
    int value_;
};


// ={=========================================================================

class ThreadWorker : public QObject
{
  Q_OBJECT

  public:
    ThreadWorker(std::mutex &m, std::condition_variable &cv)
      : m_(m), cv_(cv)
    {}

  public slots:

    void progress()
    {
      std::unique_lock<std::mutex> lock(m_);

      qCritical() << "Worker::progress: starts";

      // here is the expensive or blocking operations
      QThread::sleep(5);

      qCritical() << "Worker::progress: finishes";

      cv_.notify_one();
    }

  private:
    std::mutex &m_;
    std::condition_variable &cv_;
};


class ThreadWorkerUseCounter : public QObject
{
  Q_OBJECT

  public:
    ThreadWorkerUseCounter(Counter &counter, std::mutex &m, std::condition_variable &cv)
      : sender_counter_(counter), m_(m), cv_(cv)
    {
      // a's valueChanged -> b's doSomethingLong
      QObject::connect(&counter, &Counter::valueChanged,
          &counter_, &Counter::doSomethingLong, Qt::QueuedConnection);

      // qCritical() << "emit valueChanged";
      // counter.valueChanged(12);
    }

  public slots:

    void progress()
    {
      std::unique_lock<std::mutex> lock(m_);

      qCritical() << "emit valueChanged";
      sender_counter_.valueChanged(12);

      qCritical() << "Worker::progress: starts";

      // waits for signal, doSomethingLong(), runs
      while (counter_.value() != 5)
      {
        QThread::sleep(1);
        qCritical() << "checking values";
      }

      qCritical() << "Worker::progress: finishes";

      cv_.notify_one();
    }

  private:
    Counter& sender_counter_;
    Counter counter_;
    std::mutex &m_;
    std::condition_variable &cv_;
};


// ={=========================================================================

// since main thread has qt event loop but do not have this pointer as not a
// object. so try to receiver and sender qt object to exercise qt custom event.

namespace
{
  const QEvent::Type CustomEvent1 = QEvent::Type(QEvent::User + 1);
  const QEvent::Type CustomEvent2 = QEvent::Type(QEvent::User + 2);
  const QEvent::Type CustomEvent3 = QEvent::Type(QEvent::User + 3);
  const QEvent::Type CustomEvent4 = QEvent::Type(QEvent::User + 4);
  const QEvent::Type CustomEvent5 = QEvent::Type(QEvent::User + 5);
} // namespace

class CustomEventSender : public QObject
{
  Q_OBJECT

  public:
    CustomEventSender(QSharedPointer<QObject> const &receiver)
      : receiver_(receiver)
    // CustomEventSender()
    {
      timer_ = QSharedPointer<QTimer>::create(this);
      connect(timer_.data(), SIGNAL(timeout()), this, SLOT(onExpired()));

      // void QTimer::start(std::chrono::milliseconds msec)
      timer_->start(1000);

      qDebug() << "CustomEventSender created";
    }

    void customEvent(QEvent *event) override
    {
      QEvent::Type eventType = event->type();
      qDebug() << "received %d" << eventType;
    }

  public slots:
    void onExpired()
    {
      qDebug() << "CustomEventSender get timer expired";

      // send events to recivers
      // void 
      // QCoreApplication::postEvent(QObject *receiver, QEvent *event, int priority = Qt::NormalEventPriority)

      // QCoreApplication::postEvent(receiver_.data(), new QEvent(CustomEvent1));
      // QCoreApplication::postEvent(receiver_.data(), new QEvent(CustomEvent2));
      // QCoreApplication::postEvent(receiver_.data(), new QEvent(CustomEvent3));
      // QCoreApplication::postEvent(receiver_.data(), new QEvent(CustomEvent4));
      // QCoreApplication::postEvent(receiver_.data(), new QEvent(CustomEvent5));

      QCoreApplication::postEvent(this, new QEvent(CustomEvent1));
      QCoreApplication::postEvent(this, new QEvent(CustomEvent2));
      QCoreApplication::postEvent(this, new QEvent(CustomEvent3));
      QCoreApplication::postEvent(this, new QEvent(CustomEvent4));
      QCoreApplication::postEvent(this, new QEvent(CustomEvent5));

      qDebug() << "CustomEventSender sent all events";

      emit timerExpired(1);
    }

  signals:
    void timerExpired(int value);

  private:
    QSharedPointer<QObject> receiver_; 
    QSharedPointer<QTimer> timer_;
};

class CustomEventReceiver : public QObject
{
  Q_OBJECT

  public:
    CustomEventReceiver(std::mutex &m, std::condition_variable &cv);

    // void QObject::customEvent(QEvent *event)

    void customEvent(QEvent *event) override;

    int getReceived();

  private:
    std::mutex &m_;
    std::condition_variable &cv_;
    int received_{};
};


namespace qt_thread_2
{
  class Worker : public QObject
  {
    Q_OBJECT

      public slots:
      void doWork(QString const &parameters)
      {
        qCritical() << "workerThrea: doWork: got " << parameters;

        QString result("finished");

        // here is the expensive or blocking operations
        QThread::sleep(10);

        emit resultReady(result);
      }

signals:
    void resultReady(const QString &result);
  };

  class Controller : public QObject
  {
    Q_OBJECT
      QThread workerThread;

    public:
    Controller()
    {
      Worker *worker = new Worker;

      // You can use worker objects by moving them to the thread using
      // QObject::moveToThread().

      worker->moveToThread(&workerThread);

      connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
      connect(this, &Controller::operate, worker, &Worker::doWork);
      connect(worker, &Worker::resultReady, this, &Controller::handleResults);

      qCritical() << "workerThread starts";
      workerThread.start();
    }

    ~Controller()
    {
      workerThread.quit();
      workerThread.wait();
    }

    public slots:
      void handleResults(const QString &);

    signals:
      void operate(const QString &);
  };
} // namespace

#endif // QT_CLASS_H
