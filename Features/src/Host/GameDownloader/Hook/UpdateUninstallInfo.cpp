#include <Host/GameDownloader/Hook/UpdateUninstallInfo.h>
#include <Host/Installer/UninstallInfo.h>

#include <Host/ServiceSettings.h>
#include <Host/ServiceProcess/ServiceLoader.h>

#include <GameDownloader/GameDownloadService.h>
#include <GameDownloader/ServiceState.h>
#include <GameDownloader/StartType.h>

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

        UpdateUninstallInfo::UpdateUninstallInfo(QObject *parent /*= 0*/)
          : HookBase("65CE6F6B-B21A-4d8b-8FD4-B5B750D556CD", parent)
        {
        }

        UpdateUninstallInfo::~UpdateUninstallInfo()
        {
        }

        HookBase::HookResult UpdateUninstallInfo::beforeDownload(GameDownloadService *gameDownloader, ServiceState *state)
        {
          const GGS::Core::Service *service = state->service();
          Q_ASSERT(service);

          if (state->startType() == GGS::GameDownloader::Uninstall) {
            this->removeUninstallInfo(service);
          } else {
            this->saveUninstallInfo(service);
          }

          return HookBase::Continue;
        }

        HookBase::HookResult UpdateUninstallInfo::afterDownload(GameDownloadService *gameDownloader, ServiceState *state)
        {
          return HookBase::Continue;
        }

        void UpdateUninstallInfo::saveUninstallInfo(const GGS::Core::Service *service)
        {
          Q_ASSERT(service);

          GameNet::Host::Installer::UninstallInfo info(service->id());
          info.setDisplayName(service->displayName());
          info.save();
        }

        void UpdateUninstallInfo::removeUninstallInfo(const GGS::Core::Service *service)
        {
          Q_ASSERT(service);

          GameNet::Host::Installer::UninstallInfo info(service->id());
          info.remove();
        }
      }
    }
  }
}


