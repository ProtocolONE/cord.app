#include <Host/LicenseManager.h>

#include <QtCore/QStringList>
#include <QtCore/QSettings>

#include <Core/Marketing.h>

namespace GameNet {
  namespace Host {
    LicenseManager::LicenseManager(QObject *parent)
      : QObject(parent)
    {
    }

    LicenseManager::~LicenseManager()
    {
    }

    void LicenseManager::setServices(const QStringList &value)
    {
      this->_services = value;
    }

    bool LicenseManager::hasAcceptedLicense()
    {
      Q_FOREACH(const QString &id, this->_services) {
        if (this->hasAcceptedLicense(id))
          return true;
      }

      return false;
    }

    bool LicenseManager::hasAcceptedLicense(const QString &serviceId)
    {
      QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);
      settings.beginGroup(serviceId);
      QString hash = settings.value("LicenseHash", "").toString();
 
      if (!hash.isEmpty()) {
        return true;
      }

      return false;
    }

    void LicenseManager::acceptLicense(const QString &serviceId, const QString &hash)
    {
      using GGS::Core::Marketing;

      QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);
      settings.beginGroup(serviceId);
      settings.setValue("LicenseHash", hash.isEmpty() ? "1" : hash);
      settings.endGroup();

      Marketing::sendInstallerStepOnce(Marketing::InstallAcceptLicense);
    }
  }
}