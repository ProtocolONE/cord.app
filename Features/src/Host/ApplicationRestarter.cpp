#include <Host/ApplicationRestarter.h>
#include <Host/ShutdownManager.h>

#include <Helper/DebugLog.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QString>

#include <Windows.h>

namespace P1 {
  namespace Host {

    ApplicationRestarter::ApplicationRestarter(QObject* parent /*= 0*/)
      : _shutdown(nullptr)
      , QObject(parent)
    {
    }

    ApplicationRestarter::~ApplicationRestarter()
    {
    }

    void ApplicationRestarter::setShutdownManager(ShutdownManager *value)
    {
      Q_ASSERT(value);
      this->_shutdown = value;
    }

    void ApplicationRestarter::restartApplication(bool shouldStartWithSameArguments, bool isMinimized)
    {
      Q_ASSERT(this->_shutdown);

      QObject::connect(this->_shutdown, &ShutdownManager::shutdownCompleted,
        [this, shouldStartWithSameArguments, isMinimized]() {
          this->internalRestartApplication(shouldStartWithSameArguments, isMinimized);
      });

      this->_shutdown->shutdown();
    }

    void ApplicationRestarter::internalRestartApplication(bool shouldStartWithSameArguments, bool isMinimized)
    {
      QString dir = QCoreApplication::applicationDirPath();
      QString exe = QCoreApplication::applicationFilePath();

      QString commandLineArgs("");
      if (shouldStartWithSameArguments) {
        QStringList args = QCoreApplication::arguments();
        args.removeFirst();

        if (args.size() > 0) {
          commandLineArgs = args.join("\" \"");
          commandLineArgs.prepend(L'"');
          commandLineArgs.append(L'"');
        }

      } else {
        if (isMinimized)
          commandLineArgs = "/minimized";
      }

      if (!this->executeFile(dir, exe, commandLineArgs))
        WARNING_LOG << "Can't restart Launcher";

      QCoreApplication::quit();
    }

    bool ApplicationRestarter::executeFile(const QString& path, const QString& fileName, const QString& args)
    {
      SHELLEXECUTEINFOW shex;
      ZeroMemory(&shex, sizeof(shex));

      shex.cbSize			  = sizeof(SHELLEXECUTEINFO);
      shex.fMask			  = 0; 
      shex.hwnd			    = 0;
      shex.lpVerb			  = L"open"; 
      shex.lpFile			  = reinterpret_cast<const WCHAR*>(fileName.utf16()); 
      shex.lpParameters	= reinterpret_cast<const WCHAR*>(args.utf16()); 
      shex.lpDirectory	= reinterpret_cast<const WCHAR*>(path.utf16()); 
      shex.nShow			  = SW_NORMAL; 

      return ::ShellExecuteExW(&shex);
    }

  }
}

