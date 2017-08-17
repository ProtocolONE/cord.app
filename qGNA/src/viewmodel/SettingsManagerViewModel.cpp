#include <viewmodel/SettingsManagerViewModel.h>

#include <Host/DBus/DbusConnection.h>
#include <Host/DBus/SettingsBridgeProxy.h>

using GameNet::Host::DBus::DBusConnection;

SettingsManagerViewModel::SettingsManagerViewModel(QObject *parent /*= 0*/) : QObject(parent)
{
  QString dbusService("com.gamenet.dbus");
  this->_settingsProxy = new SettingsBridgeProxy(dbusService, "/remotesettings", DBusConnection::bus(), this);
}

SettingsManagerViewModel::~SettingsManagerViewModel()
{

}

void SettingsManagerViewModel::setValue(const QString& path, const QString& key, const QString& data)
{
  this->_settingsProxy->setValue(path, key, data);
}

QString SettingsManagerViewModel::value(const QString& path, const QString& key, const QString& defValue)
{
  return this->_settingsProxy->value(path, key, defValue);
}
