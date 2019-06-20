#ifndef QT_CLASS_H
#define QT_CLASS_H

#include <QTimer>


class UseTimer : public QObject
{
  Q_OBJECT

  public:
    explicit UseTimer()
    {
      QObject::connect(timer_, &QTimer::timeout,
          this, &timer_update_slot);
      timer_.start(1000);
    }

    virtual ~UseTimer() {}

private slots:
  void timer_update_slot()
  {
    qCritical() << "timer updated...";
  }

  private:
    QTimer timer_;
};

#endif // QT_CLASS_H
