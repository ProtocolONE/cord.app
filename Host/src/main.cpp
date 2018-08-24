#include <Host/Application.h>

#include <Application/SingleApplication.h>
#include <Application/ArgumentParser.h>

#include <Features/Marketing/MarketingIntegrationMarker.h>

#include <Helper/BugTrap.hpp>
#include <Helper/Database.hpp>
#include <Helper/Logger.hpp>
#include <Helper/ElevateRights.hpp>
#include <Helper/UserInfoMigration.hpp>

#include <Core/System/Shell/UrlProtocolHelper.h>

#include <Uninstall.h>

#include <QtCore/QThread>
#include <QtCore/QThreadPool>
#include <QtCore/QCoreApplication>
#include <QtCore/QTime>
#include <QtCore/QStandardPaths>
#include <QtCore/QFile>

#include <QtGui/QIcon>

using namespace GameNet::Host;
using P1::Application::SingleApplication;

Application *createApplication(SingleApplication *app) 
{
  Application *application = new Application(0);
  application->setSingleApplication(app);

  return application;
}

int main(int argc, char *argv[])
{
  SingleApplication app(argc, argv, "{CCC143CA-F620-41B2-A3DD-CB5DFAEE5DD7}");
  QCoreApplication::setOrganizationName("Vebanaul");
  QCoreApplication::setApplicationName("GameNet");
  migrateUserInfo();

  QString logPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QString("/logs/");
  initBugTrap(logPath);

  if (app.containsCommand("uninstall") && app.getCommandArguments("uninstall").empty()) {
    Uninstall::run(app.arguments());
    return 0;
  }

  QString path = QCoreApplication::applicationDirPath();
  app.setLibraryPaths(QStringList() << path + "/plugins5");
  app.setIpcPortPath("HKEY_CURRENT_USER\\Software\\GGS\\QGNA\\Host");
  app.setWindowIcon(QIcon(path + "/Assets/Images/qgna.ico"));

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

  LoggerHelper logger(logPath + "host.log");
  if (!requireAdminRights())
    return -1;

  Features::Marketing::MarketingIntegrationMarker marketingIntegrationMarker;
  marketingIntegrationMarker.init();

  qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()));

  if (!initDatabase()) {
    MessageBoxW(0, L"Could not create settings.", L"Error", MB_OK);
    return -1;
  }

  P1::Settings::SettingsSaver saver; 
  P1::Settings::Settings::setSettingsSaver(&saver); 

  P1::Core::System::Shell::UrlProtocolHelper::registerProtocol("gamenet");

  Application *application = createApplication(&app);

  QTimer::singleShot(1, application, SLOT(init()));

  int result = app.exec();

  application->finalize();
  
  // HACK на удалении падает.... в неожиданных местах. Нужна помощь.
  //delete application;
  return result;
}


