#include <Helper/DBusConnectionCheck.h>

#include <Host/Dbus/DbusConnection.h>

#include <QtCore/QDebug>

#include <QtDBus/QDBusConnectionInterface>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusServiceWatcher>

namespace GameNet {

  using Host::DBus::DBusConnection;

  DBusConnectionCheck::DBusConnectionCheck(const QString& dbusService)
    : _dbusService(dbusService)
    , _iface(nullptr)
  {
#ifdef USE_SESSION_DBUS
    QDBusConnection &connection = DBusConnection::bus();
    this->_iface = connection.interface();
     
    QDBusServiceWatcher* watcher = new QDBusServiceWatcher(this);
    watcher->setConnection(connection);
    watcher->addWatchedService(this->_dbusService);

    connect(watcher, &QDBusServiceWatcher::serviceUnregistered, this, &DBusConnectionCheck::serviceUnregistered);
#endif
  }

  DBusConnectionCheck::~DBusConnectionCheck()
  {

  }

  bool DBusConnectionCheck::checkConnection() 
  {
    if (!this->_iface) {
      return DBusConnection::bus().isConnected();
    }

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