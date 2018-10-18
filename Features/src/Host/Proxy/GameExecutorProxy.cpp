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

      void GameExecutorProxy::processExecute(
        const QString& serviceId, 
        const ProtocolOneCredential& credetial)
      {
        // INFO всегда переписываем автризацию на новую запускаемую.
        // UI приложение должно следить и не запускать из одного подключения игру, если
        // она уже запущена. Эта авторизация используется для отправки маркетинга, и поэтому ее нельзя
        // просто так чистить при закрытии игры.

        this->_executedGame[serviceId] = credetial;
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

        this->_serviceHandle->unlock(serviceId, this->_connetion);
        this->serviceFinished(serviceId, finishState);
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


    }
  }
}