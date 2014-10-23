#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Host/Application.h>
#include <Host/Thetta.h>
#include <Host/Translation.h>

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

   // slots for signals
   MOCK_METHOD0(onInitCompleted, void());
   MOCK_METHOD0(onRestartUIRequest, void());
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

class ApplicationBridgeTest : public ::testing::Test 
{
public:
  ApplicationBridgeTest() 
  {
    bridge.setApplication(&appMock);
    bridge.setThetta(&thettaMock);
    bridge.setTranslation(&translationMock);

    QObject::connect(&bridge, &ApplicationBridge::initCompleted,
      &appMock, &ApplicationMock::onInitCompleted);
    
    QObject::connect(&bridge, &ApplicationBridge::restartUIRequest,
      &appMock, &ApplicationMock::onRestartUIRequest);

    QObject::connect(&bridge, &ApplicationBridge::languageChanged,
      &translationMock, &TranslationMock::onLanguageChanged);
  }

  ApplicationBridge bridge;
  ApplicationMock appMock;
  ThettaMock thettaMock;
  TranslationMock translationMock;
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
