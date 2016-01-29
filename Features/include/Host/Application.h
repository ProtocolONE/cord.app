#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMutex>

class QTranslator;

namespace Features {

  namespace Thetta {
    class AppDistrIntegrity;
  }

  namespace GameDownloader {
    class GameDownloadStatistics;
  }

  namespace Marketing {
    namespace SystemInfo {
      class SystemInfoManager;
    }
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
    namespace ServiceProcess {
      class ServiceLoader;
      class ServicesListRequest;
    }

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
    class LicenseManager;
    class ThettaImageChecker;

    namespace DBus {
      class DBusServer;
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

      void finalize();

      void setSingleApplication(GGS::Application::SingleApplication *value);

      virtual void restartApplication(bool shouldStartWithSameArguments, bool isMinimized);
      virtual void shutdownUIResult();

    public slots:
      void init();
      void shutdown();
      virtual bool isInitCompleted();
      virtual void switchClientVersion();
      virtual void cancelUninstallServiceRequest(const QString &serviceId);

    private slots:
      void updateCompletedSlot(bool needRestart);
      void onUninstallRequestSlot(const QString &serviceId);
      void internalShutdown();

    signals:
      void initCompleted();
      void restartUIRequest();
      void shutdownUIRequest();
      void restartApplicationRequest(bool shouldStartWithSameArguments, bool isMinimized);
      void uninstallServiceRequest(const QString &service);
      void additionalResourcesReady();

      // INFO Необходим для реимита результата из dbus.
      // в Версии Qt 5.4 можно будет заменитьна QTimer::singleShot
      void internalShutdownUIResult();
      void internalCancelUninstallRequest(const QString &serviceId);

    private:
      friend class ConnectionManager;

      void setInitFinished();
      void setUpdateFinished();
      
      void sendInitFinished();

      void startUi();
      void initGameDownloader();
      bool registerDbusServices();

      void initRestApi();
      void initMarketing();

      bool executedGameCredential(GGS::RestApi::GameNetCredential& credetial, QString& name);
      void internalRestartApplication(bool shouldStartWithSameArguments, bool isMinimized);

      void onNewConnection(Connection *connection);
      void onConnectionLogoutMain();

      void setDownloaderCredential(const GGS::RestApi::GameNetCredential &creds);

      void setUiCommandConnection();
      
      GGS::Application::SingleApplication *_singleApplication;
      ServiceProcess::ServiceLoader *_serviceLoader;
      ServiceProcess::ServicesListRequest *_servicesListRequest;
      GGS::GameDownloader::GameDownloadService *_gameDownloader;
      LicenseManager *_licenseManager;

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
      DBus::DBusServer *_dbusServer;
      Features::Marketing::SystemInfo::SystemInfoManager *_systemInfoManager;
      Features::Thetta::AppDistrIntegrity * _applicationDistrMon;

      bool _initFinished;
      bool _updateFinished;
      bool _closing;
      QMutex _closeMutex;
    };

  }
}
