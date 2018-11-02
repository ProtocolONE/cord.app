#include <Helper/Logger.hpp>

#include <Host/Application.h>

#include <Application/SingleApplication.h>
#include <Application/ArgumentParser.h>

#include <Features/Marketing/MarketingIntegrationMarker.h>

#include <Helper/BugTrap.hpp>
#include <Helper/Database.hpp>
#include <Helper/ElevateRights.hpp>
#include <Helper/UserInfoMigration.hpp>
#include <Helper/RegisterTypes.h>

#include <Core/System/Shell/UrlProtocolHelper.h>

#include <Uninstall.h>

#include <QtCore/QThread>
#include <QtCore/QThreadPool>
#include <QtCore/QCoreApplication>
#include <QtCore/QTime>
#include <QtCore/QStandardPaths>
#include <QtCore/QFile>
#include <QtCore/QDir>

#include <QtGui/QIcon>

#include <QtYaml/ConfigManager.h>

using namespace P1::Host;
using P1::Application::SingleApplication;

Application *createApplication(SingleApplication *app) 
{
  Application *application = new Application(0);
  application->setSingleApplication(app);

  return application;
}

int main(int argc, char *argv[])
{
  // Get current flag
// Turn off memory profiler - Qt working too long with this
  int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
  tmpFlag = _CrtSetDbgFlag(0);

  SingleApplication app(argc, argv, "{0DF08C1B-5888-4623-A414-398C01CF831B}");
  QString path = QCoreApplication::applicationDirPath();

  QString configPath = path + "/Config.yaml";
  P1::QtYaml::ConfigManager configManager;
  configManager.load(configPath);

  QCoreApplication::setOrganizationName(configManager.value<QString>("organizationName", "ProtocolOne"));
  QCoreApplication::setApplicationName(configManager.value<QString>("applicationName", "Launcher"));

  QSettings settings(
    QSettings::NativeFormat, 
    QSettings::UserScope, 
    QCoreApplication::organizationName(), 
    QCoreApplication::applicationName());
  settings.setValue("Path", QDir::toNativeSeparators(path));

  registerDependenicesTypes();

  migrateUserInfo();

  QString logPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QString("/logs/");
  initBugTrap(logPath);

  if (app.containsCommand("uninstall") && app.getCommandArguments("uninstall").empty()) {
    Uninstall::run(app.arguments());
    return 0;
  }
  
  app.setLibraryPaths(QStringList() << path + "/plugins5");
  app.setIpcPortPath(QString("HKEY_CURRENT_USER\\Software\\%1\\%2\\Host").arg(QCoreApplication::organizationName(), QCoreApplication::applicationName()));
  app.setWindowIcon(QIcon(path + "/Assets/Images/launcher.ico"));

  QThread::currentThread()->setObjectName("Main host thread");

  // HACK В приложении активно используются QtConcurrent и другие сущности использующие QThreadPool
  // Так как некоторые задачи критичны для запуска, а в случаи переполнение пула они будут положены в очередь,
  // увеличим пул по умолчанию, чтобы пока проблемы не наблюдалось. В будущем надо для критичных
  // задачь избегать QThreadPool и возможно переписать некоторые компоненты, например, GameExecutor, которые не должен
  // использовать по одному потоку на игру.
  QThreadPool::globalInstance()->setMaxThreadCount(50);

  if (app.isAlreadyRunning()) {
    QObject::connect(&app, SIGNAL(sendMessageFinished()), &app, SLOT(quit()), Qt::QueuedConnection);
    QStringList arguments;

    if (QCoreApplication::arguments().count() <= 1) {
      arguments << "-activate";
    }

    app.sendArguments(arguments);
    QTimer::singleShot(50000, &app, SLOT(quit()));
    return app.exec();
  } else {
    app.startListen();
  }

  LoggerHelper logger(logPath + "launcher.host.log");

  if (configManager.value<QString>("requireAdminRights", "false").toLower() == "true") {
    if (!requireAdminRights())
      return -1;
  }

  Features::Marketing::MarketingIntegrationMarker marketingIntegrationMarker;
  marketingIntegrationMarker.init();

  qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()));

  if (!initDatabase()) {
    MessageBoxW(0, L"Could not create settings.", L"Error", MB_OK);
    return -1;
  }

  P1::Settings::SettingsSaver saver; 
  P1::Settings::Settings::setSettingsSaver(&saver); 

  P1::Core::System::Shell::UrlProtocolHelper::registerProtocol(configManager.value<QString>("urlProtocolScheme","protocolone"));

  Application *application = createApplication(&app);

  QTimer::singleShot(1, application, SLOT(init()));

  int result = app.exec();

  application->finalize();
  
  // HACK на удалении падает.... в неожиданных местах. Нужна помощь.
  //delete application;
  return result;
}


