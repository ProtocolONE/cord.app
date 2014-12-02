#include <Host/ServiceLoader.h>
#include <Host/ServiceDescription.h>
#include <Host/HookFactory.h>
#include <Host/ExecutorHookFactory.h>

#include <Features/Thetta/ThettaMonitor.h>

#include <Core/Service.h>

#include <Settings/Settings.h>

#include <GameDownloader/GameDownloadService.h>

#include <GameExecutor/GameExecutorService.h>
#include <GameExecutor/HookInterface.h>
#include <GameExecutor/Hook/SendPlayingInfo.h>
#include <GameExecutor/Hook/ActivateWindow.h>

#include <QtCore/QSettings>

using GGS::Core::Service;
using GGS::GameDownloader::GameDownloadService;
using GGS::GameExecutor::GameExecutorService;

namespace GameNet {
  namespace Host {

    ServiceLoader::ServiceLoader(QObject *parent)
      : QObject(parent)
      , _downloader(nullptr)
      , _executor(nullptr)
      , _secondExecutor(nullptr)
      , _simpleMainExecutor(nullptr)
      , _factory(nullptr)
      , _executorHookFactory(nullptr)
      , _gameArea(Service::Live)
    {
    }

    ServiceLoader::~ServiceLoader()
    {
    }

    void ServiceLoader::setDownloader(GameDownloadService *value)
    {
      Q_ASSERT(value);
      this->_downloader = value;
    }

    void ServiceLoader::setExecutor(GameExecutorService *value)
    {
      Q_ASSERT(value);
      this->_executor = value;
    }

    void ServiceLoader::setSimpleMainExecutor(GameExecutorService *value)
    {
      Q_ASSERT(value);
      this->_simpleMainExecutor = value;
    }

    void ServiceLoader::setSecondExecutor(GameExecutorService *value)
    {
      Q_ASSERT(value);
      this->_secondExecutor = value;
    }

    void ServiceLoader::setDownloaderHookFactory(HookFactory *value)
    {
      Q_ASSERT(value);
      this->_factory = value;
    }

    void ServiceLoader::setExecuterHookFactory(ExecutorHookFactory *value)
    {
      Q_ASSERT(value);
      this->_executorHookFactory = value;
    }

    GGS::Core::Service::Area ServiceLoader::gameArea() const
    {
      return this->_gameArea;
    }

    void ServiceLoader::setGameArea(Service::Area val)
    {
      this->_gameArea = val;
    }

    QString ServiceLoader::applicationPath() const
    {
      return this->_applicationPath;
    }

    void ServiceLoader::setApplicationPath(const QString& val)
    {
      this->_applicationPath = val;
    }

    GGS::Core::Service* ServiceLoader::getService(const QString& serviceId)
    {
      if (!this->_serviceMap.contains(serviceId))
        return nullptr;

      return this->_serviceMap[serviceId];
    }

    QHash<QString, GGS::Core::Service *>& ServiceLoader::servicesMap()
    {
      return this->_serviceMap;
    }

    void ServiceLoader::registerService(const ServiceDescription& description)
    {
      Q_ASSERT(this->_downloader);
      Q_ASSERT(this->_executor);
      Q_ASSERT(this->_factory);
      Q_ASSERT(this->_executorHookFactory);

      Service *service = new Service(this);
      
      service->setId(description.id());
      service->setGameId(description.gameId());
      service->setArea(this->_gameArea);
      service->setIsDownloadable(description.isDownloadable());
      service->setName(description.name());
      service->setHashDownloadPath(description.hasDownloadPath());
      service->setExtractorType(description.extractorType());
      service->setTorrentUrl(description.torrentUrl());
      service->setGameSize(description.gameSize());
      service->setUrlTemplate(description.executeUrl());
      service->setExternalDependencyList(description.dependencyList());
      
      QString currentInstallPath = this->getServiceInstallPath(description.id());
      if (currentInstallPath.isEmpty()) {
        service->setIsDefaultInstallPath(true);
        currentInstallPath = QString("%1/Games/%2").arg(this->_applicationPath, description.name()); 
      } else {
        service->setIsDefaultInstallPath(false);
      }
      
      QString currentDownloadPath = this->getServiceDownloadPath(description.id());
      if (currentDownloadPath.isEmpty()) 
        currentDownloadPath = QString("%1/Downloads/%2/").arg(this->_applicationPath, description.name());

      service->setDownloadPath(
        description.hasDownloadPath() 
          ? currentDownloadPath 
          : currentInstallPath);

      service->setInstallPath(currentInstallPath);

      service->setTorrentFilePath(
        description.hasDownloadPath() 
          ? currentDownloadPath 
          : currentInstallPath);
      
      this->initDownloaderHooks(description);

      if (description.isDownloadable())
        this->migrateInstallDate(description.id());

      this->initExecutorHooks(service, description);

      this->_serviceMap[description.id()] = service;
    }

    QString ServiceLoader::getServiceInstallPath(const QString& id)
    {
      QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);
      settings.beginGroup(id);

      return settings.value("InstallPath").toString();
    }

    QString ServiceLoader::getServiceDownloadPath(const QString& id)
    {
      QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);
      settings.beginGroup(id);

      return settings.value("DownloadPath").toString();
    }

    void ServiceLoader::initDownloaderHooks(const ServiceDescription& description)
    {
      Q_ASSERT(this->_downloader);

      using GGS::GameDownloader::HookBase;
      Q_FOREACH(DownloadHookDescription info, description.downloadHooks()) {
        HookBase* hook = this->_factory->create(info.first);
        if (!hook) {
          qDebug() << "Unable to create download hook for service" 
            << description.id() << "with guid" << info.first;

          continue;
        }

        this->_downloader->registerHook(description.id(), info.second.first, info.second.second, hook);
      }

      if (description.isDownloadable()) {
        HookBase *saveInstallPath = this->_factory->create(QLatin1String("D4D358CD-DFF5-4B56-AF30-349CCAE86EED"));
        Q_ASSERT(saveInstallPath);
        this->_downloader->registerHook(description.id(), 100, 0, saveInstallPath);
      }
    }

    void ServiceLoader::initExecutorHooks(const Service *service, const ServiceDescription& description)
    {
      Q_ASSERT(this->_executor);
      Q_ASSERT(this->_secondExecutor);
      Q_ASSERT(this->_simpleMainExecutor);

      using GGS::GameExecutor::HookInterface;
      using GGS::GameExecutor::Hook::SendPlayingInfo;
      using GGS::GameExecutor::Hook::ActivateWindow;
      using Features::Thetta::ThettaMonitor;

      Q_FOREACH(const ExecutorHookDescription& info, description.executorHooks()) {
        HookInterface *hook = this->_executorHookFactory->create(info.first);
        if (!hook)
          continue;

        this->_executor->addHook(*service, hook, info.second);
      }

      // INFO тут обязательные хуки !!! Внимание эти хуки должны быть зарегистрированы в тестах!
      if (description.isDownloadable()) {
        HookInterface *thetta = this->_executorHookFactory->create(ThettaMonitor::id());
        Q_ASSERT(thetta);
        this->_executor->addHook(*service, thetta, 99);

        HookInterface *playing = this->_executorHookFactory->create(SendPlayingInfo::id());
        Q_ASSERT(playing);
        this->_executor->addHook(*service, playing, 0);

        playing = this->_executorHookFactory->create(SendPlayingInfo::id());
        Q_ASSERT(playing);
        this->_secondExecutor->addHook(*service, playing, 0);

        playing = this->_executorHookFactory->create(SendPlayingInfo::id());
        Q_ASSERT(playing);
        this->_simpleMainExecutor->addHook(*service, playing, 0);

        HookInterface* activateWindow = this->_executorHookFactory->create(ActivateWindow::id());
        Q_ASSERT(activateWindow);
        this->_executor->addHook(*service, activateWindow, 0);

        activateWindow = this->_executorHookFactory->create(ActivateWindow::id());
        Q_ASSERT(activateWindow);
        this->_secondExecutor->addHook(*service, activateWindow, 0);

        activateWindow = this->_executorHookFactory->create(ActivateWindow::id());
        Q_ASSERT(activateWindow);
        this->_simpleMainExecutor->addHook(*service, activateWindow, 0);
      }
    }

    void ServiceLoader::migrateInstallDate(const QString& serviceId)
    {
      Q_ASSERT(this->_downloader);

      if (!this->_downloader->isInstalled(serviceId))
        return;

      GGS::Settings::Settings settings;
      settings.beginGroup("GameDownloader");
      settings.beginGroup(serviceId);

      if (settings.contains("installDate"))
        return;

      settings.setValue("installDate", QDateTime::currentDateTime());
    }

  }
}
