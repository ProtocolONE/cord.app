#include <BestInstallPath.h>
#include <Host/Dbus/ServiceSettingsBridgeProxy.h>
#include <Windows.h>

BestInstallPath::BestInstallPath(QObject *parent /*= 0*/)
  : QObject(parent)
  , _serviceSettings(nullptr)
{

}

BestInstallPath::~BestInstallPath()
{
}

void BestInstallPath::setServiceSettings(ServiceSettingsBridgeProxy* value)
{
  Q_ASSERT(value);
  this->_serviceSettings = value;
}

QString BestInstallPath::expectedPath(const QString& serviceId) const
{
  Q_ASSERT(this->_serviceSettings);

  if (!this->_serviceSettings->isDefaultInstallPath(serviceId))
    return this->_serviceSettings->installPath(serviceId);

  return this->bestInstallPath(serviceId);
}

QString BestInstallPath::bestInstallPath(const QString& serviceId) const
{
  QString defaultDownloadPath = QString("%1Games").arg(this->getBestDrive(serviceId));
  QString name = this->_serviceSettings->name(serviceId);
  return QDir::cleanPath(QString("%1/%2/").arg(defaultDownloadPath, name));
}

QString BestInstallPath::getBestDrive(const QString& serviceId) const
{
  DWORD size = GetLogicalDriveStrings(0, NULL);
  wchar_t *tmp = new wchar_t[size+1]();
  DWORD size1 = GetLogicalDriveStringsW(size, tmp);

  QString systemDrive = QDir::toNativeSeparators(QDir::rootPath());
  QString secondDrive;
  int systemDriveSize = 0;
  int secondDriveSize = 0;

  for (unsigned int i = 0; i < size; i+=4) {
    wchar_t *currentDisk = tmp + i;
    if (GetDriveTypeW(currentDisk) != DRIVE_FIXED)
      continue;

    int freeSpace = this->getDiskFreeSpaceInMb(currentDisk);
    QString diskName = QString::fromWCharArray(currentDisk);

    if (diskName == systemDrive) {
      systemDriveSize = freeSpace;
      continue;
    } 

    if (freeSpace > secondDriveSize) {
      secondDriveSize = freeSpace;
      secondDrive = diskName;
    }
  }

  delete [] tmp;

  if (secondDrive.isEmpty())
    return systemDrive;

  if (this->hasEnoughSpace(serviceId, secondDriveSize))
    return secondDrive;

  if (this->hasEnoughSpace(serviceId, systemDriveSize))
    return systemDrive;

  return secondDrive;
}

int BestInstallPath::getDiskFreeSpaceInMb(const wchar_t* drive) const
{
  ULARGE_INTEGER freeBytes;
  freeBytes.QuadPart = 0L;

  if (!GetDiskFreeSpaceEx(drive, &freeBytes, NULL, NULL))
    return 0;

  return freeBytes.QuadPart / 1048576;
}

bool BestInstallPath::hasEnoughSpace(const QString& serviceId, int free) const
{
  int gameSize = this->_serviceSettings->gameSize(serviceId);
  return free > gameSize;
}
