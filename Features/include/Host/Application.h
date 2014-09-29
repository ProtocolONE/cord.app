#pragma once

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>

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
    class ArgumentParser;
  }

  namespace GameDownloader {
    class GameDownloadService;
  }

  namespace RestApi {
    class RestApiManager;
    class FakeCache;
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

    namespace Bridge {
      class DownloaderBridge;
      class DownloaderSettingsBridge;
      class ServiceSettingsBridge;
      class ExecutorBridge;
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

    public slots:
      virtual bool isInitCompleted();

      void setInitFinished();
      void setUpdateFinished();

      virtual void switchClientVersion();

    private slots:
      void updateCompletedSlot(bool needRestart);
      void commandRecieved(QString name, QStringList arguments);       

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

      GGS::Application::SingleApplication *_singleApplication;
      ServiceLoader *_serviceLoader;
      GGS::GameDownloader::GameDownloadService *_gameDownloader;
      DownloaderSettings *_downloaderSettings;
      ServiceSettings *_serviceSettings;
      GameExecutor *_executor;
      ShutdownManager *_shutdown;
      HookFactory *_downloaderHookFactory;
      ExecutorHookFactory *_executorHookFactory;

      Bridge::DownloaderBridge* _downloaderBridge;
      Bridge::DownloaderSettingsBridge *_downloaderSettingsBridge;
      Bridge::ServiceSettingsBridge *_serviceSettingsBridge;
      Bridge::ExecutorBridge *_excutorBridge;

      Features::GameDownloader::GameDownloadStatistics *_downloadStatistics;
      Features::StopDownloadServiceWhileExecuteAnyGame *_stopDownloadServiceOnExecuteGame;
      Thetta *_thetta;
      GGS::RestApi::RestApiManager *_restApiManager;
      GGS::RestApi::FakeCache *_restApiCache;

      Updater* _updater;
      UIProcess* _uiProcess;
      ApplicationRestarter* _applicationRestarter;

      bool _initFinished;
      bool _updateFinished;

      QMap<QString, QTranslator*> _translators;
      
      GGS::Application::ArgumentParser* _commandLineArguments;
    };

  }
}
