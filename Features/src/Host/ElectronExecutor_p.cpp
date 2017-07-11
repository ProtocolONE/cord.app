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
#include <Host/ElectronExecutor_p.h>
#include <RestApi/Auth/GetRedirectToken.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QUrlQuery>
#include <QtCore/QDebug>

#include <sstream>

namespace GameNet {
  namespace Host {

    using namespace GGS::GameExecutor;
    using namespace GGS::RestApi::Auth;

    ElectronExecutorPrivate::ElectronExecutorPrivate(QObject *parent) : QObject(parent), _process(this)
    {

    }

    ElectronExecutorPrivate::~ElectronExecutorPrivate()
    {
    }

    void ElectronExecutorPrivate::execute(
      const GGS::Core::Service &service,
      GGS::GameExecutor::GameExecutorService *executorService,
      const GGS::RestApi::GameNetCredential& credential,
      const GGS::RestApi::GameNetCredential& secondCredential, 
      const QString& scheme)
    {

      this->_service = service;
      this->_credential = !secondCredential.userId().isEmpty() ? secondCredential : credential;
      this->_scheme = _scheme;

      GetRedirectToken* command = new GetRedirectToken;
      command->setAuthRequire(false);
      command->appendParameter("userId", this->_credential.userId());
      command->appendParameter("appKey", this->_credential.appKey());

      QObject::connect(command, &GetRedirectToken::result, this, &ElectronExecutorPrivate::onGetRedirectTokenResult, Qt::DirectConnection);
      
      command->execute();
    }

    void ElectronExecutorPrivate::onGetRedirectTokenResult(GGS::RestApi::CommandBase::CommandResults result)
    {
      GetRedirectToken *command = qobject_cast<GetRedirectToken*>(QObject::sender());
      if (!command)
        return;

      command->deleteLater();

      if (result != GGS::RestApi::CommandBase::NoError) {

        int errorCode = command->getGenericErrorMessageCode();
        WARNING_LOG << "GetRedirectToken result error: " << errorCode;

        FinishState state = (GGS::RestApi::CommandBase::GenericError == result)
          ? ExecutorBase::finishStateFromRestApiErrorCode(errorCode) : ExternalFatalError;

        this->finished(this->_service, state);
        return;
      }

      QUrl url = this->_service.url();
      if (this->_scheme == "electron")
        url.setScheme("http");
      else if (this->_scheme == "electrons")
        url.setScheme("https");

      QObject::connect(&_process, &QProcess::started, this, &ElectronExecutorPrivate::onStarted, Qt::DirectConnection);

      QObject::connect(&_process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), 
        this, &ElectronExecutorPrivate::onFinished, Qt::DirectConnection);

      QObject::connect(&_process, static_cast<void(QProcess::*)(QProcess::ProcessError)>(&QProcess::error), 
        this, &ElectronExecutorPrivate::onError, Qt::DirectConnection);

      QString path = QDir::cleanPath(QCoreApplication::applicationDirPath() + "\\WebPlayer\\WebPlayer.exe");
      this->_process.setProgram(path);

      QStringList arg;
      arg << this->_credential.userId() << command->token() << url.toString();
      this->_process.setArguments(arg);

      this->_process.start();
    }

    void ElectronExecutorPrivate::onStarted()
    {
      emit this->started(this->_service);
    }

    void ElectronExecutorPrivate::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
    {
      if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
        emit this->finished(this->_service, Success);
        return;
      }

      CRITICAL_LOG << "Process is finished with error code: " << exitCode;

      if (exitCode == 0 && exitStatus == QProcess::CrashExit)
        this->internalError(-1);
      else
        this->internalError(exitCode);

    }

    void ElectronExecutorPrivate::onError(QProcess::ProcessError error)
    {
      if (_shutdown) {
        emit this->finished(this->_service, Success);
        return;
      }

      CRITICAL_LOG << "Process failed with error: " << error;
      this->internalError(-1);
    }

    void ElectronExecutorPrivate::onShutdown(const QString& serviceId)
    {
      if (!serviceId.isEmpty() && this->_service.id() != serviceId)
        return;

      _shutdown = true;
      this->_process.close();
    }

    void ElectronExecutorPrivate::internalError(int errorCode)
    {
      QUrl url = this->_service.url();
      QUrlQuery urlQuery(url);
      urlQuery.addQueryItem("exitCode", QString::number(errorCode));
      url.setQuery(urlQuery);

      this->_service.setUrl(url);
      emit this->finished(this->_service, ExternalFatalError);
    }
  }
}
