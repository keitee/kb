#include <QDebug>
#include <QList>
#include <QSharedPointer>

#include <configmodelsettings.h>

/*
-{-----------------------------------------------------------------------------
*/

ConfigModelSettingsData::ConfigModelSettingsData(const QJsonObjet &json)
    : m_valid(false)
    , m_diabled(false)
    , m_hasConnParams(false)
    , m_serviceSupported(0);
{
  const QJsonValue name = json["name"];
}

/*
-{-----------------------------------------------------------------------------
*/

ConfigModelSettings::ConfigModelSettings(const QJsonObjet &json)
    : m_data(QSharedPointer<ConfigModelSettingsData>::create(json))
{}
