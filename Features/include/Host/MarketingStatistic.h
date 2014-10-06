#pragma once

#include <RestApi/GameNetCredential.h>

#include <QtCore/QObject>
#include <QtCore/QString>

namespace GameNet {
  namespace Host {

    namespace Proxy {
      class GameExecutorProxy;
      class DownloaderProxy;
    }
    
    class MarketingStatistic : public QObject
    {
    public:
      explicit MarketingStatistic(QObject *parent = 0);
      virtual ~MarketingStatistic();

      void init();

      void setDownloader(Proxy::DownloaderProxy *value);
      void setExecutor(Proxy::GameExecutorProxy *value);

    private:
      void setCredential(QVariantMap &params, const GGS::RestApi::GameNetCredential &credetial);

      void onGameStarted(
        const QString& serviceId, 
        const GGS::RestApi::GameNetCredential credetial);

      void onGameFinished(
        const QString& serviceId, 
        const GGS::RestApi::GameNetCredential credetial);

      void onGameDownloadStarted(
        const QString& serviceId, 
        const GGS::RestApi::GameNetCredential credetial);

      void onGameTorrentDownloadFinished(
        const QString& serviceId, 
        const GGS::RestApi::GameNetCredential credetial);

      void onGameDownloadFinished(
        const QString& serviceId, 
        const GGS::RestApi::GameNetCredential credetial);

      Proxy::DownloaderProxy *_downloader;
      Proxy::GameExecutorProxy *_executor;

    };

  }
}
