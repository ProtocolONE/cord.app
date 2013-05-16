#include "viewmodel\settingsviewmodel.h"
#include <Settings\Settings.h>
#include <Core/UI/Message>

#include <AutoRunHelper.h>
#include <QtConcurrentRun>

#include <QtCore/QDateTime>

SettingsViewModel::SettingsViewModel(QObject *parent)
  : QObject(parent)
{
  _instantlySave = true;
}

SettingsViewModel::~SettingsViewModel()
{

}

void SettingsViewModel::addToAutoStart(bool autostart, bool isMinimized)   
{
  GGS::AutoRunHelper::AutoRunHelper autorunHelper;
  autorunHelper.setTaskName("GameNet");

  if (!autostart) {
    qDebug() << "remove from autorun result: " << autorunHelper.removeFromAutoRun();
  } else {
    autorunHelper.setPathToExe(QCoreApplication::applicationFilePath());
    
    autorunHelper.setCommandLineArguments(isMinimized ? QString("/minimized") : QString(""));
    autorunHelper.setTaskDescription(tr("AUTORUN_TASK_DESCRIPTION"));
    qDebug() << "add to autorun result: " << autorunHelper.addToAutoRun();  
  }
}

void SettingsViewModel::setAutoStart(int _autostart)
{
  //https://corp.syncopate.local/!%D0%9F%D1%80%D0%BE%D0%B5%D0%BA%D1%82%D1%8B/QGNA/%D0%9A%D0%BB%D1%8E%D1%87%D0%B8_%D1%80%D0%B5%D0%B5%D1%81%D1%82%D1%80%D0%B0
  if (!(_autostart == 0 || _autostart == 1 || _autostart == 2))   
    return;

  int tmp = this->autoStart();
  if (tmp == _autostart)
    return;

  QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);
  settings.setValue("AutoRun", _autostart);
  this->addToAutoStart(_autostart == 1 || _autostart == 2, _autostart == 2);
  emit this->autoStartChanged();
}

int SettingsViewModel::autoStart()
{
  QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);         
  bool ok;
  int result = settings.value("AutoRun", 0).toInt(&ok);
  return ok ? result : 0;
}

QString SettingsViewModel::incomingPort()
{
  GGS::Settings::Settings settings;
  QString inport = settings.value("qGNA/inPort").toString();
  return inport;
}

void SettingsViewModel::setIncomingPort(const QString& _port)
{
  bool ok;
  int port = _port.toInt(&ok);
  if (!ok) {
    emit this->incomingPortChanged();
    return ;
  }

  QString tmp = this->incomingPort();
  if (tmp == _port)
    return;

  GGS::Settings::Settings settings;
  settings.setValue("qGNA/inPort", _port, _instantlySave); 
  emit this->incomingPortChanged();
}

QString SettingsViewModel::numConnections()
{
  GGS::Settings::Settings settings;
  QString num = settings.value("qGNA/numConnections").toString();
  return num;
}

void SettingsViewModel::setNumConnections( const QString& _num )
{
  QString tmp = this->numConnections();
  if (tmp == _num)
    return;

  GGS::Settings::Settings settings;
  settings.setValue("qGNA/numConnections", _num, _instantlySave); 
  emit this->numConnectionsChanged();
}

bool SettingsViewModel::notifyWhenStartedGameNet()
{
  GGS::Settings::Settings settings;
  return settings.value("qGNA/notifyWhenStartedGameNet").toBool();
}

void SettingsViewModel::setNotifyWhenStartedGameNet(bool _notify)
{
  bool tmp = this->notifyWhenStartedGameNet();
  if (tmp == _notify)
    return;

  GGS::Settings::Settings settings;
  settings.setValue("qGNA/notifyWhenStartedGameNet", _notify, _instantlySave); 
  emit this->notifyWhenStartedGameNetChanged();
}

bool SettingsViewModel::notifyStartEndGameDownload()
{
  GGS::Settings::Settings settings;
  return settings.value("qGNA/notifyStartEndGameDownload").toBool();
}

void SettingsViewModel::setNotifyStartEndGameDownload(bool _notify)
{
  bool tmp = this->notifyStartEndGameDownload();
  if (tmp == _notify)
    return;

  GGS::Settings::Settings settings;
  settings.setValue("qGNA/notifyStartEndGameDownload", _notify, _instantlySave); 
  emit this->notifyStartEndGameDownloadChanged();
}

bool SettingsViewModel::notifyDisconnectFriendFromNetwork()
{
  GGS::Settings::Settings settings;
  return settings.value("qGNA/notifyDisconnectFriendFromNetwork").toBool();
}

void SettingsViewModel::setNotifyDisconnectFriendFromNetwork(bool _notify)
{
  bool tmp = this->notifyDisconnectFriendFromNetwork();
  if (tmp == _notify)
    return;

  GGS::Settings::Settings settings;
  settings.setValue("qGNA/notifyDisconnectFriendFromNetwork", _notify, _instantlySave); 
  emit this->notifyDisconnectFriendFromNetworkChanged();
}

bool SettingsViewModel::notifyDisconnectFriendFromGame()
{
  GGS::Settings::Settings settings;
  return settings.value("qGNA/notifyDisconnectFriendFromGame").toBool();
}

void SettingsViewModel::setNotifyDisconnectFriendFromGame(bool _notify)
{
  bool tmp = this->notifyDisconnectFriendFromGame();
  if (tmp == _notify)
    return;

  GGS::Settings::Settings settings;
  settings.setValue("qGNA/notifyDisconnectFriendFromGame", _notify, _instantlySave); 
  emit this->notifyDisconnectFriendFromGameChanged();
}

bool SettingsViewModel::notifyNewMessage()
{
  GGS::Settings::Settings settings;
  return settings.value("qGNA/notifyNewMessage").toBool();
}

void SettingsViewModel::setNotifyNewMessage(bool _notify)
{
  bool tmp = this->notifyNewMessage();
  if (tmp == _notify)
    return;

  GGS::Settings::Settings settings;
  settings.setValue("qGNA/notifyNewMessage", _notify, _instantlySave); 
  emit this->notifyNewMessageChanged();
}

QString SettingsViewModel::downloadSpeed()
{
  GGS::Settings::Settings settings;
  return settings.value("qGNA/downloadSpeed").toString();
}

void SettingsViewModel::setDownloadSpeed(const QString& _speed)
{
  QString tmp = this->downloadSpeed();
  if (tmp == _speed)
    return;

  GGS::Settings::Settings settings;
  settings.setValue("qGNA/downloadSpeed", _speed, _instantlySave); 
  emit this->downloadSpeedChanged();
}

QString SettingsViewModel::uploadSpeed()
{
  GGS::Settings::Settings settings;
  return settings.value("qGNA/uploadSpeed").toString();
}

void SettingsViewModel::setUploadSpeed(const QString& _speed)
{
  QString tmp = this->uploadSpeed();
  if (tmp == _speed)
    return;

  GGS::Settings::Settings settings;
  settings.setValue("qGNA/uploadSpeed", _speed, _instantlySave); 
  emit this->uploadSpeedChanged();
}

void SettingsViewModel::setDefaultSettings()
{
  _instantlySave = false;

  setIncomingPort("11888");
  setNumConnections("10");
  setNotifyWhenStartedGameNet(false);
  setNotifyStartEndGameDownload(true);
  setNotifyDisconnectFriendFromNetwork(true);
  setNotifyDisconnectFriendFromGame(true);
  setNotifyNewMessage(true);
  setDownloadSpeed("0");
  setUploadSpeed("0"); 

  GGS::Settings::Settings settings;
  settings.setValue("qGNA/language", "ru" , true);

  _instantlySave = true;
}

void SettingsViewModel::loadSettings()
{

}

bool SettingsViewModel::isPublicTestVersion()
{
  QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\GGS\\QGNA", QSettings::NativeFormat);
  bool ok = false;
  int area = settings.value("Repository", 0).toInt(&ok);
  if (!ok)
    area = 0;

  return area == 1;
}

void SettingsViewModel::switchClientVersion()
{
  using namespace GGS::Core::UI;

  if (Message::question(
    tr("INFO_CAPTION"), 
    tr("CHANGE_APPLICATION_AREA"), 
    (Message::StandardButton)(Message::Yes | Message::Cancel)) != Message::Yes) {
      emit this->isPublicTestVersionChanged();
      return;
  }
  
  QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\GGS\\QGNA", QSettings::NativeFormat);
  bool ok = false;
  int area = settings.value("Repository", 0).toInt(&ok);
  if (!ok)
    area = 0;

  area = area == 0 ? 1 : 0;

  settings.setValue("Repository", area);
  emit this->applicationAreaChanged();
}
