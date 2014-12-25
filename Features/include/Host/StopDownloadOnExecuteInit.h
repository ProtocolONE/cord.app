#pragma once

namespace GGS {
  namespace GameDownloader {
    class GameDownloadService;
  }
}

namespace Features {
  class StopDownloadServiceWhileExecuteAnyGame;
}

namespace GameNet {
  namespace Host {

    class GameExecutor;

    namespace ServiceProcess {
      class ServiceLoader;
    }

    class StopDownloadOnExecuteInit
    {
    public:
      StopDownloadOnExecuteInit();
      virtual ~StopDownloadOnExecuteInit();

      void setTarget(Features::StopDownloadServiceWhileExecuteAnyGame *value);
      void setDownloader(GGS::GameDownloader::GameDownloadService *value);
      void setServices(ServiceProcess::ServiceLoader *value);
      void setExecutor(GameExecutor *value);

      void init();

    private:
      Features::StopDownloadServiceWhileExecuteAnyGame *_target;
      GGS::GameDownloader::GameDownloadService *_downloader;
      ServiceProcess::ServiceLoader *_services;
      GameExecutor *_excutor;
    };

  }
}