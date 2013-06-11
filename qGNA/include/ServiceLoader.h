#pragma once

#include <Core/Service>

#include <GameDownloader/GameDownloadService.h>
#include <GameDownloader/Hooks/InstallDependency.h>

#include <GameExecutor/Hook/ExternalDependency.h>
#include <GameExecutor/GameExecutorService.h>

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QString>

#include <Windows.h>

class ServiceLoader : public QObject
{
  Q_OBJECT
public:
  ServiceLoader(QObject *parent = 0);
  ~ServiceLoader();

  void init(GGS::Core::Service::Area gameArea = GGS::Core::Service::Live);
  void setExecutor(GGS::GameExecutor::GameExecutorService *executor);
  void setGameDownloader(GGS::GameDownloader::GameDownloadService *gameDownloader);

  void setExecuteUrl(const QString& id, QString currentInstallPath);

  bool hasDefaultDownloadPath(const QString& id);
  bool hasDefaultInstallPath(const QString& id);

  GGS::Core::Service* getService(const QString& id);
  QHash<QString, GGS::Core::Service *>& serviceMap();

  QString getExpectedInstallPath(const QString& serviceId);
  bool anyLicenseAccepted();

  QString applicationVersion() const;
  void setApplicationVersion(const QString& value);

private:
  QHash<QString, GGS::Core::Service *> _serviceMap;
  GGS::GameExecutor::GameExecutorService *_gameExecutorService;
  GGS::GameDownloader::GameDownloadService *_gameDownloader;

  GGS::GameDownloader::Hooks::InstallDependency _installDependencyHook;
  GGS::Core::Service::Area _gameArea;
  QString _applicationVersion;

  void initService(const QString& id, const QString& torrentUrl, const QString& name);
  void initHooks(const QString& id, GGS::Core::Service* service);

  void initGAService();
  void initFJService();
  void migrateInstallDate(const QString& serviceId);

  QString getBestDrive(const QString& serviceId);
  int getDiskFreeSpaceInMb(LPCWSTR drive);
  bool hasEnoughSpace(const QString& serviceId, int free);

signals:
  void startGameRequest(QString);
};

