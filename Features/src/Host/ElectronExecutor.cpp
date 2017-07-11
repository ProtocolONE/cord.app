/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#include <GameExecutor/GameExecutorService.h>
#include <GameExecutor/Extension.h>
#include <Host/ElectronExecutor.h>
#include <Host/ElectronExecutor_p.h>

#include <RestApi/RestApiManager>
#include <RestApi/GameNetCredential>

#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>
#include <QtCore/QDebug>

#include <qt_windows.h>
#include <shellapi.h>

namespace GameNet {
  namespace Host {

    using namespace GGS::GameExecutor;

    ElectronExecutor::ElectronExecutor(QObject *parent) : ExecutorBase(parent)
    {
      this->_scheme = QString("electron");
    }

    ElectronExecutor::~ElectronExecutor()
    {
    }

    void ElectronExecutor::execute(
      const GGS::Core::Service &service,
      GGS::GameExecutor::GameExecutorService *executorService,
      const GGS::RestApi::GameNetCredential& credential,
      const GGS::RestApi::GameNetCredential& secondCredential)
    {
      QString id = service.id();
      ElectronExecutorPrivate* executor = new ElectronExecutorPrivate(this);

      QObject::connect(executor, &ElectronExecutorPrivate::started, this, &ElectronExecutor::started, Qt::DirectConnection);
      QObject::connect(executor, &ElectronExecutorPrivate::finished, this, &ElectronExecutor::internalFinished, Qt::DirectConnection);
      QObject::connect(executorService, &GGS::GameExecutor::GameExecutorService::stopExecution, executor, &ElectronExecutorPrivate::onShutdown);

      executor->execute(service, executorService, credential, secondCredential, this->_scheme);
    }

    void ElectronExecutor::internalFinished(const GGS::Core::Service &service, FinishState state)
    {
      QObject::sender()->deleteLater();
      emit this->finished(service, state);
    }
  }
}
