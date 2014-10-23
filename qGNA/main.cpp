﻿#include "mainwindow.h"
#include "Uninstall.h"
#include "viewmodel\SettingsViewModel.h"

#include <version.h>

#include <Features/RemouteStartGame.h>
#include <Features/RememberGameDownloading.h>
#include <Features/SilentMode.h>

#include <Features/Thetta/TlsInitializer.h>
#include <Features/Thetta/Protector.h>

#include <Features/TaskBarEventFilter.h>
#include <Features/LanguageChangeEventFilter.h>

#include <Core/System/Shell/UrlProtocolHelper.h>
#include <Core/Marketing.h>

#include <ResourceHelper/ResourceLoader.h>

#include <Settings/Settings>
#include <Settings/InitializeHelper>

#include <AutoRunHelper/AutoRunHelper.h>

#include <Application/SingleApplication.h>

#include <QtCore/QDebug>
#include <QtCore/QThreadPool>

#include <QtWidgets/QApplication>
#include <QResource>

#include <Log4Qt/LogManager>
#include <Log4Qt/Level>
#include <Log4Qt/RollingFileAppender>
#include <Log4Qt/TTCCLayout>

#include <BugTrap/BugTrap.h>

#include <Helper/DBusConnectionCheck.h>

using namespace Log4Qt; 
using namespace GameNet;

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

void initBugTrap(const QString &path)
{
  BT_SetAppName(_T("QGNA Application"));
  BT_SetAppVersion(_T(STRFILEVER));
  BT_SetSupportEMail(_T("support@gamenet.ru"));
  BT_SetSupportURL(_T("https://support.gamenet.ru"));
  BT_SetFlags(BTF_DETAILEDMODE | BTF_ATTACHREPORT | BTF_SCREENCAPTURE);
  BT_SetSupportServer(_T("fs1.gamenet.ru"), 9999);
  BT_AddLogFile(QString(path + "/qgna.log").utf16());
  BT_AddLogFile(QString(path + "/qgna.log.1").utf16());
  BT_InstallSehFilter();
}

int main(int argc, char *argv[]) 
{
  using GGS::Application::SingleApplication;
  SingleApplication app(argc, argv, "{34688F78-432F-4C5A-BFC7-CD1BC88A30CC}");
  QString path = QCoreApplication::applicationDirPath();
  
  QStringList plugins;
  plugins << path + "/plugins";
  app.setLibraryPaths(plugins);
  
  MemoryProtector_CheckFunction1(26500, 19169, 15724, 61393);

  if (app.containsCommand("silent")) {
    Features::SilentMode mode;
    mode.activate();
  }

  QString path = QCoreApplication::applicationDirPath();

  app.setIpcPortPath("HKEY_CURRENT_USER\\Software\\GGS\\QGNA");
  app.setWindowIcon(QIcon(path + "/Assets/Images/qgna.ico"));

  initBugTrap(path);
  QThread::currentThread()->setObjectName("Main thread");

  if (app.isAlreadyRunning()) {
    QObject::connect(&app, SIGNAL(sendMessageFinished()), &app, SLOT(quit()), Qt::QueuedConnection);
    
    QStringList arguments;
    
    if (!app.containsCommand("gogamenetmoney")) {
      arguments << "-activate"; 
    }

    app.sendArguments(arguments);
    QTimer::singleShot(50000, &app, SLOT(quit()));

    return app.exec();
  } else {
    app.startListen();
  }

  
  // HACK В приложении активно используются QtConcurrent и другие сущности использующие QThreadPool
  // Так как некоторые задачи критичны для запуска, а в случаи переполнение пула они будут положены в очередь,
  // увеличим пул по умолчанию, чтобы пока проблемы не наблюдалось. В будущем надо для критичных
  // задачь избегать QThreadPool и возможно переписать некоторые компоненты, например, GameExecutor, которые не должен
  // использовать по одному потоку на игру.
  QThreadPool::globalInstance()->setMaxThreadCount(50); 

  TTCCLayout layout(TTCCLayout::ISO8601);
  layout.retain();

  RollingFileAppender appender(&layout, path + "/qgna.log", true);
  appender.setMaximumFileSize(1000000);
  appender.setMaxBackupIndex(1);
  appender.retain();
  appender.activateOptions();

  LogManager::qtLogger()->addAppender(&appender);
  LogManager::setThreshold(Level::ALL_INT);
  
#ifndef _DEBUG
  LogManager::setHandleQtMessages(true);
#endif

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

  DBusConnectionCheck dbusConnectionCheck("com.gamenet.dbus");
  if (!dbusConnectionCheck.checkConnection()) {
    LogManager::qtLogger()->removeAllAppenders(); 
    return 0;
  }

  GGS::Application::TaskBarEventFilter *taskBarFilter = new GGS::Application::TaskBarEventFilter(&app);
  app.installNativeEventFilter(taskBarFilter);

  GGS::Application::LanguageChangeEventFilter *languageChangeEventFilter = new GGS::Application::LanguageChangeEventFilter(&app);
  app.installNativeEventFilter(languageChangeEventFilter);

  GGS::ResourceHelper::ResourceLoader loader;
  loader.load(path + "/qGNA.rcc"); 

  QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);
  settings.setValue("Path",  QDir::toNativeSeparators(path));

  if (!initDatabase()) {
    MessageBoxW(0, L"Could not create settings.", L"Error", MB_OK);
    LogManager::qtLogger()->removeAllAppenders(); 
    return -1;
  }

  if (app.containsCommand("uninstall")) {
    Uninstall::run(app.arguments());
    LogManager::qtLogger()->removeAllAppenders();
    return 0;
  }

  MainWindow w;
  QObject::connect(&dbusConnectionCheck, &DBusConnectionCheck::serviceDisconnected, &w, &MainWindow::quit);
  if (!dbusConnectionCheck.checkConnection()) {
    emit w.quit();
  }

  QTimer::singleShot(0, &w, SLOT(initialize()));

  QObject::connect(&app, &SingleApplication::forceQuit, 
    &w, &MainWindow::onWindowClose, Qt::DirectConnection);

  SIGNAL_CONNECT_CHECK(QObject::connect(&w, SIGNAL(secondInstanceExecuteRequest()), &app, SLOT(allowSecondInstance()), Qt::DirectConnection)); 

  SIGNAL_CONNECT_CHECK(QObject::connect(&app, SIGNAL(commandRecieved(QString, QStringList)), &w, SLOT(commandRecieved(QString, QStringList)), Qt::QueuedConnection)); 

  Features::RemouteStartGame remouteStartGame;
  SIGNAL_CONNECT_CHECK(QObject::connect(&app, SIGNAL(commandRecieved(QString, QStringList)), &remouteStartGame, SLOT(commandRecieved(QString, QStringList)), Qt::QueuedConnection));  
  SIGNAL_CONNECT_CHECK(QObject::connect(&remouteStartGame, SIGNAL(startGameRequest(QString)), &w, SLOT(removeStartGame(QString))));

  GGS::Settings::SettingsSaver saver; 
  GGS::Settings::Settings::setSettingsSaver(&saver); 

  SIGNAL_CONNECT_CHECK(QObject::connect(&w, SIGNAL(updateFinished()), &app, SLOT(initializeFinished())));
  SIGNAL_CONNECT_CHECK(QObject::connect(&w, SIGNAL(taskBarButtonMsgRegistered(unsigned int)), taskBarFilter, SLOT(onTaskBarButtonMsgRegistered(unsigned int))));
  SIGNAL_CONNECT_CHECK(QObject::connect(taskBarFilter, SIGNAL(taskBarButtonCreated()), &w, SLOT(onTaskbarButtonCreated())));
  SIGNAL_CONNECT_CHECK(QObject::connect(languageChangeEventFilter, SIGNAL(languageChanged()), &w, SLOT(onLanguageChanged())));

  int result = app.exec();

  LogManager::qtLogger()->removeAllAppenders(); 

  return result;
}


