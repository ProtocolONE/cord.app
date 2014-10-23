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

  namespace GameExecutor {
    class GameExecutorService;
  }
}

namespace GameNet {
  namespace Host {

    class HookFactory;
    class ExecutorHookFactory;
    class GameExecutor;
    class ServiceSettings;
    class Thetta;

    class ServiceLoader : public QObject
    {
      Q_OBJECT

    public:
      explicit ServiceLoader(QObject *parent = 0);
      virtual ~ServiceLoader();

      void setDownloader(GGS::GameDownloader::GameDownloadService *value);
      void setExecutor(GGS::GameExecutor::GameExecutorService *value);

      void setSimpleMainExecutor(GGS::GameExecutor::GameExecutorService *value);
      void setSecondExecutor(GGS::GameExecutor::GameExecutorService *value);
      
      void setDownloaderHookFactory(HookFactory *value);
      void setExecuterHookFactory(ExecutorHookFactory *value);

      GGS::Core::Service::Area gameArea() const;
      void setGameArea(GGS::Core::Service::Area val);

      QString applicationPath() const;
      void setApplicationPath(const QString& val);

      void registerService(const ServiceDescription& description);
      
      virtual GGS::Core::Service *getService(const QString& serviceId);
      QHash<QString, GGS::Core::Service *>& servicesMap();

    private:
      QString _applicationPath;
      
      GGS::Core::Service::Area _gameArea;
      GGS::Core::Service::Area _applicationArea;
      QHash<QString, GGS::Core::Service *> _serviceMap;

      GGS::GameDownloader::GameDownloadService *_downloader;

      GGS::GameExecutor::GameExecutorService *_executor;
      GGS::GameExecutor::GameExecutorService *_secondExecutor; // Executor for premium second game
      GGS::GameExecutor::GameExecutorService *_simpleMainExecutor; // Executor for main copy of game when second still alive.

      HookFactory *_factory;
      ExecutorHookFactory *_executorHookFactory;

      QString getServiceInstallPath(const QString& id);
      QString getServiceDownloadPath(const QString& id);

      void initDownloaderHooks(const ServiceDescription& description);
      void initExecutorHooks(const GGS::Core::Service *service, const ServiceDescription& description);
      void migrateInstallDate(const QString& serviceId);
    };

  }
}