#pragma once

#include <GameDownloader/HookBase.h>

namespace P1 {
  namespace Core {
    class Service;
  }

  namespace GameDownloader {
    class GameDownloadService;
    class ServiceState;
  }
}

namespace P1 {
  namespace Host {
    namespace GameDownloader {
      namespace Hook {

        class UpdateUninstallInfo : public P1::GameDownloader::HookBase
        {
          Q_OBJECT
        public:
          explicit UpdateUninstallInfo(QObject *parent = 0);
          virtual ~UpdateUninstallInfo();

        virtual HookResult beforeDownload(
          P1::GameDownloader::GameDownloadService *gameDownloader, 
          P1::GameDownloader::ServiceState *state) override;

        virtual HookResult afterDownload(
          P1::GameDownloader::GameDownloadService *gameDownloader, 
          P1::GameDownloader::ServiceState *state) override;

        private:
          void saveUninstallInfo(const P1::Core::Service *service);
          void removeUninstallInfo(const P1::Core::Service *service);

        };
      }
    }
  }
}

