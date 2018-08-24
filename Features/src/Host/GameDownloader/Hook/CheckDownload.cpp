#include <QEventLoop>

#include <Host/ServiceSettings.h>
#include <Host/ServiceProcess/ServiceLoader.h>
#include <Host/GameDownloader/Hook/CheckDownload.h>

#include <GameDownloader/GameDownloadService.h>
#include <GameDownloader/ServiceState.h>

#include <Features/RestApi/ServiceHasAccess.h>

#include <Core/Service.h>
#include <Core/UI/Message.h>

#include <QtCore/QSettings>

using P1::GameDownloader::GameDownloadService;
using P1::GameDownloader::ServiceState;
using P1::GameDownloader::HookBase;
using GameNet::Host::ServiceProcess::ServiceLoader;
using P1::Core::Service;
using P1::RestApi::GameNetCredential;
using P1::Core::UI::Message;

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

          ServiceHasAccess *command = new ServiceHasAccess();
          command->setServiceId(state->id());
          command->setAuthRequire(false);
          command->appendParameter("userId", credential.userId());
          command->appendParameter("appKey", credential.appKey());

          P1::RestApi::CommandBase::CommandResults requestResult = P1::RestApi::CommandBase::GenericError;

          QObject::connect(command, &ServiceHasAccess::result, [command, &hasAccess, &requestResult](P1::RestApi::CommandBase::CommandResults result) {
            hasAccess = command->hasAccess();
            requestResult = result;
            command->deleteLater();
          });

          QObject::connect(command, &ServiceHasAccess::result, &loop, &QEventLoop::quit);

          command->execute();
          loop.exec();
          
          if (requestResult != P1::RestApi::CommandBase::NoError ) {
            emit this->internalError();
            return HookBase::Abort;
          }

          if (hasAccess != 1) {
            gameDownloader->accessRequired(state->service());
          }

          return hasAccess == 1 ? HookBase::Continue : HookBase::Abort;
        }

        HookBase::HookResult CheckDownload::afterDownload(GameDownloadService *gameDownloader, ServiceState *state)
        {
          return HookBase::Continue;
        }

        void CheckDownload::setCredential(std::function< P1::RestApi::GameNetCredential(const QString& serviceId) > value)
        {
          this->_credential = value;
        }

      }
    }
  }
}


