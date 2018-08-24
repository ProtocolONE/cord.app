#pragma once
#include <GameDownloader/HookBase.h>
#include <RestApi/GameNetCredential.h>
#include <functional>

namespace P1 {
  namespace GameDownloader {
    class GameDownloadService;
    class ServiceState;
  }
}

namespace GameNet {
  namespace Host {
    namespace GameDownloader {
      namespace Hook {

        class CheckDownload : public P1::GameDownloader::HookBase
        {
          Q_OBJECT
        public:
          explicit CheckDownload(QObject *parent = 0);
          virtual ~CheckDownload();

          virtual HookResult beforeDownload(
            P1::GameDownloader::GameDownloadService *gameDownloader, 
            P1::GameDownloader::ServiceState *state) override;

          virtual HookResult afterDownload(
            P1::GameDownloader::GameDownloadService *gameDownloader, 
            P1::GameDownloader::ServiceState *state) override;

          void setCredential(std::function< P1::RestApi::GameNetCredential(const QString& serviceId) > value);

        signals:
          void accessRequired();

          void internalError();

        private:
          std::function< P1::RestApi::GameNetCredential(const QString& serviceId) > _credential;

        };

      }
    }
  }
}

 