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

// #include <gmock/gmock.h>
// #include <gtest/gtest.h>

#include "qclass.h"

using namespace std;

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  // TEST(QtThread, ThreadAffinity_3)
  {
    qDebug() << "main: " << app.thread();

    QThread *thread = new QThread;

    // comes from qclass.h
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
