#pragma once
#include <GameDownloader/HookBase.h>

namespace GGS {
  namespace GameDownloader {
    class GameDownloadService;
    class ServiceState;
    
  }
}

namespace GameNet {
  namespace Host {
    class ServiceSettings;

    namespace ServiceProcess {
      class ServiceLoader;
    }

    namespace GameDownloader {
      namespace Hook {

        class SaveInstallPath : public GGS::GameDownloader::HookBase
        {
          Q_OBJECT
        public:
          explicit SaveInstallPath(QObject *parent = 0);
          virtual ~SaveInstallPath();

          virtual HookResult beforeDownload(
            GGS::GameDownloader::GameDownloadService *gameDownloader, 
            GGS::GameDownloader::ServiceState *state) override;

          virtual HookResult afterDownload(
            GGS::GameDownloader::GameDownloadService *gameDownloader, 
            GGS::GameDownloader::ServiceState *state) override;

          void setServiceSettings(ServiceSettings *value);
          void setServiceLoader(ServiceProcess::ServiceLoader *value);

        private:
          ServiceSettings *_settings;
          ServiceProcess::ServiceLoader *_services;
        };

      }
    }
  }
}

