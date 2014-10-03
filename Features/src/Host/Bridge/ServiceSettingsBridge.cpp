#include <Host/Bridge/ServiceSettingsBridge.h>
#include <Host/ServiceSettings.h>

namespace GameNet {
  namespace Host {
    namespace Bridge {

      ServiceSettingsBridge::ServiceSettingsBridge(QObject *parent /*= 0*/)
        : QObject(parent)
        , _settings(nullptr)
      {
      }

      ServiceSettingsBridge::~ServiceSettingsBridge()
      {
      }

      void ServiceSettingsBridge::setSettings(ServiceSettings *value)
      {
        Q_ASSERT(value);
        this->_settings = value;
      }

      bool ServiceSettingsBridge::isDefaultInstallPath(const QString& serviceId) const
      {
        Q_ASSERT(this->_settings);
        return this->_settings->isDefaultInstallPath(serviceId);
      }

      bool ServiceSettingsBridge::isDefaultDownloadPath(const QString& serviceId) const
      {
        Q_ASSERT(this->_settings);
        return this->_settings->isDefaultDownloadPath(serviceId);
      }

      bool ServiceSettingsBridge::hasDownloadPath(const QString& serviceId) const
      {
        Q_ASSERT(this->_settings);
        return this->_settings->hasDownloadPath(serviceId);
      }

      QString ServiceSettingsBridge::downloadPath(const QString& serviceId) const
      {
        Q_ASSERT(this->_settings);
        return this->_settings->downloadPath(serviceId);
      }

      void ServiceSettingsBridge::setDownloadPath(const QString& serviceId, const QString& path)
      {
        Q_ASSERT(this->_settings);
        this->_settings->setDownloadPath(serviceId, path);
      }

      QString ServiceSettingsBridge::installPath(const QString& serviceId) const
      {
        Q_ASSERT(this->_settings);
        return this->_settings->installPath(serviceId);
      }

      void ServiceSettingsBridge::setInstallPath(const QString& serviceId, const QString& path)
      {
        Q_ASSERT(this->_settings);
        this->_settings->setInstallPath(serviceId, path);
      }

      quint64 ServiceSettingsBridge::gameSize(const QString& serviceId) const
      {
        Q_ASSERT(this->_settings);
        return this->_settings->gameSize(serviceId);
      }

      QString ServiceSettingsBridge::name(const QString& serviceId) const
      {
        Q_ASSERT(this->_settings);
        return this->_settings->name(serviceId);
      }

      bool ServiceSettingsBridge::isDownloadable(const QString& serviceId) const
      {
        Q_ASSERT(this->_settings);
        return this->_settings->isDownloadable(serviceId);
      }

      bool ServiceSettingsBridge::isOverlayEnabled(const QString& serviceId) const
      {
        Q_ASSERT(this->_settings);
        return this->_settings->isOverlayEnabled(serviceId);
      }

      void ServiceSettingsBridge::setOverlayEnabled(const QString& serviceId, bool enabled)
      {
        Q_ASSERT(this->_settings);
        this->_settings->setOverlayEnabled(serviceId, enabled);
      }

    }
  }
}