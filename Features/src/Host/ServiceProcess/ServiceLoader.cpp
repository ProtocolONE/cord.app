#include <Host/ServiceProcess/ServiceLoader.h>
#include <Host/ServiceProcess/ServiceDescription.h>
#include <Host/HookFactory.h>
#include <Host/ExecutorHookFactory.h>

#include <Features/WorkStationLock/WorkStationLockHook.h>

#include <Core/Service.h>

#include <Settings/Settings.h>

#include <GameDownloader/GameDownloadService.h>

#include <GameExecutor/GameExecutorService.h>
#include <GameExecutor/HookInterface.h>
#include <GameExecutor/Hook/ActivateWindow.h>

#include <QtCore/QSettings>
#include <QtCore/QCoreApplication>

using P1::Core::Service;
using P1::GameDownloader::GameDownloadService;
using P1::GameExecutor::GameExecutorService;

namespace P1 {
  namespace Host {
    namespace ServiceProcess {

      ServiceLoader::ServiceLoader(QObject *parent)
        : QObject(parent)
        , _downloader(nullptr)
        , _executor(nullptr)
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

      P1::Core::Service::Area ServiceLoader::gameArea() const
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

      P1::Core::Service* ServiceLoader::getService(const QString& serviceId)
      {
        if (!this->_serviceMap.contains(serviceId))
          return nullptr;

        return this->_serviceMap[serviceId];
      }

      QHash<QString, P1::Core::Service *>& ServiceLoader::servicesMap()
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
        service->setDisplayName(description.displayName());
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
        QSettings settings(
          QSettings::NativeFormat,
          QSettings::UserScope,
          QCoreApplication::organizationName(),
          QCoreApplication::applicationName());

        settings.beginGroup(id);

        return settings.value("InstallPath").toString();
      }

      QString ServiceLoader::getServiceDownloadPath(const QString& id)
      {
        QSettings settings(
          QSettings::NativeFormat,
          QSettings::UserScope,
          QCoreApplication::organizationName(),
          QCoreApplication::applicationName());

        settings.beginGroup(id);

        return settings.value("DownloadPath").toString();
      }

      void ServiceLoader::initDownloaderHooks(const ServiceDescription& description)
      {
        Q_ASSERT(this->_downloader);

        using P1::GameDownloader::HookBase;
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

          HookBase *updateUnistallInfo = this->_factory->create(QLatin1String("65CE6F6B-B21A-4d8b-8FD4-B5B750D556CD"));
          Q_ASSERT(updateUnistallInfo);
          this->_downloader->registerHook(description.id(), 90, 0, updateUnistallInfo);
        }
      }

      void ServiceLoader::initExecutorHooks(const Service *service, const ServiceDescription& description)
      {
        Q_ASSERT(this->_executor);

        using P1::GameExecutor::HookInterface;
        using P1::GameExecutor::Hook::ActivateWindow;

        using Features::WorkStationLock::WorkStationLockHook;

        Q_FOREACH(const ExecutorHookDescription& info, description.executorHooks()) {
          HookInterface *hook = this->_executorHookFactory->create(info.first);
          if (!hook)
            continue;

          this->_executor->addHook(*service, hook, info.second);
        }

        // INFO тут обязательные хуки !!! Внимание эти хуки должны быть зарегистрированы в тестах!
        if (description.isDownloadable()) {
          HookInterface* activateWindow = this->_executorHookFactory->create(ActivateWindow::id());
          Q_ASSERT(activateWindow);
          this->_executor->addHook(*service, activateWindow, 0);

          HookInterface* workStationLock = this->_executorHookFactory->create(WorkStationLockHook::id());
          Q_ASSERT(workStationLock);
          this->_executor->addHook(*service, workStationLock, 0);
        }
      }

      void ServiceLoader::migrateInstallDate(const QString& serviceId)
      {
        Q_ASSERT(this->_downloader);

        if (!this->_downloader->isInstalled(serviceId))
          return;

        P1::Settings::Settings settings;
        settings.beginGroup("GameDownloader");
        settings.beginGroup(serviceId);

        if (settings.contains("installDate"))
          return;

        settings.setValue("installDate", QDateTime::currentDateTime());
      }
    }
  }
}
