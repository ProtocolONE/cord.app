#pragma once
#include <Host/GameExecutor.h>

#include <RestApi/GameNetCredential.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QHash>

namespace GameNet {
  namespace Host {
    namespace Proxy {

      class GameExecutorProxy :
        public GameNet::Host::GameExecutor
      {
        Q_OBJECT
      public:
        explicit GameExecutorProxy(QObject *parent = 0);
        virtual ~GameExecutorProxy();

        virtual void init();

        virtual void execute(
          const QString& serviceId,
          const GGS::RestApi::GameNetCredential& credetial);

        virtual void executeSecond(
          const QString& serviceId,
          const GGS::RestApi::GameNetCredential& credetial,
          const GGS::RestApi::GameNetCredential& secondCredetial);

      signals:
        void startedWithCredential(
          const QString& serviceId, 
          const GGS::RestApi::GameNetCredential credetial);

        void finishedWithCredential(
          const QString& serviceId, 
          const GGS::RestApi::GameNetCredential credetial);

      private:
        void onServiceStarted(const QString& serviceId);
        void onServiceFinished(const QString& serviceId, int finishState);
        void onSecondServiceStarted(const QString& serviceId);
        void onSecondServiceFinished(const QString& serviceId, int finishState);

        virtual void processExecute(
          const QString& serviceId,
          const GGS::RestApi::GameNetCredential& credetial,
          const GGS::RestApi::GameNetCredential& secondCredetial  = GGS::RestApi::GameNetCredential());

        QHash<QString, GGS::RestApi::GameNetCredential> _executedGame;
        QHash<QString, GGS::RestApi::GameNetCredential> _executedSecondGame;
     };

    }
  }
}