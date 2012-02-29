#include <ViewModel/GameSettingsViewModel.h>

#include <Core/Service>
#include <Core/System/Shell/ShortCut.h>
#include <GameDownloader/GameDownloadService>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtGui/QFileDialog>
#include <QtCore/QSettings>

#include <Windows.h>
#include <Shlobj.h>

using GGS::Core::Service;

#define CRITICAL_LOG qCritical() << __FILE__ << __LINE__ << __FUNCTION__

GameSettingsViewModel::GameSettingsViewModel(QObject *parent)
  : QObject(parent)
{
}

GameSettingsViewModel::~GameSettingsViewModel()
{
}

void GameSettingsViewModel::createShortcutOnDesktop()
{
  Q_CHECK_PTR(this->_serviceList);
  if (!this->_serviceList->contains(this->_currentServiceId)) {
    CRITICAL_LOG << "Unknown service";
    return;
  }

  GGS::Core::Service *service = (*this->_serviceList)[this->_currentServiceId];
  Q_CHECK_PTR(service);
  this->createShortcutOnDesktop(service);
}

void GameSettingsViewModel::createShortcutOnDesktop(GGS::Core::Service *service)
{
  wchar_t tmp[MAX_PATH];
  BOOL res = SHGetSpecialFolderPath(0, tmp, CSIDL_DESKTOPDIRECTORY, false);

  if (res) {
    QString directorypath = QString::fromWCharArray(tmp);
    this->createShortcut(directorypath, service);
  }
}

void GameSettingsViewModel::createShortcutInMainMenu()
{
  Q_CHECK_PTR(this->_serviceList);
  if (!this->_serviceList->contains(this->_currentServiceId)) {
    CRITICAL_LOG << "Unknown service";
    return;
  }

  GGS::Core::Service *service = (*this->_serviceList)[this->_currentServiceId];
  Q_CHECK_PTR(service);
  this->createShortcutInMainMenu(service);
}

void GameSettingsViewModel::createShortcutInMainMenu(GGS::Core::Service *service)
{
  wchar_t tmp[MAX_PATH];
  BOOL res = SHGetSpecialFolderPath(0, tmp, CSIDL_STARTMENU, false);
  if (res) {
    QString directorypath = QString::fromWCharArray(tmp);
    this->createShortcut(directorypath, service);
  }
}

void GameSettingsViewModel::createShortcut(const QString& path, GGS::Core::Service *service)
{
  QString lnkroot = path;
  lnkroot.append("\\");
  lnkroot.append(service->name());
  lnkroot.append(".lnk");

  GGS::Core::System::Shell::ShortCut object;
  object.setArguments(QString("/uri:gamenet://startservice/%1").arg(service->id()));
  object.setDescription(QString("Short cut for game %1").arg(service->name()));
  object.setShowCmd(GGS::Core::System::Shell::ShortCut::MinNoActive);
  object.setWorkingDirectory(QCoreApplication::applicationDirPath());
  // UNDONE: проверить что эта функция кошерна - у нее в дескрипшене есть фишка про смену дериктории самим аппликейшеном.
  object.setPath(QCoreApplication::applicationFilePath());

  QString iconPath = QString("%1/images/icons/%2.ico").arg(QCoreApplication::applicationDirPath(), service->id());
  iconPath = QFile::exists(iconPath) ? iconPath : QCoreApplication::applicationFilePath();
  object.setIconLocation(iconPath);
  object.setIconIndex(0);
  object.save(lnkroot);
}

void GameSettingsViewModel::submitSettings()
{
  Q_CHECK_PTR(this->_serviceList);
  if (!this->_serviceList->contains(this->_currentServiceId)) {
    CRITICAL_LOG << "Unknown service";
    return;
  }

  GGS::Core::Service *service = (*this->_serviceList)[this->_currentServiceId];
  Q_CHECK_PTR(service);

  bool secondPath = service->hashDownloadPath();
  if (service->installPath() != this->_installPath || 
    (secondPath && service->downloadPath() != this->_downloadPath)) {
    service->setInstallPath(this->_installPath);
    service->setDownloadPath(secondPath ? this->_downloadPath : this->_installPath);
    service->setTorrentFilePath(secondPath ? this->_downloadPath : this->_installPath);

    QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);
    settings.beginGroup(service->id());
    settings.setValue("DownloadPath", service->downloadPath());
    settings.setValue("InstallPath", service->installPath());
    service->setIsDefaultInstallPath(true);
    settings.endGroup();

    this->_gameDownloader->directoryChanged(service);
  }
}

void GameSettingsViewModel::restoreClient()
{
  Q_CHECK_PTR(this->_serviceList);
  if (!this->_serviceList->contains(this->_currentServiceId)) {
    CRITICAL_LOG << "Unknown service";
    return;
  }

  GGS::Core::Service *service = (*this->_serviceList)[this->_currentServiceId];
  Q_CHECK_PTR(service);
  this->_gameDownloader->start(service, GGS::GameDownloader::Recheck);
}

void GameSettingsViewModel::switchGame(const QString& serviceId)
{
  Q_CHECK_PTR(this->_serviceList);
  if (!this->_serviceList->contains(serviceId)) {
    CRITICAL_LOG << "Unknown service";
    return;
  }

  GGS::Core::Service *service = (*this->_serviceList)[serviceId];
  Q_CHECK_PTR(service);
  this->_currentServiceId = serviceId;
  this->_installPath = service->installPath();
  this->_downloadPath = service->downloadPath();
  this->_hasDownloadPath = service->hashDownloadPath();

  emit this->installPathChanged();
  emit this->downloadPathChanged();
  emit this->hasDownloadPathChanged();
}

void GameSettingsViewModel::setServiceList(QHash<QString, GGS::Core::Service *> *serviceList)
{
  this->_serviceList = serviceList;
}

void GameSettingsViewModel::setGameDownloader(GGS::GameDownloader::GameDownloadService *gameDownloader)
{
  this->_gameDownloader = gameDownloader;
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

void GameSettingsViewModel::browseInstallPath()
{
  Q_CHECK_PTR(this->_serviceList);
  if (!this->_serviceList->contains(this->_currentServiceId)) {
    CRITICAL_LOG << "Unknown service";
    return;
  }

  GGS::Core::Service *service = (*this->_serviceList)[this->_currentServiceId];
  Q_CHECK_PTR(service);

  QString newDirectory = QFileDialog::getExistingDirectory(0, tr("CAPTION_OPEN_DIR"), service->installPath(), 
    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if (newDirectory.isEmpty())
    return;

  QDir dir(newDirectory);
  if (!dir.exists())
    return;

  this->setInstallPath(newDirectory);
}

void GameSettingsViewModel::browseDownloadPath()
{
  Q_CHECK_PTR(this->_serviceList);
  if (!this->_serviceList->contains(this->_currentServiceId)) {
    CRITICAL_LOG << "Unknown service";
    return;
  }

  GGS::Core::Service *service = (*this->_serviceList)[this->_currentServiceId];
  Q_CHECK_PTR(service);
  if (!service->hashDownloadPath())
    return;

  QString newDirectory = QFileDialog::getExistingDirectory(0, tr("CAPTION_OPEN_DIR"), service->downloadPath(), 
    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if (newDirectory.isEmpty())
    return;

  QDir dir(newDirectory);
  if (!dir.exists())
    return;

  this->setDownloadPath(newDirectory);
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
    if (QFile::exists(lnkroot))
      QFile::remove(lnkroot);
  }

  wchar_t tmp2[MAX_PATH] = {0};
  res = SHGetSpecialFolderPath(0, tmp2, CSIDL_STARTMENU, false);

  if (res) {
    QString lnkroot = QString::fromWCharArray(tmp2);
    lnkroot.append("\\");
    lnkroot.append(name);
    lnkroot.append(".lnk");
    if (QFile::exists(lnkroot))
      QFile::remove(lnkroot);
  }
}


