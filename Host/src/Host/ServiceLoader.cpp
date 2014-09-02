#include <Host/ServiceLoader.h>
#include <Core/Service.h>

#include <Settings/Settings.h>

#include <Host/ServiceDescription.h>
#include <Host/HookFactory.h>

#include <GameDownloader/GameDownloadService.h>

#include <QtCore/QSettings>

namespace GameNet {
  namespace Host {

    ServiceLoader::ServiceLoader(QObject *parent)
      : QObject(parent)
      , _downloader(nullptr)
      , _serviceSettings(nullptr)
      , _factory(new HookFactory(this))
    {
      this->_factory->setServiceLoader(this);
    }

    ServiceLoader::~ServiceLoader()
    {
    }

    void ServiceLoader::setDownloader(GGS::GameDownloader::GameDownloadService *value)
    {
      Q_ASSERT(value);
      this->_downloader = value;
    }

    void ServiceLoader::setServiceSettings(ServiceSettings *value)
    {
      Q_ASSERT(value);
      this->_serviceSettings = value;
      this->_factory->setServiceSettings(value);
    }

    GGS::Core::Service::Area ServiceLoader::gameArea() const
    {
      return this->_gameArea;
    }

    void ServiceLoader::setGameArea(GGS::Core::Service::Area val)
    {
      this->_gameArea = val;
    }

    GGS::Core::Service::Area ServiceLoader::applicationArea() const
    {
      return this->_applicationArea;
    }

    void ServiceLoader::setApplicationArea(GGS::Core::Service::Area val)
    {
      this->_applicationArea = val;
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

    void ServiceLoader::registerService(const ServiceDescription& description)
    {
      Q_ASSERT(this->_downloader);
      GGS::Core::Service *service = new GGS::Core::Service();
      
      service->setId(description.id());
      service->setArea(this->_gameArea);
      service->setIsDownloadable(description.isDownloadable());
      service->setName(description.name());
      service->setHashDownloadPath(description.hasDownloadPath());
      service->setExtractorType(description.extractorType());
      service->setTorrentUrl(description.torrentUrl());
      
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

    void ServiceLoader::migrateInstallDate(const QString& serviceId)
    {
      // UNDONE 04.09.2014 Должна куда-то мигрировать.
      // Эта информация сейчас на пряму считывается с QML
      // Должно сохранение переехать в хост, а методы уже у бриджа спрашивать когда там инстал дейт был
      // Может быть миграция не нужна и ее можно просто удалить.

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
