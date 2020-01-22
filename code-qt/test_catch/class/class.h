#ifndef CLASS_H
#define CLASS_H

#include <QObject>
#include <mutex>
#include <condition_variable>

class ThreadFoo : public QObject
{
  Q_OBJECT

public:
  ThreadFoo(std::mutex &m, std::condition_variable &cv)
      : m_(m)
      , cv_(cv)
  {}

public slots:

  void progress();

private:
  std::mutex &m_;
  std::condition_variable &cv_;
};

#endif // CLASS_H
