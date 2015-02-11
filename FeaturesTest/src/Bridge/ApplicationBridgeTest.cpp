#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Host/Application.h>
#include <Host/Thetta.h>
#include <Host/Translation.h>
#include <Host/AutoRunManager.h>

#include <Host/Bridge/ApplicationBridge.h>

#include <RestApi/GameNetCredential.h>

#include <Settings/Settings.h>

using ::testing::Return;

using GameNet::Host::Bridge::ApplicationBridge;
using GGS::RestApi::GameNetCredential;

class ApplicationMock : public GameNet::Host::Application
{
public:
   MOCK_METHOD0(isInitCompleted, bool());
   MOCK_METHOD2(restartApplication, void(bool, bool));
   MOCK_METHOD0(switchClientVersion, void());
   MOCK_METHOD0(shutdownUIResult, void());
   MOCK_METHOD1(cancelUninstallServiceRequest, void(const QString &));
   
   // slots for signals
   MOCK_METHOD0(onInitCompleted, void());
   MOCK_METHOD0(onRestartUIRequest, void());
   MOCK_METHOD0(onShutdownUIRequest, void());
   MOCK_METHOD1(onUninstallServiceRequest, void(const QString&));
};

class ThettaMock : public GameNet::Host::Thetta
{
public:
  MOCK_METHOD1(openBrowser, void(const QString&));
};

class TranslationMock : public GameNet::Host::Translation
{
public:
  MOCK_CONST_METHOD0(language, QString());
  MOCK_METHOD1(setLanguage, void(const QString&));

  // slots for signals
  MOCK_METHOD0(onLanguageChanged, void());
};

class AutoRunManagerMock : public GameNet::Host::AutoRunManager
{
public:
  MOCK_CONST_METHOD0(autoStartMode, int());
  MOCK_METHOD1(setAutoStartMode, void(int));

  // slots for signals
  MOCK_METHOD0(onAutoStartModeChanged, void());
};

class ApplicationBridgeTest : public ::testing::Test 
{
public:
  ApplicationBridgeTest() 
  {
    serviceId = "1234567890";

    bridge.setApplication(&appMock);
    bridge.setThetta(&thettaMock);
    bridge.setTranslation(&translationMock);
    bridge.setAutoRunManager(&autoRunManager);

    QObject::connect(&bridge, &ApplicationBridge::initCompleted,
      &appMock, &ApplicationMock::onInitCompleted);
    
    QObject::connect(&bridge, &ApplicationBridge::restartUIRequest,
      &appMock, &ApplicationMock::onRestartUIRequest);

    QObject::connect(&bridge, &ApplicationBridge::shutdownUIRequest,
      &appMock, &ApplicationMock::onShutdownUIRequest);

     QObject::connect(&bridge, &ApplicationBridge::uninstallServiceRequest,
      &appMock, &ApplicationMock::onUninstallServiceRequest);
     
     QObject::connect(&bridge, &ApplicationBridge::languageChanged,
      &translationMock, &TranslationMock::onLanguageChanged);

    QObject::connect(&bridge, &ApplicationBridge::autoStartModeChanged,
      &autoRunManager, &AutoRunManagerMock::onAutoStartModeChanged);
  }

  QString serviceId;
  ApplicationBridge bridge;
  ApplicationMock appMock;
  ThettaMock thettaMock;
  TranslationMock translationMock;
  AutoRunManagerMock autoRunManager;
};

TEST_F(ApplicationBridgeTest, onInitCompleted)
{
  EXPECT_CALL(appMock, onInitCompleted()).Times(1);
  appMock.initCompleted();
}

TEST_F(ApplicationBridgeTest, restartUIRequest)
{
  EXPECT_CALL(appMock, onRestartUIRequest()).Times(1);
  appMock.restartUIRequest();
}

TEST_F(ApplicationBridgeTest, shutdownUIRequest)
{
  EXPECT_CALL(appMock, onShutdownUIRequest()).Times(1);
  appMock.shutdownUIRequest();
}

TEST_F(ApplicationBridgeTest, uninstallServiceRequest)
{
  EXPECT_CALL(appMock, onUninstallServiceRequest(serviceId)).Times(1);
  appMock.uninstallServiceRequest(serviceId);
}

TEST_F(ApplicationBridgeTest, switchClientVersion)
{
  EXPECT_CALL(appMock, switchClientVersion())
    .Times(1);

  bridge.switchClientVersion();
}

TEST_F(ApplicationBridgeTest, restartApplication)
{
  EXPECT_CALL(appMock, restartApplication(false, true))
    .Times(1);

  bridge.restartApplication(true);
}

TEST_F(ApplicationBridgeTest, shutdownUIResult)
{
  EXPECT_CALL(appMock, shutdownUIResult())
    .Times(1);

  bridge.shutdownUIResult();
}

TEST_F(ApplicationBridgeTest, uninstallService)
{
  EXPECT_CALL(appMock, onUninstallServiceRequest(serviceId)).Times(1);
  bridge.uninstallServiceRequest(serviceId);
}

TEST_F(ApplicationBridgeTest, cancelServiceUninstall)
{
  EXPECT_CALL(appMock, cancelUninstallServiceRequest(serviceId)).Times(1);
  bridge.cancelUninstallServiceRequest(serviceId);
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

TEST_F(ApplicationBridgeTest, language)
{
  QString expectedLanguage("fr");

  GGS::Settings::Settings settings;
  settings.setValue("qGNA/language", expectedLanguage);

  EXPECT_CALL(translationMock, language())
    .WillOnce(Return(expectedLanguage));

  ASSERT_EQ(expectedLanguage, bridge.language());
}

TEST_F(ApplicationBridgeTest, setLanguage)
{
  QString expectedLanguage("fr");
  EXPECT_CALL(translationMock, setLanguage(expectedLanguage))
    .Times(1);

  bridge.setLanguage(expectedLanguage);
}

TEST_F(ApplicationBridgeTest, languageChanged)
{
  EXPECT_CALL(translationMock, onLanguageChanged()).Times(1);
  translationMock.languageChanged();
}

TEST_F(ApplicationBridgeTest, autoStartMode)
{
  int expectedMode = 2;

  EXPECT_CALL(autoRunManager, autoStartMode())
    .WillOnce(Return(expectedMode));

  ASSERT_EQ(expectedMode, bridge.autoStartMode());
}

TEST_F(ApplicationBridgeTest, setAutoStartMode)
{
  int expectedMode = 2;
  EXPECT_CALL(autoRunManager, setAutoStartMode(expectedMode))
    .Times(1);

  bridge.setAutoStartMode(expectedMode);
}

TEST_F(ApplicationBridgeTest, autoStartModeChanged)
{
  EXPECT_CALL(autoRunManager, onAutoStartModeChanged()).Times(1);
  autoRunManager.autoStartModeChanged();
}
