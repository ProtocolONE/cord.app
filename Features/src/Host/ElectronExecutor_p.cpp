
#include <GameExecutor/GameExecutorService.h>
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

    using namespace P1::GameExecutor;
    using namespace P1::RestApi::Auth;

    ElectronExecutorPrivate::ElectronExecutorPrivate(QObject *parent) 
      : QObject(parent)
    {
    }

    ElectronExecutorPrivate::~ElectronExecutorPrivate()
    {
    }

    void ElectronExecutorPrivate::execute(
      const P1::Core::Service &service,
      P1::GameExecutor::GameExecutorService *executorService,
      const P1::RestApi::GameNetCredential& credential,
      const QString& scheme)
    {
      if (QSysInfo::WindowsVersion == QSysInfo::WV_XP) {
        qWarning() << "Electron does not support Windows Xp";
      }

      this->_service = service;
      this->_credential = credential;
      this->_scheme = scheme;

      GetRedirectToken* command = new GetRedirectToken(this);
      command->setAuthRequire(false);
      command->appendParameter("userId", this->_credential.userId());
      command->appendParameter("appKey", this->_credential.appKey());

      QObject::connect(command, &GetRedirectToken::result, 
        this, &ElectronExecutorPrivate::onGetRedirectTokenResult, Qt::DirectConnection);
      
      command->execute();
    }

    void ElectronExecutorPrivate::onGetRedirectTokenResult(P1::RestApi::CommandBase::CommandResults result)
    {
      GetRedirectToken *command = qobject_cast<GetRedirectToken*>(QObject::sender());
      if (!command)
        return;

      command->deleteLater();

      if (result != P1::RestApi::CommandBase::NoError) {

        int errorCode = command->getGenericErrorMessageCode();
        WARNING_LOG << "GetRedirectToken result error: " << errorCode;

        FinishState state = (P1::RestApi::CommandBase::GenericError == result)
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
        emit this->finished(this->_service, P1::GameExecutor::ExternalFatalError);
        return;
      }

      ::WaitForInputIdle(pi.hProcess, INFINITE);
      CloseHandle(pi.hThread);
      CloseHandle(pi.hProcess);

      emit this->finished(this->_service, P1::GameExecutor::Success);

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

            P1::Application::WindowHelper::activate(hwnd);

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
