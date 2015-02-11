#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>

#include <Windows.h>

namespace GGS {
  namespace Core {
    class Service;
  }
}
 
namespace GameNet {
  namespace Host {
    namespace Installer {
      
      class UninstallResult : public QObject
      {
        Q_OBJECT
      public:
        enum UninstallResultType
        {
          Success = 0,
          Canceled = 1,
          Failed = 2
        };

        UninstallResult(const QString &serviceId, QObject *parent = 0);
        ~UninstallResult();

        void setFinished(int result);
        void asyncWaitResult();

      public slots:
        void onUninstallFinished(const GGS::Core::Service *service);
        void onUninstallFailed(const GGS::Core::Service *service);
        void onUninstallCancelled(const QString &serviceId);

      signals:
        void finished(int reason);

      private:
        bool createEvent();
        bool openSharedMemory();
        void internalWaitUninstall();

        QString _serviceId;
        QString _eventName;
        QString _sharedMemoryName;

        HANDLE _eventHandle;
        HANDLE _memoryHandle;
        quint32 *_uninstallResult;
      };
    }
  }
}