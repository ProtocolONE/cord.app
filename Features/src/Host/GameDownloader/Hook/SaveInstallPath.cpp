#include <Host/ServiceSettings.h>
#include <Host/ServiceProcess/ServiceLoader.h>
#include <Host/GameDownloader/Hook/SaveInstallPath.h>
#include <GameDownloader/GameDownloadService.h>
#include <GameDownloader/ServiceState.h>

#include <Core/Service.h>

#include <QtCore/QSettings>

using GGS::GameDownloader::GameDownloadService;
using GGS::GameDownloader::ServiceState;
using GGS::GameDownloader::HookBase;
using GameNet::Host::ServiceProcess::ServiceLoader;
using GGS::Core::Service;

namespace GameNet {
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
          
          QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);
          settings.beginGroup(id);
          settings.setValue("DownloadPath", this->_settings->downloadPath(id));
          settings.setValue("InstallPath", this->_settings->installPath(id));

          GGS::Core::Service *service = this->_services->getService(id);
          Q_ASSERT(service);
          service->setIsDefaultInstallPath(false);
          
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


