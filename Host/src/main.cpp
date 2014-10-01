#include <Host/Application.h>

#include <Application/SingleApplication.h>

#include <Features/ThronInstaller.h>

#include <Features/Thetta/TlsInitializer.h>
#include <Features/Thetta/Protector.h>

#include <Helper/BugTrap.hpp>
#include <Helper/Database.hpp>
#include <Helper/Logger.hpp>
#include <Helper/ElevateRights.hpp>

#include <Core/System/Shell/UrlProtocolHelper.h>

#include <QtCore/QThread>
#include <QtCore/QThreadPool>
#include <QtCore/QCoreApplication>

#include <QtGui/QIcon>

using namespace GameNet::Host;
using Features::ThronInstaller;
using GGS::Application::SingleApplication;

Application *createApplication(SingleApplication *app) 
{
  Application *application = new Application(app);
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
    QString path = QCoreApplication::applicationDirPath();

    app.setLibraryPaths(QStringList() << path + "/plugins");
    app.setIpcPortPath("HKEY_CURRENT_USER\\Software\\GGS\\QGNA\\Host");
    app.setWindowIcon(QIcon(path + "/Assets/Images/qgna.ico"));

    MemoryProtector_CheckFunction1(26500, 19169, 15724, 61393);

    QThread::currentThread()->setObjectName("Main host thread");
    QThreadPool::globalInstance()->setMaxThreadCount(50);
    
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
    
    LoggerHelper logger(path + "/host.log");
    if (!requireAdminRights())
      return -1;

    initBugTrap(path);

    if (!initDatabase()) {
      MessageBoxW(0, L"Could not create settings.", L"Error", MB_OK);
      return -1;
    }
    
    GGS::Settings::SettingsSaver saver; 
    GGS::Settings::Settings::setSettingsSaver(&saver); 

    GGS::Core::System::Shell::UrlProtocolHelper::registerProtocol("gamenet");

    Application *application = createApplication(&app);
    application->init();

    int result = app.exec();
    application->finalize();

    return result;
}

