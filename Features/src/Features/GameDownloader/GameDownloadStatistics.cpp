/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates. 
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#include <Features/GameDownloader/GameDownloadStatistics.h>
#include <Core/Marketing.h>

#include <QtCore/QVariantMap>
#include <QtCore/QMutexLocker>
#include <QtCore/QDebug>

namespace Features {
  namespace GameDownloader {

    GameDownloadStatistics::GameDownloadStatistics(QObject *parent) : QObject(parent)
    {
    }

    GameDownloadStatistics::~GameDownloadStatistics()
    {
    }

    void GameDownloadStatistics::init(GGS::GameDownloader::GameDownloadService* downloader)
    {
      SIGNAL_CONNECT_CHECK(QObject::connect(downloader, 
        SIGNAL(progressDownloadChanged(QString, qint8, GGS::Libtorrent::EventArgs::ProgressEventArgs)), 
        this, 
        SLOT(progressDownloadChanged(QString, qint8, GGS::Libtorrent::EventArgs::ProgressEventArgs))));

      SIGNAL_CONNECT_CHECK(QObject::connect(
        downloader, SIGNAL(started(const GGS::Core::Service*, GGS::GameDownloader::StartType)), 
        this, SLOT(started(const GGS::Core::Service *, GGS::GameDownloader::StartType))));
      SIGNAL_CONNECT_CHECK(QObject::connect(downloader, SIGNAL(finished(const GGS::Core::Service *)), 
        this, SLOT(finished(const GGS::Core::Service *))));
      SIGNAL_CONNECT_CHECK(QObject::connect(downloader, SIGNAL(stopped(const GGS::Core::Service *)), 
        this, SLOT(stopped(const GGS::Core::Service *))));
      SIGNAL_CONNECT_CHECK(QObject::connect(downloader, SIGNAL(failed(const GGS::Core::Service *)), 
        this, SLOT(failed(const GGS::Core::Service *))));

      this->_timer.setInterval(30000);
      SIGNAL_CONNECT_CHECK(QObject::connect(&this->_timer, SIGNAL(timeout()), this, SLOT(saveTimer())));
      this->_timer.start();
    }

    void GameDownloadStatistics::progressDownloadChanged(
      QString serviceId,
      qint8 progress,
      GGS::Libtorrent::EventArgs::ProgressEventArgs args)
    {
      if (!this->_mutex.tryLock())
        return;

      if (this->_statisticMap.contains(serviceId)) {
        DownloadStats *stats = this->_statisticMap[serviceId];
        stats->update(args.downloadRate(), args.uploadRate(), args.totalWanted());
      }

      this->_mutex.unlock();
    }

    void GameDownloadStatistics::started(const GGS::Core::Service *service, GGS::GameDownloader::StartType startType)
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

    void GameDownloadStatistics::finished(const GGS::Core::Service *service)
    {
      QMutexLocker locker(&this->_mutex);

      using namespace GGS::Core;
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
        " DownloadPath" << service->installPath();

      Marketing::sendOnceByService(Marketing::SendDownloadServiceInfo, service->id(), args);
      this->_statisticMap.remove(service->id());
    }

    void GameDownloadStatistics::stopped(const GGS::Core::Service *service)
    {
      QMutexLocker locker(&this->_mutex);

      if (!this->_statisticMap.contains(service->id()))
        return;

      DownloadStats *stats = this->_statisticMap[service->id()];
      stats->stop();
    }

    void GameDownloadStatistics::failed(const GGS::Core::Service *service)
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

  }
}
