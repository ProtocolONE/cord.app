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
using GGS::RestApi::GameNetCredential;

namespace GameNet {
  namespace Host {
    namespace GameDownloader {
      namespace Hook {

        CheckDownload::CheckDownload(QObject *parent /*= 0*/)
          : HookBase("81F2D0B8-298E-4041-83B0-EA5D417F580A", parent)
        {
        }

        CheckDownload::~CheckDownload()
        {
        }

        HookBase::HookResult CheckDownload::beforeDownload(GameDownloadService *gameDownloader, ServiceState *state)
        {
          Q_ASSERT(this->_credential);

          GameNetCredential credential = this->_credential(state->id());
          if (credential.isEmpty())
            return HookBase::Abort;

          QEventLoop loop;
          int hasAccess = 0;

          ServiceHasAccess *command = new ServiceHasAccess(this);
          command->setServiceId(state->id());
          command->setAuthRequire(false);
          command->appendParameter("userId", credential.userId());
          command->appendParameter("appKey", credential.appKey());

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

        void CheckDownload::setCredential(std::function< GGS::RestApi::GameNetCredential(const QString& serviceId) > value)
        {
          this->_credential = value;
        }

      }
    }
  }
}


