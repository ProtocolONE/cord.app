#pragma once

#include <GameDownloader/GameDownloadService.h>

#include <RestApi/GameNetCredential.h>

#include <QtCore/QObject>

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

    class Connection;
    class ServiceHandle;

    namespace Proxy {

      class DownloaderProxy : public QObject
      {
        Q_OBJECT
      public:
        explicit DownloaderProxy(QObject *parent = 0);
        virtual ~DownloaderProxy();

        void setDownloader(GGS::GameDownloader::GameDownloadService* value);
        void setConnection(Connection *value);
        void setServiceHandle(ServiceHandle *value);

        const Connection* connection();
        GGS::RestApi::GameNetCredential credential(const QString& serviceId);

        virtual bool isInProgress(const GGS::Core::Service *service);
        virtual bool isAnyServiceInProgress();
        virtual bool isInstalled(const QString& serviceId);

        virtual void start(const GGS::Core::Service *service, GGS::GameDownloader::StartType startType);
        virtual void stop(const GGS::Core::Service *service);

        virtual void pauseSession();
        virtual void resumeSession();

      signals:
        void started(const GGS::Core::Service *service, GGS::GameDownloader::StartType startType);
        void finished(const GGS::Core::Service *service);
        void stopped(const GGS::Core::Service *service);
        void stopping(const GGS::Core::Service *service);
        void failed(const GGS::Core::Service *service);
        void shutdownCompleted();
        void serviceInstalled(const GGS::Core::Service *service);
        void serviceUpdated(const GGS::Core::Service *service);
        void statusMessageChanged(const GGS::Core::Service *service, const QString& message);
        void totalProgressChanged(const GGS::Core::Service *service, qint8 progress);
        void downloadProgressChanged(
          const GGS::Core::Service *service, 
          qint8 progress, 
          GGS::Libtorrent::EventArgs::ProgressEventArgs args);
          
        void finishedDownloading(const GGS::Core::Service *service);

      private:
        void onStarted(const GGS::Core::Service *service, GGS::GameDownloader::StartType startType);
        void onFinished(const GGS::Core::Service *service);
        void onStopped(const GGS::Core::Service *service);
        void onStopping(const GGS::Core::Service *service);
        void onFailed(const GGS::Core::Service *service);
        
        void onServiceInstalled(const GGS::Core::Service *service);
        void onServiceUpdated(const GGS::Core::Service *service);
        void onStatusMessageChanged(const GGS::Core::Service *service, const QString& message);
        void onTotalProgressChanged(const GGS::Core::Service *service, qint8 progress);
        void onDownloadProgressChanged(
          const GGS::Core::Service *service, 
          qint8 progress, 
          GGS::Libtorrent::EventArgs::ProgressEventArgs args);

        void onFinishedDownloading(const GGS::Core::Service *service);

        bool isConnectionLockedService(const GGS::Core::Service *service);

        Connection *_connection;
        GGS::GameDownloader::GameDownloadService *_downloader;
        QHash<QString, GGS::RestApi::GameNetCredential> _credentialMap;
        ServiceHandle *_serviceHandle;
      };

    }
  }
}