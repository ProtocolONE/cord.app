#include <viewmodel/UpdateViewModel.h>

#include <Host/DBus/DbusConnection.h>
#include <Host/DBus/UpdateManagerBridgeProxy.h>

using P1::Host::DBus::DBusConnection;

UpdateViewModel::UpdateViewModel(QObject *parent /* = 0 */)
  : QObject(parent)
{
  QString dbusService("com.protocolone.launcher.dbus");
    
  this->_updaterProxy = new UpdateManagerBridgeProxy(dbusService, "/updater", DBusConnection::bus(), this);

  connect(this->_updaterProxy, &UpdateManagerBridgeProxy::downloadUpdateProgress, 
          this, &UpdateViewModel::downloadUpdateProgress);

  connect(this->_updaterProxy, &UpdateManagerBridgeProxy::updatesFound, 
    this, &UpdateViewModel::updatesFound);

  connect(this->_updaterProxy, &UpdateManagerBridgeProxy::noUpdatesFound, 
    this, &UpdateViewModel::noUpdatesFound);

  connect(this->_updaterProxy, &UpdateManagerBridgeProxy::downloadRetryNumber, 
    this, &UpdateViewModel::downloadRetryNumber);

  connect(this->_updaterProxy, &UpdateManagerBridgeProxy::allCompleted, 
    this, &UpdateViewModel::allCompleted);

  connect(this->_updaterProxy, &UpdateManagerBridgeProxy::updateStateChanged, 
    this, &UpdateViewModel::updateStateChanged);

  connect(this->_updaterProxy, &UpdateManagerBridgeProxy::updateError, 
    this, &UpdateViewModel::updateError);
}

UpdateViewModel::~UpdateViewModel() 
{

}

int UpdateViewModel::updateState()
{
  return this->_updaterProxy->updateState();
}
