#include <Host/Bridge/ExecutorBridge.h>

#include <Host/Proxy/GameExecutorProxy.h>

#include <RestApi/ProtocolOneCredential.h>

using P1::RestApi::ProtocolOneCredential;
using P1::Host::Proxy::GameExecutorProxy;

namespace P1 {
  namespace Host {
    namespace Bridge {

      ExecutorBridge::ExecutorBridge(QObject *parent /*= 0*/)
        : QObject(parent)
        , _executor(nullptr)
      {
      }

      ExecutorBridge::~ExecutorBridge()
      {
      }

      void ExecutorBridge::setExecutor(GameExecutorProxy *value)
      {
        Q_ASSERT(value);
        this->_executor = value;

        QObject::connect(value, &GameExecutorProxy::serviceStarted,
          this, &ExecutorBridge::serviceStarted);

        QObject::connect(value, &GameExecutorProxy::serviceFinished,
          this, &ExecutorBridge::serviceFinished);
      }

      void ExecutorBridge::execute(
        const QString& serviceId,
        const QString& accessToken, 
        const QString& acccessTokenExpiredTime)
      {
        Q_ASSERT(this->_executor);
        ProtocolOneCredential base(accessToken, acccessTokenExpiredTime);
        this->_executor->execute(serviceId, base);
      }

      bool ExecutorBridge::isGameStarted(const QString& serviceId) const
      {
        Q_ASSERT(this->_executor);
        return this->_executor->isGameStarted(serviceId);
      }

      bool ExecutorBridge::isAnyGameStarted() const
      {
        Q_ASSERT(this->_executor);
        return this->_executor->isAnyGameStarted();
      }

      Q_NOREPLY void ExecutorBridge::terminateGame(const QString& serviceId /*= QString()*/)
      {
        Q_ASSERT(this->_executor);
        this->_executor->terminateGame(serviceId);
      }

    }
  }
}
