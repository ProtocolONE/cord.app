#pragma once
#include <Host/GameExecutor.h>

#include <RestApi/ProtocolOneCredential.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QHash>

namespace P1 {
  namespace Host {

    class Connection;
    class ServiceHandle;

    namespace Proxy {

      class GameExecutorProxy : public QObject
      {
        Q_OBJECT
      public:
        explicit GameExecutorProxy(QObject *parent = 0);
        virtual ~GameExecutorProxy();

        void setExecutor(P1::Host::GameExecutor *value);
        void setConnection(Connection *value);
        void setServiceHandle(ServiceHandle *value);

        virtual void execute(
          const QString& serviceId,
          const P1::RestApi::ProtocolOneCredential& credetial);

        virtual void executeSecond(
          const QString& serviceId,
          const P1::RestApi::ProtocolOneCredential& credetial,
          const P1::RestApi::ProtocolOneCredential& secondCredetial);

        virtual bool isGameStarted(const QString& serviceId) const;
        virtual bool isAnyGameStarted() const;
        virtual bool canExecuteSecond(const QString& serviceId) const;
        virtual void shutdownSecond();

        virtual void terminateGame(const QString& serviceId = QString());

        P1::RestApi::ProtocolOneCredential gameCredential(const QString& serviceId);
        P1::RestApi::ProtocolOneCredential secondGameCredential(const QString& serviceId);

      signals:
        void serviceStarted(const QString& serviceId);
        void serviceFinished(const QString& serviceId, int finishState);
        void secondServiceStarted(const QString& serviceId);
        void secondServiceFinished(const QString& serviceId, int finishState);

      private:
        void onServiceStarted(const QString& serviceId);
        void onServiceFinished(const QString& serviceId, int finishState);
        void onSecondServiceStarted(const QString& serviceId);
        void onSecondServiceFinished(const QString& serviceId, int finishState);

        virtual void processExecute(
          const QString& serviceId,
          const P1::RestApi::ProtocolOneCredential& credetial,
          const P1::RestApi::ProtocolOneCredential& secondCredetial  = P1::RestApi::ProtocolOneCredential());

        Connection *_connetion;
        P1::Host::GameExecutor *_executor;
        ServiceHandle *_serviceHandle;

        QHash<QString, P1::RestApi::ProtocolOneCredential> _executedGame;
        QHash<QString, P1::RestApi::ProtocolOneCredential> _executedSecondGame;
        
     };

    }
  }
}