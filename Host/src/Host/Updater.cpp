/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#include <GameDownloader/GameDownloadService.h>
#include <UpdateSystem/UpdateManagerWorker.h>
#include <UpdateSystem/CheckUpdateHelper.h>

#include <QtCore/QSettings>

#include <Core/Service>
#include <Host/Updater.h>

namespace GameNet {
  namespace Host {

    using GGS::UpdateSystem::UpdateManagerWorker;
    using GGS::GameDownloader::GameDownloadService;

    Updater::Updater(QObject* parent)
      : QObject(parent)
      , _updateState(-1)
      , _updateThread(0)
      , _retryTimer(new QTimer(this))
    {
      this->_updateManagerWorker = new UpdateManagerWorker();
      this->_retryTimer->setInterval(60000);
      this->_retryTimer->setSingleShot(true);

      QObject::connect(this->_retryTimer, &QTimer::timeout,
              this, &Updater::startCheckUpdateRetry);

      QObject::connect(this->_updateManagerWorker, &UpdateManagerWorker::updateProgressChanged,
              this, &Updater::downloadUpdateProgress);

      QObject::connect(this->_updateManagerWorker, &UpdateManagerWorker::updateWarning, [=](GGS::Downloader::DownloadResults error){
        DEBUG_LOG << "Update warning" << error;
      });

      QObject::connect(this->_updateManagerWorker, &UpdateManagerWorker::updateError,
              this, &Updater::updateError);

      QObject::connect(this->_updateManagerWorker, &UpdateManagerWorker::updateStateChanged,
              this, &Updater::setUpdateState);

      QObject::connect(this->_updateManagerWorker, &UpdateManagerWorker::noUpdatesFound,
              this, &Updater::noUpdatesFound);

      QObject::connect(this->_updateManagerWorker, &UpdateManagerWorker::updatesFound,
              this, &Updater::updatesFound);

      QObject::connect(this->_updateManagerWorker, &UpdateManagerWorker::downloadRetryNumber,
              this, &Updater::downloadRetryNumber);

      QObject::connect(this->_updateManagerWorker, &UpdateManagerWorker::updatesFound,
              this->_updateManagerWorker, &UpdateManagerWorker::installUpdates);

      QObject::connect(this->_updateManagerWorker, &UpdateManagerWorker::allCompleted, 
              this, &Updater::allCompleted);

      QObject::connect(this->_updateManagerWorker, &UpdateManagerWorker::allCompleted, 
        this, &Updater::allCompletedSlot);

      QObject::connect(this->_updateManagerWorker, &UpdateManagerWorker::updateError,
              this, &Updater::updateErrorSlot);

      this->initializeUpdateSettings();
    }

    Updater::~Updater()
    {
    }

    void Updater::updateErrorSlot(int errorCode)
    {
      if (errorCode == GGS::UpdateSystem::NotEnoughSpace) {
        this->_retryTimer->start();
      }
    }

    void Updater::initializeUpdateSettings()
    {
      QString updateArea = QString("live");
      QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\GGS\\QGNA", QSettings::NativeFormat);
      bool ok = false;
      int area = settings.value("Repository", 0).toInt(&ok);
      if (!ok)
        area = 0;

      switch(area)
      {
      case 1:
        updateArea = QString("pts");
        this->_applicationArea = GGS::Core::Service::Pts;
        break;
      case 2:
        updateArea = QString("tst");
        this->_applicationArea = GGS::Core::Service::Tst;
        break;
      case 3:
        updateArea = QString("2live");
        this->_applicationArea = GGS::Core::Service::Live;
        break;
      default:
        updateArea = QString("live");  
        this->_applicationArea = GGS::Core::Service::Live;
      }

      QString installUpdateGnaPath = QString("");

#ifdef QGNA_NO_UPDATE
      installUpdateGnaPath = QString("tst");
#endif

      QString updateUrl = QString("http://fs0.gamenet.ru/update/qgna/%1/").arg(updateArea);
      QString updateCrc = QString("%1update.crc.7z").arg(updateUrl);
      this->_checkUpdateHelper.setUpdateUrl(updateCrc);

      this->_updateManagerWorker->setUpdateUrl(updateUrl);
      this->_updateManagerWorker->setInstallSubDir(installUpdateGnaPath);

      connect(&this->_checkUpdateHelper, &GGS::UpdateSystem::CheckUpdateHelper::finished,
        this, &Updater::checkUpdateHelperFinished);
    }

    void Updater::checkUpdateHelperFinished(GGS::UpdateSystem::CheckUpdateHelper::Results result)
    {
      switch(result)
      {
      case GGS::UpdateSystem::CheckUpdateHelper::Error:
        QTimer::singleShot(300000, &this->_checkUpdateHelper, SLOT(checkUpdate()));
        break;
      case GGS::UpdateSystem::CheckUpdateHelper::FoundUpdate: {
        DEBUG_LOG << "New update found. Restart required.";

// UNDONE доделать когда Илья закончит с запускатором
        if (//!this->_premiumExecutor.isAnyGameStarted() && 
            !this->_downloader->isAnyServiceInProgress()) {

            emit this->allCompleted(true);
            return;
        }

        QTimer::singleShot(this->checkUpdateInterval(), &this->_checkUpdateHelper, SLOT(checkUpdate()));
      }
        break;
      case GGS::UpdateSystem::CheckUpdateHelper::NotFound:
        QTimer::singleShot(this->checkUpdateInterval(), &this->_checkUpdateHelper, SLOT(checkUpdate()));
        break;
      default:
        DEBUG_LOG << "Unknown result " << result;
        break;
      }
    }

    void Updater::switchClientVersion() 
    {
      QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\GGS\\QGNA", QSettings::NativeFormat);
      bool ok = false;
      int area = settings.value("Repository", 0).toInt(&ok);
      if (!ok)
        area = 0;

      area = area == 0 ? 1 : 0;

      settings.setValue("Repository", area);

      emit this->restartApplication(true, false);
    }

    int Updater::checkUpdateInterval()
    {
      QDateTime now = QDateTime::currentDateTimeUtc();
      now = now.addSecs(14400); // Moscow time UTC+4
      int hour = now.time().hour();
      if (hour >= 14)
        return 10800000;

      return 1800000;
    }

    void Updater::setUpdateState(int updateState) 
    {
      this->_updateState = updateState; 
      emit this->updateStateChanged(updateState);
    }

    void Updater::updateThreadFinished()
    {
      if (this->_updateThread) {
        this->_updateThread->deleteLater();
        this->_updateThread = 0;
      }

      qDebug() << "[DEBUG] UpdateLater called";
    }

    void Updater::startCheckUpdateRetry() 
    {
      Q_ASSERT(this->_updateManagerWorker);

      QMetaObject::invokeMethod(this->_updateManagerWorker, "checkUpdate", Qt::QueuedConnection);
    }

    void Updater::startCheckUpdate()
    {
      this->_updateThread = new QThread();
      this->_updateThread->setObjectName("Update manager thread");
      connect(this->_updateThread, SIGNAL(finished()), this, SLOT(updateThreadFinished()));

      this->_updateManagerWorker->setWorkingDir(QCoreApplication::applicationDirPath());
      this->_updateManagerWorker->moveToThread(this->_updateThread);

      this->_updateThread->start();
      QMetaObject::invokeMethod(this->_updateManagerWorker, "checkUpdate", Qt::QueuedConnection);
    }

    int Updater::updateState()
    {
      return this->_updateState;
    }

    void Updater::allCompletedSlot(bool isNeedRestart)
    {
      if (!isNeedRestart) {
        this->_checkUpdateHelper.checkUpdate();
      }
    }

    void Updater::setDownloader(GameDownloadService *downloader)
    {
      this->_downloader = downloader;
    }

  }
}