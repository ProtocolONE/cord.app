#include <Host/SettingsManager.h> 
#include <Settings/Settings.h>

namespace GameNet {
  namespace Host {

    SettingsManager::SettingsManager(QObject * parent /*= 0*/) : QObject(parent)
    {

    }

    SettingsManager::~SettingsManager()
    {

    }

    void SettingsManager::setValue(const QString& path, const QString& key, const QString& data)
    {
      GGS::Settings::Settings settings;
      settings.beginGroup(path);
      settings.setValue(key, data);
    }

    QString SettingsManager::value(const QString& path, const QString& key, const QString& defValue)
    {
      GGS::Settings::Settings settings;
      settings.beginGroup(path);
      QString res = settings.value(key, QString()).toString();
      
      if (res.isEmpty())
        res = defValue;
      
      return res;
    }

  }
}
