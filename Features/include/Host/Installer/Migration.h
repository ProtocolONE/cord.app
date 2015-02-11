#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>

namespace GGS {
  namespace GameDownloader {
    class GameDownloadService;
  }
}

namespace GameNet {
  namespace Host {
    namespace ServiceProcess {
      class ServiceLoader;
    }

    namespace Installer { 

      class Migration
      {
      public:
        Migration();
        virtual ~Migration();

        static bool isMigrated();
        void setServices(ServiceProcess::ServiceLoader *value);
        void setDownloader(GGS::GameDownloader::GameDownloadService *value);
        void migrate();

      private:
        void removeOldUninstallKeys();

        ServiceProcess::ServiceLoader* _services;
        GGS::GameDownloader::GameDownloadService* _downloader;

        QStringList _oldKeys;
      };
    }
  }
}