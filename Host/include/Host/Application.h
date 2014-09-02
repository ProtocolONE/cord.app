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
}

namespace GameNet {
  namespace Host {

    class ServiceLoader;
    class DownloaderSettings;
    class ServiceSettings;

    namespace Bridge {
      class DownloaderBridge;
      class DownloaderSettingsBridge;
      class ServiceSettingsBridge;
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
      DownloaderSettings *_downloaderSettings;
      ServiceSettings *_serviceSettings;
      Bridge::DownloaderBridge* _downloaderBridge;
      Bridge::DownloaderSettingsBridge *_downloaderSettingsBridge;
      Bridge::ServiceSettingsBridge *_serviceSettingsBridge;
      Features::GameDownloader::GameDownloadStatistics *_downloadStatistics;

      QMap<QString, QTranslator*> _translators;

      void registerServices();
      void initGameDownloader();
      void initTranslations();
    };

  }
}