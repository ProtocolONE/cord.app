#include <Features/GameDownloader/GameDownloadStatistics.h>
#include <Features/GameDownloader/ExternalIpAddress.h>
#include <Core/Marketing.h>
#include <Core/Service.h>

#include <QtCore/QVariantMap>
#include <QtCore/QMutexLocker>
#include <QtCore/QDebug>

using P1::GameDownloader::GameDownloadService;

namespace Features {
  namespace GameDownloader {

    GameDownloadStatistics::GameDownloadStatistics(QObject *parent) : QObject(parent)
    {
    }

    GameDownloadStatistics::~GameDownloadStatistics()
    {
    }

    void GameDownloadStatistics::init(P1::GameDownloader::GameDownloadService* downloader)
    {
      this->_downloader = downloader;

      QObject::connect(downloader, &GameDownloadService::downloadProgressChanged,
        this, &GameDownloadStatistics::progressDownloadChanged);

      QObject::connect(downloader, &GameDownloadService::started,
        this, &GameDownloadStatistics::started);

      QObject::connect(downloader, &GameDownloadService::finished,
        this, &GameDownloadStatistics::finished);

      QObject::connect(downloader, &GameDownloadService::stopped,
        this, &GameDownloadStatistics::stopped);

      QObject::connect(downloader, &GameDownloadService::failed,
        this, &GameDownloadStatistics::failed);

      QObject::connect(&this->_timer, &QTimer::timeout, this, &GameDownloadStatistics::saveTimer);
      this->_timer.start(30000);

      ExternalIpAddress *externalIp = new ExternalIpAddress(this);
      QObject::connect(externalIp, &ExternalIpAddress::result, 
        this, &GameDownloadStatistics::externalIpResult);

      externalIp->execute();
    }

    void GameDownloadStatistics::progressDownloadChanged(
      const P1::Core::Service *service,
      qint8 progress,
      P1::Libtorrent::EventArgs::ProgressEventArgs args)
    {
      if (!this->_mutex.tryLock())
        return;

      QString serviceId(service->id());
      if (this->_statisticMap.contains(serviceId)) {
        DownloadStats *stats = this->_statisticMap[serviceId];
        stats->update(args.downloadRate(), args.uploadRate(), args.totalWanted());
      }

      this->_mutex.unlock();
    }

    void GameDownloadStatistics::started(const P1::Core::Service *service, P1::GameDownloader::StartType startType)
    {
      QMutexLocker locker(&this->_mutex);

      if (this->_downloader->isInstalled(service))
        return;

      DownloadStats *stats = NULL;
      QString id = service->id();
      if (this->_statisticMap.contains(id)) {
        stats = this->_statisticMap[id];
      } else {
        stats = new DownloadStats(this);
        stats->setId(id);
        this->_statisticMap[id] = stats;
        stats->load();
      }

      stats->start();
    }

    void GameDownloadStatistics::finished(const P1::Core::Service *service)
    {
      QMutexLocker locker(&this->_mutex);

      using namespace P1::Core;
      if (!this->_statisticMap.contains(service->id()))
        return;

      DownloadStats *stats = this->_statisticMap[service->id()];
      stats->stop();

      QVariantMap args;
      args["MaxSpeed"] = stats->maxDownloadSpeed();
      args["MaxUploadSpeed"] = stats->maxUploadSpeed();
      
      int downloadTime = stats->downloadTime();
      args["DownloadServiceTime"] = downloadTime;

      int avgSpeed = 0;
      if (downloadTime > 0) {
        avgSpeed = stats->totalSize() / downloadTime;
        args["AvgSpeed"] = avgSpeed;
      }

      args["GnaRestartCount"] = stats->applicationRestartCount();
      args["DownloadPath"] = service->installPath();

      DEBUG_LOG << "Download stats " << service->id() << 
        " MaxSpeed" << stats->maxDownloadSpeed() << 
        " MaxUploadSpeed" << stats->maxUploadSpeed() << 
        " DownloadServiceTime" << downloadTime << 
        " AvgSpeed" << avgSpeed << 
        " GnaRestartCount" << stats->applicationRestartCount() << 
        " DownloadPath" << service->installPath() <<
        " Ip" << this->_externalIp;

      if (!this->_externalIp.isEmpty())
        args["ExternalIp"] = this->_externalIp;

      Marketing::sendOnceByService(Marketing::SendDownloadServiceInfo, service->id(), args);
      this->_statisticMap.remove(service->id());
    }

    void GameDownloadStatistics::stopped(const P1::Core::Service *service)
    {
      QMutexLocker locker(&this->_mutex);

      if (!this->_statisticMap.contains(service->id()))
        return;

      DownloadStats *stats = this->_statisticMap[service->id()];
      stats->stop();
    }

    void GameDownloadStatistics::failed(const P1::Core::Service *service)
    {
      QMutexLocker locker(&this->_mutex);

      if (!this->_statisticMap.contains(service->id()))
        return;

      DownloadStats *stats = this->_statisticMap[service->id()];
      stats->stop();
    }

    void GameDownloadStatistics::saveTimer()
    {
      QMutexLocker locker(&this->_mutex);

      Q_FOREACH(QString id, this->_statisticMap.keys()) {
        DownloadStats *stats = this->_statisticMap[id];
        stats->update(0, 0, 0);
        stats->save();
      }
    }

    void GameDownloadStatistics::externalIpResult(QString ip)
    {
      ExternalIpAddress *sender = qobject_cast<ExternalIpAddress *>(QObject::sender());
      if (sender)
        sender->deleteLater();

      this->_externalIp = ip;
    }

  }
}
