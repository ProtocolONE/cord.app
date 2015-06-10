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
}