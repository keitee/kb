#include <exception>
#include <iostream>
#include <memory>
#include <sstream>

#include <QDebug>
#include <QList>
#include <QLoggingCategory>
#include <QString>
// #include <QApplication>
#include <QQueue>
#include <QSignalSpy>
#include <QTime>
#include <QTimer>

#include "qclass.h"

using namespace std;

#if 0
int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  // TEST(QtThread, ThreadAffinity_1)
  {
    // https://wiki.qt.io/QThreads_general_usage

    QThread *thread = new QThread;
    ThreadStandAlone *wo = new ThreadStandAlone();
    wo->moveToThread(thread);

    QObject::connect(thread, &QThread::started, wo,
                     &ThreadStandAlone::progress);

    thread->start();

    // WHY do not work when uses in standalone setting?
    // cv.wait(lock);
  }

  return app.exec();
}

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  // TEST(QtThread, ThreadAffinity_2)
  {
    Counter a;

    QThread *thread       = new QThread;
    ThreadStandAloneX *wo = new ThreadStandAloneX(a);
    wo->moveToThread(thread);

    // start thread
    QObject::connect(thread, &QThread::started, wo,
                     &ThreadStandAloneX::progress);

    thread->start();
  }

  return app.exec();
}

#endif

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  // TEST(QtThread, ThreadAffinity_3)
  {
    qDebug() << "main: " << app.thread();

    QThread *thread       = new QThread;
    ThreadStandAloneY *wo = new ThreadStandAloneY();
    wo->moveToThread(thread);

    // start thread
    QObject::connect(thread,
                     &QThread::started,
                     wo,
                     &ThreadStandAloneY::progress);

    thread->start();
  }

  return app.exec();
}
