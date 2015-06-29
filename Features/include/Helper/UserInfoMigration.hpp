#include <Helper/FileUtils.h>

#include <QtCore/QStandardPaths>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QTextStream>

bool checkUserInfoMigration(bool saveFlag)
{
  QString fileFlag = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/version.info";
  if (QFile::exists(fileFlag))
    return true;

  if (!saveFlag)
    return false;

  QFile file(fileFlag);
  if (!file.open(QIODevice::ReadWrite))
    return false;

  QTextStream stream(&file);
  stream << "3.4";
  return true;

}

void migrateUserInfo()
{
  using namespace Features;

  if (checkUserInfoMigration(false))
    return;

  QString appRoot = QCoreApplication::applicationDirPath();
  FileUtils::removeRecursively(QString("%1/Cache").arg(appRoot));

  QString newTorrentsPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/torrents";
  QString oldTorrentsPath = QString("%1/torrents").arg(appRoot);
  FileUtils::recurseCopyDiretory(oldTorrentsPath, newTorrentsPath);
  FileUtils::removeRecursively(oldTorrentsPath);

  QString newMessengerPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/db/Account";
  QString oldMessengerPath = QString("%1/Account").arg(appRoot);
  FileUtils::recurseCopyDiretory(oldMessengerPath, newMessengerPath);
  FileUtils::removeRecursively(oldMessengerPath);

  QString newSettingsPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/settings";
  FileUtils::createDirectory(newSettingsPath);
  QString settingsPath = QString("%1/settings.sql").arg(appRoot);
  QString hostSettingsPath = QString("%1/host.sql").arg(appRoot);
  QFile::copy(settingsPath, QString("%1/settings.sql").arg(newSettingsPath));
  QFile::copy(hostSettingsPath, QString("%1/host.sql").arg(newSettingsPath));
  QFile::remove(settingsPath);
  QFile::remove(hostSettingsPath);

  QString newLogsPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/logs";
  FileUtils::createDirectory(newLogsPath);
  QString hostLog = QString("%1/host.log").arg(appRoot);
  QString hostLog1 = QString("%1/host.log.1").arg(appRoot);
  QFile::copy(hostLog, QString("%1/host.log").arg(newLogsPath));
  QFile::copy(hostLog1, QString("%1/host.log.1").arg(newLogsPath));

  QString qgnaLog = QString("%1/qgna.log").arg(appRoot);
  QString qgnaLog1 = QString("%1/qgna.log.1").arg(appRoot);
  QFile::copy(qgnaLog, QString("%1/qgna.log").arg(newLogsPath));
  QFile::copy(qgnaLog1, QString("%1/qgna.log.1").arg(newLogsPath));

  QFile::remove(hostLog);
  QFile::remove(hostLog1);
  QFile::remove(qgnaLog);
  QFile::remove(qgnaLog1);

  QString integrationPath = QString("%1/Integration/ZZima/ZZimaLauncher.exe").arg(QCoreApplication::applicationDirPath());
  FileUtils::removeRecursively(integrationPath);

  checkUserInfoMigration(true);
}