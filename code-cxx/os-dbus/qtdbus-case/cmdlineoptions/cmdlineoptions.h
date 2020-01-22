#ifndef CMDLINEOPTIONS_H
#define CMDLINEOPTIONS_H

#include <functional>

#include <QCommandLineParser>
#include <QCoreApplication>

class CmdLineOptions
{
public:
  enum class DBusType
  {
    NoDBus,
    SessionBus,
    SystemBus,
    CustomBus
  };

private:
  using OptionHandler = std::function<void(const QString&)>;

  QCommandLineParser m_parser;
  QList<QPair<QCommandLineOption, OptionHandler>> m_options;

  // use class init for defaults
  DBusType m_busType{DBusType::SystemBus};
  QString m_serviceName{"com.sky.blercu"};
  QString m_busAddress{};

public:
  explicit CmdLineOptions();
  ~CmdLineOptions() = default;

  void process(const QCoreApplication &app);

  bool isSet(const QString &name) const;
  QString value(const QString &name) const;
  void closeConsole(const QString);

public:
  DBusType dbusType() const;
  QString dbusAddress() const;
  QString dbusService() const;

  void setDBusSession(const QString);
  void setNetworkNamespace(const QString);
  void setAudioFifoDirectory(const QString);
};

#endif // CMDLINEOPTIONS_H
