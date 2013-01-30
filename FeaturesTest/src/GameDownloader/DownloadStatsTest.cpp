#include <GameDownloader/DownloadStatsPub.h>

#include <Settings/Settings>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QDebug>

#include <gtest/gtest.h>
#include <Windows.h>

class DownloadStats : public ::testing::Test 
{
public:
  void SetUp()
  {
    initDatabase();
  }

  void initDatabase() 
  {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QSqlQuery query;
    QString dbSettingsPath = QString("%1/settings.sql").arg(QCoreApplication::applicationDirPath());
    if (QFile::exists(dbSettingsPath)) 
      QFile::remove(dbSettingsPath);

    db.setDatabaseName(dbSettingsPath);
    bool needToSetDefaultSettings = false;

    if (db.open("admin", "admin")) {

      if (!db.tables().contains("app_settings")) {
        query = db.exec("CREATE TABLE app_settings "
          "( "
          "	key_column text NOT NULL, "
          "	value_column text, "
          "	CONSTRAINT app_settings_pk PRIMARY KEY (key_column) "
          ")");
        needToSetDefaultSettings = true;

        if (query.lastError().isValid())
          qDebug() << query.lastError().text();
      } 
    }

    GGS::Settings::Settings::setConnection(db.connectionName());
  }
};

TEST_F(DownloadStats, SaveLoadTest)
{
  DownloadStatsPub downloadStats3;
  downloadStats3.setId("someFakeId");
  downloadStats3.load();
  ASSERT_FALSE(downloadStats3._initialized);

  DownloadStatsPub downloadStats;
  downloadStats.setId("someFakeId");
  downloadStats._initialized = true;
  downloadStats._downloadTime = 1235;
  downloadStats._applicationRestartCount = 3;
  downloadStats._maxDownloadSpeed = 1000;
  downloadStats._maxUploadSpeed = 9874;
  downloadStats._totalSize = 909090;
  downloadStats.save();

  DownloadStatsPub downloadStats2;
  downloadStats2.setId("someFakeId");
  downloadStats2.load();

  ASSERT_TRUE(downloadStats2._initialized);
  ASSERT_EQ(1235, downloadStats2._downloadTime);
  ASSERT_EQ(3, downloadStats2._applicationRestartCount);
  ASSERT_EQ(1000, downloadStats2._maxDownloadSpeed);
  ASSERT_EQ(9874, downloadStats2._maxUploadSpeed);
  ASSERT_EQ(909090, downloadStats2._totalSize);
}

TEST_F(DownloadStats, StartFinish)
{
  DownloadStatsPub downloadStats;
  downloadStats.setId("someFakeId");
  downloadStats.start();
  downloadStats.update(1000, 2000, 1000);
  Sleep(1000);
  downloadStats.stop();

  ASSERT_EQ(1000, downloadStats.maxDownloadSpeed());
  ASSERT_EQ(2000, downloadStats.maxUploadSpeed());
  ASSERT_EQ(1, downloadStats.applicationRestartCount());

  int downloadTime = downloadStats.downloadTime();
  ASSERT_LE(1, downloadTime);
  ASSERT_GE(5, downloadTime);
}

TEST_F(DownloadStats, StartStopStartFinish)
{
  DownloadStatsPub downloadStats;
  downloadStats.setId("someFakeId");
  downloadStats.start();
  downloadStats.update(1000, 2000, 1000);
  Sleep(1000);
  downloadStats.stop();
  downloadStats.start();
  Sleep(1000);
  downloadStats.stop();

  ASSERT_EQ(1000, downloadStats.maxDownloadSpeed());
  ASSERT_EQ(2000, downloadStats.maxUploadSpeed());
  ASSERT_EQ(1, downloadStats.applicationRestartCount());

  int downloadTime = downloadStats.downloadTime();
  ASSERT_LE(2, downloadTime);
  ASSERT_GE(10, downloadTime);
}
