#include <QCoreApplication>
#include <QtWidgets>
#include "worker.h"

/*

To use thread

*/

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QThread* thread = new QThread;
    Worker* worker = new Worker();
    worker->moveToThread(thread);

    QObject::connect(thread, &QThread::started, worker, &Worker::progress);

    // QObject::connect(worker, SIGNAL (finished()), thread, SLOT (quit()));
   
    QObject::connect(worker, SIGNAL (finished()), QApplication::instance(), SLOT (quit()));
    QObject::connect(worker, SIGNAL (finished()), worker, SLOT (deleteLater()));
    QObject::connect(thread, SIGNAL (finished()), thread, SLOT (deleteLater()));

    thread->start();

    return a.exec();
}
