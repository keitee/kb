#include <gtest/gtest.h>

#include "configsettings.h"

TEST(ConfigSettingsTest, defaultConfig)
{
	QSharedPointer<ConfigSettings> config = ConfigSettings::defaults();
	ASSERT_FALSE(config.isNull());
}
