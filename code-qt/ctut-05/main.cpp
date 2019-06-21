#include <QCoreApplication>
#include <QtWidgets>
#include "worker.h"

/*

To use timer

Signals
void	timeout()

o In multithreaded applications, you can use QTimer in any thread that has an
  event loop.

  To start an event loop from a non-GUI thread, use QThread::exec(). Qt uses the
  timer's thread affinity to determine which thread will emit the timeout()
  signal. Because of this, you must start and stop the timer in its thread; it
  is not possible to start a timer from another thread.

o As a special case, a QTimer with a timeout of 0 will time out as soon as all
  the events in the window system's event queue have been processed. This can be
  used to do heavy work while providing a snappy user interface:

void QTimer::start(int msec)

Starts or restarts the timer with a timeout interval of msec milliseconds.

If the timer is already running, it will be stopped and restarted.

If singleShot is true, the timer will be activated only once.

*/

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  //   // QObject::connect: No such signal QTimer::QTimer::timeout()
  //   // QObject::startTimer: Timers can only be used with threads started with QThread

  UseTimer us;

  // us.fire_repeat_timer();
  us.fire_single_timer();
  
  // *error* core dump
  // us.fire_single_timer_2();

  // do not work since no way to exit event loop
  // a.exec();
  // QApplication::instance()->quit();
  // return 0;

  return a.exec();
}
