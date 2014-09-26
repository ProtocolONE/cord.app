#pragma once

#include <GameDownloader\GameDownloadService.h>

namespace GGS {
  namespace Core {
    class Service;
  }
}

class GameDownloadServiceFixture : public GGS::GameDownloader::GameDownloadService
{
  Q_OBJECT
public:
  GameDownloadServiceFixture();
  ~GameDownloadServiceFixture();

  void setService(GGS::Core::Service *service);

  void setListeningPort(unsigned short port){};
  void setTorrentConfigDirectoryPath(const QString& path){};
  void changeListeningPort(unsigned short port){};
  void setUploadRateLimit(int bytesPerSecond){};
  void setDownloadRateLimit(int bytesPerSecond){};
  void setMaxConnection(int maxConnection){};
  void setSeedEnabled(bool value){};

  void emitStarted(GGS::GameDownloader::StartType startType);
  void emitFinished();
  void emitStopped();
  void emitStopping();
  void emitFailed();
  void emitShutdownCompleted();
  void emitServiceInstalled();
  void emitServiceUpdated();
  void emitStatusMessageChanged(const QString &msg);
  void emitProgressChanged(quint8 progress);
  void emitProgressDownloadChanged(qint8 progress, GGS::Libtorrent::EventArgs::ProgressEventArgs args);
  void emitProgressExtractionChanged(qint8 progress, qint64 current, qint64 total);
  void emitTotalProgressChanged(quint8 progress);
  void emitDownloadProgressChanged(quint8 progress, GGS::Libtorrent::EventArgs::ProgressEventArgs args);
  void emitListeningPortChanged(unsigned short port);

signals:
  /*void started(const GGS::Core::Service *service, GGS::GameDownloader::StartType startType);
  void finished(const GGS::Core::Service *service);
  void stopped(const GGS::Core::Service *service);
  void stopping(const GGS::Core::Service *service);
  void failed(const GGS::Core::Service *service);
  void shutdownCompleted();
  void serviceInstalled(const GGS::Core::Service *service);
  void serviceUpdated(const GGS::Core::Service *service);

  void statusMessageChanged(const GGS::Core::Service *service, const QString& message);

  void progressChanged(QString serviceId, qint8 progress);
  void progressDownloadChanged(QString serviceId, qint8 progress, GGS::Libtorrent::EventArgs::ProgressEventArgs args);
  void progressExtractionChanged(QString serviceId, qint8 progress, qint64 current, qint64 total);

  void totalProgressChanged(const GGS::Core::Service *service, qint8 progress);
  void downloadProgressChanged(
    const GGS::Core::Service *service, 
    qint8 progress, 
    GGS::Libtorrent::EventArgs::ProgressEventArgs args);

  void listeningPortChanged(unsigned short port);*/

private:
  GGS::Core::Service *_service;
};
