#include <Host/Application.h>

#include <Application/SingleApplication.h>
#include <Application/ArgumentParser.h>

#include <Features/ThronInstaller.h>

#include <Features/Thetta/TlsInitializer.h>
#include <Features/Thetta/Protector.h>

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
using Features::ThronInstaller;
using GGS::Application::SingleApplication;

Application *createApplication(SingleApplication *app) 
{
  Application *application = new Application(0);
  application->setSingleApplication(app);

  QObject::connect(application, &Application::initCompleted, [application]() {
    ThronInstaller *installer = new ThronInstaller(application);
    installer->downloadAndInstall();
  });
    
  return application;
}

int main(int argc, char *argv[])
{
  SingleApplication app(argc, argv, "{CCC143CA-F620-41B2-A3DD-CB5DFAEE5DD7}");
  QCoreApplication::setOrganizationName("Vebanaul");
  QCoreApplication::setApplicationName("GameNet");
  migrateUserInfo();

  QString path = QCoreApplication::applicationDirPath();
  initBugTrap(path);

  if (app.containsCommand("uninstall") && app.getCommandArguments("uninstall").empty()) {
    Uninstall::run(app.arguments());
    return 0;
  }

  app.setLibraryPaths(QStringList() << path + "/plugins");
  app.setIpcPortPath("HKEY_CURRENT_USER\\Software\\GGS\\QGNA\\Host");
  app.setWindowIcon(QIcon(path + "/Assets/Images/qgna.ico"));

  MemoryProtector_CheckFunction1(26500, 19169, 15724, 61393);

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

  LoggerHelper logger(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/logs/host.log");
  if (!requireAdminRights())
    return -1;

  qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()));

  if (!initDatabase()) {
    MessageBoxW(0, L"Could not create settings.", L"Error", MB_OK);
    return -1;
  }

  GGS::Settings::SettingsSaver saver; 
  GGS::Settings::Settings::setSettingsSaver(&saver); 

  GGS::Core::System::Shell::UrlProtocolHelper::registerProtocol("gamenet");

  Application *application = createApplication(&app);

  QTimer::singleShot(1, application, SLOT(init()));

  int result = app.exec();

  application->finalize();
  
  // HACK на удалении падает.... в неожиданных местах. Нужна помощь.
  //delete application;
  return result;
}


