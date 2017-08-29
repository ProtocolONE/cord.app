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
#include <Application/WindowHelper.h>
#include <GameExecutor/Executor/WebLink.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QUrlQuery>
#include <QtCore/QDebug>

#include <sstream>

namespace GameNet {
  namespace Host {

    using namespace GGS::GameExecutor;
    using namespace GGS::RestApi::Auth;

    ElectronExecutorPrivate::ElectronExecutorPrivate(QObject *parent) 
      : QObject(parent)
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

      if (QSysInfo::WindowsVersion == QSysInfo::WV_XP) {
        GGS::GameExecutor::Executor::WebLink* webLink = new GGS::GameExecutor::Executor::WebLink(this);

        QObject::connect(webLink, &GGS::GameExecutor::Executor::WebLink::started, this, &ElectronExecutorPrivate::started, Qt::DirectConnection);
        QObject::connect(webLink, &GGS::GameExecutor::Executor::WebLink::finished, this, &ElectronExecutorPrivate::finished, Qt::DirectConnection);

        webLink->execute(service, executorService, secondCredential);
        return;
      }

      this->_service = service;
      this->_credential = !secondCredential.userId().isEmpty() ? secondCredential : credential;
      this->_scheme = scheme;

      GetRedirectToken* command = new GetRedirectToken(this);
      command->setAuthRequire(false);
      command->appendParameter("userId", this->_credential.userId());
      command->appendParameter("appKey", this->_credential.appKey());

      QObject::connect(command, &GetRedirectToken::result, 
        this, &ElectronExecutorPrivate::onGetRedirectTokenResult, Qt::DirectConnection);
      
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

      QString path = '\"' + QDir::toNativeSeparators(QDir::cleanPath(QCoreApplication::applicationDirPath() + "\\WebPlayer\\WebPlayer.exe")) + '\"';
      path += " " + this->_credential.userId() + " " + command->token() + " " + url.toString();

      PROCESS_INFORMATION pi = { 0 };
      STARTUPINFO si = { 0 };
      si.cb = sizeof(si);
      si.dwFlags = STARTF_USESHOWWINDOW;
      si.wShowWindow = SW_MAXIMIZE;

      emit this->started(this->_service);

      std::vector<wchar_t> cmd(path.utf16(), path.utf16() + path.size());
      if (!::CreateProcessW(nullptr, cmd.data(), nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi)) {

        DEBUG_LOG << "Failed to execute " << path;
        emit this->finished(this->_service, GGS::GameExecutor::ExternalFatalError);
        return;
      }

      ::WaitForInputIdle(pi.hProcess, INFINITE);
      CloseHandle(pi.hThread);
      CloseHandle(pi.hProcess);

      emit this->finished(this->_service, GGS::GameExecutor::Success);

      activateElectron(pi.dwProcessId);
    }

    void ElectronExecutorPrivate::activateElectron(quint32 pid)
    {
      qDebug() << "ElectronExecutorPrivate::activateElectron start";

      std::pair<LPARAM, bool> param = std::make_pair(pid, false);

      for (int i = 0; i < 10 && !param.second; i++) {

        ::EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL
        {
          std::pair<LPARAM, bool>* param = reinterpret_cast<std::pair<LPARAM, bool>*>(lParam);
          DWORD processId;

          ::GetWindowThreadProcessId(hwnd, &processId);
          if (processId == param->first && ::GetWindow(hwnd, GW_OWNER) == 0 && ::IsWindowVisible(hwnd)) {

            ::AllowSetForegroundWindow(::GetCurrentProcessId());
            ::AllowSetForegroundWindow(processId);

            GGS::Application::WindowHelper::activate(hwnd);

            param->second = true;
            return false;
          }
          return true;
        }, reinterpret_cast<LPARAM>(&param));

        ::Sleep(500);
      }

      qDebug() << "ElectronExecutorPrivate::activateElectron finish";
    }

    void ElectronExecutorPrivate::onShutdown(const QString& serviceId)
    {
      if (!serviceId.isEmpty() && this->_service.id() != serviceId)
        return;
    }
  }
}
