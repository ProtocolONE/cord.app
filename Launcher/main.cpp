#include <Helper/Logger.hpp>

#include "mainwindow.h"
#include "Uninstall.h"
#include "viewmodel\SettingsViewModel.h"

#include <version.h>

#include <Features/RemouteStartGame.h>

#include <Features/TaskBarEventFilter.h>
#include <Features/LanguageChangeEventFilter.h>

#include <Features/Marketing/MarketingIntegrationMarker.h>

#include <Core/System/Shell/UrlProtocolHelper.h>
#include <Core/Marketing.h>

#include <Settings/Settings.h>
#include <Settings/InitializeHelper.h>

#include <AutoRunHelper/AutoRunHelper.h>

#include <Application/SingleApplication.h>

#include <QtCore/QDebug>
#include <QtCore/QThreadPool>
#include <QtCore/QProcess>
#include <QtCore/QTime>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include <QtWidgets/QApplication>
#include <QResource>

#include <Log4Qt/LogManager>
#include <Log4Qt/Level>
#include <Log4Qt/RollingFileAppender>
#include <Log4Qt/TTCCLayout>

#include <BugTrap/BugTrap.h>

#include <Helper/DBusConnectionCheck.h>
#include <Helper/FileUtils.h>

#include <QtWebEngine/QtWebEngine>

#include <QtYaml/ConfigManager.h>

#include <Helper/RegisterTypes.h>

using namespace Log4Qt;
using namespace P1;
using P1::Application::SingleApplication;

#define SIGNAL_CONNECT_CHECK(X) { bool result = X; Q_ASSERT_X(result, __FUNCTION__ , #X); }
#define CRITICAL_LOG qCritical() << __FILE__ << __LINE__ << __FUNCTION__

bool initDatabase()
{
  P1::Settings::InitializeHelper helper;
  QString settingsPath = QString("%1/settings/settings.sql")
    .arg(QStandardPaths::writableLocation(QStandardPaths::DataLocation));

  FileUtils::createDirectoryForFile(settingsPath);

  helper.setFileName(settingsPath);
  if (!helper.init())
    return false;

  if (helper.isRecreated())
  {
    SettingsViewModel settingsViewModel;
    settingsViewModel.setDefaultSettings();
  }

  P1::Settings::Settings::setCacheEnabled(true);
  return true;
}

void initBugTrap(const QString &path)
{
  BT_SetAppName(_T("ProtocolOne Launcher UI Application"));
  BT_SetAppVersion(_T(STRFILEVER));
  BT_SetSupportEMail(_T("support@protocol.one"));
  BT_SetSupportURL(_T("https://support.protocol.one"));
  BT_SetFlags(BTF_DETAILEDMODE | BTF_ATTACHREPORT | BTF_SCREENCAPTURE);
  BT_SetSupportServer(_T("fs1.protocol.one"), 9999); // UNDONE add support of config
  BT_AddLogFile(reinterpret_cast<const wchar_t*>(QString(path + "/launcher.ui.log").utf16()));
  BT_AddLogFile(reinterpret_cast<const wchar_t*>(QString(path + "/launcher.ui.log.1").utf16()));
  BT_AddLogFile(reinterpret_cast<const wchar_t*>(QString(path + "/launcher.host.log").utf16()));
  BT_AddLogFile(reinterpret_cast<const wchar_t*>(QString(path + "/launcher.host.log.1").utf16()));
  BT_InstallSehFilter();
}

void initOpenglRender(SingleApplication& app) 
{
  qputenv("QT_OPENGL_BUGLIST", "render.json");

  bool hasRender = app.containsCommand("render");
  if (hasRender) {
    QStringList args = app.getCommandArguments("render");
    if (args.length() > 0) {
      QString render = args[0];
      if (render == "angle" || render == "desktop" || render == "software") {
        qDebug() << "Override QT_OPENGL: " << render;
        qputenv("QT_OPENGL", render.toLatin1());
        if (render == "software") {
          qDebug() << "Set QT_QPA_UPDATE_IDLE_TIME 100";
          qputenv("QT_QPA_UPDATE_IDLE_TIME", "100");
        }
      }
      else {
        hasRender = false;
      }
    }
    else {
      hasRender = false;
    }
  }

  bool hasRenderLoop = app.containsCommand("renderloop");
  if (hasRenderLoop) {
    QStringList args = app.getCommandArguments("renderloop");
    if (args.length() > 0) {
      QString renderloop = args[0];
      if (renderloop == "threaded" || renderloop == "basic" || renderloop == "windows") {
        qDebug() << "Override QSG_RENDER_LOOP: " << renderloop;
        qputenv("QSG_RENDER_LOOP", renderloop.toLatin1());
      }
      else {
        hasRenderLoop = false;
      }
    }
    else {
      hasRenderLoop = false;
    }
  }

  if (!hasRender && !hasRenderLoop) {
    if (QSysInfo::WindowsVersion == QSysInfo::WV_XP) {
      qputenv("QT_OPENGL", "software");
      qputenv("QT_QPA_UPDATE_IDLE_TIME", "100");
      qputenv("QSG_RENDER_LOOP", "threaded");
    }
    else {
      // INFO задает интервал для перерисовки кадров.
      qputenv("QT_QPA_UPDATE_IDLE_TIME", "33");
    }
  }
}

int main(int argc, char *argv[])
{
  SingleApplication app(argc, argv, "{34688F78-432F-4C5A-BFC7-CD1BC88A30CC}");
  app.setQuitOnLastWindowClosed(false);

  // Get current flag
  // Turn off memory profiler - Qt working too long with this
  int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
  tmpFlag = _CrtSetDbgFlag(0);

  registerDependenicesTypes();

  QString configPath = QCoreApplication::applicationDirPath() + "/Config.yaml";
  
  P1::QtYaml::ConfigManager configManager;
  configManager.load(configPath);

  QCoreApplication::setOrganizationName(configManager.value<QString>("organizationName", "ProtocolOne"));
  QCoreApplication::setApplicationName(configManager.value<QString>("applicationName", "Launcher"));
  
  QString path = QCoreApplication::applicationDirPath();

  QStringList plugins;
  plugins << path + "/plugins5";
  app.setLibraryPaths(plugins);

  app.setIpcPortPath(QString("HKEY_CURRENT_USER\\Software\\%1\\%2").arg(QCoreApplication::organizationName(), QCoreApplication::applicationName()));
  app.setWindowIcon(QIcon(path + "/Assets/Images/launcher.ico"));

  QString logPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QString("/logs/");
  initBugTrap(logPath); 

  QThread::currentThread()->setObjectName("Main thread");

  if (app.isAlreadyRunning()) {
    QObject::connect(&app, SIGNAL(sendMessageFinished()), &app, SLOT(quit()), Qt::QueuedConnection);

    QStringList arguments;

    bool skipActivation = false;

    skipActivation |= app.containsCommand("goprotocolonemoney");
    skipActivation |= app.containsCommand("minimized");

    if (!skipActivation) {
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

  P1::Host::LoggerHelper logger(logPath + "launcher.ui.log");
  
  initOpenglRender(app);

  if (configManager.value<QString>("requireAdminRights", "false").toLower() == "true") {
#ifndef LAUNCHER_NO_ADMIN_REQUIRED
    if (!P1::AutoRunHelper::UACHelper::isUserAdminByRole()) {
      if (!P1::AutoRunHelper::UACHelper::restartToElevateRights()) {
        qDebug() << "Restart failed. May be user didn't accept UAC.";

        LogManager::qtLogger()->removeAllAppenders();
        return -1;
      }

      LogManager::qtLogger()->removeAllAppenders();
      return 0;
    }
#endif
  }
  if (!initDatabase()) {
    MessageBoxW(0, L"Could not create settings.", L"Error", MB_OK);
    LogManager::qtLogger()->removeAllAppenders(); 
    return -1;
  }

  if (app.containsCommand("uninstall") && app.getCommandArguments("uninstall").empty()) {
    Uninstall::run(app.arguments());
    LogManager::qtLogger()->removeAllAppenders();
    return 0;
  }

  qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()));

  DBusConnectionCheck dbusConnectionCheck("com.protocolone.launcher.dbus");
  if (!dbusConnectionCheck.checkConnection()) {
    QProcess::startDetached("launcher.host.exe", app.arguments(), path);
    qDebug() << "Can't connect to DBUS.";
    LogManager::qtLogger()->removeAllAppenders(); 
    return 0;
  }

  P1::Application::TaskBarEventFilter *taskBarFilter = new P1::Application::TaskBarEventFilter(&app);
  app.installNativeEventFilter(taskBarFilter);

  P1::Application::LanguageChangeEventFilter *languageChangeEventFilter = new P1::Application::LanguageChangeEventFilter(&app);
  app.installNativeEventFilter(languageChangeEventFilter);
  
  QResource::registerResource(path + "/Launcher.rcc");
  QResource::registerResource(path + "/smiles.rcc");

  QSettings settings(QString("HKEY_CURRENT_USER\\Software\\%1\\%2").arg(
    QCoreApplication::organizationName(), 
    QCoreApplication::applicationName()), 
    QSettings::NativeFormat);

  settings.setValue("Path",  QDir::toNativeSeparators(path));

  MainWindow w;

  QObject::connect(&dbusConnectionCheck, &DBusConnectionCheck::serviceDisconnected, &w, &MainWindow::quit);
  if (!dbusConnectionCheck.checkConnection()) {
    emit w.quit();
  }

  QtWebEngine::initialize();

  QTimer::singleShot(0, &w, SLOT(initialize()));

  QObject::connect(&app, &SingleApplication::forceQuit, 
    &w, &MainWindow::onWindowClose, Qt::DirectConnection);

  QObject::connect(&w, &MainWindow::secondInstanceExecuteRequest, 
    &app, &SingleApplication::allowSecondInstance, Qt::DirectConnection);

  QObject::connect(&app, &SingleApplication::commandRecieved,
    &w, &MainWindow::commandRecieved, Qt::QueuedConnection);

  Features::RemouteStartGame remouteStartGame;
  QObject::connect(&app, &SingleApplication::commandRecieved,
    &remouteStartGame, &Features::RemouteStartGame::commandRecieved, Qt::QueuedConnection);
  QObject::connect(&remouteStartGame, &Features::RemouteStartGame::startGameRequest,
    &w, &MainWindow::removeStartGame, Qt::QueuedConnection);

  P1::Settings::SettingsSaver saver; 
  P1::Settings::Settings::setSettingsSaver(&saver); 
  
  QObject::connect(&w, &MainWindow::updateFinished,
    &app, &SingleApplication::initializeFinished, Qt::QueuedConnection);

  QObject::connect(&w, &MainWindow::taskBarButtonMsgRegistered,
    taskBarFilter, &P1::Application::TaskBarEventFilter::onTaskBarButtonMsgRegistered, Qt::QueuedConnection);
  QObject::connect(taskBarFilter, &P1::Application::TaskBarEventFilter::taskBarButtonCreated,
    &w, &MainWindow::onTaskbarButtonCreated, Qt::QueuedConnection);
  

  QObject::connect(languageChangeEventFilter, &P1::Application::LanguageChangeEventFilter::languageChanged,
    &w, &MainWindow::onLanguageChanged, Qt::QueuedConnection);

  int result = app.exec();

  LogManager::qtLogger()->removeAllAppenders(); 

  return result;
}
