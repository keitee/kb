#include <QDebug>
#include <QJsonObject>
#include <QList>
#include <QSharedPointer>

#if defined(Q_XXXX)
// NOTE: what is it for?
// #if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
#include <QDBusObjectPath>
#endif

#include <configmodelsettings.h>

/*
-{-----------------------------------------------------------------------------
*/

ConfigModelSettingsData::ConfigModelSettingsData()
    : m_valid(false)
    , m_disabled(false)
    , m_filterByte(0xff)
    , m_servicesType(ConfigModelSettings::ServicesType::DBusServiceType)
    , m_servicesSupported(0)
{}

ConfigModelSettingsData::ConfigModelSettingsData(
  const ConfigModelSettingsData &other)
    : m_valid(other.m_valid)
    , m_oui(other.m_oui)
    , m_name(other.m_name)
    , m_manufacturer(other.m_manufacturer)
    , m_disabled(other.m_disabled)
    , m_pairingNameFormat(other.m_pairingNameFormat)
    , m_filterByte(other.m_filterByte)
    , m_hasConnParams(other.m_hasConnParams)
    // , m_connParams(other.m_connParams)
    , m_servicesType(other.m_servicesType)
    , m_servicesSupported(other.m_servicesSupported)
{}

/*
	Constructs some vendor settings from the supplied json object, if the json
	object has errors then an invalid object is created.

	The object should be formatted like the following:

	"models": [
		{
			"name": "EC05x",
			"manufacturer": "Ruwido",
			"oui": "1C:A2:B1",
			"pairingNameFormat": "U%03hhu*",
			"connectionParams": {
				"maxInterval": 15.0,
				"minInterval": 15.0,
				"latency": 332,
				"supervisionTimeout": 15000
			},
			"services": {
				"type": "dbus",
				"dbusServiceName": "com.ruwido.rcu",
				"dbusObjectPath": "/com/ruwido/rcu",
				"supported": [
					"audio",
					"battery",
					"deviceInfo",
					"findMe",
					"infrared",
					"touch"
				]
			}
		},
    ...
    ]
*/

ConfigModelSettingsData::ConfigModelSettingsData(const QJsonObject &json)
    : m_valid(false)
    , m_disabled(false)
    , m_hasConnParams(false)
    , m_servicesSupported(0)
{
  // name field
  {
    const QJsonValue name = json["name"];
    if (!name.isString())
      qWarning("invalid or missing 'name' field");
    else
      m_name = name.toString();
  }

  qDebug() << "---------------------";
  qDebug() << "config.name:         " << m_name;

  // manufacturer field
  {
    const QJsonValue manufacturer = json["manufacturer"];
    if (!manufacturer.isString())
      qWarning("invalid or missing 'manufacturer' field");
    else
      m_manufacturer = manufacturer.toString();
  }

  qDebug() << "config.manufacturer: " << m_manufacturer;

  // oui field
  {
    const QJsonValue oui = json["oui"];
    if (!oui.isString())
    {
      qWarning("invalid or missing 'oui' field");
      return;
    }

    quint8 bytes[3];
    if (sscanf(oui.toString().toLatin1().constData(),
               "%02hhx:%02hhx:%02hhx",
               &bytes[0],
               &bytes[1],
               &bytes[2]) != 3)
    {
      qWarning("invalid or missing 'oui' field");
      return;
    }

    m_oui = (quint32(bytes[0]) << 16) | (quint32(bytes[1]) << 8) |
            (quint32(bytes[2]));
  }

  qDebug() << "config.oui:        " << m_oui;

  // pairingFormat field
  // NOTE: this will contiue processing even if see error and which means
  // `optional` field
  {
    const QJsonValue pairingFormat = json["pairingNameFormat"];
    if (!pairingFormat.isString())
    {
      qWarning("invalid or missing 'pairingNameFormat' field");
      return;
    }

    m_pairingNameFormat = pairingFormat.toString().toLatin1();
  }

  qDebug() << "config.pairingNameFormat: " << m_pairingNameFormat;

  // service field which is nested one
  {
    const QJsonValue service = json["services"];
    if (!service.isObject())
    {
      qWarning("invalid or missing 'service' field");
      return;
    }

    const QJsonObject serviceObject = service.toObject();

    // service.type field
    {
      const QJsonValue type = serviceObject["type"];
      if (!type.isString())
      {
        qWarning("invalid or missing 'service.type' field");
        return;
      }

      qDebug() << "config.service.type: " << type.toString();

      const QString typeString = type.toString();

      if (0 == typeString.compare("dbus", Qt::CaseInsensitive))
      {
        // NOTE: if not use enum class
        // m_serviceType = ConfigModelSettings::DBusServiceType;
        m_servicesType = ConfigModelSettings::ServicesType::DBusServiceType;
      }
      else if (0 == typeString.compare("gatt", Qt::CaseInsensitive))
        m_servicesType = ConfigModelSettings::ServicesType::GattServiceType;
      else
      {
        qWarning("invalid or missing 'service.type' field");
        return;
      }

#if defined(Q_XXXX)
      // #if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
      // service.dbus* field. have to use serviceObject.
      {
        if (ConfigModelSettings::ServicesType::DBusServiceType ==
            m_servicesType)
        {
          const QJsonValue serviceName = serviceObject["dbusServiceName"];
          const QJsonValue objectPath = serviceObject["dbusObjectPath"];

          if (!serviceName.isString() || !objectPath.isString())
          {
            qWarning("invalid 'dbusServiceName' or 'dbusObjectPath' field");
            return;
          }

          QDBusObjectPath objectPathCheck(objectPath.toString());
          if (objectPathCheck.path().isEmpty())
          {
            qWarning("invalid 'dbusObjectPath' field");
            return;
          }

          m_dbusObjectPath = objectPathCheck.path();
          m_dbusServiceName = serviceName.toString();

          qDebug() << "config.service.dbusServiceName: " << m_dbusServiceName;
          qDebug() << "config.service.dbusObjectPath : " << m_dbusObjectPath;
        }
      }
#endif
    }
  }

  m_valid = true;
}

/*
-{-----------------------------------------------------------------------------
*/

ConfigModelSettings::ConfigModelSettings(const QJsonObject &json)
    : m_data(QSharedPointer<ConfigModelSettingsData>::create(json))
{}

bool ConfigModelSettings::isValid() const
{
  return m_data && m_data->m_valid;
}

QString ConfigModelSettings::manufacturer() const
{
  return m_data->m_manufacturer;
}

QString ConfigModelSettings::name() const
{
  return m_data->m_name;
}

quint32 ConfigModelSettings::oui() const
{
  return m_data->m_oui;
}
