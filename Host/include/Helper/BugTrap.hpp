#pragma once

#include <version.h>

#include <BugTrap/BugTrap.h>

#define PASTE(x, y) x##y
#define MAKEWIDE(x) PASTE(L,x)

namespace P1 {
  namespace Host {

    void initBugTrap(const QString &path)
    {
      BT_SetAppName(L"ProtocolOne Launcher Host Application");
      BT_SetAppVersion(MAKEWIDE(STRFILEVER));
      BT_SetSupportEMail(L"support@protocol.one");
      BT_SetSupportURL(L"https://support.protocol.one");
      BT_SetFlags(BTF_DETAILEDMODE | BTF_ATTACHREPORT | BTF_SCREENCAPTURE);
      BT_SetSupportServer(L"fs1.protocol.one", 9999);
      BT_AddLogFile(reinterpret_cast<const wchar_t*>(QString(path + "/launcher.ui.log").utf16()));
      BT_AddLogFile(reinterpret_cast<const wchar_t*>(QString(path + "/launcher.ui.log.1").utf16()));
      BT_AddLogFile(reinterpret_cast<const wchar_t*>(QString(path + "/launcher.host.log").utf16()));
      BT_AddLogFile(reinterpret_cast<const wchar_t*>(QString(path + "/launcher.host.log.1").utf16()));
      BT_InstallSehFilter();
    }
  }
}