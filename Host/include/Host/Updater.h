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

#include <UpdateSystem/CheckUpdateHelper.h>

#include <QtCore/QTimer>
#include <QtCore/QThread>

namespace GGS {
  namespace GameDownloader {
    class GameDownloadService;
  }

  namespace UpdateSystem {
    class UpdateManagerWorker;
    class CheckUpdateHelper;
  }
}

namespace GameNet {
  namespace Host {

    class Updater : public QObject
    {
      Q_OBJECT
      Q_PROPERTY(int updateState READ updateState NOTIFY updateStateChanged)

    public:
      explicit Updater(QObject* parent = 0);
      virtual ~Updater();
      
      void setDownloader(GGS::GameDownloader::GameDownloadService *downloader);

      int updateState();

      void startCheckUpdate();

   public slots:
      void switchClientVersion();

    signals:
      void downloadUpdateProgress(quint64 currentSize, quint64 totalSize);
      void allCompleted(bool isNeedRestart);
      void fileDownloadString(QString str);
      void updateError(int errorCode);
      void updateStateChanged(int updateState);
      void noUpdatesFound();
      void updatesFound();

      void downloadRetryNumber(int retryNumber);
      void restartApplication(bool shouldStartWithSameArguments, bool isMinimized);

      /*
          Сигнал от Host к UI с просьбой перезапуска приложения
      */
      void restartUIRequest();

    private slots:
      void updateThreadFinished();
      void setUpdateState(int updateState);

      void checkUpdateHelperFinished(GGS::UpdateSystem::CheckUpdateHelper::Results result);
      int checkUpdateInterval();
      void updateErrorSlot(int errorCode);
      
      void startCheckUpdateRetry();
      void allCompletedSlot(bool isNeedRestart);

    private:
      void initializeUpdateSettings();

      GGS::UpdateSystem::UpdateManagerWorker *_updateManagerWorker;
      GGS::UpdateSystem::CheckUpdateHelper _checkUpdateHelper;

      QString _applicationArea;
      int _updateState;
      QThread* _updateThread;
      QTimer* _retryTimer;
      GGS::GameDownloader::GameDownloadService* _downloader;
    };
  }
}
