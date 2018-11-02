#include <HostProcess.h>
#include <Host/Installer/UninstallResult.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QThread>

#include <Application/ArgumentParser.h>

#include <Windows.h>

using namespace P1::Host::Installer;

HostProcess::HostProcess(QObject* parent) 
  : QObject(parent)
  , _waitAttempts(45)
{
}

HostProcess::~HostProcess()
{

}

void HostProcess::start()
{
  P1::Application::ArgumentParser argumentsParser;
  argumentsParser.parse(QCoreApplication::arguments());

  if (!argumentsParser.contains("uninstall")) {
    emit this->uninstallFinished(static_cast<int>(UninstallResult::Failed));
    return;
  }

  QStringList commandArguments = argumentsParser.commandArguments("uninstall");
  if (commandArguments.empty()) {
    emit this->uninstallFinished(static_cast<int>(UninstallResult::Failed));
    return;
  }

  this->_serviceId = commandArguments.at(0);

  UninstallResult *uninstallResult = new UninstallResult(this->_serviceId, this);
  QObject::connect(uninstallResult, &UninstallResult::finished, this, &HostProcess::onUninstallFinished, Qt::QueuedConnection);
  uninstallResult->asyncWaitResult();


  QString launcherPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath()) + "\\launcher.hosh.exe";

  QProcess::startDetached(launcherPath, QCoreApplication::arguments());

  this->_waitAttempts = 45;
  connect(&this->_waitTimer, SIGNAL(timeout()), this, SLOT(testLauncherReady()));
  this->_waitTimer.start(1000);
}

void HostProcess::onUninstallFinished(int result)
{
  emit this->uninstallFinished(result);
}

void HostProcess::testLauncherReady()
{
  WCHAR *mutexName = L"Global\\{0DF08C1B-5888-4623-A414-398C01CF831B}";

  HANDLE hMutex = ::OpenMutex(SYNCHRONIZE, FALSE, mutexName);
  if (hMutex != NULL) {
    CloseHandle(hMutex);
    return;
  }

  this->_waitAttempts--;

  if (this->_waitAttempts == 0) {
    this->_waitTimer.stop();
    emit this->uninstallFinished(static_cast<int>(UninstallResult::Failed));
  }
}