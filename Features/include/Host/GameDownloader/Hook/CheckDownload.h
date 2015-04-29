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

 