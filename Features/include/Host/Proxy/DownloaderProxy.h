#pragma once

#include <GameDownloader/GameDownloadService.h>

#include <RestApi/GameNetCredential.h>

#include <QtCore/QObject>

namespace P1 {
  namespace Core {
    class Service;
  }

  namespace GameDownloader {
    class GameDownloadService;
  }
}

namespace GameNet {
  namespace Host {

    class Connection;
    class ServiceHandle;

    namespace Proxy {

      class DownloaderProxy : public QObject
      {
        Q_OBJECT
      public:
        explicit DownloaderProxy(QObject *parent = 0);
        virtual ~DownloaderProxy();

        void setDownloader(P1::GameDownloader::GameDownloadService* value);
        void setConnection(Connection *value);
        void setServiceHandle(ServiceHandle *value);

        const Connection* connection();
        P1::RestApi::GameNetCredential credential(const QString& serviceId);

        virtual bool isInProgress(const P1::Core::Service *service);
        virtual bool isAnyServiceInProgress();
        virtual bool isInstalled(const QString& serviceId);

        virtual void start(const P1::Core::Service *service, P1::GameDownloader::StartType startType);
        virtual void stop(const P1::Core::Service *service);

        virtual void pauseSession();
        virtual void resumeSession();
        
      public slots:
        void onDisableDownloadUnlock();
        void onEnableDownloadUnlock();
        
      signals:
        void started(const P1::Core::Service *service, P1::GameDownloader::StartType startType);
        void finished(const P1::Core::Service *service);
        void stopped(const P1::Core::Service *service);
        void stopping(const P1::Core::Service *service);
        void failed(const P1::Core::Service *service);
        void shutdownCompleted();
        void serviceInstalled(const P1::Core::Service *service);
        void serviceUninstalled(const P1::Core::Service *service);
        void serviceUpdated(const P1::Core::Service *service);
        void statusMessageChanged(const P1::Core::Service *service, const QString& message);
        void totalProgressChanged(const P1::Core::Service *service, qint8 progress);
        void downloadProgressChanged(
          const P1::Core::Service *service, 
          qint8 progress, 
          P1::Libtorrent::EventArgs::ProgressEventArgs args);
          
        void finishedDownloading(const P1::Core::Service *service);

        void accessRequired(const P1::Core::Service *service);

      private:
        void onStarted(const P1::Core::Service *service, P1::GameDownloader::StartType startType);
        void onFinished(const P1::Core::Service *service);
        void onStopped(const P1::Core::Service *service);
        void onStopping(const P1::Core::Service *service);
        void onFailed(const P1::Core::Service *service);
        
        void onServiceInstalled(const P1::Core::Service *service);
        void onServiceUninstalled(const P1::Core::Service *service);
        void onServiceUpdated(const P1::Core::Service *service);
        void onStatusMessageChanged(const P1::Core::Service *service, const QString& message);
        void onTotalProgressChanged(const P1::Core::Service *service, qint8 progress);
        void onDownloadProgressChanged(
          const P1::Core::Service *service, 
          qint8 progress, 
          P1::Libtorrent::EventArgs::ProgressEventArgs args);

        void onFinishedDownloading(const P1::Core::Service *service);
        void onAccessRequired(const P1::Core::Service *service);

        bool isConnectionLockedService(const P1::Core::Service *service);

        Connection *_connection;
        P1::GameDownloader::GameDownloadService *_downloader;
        QHash<QString, P1::RestApi::GameNetCredential> _credentialMap;
        ServiceHandle *_serviceHandle;
        bool _unlockDisabled;
      };

    }
  }
}