#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMetaType>

namespace GameNet {
  namespace Host {

    class Updater;

    namespace Bridge {

      class UpdateManagerBridge: public QObject 
      {
        Q_OBJECT
        Q_CLASSINFO("Version", "1.0.0.0")
        Q_CLASSINFO("D-Bus Interface", "com.gamenet.dbus.Updater")
      public:
        explicit UpdateManagerBridge(QObject *parent = 0);
        virtual ~UpdateManagerBridge();

        void setUpdateManager(Updater* updater);

     public slots:
        int updateState();

      signals:
        void downloadUpdateProgress(quint64 currentSize, quint64 totalSize);
        void updatesFound();
        void noUpdatesFound();
        void downloadRetryNumber(int retryNumber);
        void allCompleted(bool isNeedRestart);
        void updateStateChanged(int updateState);
        void updateError(int errorCode);

      private:
        Updater* _updater;
      };

    }
  }
}
