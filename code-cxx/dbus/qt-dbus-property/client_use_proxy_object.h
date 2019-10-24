#include <QtCore/QCoreApplication>
#include <QtDBus/QtDBus>

class ClientUseProxy : public QObject
{
  Q_OBJECT
  public:
    ClientUseProxy();

  public slots:
    void onSignalReceived(const QString &name, const QString &text);
};
