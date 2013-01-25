#include "mainwindow.h"
#include "viewmodel\SettingsViewModel.h"

#include <version.h>

#include <Features/RemouteStartGame.h>
#include <Features/RememberGameDownloading.h>
#include <Features/ThronInstaller.h>

#include <Core/Marketing.h>
#include <Core/System/Shell/UrlProtocolHelper.h>

#include <ResourceHelper/ResourceLoader.h>

#include <Settings/Settings>
#include <Settings/InitializeHelper>

#include <AutoRunHelper/AutoRunHelper.h>

#include <Application/SingleApplication.h>

#include <QtCore/QDebug>

#include <QtGui/QApplication>
#include <QResource>

#include <Log4Qt/LogManager>
#include <Log4Qt/Level>
#include <Log4Qt/FileAppender>
#include <Log4Qt/TTCCLayout>

#include <BugTrap\BugTrap.h>

using namespace Log4Qt;

#define SIGNAL_CONNECT_CHECK(X) { bool result = X; Q_ASSERT_X(result, __FUNCTION__ , #X); }
#define CRITICAL_LOG qCritical() << __FILE__ << __LINE__ << __FUNCTION__

bool initDatabase() 
{
  GGS::Settings::InitializeHelper helper;
  helper.setFileName(QString("%1/settings.sql").arg(QCoreApplication::applicationDirPath()));
  if (!helper.init())
    return false;

  if (helper.isRecreated())
  {
    SettingsViewModel settingsViewModel;
    settingsViewModel.setDefaultSettings();
  }

  GGS::Settings::Settings::setCacheEnabled(true);
  return true;
}

void initBugTrap()
{
    BT_SetAppName(_T("QGNA Application"));
    BT_SetAppVersion(_T(STRFILEVER));
    BT_SetSupportEMail(_T("support@gamenet.ru"));
    BT_SetSupportURL(_T("https://support.gamenet.ru"));
    BT_SetFlags(BTF_DETAILEDMODE | BTF_ATTACHREPORT | BTF_SCREENCAPTURE);
    BT_SetSupportServer(_T("fs1.gamenet.ru"), 9999);
    BT_InstallSehFilter();
}

int main(int argc, char *argv[]) 
{
  GGS::Application::SingleApplication app(argc, argv, "{34688F78-432F-4C5A-BFC7-CD1BC88A30CC}");
  app.setIpcPortPath("HKEY_CURRENT_USER\\Software\\GGS\\QGNA");
  app.setWindowIcon(QIcon(QCoreApplication::applicationDirPath() + "/images/icon.png"));

  initBugTrap();
  
  if (app.isAlreadyRunning()) {
    QObject::connect(&app, SIGNAL(sendMessageFinished()), &app, SLOT(quit()), Qt::QueuedConnection);
    QStringList arguments;
    arguments << "-activate";
    app.sendArguments(arguments);
    QTimer::singleShot(50000, &app, SLOT(quit()));
    return app.exec();
  } else {
    app.startListen();
  }

  QString root = QCoreApplication::applicationDirPath();
  QString qrcPath = root;
  qrcPath.append("/qGNA.rcc");

  GGS::ResourceHelper::ResourceLoader loader;
  loader.load(qrcPath); 

  QThread::currentThread()->setObjectName("Main thread");

  TTCCLayout layout(TTCCLayout::ISO8601);
  layout.retain();

  QString logPath = app.applicationDirPath();
  logPath.append("/qgna.log");

  FileAppender appender(&layout, logPath, true);
  appender.retain();
  appender.activateOptions();

  LogManager::qtLogger()->addAppender(&appender);
  LogManager::setThreshold(Level::ALL_INT);
   
  //LogManager::setHandleQtMessages(true);

#ifndef _DEBUG
  LogManager::setHandleQtMessages(true);
#endif

  qDebug() << "started";
  qDebug() << "UAC: " << GGS::AutoRunHelper::UACHelper::isUacEnabled();
  qDebug() << "Admin Group: " << GGS::AutoRunHelper::UACHelper::isUserAdminByRole(); 
  qDebug() << "Admin: " << GGS::AutoRunHelper::UACHelper::isUserAdmin();

#ifndef QGNA_NO_ADMIN_REQUIRED
  if(!GGS::AutoRunHelper::UACHelper::isUserAdminByRole()) {
    if (!GGS::AutoRunHelper::UACHelper::restartToElevateRights()) {    
      qDebug() << "Restart failed. May be user didn't accept UAC.";
      
      LogManager::qtLogger()->removeAllAppenders(); 
      return -1;
    }

    LogManager::qtLogger()->removeAllAppenders(); 
    return 0;
  }
#endif

  if (!initDatabase()) {
    MessageBoxW(0, L"Could not create settings.", L"Error", MB_OK);
    return -1;
  }

  GGS::Core::System::Shell::UrlProtocolHelper::registerProtocol("gamenet");

  QString imageFormatsPath = root;
  root.append("/plugins");
  app.addLibraryPath(root);
   
  MainWindow w;
  if (!app.containsCommand("minimized"))
      w.activateWindow();

  SIGNAL_CONNECT_CHECK(QObject::connect(&app, SIGNAL(forceQuit()), &w, SLOT(onWindowClose()), Qt::DirectConnection)); 

  SIGNAL_CONNECT_CHECK(QObject::connect(&w, SIGNAL(secondInstanceExecuteRequest()), &app, SLOT(allowSecondInstance()), Qt::DirectConnection)); 

  SIGNAL_CONNECT_CHECK(QObject::connect(&app, SIGNAL(commandRecieved(QString, QStringList)), &w, SLOT(commandRecieved(QString, QStringList)), Qt::QueuedConnection)); 

  Features::RemouteStartGame remouteStartGame;
  SIGNAL_CONNECT_CHECK(QObject::connect(&app, SIGNAL(commandRecieved(QString, QStringList)), &remouteStartGame, SLOT(commandRecieved(QString, QStringList)), Qt::QueuedConnection));  
  SIGNAL_CONNECT_CHECK(QObject::connect(&remouteStartGame, SIGNAL(startGameRequest(QString)), &w, SLOT(downloadButtonStart(QString))));

  GGS::Settings::SettingsSaver saver; 
  GGS::Settings::Settings::setSettingsSaver(&saver); 
  
  Features::ThronInstaller installer;
  SIGNAL_CONNECT_CHECK(QObject::connect(&w, SIGNAL(updateFinished()), &app, SLOT(initializeFinished())));
  SIGNAL_CONNECT_CHECK(QObject::connect(&w, SIGNAL(updateFinished()), &installer, SLOT(downloadAndInstall())));

  GGS::Core::Marketing::send(GGS::Core::Marketing::AnyStartQGna);
  GGS::Core::Marketing::sendOnce(GGS::Core::Marketing::FirstRunGna);

  int result = app.exec();

  LogManager::qtLogger()->removeAllAppenders(); 

  return result;
}         
