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

public:
  ConfigModelSettingsData();
  ConfigModelSettingsData(const QJsonObject &json);
  ConfigModelSettingsData(const ConfigModelSettingsData &rhs);
  ~ConfigModelSettingsData() = default;

public:
  bool m_valid;

  quint32 m_oui;
  QString m_name;
  QString m_manufacturer;
  bool m_disabled;
  QByteArray m_pairingNameFormat;
  QRegExp m_scanNameMatcher;
  quint8 m_filterByte;

  bool m_hasConnParams;
  // BleConnectionParameters m_connParams;
  QString m_dbusObjectPath;
  QString m_dbusServiceName;

  // NOTE can use enums from other class
  ConfigModelSettings::ServicesType m_servicesType;
  // this comes from Q_DECLARE_FLAGS
  ConfigModelSettings::Services m_servicesSupported;
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
  enum class ServicesType
  {
    DBusServiceType,
    GattServiceType,
    RciServiceType
  };

  enum class Service
  {
    NoServices        = 0x00,
    AudioService      = 0x01,
    BatteryService    = 0x02,
    DeviceInfoService = 0x04,
    FindMeService     = 0x08,
    InfraredService   = 0x10,
    TouchService      = 0x20,
  };

  // https://doc.qt.io/qt-5/qflags.html
  Q_DECLARE_FLAGS(Services, Service)
};

#endif // CONFIGMODELSETTINGS_H
