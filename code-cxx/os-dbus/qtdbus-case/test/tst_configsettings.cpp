#include <QSharedPointer>
#include <configsettings.h>
#include <gmock/gmock.h>

using namespace std;
using namespace testing;

TEST(ConfigSettingsTest, defaultConfig)
{
  QSharedPointer<ConfigSettings> config = ConfigSettings::defaults();
  EXPECT_THAT(config.isNull(), false);

  // use public interface
  EXPECT_THAT(config->discoveryTimeout(), 15000);

  // directly use the list of model setting
  auto settings = config->modelSettings();

  for (auto const &model : settings)
  {
    qDebug() << "name        : " << qPrintable(model.name());
    qDebug() << "manufacturer: " << qPrintable(model.manufacturer());
    qDebug() << "oui         : " << model.oui();
  }
}
