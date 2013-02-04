/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates. 
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#pragma once

#include <Features/GameDownloader/DownloadStats.h>

#include <GameDownloader/GameDownloadService.h>
#include <LibtorrentWrapper/EventArgs/ProgressEventArgs.h>

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QTimer>
#include <QtCore/QMutex>

namespace Features {
  namespace GameDownloader {
    class GameDownloadStatistics : public QObject
    {
      Q_OBJECT
    public:
      GameDownloadStatistics(QObject *parent = 0);
      ~GameDownloadStatistics();

      void init(GGS::GameDownloader::GameDownloadService* downloader);

    private slots:
      void progressDownloadChanged(QString serviceId, qint8 progress, GGS::Libtorrent::EventArgs::ProgressEventArgs args); 

      void started(const GGS::Core::Service *service, GGS::GameDownloader::StartType startType);
      void finished(const GGS::Core::Service *service);
      void stopped(const GGS::Core::Service *service);
      void failed(const GGS::Core::Service *service); 

      void saveTimer();
      void externalIpResult(QString ip);

    private:
      GGS::GameDownloader::GameDownloadService* _downloader;
      QHash<QString, DownloadStats*> _statisticMap;
      QTimer _timer;
      QMutex _mutex;
      QString _externalIp;
    };

  }
}