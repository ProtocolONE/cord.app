#include <Host/Proxy/DownloaderProxy.h>

#include <Host/Application.h>

#include <Core/Service.h>

#include <RestApi/RestApiManager.h>

using GGS::GameDownloader::GameDownloadService;
using GGS::RestApi::GameNetCredential;
using GGS::Core::Service;

namespace GameNet {
  namespace Host {
    namespace Proxy {
      
      DownloaderProxy::DownloaderProxy(QObject *parent /*= 0*/)
        : GameDownloadService(parent)
        , _application(nullptr)
      {
      }

      DownloaderProxy::~DownloaderProxy()
      {
      }

      void DownloaderProxy::setApplication(Application* value)
      {
        Q_ASSERT(value);
        this->_application = value;
      }

      void DownloaderProxy::init()
      {
        Q_ASSERT(this->_application);
        this->GameDownloadService::init();

        QObject::connect(this, &GameDownloadService::started,
          this, &DownloaderProxy::internalStarted);

        QObject::connect(this, &GameDownloadService::finishedDownloading,
          this, &DownloaderProxy::internalDownloaded);

        QObject::connect(this, &GameDownloadService::finished,
          this, &DownloaderProxy::internalFinished);
      }

      void DownloaderProxy::startWithName(
        const QString& connectionName, 
        const Service *service, 
        GGS::GameDownloader::StartType startType)
      {
        Q_ASSERT(this->_application);
        GameNetCredential credential = this->_application->credential(connectionName);
        this->_credentialMap[service->id()] = credential;
        this->start(service, startType);
      }

      void DownloaderProxy::internalStarted(const GGS::Core::Service* service)
      {
        const QString& id = service->id();
        GameNetCredential credential;
        if (this->_credentialMap.contains(id))
          credential = this->_credentialMap[id];

        emit this->startedWithCredential(id, credential);
      }

      void DownloaderProxy::internalDownloaded(const GGS::Core::Service* service)
      {
        const QString& id = service->id();
        GameNetCredential credential;
        if (this->_credentialMap.contains(id))
          credential = this->_credentialMap[id];

        emit this->finishedDownloadingWithCredential(id, credential);
      }

      void DownloaderProxy::internalFinished(const GGS::Core::Service* service)
      {
        const QString& id = service->id();
        GameNetCredential credential;
        if (this->_credentialMap.contains(id))
          credential = this->_credentialMap[id];

        this->_credentialMap.remove(id);
        emit this->finishedWithCredential(id, credential);
      }

    }
  }
}
