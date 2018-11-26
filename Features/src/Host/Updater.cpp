#include <Host/Updater.h>
#include <Host/GameExecutor.h>

#include <UpdateSystem/UpdateManagerWorker.h>
#include <UpdateSystem/Extractor/MiniZipExtractor.h>
#include <UpdateSystem/Extractor/SevenZipExtractor.h>

#include <UpdateSystem/UpdateManagerWorker.h>
#include <UpdateSystem/CheckUpdateHelper.h>

#include <Core/Service.h>

#include <QtCore/QSettings>

#include <QtYaml/ConfigManager.h>

using P1::UpdateSystem::UpdateManagerWorker;
using P1::UpdateSystem::CheckUpdateHelper;
using P1::Core::Service;

namespace P1 {
  namespace Host {

    Updater::Updater(QObject* parent)
      : QObject(parent)
      , _updateState(-1)
      , _updateThread(0)
      , _retryTimer(new QTimer(this))
      , _enabled(true)
    {
      this->_updateManagerWorker = new UpdateManagerWorker(new P1::Extractor::MiniZipExtactor);
      this->_retryTimer->setInterval(60000);
      this->_retryTimer->setSingleShot(true);

      QObject::connect(this->_retryTimer, &QTimer::timeout,
              this, &Updater::startCheckUpdateRetry);

      QObject::connect(this->_updateManagerWorker, &UpdateManagerWorker::updateProgressChanged,
              this, &Updater::downloadUpdateProgress);

      QObject::connect(this->_updateManagerWorker, &UpdateManagerWorker::updateWarning, 
        [=](P1::Downloader::DownloadResults error) {
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
      if (errorCode == P1::UpdateSystem::NotEnoughSpace) {
        this->_retryTimer->start();
      }
    }

    void Updater::initializeUpdateSettings()
    {
      this->_applicationArea.load();

      QString configPath = QCoreApplication::applicationDirPath() + "/Config.yaml";
      P1::QtYaml::ConfigManager configManager;
      configManager.load(configPath);

      this->_enabled = configManager.value<QString>("update\\enabled", "true") == "true";
      
      QString installUpdateGnaPath = configManager.value<QString>("update\\directory", "");

      
      QString updateUrl = configManager.value<QString>("update\\url", "");

      if (!updateUrl.isEmpty()) {
        if (!updateUrl.endsWith('/')) {
          updateUrl += '/';
        }

        updateUrl += QString(this->_applicationArea) + "/";
      }

      QString updateCrc = QString("%1update.crc.7z").arg(updateUrl);
      this->_checkUpdateHelper.setUpdateUrl(updateCrc);

      this->_updateManagerWorker->setUpdateUrl(updateUrl);
      this->_updateManagerWorker->setInstallSubDir(installUpdateGnaPath);
      QObject::connect(
        &this->_checkUpdateHelper, &CheckUpdateHelper::finished,
        this, &Updater::checkUpdateHelperFinished);
    }

    void Updater::checkUpdateHelperFinished(CheckUpdateHelper::Results result)
    {
      Q_ASSERT(this->_canRestart);
      switch(result)
      {
      case CheckUpdateHelper::Error:
        QTimer::singleShot(300000, &this->_checkUpdateHelper, SLOT(checkUpdate()));
        break;
      case CheckUpdateHelper::FoundUpdate: {
        DEBUG_LOG << "New update found. Restart required.";

        if (this->_canRestart()) {
          emit this->allCompleted(true);
          return;
        }

        QTimer::singleShot(this->checkUpdateInterval(), &this->_checkUpdateHelper, SLOT(checkUpdate()));
      }
        break;
      case CheckUpdateHelper::NotFound:
        QTimer::singleShot(this->checkUpdateInterval(), &this->_checkUpdateHelper, SLOT(checkUpdate()));
        break;
      default:
        DEBUG_LOG << "Unknown result " << result;
        break;
      }
    }

    int Updater::checkUpdateInterval()
    {
      QDateTime now = QDateTime::currentDateTimeUtc();
      now = now.addSecs(14400); // Moscow time UTC+4
      int hour = now.time().hour();
      
      if (hour == 13) {
        QTime time = now.time();
        QTime time13(13, 0, 0);
        int secs = time13.secsTo(time);
        return static_cast<int>(((secs % 1800) / 1800.0f) * 10800.0f + 1800) * 1000;
      }

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
      if (!this->_enabled) {
        emit this->allCompleted(false);
        return;
      }

      Q_ASSERT(this->_canRestart);
      this->_updateThread = new QThread();
      this->_updateThread->setObjectName("Update manager thread");
      QObject::connect(this->_updateThread, &QThread::finished,
        this, &Updater::updateThreadFinished);

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

    P1::ApplicationArea Updater::applicationArea() const
    {
      return this->_applicationArea;
    }

    void Updater::setCanRestart(std::function<bool ()> value)
    {
      Q_ASSERT(value);
      this->_canRestart = value;
    }

  }
}