#pragma once

#include <UpdateSystem/CheckUpdateHelper.h>

#include <Core/Service.h>

#include <QtCore/QTimer>
#include <QtCore/QThread>

#include <functional>

#include <Helper/ApplicationArea.hpp>

namespace P1 {
  namespace UpdateSystem {
    class UpdateManagerWorker;
    class CheckUpdateHelper;
  }
}

namespace P1 {
  namespace Host {

    class Updater : public QObject
    {
      Q_OBJECT
      Q_PROPERTY(int updateState READ updateState NOTIFY updateStateChanged)

    public:
      explicit Updater(QObject* parent = 0);
      virtual ~Updater();

      /**
       * \fn  void Updater::setCanRestart(std::function<bool ()> value);
       *
       * \brief Sets a can restart.
       *
       * \author Igor Bugaev 
       * \date  25.09.2014
       *
       * \param value The value.
       */

      void setCanRestart(std::function<bool ()> value);

      /**
       * \fn  virtual int Updater::updateState();
       *
       * \brief Updates the state.
       *
       * \author Igor Bugaev 
       * \date  25.09.2014
       *
       * \return  .
       */

      virtual int updateState();

      /**
       * \fn  void Updater::startCheckUpdate();
       *
       * \brief Starts a check update.
       *
       * \author Igor Bugaev 
       * \date  25.09.2014
       */

      void startCheckUpdate();

      /**
       * \fn  P1::Core::Service::Area Updater::applicationArea() const;
       *
       * \brief Gets the application area.
       *
       * \author Igor Bugaev 
       * \date  25.09.2014
       *
       * \return  .
       */

      P1::ApplicationArea applicationArea() const;

signals:

      /**
       * \fn  void Updater::downloadUpdateProgress(quint64 currentSize, quint64 totalSize);
       *
       * \brief Downloads the update progress.
       *
       * \author Igor Bugaev 
       * \date  25.09.2014
       *
       * \param currentSize Size of the current.
       * \param totalSize   Size of the total.
       */

      void downloadUpdateProgress(quint64 currentSize, quint64 totalSize);

      /**
       * \fn  void Updater::allCompleted(bool isNeedRestart);
       *
       * \brief All completed.
       *
       * \author  
       * \date  25.09.2014
       *
       * \param isNeedRestart true if is need restart.
       */

      void allCompleted(bool isNeedRestart);

      /**
       * \fn  void Updater::fileDownloadString(QString str);
       *
       * \brief File download string.
       *
       * \author  
       * \date  25.09.2014
       *
       * \param str The string.
       */

      void fileDownloadString(QString str);

      /**
       * \fn  void Updater::updateError(int errorCode);
       *
       * \brief Updates the error described by errorCode.
       *
       * \author  
       * \date  25.09.2014
       *
       * \param errorCode The error code.
       */

      void updateError(int errorCode);

      /**
       * \fn  void Updater::updateStateChanged(int updateState);
       *
       * \brief Updates the state changed described by updateState.
       *
       * \author  
       * \date  25.09.2014
       *
       * \param updateState State of the update.
       */

      void updateStateChanged(int updateState);

      /**
       * \fn  void Updater::noUpdatesFound();
       *
       * \brief No updates found signal.
       *
       * \author  
       * \date  25.09.2014
       */

      void noUpdatesFound();

      /**
       * \fn  void Updater::updatesFound();
       *
       * \brief Updates found signal.
       *
       * \author  
       * \date  25.09.2014
       */

      void updatesFound();

      /**
       * \fn  void Updater::downloadRetryNumber(int retryNumber);
       *
       * \brief Downloads the retry number described by retryNumber.
       *
       * \author  Igor Bugaev
       * \date  25.09.2014
       *
       * \param retryNumber The retry number.
       */

      void downloadRetryNumber(int retryNumber);

      /**
       * \fn  void Updater::restartApplication(bool shouldStartWithSameArguments, bool isMinimized);
       *
       * \brief Restart application.
       *
       * \author  Igor Bugaev
       * \date  25.09.2014
       *
       * \param shouldStartWithSameArguments  true if should start with same arguments.
       * \param isMinimized                   true if is minimized.
       */

      void restartApplication(bool shouldStartWithSameArguments, bool isMinimized);

      /**
       * \fn  void Updater::restartUIRequest();
       *
       * \brief Restart user interface request.
       *
       * \author  Igor.Bugaev
       * \date  25.09.2014
       */

      void restartUIRequest();

    private slots:
      void updateThreadFinished();
      void setUpdateState(int updateState);

      void checkUpdateHelperFinished(P1::UpdateSystem::CheckUpdateHelper::Results result);
      int checkUpdateInterval();
      void updateErrorSlot(int errorCode);
      
      void startCheckUpdateRetry();
      void allCompletedSlot(bool isNeedRestart);

    private:
      void initializeUpdateSettings();

      P1::UpdateSystem::UpdateManagerWorker *_updateManagerWorker;
      P1::UpdateSystem::CheckUpdateHelper _checkUpdateHelper;

      P1::ApplicationArea _applicationArea;
      int _updateState;
      QThread* _updateThread;
      QTimer* _retryTimer;
      
      std::function<bool ()> _canRestart;
    };
  }
}
