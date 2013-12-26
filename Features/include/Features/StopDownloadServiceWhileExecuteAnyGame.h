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

#include <GameDownloader/StartType>

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QMutex>
#include <QtCore/QSet>

namespace GGS {
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
    void onServiceStartDownload(const GGS::Core::Service *service, GGS::GameDownloader::StartType startType);
    void onServiceFinishDownload(const GGS::Core::Service *service);
    void onGameExecuted(const QString& id);
    void onGameFinished(const QString& id);

  signals:
    void downloadStopRequest(const GGS::Core::Service *service);
    void downloadStartRequest(const GGS::Core::Service *service, GGS::GameDownloader::StartType startType);
    void torrentSessionPauseRequest();
    void torrentSessionResumeRequest();

  private:
    QMutex _mutex;
    QHash<const GGS::Core::Service *, GGS::GameDownloader::StartType> _downloadingServices;
    QHash<const GGS::Core::Service *, GGS::GameDownloader::StartType> _stoppedServices;
    QSet<QString> _ignoreList;
  };

}
