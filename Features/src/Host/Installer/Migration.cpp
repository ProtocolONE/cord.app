#include <Host/Installer/Migration.h>
#include <Host/Installer/UninstallInfo.h>

#include <Host/ServiceProcess/ServiceLoader.h>

#include <Core/Service.h>
#include <GameDownloader/GameDownloadService.h>
#include <Settings/Settings.h>

namespace GameNet {
  namespace Host {
    namespace ServiceProcess {
      class ServiceLoader;
    }

    namespace Installer { 

      Migration::Migration()
        : _services(nullptr)
        , _downloader(nullptr)
      {
        this->_oldKeys << "A56B3C4B-0765-4B60-B6D4-0DD245DFEE4D_is1"; 	// Aika
        this->_oldKeys << "2BA01940-9A12-40CD-A9AD-F3E68C5E2918_is1"; 	// BS
        this->_oldKeys << "A8C0F819-A08C-4F1A-95D1-B8ADAA6417B2_is1"; // Combat ARms
        this->_oldKeys << "5F78AEE5-E587-40B8-B500-519CBB79033D_is1"; // reborn
        this->_oldKeys << "85F4D963-CB2B-4BDC-B860-B5F40D7F64F7_is1"; // Warinc
      }

      Migration::~Migration()
      {

      }

      void Migration::setServices(ServiceProcess::ServiceLoader *value)
      {
        Q_ASSERT(value);
        this->_services = value;
      }

      void Migration::setDownloader(GGS::GameDownloader::GameDownloadService *value)
      {
        Q_ASSERT(value);
        this->_downloader = value;
      }

      bool Migration::isMigrated()
      {
        GGS::Settings::Settings settings;
        return settings.value("qGNA/Installer/Migrated", 0).toBool();
      }

      void Migration::migrate()
      {
        Q_ASSERT(this->_services);
        Q_ASSERT(this->_downloader);

        this->removeOldUninstallKeys();

        QHash<QString, GGS::Core::Service *> servicesMap = this->_services->servicesMap();
        QHash<QString, GGS::Core::Service *>::const_iterator it = servicesMap.constBegin();
        while (it != servicesMap.constEnd()) {
          GGS::Core::Service *service = it.value();
          QString serviceId = service->id();
          
          if (!service->isDownloadable()) {
            ++it;
            continue;
          }

          if (!this->_downloader->isInstalled(serviceId)) {
            ++it;
            continue;
          }

          UninstallInfo info(serviceId);
          info.setDisplayName(service->displayName());
          info.save();

          ++it;
        }

        GGS::Settings::Settings settings;
        settings.setValue("qGNA/Installer/Migrated", 1);
      }

      void Migration::removeOldUninstallKeys()
      {
        QSettings settings("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall", QSettings::NativeFormat);
        foreach(QString gameKey, this->_oldKeys) {
          settings.beginGroup(gameKey);
          settings.remove("");
          settings.endGroup();
        }
      }
    }
  }
}