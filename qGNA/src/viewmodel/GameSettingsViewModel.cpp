#include <ViewModel/GameSettingsViewModel.h>

#include <Core/Service>
#include <Core/System/Shell/ShortCut.h>
#include <GameDownloader/GameDownloadService>
#include <Core/UI/Message>
#include <Settings/Settings>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtWidgets/QFileDialog>
#include <QtCore/QSettings>
#include <QDirIterator>

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

void GameSettingsViewModel::createShortcut(const QString& path, GGS::Core::Service *service)
{
  QString lnkroot = path;
  lnkroot.append("\\");
  lnkroot.append(service->name());
  lnkroot.append(".lnk");

  GGS::Settings::Settings settings;
  settings.beginGroup("GameInstallInfo");
  settings.beginGroup(service->id());
  QStringList icons = this->deserialize(settings.value("iconPaths", QByteArray()).toByteArray());
  icons << lnkroot;
  settings.setValue("iconPaths", this->serialize(icons));
  
  settings.endGroup();
  QStringList filesToDelete = this->deserialize(settings.value("filesToDelete", QByteArray()).toByteArray());
  filesToDelete << lnkroot;
  settings.setValue("filesToDelete", this->serialize(filesToDelete));
  
  GGS::Core::System::Shell::ShortCut object;
  object.setArguments(QString("/uri:gamenet://startservice/%1").arg(service->id()));
  object.setDescription(QString("Short cut for game %1").arg(service->name()));
  object.setShowCmd(GGS::Core::System::Shell::ShortCut::MinNoActive);
  object.setWorkingDirectory(QCoreApplication::applicationDirPath());
  // UNDONE: ��������� ��� ��� ������� ������� - � ��� � ����������� ���� ����� ��� ����� ���������� ����� �������������.
  object.setPath(QCoreApplication::applicationFilePath());

  QString iconPath = QString("%1/Assets/Images/icons/%2.ico").arg(QCoreApplication::applicationDirPath(), service->id());
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

QString GetFolderName(int type) {
  wchar_t tmp[MAX_PATH];
  if (SHGetSpecialFolderPath(0, tmp, type, false))
    return QString::fromWCharArray(tmp);

  return QString();
}

QString GameSettingsViewModel::browseDirectory(const QString& serviceId, const QString& name, const QString& defaultDir)
{
  GGS::Core::Service service;
  service.setName(name);
  return this->getGameDirectory(0, &service, defaultDir);
}

QString GameSettingsViewModel::getGameDirectory(QWidget* parent, GGS::Core::Service *service, const QString& defaultDir)
{
  QString newDirectory = QFileDialog::getExistingDirectory(parent, tr("CAPTION_OPEN_DIR"), defaultDir,
    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if (newDirectory.isEmpty())
    return QString();

  newDirectory = QDir::toNativeSeparators(newDirectory);

  int deniedFolders[] = { CSIDL_PROGRAM_FILES,
    CSIDL_PROGRAM_FILES_COMMON,
    CSIDL_SYSTEM,
    CSIDL_WINDOWS,
    -1 };

  for (int i = 0; i < sizeof(deniedFolders) / sizeof(int); ++i) {
    QString folderName;

    if (deniedFolders[i] == -1) {
      WCHAR szNativeProgramFilesFolder[MAX_PATH];
      ExpandEnvironmentStrings(L"%ProgramW6432%", 
        szNativeProgramFilesFolder, 
        ARRAYSIZE(szNativeProgramFilesFolder));
      folderName = QString::fromWCharArray(szNativeProgramFilesFolder);
    } else {
      folderName = QDir::toNativeSeparators(GetFolderName(deniedFolders[i]));
    }

    if (newDirectory.compare(folderName, Qt::CaseInsensitive) == 0) {
      GGS::Core::UI::Message::information(tr("DIRECTORY_NOT_ACCEPTED_TO_INSTALL_INFO"),
        tr("DIRECTORY_NOT_ACCEPTED_TO_INSTALL_BODY_INFORMATION").arg(folderName));

      return QString();
    }
  }

  QString winDir = QDir::toNativeSeparators(GetFolderName(CSIDL_WINDOWS));

  if (newDirectory.startsWith(winDir, Qt::CaseInsensitive)) {
    GGS::Core::UI::Message::information(tr("DIRECTORY_NOT_ACCEPTED_TO_INSTALL_INFO"), 
      tr("DIRECTORY_NOT_ACCEPTED_TO_INSTALL_BODY_INFORMATION").arg(winDir));

    return QString();
  }

  QDir dir(newDirectory);
  dir.setFilter(QDir::NoDotAndDotDot);

  if (dir.isRoot()) {
    return newDirectory + service->name();
  } 
  
  QDirIterator it(newDirectory, QDirIterator::Subdirectories);
  for (int count = 0; it.hasNext(); ++count, it.next()) {
    if (count > 2) {
      if (GGS::Core::UI::Message::question(tr("DIRECTORY_NOT_EMPTY_INFO"), 
          tr("DIRECTORY_NOT_EMPTY_QUESTION"), 
          static_cast<GGS::Core::UI::Message::StandardButton>(GGS::Core::UI::Message::Ok | GGS::Core::UI::Message::Cancel)) == GGS::Core::UI::Message::Cancel) {
            return QString();
        }

      break;
    }
  }

  return newDirectory;
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

  QString newDirectory = GameSettingsViewModel::getGameDirectory(qobject_cast<QWidget*>(parent()), service, service->installPath());
  if (newDirectory.isEmpty()) {
    return;
  }

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

  QString newDirectory = GameSettingsViewModel::getGameDirectory(qobject_cast<QWidget*>(parent()), service, service->downloadPath());
  if (newDirectory.isEmpty())
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


