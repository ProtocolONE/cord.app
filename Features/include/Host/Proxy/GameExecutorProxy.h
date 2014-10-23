#pragma once
#include <Host/GameExecutor.h>

#include <RestApi/GameNetCredential.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QHash>

namespace GameNet {
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

        void setExecutor(GameNet::Host::GameExecutor *value);
        void setConnection(Connection *value);
        void setServiceHandle(ServiceHandle *value);

        virtual void execute(
          const QString& serviceId,
          const GGS::RestApi::GameNetCredential& credetial);

        virtual void executeSecond(
          const QString& serviceId,
          const GGS::RestApi::GameNetCredential& credetial,
          const GGS::RestApi::GameNetCredential& secondCredetial);

        virtual bool isGameStarted(const QString& serviceId) const;
        virtual bool isAnyGameStarted() const;
        virtual bool canExecuteSecond(const QString& serviceId) const;
        virtual void shutdownSecond();

        GGS::RestApi::GameNetCredential gameCredential(const QString& serviceId);
        GGS::RestApi::GameNetCredential secondGameCredential(const QString& serviceId);

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
          const GGS::RestApi::GameNetCredential& credetial,
          const GGS::RestApi::GameNetCredential& secondCredetial  = GGS::RestApi::GameNetCredential());

        Connection *_connetion;
        GameNet::Host::GameExecutor *_executor;
        ServiceHandle *_serviceHandle;

        QHash<QString, GGS::RestApi::GameNetCredential> _executedGame;
        QHash<QString, GGS::RestApi::GameNetCredential> _executedSecondGame;
        
     };

    }
  }
}