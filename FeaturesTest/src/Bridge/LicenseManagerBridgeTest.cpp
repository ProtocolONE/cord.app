#include <Host/LicenseManager.h>
#include <Host/Bridge/LicenseManagerBridge.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using GameNet::Host::LicenseManager;
using GameNet::Host::Bridge::LicenseManagerBridge;
using ::testing::Return;

class LicenseManagerMock : public LicenseManager
{
public:
  MOCK_METHOD1(hasAcceptedLicense, bool(const QString&));
  MOCK_METHOD0(hasAcceptedLicense, bool());
  MOCK_METHOD2(acceptLicense, void(const QString&, const QString&));
};

class LicenseManagerBridgeTest: public ::testing::Test 
{
public:
  LicenseManagerBridgeTest()
  {
    bridge.setLicenseManager(&mock);
  }

  LicenseManagerMock mock;
  LicenseManagerBridge bridge;
};

TEST_F(LicenseManagerBridgeTest, hasAcceptedLicense)
{
  EXPECT_CALL(mock, hasAcceptedLicense())
    .WillOnce(Return(true));

  ASSERT_TRUE(bridge.hasAcceptedLicense());
}

TEST_F(LicenseManagerBridgeTest, hasAcceptedLicenseWithArg)
{
  QString expectedValue = "12";
  EXPECT_CALL(mock, hasAcceptedLicense(expectedValue))
    .WillOnce(Return(true));

  ASSERT_TRUE(bridge.hasAcceptedLicense(expectedValue));
}

TEST_F(LicenseManagerBridgeTest, acceptLicense)
{
  QString expectedValue = "12";
  QString expectedHash = "hash";

  EXPECT_CALL(mock, acceptLicense(expectedValue, expectedHash));
    
  bridge.acceptLicense(expectedValue, expectedHash);
}