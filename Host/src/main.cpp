#include <Host/Application.h>

#include <QtCore/QCoreApplication>
#include <QDBusConnection>

using GameNet::Host::Application;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Application application;
    application.init();

    return a.exec();
}
