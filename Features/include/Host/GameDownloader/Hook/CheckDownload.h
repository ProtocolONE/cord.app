#pragma once
#include <GameDownloader/HookBase.h>
#include <RestApi/GameNetCredential.h>
#include <functional>

namespace GGS {
  namespace GameDownloader {
    class GameDownloadService;
    class ServiceState;
  }
}

namespace GameNet {
  namespace Host {

    namespace GameDownloader {
      namespace Hook {

        class CheckDownload : public GGS::GameDownloader::HookBase
        {
        public:
          explicit CheckDownload(QObject *parent = 0);
          virtual ~CheckDownload();

          virtual HookResult beforeDownload(
            GGS::GameDownloader::GameDownloadService *gameDownloader, 
            GGS::GameDownloader::ServiceState *state) override;

          virtual HookResult afterDownload(
            GGS::GameDownloader::GameDownloadService *gameDownloader, 
            GGS::GameDownloader::ServiceState *state) override;

          void setCredential(std::function< GGS::RestApi::GameNetCredential(const QString& serviceId) > value);

        signals:
          void accessRequired();

        private:
          std::function< GGS::RestApi::GameNetCredential(const QString& serviceId) > _credential;

        };

      }
    }
  }
}

 