#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMetaType>

namespace P1 {
  namespace Host {

    class Updater;

    namespace Bridge {

      class UpdateManagerBridge: public QObject 
      {
        Q_OBJECT
        Q_CLASSINFO("Version", "1.0.0.0")
        Q_CLASSINFO("D-Bus Interface", "com.protocolone.launcher.dbus.Updater")
      public:
        explicit UpdateManagerBridge(QObject *parent = 0);
        virtual ~UpdateManagerBridge();

        /**
         * \fn  void UpdateManagerBridge::setUpdateManager(Updater* updater);
         *
         * \brief Sets a update manager.
         *
         * \author Igor Bugaev
         * \date  26.09.2014
         *
         * \param [in,out]  updater If non-null, the updater.
         */

        void setUpdateManager(Updater* updater);

     public slots:

        /**
         * \fn  int UpdateManagerBridge::updateState();
         *
         * \brief Updates the state.
         *
         * \author Igor Bugaev 
         * \date  26.09.2014
         *
         * \return  .
         */

        int updateState();

      signals:

        /**
         * \fn  void UpdateManagerBridge::downloadUpdateProgress(quint64 currentSize, quint64 totalSize);
         *
         * \brief Downloads the update progress.
         *
         * \author Igor Bugaev
         * \date  26.09.2014
         *
         * \param currentSize Size of the current.
         * \param totalSize   Size of the total.
         */

        void downloadUpdateProgress(quint64 currentSize, quint64 totalSize);

        /**
         * \fn  void UpdateManagerBridge::updatesFound();
         *
         * \brief Updates found.
         *
         * \author Igor Bugaev
         * \date  26.09.2014
         */

        void updatesFound();

        /**
         * \fn  void UpdateManagerBridge::noUpdatesFound();
         *
         * \brief No updates found.
         *
         * \author Igor Bugaev
         * \date  26.09.2014
         */

        void noUpdatesFound();

        /**
         * \fn  void UpdateManagerBridge::downloadRetryNumber(int retryNumber);
         *
         * \brief Downloads the retry number described by retryNumber.
         *
         * \author Igor Bugaev
         * \date  26.09.2014
         *
         * \param retryNumber The retry number.
         */

        void downloadRetryNumber(int retryNumber);

        /**
         * \fn  void UpdateManagerBridge::allCompleted(bool isNeedRestart);
         *
         * \brief All completed.
         *
         * \author  
         * \date  26.09.2014
         *
         * \param isNeedRestart true if is need restart.
         */

        void allCompleted(bool isNeedRestart);

        /**
         * \fn  void UpdateManagerBridge::updateStateChanged(int updateState);
         *
         * \brief Updates the state changed described by updateState.
         *
         * \author Igor Bugaev 
         * \date  26.09.2014
         *
         * \param updateState State of the update.
         */

        void updateStateChanged(int updateState);

        /**
         * \fn  void UpdateManagerBridge::updateError(int errorCode);
         *
         * \brief Updates the error described by errorCode.
         *
         * \author Igor Bugaev
         * \date  26.09.2014
         *
         * \param errorCode The error code.
         */

        void updateError(int errorCode);

      private:
        Updater* _updater;
      };

    }
  }
}
