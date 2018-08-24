#pragma once

#include <Host/ServiceProcess/ServiceDescription.h>

#include <Core/Service.h>

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QString>

namespace P1 {
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

    namespace ServiceProcess {

      class ServiceLoader : public QObject
      {
        Q_OBJECT

      public:
        explicit ServiceLoader(QObject *parent = 0);
        virtual ~ServiceLoader();

        void setDownloader(P1::GameDownloader::GameDownloadService *value);
        void setExecutor(P1::GameExecutor::GameExecutorService *value);

        void setSimpleMainExecutor(P1::GameExecutor::GameExecutorService *value);
        void setSecondExecutor(P1::GameExecutor::GameExecutorService *value);

        void setDownloaderHookFactory(HookFactory *value);
        void setExecuterHookFactory(ExecutorHookFactory *value);

        P1::Core::Service::Area gameArea() const;
        void setGameArea(P1::Core::Service::Area val);

        QString applicationPath() const;
        void setApplicationPath(const QString& val);

        void registerService(const ServiceDescription& description);

        virtual P1::Core::Service *getService(const QString& serviceId);
        QHash<QString, P1::Core::Service *>& servicesMap();

      private:
        QString _applicationPath;

        P1::Core::Service::Area _gameArea;
        P1::Core::Service::Area _applicationArea;
        QHash<QString, P1::Core::Service *> _serviceMap;

        P1::GameDownloader::GameDownloadService *_downloader;

        P1::GameExecutor::GameExecutorService *_executor;
        P1::GameExecutor::GameExecutorService *_secondExecutor; // Executor for premium second game
        P1::GameExecutor::GameExecutorService *_simpleMainExecutor; // Executor for main copy of game when second still alive.

        HookFactory *_factory;
        ExecutorHookFactory *_executorHookFactory;

        QString getServiceInstallPath(const QString& id);
        QString getServiceDownloadPath(const QString& id);

        void initDownloaderHooks(const ServiceDescription& description);
        void initExecutorHooks(const P1::Core::Service *service, const ServiceDescription& description);
        void migrateInstallDate(const QString& serviceId);
      };
    }
  }
}