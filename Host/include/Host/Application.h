#pragma once

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QTranslator>

namespace Features {
  namespace GameDownloader {
    class GameDownloadStatistics;
  }
}
namespace GGS {
  namespace GameDownloader {
    class GameDownloadService;
  }

  namespace Application {
    class ArgumentParser;
  }
}

namespace GameNet {
  namespace Host {

    class ServiceLoader;
    class DownloaderSettings;
    class ServiceSettings;
    class UpdateManagerBridge;
    class Updater;
    class UIProcess;
    class ApplicationRestarter;

    namespace Bridge {
      class DownloaderBridge;
      class DownloaderSettingsBridge;
      class ServiceSettingsBridge;
      class UpdateManagerBridge;
      class ApplicationBridge;
    }

    class Application : public QObject
    {
      Q_OBJECT
    public:
      explicit Application(QObject *parent = 0);
      ~Application();

      void init();
      void restartApplication(bool shouldStartWithSameArguments, bool isMinimized);

    public slots:
      bool isInitCompleted();

      void setInitFinished();
      void setUpdateFinished();

      void switchClientVersion();

   private slots:
      void shutdownCompleted();
      void updateCompletedSlot(bool needRestart);

    signals:
      void initCompleted();

    private:
      void registerServices();
      void initGameDownloader();
      void initTranslations();

      ServiceLoader *_serviceLoader;
      GGS::GameDownloader::GameDownloadService *_gameDownloader;
      DownloaderSettings *_downloaderSettings;
      ServiceSettings *_serviceSettings;
      Bridge::DownloaderBridge* _downloaderBridge;
      Bridge::DownloaderSettingsBridge *_downloaderSettingsBridge;
      Bridge::ServiceSettingsBridge *_serviceSettingsBridge;
      Bridge::UpdateManagerBridge* _updateManagerBridge;
      Bridge::ApplicationBridge* _applicationBridge;

      Features::GameDownloader::GameDownloadStatistics *_downloadStatistics;

      bool _initFinished;
      bool _updateFinished;

      QMap<QString, QTranslator*> _translators;
      Updater* _updater;
      UIProcess* _uiProcess;
      ApplicationRestarter* _applicationRestarter;
      
      GGS::Application::ArgumentParser* _commandLineArguments;
    };

  }
}