#include <Host/GameDownloader/Hook/UpdateUninstallInfo.h>
#include <Host/Installer/UninstallInfo.h>

#include <Host/ServiceSettings.h>
#include <Host/ServiceProcess/ServiceLoader.h>

#include <GameDownloader/GameDownloadService.h>
#include <GameDownloader/ServiceState.h>
#include <GameDownloader/StartType.h>

#include <Core/Service.h>

#include <QtCore/QSettings>

using P1::GameDownloader::GameDownloadService;
using P1::GameDownloader::ServiceState;
using P1::GameDownloader::HookBase;
using P1::Host::ServiceProcess::ServiceLoader;
using P1::Core::Service;

namespace P1 {
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
          const P1::Core::Service *service = state->service();
          Q_ASSERT(service);

          if (state->startType() == P1::GameDownloader::Uninstall) {
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

        void UpdateUninstallInfo::saveUninstallInfo(const P1::Core::Service *service)
        {
          Q_ASSERT(service);

          P1::Host::Installer::UninstallInfo info(service->id());
          info.setDisplayName(service->displayName());
          info.save();
        }

        void UpdateUninstallInfo::removeUninstallInfo(const P1::Core::Service *service)
        {
          Q_ASSERT(service);

          QSettings settings("HKEY_LOCAL_MACHINE\\Software\\ProtocolOne\\Launcher", QSettings::NativeFormat);
          settings.beginGroup(service->id());
          settings.remove("LicenseHash");
          settings.endGroup();

          P1::Host::Installer::UninstallInfo info(service->id());
          info.remove();
        }
      }
    }
  }
}


