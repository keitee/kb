#ifndef CONFIGSETTINGS_H
#define CONFIGSETTINGS_H

#include <QDebug>
#include <QList>
#include <QSharedPointer>

class ConfigSettings
{
private:
  struct TimeOuts
  {
    int discoveryMSecs;
    int pairingMSecs;
    int setupMSecs;
    int upairingMSecs;

    int hidrawWaitPollMSecs;
    int hidrawWaitLimitMSecs;
  };

  static TimeOuts parseTimeouts_(const QJsonObject &json);

private:
  // ConfigSettings(const Timeouts &timeouts,
  //                QList<ConfigModelSettings> &&models);

  ConfigSettings();

public:
  ~ConfigSettings() = default;

  static QSharedPointer<ConfigSettings> defaults();
  static QSharedPointer<ConfigSettings> fromJsonFile_(const QString &path);
  static QSharedPointer<ConfigSettings> fromJsonFile_(QIODevice *file);
};

#endif // CONFIGSETTINGS_H
