#include <Host/ApplicationStatistic.h>

#include <Settings/Settings.h>

#include <Application/ArgumentParser.h>

#include <GameDownloader/GameDownloadService.h>

#include <QtCore/QDateTime>
#include <QtCore/QStringList>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using GameNet::Host::ApplicationStatistic;
using GGS::Settings::Settings;
using GGS::GameDownloader::GameDownloadService;
using GGS::Application::ArgumentParser;
using ::testing::Return;

class ApplicationStatisticTestGameDownloadServiceMock : public GameDownloadService
{
public:
  MOCK_METHOD1(isInstalled, bool(const QString&));
};

class ApplicationStatisticTest : public ::testing::Test
{
public:
  ApplicationStatisticTest()
  {
    serviceId = "ApplicationStatisticTestServiceId";
    statistic.setDownloader(&downloader);
  }

  ApplicationStatistic statistic;
  ApplicationStatisticTestGameDownloadServiceMock downloader;
  QString serviceId;
};

TEST_F(ApplicationStatisticTest, isGameInstalled)
{
  EXPECT_CALL(downloader, isInstalled(serviceId))
    .WillOnce(Return(true));

  ASSERT_TRUE(statistic.isGameInstalled(serviceId));
}

TEST_F(ApplicationStatisticTest, gameInstallDate)
{
  QDateTime now = QDateTime::currentDateTime();
  Settings settings;
  settings.beginGroup("GameDownloader");
  settings.beginGroup(serviceId);
  settings.setValue("installDate", now);

  ASSERT_EQ(now.toMSecsSinceEpoch(), statistic.gameInstallDate(serviceId));
}

TEST_F(ApplicationStatisticTest, gameLastExecutionTime)
{
  QDateTime now = QDateTime::currentDateTime();
  Settings settings;
  settings.beginGroup("gameExecutor");
  settings.beginGroup("serviceInfo");
  settings.beginGroup(serviceId);
  settings.setValue("lastExecutionTime", now);

  ASSERT_EQ(now.toMSecsSinceEpoch(), statistic.gameLastExecutionTime(serviceId));
}

TEST_F(ApplicationStatisticTest, executeGameSuccessCount)
{
  int expectedCount = 412;
  Settings settings;
  settings.beginGroup("gameExecutor");
  settings.beginGroup("serviceInfo");
  settings.beginGroup(serviceId);
  settings.setValue("successCount", expectedCount);

  ASSERT_EQ(expectedCount, statistic.executeGameSuccessCount(serviceId));
}

TEST_F(ApplicationStatisticTest, executeGameFailedCount)
{
  int expectedCount = 413;
  Settings settings;
  settings.beginGroup("gameExecutor");
  settings.beginGroup("serviceInfo");
  settings.beginGroup(serviceId);
  settings.setValue("failedCount", expectedCount);

  ASSERT_EQ(expectedCount, statistic.executeGameFailedCount(serviceId));
}

TEST_F(ApplicationStatisticTest, executeGameTotalCount)
{
  int failCount = 417;
  int successCount = 412;
  Settings settings;
  settings.beginGroup("gameExecutor");
  settings.beginGroup("serviceInfo");
  settings.beginGroup(serviceId);
  settings.setValue("failedCount", failCount);
  settings.setValue("successCount", successCount);

  ASSERT_EQ(failCount + successCount, statistic.executeGameTotalCount(serviceId));
}

TEST_F(ApplicationStatisticTest, installWithService)
{
  Settings settings;
  settings.beginGroup("qGNA");
  settings.setValue("installWithService", serviceId);
  ASSERT_EQ(serviceId, statistic.installWithService());
}

TEST_F(ApplicationStatisticTest, installDate)
{
  quint64 installDate = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000;

  Settings settings;
  settings.beginGroup("qGNA");
  settings.setValue("installDate", installDate);
  ASSERT_EQ(installDate, statistic.installDate());
}

TEST_F(ApplicationStatisticTest, init)
{
  QString expectedServiceId = "ApplicationStatisticTestOtherServiceId";

  ArgumentParser parser;
  statistic.setCommandLineArgs(&parser);

  Settings settings;
  settings.beginGroup("qGNA");
  settings.setValue("installDate", 0);
  settings.setValue("installWithService", expectedServiceId);

  statistic.init();

  ASSERT_NE(0, statistic.installDate());
  ASSERT_EQ(expectedServiceId, statistic.installWithService());
}

TEST_F(ApplicationStatisticTest, initWithStartingGame)
{
  QString expectedServiceId = "300012010000000000";

  ArgumentParser parser;
  QStringList args;
  args << "C:/test.exe" << "/uri:gamenet://startservice/300012010000000000";
  parser.parse(args);

  statistic.setCommandLineArgs(&parser);

  Settings settings;
  settings.beginGroup("qGNA");
  settings.setValue("installDate", 0);
  settings.setValue("installWithService", "");

  statistic.init();

  ASSERT_NE(0, statistic.installDate());
  ASSERT_EQ(expectedServiceId, statistic.installWithService());
}

TEST_F(ApplicationStatisticTest, secondInit)
{
  quint64 expectedInstallDate = 123123123;
  Settings settings;
  settings.beginGroup("qGNA");
  settings.setValue("installDate", expectedInstallDate);

  statistic.init();
  ASSERT_EQ(expectedInstallDate, statistic.installDate());
}

