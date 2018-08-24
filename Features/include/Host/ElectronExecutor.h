#pragma once

#include <GameExecutor/gameexecutor_global.h>
#include <GameExecutor/ExecutorBase.h>

#include <QObject>

namespace GameNet {
  namespace Host {

    class ElectronExecutor : public P1::GameExecutor::ExecutorBase
    {
      Q_OBJECT
    public:
      explicit ElectronExecutor(QObject *parent = 0);
      virtual ~ElectronExecutor();

      virtual void execute(
        const P1::Core::Service &service, 
        P1::GameExecutor::GameExecutorService *executorService,
        const P1::RestApi::GameNetCredential& credential) override;

    private:
      void internalFinished(const P1::Core::Service &service, P1::GameExecutor::FinishState state);
    };
  }
}
