#include <Host/ServiceSettings.h>
#include <Host/ServiceProcess/ServiceLoader.h>

#include <Core/Service.h>
#include <Settings/Settings.h>

#include <GameDownloader/GameDownloadService.h>

#include <QtCore/QSettings>

using P1::Core::Service;
using P1::Settings::Settings;

namespace P1 {
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

    void ServiceSettings::setServices(ServiceProcess::ServiceLoader *value)
    {
      Q_ASSERT(value);
      this->_services = value;
    }

    void ServiceSettings::setDownloader(P1::GameDownloader::GameDownloadService *value)
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

      QSettings settings("HKEY_LOCAL_MACHINE\\Software\\ProtocolOne\\Launcher", QSettings::NativeFormat);
      settings.beginGroup(serviceId);

      if (service->downloadPath() == path && 
          service->downloadPath() == settings.value("DownloadPath"))
        return;

      service->setDownloadPath(path);
      service->setTorrentFilePath(path);
      this->_downloader->directoryChanged(service);

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

      QSettings settings("HKEY_LOCAL_MACHINE\\Software\\ProtocolOne\\Launcher", QSettings::NativeFormat);
      settings.beginGroup(serviceId);

      if (service->installPath() == path && 
          service->installPath() == settings.value("InstallPath"))
        return;

      service->setInstallPath(path);
      service->setIsDefaultInstallPath(false);
      this->_downloader->directoryChanged(service);

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
      QSettings settings("HKEY_LOCAL_MACHINE\\Software\\ProtocolOne\\Launcher", QSettings::NativeFormat);
      settings.beginGroup(serviceId);
      return settings.value("DownloadPath", "").toString().isEmpty();
    }

    quint64 ServiceSettings::gameSize(const QString& serviceId) const
    {
      Q_ASSERT(this->_services);
      Service* service = this->_services->getService(serviceId);
      if (!service)
        return false;

      return service->gameSize();
    }

    QString ServiceSettings::name(const QString& serviceId) const
    {
      Q_ASSERT(this->_services);
      Service* service = this->_services->getService(serviceId);
      if (!service)
        return false;

      return service->name();
    }

    bool ServiceSettings::isDownloadable(const QString& serviceId) const
    {
      Q_ASSERT(this->_services);
      Service* service = this->_services->getService(serviceId);
      if (!service)
        return false;

      return service->isDownloadable();
    }

    bool ServiceSettings::isOverlayEnabled(const QString& serviceId) const
    {
      Settings settings;
      settings.beginGroup("gameExecutor");
      settings.beginGroup("serviceInfo");
      settings.beginGroup(serviceId);
      return settings.value("overlayEnabled", 1).toInt() == 1;
    }

    void ServiceSettings::setOverlayEnabled(const QString& serviceId, bool enabled)
    {
      Settings settings;
      settings.beginGroup("gameExecutor");
      settings.beginGroup("serviceInfo");
      settings.beginGroup(serviceId);
      settings.setValue("overlayEnabled", enabled ? 1 : 0);
    }

    bool ServiceSettings::isPrefer32Bit(const QString& serviceId) const
    {
      Settings settings;
      settings.beginGroup("gameExecutor");
      settings.beginGroup("serviceInfo");
      settings.beginGroup(serviceId);
      return settings.value("Prefer32Bit", 0).toInt() == 1;
    }

    void ServiceSettings::setPrefer32Bit(const QString& serviceId, bool value)
    {
      Settings settings;
      settings.beginGroup("gameExecutor");
      settings.beginGroup("serviceInfo");
      settings.beginGroup(serviceId);
      settings.setValue("Prefer32Bit", value ? 1 : 0);
    }

  }
}