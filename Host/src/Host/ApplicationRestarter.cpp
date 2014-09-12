#include <QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QString>
#include <QtCore/QDebug>

#include <windows.h>

#include <Host/ApplicationRestarter.h>
#include <Helper/DebugLog.h>

namespace GameNet {
  namespace Host {

    ApplicationRestarter::ApplicationRestarter(QObject* parent /*= 0*/)
      : _gameDownloadInitialized(false)
      , QObject(parent)
    {

    }

    ApplicationRestarter::~ApplicationRestarter()
    {

    }

    // UNDONE 
    // Илья переносит запускатор, поэтому пока нет возможности выполнить код ниже.
    // раскомментить это, и заставить работать после мержа с веткой Ильи
    //     void Application::restartApplicationAfterDriverDisconnect(Features::Thetta::ThettaInstaller::Result result)
    //     {
    // 
    //       if (result == Features::Thetta::ThettaInstaller::Running) // походу 2ой дисконнект
    //         return;
    // 
    //       this->internalRestartApplication(this->_restartArguments);
    //     }

    void ApplicationRestarter::restartApplication(bool shouldStartWithSameArguments, bool isMinimized)
    {
      this->internalRestartApplication(shouldStartWithSameArguments, isMinimized);

      // UNDONE 
      // Илья переносит запускатор, поэтому пока нет возможности выполнить код ниже.
      // раскомментить это, и заставить работать после мержа с веткой Ильи
      //
      //       if (!this->_serviceLoader.thettaInstaller()) {
      //         this->internalRestartApplication(shouldStartWithSameArguments);
      //         return;
      //       }
      // 
      //       using Features::Thetta::ThettaInstaller;
      // 
      //       QObject::connect(this->_serviceLoader.thettaInstaller(), &ThettaInstaller::disconnected,
      //         this, &Application::restartApplicationAfterDriverDisconnect, Qt::QueuedConnection);
      // 
      //       this->_restartArguments = shouldStartWithSameArguments;
      //       this->_serviceLoader.thettaInstaller()->disconnectFromDriver();
    }

    void ApplicationRestarter::internalRestartApplication(bool shouldStartWithSameArguments, bool isMinimized)
    {
      emit this->secondInstanceExecuteRequest();
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

      if (this->executeFile(dir, exe, commandLineArgs)) {
         if (this->_gameDownloadInitialized) {
           emit this->shutdownRequest();
        } else {
          DEBUG_LOG << "fast shutdownCompleted";
          QCoreApplication::quit();
        }
        return;
      }

      WARNING_LOG << "Can't restart qGNA";
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

    void ApplicationRestarter::setGameDownloadInitialized()
    {
      this->_gameDownloadInitialized = true;
    }

  }
}

