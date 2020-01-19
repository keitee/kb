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

  public slots:
    void setValue(int value);
    void doSomethingLong();

  // Error: Missing access specifier for slots
  // slots:
  //
  // private slots:
  // error: ‘void Counter::setValue(int)’ is private within this context


  signals:
    void valueChanged(int newValue);

  private:
    int m_value;
};


// ={=========================================================================

class Timer : public QObject
{
  Q_OBJECT

  public:
    Timer() : count_(0) 
    { 
      timer_ = QSharedPointer<QTimer>::create(this);
    }

    int getCount() const { return count_; }

  public slots:
    void setSingleShot()
    {
      connect(timer_.data(), SIGNAL(timeout()), 
          this, SLOT(onExpired()));

      timer_->start(1000);
    }

    void useSingleShot()
    {
      QTimer::singleShot(1000, this, SLOT(onExpired()));
    }

    void onExpired()
    {
      // qDebug() << "UseTimer: timer expired";
      emit timerExpired(count_);
    }

    void setContinuousShot()
    {
      connect(timer_.data(), SIGNAL(timeout()), 
          this, SLOT(onExpiredAndContinue()));

      timer_->start(1000);
    }

    void onExpiredAndContinue()
    {
      // qDebug() << "timer expired: isSingleShot: " << timer_->isSingleShot();

      if (count_ < 3)
      {
        count_++;

        // NOTE: From then on, it will emit the timeout() signal 
        // *at constant intervals.* so don't need this
        // timer_->start(1000);
      }
      else
        emit timerExpired(count_);
    }

  signals:
    void timerExpired(int count);

  private:
    int count_;
    QSharedPointer<QTimer> timer_;
};


class TimerQObject : public QObject
{
  Q_OBJECT

  public:
    // TimerQObject(std::condition_variable &cv)
    //   : cv_(cv)

  public:
    TimerQObject()
    { 
      int timerid{};

      timerid = startTimer(1000);
      // if (timerid <= 0)
      //   qDebug() << "failed to create timer(1000)";
      // else
      //   qDebug() << "created timer id: " << timerid;

      count_ = 0; 
    }

    int getCount() const { return count_; }

  protected:

    // void QObject::timerEvent(QTimerEvent *event)
    void timerEvent(QTimerEvent *event)
    {
      count_++;

      // qDebug() << "expired timer id: " << event->timerId();

      // if (count_ >= 3)
      //   cv_.notify_one();
    }


  // to use QSignalSpy
  signals:
    void timerExpired(int value);

  private:
    // std::condition_variable &cv_;
    int count_;
};



// ={=========================================================================

class Thread : public QObject
{
  Q_OBJECT

public:
  Thread(std::mutex &m, std::condition_variable &cv) : m_(m), cv_(cv) {}

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

class ThreadStandAlone : public QObject
{
  Q_OBJECT

public:
  ThreadStandAlone() {}

public slots:

  void progress()
  {
    qCritical() << "Worker::progress: starts";

    // here is the expensive or blocking operations
    QThread::sleep(5);

    qCritical() << "Worker::progress: finishes";

    QMetaObject::invokeMethod(QCoreApplication::instance(), "quit");
  }
};

class ThreadDoNotWork : public QObject
{
  Q_OBJECT

public:
  ThreadDoNotWork(Counter &counter, std::mutex &m, std::condition_variable &cv)
      : sender_counter_(counter), m_(m), cv_(cv)
  {
    QObject::connect(&counter, &Counter::valueChanged, &counter,
                     &Counter::doSomethingLong);
  }

public slots:

  void progress()
  {
    std::unique_lock<std::mutex> lock(m_);

    qCritical() << "emit valueChanged";
    sender_counter_.valueChanged(12);

    qCritical() << "Worker::progress: starts";

    // waits for doSomethingLong() which do "value =5"
    //
    // while (counter_.value() != 5)
    // {
    //   QThread::sleep(1);
    //   qCritical() << "checking values";
    // }

    if (sender_counter_.value() == 0)
      qCritical() << "counter value is 0";

    qCritical() << "counter value is " << sender_counter_.value();

    qCritical() << "Worker::progress: finishes";

    cv_.notify_one();
  }

private:
  Counter &sender_counter_;
  std::mutex &m_;
  std::condition_variable &cv_;
};

class ThreadStandAloneX : public QObject
{
  Q_OBJECT

public:
  ThreadStandAloneX(Counter &counter) : sender_counter_(counter)
  {
    QObject::connect(&counter, &Counter::valueChanged, &counter,
                     &Counter::doSomethingLong);
  }

public slots:

  void progress()
  {
    qCritical() << "emit valueChanged";
    sender_counter_.valueChanged(12);

    qCritical() << "Worker::progress: starts";

    // here is the expensive or blocking operations
    QThread::sleep(5);

    qCritical() << "counter value is " << sender_counter_.value();

    qCritical() << "Worker::progress: finishes";

    QMetaObject::invokeMethod(QCoreApplication::instance(), "quit");
  }

private:
  Counter &sender_counter_;
};

class ThreadWork : public QObject
{
  Q_OBJECT

public:
  ThreadWork(std::mutex &m, std::condition_variable &cv) : m_(m), cv_(cv)
  {
    QObject::connect(&counter_, &Counter::valueChanged, &counter_,
                     &Counter::doSomethingLong);
  }

public slots:

  void progress()
  {
    std::unique_lock<std::mutex> lock(m_);

    qCritical() << "emit valueChanged";
    counter_.valueChanged(12);

    qCritical() << "Worker::progress: starts";

    // waits for doSomethingLong() which do "value =5"
    //
    while (counter_.value() != 5)
    {
      QThread::sleep(1);
      qCritical() << "counter value is " << counter_.value();
    }

    if (counter_.value() == 5)
      qCritical() << "counter value is 5";

    qCritical() << "counter value is " << counter_.value();

    qCritical() << "Worker::progress: finishes";

    cv_.notify_one();
  }

private:
  Counter counter_;
  std::mutex &m_;
  std::condition_variable &cv_;
};

class ThreadStandAloneY : public QObject
{
  Q_OBJECT

public:
  ThreadStandAloneY()
  {
    QObject::connect(&counter_, &Counter::valueChanged, &counter_,
                     &Counter::doSomethingLong);
  }

public slots:

  void progress()
  {
    qDebug() << "thread: " << thread();

    qCritical() << "emit valueChanged";
    counter_.valueChanged(12);

    qCritical() << "Worker::progress: starts";

    // here is the expensive or blocking operations
    QThread::sleep(5);

    qCritical() << "counter value is " << counter_.value();

    qCritical() << "Worker::progress: finishes";

    QMetaObject::invokeMethod(QCoreApplication::instance(), "quit");
  }

private:
  Counter counter_;
};

#if 0



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

#endif 

#endif // QT_CLASS_H
