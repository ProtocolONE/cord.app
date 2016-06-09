#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMetaType>

#include <QtDBus/QDBusArgument>
#include <QtDBus/QDBusAbstractAdaptor>

namespace GameNet {
  namespace Host {

    class LicenseManager;

    namespace Bridge {
      class LicenseManagerBridge : public QObject
      {
        Q_OBJECT
        Q_CLASSINFO("Version", "1.0.0.0")
        Q_CLASSINFO("D-Bus Interface", "com.gamenet.dbus.LicenseManager")

      public:
        explicit LicenseManagerBridge(QObject *parent = 0);
        virtual ~LicenseManagerBridge();

        void setLicenseManager(LicenseManager * value);
      
      public slots:
        bool hasAcceptedLicense();
        bool hasAcceptedLicense(const QString &serviceId);
        Q_NOREPLY void acceptLicense(const QString &serviceId, const QString &hash);
      
      private:
        LicenseManager *_licenseManager;
      };
    }
  }
}