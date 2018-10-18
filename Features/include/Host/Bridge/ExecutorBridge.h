#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMetaType>

#include <QtDBus/QDBusArgument>
#include <QtDBus/QDBusAbstractAdaptor>

namespace P1 {
  namespace Host {

    namespace Proxy {
      class GameExecutorProxy;
    };

    namespace Bridge {

      class ExecutorBridge : public QObject
      {
        Q_OBJECT
        Q_CLASSINFO("Version", "1.0.0.0")
        Q_CLASSINFO("D-Bus Interface", "com.protocolone.launcher.dbus.Executor")

      public:
        explicit ExecutorBridge(QObject *parent = 0);
        virtual ~ExecutorBridge();

        void setExecutor(Proxy::GameExecutorProxy *value);
        
      public slots:
        Q_NOREPLY void execute(const QString& serviceId, const QString& accessToken, const QString& acccessTokenExpiredTime);

        bool isGameStarted(const QString& serviceId) const;
        bool isAnyGameStarted() const;
        
        /*!
        \fn Q_NOREPLY void ExecutorBridge::terminateGame(const QString& serviceId = QString());
        
        \brief  Terminate game by serviceId. Close all games if serviceId is empty.
        
        \author "Ilya Tkachenko"
        \date 13.09.2017
        
        \param  serviceId Identifier for the service.
         */

        Q_NOREPLY void terminateGame(const QString& serviceId = QString());

      signals:
        void serviceStarted(const QString& serviceId);
        void serviceFinished(const QString& serviceId, int finishState);
        
      private:
        Proxy::GameExecutorProxy *_executor;
      };

    }
  }
}
