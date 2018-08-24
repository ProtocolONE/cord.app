#include <viewmodel/settingsviewmodel.h>

#include <Host/Dbus/DownloaderSettingsBridgeProxy.h>
#include <Host/DBus/UpdateManagerBridgeProxy.h>
#include <Host/Dbus/ApplicationBridgeProxy.h>

#include <Settings/Settings.h>
#include <Core/UI/Message.h>

#include <AutoRunHelper.h>
#include <QtConcurrent/QtConcurrentRun>

#include <QtCore/QDateTime>

#include <Helper/ApplicationArea.hpp>

SettingsViewModel::SettingsViewModel(QObject *parent)
  : QObject(parent)
  , _downloaderSettings(nullptr)
  , _updaterProxy(nullptr)
  , _applicationProxy(nullptr)
{
  this->_instantlySave = true;
}

SettingsViewModel::~SettingsViewModel()
{
}

void SettingsViewModel::setAutoStart(int value)
{
  this->_applicationProxy->setAutoStartMode(value);
}

int SettingsViewModel::autoStart()
{
  return this->_applicationProxy->autoStartMode();
}

bool SettingsViewModel::notifyWhenStartedGameNet()
{
  P1::Settings::Settings settings;
  return settings.value("qGNA/notifyWhenStartedGameNet").toBool();
}

void SettingsViewModel::setNotifyWhenStartedGameNet(bool _notify)
{
  bool tmp = this->notifyWhenStartedGameNet();
  if (tmp == _notify)
    return;

  P1::Settings::Settings settings;
  settings.setValue("qGNA/notifyWhenStartedGameNet", _notify, _instantlySave); 
  emit this->notifyWhenStartedGameNetChanged();
}

bool SettingsViewModel::notifyStartEndGameDownload()
{
  P1::Settings::Settings settings;
  return settings.value("qGNA/notifyStartEndGameDownload").toBool();
}

void SettingsViewModel::setNotifyStartEndGameDownload(bool _notify)
{
  bool tmp = this->notifyStartEndGameDownload();
  if (tmp == _notify)
    return;

  P1::Settings::Settings settings;
  settings.setValue("qGNA/notifyStartEndGameDownload", _notify, _instantlySave); 
  emit this->notifyStartEndGameDownloadChanged();
}

bool SettingsViewModel::notifyDisconnectFriendFromNetwork()
{
  P1::Settings::Settings settings;
  return settings.value("qGNA/notifyDisconnectFriendFromNetwork").toBool();
}

void SettingsViewModel::setNotifyDisconnectFriendFromNetwork(bool _notify)
{
  bool tmp = this->notifyDisconnectFriendFromNetwork();
  if (tmp == _notify)
    return;

  P1::Settings::Settings settings;
  settings.setValue("qGNA/notifyDisconnectFriendFromNetwork", _notify, _instantlySave); 
  emit this->notifyDisconnectFriendFromNetworkChanged();
}

bool SettingsViewModel::notifyDisconnectFriendFromGame()
{
  P1::Settings::Settings settings;
  return settings.value("qGNA/notifyDisconnectFriendFromGame").toBool();
}

void SettingsViewModel::setNotifyDisconnectFriendFromGame(bool _notify)
{
  bool tmp = this->notifyDisconnectFriendFromGame();
  if (tmp == _notify)
    return;

  P1::Settings::Settings settings;
  settings.setValue("qGNA/notifyDisconnectFriendFromGame", _notify, _instantlySave); 
  emit this->notifyDisconnectFriendFromGameChanged();
}

bool SettingsViewModel::notifyNewMessage()
{
  P1::Settings::Settings settings;
  return settings.value("qGNA/notifyNewMessage").toBool();
}

void SettingsViewModel::setNotifyNewMessage(bool _notify)
{
  bool tmp = this->notifyNewMessage();
  if (tmp == _notify)
    return;

  P1::Settings::Settings settings;
  settings.setValue("qGNA/notifyNewMessage", _notify, _instantlySave); 
  emit this->notifyNewMessageChanged();
}

void SettingsViewModel::setDefaultSettings()
{
  _instantlySave = false;

  setNotifyWhenStartedGameNet(false);
  setNotifyStartEndGameDownload(true);
  setNotifyDisconnectFriendFromNetwork(true);
  setNotifyDisconnectFriendFromGame(true);
  setNotifyNewMessage(true);

  P1::Settings::Settings settings;
  settings.setValue("qGNA/language", "ru" , true);

  _instantlySave = true;
}

void SettingsViewModel::loadSettings()
{

}

bool SettingsViewModel::seedEnabled()
{
  return this->_downloaderSettings->seedEnabled();
}

void SettingsViewModel::setSeedEnabled(bool value)
{
  this->_downloaderSettings->setSeedEnabled(value);
}

QString SettingsViewModel::incomingPort()
{
  return QString::number(this->_downloaderSettings->listeningPort());
}

void SettingsViewModel::setIncomingPort(const QString& _port)
{
  bool ok;
  int port = _port.toInt(&ok);
  if (!ok) {
    emit this->incomingPortChanged();
    return;
  }

  this->_downloaderSettings->setListeningPort(port);
}

QString SettingsViewModel::numConnections()
{
  return QString::number(this->_downloaderSettings->maxConnection());
}

void SettingsViewModel::setNumConnections(const QString& _num)
{
  bool ok;
  int num = _num.toInt(&ok);
  if (!ok) {
    emit this->numConnectionsChanged();
    return;
  }

  this->_downloaderSettings->setMaxConnection(num);
}

QString SettingsViewModel::downloadSpeed()
{
  return QString::number(this->_downloaderSettings->downloadRate());
}

void SettingsViewModel::setDownloadSpeed(const QString& _speed)
{
  bool ok;
  int rate = _speed.toInt(&ok);
  if (!ok) {
    emit this->downloadSpeedChanged();
    return;
  }

  this->_downloaderSettings->setDownloadRate(rate);
}

QString SettingsViewModel::uploadSpeed()
{
  return QString::number(this->_downloaderSettings->uploadRate());
}

void SettingsViewModel::setUploadSpeed(const QString& _speed)
{
  bool ok;
  int rate = _speed.toInt(&ok);
  if (!ok) {
    emit this->uploadSpeedChanged();
    return;
  }

  this->_downloaderSettings->setUploadRate(rate);
}


void SettingsViewModel::setTorrentProfile(int value)
{
  this->_downloaderSettings->setTorrentProfile(value);
}

int SettingsViewModel::torrentProfile()
{
  return this->_downloaderSettings->torrentProfile();
}

void SettingsViewModel::setDownloaderSettings(DownloaderSettingsBridgeProxy *value)
{
  Q_ASSERT(value);
  this->_downloaderSettings = value;

  QObject::connect(value, &DownloaderSettingsBridgeProxy::listeningPortChanged,
    this, &SettingsViewModel::incomingPortChanged);

  QObject::connect(value, &DownloaderSettingsBridgeProxy::maxConnectionChanged,
    this, &SettingsViewModel::numConnectionsChanged);

  QObject::connect(value, &DownloaderSettingsBridgeProxy::downloadRateChanged,
    this, &SettingsViewModel::downloadSpeedChanged);

  QObject::connect(value, &DownloaderSettingsBridgeProxy::uploadRateChanged,
    this, &SettingsViewModel::uploadSpeedChanged);

  QObject::connect(value, &DownloaderSettingsBridgeProxy::seedEnabledChanged,
    this, &SettingsViewModel::seedEnabledChanged);

  QObject::connect(value, &DownloaderSettingsBridgeProxy::torrentProfileChanged,
    this, &SettingsViewModel::torrentProfileChanged);
}

QString SettingsViewModel::updateArea()
{
  P1::ApplicationArea area;
  area.load();
  return QString(area);
}

void SettingsViewModel::setApplicationProxy(ApplicationBridgeProxy *value)
{
  Q_ASSERT(value);
  this->_applicationProxy = value;

  QObject::connect(value, &ApplicationBridgeProxy::autoStartModeChanged,
    this, &SettingsViewModel::autoStartChanged);
}
