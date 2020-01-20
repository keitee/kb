#ifndef CONFIGSETTINGS_H
#define CONFIGSETTINGS_H

#include <QDebug>
#include <QList>
#include <QSharedPointer>

#include <configmodelsettings.h>

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
  const TimeOuts m_timeout;
  const QList<ConfigModelSettings> m_modelDetails;

private:
  // NOTE: private constructor which is used to return in staic functions so
  // used in cxx-move context.

  friend class QSharedPointer<ConfigSettings>;
  ConfigSettings(const TimeOuts &timeout, QList<ConfigModelSettings> &&models);

  // ConfigSettings();

public:
  ~ConfigSettings() = default;

  static QSharedPointer<ConfigSettings> defaults();
  static QSharedPointer<ConfigSettings> fromJsonFile_(const QString &path);
  static QSharedPointer<ConfigSettings> fromJsonFile_(QIODevice *file);
};

#endif // CONFIGSETTINGS_H
