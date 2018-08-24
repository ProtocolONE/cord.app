#include <GameExecutor/GameExecutorService.h>

#include <Host/ElectronExecutor.h>
#include <Host/ElectronExecutor_p.h>

#include <RestApi/RestApiManager.h>
#include <RestApi/GameNetCredential.h>

#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>
#include <QtCore/QDebug>

#include <qt_windows.h>
#include <shellapi.h>

namespace GameNet {
  namespace Host {

    using namespace P1::GameExecutor;

    ElectronExecutor::ElectronExecutor(QObject *parent) : ExecutorBase(parent)
    {
      this->_scheme = QString("electron");
    }

    ElectronExecutor::~ElectronExecutor()
    {
    }

    void ElectronExecutor::execute(
      const P1::Core::Service &service,
      P1::GameExecutor::GameExecutorService *executorService,
      const P1::RestApi::GameNetCredential& credential)
    {
      QString id = service.id();
      ElectronExecutorPrivate* executor = new ElectronExecutorPrivate(this);

      QObject::connect(executor, &ElectronExecutorPrivate::started, this, &ElectronExecutor::started, Qt::DirectConnection);
      QObject::connect(executor, &ElectronExecutorPrivate::finished, this, &ElectronExecutor::internalFinished, Qt::DirectConnection);
      QObject::connect(executorService, &P1::GameExecutor::GameExecutorService::stopExecution, executor, &ElectronExecutorPrivate::onShutdown);

      executor->execute(service, executorService, credential, this->_scheme);
    }

    void ElectronExecutor::internalFinished(const P1::Core::Service &service, FinishState state)
    {
      QObject::sender()->deleteLater();
      emit this->finished(service, state);
    }
  }
}
