#pragma once

#include <QtCore/QObject>

namespace GGS {
  namespace GameDownloader {
    class GameDownloadService;
  }
}

namespace GameNet {
  namespace Host {

    class ServiceLoader;

    namespace Bridge {
      class DownloaderBridge;
    }


    class Application : public QObject
    {
      Q_OBJECT
    public:
      explicit Application(QObject *parent = 0);
      ~Application();

      void init();
    private:

      ServiceLoader *_serviceLoader;
      GGS::GameDownloader::GameDownloadService *_gameDownloader;
      Bridge::DownloaderBridge* _downloadBridge;

      void registerServices();
      void initGameDownloader();
    };

  }
}