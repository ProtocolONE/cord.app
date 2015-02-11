#include <HostProcess.h>
#include <Host/Installer/UninstallResult.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QThread>

#include <Application/ArgumentParser.h>

#include <Windows.h>

using namespace GameNet::Host::Installer;

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
  GGS::Application::ArgumentParser argumentsParser;
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


  QString qgnaPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath()) + "\\qGNA.exe";

  QProcess::startDetached(qgnaPath, QCoreApplication::arguments());

  this->_waitAttempts = 45;
  connect(&this->_waitTimer, SIGNAL(timeout()), this, SLOT(testQGNAReady()));
  this->_waitTimer.start(1000);
}

void HostProcess::onUninstallFinished(int result)
{
  emit this->uninstallFinished(result);
}

void HostProcess::testQGNAReady()
{
  WCHAR *mutexName = L"Global\\{CCC143CA-F620-41B2-A3DD-CB5DFAEE5DD7}";

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