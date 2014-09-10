
#include <Host/UIProcess.h>
#include <Helper/DebugLog.h>

#include <QtCore/QWinEventNotifier>
#include <QtCore/QDir>

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

    UIProcess::UIProcess(QObject* parent) 
      : QObject(parent)
      , _processHandle(0)
    {
    }

    UIProcess::~UIProcess()
    {
      if (this->_processHandle)
        CloseHandle(this->_processHandle);
    }

    bool UIProcess::start(const QString& path, const QString& fileName, const QStringList& args)
    {
      if (this->_processHandle) {
        return false;
      }

      QString nativePath = QDir::toNativeSeparators(path);
      QString commandLine = QString();
      if (args.size() > 0) {
        commandLine = args.join("\" \"");
        commandLine.prepend(L'"');
        commandLine.append(L'"');
      }

      QStringToWChar exe(nativePath + QDir::separator() + fileName);
      QStringToWChar cmd(commandLine);
      QStringToWChar dir(nativePath);

      STARTUPINFO si;
      PROCESS_INFORMATION pi;
      ZeroMemory(&si, sizeof(STARTUPINFO));

      si.cb = sizeof(STARTUPINFO);

      if (!CreateProcessW(exe.data(), cmd.data(), 0, 0, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE, NULL, dir.data(), &si, &pi)) {
        DEBUG_LOG << "Create process failed" << GetLastError();
        return false;
      }

      if (pi.hProcess == INVALID_HANDLE_VALUE) {
        DEBUG_LOG << "Create process invalid handle" << GetLastError();
        return false;
      }

      this->_processHandle = pi.hProcess;
      
      QWinEventNotifier *monitor = new QWinEventNotifier(pi.hProcess, this);

      QObject::connect(monitor, &QWinEventNotifier::activated, 
        this, &UIProcess::handleActivated);

      return true;
    }

    bool UIProcess::isRunning()
    {
      return this->_processHandle != 0;
    }

    void UIProcess::handleActivated(HANDLE handle)
    {
      QWinEventNotifier *monitor = qobject_cast<QWinEventNotifier*>(QObject::sender());
      monitor->setEnabled(false);
      monitor->deleteLater();
      DWORD exitCode = 0;
      GetExitCodeProcess(handle, &exitCode);

      this->_processHandle = 0;

      DEBUG_LOG << "qGNA UI process finished with exit code" << exitCode;
    }

  }
}