#include <Host/GameExecutor.h>
#include <Host/CredentialConverter.h>
#include <Host/Bridge/ExecutorBridge.h>

#include <Host/Proxy/GameExecutorProxy.h>

#include <RestApi/GameNetCredential.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <QtCore/QString>

using GameNet::Host::GameExecutor;
using GameNet::Host::Proxy::GameExecutorProxy;
using GameNet::Host::Bridge::ExecutorBridge;
using GameNet::Host::Bridge::Credential;
using GameNet::Host::Bridge::createGameNetCredential;
using GGS::RestApi::GameNetCredential;

using ::testing::Return;

class GameExecutorMock : public GameExecutorProxy
{
public:
  using GameExecutorProxy::execute;
  using GameExecutorProxy::executeSecond;
  
// slots
  MOCK_METHOD2(
    execute, 
    void(const QString& serviceId, const GameNetCredential& credential));

  MOCK_METHOD3(
    executeSecond, 
    void(const QString& serviceId, 
         const GameNetCredential& credential, 
         const GameNetCredential& secondCredential));

  MOCK_CONST_METHOD1(isGameStarted, bool(const QString&));
  MOCK_CONST_METHOD0(isAnyGameStarted, bool());
  MOCK_CONST_METHOD1(canExecuteSecond, bool(const QString&));
  MOCK_METHOD0(shutdownSecond, void());
  
// slots for signals
  MOCK_CONST_METHOD1(onServiceStarted, void(const QString&));
  MOCK_CONST_METHOD2(onServiceFinished, void(const QString&, int));
  MOCK_CONST_METHOD1(onSecondServiceStarted, void(const QString&));
  MOCK_CONST_METHOD2(onSecondServiceFinished, void(const QString&, int));
};

class ExecutorBridgeTest : public ::testing::Test 
{
public:
  ExecutorBridgeTest() 
  {
    serviceId = "123456";
    credential.userId = "qwe123eqwe123";
    credential.appKey = "asdfgdfjkghlk";
    credential.cookie = "zxcvbmvbxmcvb";

    secondCredential.userId = "secondUserId";
    secondCredential.appKey = "secondAppKey";
    secondCredential.cookie = "secondcookie";

    expectedCredential = createGameNetCredential(credential);
    expectedSecondCredential = createGameNetCredential(secondCredential);

    bridge.setExecutor(&executor);
    
    QObject::connect(&bridge, &ExecutorBridge::serviceStarted,
      &executor, &GameExecutorMock::onServiceStarted);

    QObject::connect(&bridge, &ExecutorBridge::serviceFinished,
      &executor, &GameExecutorMock::onServiceFinished);

    QObject::connect(&bridge, &ExecutorBridge::secondServiceStarted,
      &executor, &GameExecutorMock::onSecondServiceStarted);

    QObject::connect(&bridge, &ExecutorBridge::secondServiceFinished,
      &executor, &GameExecutorMock::onSecondServiceFinished);
  }

  QString serviceId;

  Credential credential;
  GameNetCredential expectedCredential;

  Credential secondCredential;
  GameNetCredential expectedSecondCredential;

  ExecutorBridge bridge;
  GameExecutorMock executor;
};

TEST_F(ExecutorBridgeTest, execute)
{
  EXPECT_CALL(executor, execute(serviceId, expectedCredential))
    .Times(1);

  bridge.execute(serviceId, credential);
}

TEST_F(ExecutorBridgeTest, executeSecond)
{
  EXPECT_CALL(executor, executeSecond(serviceId, expectedCredential, expectedSecondCredential))
    .Times(1);

  bridge.executeSecond(serviceId, credential, secondCredential);
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

TEST_F(ExecutorBridgeTest, canExecuteSecond)
{
  EXPECT_CALL(executor, canExecuteSecond(serviceId))
    .WillOnce(Return(true))
    .WillOnce(Return(false));

  ASSERT_TRUE(bridge.canExecuteSecond(serviceId));
  ASSERT_FALSE(bridge.canExecuteSecond(serviceId));
}

TEST_F(ExecutorBridgeTest, shutdownSecond)
{
  EXPECT_CALL(executor, shutdownSecond()).Times(1);
  bridge.shutdownSecond();
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

TEST_F(ExecutorBridgeTest, secondServiceStarted)
{
  EXPECT_CALL(executor, onSecondServiceStarted(serviceId)).Times(1);
  executor.secondServiceStarted(serviceId);
}

TEST_F(ExecutorBridgeTest, secondServiceFinished)
{
  EXPECT_CALL(executor, onSecondServiceFinished(serviceId, 42)).Times(1);
  executor.secondServiceFinished(serviceId, 42);
}
