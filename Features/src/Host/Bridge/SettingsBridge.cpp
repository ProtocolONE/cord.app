#include <Host/Bridge/SettingsBridge.h>
#include <Host/SettingsManager.h>

#include <Settings/Settings.h>

namespace P1 {
  namespace Host {
    namespace Bridge {

      SettingsBridge::SettingsBridge(QObject *parent /*= 0*/) : QObject(parent), _settingsManager(nullptr)
      {

      }

      SettingsBridge::~SettingsBridge()
      {

      }

      void SettingsBridge::setProxy(Host::SettingsManager *proxy)
      {
        this->_settingsManager = proxy;
      }

      void SettingsBridge::setValue(const QString& path, const QString& key, const QString& data)
      {
        this->_settingsManager->setValue(path, key, data);
      }

      QString SettingsBridge::value(const QString& path, const QString& key, const QString& defValue)
      {
        return this->_settingsManager->value(path, key, defValue);
      }

    }
  }
}
