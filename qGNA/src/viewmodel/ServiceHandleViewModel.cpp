#include <viewmodel/ServiceHandleViewModel.h>

#include <Host/DBus/DbusConnection.h>
#include <Host/DBus/ServiceHandleBridgeProxy.h>

using GameNet::Host::DBus::DBusConnection;

ServiceHandleViewModel::ServiceHandleViewModel(QObject *parent /*= 0*/)
  : QObject(parent)
{
  QString dbusService("com.gamenet.dbus");

  this->_serviceHandleProxy = new ServiceHandleBridgeProxy(
    dbusService, "/serviceHandle", DBusConnection::bus(), this);

  connect(this->_serviceHandleProxy, &ServiceHandleBridgeProxy::locked, 
    this, &ServiceHandleViewModel::serviceLocked);

  connect(this->_serviceHandleProxy, &ServiceHandleBridgeProxy::unlocked, 
    this, &ServiceHandleViewModel::serviceUnlocked);
}

ServiceHandleViewModel::~ServiceHandleViewModel()
{
}
