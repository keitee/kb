#include "mainwindow.h"
#include "ui_mainwindow.h"

/*

https://doc.qt.io/qt-5/qtimer.html

Detailed Description

The QTimer class provides a high-level programming interface for timers. To use
it, create a QTimer, connect its timeout() signal to the appropriate slots, and
call start(). From then on, it will emit the timeout() signal at constant
intervals.

Example for a one second (1000 millisecond) timer (from the Analog Clock example):

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&AnalogClock::update));
    timer->start(1000);

From then on, the update() slot is called every second.

You can set a timer to time out only once by calling setSingleShot(true). You
can also use the static QTimer::singleShot() function to call a slot after a
specified interval:

    QTimer::singleShot(200, this, SLOT(updateCaption()));

In multithreaded applications, you can use QTimer in any thread that has an
event loop. To start an event loop from a non-GUI thread, use QThread::exec().

Qt uses the timer's thread affinity to determine which thread will emit the
timeout() signal. Because of this, you must start and stop the timer in its
thread; it is not possible to start a timer from another thread.

As a special case, a QTimer with a timeout of 0 will time out as soon as all the
events in the window system's event queue have been processed. This can be used
to do heavy work while providing a snappy user interface:

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(processOneThing()));
    timer->start();

From then on, processOneThing() will be called repeatedly. It should be written
in such a way that it always returns quickly (typically after processing one
data item) so that Qt can deliver events to the user interface and stop the
timer as soon as it has done all its work. This is the traditional way of
implementing heavy work in GUI applications, but as multithreading is nowadays
becoming available on more and more platforms, we expect that zero-millisecond
QTimer objects will gradually be replaced by QThreads.

Accuracy and Timer Resolution

The accuracy of timers depends on the underlying operating system and hardware.
Most platforms support a resolution of 1 millisecond, though the accuracy of the
timer will not equal this resolution in many real-world situations.

The accuracy also depends on the timer type. For Qt::PreciseTimer, QTimer will
try to keep the accuracy at 1 millisecond. Precise timers will also never time
out earlier than expected.

For Qt::CoarseTimer and Qt::VeryCoarseTimer types, QTimer may wake up earlier
than expected, within the margins for those types: 5% of the interval for
Qt::CoarseTimer and 500 ms for Qt::VeryCoarseTimer.

All timer types may time out later than expected if the system is busy or unable
to provide the requested accuracy. In such a case of timeout overrun, Qt will
emit timeout() only once, even if multiple timeouts have expired, and then will
resume the original interval.


Alternatives to QTimer

An alternative to using QTimer is to call QObject::startTimer() for your object
and reimplement the QObject::timerEvent() event handler in your class (which
    must inherit QObject). The disadvantage is that timerEvent() does not
support such high-level features as single-shot timers or signals.

Another alternative is QBasicTimer. It is typically less cumbersome than using
QObject::startTimer() directly. See Timers for an overview of all three
approaches.

Some operating systems limit the number of timers that may be used; Qt tries to
work around these limitations.

See also QBasicTimer, QTimerEvent, QObject::timerEvent(), Timers, Analog Clock
Example, and Wiggly Example.


https://doc.qt.io/qt-5/qelapsedtimer.html

The QElapsedTimer class is usually used to quickly calculate how much time has
elapsed between two events. Its API is similar to that of QTime, so code that
was using that can be ported quickly to the new class.

However, unlike QTime, QElapsedTimer tries to use monotonic clocks if possible.
This means it's not possible to convert QElapsedTimer objects to a
human-readable time.

The typical use-case for the class is to determine how much time was spent in a
slow operation. The simplest example of such a case is for debugging purposes,
as in the following example:

    QElapsedTimer timer;
    timer.start();

    slowOperation1();

    qDebug() << "The slow operation took" << timer.elapsed() << "milliseconds";

In this example, the timer is started by a call to start() and the elapsed timer
is calculated by the elapsed() function.

The time elapsed can also be used to recalculate the time available for another
operation, after the first one is complete. This is useful when the execution
must complete within a certain time period, but several steps are needed. The
waitFor-type functions in QIODevice and its subclasses are good examples of such
need. In that case, the code could be as follows:

void executeSlowOperations(int timeout)
{
    QElapsedTimer timer;
    timer.start();
    slowOperation1();

    int remainingTime = timeout - timer.elapsed();
    if (remainingTime > 0)
        slowOperation2(remainingTime);
}

Another use-case is to execute a certain operation for a specific timeslice. For
this, QElapsedTimer provides the hasExpired() convenience function, which can be
used to determine if a certain number of milliseconds has already elapsed:

void executeOperationsForTime(int ms)
{
    QElapsedTimer timer;
    timer.start();

    while (!timer.hasExpired(ms))
        slowOperation1();
}

It is often more convenient to use QDeadlineTimer in this case, which counts
towards a timeout in the future instead of tracking elapsed time.

 
*/


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    count_(0)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(timer_expired()));
  timer->start(1000);
}

void MainWindow::timer_expired()
{
    ++count_;
    QString mesg = QString("timer expired %1").arg(count_);
    ui->textEdit->setPlainText(mesg);
}

void MainWindow::on_pushButton_2_clicked()
{
  ui->textEdit->setPlainText("Single Shot timer is started");
  QTimer *timer = new QTimer(this);
  timer->setSingleShot(true);
  timer->setInterval(2000);
  timer->start();
  connect(timer, SIGNAL(timeout()), this, SLOT(single_timer_expired()));
}

void MainWindow::single_timer_expired()
{
    ui->textEdit->setPlainText("Single Shot timer is expired");
}

void MainWindow::on_pushButton_3_clicked()
{
    QElapsedTimer *timer = new QElapsedTimer();

    for (int i = 0; i < 2000000; ++i)
    {
        QString mesg = "mesg";
    }

    QString mesg = QString("timer elapsed %1 ms").arg(timer->elapsed());
    ui->textEdit->setPlainText(mesg);
}
