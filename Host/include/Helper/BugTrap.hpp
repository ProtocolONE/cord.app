#pragma once

#include <version.h>

#include <BugTrap/BugTrap.h>

#define PASTE(x, y) x##y
#define MAKEWIDE(x) PASTE(L,x)

namespace GameNet {
  namespace Host {

    void initBugTrap(const QString &path)
    {
      BT_SetAppName(L"QGNA Host Application");
      BT_SetAppVersion(MAKEWIDE(STRFILEVER));
      BT_SetSupportEMail(L"support@gamenet.ru");
      BT_SetSupportURL(L"https://support.gamenet.ru");
      BT_SetFlags(BTF_DETAILEDMODE | BTF_ATTACHREPORT | BTF_SCREENCAPTURE);
      BT_SetSupportServer(L"fs1.gamenet.ru", 9999);
      BT_AddLogFile(reinterpret_cast<const wchar_t*>(QString(path + "/qgna.log").utf16()));
      BT_AddLogFile(reinterpret_cast<const wchar_t*>(QString(path + "/qgna.log.1").utf16()));
      BT_AddLogFile(reinterpret_cast<const wchar_t*>(QString(path + "/host.log").utf16()));
      BT_AddLogFile(reinterpret_cast<const wchar_t*>(QString(path + "/host.log.1").utf16()));
      BT_InstallSehFilter();
    }
  }
}