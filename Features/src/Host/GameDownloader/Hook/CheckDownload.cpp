#include <QEventLoop>

#include <Host/ServiceSettings.h>
#include <Host/ServiceProcess/ServiceLoader.h>
#include <Host/GameDownloader/Hook/CheckDownload.h>

#include <GameDownloader/GameDownloadService.h>
#include <GameDownloader/ServiceState.h>

#include <Features/RestApi/ServiceHasAccess.h>

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

        CheckDownload::CheckDownload(QObject *parent /*= 0*/)
          : HookBase("81F2D0B8-298E-4041-83B0-EA5D417F580A", parent)
          , _settings(nullptr)
          , _services(nullptr)
        {
        }

        CheckDownload::~CheckDownload()
        {
        }

        HookBase::HookResult CheckDownload::beforeDownload(GameDownloadService *gameDownloader, ServiceState *state)
        {
          QEventLoop loop;
          int hasAccess = 0;

          ServiceHasAccess *command = new ServiceHasAccess(this);
          command->setServiceId(state->id());

          QObject::connect(command, &ServiceHasAccess::result, [command, &hasAccess]() {
            hasAccess = command->hasAccess();
            command->deleteLater();
          });

          QObject::connect(command, &ServiceHasAccess::result, &loop, &QEventLoop::quit);

          command->execute();
          loop.exec();

          return hasAccess == 1 ? HookBase::Continue : HookBase::Abort;
        }

        HookBase::HookResult CheckDownload::afterDownload(GameDownloadService *gameDownloader, ServiceState *state)
        {
          return HookBase::Continue;
        }

        void CheckDownload::setServiceSettings(ServiceSettings *value)
        {
          Q_ASSERT(value);
          this->_settings = value;
        }

        void CheckDownload::setServiceLoader(ServiceProcess::ServiceLoader *value)
        {
          Q_ASSERT(value);
          this->_services = value;
        }

      }
    }
  }
}


