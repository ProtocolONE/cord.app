/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#pragma once

#include <GameExecutor/gameexecutor_global.h>
#include <GameExecutor/ExecutorBase.h>

#include <Core/Service>

#include <QtCore/QProcess>

namespace GameNet {
  namespace Host {

    class GameExecutorService;

    class ElectronExecutorPrivate : public QObject
    {
      Q_OBJECT

    public:
      explicit ElectronExecutorPrivate(QObject *parent);
      virtual ~ElectronExecutorPrivate();

      void execute(
        const GGS::Core::Service &service,
        GGS::GameExecutor::GameExecutorService *executorService,
        const GGS::RestApi::GameNetCredential& credential,
        const GGS::RestApi::GameNetCredential& secondCredential, 
        const QString& scheme);

      void onShutdown(const QString& serviceId);

    private:
      void internalError(int errorCode);
      void onGetRedirectTokenResult(GGS::RestApi::CommandBase::CommandResults result);
      void onGenericError(GGS::RestApi::CommandBase::Error error, QString message);

    signals:
      void started(const GGS::Core::Service &service);
      void finished(const GGS::Core::Service &service, GGS::GameExecutor::FinishState state);

    private:
      void onStarted();
      void onFinished(int exitCode, QProcess::ExitStatus exitStatus);
      void onError(QProcess::ProcessError error);

    private:
      GGS::Core::Service _service;
      GGS::RestApi::GameNetCredential _credential;
      QString _scheme;
      QProcess _process;
      bool _shutdown = false;
    };
  }
}
