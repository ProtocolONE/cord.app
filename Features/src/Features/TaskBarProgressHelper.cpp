/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2013, Syncopate Limited and/or affiliates. 
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#include <Features/TaskBarProgressHelper.h>

#include <QtGui/QMainWindow>

#include <Windows.h>

namespace Features {

  typedef BOOL (WINAPI *CHANGEWINDOWMESSAGEFILTEREX)(HWND, UINT, DWORD, PCHANGEFILTERSTRUCT); 

  TaskBarHelper::TaskBarHelper(QObject *parent)
    : QObject(parent)
    , _appMainWindow(0)
    , _taskBarCreatedMsgId(0)
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
    DWORD dwMajor = LOBYTE(LOWORD(GetVersion()));
    DWORD dwMinor = HIBYTE(LOWORD(GetVersion()));

    // Check that the OS is Win 7 or later (Win 7 is v6.1).
    if (dwMajor > 6 || (dwMajor == 6 && dwMinor > 0))
      return true;

    return false;
  }

  void TaskBarHelper::init()
  {
    if (this->_isPlatformSupported) {
      this->_taskbarList.Release();
      this->_taskbarList.CoCreateInstance(CLSID_TaskbarList);
    }
  }

  void TaskBarHelper::setProgress(int progress)
  {
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
}