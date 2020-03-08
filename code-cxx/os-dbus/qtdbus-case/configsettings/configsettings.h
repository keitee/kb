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
  // NOTE: 
  // 1. private constructor which is used to return in staic functions so
  // used in cxx-move context.
  // 2. Have to make QSharedPointer<T> friend

  friend class QSharedPointer<ConfigSettings>;
  ConfigSettings(const TimeOuts &timeout, QList<ConfigModelSettings> &&models);

  // ConfigSettings();

public:
  ~ConfigSettings() = default;

  static QSharedPointer<ConfigSettings> defaults();
  static QSharedPointer<ConfigSettings> fromJsonFile_(const QString &path);
  static QSharedPointer<ConfigSettings> fromJsonFile_(QIODevice *file);

public:
  // NOTE: return QList and is that cheap?
  QList<ConfigModelSettings> modelSettings() const;

  // public getters to get timeout values. for example:
  int discoveryTimeout() const;
};

#endif // CONFIGSETTINGS_H
