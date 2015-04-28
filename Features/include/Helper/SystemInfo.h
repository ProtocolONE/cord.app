#pragma once

#include <windows.h>

namespace Features {

  bool isWow64()
  {
    typedef BOOL (APIENTRY *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
    LPFN_ISWOW64PROCESS fnIsWow64Process;

    fnIsWow64Process = reinterpret_cast<LPFN_ISWOW64PROCESS>(
                         GetProcAddress(GetModuleHandleW(L"kernel32"), "IsWow64Process"));

    if (!fnIsWow64Process)
      return false;

    BOOL bIsWow64 = FALSE;
    if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64)) {
      DWORD error = GetLastError();
      return false;
    }
    return bIsWow64 != FALSE;
  }

}