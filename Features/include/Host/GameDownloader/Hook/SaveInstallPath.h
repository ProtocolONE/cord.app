#pragma once
#include <GameDownloader/HookBase.h>

namespace P1 {
  namespace GameDownloader {
    class GameDownloadService;
    class ServiceState;
    
  }
}

namespace P1 {
  namespace Host {
    class ServiceSettings;

    namespace ServiceProcess {
      class ServiceLoader;
    }

    namespace GameDownloader {
      namespace Hook {

        class SaveInstallPath : public P1::GameDownloader::HookBase
        {
          Q_OBJECT
        public:
          explicit SaveInstallPath(QObject *parent = 0);
          virtual ~SaveInstallPath();

          virtual HookResult beforeDownload(
            P1::GameDownloader::GameDownloadService *gameDownloader, 
            P1::GameDownloader::ServiceState *state) override;

          virtual HookResult afterDownload(
            P1::GameDownloader::GameDownloadService *gameDownloader, 
            P1::GameDownloader::ServiceState *state) override;

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

