#pragma once

#include <QtCore/QObject>

namespace P1 {
  namespace Application {
    class SingleApplication;
  }

  namespace GameDownloader {
    class GameDownloadService;
  }
}

namespace P1 {
  namespace Host {

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

      void setExecutor(GameExecutor *value);
      void setDownloader(P1::GameDownloader::GameDownloadService *value);
      void setApplication(Application *value);
      void setSingleApplication(P1::Application::SingleApplication *value);
      void setConnectionManager(ConnectionManager *value);

    signals:
      void shutdownCompleted();

    private:
      void shutdownDownloader();
      void shutdownDownloaderCompleted();

      GameExecutor *_executor;
      P1::GameDownloader::GameDownloadService *_downloader;
      Application *_application;
      P1::Application::SingleApplication *_singleApplication;
      ConnectionManager *_connectionManager;
      bool _gameDownloadInitialized;
    };

  }
}