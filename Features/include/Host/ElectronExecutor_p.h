#pragma once

#include <GameExecutor/gameexecutor_global.h>
#include <GameExecutor/ExecutorBase.h>

#include <Core/Service.h>

#include <QtCore/QProcess>

namespace P1 {
  namespace Host {

    class GameExecutorService;

    class ElectronExecutorPrivate : public QObject
    {
      Q_OBJECT

    public:
      explicit ElectronExecutorPrivate(QObject *parent);
      virtual ~ElectronExecutorPrivate();

      void execute(
        const P1::Core::Service &service,
        P1::GameExecutor::GameExecutorService *executorService,
        const P1::RestApi::ProtocolOneCredential& credential,
        const QString& scheme);

      void onShutdown(const QString& serviceId);

    private:
      void onGetRedirectTokenResult(P1::RestApi::CommandBase::CommandResults result);
      void activateElectron(quint32 pid);

    signals:
      void started(const P1::Core::Service &service);
      void finished(const P1::Core::Service &service, P1::GameExecutor::FinishState state);

    private:
      P1::Core::Service _service;
      P1::RestApi::ProtocolOneCredential _credential;
      QString _scheme;
    };
  }
}
