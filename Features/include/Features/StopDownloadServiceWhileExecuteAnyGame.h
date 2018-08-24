#pragma once

#include <GameDownloader/StartType.h>

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QMutex>
#include <QtCore/QSet>

namespace P1 {
  namespace Core {
    class Service;
  }
}
   
namespace Features {

  class StopDownloadServiceWhileExecuteAnyGame : public QObject
  {
    Q_OBJECT
  public:
    StopDownloadServiceWhileExecuteAnyGame(QObject *parent = 0);
    ~StopDownloadServiceWhileExecuteAnyGame();

    void ignoreService(const QString& id);

  public slots:
    void onServiceStartDownload(const P1::Core::Service *service, P1::GameDownloader::StartType startType);
    void onServiceFinishDownload(const P1::Core::Service *service);
    void onGameExecuted(const QString& id);
    void onGameFinished(const QString& id);

  signals:
    void downloadStopRequest(const P1::Core::Service *service);
    void downloadStartRequest(const P1::Core::Service *service, P1::GameDownloader::StartType startType);
    void torrentSessionPauseRequest();
    void torrentSessionResumeRequest();

    void disableDownloadUnlock();
    void enableDownloadUnlock();

  private:
    QMutex _mutex;
    QHash<const P1::Core::Service *, P1::GameDownloader::StartType> _downloadingServices;
    QHash<const P1::Core::Service *, P1::GameDownloader::StartType> _stoppedServices;
    QSet<QString> _ignoreList;
  };

}
