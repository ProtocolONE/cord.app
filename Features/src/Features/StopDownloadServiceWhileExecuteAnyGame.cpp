#include <Features/StopDownloadServiceWhileExecuteAnyGame.h>

#include <Core/Service.h>

#include <QtCore/QMutexLocker>

namespace Features {

  StopDownloadServiceWhileExecuteAnyGame::StopDownloadServiceWhileExecuteAnyGame(QObject *parent)
    : QObject(parent)
  {
  }

  StopDownloadServiceWhileExecuteAnyGame::~StopDownloadServiceWhileExecuteAnyGame()
  {
  }

  void StopDownloadServiceWhileExecuteAnyGame::onServiceStartDownload(const P1::Core::Service *service, P1::GameDownloader::StartType startType)
  {
    QMutexLocker locker(&this->_mutex);
    this->_downloadingServices[service] = startType;
  }

  void StopDownloadServiceWhileExecuteAnyGame::onServiceFinishDownload(const P1::Core::Service *service)
  {
    QMutexLocker locker(&this->_mutex);
    this->_downloadingServices.remove(service);
  }

  void StopDownloadServiceWhileExecuteAnyGame::onGameExecuted(const QString& id)
  {
    QMutexLocker locker(&this->_mutex);
    if (this->_ignoreList.contains(id))
      return;

    emit this->disableDownloadUnlock();
    Q_FOREACH(const P1::Core::Service* service, this->_downloadingServices.keys()) {
      emit this->downloadStopRequest(service);
      this->_stoppedServices[service] = this->_downloadingServices[service];
    }

    emit this->torrentSessionPauseRequest();
  }

  void StopDownloadServiceWhileExecuteAnyGame::onGameFinished(const QString& id)
  {
    QMutexLocker locker(&this->_mutex);
    if (this->_ignoreList.contains(id))
      return;

    emit this->enableDownloadUnlock();

    Q_FOREACH(const P1::Core::Service* service, this->_stoppedServices.keys()) {
      emit this->downloadStartRequest(service, this->_stoppedServices[service]);
    }

    this->_stoppedServices.clear();
    emit this->torrentSessionResumeRequest();
  }

  void StopDownloadServiceWhileExecuteAnyGame::ignoreService(const QString& id)
  {
    this->_ignoreList.insert(id);
  }

}
