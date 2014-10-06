#include <Host/Bridge/ExecutorBridge.h>
#include <Host/GameExecutor.h>
#include <Host/CredentialConverter.h>

#include <RestApi/GameNetCredential.h>

using GGS::RestApi::GameNetCredential;

namespace GameNet {
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

      void ExecutorBridge::setExecutor(GameExecutor *value)
      {
        Q_ASSERT(value);
        this->_executor = value;

        QObject::connect(value, &GameExecutor::serviceStarted,
          this, &ExecutorBridge::serviceStarted);

        QObject::connect(value, &GameExecutor::serviceFinished,
          this, &ExecutorBridge::serviceFinished);

        QObject::connect(value, &GameExecutor::secondServiceStarted,
          this, &ExecutorBridge::secondServiceStarted);

        QObject::connect(value, &GameExecutor::secondServiceFinished,
          this, &ExecutorBridge::secondServiceFinished);
      }

      void ExecutorBridge::execute(const QString& serviceId, const Credential& credential)
      {
        Q_ASSERT(this->_executor);
        GameNetCredential base = createGameNetCredential(credential);
        this->_executor->execute(serviceId, base);
      }

      void ExecutorBridge::executeSecond(
        const QString& serviceId, 
        const Credential& credential, 
        const Credential& secondCredential)
      {
        Q_ASSERT(this->_executor);
        GameNetCredential base = createGameNetCredential(credential);
        GameNetCredential second = createGameNetCredential(secondCredential);

        this->_executor->executeSecond(serviceId, base, second);
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

      bool ExecutorBridge::canExecuteSecond(const QString& serviceId) const
      {
        Q_ASSERT(this->_executor);
        return this->_executor->canExecuteSecond(serviceId);
      }

      void ExecutorBridge::shutdownSecond()
      {
        Q_ASSERT(this->_executor);
        this->_executor->shutdownSecond();
      }

    }
  }
}
