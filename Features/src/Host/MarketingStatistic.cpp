#include <Host/MarketingStatistic.h>

#include <Host/Proxy/DownloaderProxy.h>
#include <Host/Proxy/GameExecutorProxy.h>

#include <Core/Marketing.h>

using GameNet::Host::Proxy::DownloaderProxy;
using GameNet::Host::Proxy::GameExecutorProxy;

using GGS::Core::Marketing;

using GGS::RestApi::GameNetCredential;

namespace GameNet {
  namespace Host {

    MarketingStatistic::MarketingStatistic(QObject *parent /*= 0*/)
      : QObject(parent)
      , _downloader(nullptr)
      , _executor(nullptr)
    {
    }

    MarketingStatistic::~MarketingStatistic()
    {
    }

    void MarketingStatistic::init()
    {
      Q_ASSERT(this->_downloader);
      Q_ASSERT(this->_executor);

      QObject::connect(this->_executor, &GameExecutorProxy::startedWithCredential,
        this, &MarketingStatistic::onGameStarted);

      QObject::connect(this->_executor, &GameExecutorProxy::finishedWithCredential,
        this, &MarketingStatistic::onGameFinished);
        
      QObject::connect(this->_downloader, &DownloaderProxy::startedWithCredential,
        this, &MarketingStatistic::onGameDownloadStarted);

      QObject::connect(this->_downloader, &DownloaderProxy::finishedDownloadingWithCredential,
        this, &MarketingStatistic::onGameTorrentDownloadFinished);

      QObject::connect(this->_downloader, &DownloaderProxy::finishedWithCredential,
        this, &MarketingStatistic::onGameDownloadFinished);
    }

    void MarketingStatistic::setDownloader(Proxy::DownloaderProxy *value)
    {
      Q_ASSERT(value);
      this->_downloader = value;
    }

    void MarketingStatistic::setExecutor(Proxy::GameExecutorProxy *value)
    {
      Q_ASSERT(value);
      this->_executor = value;
    }

    void MarketingStatistic::onGameStarted(
      const QString& serviceId, 
      const GGS::RestApi::GameNetCredential credetial)
    {
      QVariantMap params;
      this->setCredential(params, credetial);
      
      Marketing::sendOnceByService(Marketing::FirstRunService, serviceId, params);
      Marketing::send(Marketing::StartService, serviceId, params);
    }

    void MarketingStatistic::onGameFinished(
      const QString& serviceId, 
      const GGS::RestApi::GameNetCredential credetial)
    {
      QVariantMap params;
      this->setCredential(params, credetial);
      Marketing::send(Marketing::CloseService, serviceId, params);
    }

    void MarketingStatistic::onGameDownloadStarted(
      const QString& serviceId,
      const GameNetCredential credetial)
    {
      QVariantMap params;
      this->setCredential(params, credetial);
      Marketing::send(Marketing::StartDownloadService, serviceId, params);
    }

    void MarketingStatistic::onGameTorrentDownloadFinished(
      const QString& serviceId,
      const GameNetCredential credetial)
    {
      QVariantMap params;
      this->setCredential(params, credetial);
      Marketing::sendOnceByService(Marketing::FinishDownloadService, serviceId, params);
    }

    void MarketingStatistic::onGameDownloadFinished(
      const QString& serviceId,
      const GameNetCredential credetial)
    {
      QVariantMap params;
      this->setCredential(params, credetial);
      Marketing::sendOnceByService(Marketing::FinishInstallService, serviceId, params);
    }

    void MarketingStatistic::setCredential(QVariantMap &params, const GameNetCredential &credetial)
    {
      params["overrideUserId"] = credetial.userId();
      params["overrideAppKey"] = credetial.appKey();
    }

  }
}