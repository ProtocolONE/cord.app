
#include <Host/UIProcess.h>
#include <Helper/DebugLog.h>

#include <QtCore/QWinEventNotifier>
#include <QtCore/QDir>

#include <Windows.h>

namespace GameNet {
  namespace Host {

    class QStringToWChar {
    public:
      QStringToWChar(const QString& str) : _data(NULL)
      {
        this->_size = str.size();
        this->_data = new wchar_t[this->_size + 1]();
        str.toWCharArray(this->_data);
      }

      ~QStringToWChar() 
      {
        delete [] _data;
      }

      wchar_t* data() { return this->_data; }
      size_t size() { return this->_size; }

    private:
      wchar_t *_data;
      size_t _size;
    };

    class UIProcessPrivate 
    {
    public:
      UIProcessPrivate(UIProcess *parent)
        : _parent(parent)
        , _processHandle(INVALID_HANDLE_VALUE)
        , _threadHandle(INVALID_HANDLE_VALUE)
      {
      }

      ~UIProcessPrivate()
      {
        this->closeHandle();
      }

      bool start(const QStringList& args)
      {
        QString nativePath = QDir::toNativeSeparators(this->_directory);
        QString commandLineArgs = QString();
        if (args.size() > 0) {
          commandLineArgs = args.join("\" \"");
          commandLineArgs.prepend(L'"');
          commandLineArgs.append(L'"');
        }

        QString fullExePath = nativePath + QDir::separator() + this->_filename;
        QString commandLine = QString("\"%1\" %2").arg(fullExePath, commandLineArgs);

        QStringToWChar exe(fullExePath);
        QStringToWChar cmd(commandLine);
        QStringToWChar dir(nativePath);

        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(STARTUPINFO));
        si.cb = sizeof(STARTUPINFO);

        BOOL started = CreateProcessW(
          exe.data(),
          cmd.data(),
          0,
          0,
          FALSE,
          NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,
          NULL,
          dir.data(),
          &si,
          &pi);

        if (!started) {
          DEBUG_LOG << "Create process failed" << GetLastError();
          return false;
        }

        if (pi.hProcess == INVALID_HANDLE_VALUE) {
          DEBUG_LOG << "Create process invalid handle" << GetLastError();
          return false;
        }

        if (this->isRunning()) {
          CloseHandle(pi.hThread);
          CloseHandle(pi.hProcess);
          return true;
        }

        this->_processHandle = pi.hProcess;
        this->_threadHandle = pi.hThread;

        QWinEventNotifier *monitor = new QWinEventNotifier(pi.hProcess, this->_parent);
        QObject::connect(monitor, &QWinEventNotifier::activated, 
          this->_parent, &UIProcess::processFinished);

        return true;
      }

      inline bool isRunning() 
      {
        return this->_processHandle != INVALID_HANDLE_VALUE;
      }

      inline void closeHandle() 
      {
        if (this->_threadHandle != INVALID_HANDLE_VALUE)
          CloseHandle(this->_threadHandle);

        this->_threadHandle = INVALID_HANDLE_VALUE;

        if (this->_processHandle != INVALID_HANDLE_VALUE) {
          TerminateProcess(this->_processHandle, 0);
          CloseHandle(this->_processHandle);
        }

        this->_processHandle = INVALID_HANDLE_VALUE;
      }

      UIProcess *_parent;
      QString _directory;
      QString _filename;
      HANDLE _processHandle;
      HANDLE _threadHandle;
    };

    UIProcess::UIProcess(QObject* parent) 
      : QObject(parent)
      , _d(new UIProcessPrivate(this))
    {
    }

    UIProcess::~UIProcess()
    {
      delete this->_d;
    }

    void UIProcess::setDirectory(const QString& value)
    {
      this->_d->_directory = value;
    }

    void UIProcess::setFileName(const QString& value)
    {
      this->_d->_filename = value;
    }

    bool UIProcess::start(const QStringList& args)
    {
      return this->_d->start(args);
    }

    bool UIProcess::isRunning()
    {
      return this->_d->isRunning();
    }

    void UIProcess::sendCommand(const QString& name, const QStringList& args)
    {
      QString arg = args.join('/');
      QString commandLine = QString("/uri:gamenet://%1/%2").arg(name, arg);
      QStringList processArgs;
      processArgs << commandLine;
      this->start(processArgs);
    }

    void UIProcess::processFinished()
    {
      QWinEventNotifier *monitor = qobject_cast<QWinEventNotifier*>(QObject::sender());
      monitor->setEnabled(false);
      monitor->deleteLater();
      
      DWORD exitCode = 0;
      GetExitCodeProcess(this->_d->_processHandle, &exitCode);
      DEBUG_LOG << "qGNA UI process finished with exit code" << exitCode;

      this->_d->closeHandle();
    }

  }
}