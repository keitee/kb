#ifndef QT_CLASS_H
#define QT_CLASS_H

#include <QTimer>
#include <QDebug>
#include <QThread>

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

namespace qt_thread_1
{
  class Worker : public QObject
  {
    Q_OBJECT

      public slots:
      void progress()
      {
        qCritical() << "Worker::progress: starts";

        // here is the expensive or blocking operations
        QThread::sleep(10);

        qCritical() << "Worker::progress: finishes";

        emit finished();
      }

signals:
    void finished();
    void error(QString err);
  };
} // namespace

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
