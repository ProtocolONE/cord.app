#include <Host/GameExecutor.h>

#include <Host/Bridge/ExecutorBridge.h>

#include <Host/Proxy/GameExecutorProxy.h>

#include <RestApi/ProtocolOneCredential.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <QtCore/QString>
#include <QtCore/QDateTime>

using P1::Host::GameExecutor;
using P1::Host::Proxy::GameExecutorProxy;
using P1::Host::Bridge::ExecutorBridge;

using P1::RestApi::ProtocolOneCredential;

using ::testing::Return;

class GameExecutorMock : public GameExecutorProxy
{
public:
  using GameExecutorProxy::execute;
  
// slots
  MOCK_METHOD2(
    execute, 
    void(const QString& serviceId, const ProtocolOneCredential& credential));


  MOCK_CONST_METHOD1(isGameStarted, bool(const QString&));
  MOCK_CONST_METHOD0(isAnyGameStarted, bool());
  
  MOCK_METHOD1(terminateGame, void(const QString&));

// slots for signals
  MOCK_CONST_METHOD1(onServiceStarted, void(const QString&));
  MOCK_CONST_METHOD2(onServiceFinished, void(const QString&, int));
};

class ExecutorBridgeTest : public ::testing::Test 
{
public:
  ExecutorBridgeTest() 
  {
    QDateTime now = QDateTime::currentDateTime();
    now.addSecs(30);
    serviceId = "123456";

    accessToken = "eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiJ9.eyJpYXQiOjE1MzkxNTM0MTEsImV4cCI6MTUzOTE1NTI"
      "xMSwicm9sZXMiOlsiUk9MRV9VU0VSIl0sImlkIjoiNWJiZDlkZTMzNDg5ZDI5YzgzMGZkYjBkIn0.SKo"
      "EfLk7b4GshpXWXobtQeF00XVjWh1vQDe41hWVtcHGf_qSEG3WqcRFsgRdWsJ1vBTBSHjmoSupxSdNyHY"
      "6lP04wlnaFwr7w5ofBVeNPaP78bX6qnXvmBpfrvRzLX2IQBJLw89CAqfiCDJgTZzJ9Gu9K8Vwygjwdtj"
      "qEhq3ugEbaBslO_dHQ-6rXwk5CnjsSTsTleyTERCIathB_Zl4L0UUXHgjXZ3gE_4CdTmuCnHjFOjdGmL"
      "td1v80ZXWyuQifbwL32QPV4FniRXCrI7zEEXcBJAlGwF0gPYeOht3G2wgpYw4rD0dADYEX1J5gtAnKuj"
      "xdULdvR1S6Xnq0Rt9rKAkC0OC2Gz9RjWfY92Kw-NPDeFlXHBEaIm0Q6XOvX9ctRSb5S7K4_FbPFTlIBQ"
      "nlzipWt7pXUH6ObSJw0y_ZsmBfvbL0nCgqyZzKCAYOo2T88gjhbdl7hivICjcVNWD6R1cU5llwEC85t2"
      "AyqLpZPDdgpuevwDjXOwCDJLylrYxTK5oL_bi-2fKLzxRqNNhAYQ3Hs4CpAMML1PqmcxKXApN8U7d1jx"
      "gteL8l0XIEK47ZjoyD2kXL9fHCK0HmDeY5GWv1yRpVPExLxATcQiHyTZJQ51vdnWxn1zx9bVU8ZUCuJg"
      "GwbZYB1oVSXNWH7t5y_2Bh6_dfmxeg3ITUWZ0kCI";
    acccessTokenExpiredTime = QString::number(now.toMSecsSinceEpoch() / 1000);

    expectedCredential = ProtocolOneCredential(accessToken, acccessTokenExpiredTime);

    bridge.setExecutor(&executor);
    
    QObject::connect(&bridge, &ExecutorBridge::serviceStarted,
      &executor, &GameExecutorMock::onServiceStarted);

    QObject::connect(&bridge, &ExecutorBridge::serviceFinished,
      &executor, &GameExecutorMock::onServiceFinished);

  }

  QString serviceId;

  QString accessToken;
  QString acccessTokenExpiredTime;
  ProtocolOneCredential expectedCredential;

  ExecutorBridge bridge;

  GameExecutorMock executor;
};

TEST_F(ExecutorBridgeTest, execute)
{
  EXPECT_CALL(executor, execute(serviceId, expectedCredential))
    .Times(1);

  bridge.execute(serviceId, accessToken, acccessTokenExpiredTime);
}

TEST_F(ExecutorBridgeTest, isGameStarted)
{
  EXPECT_CALL(executor, isGameStarted(serviceId))
    .WillOnce(Return(true))
    .WillOnce(Return(false));

  ASSERT_TRUE(bridge.isGameStarted(serviceId));
  ASSERT_FALSE(bridge.isGameStarted(serviceId));
}

TEST_F(ExecutorBridgeTest, isAnyGameStarted)
{
  EXPECT_CALL(executor, isAnyGameStarted())
    .WillOnce(Return(true))
    .WillOnce(Return(false));

  ASSERT_TRUE(bridge.isAnyGameStarted());
  ASSERT_FALSE(bridge.isAnyGameStarted());
}

TEST_F(ExecutorBridgeTest, serviceStarted)
{
  EXPECT_CALL(executor, onServiceStarted(serviceId)).Times(1);
  executor.serviceStarted(serviceId);
}

TEST_F(ExecutorBridgeTest, serviceFinished)
{
  EXPECT_CALL(executor, onServiceFinished(serviceId, 42)).Times(1);
  executor.serviceFinished(serviceId, 42);
}

TEST_F(ExecutorBridgeTest, terminateGame)
{
  EXPECT_CALL(executor, terminateGame(serviceId)).Times(1);
  bridge.terminateGame(serviceId);
}

