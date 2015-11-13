#include <ViewModel/GameSettingsViewModel.h>
#include <Helper/GetDirectoryDialog.h>

#include <Core/Service>
#include <Core/System/Shell/ShortCut.h>
#include <GameDownloader/GameDownloadService>
#include <Core/UI/Message>
#include <Settings/Settings>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtWidgets/QFileDialog>
#include <QtCore/QSettings>
#include <QDirIterator>

#include <Windows.h>
#include <Shlobj.h>

#include <Host/Dbus/ServiceSettingsBridgeProxy.h>
#include <Host/Dbus/DownloaderBridgeProxy.h>

using GGS::Core::Service;

#define CRITICAL_LOG qCritical() << __FILE__ << __LINE__ << __FUNCTION__

GameSettingsViewModel::GameSettingsViewModel(QObject *parent)
  : QObject(parent)
  , _serviceSettings(nullptr)
{
}

GameSettingsViewModel::~GameSettingsViewModel()
{
}

void GameSettingsViewModel::createShortcutOnDesktop(const QString& serviceId)
{
  wchar_t tmp[MAX_PATH] = {0};
  if (!SHGetSpecialFolderPath(0, tmp, CSIDL_DESKTOPDIRECTORY, false))
    return;

  QString directorypath = QString::fromWCharArray(tmp);
  this->createShortcut(directorypath, serviceId);
}

void GameSettingsViewModel::createShortcutInMainMenu(const QString& serviceId)
{
  wchar_t tmp[MAX_PATH] = {0};
  if (!SHGetSpecialFolderPath(0, tmp, CSIDL_STARTMENU, false))
    return;

  QString directorypath = QString::fromWCharArray(tmp);
  this->createShortcut(directorypath, serviceId);
}

void GameSettingsViewModel::removeShortcuts(const QString& serviceId)
{
  QString name = this->_serviceSettings->name(serviceId);
  if (name.isEmpty())
    return;

  this->removeShortCutByName(name);
}

QStringList GameSettingsViewModel::deserialize(QByteArray serialized)
{
  QStringList result;
  QDataStream in(&serialized, QIODevice::ReadOnly);
  in >> result;
  return result;
}

QByteArray GameSettingsViewModel::serialize(QStringList stringList)
{
  QByteArray byteArray;
  QDataStream out(&byteArray, QIODevice::WriteOnly);
  out << stringList;
  return byteArray;
}

void GameSettingsViewModel::createShortcut(const QString& path, const QString& serviceId)
{
  QString name = this->_serviceSettings->name(serviceId);
  if (name.isEmpty())
    return;

  QString lnkroot = path;
  lnkroot.append("\\");
  lnkroot.append(name);
  lnkroot.append(".lnk");

  GGS::Settings::Settings settings;
  settings.beginGroup("GameInstallInfo");
  settings.beginGroup(serviceId);
  QStringList icons = this->deserialize(settings.value("iconPaths", QByteArray()).toByteArray());
  icons << lnkroot;
  settings.setValue("iconPaths", this->serialize(icons));
  
  settings.endGroup();
  QStringList filesToDelete = this->deserialize(settings.value("filesToDelete", QByteArray()).toByteArray());
  filesToDelete << lnkroot;

  settings.setValue("filesToDelete", this->serialize(filesToDelete));
  
  GGS::Core::System::Shell::ShortCut object;
  object.setDescription(QString("Short cut for game %1").arg(name));
  object.setShowCmd(GGS::Core::System::Shell::ShortCut::MinNoActive);
  object.setWorkingDirectory(QCoreApplication::applicationDirPath());
  object.setPath(QString("gamenet://startservice/%1").arg(serviceId));

  QString iconPath = QString("%1/icons/%2.ico")
    .arg(QStandardPaths::writableLocation(QStandardPaths::DataLocation)).arg(serviceId);

  iconPath = QFile::exists(iconPath) ? iconPath : QCoreApplication::applicationFilePath();
  object.setIconLocation(iconPath);
  object.setIconIndex(0);
  object.save(lnkroot);
}

void GameSettingsViewModel::submitSettings()
{
  bool secondPath = this->_serviceSettings->hasDownloadPath(this->_currentServiceId);
  QString downloadPath = secondPath ? this->_downloadPath : this->_installPath;
  this->_serviceSettings->setInstallPath(this->_currentServiceId, this->_installPath);
  this->_serviceSettings->setDownloadPath(this->_currentServiceId, downloadPath);
}

void GameSettingsViewModel::restoreClient()
{
  this->_downloader->start(this->_currentServiceId, static_cast<int>(GGS::GameDownloader::Recheck));
}

void GameSettingsViewModel::switchGame(const QString& serviceId)
{
  this->_currentServiceId = serviceId;
  
  this->_installPath = this->_serviceSettings->installPath(this->_currentServiceId);
  this->_downloadPath = this->_serviceSettings->downloadPath(this->_currentServiceId);
  this->_hasDownloadPath = this->_serviceSettings->hasDownloadPath(this->_currentServiceId);

  emit this->installPathChanged();
  emit this->downloadPathChanged();
  emit this->hasDownloadPathChanged();
}

const QString& GameSettingsViewModel::installPath()
{
  return this->_installPath;
}

void GameSettingsViewModel::setInstallPath(const QString& installPath)
{
  if (this->_installPath != installPath) {
    this->_installPath = installPath;
    emit this->installPathChanged();
  }
}

const QString& GameSettingsViewModel::downloadPath()
{
  return this->_downloadPath;
}

void GameSettingsViewModel::setDownloadPath(const QString& downloadPath)
{
  if (this->_downloadPath != downloadPath) {
    this->_downloadPath = downloadPath;
    emit this->downloadPathChanged();
  }
}

bool GameSettingsViewModel::hasDownloadPath()
{
  return this->_hasDownloadPath;
}

void GameSettingsViewModel::setHasDownloadPath(bool hasDownloadPath)
{
  if (this->_hasDownloadPath != hasDownloadPath) {
    this->_hasDownloadPath = hasDownloadPath;
    emit this->hasDownloadPathChanged();
  }
}

void GameSettingsViewModel::browseInstallPath(const QString &preferedPath)
{
  QString defaultPath = !preferedPath.isEmpty()
    ? preferedPath
    : this->_serviceSettings->installPath(this->_currentServiceId);
  
  GetDirectoryDialog *dialog = new GetDirectoryDialog(qobject_cast<QWidget*>(this->parent()));
  dialog->getDirectory(this->_serviceSettings->name(this->_currentServiceId), defaultPath);

  QObject::connect(dialog, &GetDirectoryDialog::directoryEntered, [dialog, this](const QString& newDirectory) {
    dialog->deleteLater();

    if (!newDirectory.isEmpty()) 
      this->setInstallPath(newDirectory);
  });
}

void GameSettingsViewModel::browseDownloadPath()
{
  if (!this->_serviceSettings->hasDownloadPath(this->_currentServiceId))
    return;

  QString name = this->_serviceSettings->name(this->_currentServiceId);
  QString downloadPath = this->_serviceSettings->downloadPath(this->_currentServiceId);

  GetDirectoryDialog *dialog = new GetDirectoryDialog(qobject_cast<QWidget*>(this->parent()));
  dialog->getDirectory(name, downloadPath);

  QObject::connect(dialog, &GetDirectoryDialog::directoryEntered, [dialog, this](const QString& newDirectory) {
    dialog->deleteLater();

    if (!newDirectory.isEmpty()) 
      this->setDownloadPath(newDirectory);
  });
}

void GameSettingsViewModel::removeShortCutByName(const QString& name)
{
  wchar_t tmp[MAX_PATH] = {0};
  BOOL res = SHGetSpecialFolderPath(0, tmp, CSIDL_DESKTOPDIRECTORY, false);

  if (res) {
    QString lnkroot = QString::fromWCharArray(tmp);
    lnkroot.append("\\");
    lnkroot.append(name);
    lnkroot.append(".lnk");
    QFile::remove(lnkroot);
  }

  wchar_t tmp2[MAX_PATH] = {0};
  res = SHGetSpecialFolderPath(0, tmp2, CSIDL_STARTMENU, false);

  if (res) {
    QString lnkroot = QString::fromWCharArray(tmp2);
    lnkroot.append("\\");
    lnkroot.append(name);
    lnkroot.append(".lnk");
    QFile::remove(lnkroot);
  }
}

void GameSettingsViewModel::setServiceSettings(ServiceSettingsBridgeProxy *value)
{
  Q_ASSERT(value);
  this->_serviceSettings = value;
}

void GameSettingsViewModel::setDownloader(DownloaderBridgeProxy *value)
{
  Q_ASSERT(value);
  this->_downloader = value;
}

bool GameSettingsViewModel::isOverlayEnabled(const QString& serviceId)
{
  return this->_serviceSettings->isOverlayEnabled(serviceId);
}

void GameSettingsViewModel::setOverlayEnabled(const QString& serviceId, bool enabled)
{
  this->_serviceSettings->setOverlayEnabled(serviceId, enabled);
}

bool GameSettingsViewModel::isPrefer32Bit(const QString& serviceId)
{
  return this->_serviceSettings->isPrefer32Bit(serviceId);
}

void GameSettingsViewModel::setPrefer32Bit(const QString& serviceId, bool enabled)
{
  this->_serviceSettings->setPrefer32Bit(serviceId, enabled);
}
