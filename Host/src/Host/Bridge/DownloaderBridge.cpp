#include <Host/Bridge/DownloaderBridge.h>

#include <Host/ServiceLoader.h>

#include <Core/Service.h>

using GGS::GameDownloader::GameDownloadService;
using GGS::Core::Service;
using GGS::Libtorrent::EventArgs::ProgressEventArgs;

namespace GameNet {
  namespace Host {
    namespace Bridge {

      DownloaderBridge::DownloaderBridge(QObject *parent /*= 0*/)
        : QObject(parent)
      {
      }

      DownloaderBridge::~DownloaderBridge()
      {
      }

      void DownloaderBridge::setDownloader(GameDownloadService *downloader)
      {
        this->_downloader = downloader;

        QObject::connect(this->_downloader, &GameDownloadService::started, this, &DownloaderBridge::onStarted);
        QObject::connect(this->_downloader, &GameDownloadService::finished, this, &DownloaderBridge::onFinished);
        QObject::connect(this->_downloader, &GameDownloadService::stopped, this, &DownloaderBridge::onStopped);
        QObject::connect(this->_downloader, &GameDownloadService::stopping, this, &DownloaderBridge::onStopping);
        QObject::connect(this->_downloader, &GameDownloadService::failed, this, &DownloaderBridge::onFailed);
        QObject::connect(this->_downloader, &GameDownloadService::serviceInstalled, this, &DownloaderBridge::onServiceInstalled);
        QObject::connect(this->_downloader, &GameDownloadService::serviceUpdated, this, &DownloaderBridge::onServiceUpdated);
        QObject::connect(this->_downloader, &GameDownloadService::statusMessageChanged, this, &DownloaderBridge::onStatusMessageChanged);
      }

      void DownloaderBridge::setServiceLoader(ServiceLoader *serviceLoader)
      {
        this->_serviceLoader = serviceLoader;
      }

      bool DownloaderBridge::isInProgress(const QString& serviceId)
      {
        Service *service = this->_serviceLoader->getService(serviceId);
        if (!service)
          return false;

        return this->_downloader->isInProgress(service);
      }

      bool DownloaderBridge::isAnyServiceInProgress()
      {
        return this->_downloader->isAnyServiceInProgress();
      }

      bool DownloaderBridge::isInstalled(const QString& serviceId)
      {
        return this->_downloader->isInstalled(serviceId);
      }

      void DownloaderBridge::start(const QString& serviceId, int startType)
      {
        Service *service = this->_serviceLoader->getService(serviceId);
        if (!service)
          return;

        this->_downloader->start(service, static_cast<GGS::GameDownloader::StartType>(startType));
      }

      void DownloaderBridge::stop(const QString& serviceId)
      {
        Service *service = this->_serviceLoader->getService(serviceId);
        if (!service)
          return;

        this->_downloader->stop(service);
      }

      void DownloaderBridge::directoryChanged(const QString& serviceId)
      {
        // undone ����� �������� ��������� ������
        
      }

      void DownloaderBridge::pauseSession()
      {
        this->_downloader->pauseSession();
      }

      void DownloaderBridge::resumeSession()
      {
        this->_downloader->resumeSession();
      }

      void DownloaderBridge::onStarted(const Service *service, GGS::GameDownloader::StartType startType)
      {
        this->started(service->id(), static_cast<int>(startType));
      }

      void DownloaderBridge::onFinished(const Service *service)
      {
        emit this->finished(service->id());
      }

      void DownloaderBridge::onStopped(const Service *service)
      {
        emit this->stopped(service->id());
      }

      void DownloaderBridge::onStopping(const Service *service)
      {
        emit this->stopping(service->id());
      }

      void DownloaderBridge::onFailed(const Service *service)
      {
        emit this->failed(service->id());
      }

      void DownloaderBridge::onServiceInstalled(const Service *service)
      {
        emit this->serviceInstalled(service->id());
      }

      void DownloaderBridge::onServiceUpdated(const Service *service)
      {
        emit this->serviceUpdated(service->id());
      }

      void DownloaderBridge::onStatusMessageChanged(const Service *service, const QString& message)
      {
        emit this->statusMessageChanged(service->id(), message);
      }

      void DownloaderBridge::onTotalProgress(const Service *service, qint8 progress)
      {
        emit this->totalProgress(service->id(), progress);
      }

      void DownloaderBridge::onDownloadProgress(
        const Service *service, qint8 progress, GGS::Libtorrent::EventArgs::ProgressEventArgs args)
      {
        DownloadProgressArgs arg;
        arg.id = args.id();
        arg.status = args.status();
        arg.progress = args.progress();
        
        arg.downloadRate = args.downloadRate();
        arg.uploadRate = args.uploadRate();

        arg.totalWanted = args.totalWanted();
        arg.totalWantedDone = args.totalWantedDone();

        arg.directTotalDownload = args.directTotalDownload();
        arg.peerTotalDownload = args.peerTotalDownload();

        arg.payloadTotalDownload = args.payloadTotalDownload();
        arg.peerPayloadDownloadRate = args.peerPayloadDownloadRate();
        arg.payloadDownloadRate = args.payloadDownloadRate();
        
        arg.directPayloadDownloadRate = args.directPayloadDownloadRate();
        arg.payloadUploadRate = args.payloadUploadRate();
        arg.totalPayloadUpload = args.totalPayloadUpload();
        emit this->downloadProgress(service->id(), progress, arg);
      }

    }
  }
}