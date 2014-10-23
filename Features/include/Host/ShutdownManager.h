#pragma once

#include <QtCore/QObject>

namespace GGS {
  namespace Application {
    class SingleApplication;
  }

  namespace GameDownloader {
    class GameDownloadService;
  }
}

namespace GameNet {
  namespace Host {

    class Thetta;
    class GameExecutor;
    class Application;
    class ConnectionManager;

    class ShutdownManager : public QObject
    {
      Q_OBJECT
    public:
      explicit ShutdownManager(QObject *parent = 0);
      virtual ~ShutdownManager();

      void shutdown();
      void finalize();

      void setGameDownloadInitialized();

      void setThetta(Thetta *value);
      void setExecutor(GameExecutor *value);
      void setDownloader(GGS::GameDownloader::GameDownloadService *value);
      void setApplication(Application *value);
      void setSingleApplication(GGS::Application::SingleApplication *value);
      void setConnectionManager(ConnectionManager *value);

    signals:
      void shutdownCompleted();

    private:
      void shutdownDownloader();
      void shutdownDownloaderCompleted();
      void shutdownThetta();
      void shutdownThettaCompleted();

      Thetta *_thetta;
      GameExecutor *_executor;
      GGS::GameDownloader::GameDownloadService *_downloader;
      Application *_application;
      GGS::Application::SingleApplication *_singleApplication;
      ConnectionManager *_connectionManager;
      bool _gameDownloadInitialized;
    };

  }
}