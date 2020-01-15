#include <QtCore/QCoreApplication>
#include <QtDBus/QtDBus>

class Client : public QObject
{
  Q_OBJECT
public:
  Client();

public slots:
  void onSignalReceived(const QString &name, const QString &text);

private:
  QSharedPointer<QDBusInterface> m_sender;
};
