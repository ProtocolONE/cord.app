#include <Host/ServiceLoader.h>
#include <Core/Service.h>

#include <QtCore/QSettings>

namespace GameNet {
  namespace Host {

    ServiceLoader::ServiceLoader(QObject *parent)
      : QObject(parent)
    {
    }

    ServiceLoader::~ServiceLoader()
    {
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

    void ServiceLoader::registerService(const ServiceDescription& description)
    {
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

      service->setDownloadPath(description.hasDownloadPath() ? currentDownloadPath : currentInstallPath);
      service->setInstallPath(currentInstallPath);
      service->setTorrentFilePath(description.hasDownloadPath() ? currentDownloadPath : currentInstallPath);

      this->_serviceMap[description.id()] = service;
    }

    QString ServiceLoader::getServiceInstallPath(const QString& id)
    {
      QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);
      settings.beginGroup(id);

      return settings.value("InstallPath").toString();
    }

    QString ServiceLoader::applicationPath() const
    {
      return this->_applicationPath;
    }

    void ServiceLoader::setApplicationPath(QString val)
    {
      this->_applicationPath = val;
    }

    QString ServiceLoader::getServiceDownloadPath(const QString& id)
    {
      QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);
      settings.beginGroup(id);

      return settings.value("DownloadPath").toString();
    }

    GGS::Core::Service * ServiceLoader::getService(const QString& serviceId)
    {
      if (!this->_serviceMap.contains(serviceId))
        return nullptr;

      return this->_serviceMap[serviceId];
    }

  }
}
