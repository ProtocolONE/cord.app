#pragma once

#include <GameDownloader/HookBase.h>

namespace GGS {
  namespace Core {
    class Service;
  }

  namespace GameDownloader {
    class GameDownloadService;
    class ServiceState;
  }
}

namespace GameNet {
  namespace Host {
    namespace GameDownloader {
      namespace Hook {

        class UpdateUninstallInfo : public GGS::GameDownloader::HookBase
        {
          Q_OBJECT
        public:
          explicit UpdateUninstallInfo(QObject *parent = 0);
          virtual ~UpdateUninstallInfo();

        virtual HookResult beforeDownload(
          GGS::GameDownloader::GameDownloadService *gameDownloader, 
          GGS::GameDownloader::ServiceState *state) override;

        virtual HookResult afterDownload(
          GGS::GameDownloader::GameDownloadService *gameDownloader, 
          GGS::GameDownloader::ServiceState *state) override;

        private:
          void saveUninstallInfo(const GGS::Core::Service *service);
          void removeUninstallInfo(const GGS::Core::Service *service);

        };
      }
    }
  }
}

