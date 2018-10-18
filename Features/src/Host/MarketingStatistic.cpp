#include <Host/MarketingStatistic.h>

#include <Host/Proxy/DownloaderProxy.h>
#include <Host/Proxy/GameExecutorProxy.h>

#include <Settings/Settings.h>

#include <Host/GameDownloader/DownloaderSettingsRoulette.h>

#include <Core/Marketing.h>

#include <QtCore/QDebug>

using P1::Host::Proxy::DownloaderProxy;
using P1::Host::Proxy::GameExecutorProxy;

using P1::Core::Marketing;
using P1::Core::Service;

using P1::RestApi::ProtocolOneCredential;

namespace P1 {
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

    void MarketingStatistic::onGameDownloadStarted(const Service *service)
    {
      DownloaderProxy *downloader = qobject_cast<DownloaderProxy *>(QObject::sender());
      if (!downloader)
        return;

      QString serviceId(service->id());
      QVariantMap params;
      this->setCredential(params, downloader->credential(serviceId));
      Marketing::send(Marketing::StartDownloadService, serviceId, params);

      QVariantMap paramsOnce;
      this->setCredential(paramsOnce, downloader->credential(serviceId));

      Marketing::sendOnceByService(Marketing::FirstStartDownloadService, serviceId, paramsOnce);
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

    void MarketingStatistic::setCredential(QVariantMap &params, const ProtocolOneCredential &credetial)
    {
      params["userId"] = credetial.userId();
      //params["overrideUserId"] = credetial.userId();
      //params["overrideAppKey"] = credetial.appKey();
    }

  }
}
