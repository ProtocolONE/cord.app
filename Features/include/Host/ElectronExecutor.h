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

#include <QObject>

namespace GameNet {
  namespace Host {

    class ElectronExecutor : public GGS::GameExecutor::ExecutorBase
    {
      Q_OBJECT
    public:
      explicit ElectronExecutor(QObject *parent = 0);
      virtual ~ElectronExecutor();

      virtual void execute(
        const GGS::Core::Service &service, 
        GGS::GameExecutor::GameExecutorService *executorService,
        const GGS::RestApi::GameNetCredential& credential,
        const GGS::RestApi::GameNetCredential& secondCredential = GGS::RestApi::GameNetCredential()) override;

    private:
      void internalFinished(const GGS::Core::Service &service, GGS::GameExecutor::FinishState state);
    };
  }
}
