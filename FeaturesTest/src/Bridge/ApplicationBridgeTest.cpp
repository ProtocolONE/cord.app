#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Host/Application.h>
#include <Host/Bridge/ApplicationBridge.h>
#include <Host/Thetta.h>

#include <QtTest/QSignalSpy>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>

using ::testing::Return;

using GameNet::Host::Bridge::ApplicationBridge;

class ApplicationMock : public GameNet::Host::Application
{
public:

   MOCK_METHOD0(isInitCompleted, bool());
   MOCK_METHOD2(restartApplication, void(bool, bool));
   MOCK_METHOD0(switchClientVersion, void());

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
