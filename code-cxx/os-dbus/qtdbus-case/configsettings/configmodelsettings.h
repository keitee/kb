#ifndef CONFIGMODELSETTINGS_H
#define CONFIGMODELSETTINGS_H

#include <QDebug>
#include <QList>
#include <QSharedPointer>

struct ConfigModelSettingsData
{
private:
  bool stringToServiceFlag(const QString &name,
                           ConfigModelSettings::Service *service) const;
};

class ConfigModelSettings
{
private:
  QSharedPointer<ConfigModelSettingsData> m_data;

private:
  friend class ConfigSettings;
  ConfigModelSettings(const QJsonObjet &json);

public:
  // Unlike ConfigSettings, it has public ctor
  ConfigModelSettings();

  ~ConfigModelSettings() = default;

public:
  enum Service
  {
    NoServices        = 0x00,
    AudioService      = 0x01,
    BatteryService    = 0x02,
    DeviceInfoService = 0x04,
    FindMeService     = 0x08,
    InfraredService   = 0x10,
    TouchService      = 0x20,
  };
};

#endif // CONFIGMODELSETTINGS_H
