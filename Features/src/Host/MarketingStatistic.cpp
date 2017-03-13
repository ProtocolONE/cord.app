#include <Host/MarketingStatistic.h>

#include <Host/Proxy/DownloaderProxy.h>
#include <Host/Proxy/GameExecutorProxy.h>

#include <Core/Marketing.h>

#include <QtCore/QDebug>

using GameNet::Host::Proxy::DownloaderProxy;
using GameNet::Host::Proxy::GameExecutorProxy;

using GGS::Core::Marketing;
using GGS::Core::Service;

using GGS::RestApi::GameNetCredential;

namespace GameNet {
  namespace Host {

    MarketingStatistic::MarketingStatistic(QObject *parent /*= 0*/)
      : QObject(parent)
    {
    }

    MarketingStatistic::~MarketingStatistic()
    {
    }

    void MarketingStatistic::onServiceStarted(const QString& serviceId)
    {
      GameExecutorProxy *executor = qobject_cast<GameExecutorProxy *>(QObject::sender());
      if (!executor)
        return;

      QVariantMap params;
      this->setCredential(params, executor->gameCredential(serviceId));

      Marketing::sendOnceByService(Marketing::FirstRunService, serviceId, params);
      Marketing::send(Marketing::StartService, serviceId, params);
    }

    void MarketingStatistic::onServiceFinished(const QString& serviceId, int finishState)
    {
      GameExecutorProxy *executor = qobject_cast<GameExecutorProxy *>(QObject::sender());
      if (!executor)
        return;

      QVariantMap params;
      this->setCredential(params, executor->gameCredential(serviceId));
      Marketing::send(Marketing::CloseService, serviceId, params);
    }

    void MarketingStatistic::onSecondServiceStarted(const QString& serviceId)
    {
      GameExecutorProxy *executor = qobject_cast<GameExecutorProxy *>(QObject::sender());
      if (!executor)
        return;

      QVariantMap params;
      this->setCredential(params, executor->secondGameCredential(serviceId));

      Marketing::sendOnceByService(Marketing::FirstRunService, serviceId, params);
      Marketing::send(Marketing::StartService, serviceId, params);
    }

    void MarketingStatistic::onSecondServiceFinished(const QString& serviceId, int finishState)
    {
      GameExecutorProxy *executor = qobject_cast<GameExecutorProxy *>(QObject::sender());
      if (!executor)
        return;

      QVariantMap params;
      this->setCredential(params, executor->secondGameCredential(serviceId));
      Marketing::send(Marketing::CloseService, serviceId, params);
    }

    void MarketingStatistic::onGameDownloadStarted(const Service *service)
    {
      DownloaderProxy *downloader = qobject_cast<DownloaderProxy *>(QObject::sender());
      if (!downloader)
        return;

      QString serviceId(service->id());
      QVariantMap params;
      this->setCredential(params, downloader->credential(serviceId));
      Marketing::send(Marketing::StartDownloadService, serviceId, params);
      Marketing::sendOnceByService(Marketing::FirstStartDownloadService, serviceId, params);
    }

    void MarketingStatistic::onGameTorrentDownloadFinished(const Service *service)
    {
      DownloaderProxy *downloader = qobject_cast<DownloaderProxy *>(QObject::sender());
      if (!downloader)
        return;

      QString serviceId(service->id());
      QVariantMap params;
      this->setCredential(params, downloader->credential(serviceId));
      Marketing::sendOnceByService(Marketing::FinishDownloadService, serviceId, params);
    }

    void MarketingStatistic::onGameDownloadFinished(const Service *service)
    {
      DownloaderProxy *downloader = qobject_cast<DownloaderProxy *>(QObject::sender());
      if (!downloader)
        return;

      QString serviceId(service->id());
      QVariantMap params;
      this->setCredential(params, downloader->credential(serviceId));
      Marketing::sendOnceByService(Marketing::FinishInstallService, serviceId, params);
    }

    void MarketingStatistic::setCredential(QVariantMap &params, const GameNetCredential &credetial)
    {
      params["userId"] = credetial.userId();
      params["overrideUserId"] = credetial.userId();
      params["overrideAppKey"] = credetial.appKey();
    }
  }
}