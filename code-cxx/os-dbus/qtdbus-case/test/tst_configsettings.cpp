#include <QSharedPointer>
#include <configsettings.h>
#include <gtest/gtest.h>

TEST(ConfigSettingsTest, defaultConfig)
{
	QSharedPointer<ConfigSettings> config = ConfigSettings::defaults();
	ASSERT_FALSE(config.isNull());
}
