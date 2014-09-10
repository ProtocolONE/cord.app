#include <Host/Application.h>

#include <Application/SingleApplication.h>

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
using GGS::Application::SingleApplication;

int main(int argc, char *argv[])
{
    SingleApplication app(argc, argv, "{CCC143CA-F620-41B2-A3DD-CB5DFAEE5DD7}");
    QStringList plugins;
    QString path = QCoreApplication::applicationDirPath();

    plugins << path + "/plugins";
    app.setLibraryPaths(plugins);

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

    Application application;
    application.setSingleApplication(&app);
    application.init();

    int result = app.exec();

    application.finalize();

    return result;
}
