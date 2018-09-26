#include <Host/ServiceSettings.h>
#include <Host/ServiceProcess/ServiceLoader.h>
#include <Host/GameDownloader/Hook/SaveInstallPath.h>
#include <GameDownloader/GameDownloadService.h>
#include <GameDownloader/ServiceState.h>

#include <Core/Service.h>

#include <QtCore/QSettings>
#include <QtCore/QCoreApplication>

using P1::GameDownloader::GameDownloadService;
using P1::GameDownloader::ServiceState;
using P1::GameDownloader::HookBase;
using P1::GameDownloader::StartType;
using P1::Host::ServiceProcess::ServiceLoader;
using P1::Core::Service;

namespace P1 {
  namespace Host {
    namespace GameDownloader {
      namespace Hook {

        SaveInstallPath::SaveInstallPath(QObject *parent /*= 0*/)
          : HookBase("D4D358CD-DFF5-4B56-AF30-349CCAE86EED", parent)
          , _settings(nullptr)
          , _services(nullptr)
        {
        }

        SaveInstallPath::~SaveInstallPath()
        {
        }

        HookBase::HookResult SaveInstallPath::beforeDownload(GameDownloadService *gameDownloader, ServiceState *state)
        {
          Q_ASSERT(this->_settings);
          QString id(state->id());
          
          P1::Core::Service *service = this->_services->getService(id);
          Q_ASSERT(service);

          QSettings settings(
            QSettings::NativeFormat,
            QSettings::UserScope,
            QCoreApplication::organizationName(),
            QCoreApplication::applicationName());

          settings.beginGroup("Integration");
          settings.beginGroup(id);
          if (state->startType() != StartType::Uninstall) {
            settings.setValue("DownloadPath", this->_settings->downloadPath(id));
            settings.setValue("InstallPath", this->_settings->installPath(id));
            service->setIsDefaultInstallPath(false);
          } else {
            settings.remove("DownloadPath");
            settings.remove("InstallPath");
            service->setIsDefaultInstallPath(true);
          }
                              
          return HookBase::Continue;
        }

        HookBase::HookResult SaveInstallPath::afterDownload(GameDownloadService *gameDownloader, ServiceState *state)
        {
          return HookBase::Continue;
        }

        void SaveInstallPath::setServiceSettings(ServiceSettings *value)
        {
          Q_ASSERT(value);
          this->_settings = value;
        }

        void SaveInstallPath::setServiceLoader(ServiceProcess::ServiceLoader *value)
        {
          Q_ASSERT(value);
          this->_services = value;
        }

      }
    }
  }
}


