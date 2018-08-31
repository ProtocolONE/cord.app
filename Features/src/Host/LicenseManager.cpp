#include <Host/LicenseManager.h>

#include <QtCore/QStringList>
#include <QtCore/QSettings>

#include <Core/Marketing.h>
#include <Settings/Settings.h>

namespace P1 {
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
      if (this->hasAcceptedWebLicense())
        return true;

      Q_FOREACH(const QString &id, this->_services) {
        if (this->hasAcceptedLicense(id))
          return true;
      }

      return false;
    }

    bool LicenseManager::hasAcceptedLicense(const QString &serviceId)
    {
      QSettings settings("HKEY_LOCAL_MACHINE\\Software\\ProtocolOne\\Launcher", QSettings::NativeFormat);
      settings.beginGroup(serviceId);
      QString hash = settings.value("LicenseHash", "").toString();
 
      if (!hash.isEmpty()) {
        return true;
      }

      return false;
    }

    void LicenseManager::acceptLicense(const QString &serviceId, const QString &hash)
    {
      using P1::Core::Marketing;

      QSettings settings("HKEY_LOCAL_MACHINE\\Software\\ProtocolOne\\Launcher", QSettings::NativeFormat);
      settings.beginGroup(serviceId);
      settings.setValue("LicenseHash", hash.isEmpty() ? "1" : hash);
      settings.endGroup();

      Marketing::sendInstallerStepOnce(Marketing::InstallAcceptLicense);
    }

    void LicenseManager::acceptWebLicense()
    {
      P1::Settings::Settings settings;
      settings.beginGroup("launcher");
      settings.setValue("webLicenseAccepted", "1");
    }

    bool LicenseManager::hasAcceptedWebLicense()
    {
      P1::Settings::Settings settings;
      settings.beginGroup("launcher");
      QString hash = settings.value("webLicenseAccepted", QString()).toString();
      return !hash.isEmpty();
    }

  }
}