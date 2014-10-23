#include <Host/Connection.h>
#include <Host/Application.h>
#include <Host/ServiceHandle.h>

#include <Host/Proxy/DownloaderProxy.h>

#include <Core/Service.h>

#include <GameDownloader/GameDownloadService.h>

using GGS::GameDownloader::GameDownloadService;
using GGS::Core::Service;
using GGS::RestApi::GameNetCredential;

namespace GameNet {
  namespace Host {
    namespace Proxy {
      
      DownloaderProxy::DownloaderProxy(QObject *parent /*= 0*/)
        : QObject(parent)
        , _connection(nullptr)
        , _downloader(nullptr)
        , _serviceHandle(nullptr)
      {
      }

      DownloaderProxy::~DownloaderProxy()
      {
      }

      void DownloaderProxy::setDownloader(GameDownloadService* value)
      {
        Q_ASSERT(value);
        this->_downloader = value;

        QObject::connect(this->_downloader, &GameDownloadService::started, this, &DownloaderProxy::onStarted);
        QObject::connect(this->_downloader, &GameDownloadService::finished, this, &DownloaderProxy::onFinished);
        QObject::connect(this->_downloader, &GameDownloadService::stopped, this, &DownloaderProxy::onStopped);
        QObject::connect(this->_downloader, &GameDownloadService::stopping, this, &DownloaderProxy::onStopping);
        QObject::connect(this->_downloader, &GameDownloadService::failed, this, &DownloaderProxy::onFailed);
        QObject::connect(this->_downloader, &GameDownloadService::serviceInstalled, this, &DownloaderProxy::onServiceInstalled);
        QObject::connect(this->_downloader, &GameDownloadService::serviceUpdated, this, &DownloaderProxy::onServiceUpdated);
        QObject::connect(this->_downloader, &GameDownloadService::statusMessageChanged, this, &DownloaderProxy::onStatusMessageChanged);
        QObject::connect(this->_downloader, &GameDownloadService::totalProgressChanged, this, &DownloaderProxy::onTotalProgressChanged);
        QObject::connect(this->_downloader, &GameDownloadService::downloadProgressChanged, this, &DownloaderProxy::onDownloadProgressChanged);
        QObject::connect(this->_downloader, &GameDownloadService::finishedDownloading, this, &DownloaderProxy::finishedDownloading);
      }

      void DownloaderProxy::setConnection(Connection *value)
      {
        Q_ASSERT(value);
        this->_connection = value;
      }

      void DownloaderProxy::setServiceHandle(ServiceHandle *value)
      {
        Q_ASSERT(value);
        this->_serviceHandle = value;
      }

      const Connection* DownloaderProxy::connection()
      {
        return this->_connection;
      }

      void DownloaderProxy::onStarted(const Service *service, GGS::GameDownloader::StartType startType)
      {
        if (!this->isConnectionLockedService(service))
          return;

        emit this->started(service, startType);
      }

      void DownloaderProxy::onFinished(const Service *service)
      {
        Q_ASSERT(this->_serviceHandle);
        if (!this->isConnectionLockedService(service))
          return;

        this->_serviceHandle->unlock(service->id(), this->_connection);
        emit this->finished(service);
      }

      void DownloaderProxy::onStopped(const Service *service)
      {
        Q_ASSERT(this->_serviceHandle);
        if (!this->isConnectionLockedService(service))
          return;

        this->_serviceHandle->unlock(service->id(), this->_connection);
        emit this->stopped(service);
      }

      void DownloaderProxy::onStopping(const Service *service)
      {
        if (!this->isConnectionLockedService(service))
          return;

        emit this->stopping(service);
      }

      void DownloaderProxy::onFailed(const Service *service)
      {
        Q_ASSERT(this->_serviceHandle);
        if (!this->isConnectionLockedService(service))
          return;

        this->_serviceHandle->unlock(service->id(), this->_connection);
        emit this->failed(service);
      }

      void DownloaderProxy::onServiceInstalled(const Service *service)
      {
        if (!this->isConnectionLockedService(service))
          return;

        emit this->serviceInstalled(service);
      }

      void DownloaderProxy::onServiceUpdated(const GGS::Core::Service *service)
      {
        if (!this->isConnectionLockedService(service))
          return;

        emit this->serviceUpdated(service);
      }

      void DownloaderProxy::onStatusMessageChanged(const Service *service, const QString& message)
      {
        if (!this->isConnectionLockedService(service))
          return;

        emit this->statusMessageChanged(service, message);
      }

      void DownloaderProxy::onTotalProgressChanged(const Service *service, qint8 progress)
      {
        if (!this->isConnectionLockedService(service))
          return;

        emit this->totalProgressChanged(service, progress);
      }

      void DownloaderProxy::onDownloadProgressChanged(
        const GGS::Core::Service *service, 
        qint8 progress, 
        GGS::Libtorrent::EventArgs::ProgressEventArgs args)
      {
        if (!this->isConnectionLockedService(service))
          return;

        emit this->downloadProgressChanged(service, progress, args);
      }

      void DownloaderProxy::onFinishedDownloading(const GGS::Core::Service *service)
      {
        if (!this->isConnectionLockedService(service))
          return;

        emit this->finishedDownloading(service);
      }

      bool DownloaderProxy::isInProgress(const Service *service)
      {
        Q_ASSERT(this->_downloader);
        return this->_downloader->isInProgress(service);
      }

      bool DownloaderProxy::isAnyServiceInProgress()
      {
        Q_ASSERT(this->_downloader);
        return this->_downloader->isAnyServiceInProgress();
      }

      bool DownloaderProxy::isInstalled(const QString& serviceId)
      {
        Q_ASSERT(this->_downloader);
        return this->_downloader->isInstalled(serviceId);
      }

      void DownloaderProxy::start(const Service *service, GGS::GameDownloader::StartType startType)
      {
        Q_ASSERT(this->_downloader);
        Q_ASSERT(this->_serviceHandle);

        if (!this->_serviceHandle->lock(service->id(), this->_connection))
          return;

        this->_credentialMap[service->id()] = this->_connection->credential();
        this->_downloader->start(service, startType);
      }

      void DownloaderProxy::stop(const Service *service)
      {
        Q_ASSERT(this->_downloader);
        if (!this->isConnectionLockedService(service))
          return;

        this->_downloader->stop(service);
      }

      void DownloaderProxy::pauseSession()
      {
        Q_ASSERT(this->_downloader);
        this->_downloader->pauseSession();
      }

      void DownloaderProxy::resumeSession()
      {
        Q_ASSERT(this->_downloader);
        this->_downloader->resumeSession();
      }

      bool DownloaderProxy::isConnectionLockedService(const Service *service)
      {
        return this->_connection->isOwnService(service->id());
      }

      GameNetCredential DownloaderProxy::credential(const QString& serviceId)
      {
        if (!this->_credentialMap.contains(serviceId))
          return GameNetCredential();

        return this->_credentialMap[serviceId];
      }

    }
  }
}
