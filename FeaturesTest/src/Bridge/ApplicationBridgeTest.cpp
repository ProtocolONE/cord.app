#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Host/Application.h>
#include <Host/Thetta.h>
#include <Host/CredentialConverter.h>

#include <Host/Bridge/ApplicationBridge.h>
#include <Host/Bridge/Credential.h>

#include <RestApi/GameNetCredential.h>

#include <QtTest/QSignalSpy>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>

using ::testing::Return;

using GameNet::Host::Bridge::ApplicationBridge;
using GameNet::Host::Bridge::Credential;
using GameNet::Host::Bridge::createGameNetCredential;
using GGS::RestApi::GameNetCredential;

class ApplicationMock : public GameNet::Host::Application
{
public:

   MOCK_METHOD0(isInitCompleted, bool());
   MOCK_METHOD2(restartApplication, void(bool, bool));
   MOCK_METHOD0(switchClientVersion, void());

   MOCK_METHOD3(setCredential, void(const QString&, const QString&, const GameNetCredential&));

   // slots for signals
   MOCK_METHOD0(onInitCompleted, void());
};

class ThettaMock : public GameNet::Host::Thetta
{
public:
  MOCK_METHOD1(openBrowser, void(const QString&));
};

class ApplicationBridgeTest : public ::testing::Test 
{
public:
  ApplicationBridgeTest() 
  {
    bridge.setApplcation(&appMock);
    bridge.setThetta(&thettaMock);

    QObject::connect(&bridge, &ApplicationBridge::initCompleted,
      &appMock, &ApplicationMock::onInitCompleted);
  }

  ApplicationBridge bridge;
  ApplicationMock appMock;
  ThettaMock thettaMock;
};

TEST_F(ApplicationBridgeTest, onInitCompleted)
{
  EXPECT_CALL(appMock, onInitCompleted()).Times(1);
  appMock.initCompleted();
}

TEST_F(ApplicationBridgeTest, switchClientVersion)
{
  EXPECT_CALL(appMock, switchClientVersion())
    .Times(1);

  bridge.switchClientVersion();
}

TEST_F(ApplicationBridgeTest, restartApplication)
{
  EXPECT_CALL(appMock, restartApplication(true, true))
    .Times(1);

  bridge.restartApplication(true);
}

TEST_F(ApplicationBridgeTest, isInitCompleted) 
{
  EXPECT_CALL(appMock, isInitCompleted())
    .WillOnce(Return(true));

  ASSERT_EQ(true, appMock.isInitCompleted());
}

TEST_F(ApplicationBridgeTest, openBrowser)
{
  const QString& testUrlString = "http://test.url/string";

  EXPECT_CALL(thettaMock, openBrowser(testUrlString))
    .Times(1);

  bridge.openBrowser(testUrlString);
}

TEST_F(ApplicationBridgeTest, setCredential)
{
  QString appName("qgna");
  Credential dbusCredential;
  dbusCredential.userId = "123123123";
  dbusCredential.appKey = "sdjkfnksdjfnjkqwer";
  dbusCredential.cookie = "qkljerqlwkej13qwem1kl23jqwe";

  GameNetCredential credential = createGameNetCredential(dbusCredential);
  QString connectionName;

  EXPECT_CALL(appMock, setCredential(connectionName, appName, credential))
    .Times(1);

  bridge.setCredential(appName, dbusCredential);
}
