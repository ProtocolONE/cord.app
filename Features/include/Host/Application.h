#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>

class QTranslator;

namespace Features {
  namespace GameDownloader {
    class GameDownloadStatistics;
  }

  class StopDownloadServiceWhileExecuteAnyGame;
}

namespace GGS {
  namespace Application {
    class SingleApplication;
  }

  namespace GameDownloader {
    class GameDownloadService;
  }

  namespace RestApi {
    class RestApiManager;
    class FakeCache;
    class GameNetCredential;
  }

  namespace Marketing {
    class MarketingTarget;
  }
}

namespace GameNet {
  namespace Integration {
    namespace ZZima {
      class ZZimaConnection;
    }
  }

  namespace Host {

    class ServiceLoader;
    class DownloaderSettings;
    class ServiceSettings;
    class GameExecutor;
    class Thetta;
    class UpdateManagerBridge;
    class Updater;
    class UIProcess;
    class ApplicationRestarter;
    class ShutdownManager;
    class HookFactory;
    class ExecutorHookFactory;
    class ApplicationStatistic;
    class MarketingStatistic;
    class CommandLineManager;
    class Translation;
    class MessageAdapter;
    class ConnectionManager;
    class ServiceHandle;
    class AutoRunManager;
    class Connection;

    namespace Proxy {
      class GameExecutorProxy;
      class DownloaderProxy;
    }

    class Application : public QObject
    {
      Q_OBJECT
    public:
      explicit Application(QObject *parent = 0);
      ~Application();

      void init();
      void finalize();

      void setSingleApplication(GGS::Application::SingleApplication *value);

      virtual void restartApplication(bool shouldStartWithSameArguments, bool isMinimized);

    public slots:
      void shutdown();
      virtual bool isInitCompleted();
      virtual void switchClientVersion();

    private slots:
      void updateCompletedSlot(bool needRestart);

    signals:
      void initCompleted();
      void restartUIRequest();
      void restartApplicationRequest(bool shouldStartWithSameArguments, bool isMinimized);

    private:
      friend class ConnectionManager;

      void setInitFinished();
      void setUpdateFinished();

      void startUi();
      void registerServices();
      void initGameDownloader();
      void registerDbusServices();

      void initRestApi();
      void initMarketing();

      bool executedGameCredential(GGS::RestApi::GameNetCredential& credetial, QString& name);
      void internalRestartApplication(bool shouldStartWithSameArguments, bool isMinimized);
      void onNewConnection(Connection *connection);
      void onConnectionLogoutMain();

      GGS::Application::SingleApplication *_singleApplication;
      ServiceLoader *_serviceLoader;
      GGS::GameDownloader::GameDownloadService *_gameDownloader;

      DownloaderSettings *_downloaderSettings;
      ServiceSettings *_serviceSettings;
      GameExecutor *_executor;
      ShutdownManager *_shutdown;
      HookFactory *_downloaderHookFactory;
      ExecutorHookFactory *_executorHookFactory;
      ConnectionManager *_connectionManager;

      Features::GameDownloader::GameDownloadStatistics *_downloadStatistics;
      Features::StopDownloadServiceWhileExecuteAnyGame *_stopDownloadServiceOnExecuteGame;
      Thetta *_thetta;
      GGS::RestApi::RestApiManager *_restApiManager;
      GGS::RestApi::FakeCache *_restApiCache;

      Updater *_updater;
      UIProcess *_uiProcess;
      ApplicationRestarter *_applicationRestarter;
      ApplicationStatistic *_applicationStatistic;
      MarketingStatistic *_marketingStatistic;
      GGS::Marketing::MarketingTarget *_marketingTarget;
      CommandLineManager *_commandLineManager;
      Translation *_translation;
      MessageAdapter *_messageAdapter;
      ServiceHandle *_serviceHandle;
      ::GameNet::Integration::ZZima::ZZimaConnection *_zzimaConnection;
      AutoRunManager *_autoRunManager;

      bool _initFinished;
      bool _updateFinished;
    };

  }
}
