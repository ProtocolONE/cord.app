#include <Host/LicenseManager.h>
#include <Host/Bridge/LicenseManagerBridge.h>

namespace P1 {
  namespace Host {
    namespace Bridge {
      LicenseManagerBridge::LicenseManagerBridge(QObject *parent)
        : QObject(parent)
        , _licenseManager(nullptr)
      {
      }

      LicenseManagerBridge::~LicenseManagerBridge()
      {
      }

      void LicenseManagerBridge::setLicenseManager(LicenseManager *value)
      {
        Q_ASSERT(value);
        this->_licenseManager = value;
      }

      bool LicenseManagerBridge::hasAcceptedLicense()
      {
        Q_ASSERT(this->_licenseManager);
        return this->_licenseManager->hasAcceptedLicense();
      }

      bool LicenseManagerBridge::hasAcceptedLicense(const QString &serviceId)
      {
        Q_ASSERT(this->_licenseManager);
        return this->_licenseManager->hasAcceptedLicense(serviceId);
      }

      void LicenseManagerBridge::acceptLicense(const QString &serviceId, const QString &hash)
      {
        Q_ASSERT(this->_licenseManager);
        this->_licenseManager->acceptLicense(serviceId, hash);
      }

      Q_NOREPLY void LicenseManagerBridge::acceptWebLicense()
      {
        Q_ASSERT(this->_licenseManager);
        this->_licenseManager->acceptWebLicense();
      }

      bool LicenseManagerBridge::hasAcceptedWebLicense()
      {
        Q_ASSERT(this->_licenseManager);
        return this->_licenseManager->hasAcceptedWebLicense();
      }

    }
  }
}
