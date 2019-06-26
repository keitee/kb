#include <QDebug>

class Worker : public QObject
{
  Q_OBJECT

  public slots:
    void progress();

  signals:
    void finished();
    void error(QString err);
};
