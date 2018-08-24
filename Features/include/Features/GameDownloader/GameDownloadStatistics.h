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

      void init(P1::GameDownloader::GameDownloadService* downloader);

    private slots:
      void progressDownloadChanged(
        const P1::Core::Service *service, 
        qint8 progress, 
        P1::Libtorrent::EventArgs::ProgressEventArgs args); 

      void started(const P1::Core::Service *service, P1::GameDownloader::StartType startType);
      void finished(const P1::Core::Service *service);
      void stopped(const P1::Core::Service *service);
      void failed(const P1::Core::Service *service); 

      void saveTimer();
      void externalIpResult(QString ip);

    private:
      P1::GameDownloader::GameDownloadService* _downloader;
      QHash<QString, DownloadStats*> _statisticMap;
      QTimer _timer;
      QMutex _mutex;
      QString _externalIp;
    };

  }
}