#include <QThread>
#include <QDebug>
#include <QApplication>
#include "worker.h"

UseTimer::UseTimer() : count_(0)
{
  // QObject::connect: No such signal QTimer::QTimer::timeout()
  // QObject::connect(&timer_, SIGNAL(QTimer::timeout()),
  //    this, SLOT(timer_update_slot()));
 
  // QObject::connect(&timer_, SIGNAL(timeout()),
  //     this, SLOT(timer_update_slot()));

  // timer_.start(1000);
}

UseTimer::~UseTimer()
{
  if (timer_)
    delete timer_;
}

void UseTimer::fire_repeat_timer()
{
  timer_ = new QTimer(this);

  QObject::connect(timer_, SIGNAL(timeout()),
      this, SLOT(timer_update_slot()));

  timer_->start(1000);
}

void UseTimer::timer_update_slot()
{
  qCritical() << "timer get updated...";

  ++count_;

  if (count_ >= 10)
  {
    qCritical() << "timer stops...";
    timer_->stop();

    // same as using slot
    // QObject::connect(worker, SIGNAL (finished()), 
    //  QApplication::instance(), SLOT (quit()));
 
    QApplication::instance()->quit();
  }
}

void UseTimer::fire_single_timer()
{
  timer_ = new QTimer(this);

  QObject::connect(timer_, SIGNAL(timeout()),
      this, SLOT(timer_single_slot()));

  timer_->setSingleShot(true);
  timer_->start(1000);

}

void UseTimer::timer_single_slot()
{
  qCritical() << "timer get updated...";
  qCritical() << "timer stops...";
  timer_->stop();

  QApplication::instance()->quit();
}

void UseTimer::fire_single_timer_2()
{
  QTimer::singleShot(1000, this, SLOT(timer_single_slot_2()));
  // QTimer::singleShot(1000, this, SLOT(timer_single_slot));
}

void UseTimer::timer_single_slot_2()
{
  qCritical() << "timer get updated...";
  qCritical() << "timer stops...";

  QApplication::instance()->quit();
}
