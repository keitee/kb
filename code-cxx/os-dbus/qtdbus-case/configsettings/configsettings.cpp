#include "configsettings.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

// originally, this flle comes from Qt resource definition:
//
// daemon/resources/resources.eu-q-falconv2-it.qrc
// 3:      <file alias="defaultconfig.json">config.eu-q-falconv2-it.json</file>
//
// daemon/resources/resources.eu-q-mr412-it.qrc
// 3:      <file alias="defaultconfig.json">config.eu-q-mr412-it.json</file>
//
// daemon/resources/resources.default.qrc
// 3:      <file alias="defaultconfig.json">config.default.json</file>

QSharedPointer<ConfigSettings> ConfigSettings::defaults()
{
  // return fromJsonFile_(":defaultconfig.json");

  // fail
  // return fromJsonFile_("./resources/config.default.json");

  // TODO: take file name from CMake variable or property?
  return fromJsonFile_("/home/keitee/git/kb/code-cxx/os-dbus/qtdbus-case/"
                       "resources/config.default.json");
}

// parse a json config file and return QSharedPointer. If the json file is not
// vaild or one or more of mandatory fields are missing or malformed then null
// shared pointer is returned.

QSharedPointer<ConfigSettings>
ConfigSettings::fromJsonFile_(const QString &path)
{
  // try and open the file
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    qWarning() << "failed to open config file, " << path;
    return QSharedPointer<ConfigSettings>();
  }

  return fromJsonFile_(&file);
}

// Parses the \a json object to extract the timeout values (in milliseconds).
// The object must be formatted like the following
//
// "timeouts": {
// 	"discovery": 15000,
// 	"pair": 15000,
// 	"setup": 60000,
// 	"unpair": 20000
// },

ConfigSettings::TimeOuts ConfigSettings::parseTimeouts_(const QJsonObject &json)
{
  // default values
  TimeOuts timeout{15000, 15000, 60000, 20000, 20000, 65000};

  // this is only used here, so declare here
  struct
  {
    const char *name;
    int &storage;
  } fields[] = {
    {"discovery", timeout.discoveryMSecs},
    {"pair", timeout.pairingMSecs},
    {"setup", timeout.setupMSecs},
    {"unpair", timeout.upairingMSecs},
    {"hidrawPoll", timeout.hidrawWaitPollMSecs},
    {"hidrawLimit", timeout.hidrawWaitLimitMSecs},
  };

  for (size_t i = 0; i < (sizeof(fields) / sizeof(fields[0])); ++i)
  {
    const QJsonValue value = json[fields[i].name];

    qDebug() << fields[i].name << " = " << value;

    if (!value.isUndefined())
    {
      // bool QJsonValue::isDouble() const
      // Returns true if the value contains a double.
      // See also toDouble().
      //
      // int QJsonValue::toInt(int defaultValue = 0) const
      // Converts the value to an int and returns it.
      // If type() is not Double or the value is not a whole number, the
      // defaultValue will be returned.

      if (!value.isDouble())
        qWarning("invalid '%s' field, reverting to the default",
                 fields[i].name);
      else
        fields[i].storage = value.toInt(fields[i].storage);
    }
  }

  return timeout;
}

QSharedPointer<ConfigSettings> ConfigSettings::fromJsonFile_(QIODevice *file)
{
  // parse the json
  QJsonParseError error{};

  QJsonDocument doc = QJsonDocument::fromJson(file->readAll(), &error);
  if (doc.isNull())
  {
    qWarning() << "failed to parse config file, " << error.errorString();
    return QSharedPointer<ConfigSettings>();
  }

  qDebug() << "succeeded to parse config file";

  // the json must be an object
  if (!doc.isObject())
  {
    qWarning() << "json config file is not an object";
    return QSharedPointer<ConfigSettings>();
  }

  const QJsonObject obj = doc.object();

  // find the timeout
  QJsonValue timeoutValue = obj["timeouts"];
  if (!timeoutValue.isObject())
  {
    qWarning() << "missing or invalid 'timeout' field in config";
    return QSharedPointer<ConfigSettings>();
  }

  // QJsonObject QJsonValue::toObject() const
  // This is an overloaded function.
  // Converts the value to an object and returns it.

  // since we're in the static, parseTimeouts_() should also be static.
  Timeouts timeout = parseTimeouts_(timeoutValue.toObject());

  // find the vendor detail
  QJsonValue vendorValue = obj["models"];
  if (!vendorValue.isArray())
  {
    qWarning() << "missing or invalid 'models' field in config";
    return QSharedPointer<ConfigSettings>();
  }

  QList<ConfigModelSettings> models;

  const QJsonArray vendorArray = vendorValue.toArray();
  for (const QJsonValue &vendor : vendorArray)
  {
    if (!vendor.isObject())
      continue;

    // since ConfigSettings has friendship with
    ConfigModelSettings modelSettings(vendor.toObject());

    if (!modelSettings.isValid())
      continue;

    // ok, it's valid so append it
    models.append(modelSettings);
  }

  return QSharedPointer<ConfigSettings>();
}

// ConfigSettings::ConfigSettings(const Timeouts &timeouts,
//                    QList<ConfigModelSettings> &&models)
// {
//   qDebug() << "configsettings is constructed";
// }

ConfigSettings::ConfigSettings()
{
  qDebug() << "configsettings is constructed";
}
