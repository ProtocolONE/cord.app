#include <Host/Application.h>

#include <QtCore/QThread>
#include <QtCore/QThreadPool>
#include <QtCore/QCoreApplication>

#include <QtDBus/QDBusConnection>

#include <QtGui/QIcon>

#include <Application/SingleApplication.h>

#include <Helper/BugTrap.hpp>
#include <Helper/Database.hpp>
#include <Helper/Logger.hpp>

using namespace GameNet::Host;
using GGS::Application::SingleApplication;

int main(int argc, char *argv[])
{
    SingleApplication app(argc, argv, "{CCC143CA-F620-41B2-A3DD-CB5DFAEE5DD7}");
    QStringList plugins;
    QString path = QCoreApplication::applicationDirPath();

    plugins << path + "/plugins";
    app.setLibraryPaths(plugins);

    QThread::currentThread()->setObjectName("Main host thread");
    QThreadPool::globalInstance()->setMaxThreadCount(50);
    
    LoggerHelper logger(path + "/host.log");

    initBugTrap(path);
    if (!initDatabase()) {
      MessageBoxW(0, L"Could not create settings.", L"Error", MB_OK);
      return -1;
    }
    
    Application application;
    application.init();

    return app.exec();
}
