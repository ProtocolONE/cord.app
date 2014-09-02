#include <Host/ServiceSettings.h>
#include <Host/ServiceLoader.h>

#include <Core/Service.h>

#include <GameDownloader/GameDownloadService.h>

#include <QtCore/QSettings>

using GGS::Core::Service;

namespace GameNet {
  namespace Host {

    ServiceSettings::ServiceSettings(QObject *parent /*= 0*/)
      : QObject(parent)
      , _services(nullptr)
      , _downloader(nullptr)
    {
    }

    ServiceSettings::~ServiceSettings()
    {
    }

    void ServiceSettings::setServices(ServiceLoader *value)
    {
      Q_ASSERT(value);
      this->_services = value;
    }

    void ServiceSettings::setDownloader(GGS::GameDownloader::GameDownloadService *value)
    {
      Q_ASSERT(value);
      this->_downloader = value;
    }

    bool ServiceSettings::hasDownloadPath(const QString& serviceId) const
    {
      Q_ASSERT(this->_services);
      Service* service = this->_services->getService(serviceId);
      if (!service)
        return false;

      return service->hashDownloadPath();
    }

    QString ServiceSettings::downloadPath(const QString& serviceId) const
    {
      Q_ASSERT(this->_services);
      Service* service = this->_services->getService(serviceId);
      if (!service)
        return QString();

      return service->downloadPath();
    }

    void ServiceSettings::setDownloadPath(const QString& serviceId, const QString& path)
    {
      Q_ASSERT(this->_services);
      Q_ASSERT(this->_downloader);
      
      Service* service = this->_services->getService(serviceId);
      if (!service)
        return;

      service->setDownloadPath(path);
      service->setTorrentFilePath(path);
      this->_downloader->directoryChanged(service);

      QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);
      settings.beginGroup(serviceId);
      settings.setValue("DownloadPath", path);
    }

    QString ServiceSettings::installPath(const QString& serviceId) const
    {
      Q_ASSERT(this->_services);
      Service* service = this->_services->getService(serviceId);
      if (!service)
        return QString();

      return service->installPath();
    }

    void ServiceSettings::setInstallPath(const QString& serviceId, const QString& path)
    {
      Q_ASSERT(this->_services);
      Q_ASSERT(this->_downloader);
      Service* service = this->_services->getService(serviceId);
      if (!service)
        return;

      service->setInstallPath(path);
      service->setIsDefaultInstallPath(false);
      this->_downloader->directoryChanged(service);
      QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);
      settings.beginGroup(serviceId);
      settings.setValue("InstallPath", path);
    }

    bool ServiceSettings::isDefaultInstallPath(const QString& serviceId) const
    {
      Q_ASSERT(this->_services);
      Service* service = this->_services->getService(serviceId);
      if (!service)
        return false;

      return service->isDefaultInstallPath();
    }

    bool ServiceSettings::isDefaultDownloadPath(const QString& serviceId) const
    {
      QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);
      settings.beginGroup(serviceId);
      return settings.value("DownloadPath", "").toString().isEmpty();
    }

    int ServiceSettings::gameSize(const QString& serviceId) const
    {
      Q_ASSERT(this->_services);
      Service* service = this->_services->getService(serviceId);
      if (!service)
        return false;

      // UNDONE 03.09.2014 Необходимо вернуть размер игры вероятно вместе с архивами
      // размер игр есть в описании сервиса, но нет в классе сервиса. 
      // Либо добавить в сервис этот паараметр, либо сохранять описания сервисов создаваемых
      // в лоадере и такие мало обязательные параметры брать оттуда.
      if (serviceId == "300002010000000000")
        return 2800;

      if (serviceId == "300003010000000000")
        return 2400;

      if (serviceId == "300005010000000000")
        return 2500;

      if (serviceId == "300009010000000000")
        return 4800;

      if (serviceId == "300012010000000000")
        return 3300;

      return 1000;
    }

    QString ServiceSettings::name(const QString& serviceId) const
    {
      Q_ASSERT(this->_services);
      Service* service = this->_services->getService(serviceId);
      if (!service)
        return false;

      return service->name();
    }

  }
}