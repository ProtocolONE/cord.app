
#include <Host/UIProcess.h>
#include <Helper/DebugLog.h>
#include <Helper/TerminateProcess.h>

#include <QtCore/QWinEventNotifier>
#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QSet>

#include <Windows.h>
#include <psapi.h>

#include <Application/ArgumentParser.h>

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

      bool start(const QStringList& args, bool skipIgnore = false)
      {
        GGS::Application::ArgumentParser uiSendedArgs;
        uiSendedArgs.parse(args);

        if (!skipIgnore) {
          QStringList cmds = uiSendedArgs.cachedCommands();
          Q_FOREACH(const QString& cmd, cmds) {
            this->_onetimeIgnoreCommands.insert(cmd);
          }
        }

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
        QMutexLocker locker(&this->_mutex);
        if (this->_threadHandle != INVALID_HANDLE_VALUE)
          CloseHandle(this->_threadHandle);

        this->_threadHandle = INVALID_HANDLE_VALUE;

        if (this->_processHandle != INVALID_HANDLE_VALUE) { 
          WaitForSingleObject(this->_processHandle, 2000);
          TerminateProcess(this->_processHandle, 0);
          CloseHandle(this->_processHandle);
        }

        this->_processHandle = INVALID_HANDLE_VALUE;
      }

      bool checkOnetimeIgnoreCommand(const QString& name)
      {
        if (!this->_onetimeIgnoreCommands.contains(name))
          return false;

        qDebug() << "Ignore one time command " << name;
        this->_onetimeIgnoreCommands.remove(name);
        return true;
      }

      UIProcess *_parent;
      QString _directory;
      QString _filename;
      HANDLE _processHandle;
      HANDLE _threadHandle;
      QMutex _mutex;

      QSet<QString> _onetimeIgnoreCommands;
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
      if (this->_d->checkOnetimeIgnoreCommand(name))
        return;

      QString arg = args.join('/');
      QString commandLine = QString("/uri:gamenet://%1/%2").arg(name, arg);
      QStringList processArgs;
      processArgs << commandLine;
      this->_d->start(processArgs, true);
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

    void UIProcess::closeUI()
    {
      this->_d->closeHandle();
    }

    void UIProcess::destroyRunningApplication(const QString& name)
    {
      if (!this->isStarted())
        return;

      Features::terminateProcessByName(name);
    }

    void UIProcess::destroyProcess()
    {
      this->destroyRunningApplication(this->_d->_filename);
    }

    bool UIProcess::isStarted()
    {
      const std::string name("Global\\{34688F78-432F-4C5A-BFC7-CD1BC88A30CC}");
      HANDLE h = OpenMutexA(MUTEX_ALL_ACCESS, 0, name.c_str());
      bool result = h != NULL;

      if (result)
        CloseHandle(h);

      return result;
    }
  }
}
      wchar_t file_w[MAX_PATH];
      name.toWCharArray(file_w);
      file_w[name.count()] = '\0';

      DWORD count;
      DWORD processes[1024];
      if (!EnumProcesses(processes, sizeof(processes), &count)){
        return;

      count /= sizeof(DWORD);

      for (unsigned int i = 0; i < count; i++) {
        wchar_t szProcessName[MAX_PATH] = TEXT("<unknown>");

        if (processes[i] == 0) 
          continue;

        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION, FALSE, processes[i]);

        if (hProcess == NULL)
          continue;

        wchar_t processFileName[MAX_PATH];

        if (GetProcessImageFileName(hProcess, processFileName, MAX_PATH) == 0)
          continue;

        if (wcsstr(processFileName, file_w) != 0) {
          DWORD result = WAIT_OBJECT_0;

          while(result == WAIT_OBJECT_0) {
            result = WaitForSingleObject(hProcess, 100);
            TerminateProcess(hProcess, 0);
          }
        }

        CloseHandle(hProcess);
      }
#include <QtCore/QProcess>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
