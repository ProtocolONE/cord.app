#include <version.h>

#include <QtCore/QObject>
#include <QtWidgets/QApplication>

#include <Windows.h>
#include <BugTrap/BugTrap.h>

#include <HostProcess.h>

#include <tchar.h>

void initBugTrap(const QString &path)
{
  BT_SetAppName(_T("ProtocolOne Launcher Uninstaller"));
  BT_SetAppVersion(_T(STRFILEVER));
  BT_SetSupportEMail(_T("support@protocol.one"));
  BT_SetSupportURL(_T("https://support.protocol.one"));
  BT_SetFlags(BTF_DETAILEDMODE | BTF_ATTACHREPORT | BTF_SCREENCAPTURE);
  BT_SetSupportServer(_T("fs1.protocol.one"), 9999);
  BT_AddLogFile(reinterpret_cast<const wchar_t*>(QString(path + "/launcher.ui.log").utf16()));
  BT_AddLogFile(reinterpret_cast<const wchar_t*>(QString(path + "/launcher.ui.log.1").utf16()));
  BT_AddLogFile(reinterpret_cast<const wchar_t*>(QString(path + "/launcher.host.log").utf16()));
  BT_AddLogFile(reinterpret_cast<const wchar_t*>(QString(path + "/launcher.host.log.1").utf16()));
  BT_InstallSehFilter();
}

int main(int argc, char *argv[])
{
  int exitCode = 0;
  QApplication app(argc, argv);

  HostProcess hostProcess;
  QObject::connect(&hostProcess, &HostProcess::uninstallFinished, [&exitCode, &app] (int result) {
    exitCode = result;
    app.quit();
  });
  hostProcess.start();

  app.exec();

  return exitCode;
}
