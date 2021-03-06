#include <Host/LicenseManager.h>
#include <Host/Bridge/LicenseManagerBridge.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using P1::Host::LicenseManager;
using P1::Host::Bridge::LicenseManagerBridge;
using ::testing::Return;

class LicenseManagerMock : public LicenseManager
{
public:
  MOCK_METHOD1(hasAcceptedLicense, bool(const QString&));
  MOCK_METHOD0(hasAcceptedLicense, bool());
  MOCK_METHOD2(acceptLicense, void(const QString&, const QString&));

  MOCK_METHOD0(acceptWebLicense, void());
  MOCK_METHOD0(hasAcceptedWebLicense, bool());
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

TEST_F(LicenseManagerBridgeTest, hasAcceptedWebLicense)
{
  EXPECT_CALL(mock, hasAcceptedWebLicense())
    .WillOnce(Return(true));

  ASSERT_TRUE(bridge.hasAcceptedWebLicense());
}

TEST_F(LicenseManagerBridgeTest, acceptWebLicense)
{
  EXPECT_CALL(mock, acceptWebLicense());

  bridge.acceptWebLicense();
}
