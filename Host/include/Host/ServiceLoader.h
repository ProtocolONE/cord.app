#pragma once

#include <Host/ServiceDescription.h>

#include <Core/Service.h>

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QString>

namespace GGS {
  namespace Core {
    class Service;
  }

  namespace GameDownloader {
    class GameDownloadService;
  }
}

namespace GameNet {
  namespace Host {

    class HookFactory;
    class ServiceSettings;

    class ServiceLoader : public QObject
    {
      Q_OBJECT

    public:
      explicit ServiceLoader(QObject *parent = 0);
      ~ServiceLoader();

      void setDownloader(GGS::GameDownloader::GameDownloadService *value);
      void setServiceSettings(ServiceSettings *value);

      GGS::Core::Service::Area gameArea() const;
      void setGameArea(GGS::Core::Service::Area val);

      GGS::Core::Service::Area applicationArea() const;
      void setApplicationArea(GGS::Core::Service::Area val);

      QString applicationPath() const;
      void setApplicationPath(const QString& val);

      void registerService(const ServiceDescription& description);
      
      GGS::Core::Service *getService(const QString& serviceId);

    private:
      QString _applicationPath;
      
      GGS::Core::Service::Area _gameArea;
      GGS::Core::Service::Area _applicationArea;
      QHash<QString, GGS::Core::Service *> _serviceMap;

      GGS::GameDownloader::GameDownloadService *_downloader;
      ServiceSettings *_serviceSettings;
      HookFactory *_factory;

      QString getServiceInstallPath(const QString& id);
      QString getServiceDownloadPath(const QString& id);

      void initDownloaderHooks(const ServiceDescription& description);
      void migrateInstallDate(const QString& serviceId);
    };

  }
}