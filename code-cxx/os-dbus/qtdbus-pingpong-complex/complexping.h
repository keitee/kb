#ifndef COMPLEXPING_H
#define COMPLEXPING_H

#include <QtCore/QFile>
#include <QtCore/QObject>
#include <QtDBus/QDBusInterface>

class Ping : public QObject
{
  Q_OBJECT

public slots:
  void start(const QString &);
  void quit();

public:
  QFile qstdin;
  QDBusInterface *iface;
};

#endif // COMPLEXPING_H
