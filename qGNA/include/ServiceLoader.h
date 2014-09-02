/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates. 
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/
#pragma once

#include <Thetta/Driver.h>
#include <Thetta/Logger.h>

#include <Core/Service>

#include <GameDownloader/GameDownloadService.h>
#include <GameDownloader/Hooks/InstallDependency.h>

#include <GameExecutor/Hook/ExternalDependency.h>
#include <GameExecutor/GameExecutorService.h>

#include <Features/Thetta/ThettaInstaller.h>
#include <Features/Thetta/ThettaMonitorApp.h>
#include <Features/Thetta/DistrIntegrity.h>

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QString>

#include <Windows.h>

namespace Thetta {
	class Driver;
}

class ServiceSettingsBridgeProxy;

class ServiceLoader : public QObject
{
  Q_OBJECT
public:
  explicit ServiceLoader(QObject *parent = 0);
  ~ServiceLoader();

  void init(GGS::Core::Service::Area gameArea = GGS::Core::Service::Live,
    GGS::Core::Service::Area applicationArea = GGS::Core::Service::Live);

  void setExecutor(GGS::GameExecutor::GameExecutorService *executor);
  void setGameDownloader(GGS::GameDownloader::GameDownloadService *gameDownloader);

  void setExecuteUrl(const QString& id, QString currentInstallPath);

  void setServiceSettings(ServiceSettingsBridgeProxy *value);

  bool hasDefaultDownloadPath(const QString& id);
  bool hasDefaultInstallPath(const QString& id);

  GGS::Core::Service* getService(const QString& id);
  QHash<QString, GGS::Core::Service *>& serviceMap();

  QString getExpectedInstallPath(const QString& serviceId);
  bool anyLicenseAccepted();

  QString applicationVersion() const;
  void setApplicationVersion(const QString& value);

  Thetta::Driver* getDriver();
  Features::Thetta::ThettaInstaller* thettaInstaller();

  void initGameExecutorExtensions(GGS::GameExecutor::GameExecutorService* executor);

private:
  ServiceSettingsBridgeProxy *_serviceSettings;
  QHash<QString, GGS::Core::Service *> _serviceMap;
  GGS::GameExecutor::GameExecutorService *_gameExecutorService;
  GGS::GameDownloader::GameDownloadService *_gameDownloader;

  GGS::GameDownloader::Hooks::InstallDependency _installDependencyHook;
  GGS::Core::Service::Area _gameArea;
  GGS::Core::Service::Area _applicationArea;
  QString _applicationVersion;

  Thetta::Driver* _driver;
  Features::Thetta::ThettaInstaller *_installer;
  Features::Thetta::ThettaMonitorApp _thettaMonitorApp;
  Features::Thetta::DistrIntegrity _caDistIntegrity;

  void initService(const QString& id, const QString& torrentUrl, const QString& name);
  void initHooks(const QString& id, GGS::Core::Service* service);

  void installThettaHook(GGS::Core::Service* service);

  void initGAService();
  void initFJService();
  void initBDService();

  QString getBestDrive(const QString& serviceId);
  int getDiskFreeSpaceInMb(LPCWSTR drive);
  bool hasEnoughSpace(const QString& serviceId, int free);

  QString getDriverToken(const QString& salt, const QString& token);
  bool processHandlerExtension(DWORD pid, HANDLE handle);
  
};

