#include <Host/Connection.h>
#include <Host/ServiceHandle.h>

#include <Host/Proxy/GameExecutorProxy.h>

using P1::RestApi::ProtocolOneCredential;

namespace P1 {
  namespace Host {
    namespace Proxy {

      GameExecutorProxy::GameExecutorProxy(QObject *parent /*= 0*/)
        : QObject(parent)
        , _executor(nullptr)
        , _connetion(nullptr)
        , _serviceHandle(nullptr)
      {
      }

      GameExecutorProxy::~GameExecutorProxy()
      {
      }

      void GameExecutorProxy::setExecutor(P1::Host::GameExecutor *value)
      {
        Q_ASSERT(value);
        this->_executor = value;

        QObject::connect(value, &GameExecutor::serviceStarted,
          this, &GameExecutorProxy::onServiceStarted, Qt::QueuedConnection);

        QObject::connect(value, &GameExecutor::serviceFinished,
          this, &GameExecutorProxy::onServiceFinished, Qt::QueuedConnection);

        QObject::connect(value, &GameExecutor::secondServiceStarted,
          this, &GameExecutorProxy::onSecondServiceStarted, Qt::QueuedConnection);

        QObject::connect(value, &GameExecutor::secondServiceFinished,
          this, &GameExecutorProxy::onSecondServiceFinished, Qt::QueuedConnection);
      }

      void GameExecutorProxy::setConnection(Connection *value)
      {
        Q_ASSERT(value);
        this->_connetion = value;
      }

      void GameExecutorProxy::setServiceHandle(ServiceHandle *value)
      {
        Q_ASSERT(value);
        this->_serviceHandle = value;
      }

      void GameExecutorProxy::execute(
        const QString& serviceId,
        const ProtocolOneCredential& credetial)
      {
        Q_ASSERT(this->_connetion);
        Q_ASSERT(this->_executor);
        Q_ASSERT(this->_serviceHandle);

        if (!this->_serviceHandle->lock(serviceId, this->_connetion))
          return;

        this->processExecute(serviceId, credetial);
        this->_executor->execute(serviceId, credetial);
      }

      void GameExecutorProxy::executeSecond(
        const QString& serviceId,
        const ProtocolOneCredential& credetial,
        const ProtocolOneCredential& secondCredetial)
      {
        Q_ASSERT(this->_connetion);
        Q_ASSERT(this->_executor);
        Q_ASSERT(this->_serviceHandle);

        if (!this->_serviceHandle->lock(serviceId, this->_connetion))
          return;

        this->processExecute(serviceId, credetial, secondCredetial);
        this->_executor->executeSecond(serviceId, credetial, secondCredetial);
      }

      void GameExecutorProxy::processExecute(
        const QString& serviceId, 
        const ProtocolOneCredential& credetial, 
        const ProtocolOneCredential& secondCredetial)
      {
        QHash<QString, P1::RestApi::ProtocolOneCredential>& map = 
          secondCredetial.isEmpty() ? this->_executedGame : this->_executedSecondGame;

        // INFO всегда переписываем автризацию на новую запускаемую.
        // UI приложение должно следить и не запускать из одного подключения игру, если
        // она уже запущена. Эта авторизация используется для отправки маркетинга, и поэтому ее нельзя
        // просто так чистить при закрытии игры.

        const ProtocolOneCredential& mainCredential = 
          secondCredetial.isEmpty() ? credetial : secondCredetial;

        map[serviceId] = mainCredential;
      }

      void GameExecutorProxy::onServiceStarted(const QString& serviceId)
      {
        Q_ASSERT(this->_connetion);

        if (!this->_connetion->isOwnService(serviceId))
          return;

        emit this->serviceStarted(serviceId);
      }

      void GameExecutorProxy::onServiceFinished(const QString& serviceId, int finishState)
      {
        Q_ASSERT(this->_connetion);
        Q_ASSERT(this->_serviceHandle);

        if (!this->_connetion->isOwnService(serviceId))
          return;

        if (!this->_executor->isSecondGameStarted(serviceId))
          this->_serviceHandle->unlock(serviceId, this->_connetion);

        this->serviceFinished(serviceId, finishState);
      }

      void GameExecutorProxy::onSecondServiceStarted(const QString& serviceId)
      {
        Q_ASSERT(this->_connetion);

        if (!this->_connetion->isOwnService(serviceId))
          return;

        this->secondServiceStarted(serviceId);
      }

      void GameExecutorProxy::onSecondServiceFinished(const QString& serviceId, int finishState)
      {
        Q_ASSERT(this->_connetion);
        Q_ASSERT(this->_serviceHandle);

        if (!this->_connetion->isOwnService(serviceId))
          return;

        if (!this->_executor->isGameStarted(serviceId))
          this->_serviceHandle->unlock(serviceId, this->_connetion);

        this->secondServiceFinished(serviceId, finishState);
      }

      bool GameExecutorProxy::isGameStarted(const QString& serviceId) const
      {
        Q_ASSERT(this->_executor);
        return this->_executor->isGameStarted(serviceId);
      }

      bool GameExecutorProxy::isAnyGameStarted() const
      {
        Q_ASSERT(this->_executor);
        return this->_executor->isAnyGameStarted();
      }

      bool GameExecutorProxy::canExecuteSecond(const QString& serviceId) const
      {
        Q_ASSERT(this->_executor);
        return this->_executor->canExecuteSecond(serviceId);
      }

      void GameExecutorProxy::shutdownSecond()
      {
        Q_ASSERT(this->_executor);
        this->_executor->shutdownSecond();
      }

      void GameExecutorProxy::terminateGame(const QString& serviceId /*= QString()*/)
      {
        Q_ASSERT(this->_executor);
        this->_executor->terminateAll(serviceId);
      }

      ProtocolOneCredential GameExecutorProxy::gameCredential(const QString& serviceId)
      {
        if (!this->_executedGame.contains(serviceId))
          return ProtocolOneCredential();
          
        return this->_executedGame[serviceId];
      }

      ProtocolOneCredential GameExecutorProxy::secondGameCredential(const QString& serviceId)
      {
        if (!this->_executedSecondGame.contains(serviceId))
          return ProtocolOneCredential();

        return this->_executedSecondGame[serviceId];
      }

    }
  }
}