#include <Helper/DBusConnectionCheck.h>

#include <QtDBus/QDBusConnectionInterface>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusServiceWatcher>

namespace GameNet {

  DBusConnectionCheck::DBusConnectionCheck(const QString& dbusService)
    : _iface(QDBusConnection::sessionBus().interface())
    , _dbusService(dbusService)
  {
    QDBusServiceWatcher* watcher = new QDBusServiceWatcher(this);
    watcher->setConnection(QDBusConnection::sessionBus());
    watcher->addWatchedService(this->_dbusService);

    connect(watcher, &QDBusServiceWatcher::serviceUnregistered, this, &DBusConnectionCheck::serviceUnregistered);
  }

  DBusConnectionCheck::~DBusConnectionCheck()
  {

  }

  bool DBusConnectionCheck::checkConnection() 
  {
    QStringList registedServices = this->_iface->registeredServiceNames();

    return registedServices.contains(this->_dbusService);
  }

  void DBusConnectionCheck::serviceUnregistered(QString service) 
  {
    if (service != this->_dbusService) {
      return;
    }

    emit this->serviceDisconnected();
  }

}