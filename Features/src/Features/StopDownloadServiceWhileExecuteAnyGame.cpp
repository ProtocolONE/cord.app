/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates. 
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/
#include <Features/StopDownloadServiceWhileExecuteAnyGame.h>

#include <Core/Service>

#include <QtCore/QMutexLocker>

namespace Features {

  StopDownloadServiceWhileExecuteAnyGame::StopDownloadServiceWhileExecuteAnyGame(QObject *parent)
    : QObject(parent)
  {
  }

  StopDownloadServiceWhileExecuteAnyGame::~StopDownloadServiceWhileExecuteAnyGame()
  {
  }

  void StopDownloadServiceWhileExecuteAnyGame::onServiceStartDownload(const GGS::Core::Service *service, GGS::GameDownloader::StartType startType)
  {
    QMutexLocker locker(&this->_mutex);
    this->_downloadingServices[service] = startType;
  }

  void StopDownloadServiceWhileExecuteAnyGame::onServiceFinishDownload(const GGS::Core::Service *service)
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
    Q_FOREACH(const GGS::Core::Service* service, this->_downloadingServices.keys()) {
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

    Q_FOREACH(const GGS::Core::Service* service, this->_stoppedServices.keys()) {
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
