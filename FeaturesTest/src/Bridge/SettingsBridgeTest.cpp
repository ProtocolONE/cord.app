#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Host/SettingsManager.h>
#include <Host/Bridge/SettingsBridge.h>

using P1::Host::SettingsManager;
using P1::Host::Bridge::SettingsBridge;

using ::testing::Return;

class SettingsManagerMock : public SettingsManager
{
public:
  MOCK_METHOD3(setValue, void(const QString&, const QString&, const QString&));
  MOCK_METHOD3(value, QString(const QString&, const QString&, const QString&));
};

class SettingsBridgeTest : public ::testing::Test
{
public:
  SettingsBridgeTest()
  {
    bridge.setProxy(&mock);
  }

  SettingsManagerMock mock;
  SettingsBridge bridge;
};

TEST_F(SettingsBridgeTest, settingsSetValue)
{
  QString expectedPath = "launcher/settings/ABTestGroup/";
  QString expectedKey = "LAUNCHER1681";
  QString expectedValue = "A";

  EXPECT_CALL(mock, setValue(expectedPath, expectedKey, expectedValue));
  bridge.setValue(expectedPath, expectedKey, expectedValue);
}

TEST_F(SettingsBridgeTest, settingsGetValue)
{
  QString expectedPath = "launcher/settings/ABTestGroup";
  QString expectedValue = "LAUNCHER1681";
  QString defaultVal = "error";

  EXPECT_CALL(mock, value(expectedPath, expectedValue, defaultVal)).WillOnce(Return(QString()));
  bridge.value(expectedPath, expectedValue, defaultVal);
}