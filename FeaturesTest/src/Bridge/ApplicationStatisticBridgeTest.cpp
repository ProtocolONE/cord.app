#include <Host/ApplicationStatistic.h>
#include <Host/Bridge/ApplicationStatisticBridge.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using P1::Host::ApplicationStatistic;
using P1::Host::Bridge::ApplicationStatisticBridge;
using ::testing::Return;

class ApplicationStatisticMock : public ApplicationStatistic
{
public:
  MOCK_CONST_METHOD1(isGameInstalled, bool(const QString&));

  MOCK_CONST_METHOD1(gameInstallDate, quint64(const QString&));
  MOCK_CONST_METHOD1(gameLastExecutionTime, quint64(const QString&));
  
  MOCK_CONST_METHOD1(executeGameSuccessCount, int(const QString&));
  MOCK_CONST_METHOD1(executeGameFailedCount, int(const QString&));
  MOCK_CONST_METHOD1(executeGameTotalCount, int(const QString&));

  MOCK_CONST_METHOD0(installWithService, QString());
  MOCK_CONST_METHOD0(installDate, quint64());
  
  MOCK_CONST_METHOD0(isFirstStart, bool());
};

class ApplicationStatisticBridgeTest : public ::testing::Test 
{
public:
  ApplicationStatisticBridgeTest()
  {
    serviceId = "123456";
    bridge.setApplicationStatistic(&statistic);
  }

  QString serviceId;
  ApplicationStatisticMock statistic;
  ApplicationStatisticBridge bridge;
};

TEST_F(ApplicationStatisticBridgeTest, isGameInstalled)
{
  EXPECT_CALL(statistic, isGameInstalled(serviceId))
    .WillOnce(Return(true));

  ASSERT_TRUE(bridge.isGameInstalled(serviceId));
}

TEST_F(ApplicationStatisticBridgeTest, gameInstallDate)
{
  quint64 expectedValue = 1234127;
  EXPECT_CALL(statistic, gameInstallDate(serviceId))
    .WillOnce(Return(expectedValue));

  ASSERT_EQ(expectedValue, bridge.gameInstallDate(serviceId));
}

TEST_F(ApplicationStatisticBridgeTest, gameLastExecutionTime)
{
  quint64 expectedValue = 1234127;
  EXPECT_CALL(statistic, gameLastExecutionTime(serviceId))
    .WillOnce(Return(expectedValue));

  ASSERT_EQ(expectedValue, bridge.gameLastExecutionTime(serviceId));
}

TEST_F(ApplicationStatisticBridgeTest, executeGameSuccessCount)
{
  int expectedValue = 42;
  EXPECT_CALL(statistic, executeGameSuccessCount(serviceId))
    .WillOnce(Return(expectedValue));

  ASSERT_EQ(expectedValue, bridge.executeGameSuccessCount(serviceId));
}

TEST_F(ApplicationStatisticBridgeTest, executeGameFailedCount)
{
  int expectedValue = 42;
  EXPECT_CALL(statistic, executeGameFailedCount(serviceId))
    .WillOnce(Return(expectedValue));

  ASSERT_EQ(expectedValue, bridge.executeGameFailedCount(serviceId));
}

TEST_F(ApplicationStatisticBridgeTest, executeGameTotalCount)
{
  int expectedValue = 42;

  EXPECT_CALL(statistic, executeGameTotalCount(serviceId))
    .WillOnce(Return(expectedValue));

  ASSERT_EQ(expectedValue, bridge.executeGameTotalCount(serviceId));
}

TEST_F(ApplicationStatisticBridgeTest, installWithService)
{
  QString expectedValue = "42";
  EXPECT_CALL(statistic, installWithService())
    .WillOnce(Return(expectedValue));

  ASSERT_EQ(expectedValue, bridge.installWithService());
}

TEST_F(ApplicationStatisticBridgeTest, installDate)
{
  quint64 expectedValue = 1412075196;
  EXPECT_CALL(statistic, installDate())
    .WillOnce(Return(expectedValue));

  ASSERT_EQ(expectedValue, bridge.installDate());
}

TEST_F(ApplicationStatisticBridgeTest, isFirstStart)
{
  bool expectedValue = true;
  EXPECT_CALL(statistic, isFirstStart())
    .WillOnce(Return(expectedValue));

  ASSERT_EQ(expectedValue, bridge.isFirstStart());
}