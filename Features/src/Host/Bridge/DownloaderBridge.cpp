// INFO windows.h can't be include before QDbusConnection
#include <QtDBus/QDBusConnection>

#include <Host/Bridge/DownloaderBridge.h>

#include <Host/ServiceProcess/ServiceLoader.h>

#include <Host/Proxy/DownloaderProxy.h>

#include <Core/Service.h>

using GGS::GameDownloader::GameDownloadService;
using GGS::Core::Service;
using GGS::Libtorrent::EventArgs::ProgressEventArgs;
using GameNet::Host::Proxy::DownloaderProxy;

namespace GameNet {
  namespace Host {
    namespace Bridge {

      bool operator==(const DownloadProgressArgs &lhs, const DownloadProgressArgs &rhs)
      {
        return lhs.id == rhs.id &&
          lhs.status == rhs.status &&
          lhs.progress == rhs.progress &&
          lhs.downloadRate == rhs.downloadRate &&
          lhs.uploadRate == rhs.uploadRate &&
          lhs.totalWanted == rhs.totalWanted &&
          lhs.totalWantedDone == rhs.totalWantedDone &&
          lhs.directTotalDownload == rhs.directTotalDownload &&
          lhs.peerTotalDownload == rhs.peerTotalDownload &&
          lhs.payloadTotalDownload == rhs.payloadTotalDownload &&
          lhs.peerPayloadDownloadRate == rhs.peerPayloadDownloadRate &&
          lhs.payloadDownloadRate == rhs.payloadDownloadRate &&
          lhs.directPayloadDownloadRate == rhs.directPayloadDownloadRate &&
          lhs.payloadUploadRate == rhs.payloadUploadRate &&
          lhs.totalPayloadUpload == rhs.totalPayloadUpload; 
      }

      DownloaderBridge::DownloaderBridge(QObject *parent /*= 0*/)
        : QObject(parent)
        , _downloader(nullptr)
        , _serviceLoader(nullptr)
      {
      }

      DownloaderBridge::~DownloaderBridge()
      {
      }

      void DownloaderBridge::setDownloader(DownloaderProxy *downloader)
      {
        Q_ASSERT(downloader);
        this->_downloader = downloader;

        QObject::connect(this->_downloader, &DownloaderProxy::started, this, &DownloaderBridge::onStarted);
        QObject::connect(this->_downloader, &DownloaderProxy::finished, this, &DownloaderBridge::onFinished);
        QObject::connect(this->_downloader, &DownloaderProxy::stopped, this, &DownloaderBridge::onStopped);
        QObject::connect(this->_downloader, &DownloaderProxy::stopping, this, &DownloaderBridge::onStopping);
        QObject::connect(this->_downloader, &DownloaderProxy::failed, this, &DownloaderBridge::onFailed);
        QObject::connect(this->_downloader, &DownloaderProxy::serviceInstalled, this, &DownloaderBridge::onServiceInstalled);
        QObject::connect(this->_downloader, &DownloaderProxy::serviceUninstalled, this, &DownloaderBridge::onServiceUninstalled);
        QObject::connect(this->_downloader, &DownloaderProxy::serviceUpdated, this, &DownloaderBridge::onServiceUpdated);
        QObject::connect(this->_downloader, &DownloaderProxy::statusMessageChanged, this, &DownloaderBridge::onStatusMessageChanged);
        QObject::connect(this->_downloader, &DownloaderProxy::totalProgressChanged, this, &DownloaderBridge::onTotalProgress);
        QObject::connect(this->_downloader, &DownloaderProxy::downloadProgressChanged, this, &DownloaderBridge::onDownloadProgress);
      }

      void DownloaderBridge::setServiceLoader(ServiceProcess::ServiceLoader *serviceLoader)
      {
        Q_ASSERT(serviceLoader);
        this->_serviceLoader = serviceLoader;
      }

      bool DownloaderBridge::isInProgress(const QString& serviceId) const
      {
        Q_ASSERT(this->_serviceLoader);
        Q_ASSERT(this->_downloader);
        Service *service = this->_serviceLoader->getService(serviceId);
        if (!service)
          return false;

        return this->_downloader->isInProgress(service);
      }

      bool DownloaderBridge::isAnyServiceInProgress() const
      {
        Q_ASSERT(this->_downloader);
        return this->_downloader->isAnyServiceInProgress();
      }

      bool DownloaderBridge::isInstalled(const QString& serviceId) const
      {
        Q_ASSERT(this->_downloader);
        return this->_downloader->isInstalled(serviceId);
      }

      void DownloaderBridge::start(const QString& serviceId, int startType)
      {
        Q_ASSERT(this->_serviceLoader);
        Q_ASSERT(this->_downloader);
        Service *service = this->_serviceLoader->getService(serviceId);
        if (!service)
          return;

        this->_downloader->start(service, static_cast<GGS::GameDownloader::StartType>(startType));
      }

      void DownloaderBridge::stop(const QString& serviceId)
      {
        Q_ASSERT(this->_serviceLoader);
        Q_ASSERT(this->_downloader);
        Service *service = this->_serviceLoader->getService(serviceId);
        if (!service)
          return;

        this->_downloader->stop(service);
      }

      void DownloaderBridge::pauseSession()
      {
        Q_ASSERT(this->_downloader);
        this->_downloader->pauseSession();
      }

      void DownloaderBridge::resumeSession()
      {
        Q_ASSERT(this->_downloader);
        this->_downloader->resumeSession();
      }

      void DownloaderBridge::onStarted(const Service *service, GGS::GameDownloader::StartType startType)
      {
        Q_ASSERT(service);
        this->started(service->id(), static_cast<int>(startType));
      }

      void DownloaderBridge::onFinished(const Service *service)
      {
        Q_ASSERT(service);
        emit this->finished(service->id());
      }

      void DownloaderBridge::onStopped(const Service *service)
      {
        Q_ASSERT(service);
        emit this->stopped(service->id());
      }

      void DownloaderBridge::onStopping(const Service *service)
      {
        Q_ASSERT(service);
        emit this->stopping(service->id());
      }

      void DownloaderBridge::onFailed(const Service *service)
      {
        Q_ASSERT(service);
        emit this->failed(service->id());
      }

      void DownloaderBridge::onServiceInstalled(const Service *service)
      {
        Q_ASSERT(service);
        emit this->serviceInstalled(service->id());
      }

      void DownloaderBridge::onServiceUninstalled(const Service *service)
      {
        Q_ASSERT(service);
        emit this->serviceUninstalled(service->id());
      }

      void DownloaderBridge::onServiceUpdated(const Service *service)
      {
        Q_ASSERT(service);
        emit this->serviceUpdated(service->id());
      }

      void DownloaderBridge::onStatusMessageChanged(const Service *service, const QString& message)
      {
        Q_ASSERT(service);
        emit this->statusMessageChanged(service->id(), message);
      }

      void DownloaderBridge::onTotalProgress(const Service *service, qint8 progress)
      {
        Q_ASSERT(service);
        emit this->totalProgress(service->id(), progress);
      }

      void DownloaderBridge::onAccessRequired(const Service *service)
      {
        Q_ASSERT(service);
        emit this->accessRequired(service->id());
      }

      void DownloaderBridge::onDownloadProgress(
        const Service *service, qint8 progress, const GGS::Libtorrent::EventArgs::ProgressEventArgs& args)
      {
        Q_ASSERT(service);
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

QDBusArgument& operator<<(QDBusArgument &argument, const GameNet::Host::Bridge::DownloadProgressArgs& arg)
{
  argument.beginStructure();
  argument 
    << arg.id
    << arg.status
    << arg.progress

    << arg.downloadRate
    << arg.uploadRate

    << arg.totalWanted
    << arg.totalWantedDone

    << arg.directTotalDownload
    << arg.peerTotalDownload

    << arg.payloadTotalDownload
    << arg.peerPayloadDownloadRate
    << arg.payloadDownloadRate
    << arg.directPayloadDownloadRate
    << arg.payloadUploadRate
    << arg.totalPayloadUpload;

  argument.endStructure();
  return argument;
}

const QDBusArgument& operator>>(const QDBusArgument &argument, GameNet::Host::Bridge::DownloadProgressArgs& arg)
{
  argument.beginStructure();
  argument 
    >> arg.id
    >> arg.status
    >> arg.progress

    >> arg.downloadRate
    >> arg.uploadRate

    >> arg.totalWanted
    >> arg.totalWantedDone

    >> arg.directTotalDownload
    >> arg.peerTotalDownload

    >> arg.payloadTotalDownload
    >> arg.peerPayloadDownloadRate
    >> arg.payloadDownloadRate
    >> arg.directPayloadDownloadRate
    >> arg.payloadUploadRate
    >> arg.totalPayloadUpload;
  argument.endStructure();
  return argument;
}
