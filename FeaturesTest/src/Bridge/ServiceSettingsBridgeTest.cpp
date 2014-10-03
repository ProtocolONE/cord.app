#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Host/ServiceSettings.h>
#include <Host/Bridge/ServiceSettingsBridge.h>

using GameNet::Host::ServiceSettings;
using GameNet::Host::Bridge::ServiceSettingsBridge;

using ::testing::Return;

class ServiceSettingsMock : public ServiceSettings
{
public:
  MOCK_CONST_METHOD1(isDownloadable, bool(const QString&));
  MOCK_CONST_METHOD1(isDefaultInstallPath, bool(const QString&));
  MOCK_CONST_METHOD1(isDefaultDownloadPath, bool(const QString&));
  MOCK_CONST_METHOD1(hasDownloadPath, bool(const QString&));

  MOCK_CONST_METHOD1(downloadPath, QString(const QString&));
  MOCK_METHOD2(setDownloadPath, void(const QString&, const QString&));

  MOCK_CONST_METHOD1(installPath, QString(const QString&));
  MOCK_METHOD2(setInstallPath, void(const QString&, const QString&));

  MOCK_CONST_METHOD1(gameSize, quint64(const QString&));
  MOCK_CONST_METHOD1(name, QString(const QString&));

  MOCK_CONST_METHOD1(isOverlayEnabled, bool(const QString&));
  MOCK_METHOD2(setOverlayEnabled, void(const QString&, bool));
};

class ServiceSettingsBridgeTest : public ::testing::Test 
{
public:
  ServiceSettingsBridgeTest() 
  {
    serviceId = "123456";
    bridge.setSettings(&settings);
  }

  ServiceSettingsBridge bridge;
  ServiceSettingsMock settings;
  QString serviceId;
};

TEST_F(ServiceSettingsBridgeTest, isDownloadable)
{
  EXPECT_CALL(settings, isDownloadable(serviceId))
    .WillOnce(Return(true))
    .WillOnce(Return(false));

  ASSERT_TRUE(bridge.isDownloadable(serviceId));
  ASSERT_FALSE(bridge.isDownloadable(serviceId));
}

TEST_F(ServiceSettingsBridgeTest, isDefaultInstallPath)
{
  EXPECT_CALL(settings, isDefaultInstallPath(serviceId))
    .WillOnce(Return(true))
    .WillOnce(Return(false));

  ASSERT_TRUE(bridge.isDefaultInstallPath(serviceId));
  ASSERT_FALSE(bridge.isDefaultInstallPath(serviceId));
}

TEST_F(ServiceSettingsBridgeTest, isDefaultDownloadPath)
{
  EXPECT_CALL(settings, isDefaultDownloadPath(serviceId))
    .WillOnce(Return(true))
    .WillOnce(Return(false));

  ASSERT_TRUE(bridge.isDefaultDownloadPath(serviceId));
  ASSERT_FALSE(bridge.isDefaultDownloadPath(serviceId));
}

TEST_F(ServiceSettingsBridgeTest, hasDownloadPath)
{
  EXPECT_CALL(settings, hasDownloadPath(serviceId))
    .WillOnce(Return(true))
    .WillOnce(Return(false));

  ASSERT_TRUE(bridge.hasDownloadPath(serviceId));
  ASSERT_FALSE(bridge.hasDownloadPath(serviceId));
}

TEST_F(ServiceSettingsBridgeTest, downloadPath)
{
  QString path1 = "C:/tmp";
  QString path2 = "D:/Games/Bs";

  EXPECT_CALL(settings, setDownloadPath(serviceId, path1)).Times(1);
  EXPECT_CALL(settings, setDownloadPath(serviceId, path2)).Times(1);

  EXPECT_CALL(settings, downloadPath(serviceId))
    .WillOnce(Return(path1))
    .WillOnce(Return(path2));

  bridge.setDownloadPath(serviceId, path1);
  ASSERT_EQ(path1, bridge.downloadPath(serviceId));

  bridge.setDownloadPath(serviceId, path2);
  ASSERT_EQ(path2, bridge.downloadPath(serviceId));
}

TEST_F(ServiceSettingsBridgeTest, installPath)
{
  QString path1 = "C:/tmp";
  QString path2 = "D:/Games/Bs";

  EXPECT_CALL(settings, setInstallPath(serviceId, path1)).Times(1);
  EXPECT_CALL(settings, setInstallPath(serviceId, path2)).Times(1);

  EXPECT_CALL(settings, installPath(serviceId))
    .WillOnce(Return(path1))
    .WillOnce(Return(path2));

  bridge.setInstallPath(serviceId, path1);
  ASSERT_EQ(path1, bridge.installPath(serviceId));

  bridge.setInstallPath(serviceId, path2);
  ASSERT_EQ(path2, bridge.installPath(serviceId));
}

TEST_F(ServiceSettingsBridgeTest, gameSize)
{
  EXPECT_CALL(settings, gameSize(serviceId))
    .WillOnce(Return(42))
    .WillOnce(Return(956));

  ASSERT_EQ(42, bridge.gameSize(serviceId));
  ASSERT_EQ(956, bridge.gameSize(serviceId));
}

TEST_F(ServiceSettingsBridgeTest, name)
{
  QString name1("Legacy of Kain: Soul Reaver");
  QString name2("Combat Arms");

  EXPECT_CALL(settings, name(serviceId))
    .WillOnce(Return(name1))
    .WillOnce(Return(name2));

  ASSERT_EQ(name1, bridge.name(serviceId));
  ASSERT_EQ(name2, bridge.name(serviceId));
}

TEST_F(ServiceSettingsBridgeTest, isOverlayEnabled)
{
  EXPECT_CALL(settings, isOverlayEnabled(serviceId))
    .WillOnce(Return(true))
    .WillOnce(Return(false));

  ASSERT_TRUE(bridge.isOverlayEnabled(serviceId));
  ASSERT_FALSE(bridge.isOverlayEnabled(serviceId));
}

TEST_F(ServiceSettingsBridgeTest, setOverlayEnabled)
{
  bool value = true;
  bool value2 = false;

  EXPECT_CALL(settings, setOverlayEnabled(serviceId, value)).Times(1);
  EXPECT_CALL(settings, setOverlayEnabled(serviceId, value2)).Times(1);

  EXPECT_CALL(settings, isOverlayEnabled(serviceId))
    .WillOnce(Return(value))
    .WillOnce(Return(value2));

  bridge.setOverlayEnabled(serviceId, value);
  ASSERT_EQ(value, bridge.isOverlayEnabled(serviceId));

  bridge.setOverlayEnabled(serviceId, value2);
  ASSERT_EQ(value2, bridge.isOverlayEnabled(serviceId));
}