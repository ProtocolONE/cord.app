#pragma once

#include <windows.h>

namespace Features {

  inline bool isWow64()
  {
    typedef NTSTATUS(WINAPI *fnNtQueryInformationProcess)(
      _In_      HANDLE           ProcessHandle,
      _In_      DWORD            ProcessInformationClass,
      _Out_     PVOID            ProcessInformation,
      _In_      ULONG            ProcessInformationLength,
      _Out_opt_ PULONG           ReturnLength
      );

    fnNtQueryInformationProcess queryProcessInfo = reinterpret_cast<fnNtQueryInformationProcess>(
      GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQueryInformationProcess"));
    if (!queryProcessInfo)
      return false;

    DWORD ProcessInformation = 0;
    DWORD infoType = 0x1A; // ProcessPriorityClass | ProcessExceptionPort
    NTSTATUS res = queryProcessInfo(GetCurrentProcess(), infoType, &ProcessInformation, 4u, 0);

    return (res >= 0) && (ProcessInformation != 0);
  }

}