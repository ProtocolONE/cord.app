#include <Features/TaskBarProgressHelper.h>

#include <QtWidgets/QMainWindow>
#include <QtWin>
#include <QtCore/QSysInfo>

#include <Windows.h>

namespace Features {

  typedef BOOL (WINAPI *CHANGEWINDOWMESSAGEFILTEREX)(HWND, UINT, DWORD, PCHANGEFILTERSTRUCT); 

  TaskBarHelper::TaskBarHelper(QObject *parent)
    : QObject(parent)
    , _appMainWindow(0)
    , _taskBarCreatedMsgId(0)
    , _lastStatus(StatusUnknown)
    , _lastProgress(0)
  {
    this->_isPlatformSupported = this->isPlatformSupported();
  }

  void TaskBarHelper::prepare(HWND hwnd)
  {
    if (!this->_isPlatformSupported) 
      return;

    this->_taskBarCreatedMsgId = RegisterWindowMessage(_T("TaskbarButtonCreated"));
    this->_appMainWindow = hwnd;

    //  for correct Windows XP Support
    HINSTANCE userDllInstance = GetModuleHandle(L"User32.dll"); 
    if (userDllInstance != NULL) { 
      CHANGEWINDOWMESSAGEFILTEREX changeMessageFilterExProc = (CHANGEWINDOWMESSAGEFILTEREX) GetProcAddress(userDllInstance, "ChangeWindowMessageFilterEx"); 

      if (changeMessageFilterExProc != NULL) {
        CHANGEFILTERSTRUCT cfs = { sizeof(CHANGEFILTERSTRUCT) };
        changeMessageFilterExProc(this->_appMainWindow, this->_taskBarCreatedMsgId, MSGFLT_ALLOW, &cfs);
      }
    }
  }


  unsigned int TaskBarHelper::getTaskbarCreatedMessageId() const
  {
    return this->_taskBarCreatedMsgId;
  }

  bool TaskBarHelper::isPlatformSupported() const
  {
    if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7)
      return true;

    return false;
  }

  void TaskBarHelper::init()
  {
    if (this->_isPlatformSupported) {
      this->_taskbarList.Release();
      this->_taskbarList.CoCreateInstance(CLSID_TaskbarList);
      this->restore();
    }
  }
  
  void TaskBarHelper::setProgress(int progress)
  {
    this->_lastProgress = progress;

    if (!this->_taskbarList)
      return;

    int progressValue = progress;

    if (progressValue < 0) {
      progressValue = 0;
    } else if (progressValue > 100) {
      progressValue = 100;
    }
    
    this->_taskbarList->SetProgressValue(this->_appMainWindow, progressValue, 100);
  }

  void TaskBarHelper::setStatus(Status status)
  {
    this->_lastStatus = status;

    if (!this->_taskbarList)
      return;

    TBPFLAG flag;

    switch(status){
    case StatusUnknown:
      flag = TBPF_NOPROGRESS;
      
     break;
    case StatusNormal:
      flag = TBPF_NORMAL;
      break;
    case StatusPaused:
      flag = TBPF_PAUSED;
      break;
    case StatusError:
      flag = TBPF_ERROR;
      break;
    }

    this->_taskbarList->SetProgressState(this->_appMainWindow, flag);
  }

  void TaskBarHelper::setIcon(const QString &iconSource, const QString &hint) 
  {
    this->_lastIcon = iconSource;
    
    if (!this->_taskbarList)
      return;

    QIcon icon = QIcon(iconSource);
    std::wstring wHint = hint.toStdWString();

    HICON hIcon = 0;

    if (!icon.isNull())
      hIcon = QtWin::toHICON(icon.pixmap(GetSystemMetrics(SM_CXSMICON)));

    HRESULT hr = this->_taskbarList->SetOverlayIcon(this->_appMainWindow, hIcon, wHint.c_str());

    if (hIcon != NULL)
      DestroyIcon(hIcon);
  }

  void TaskBarHelper::restore()
  {
    this->setProgress(this->_lastProgress);
    this->setStatus(this->_lastStatus);
    this->setIcon(this->_lastIcon);
  }
}
