/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2015, Syncopate Limited and/or affiliates. 
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/
#include <Helper/TerminateProcess.h>

#include <QtCore/QProcess>

#include <Windows.h>
#include <psapi.h>
#include <TlHelp32.h>

namespace Features {

  void executeAndWait(const QString& file, const QString& params)
  {
    QProcess process;
    process.start(file + " " + params);
    process.waitForFinished(-1);
  }

  void terminateProcessByName(const QString &procName)
  {
    executeAndWait("taskkill.exe", "/f /im " + procName);
    executeAndWait("tskill.exe", procName);

    wchar_t file_w[MAX_PATH];
    procName.toWCharArray(file_w);
    file_w[procName.count()] = '\0';

    DWORD count;
    DWORD processes[1024];
    if (!EnumProcesses(processes, sizeof(processes), &count))
      return;

    count /= sizeof(DWORD);

    for (unsigned int i = 0; i < count; i++) {
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
  }

  void terminateProcessByName(const QString &procName, bool trySoft, int timeout)
  {
    HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
      return;

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!::Process32First(hProcessSnap, &pe32)) {
      ::CloseHandle(hProcessSnap);
      return;
    }

    do {
      if (0 == _wcsicoll(procName.toStdWString().c_str(), pe32.szExeFile) && pe32.cntThreads > 0) {

        ::CloseHandle(hProcessSnap);
        HANDLE process = ::OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE, FALSE, pe32.th32ProcessID);
        if (process == nullptr)
          return;

        if (trySoft) {

          HANDLE hThreadSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pe32.th32ProcessID);
          if (hProcessSnap == INVALID_HANDLE_VALUE)
            return;

          THREADENTRY32 th32;
          th32.dwSize = sizeof(THREADENTRY32);
          if (!::Thread32First(hThreadSnap, &th32)) {
            ::CloseHandle(hThreadSnap);
            return;
          }

          do {
            if (th32.th32OwnerProcessID == pe32.th32ProcessID)
              ::PostThreadMessage(th32.th32ThreadID, WM_QUIT, 0, 0);
          } while (::Thread32Next(hThreadSnap, &th32));

          ::CloseHandle(hThreadSnap);
        }

        if (!trySoft || WAIT_OBJECT_0 != ::WaitForSingleObject(process, timeout))
          ::TerminateProcess(process, 0); // force terminate

        return;
      }
    } while (::Process32Next(hProcessSnap, &pe32));

    ::CloseHandle(hProcessSnap);
    return;
  }
}