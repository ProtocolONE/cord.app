#include <viewmodel/ApplicationStatisticViewModel.h>

#include <Host/DBus/DbusConnection.h>
#include <Host/DBus/ApplicationStatisticBridgeProxy.h>

using P1::Host::DBus::DBusConnection;

ApplicationStatisticViewModel::ApplicationStatisticViewModel(QObject *parent /*= 0*/)
  : QObject(parent)
{
  QString dbusService("com.protocolone.launcher.dbus");
  
  this->_applicationStatisticProxy = new ApplicationStatisticBridgeProxy(
    dbusService, "/applicationStatistic", DBusConnection::bus(), this);
}

ApplicationStatisticViewModel::~ApplicationStatisticViewModel()
{
}

bool ApplicationStatisticViewModel::isGameInstalled(const QString& serviceId) const
{
  return this->_applicationStatisticProxy->isGameInstalled(serviceId);
}

quint64 ApplicationStatisticViewModel::gameInstallDate(const QString& serviceId) const
{
  return this->_applicationStatisticProxy->gameInstallDate(serviceId);
}

quint64 ApplicationStatisticViewModel::gameLastExecutionTime(const QString& serviceId) const
{
  return this->_applicationStatisticProxy->gameLastExecutionTime(serviceId);
}

int ApplicationStatisticViewModel::executeGameSuccessCount(const QString& serviceId) const
{
  return this->_applicationStatisticProxy->executeGameSuccessCount(serviceId);
}

int ApplicationStatisticViewModel::executeGameFailedCount(const QString& serviceId) const
{
  return this->_applicationStatisticProxy->executeGameFailedCount(serviceId);
}

int ApplicationStatisticViewModel::executeGameTotalCount(const QString& serviceId) const
{
  return this->_applicationStatisticProxy->executeGameTotalCount(serviceId);
}

QString ApplicationStatisticViewModel::installWithService() const
{
  return this->_applicationStatisticProxy->installWithService();
}

quint64 ApplicationStatisticViewModel::installDate() const
{
  return this->_applicationStatisticProxy->installDate();
}
