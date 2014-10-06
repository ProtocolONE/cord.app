#include <Host/Proxy/GameExecutorProxy.h>

using GGS::RestApi::GameNetCredential;

namespace GameNet {
  namespace Host {
    namespace Proxy {

      GameExecutorProxy::GameExecutorProxy(QObject *parent /*= 0*/)
        : GameExecutor(parent)
      {
      }

      GameExecutorProxy::~GameExecutorProxy()
      {
      }

      void GameExecutorProxy::init()
      {
        this->GameExecutor::init();

        QObject::connect(this, &GameExecutor::serviceStarted,
          this, &GameExecutorProxy::onServiceStarted);

        QObject::connect(this, &GameExecutor::serviceFinished,
          this, &GameExecutorProxy::onServiceFinished);

        QObject::connect(this, &GameExecutor::secondServiceStarted,
          this, &GameExecutorProxy::onSecondServiceStarted);

        QObject::connect(this, &GameExecutor::secondServiceFinished,
          this, &GameExecutorProxy::onSecondServiceFinished);
      }

      void GameExecutorProxy::execute(
        const QString& serviceId,
        const GameNetCredential& credetial)
      {
        this->processExecute(serviceId, credetial);
        this->GameExecutor::execute(serviceId, credetial);
      }

      void GameExecutorProxy::executeSecond(
        const QString& serviceId,
        const GameNetCredential& credetial,
        const GameNetCredential& secondCredetial)
      {
        this->processExecute(serviceId, credetial, secondCredetial);
        this->GameExecutor::executeSecond(serviceId, credetial, secondCredetial);
      }

      void GameExecutorProxy::processExecute(
        const QString& serviceId, 
        const GameNetCredential& credetial, 
        const GameNetCredential& secondCredetial)
      {
        QHash<QString, GGS::RestApi::GameNetCredential>& map = 
          secondCredetial.isEmpty() ? this->_executedGame : this->_executedSecondGame;

        if (map.contains(serviceId))
          return;

        const GameNetCredential& mainCredential = 
          secondCredetial.isEmpty() ? credetial : secondCredetial;

        map[serviceId] = mainCredential;
      }

      void GameExecutorProxy::onServiceStarted(const QString& serviceId)
      {
        if (!this->_executedGame.contains(serviceId))
          return;

        emit this->startedWithCredential(serviceId, this->_executedGame[serviceId]);
      }

      void GameExecutorProxy::onServiceFinished(const QString& serviceId, int finishState)
      {
        if (!this->_executedGame.contains(serviceId))
          return;

        emit this->finishedWithCredential(serviceId, this->_executedGame[serviceId]);
        this->_executedGame.remove(serviceId);
      }

      void GameExecutorProxy::onSecondServiceStarted(const QString& serviceId)
      {
        if (!this->_executedSecondGame.contains(serviceId))
          return;

        emit this->startedWithCredential(serviceId, this->_executedSecondGame[serviceId]);
      }

      void GameExecutorProxy::onSecondServiceFinished(const QString& serviceId, int finishState)
      {
        if (!this->_executedSecondGame.contains(serviceId))
          return;

        emit this->finishedWithCredential(serviceId, this->_executedSecondGame[serviceId]);
        this->_executedSecondGame.remove(serviceId);
      }

    }
  }
}