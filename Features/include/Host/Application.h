#pragma once

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QHash>
#include <QtCore/QPair>

#include <RestApi/GameNetCredential.h>

class QTranslator;
class QDBusConnection;

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
  }

  namespace Marketing {
    class MarketingTarget;
  }
}

namespace GameNet {
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

    namespace Bridge {
      class DownloaderBridge;
      class DownloaderSettingsBridge;
      class ServiceSettingsBridge;
      class ExecutorBridge;
    }

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
      void unregisterDbusServices();
      void shutdown();

      virtual void setCredential(
        const QString& connectionName,
        const QString& applicationName,
        const GGS::RestApi::GameNetCredential& credential);

      virtual GGS::RestApi::GameNetCredential credential(const QString& connectionName);

    public slots:
      virtual bool isInitCompleted();

      void setInitFinished();
      void setUpdateFinished();

      virtual void switchClientVersion();

    private slots:
      void updateCompletedSlot(bool needRestart);

    signals:
      void initCompleted();
      
    private:
      void startUi();
      void registerServices();
      void initGameDownloader();
      void initTranslations();
      void registerDbusServices();
      void registerDBusObjects(QDBusConnection *connection);

      void initRestApi();
      void initMarketing();

      GGS::Application::SingleApplication *_singleApplication;
      ServiceLoader *_serviceLoader;
      Proxy::DownloaderProxy *_gameDownloader;

      DownloaderSettings *_downloaderSettings;
      ServiceSettings *_serviceSettings;
      Proxy::GameExecutorProxy *_executor;
      ShutdownManager *_shutdown;
      HookFactory *_downloaderHookFactory;
      ExecutorHookFactory *_executorHookFactory;

      Bridge::DownloaderBridge* _downloaderBridge;
      Bridge::DownloaderSettingsBridge *_downloaderSettingsBridge;
      Bridge::ServiceSettingsBridge *_serviceSettingsBridge;
      Bridge::ExecutorBridge *_excutorBridge;
      Bridge::UpdateManagerBridge* _updateManagerBridge;
      Bridge::ApplicationBridge* _applicationBridge;

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

      bool _initFinished;
      bool _updateFinished;

      QMap<QString, QTranslator*> _translators;

      QHash<QString, QPair<QString, GGS::RestApi::GameNetCredential> > _connectionCredential;
    };

  }
}
