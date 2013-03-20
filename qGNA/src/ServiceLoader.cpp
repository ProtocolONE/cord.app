#include <ServiceLoader.h>

#include <Settings/Settings>
#include <Features/CASettingsFix.h>

#include <GameExecutor/Hook/RestoreResolution>
#include <GameExecutor/Hook/DisableIEDefalutProxy>
#include <GameExecutor/Hook/DisableDEP>
#include <GameExecutor/Hook/DownloadCustomFile>
#include <GameExecutor/Hook/Mw2DownloadAndCheckXmlConfig.h>
#include <GameExecutor/Hook/DisableAeroHook.h>
#include <GameExecutor/Hook/DefaultAikaSettings.h>
#include <GameExecutor/Hook/BannerDownload.h>
#include <GameExecutor/Hook/RestoreFileModification.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QSettings>
#include <QtCore/QDateTime>
#include <QtCore/QDir>

#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>

ServiceLoader::ServiceLoader(QObject *parent) : QObject(parent)
{
}

ServiceLoader::~ServiceLoader()
{
}

QHash<QString, GGS::Core::Service *>& ServiceLoader::serviceMap()
{
  return this->_serviceMap;
}

void ServiceLoader::init(GGS::Core::Service::Area gameArea)
{
  this->_gameArea = gameArea;

  this->initService("300002010000000000", "http://fs0.gamenet.ru/update/aika/", "Aika2");
  this->initService("300003010000000000", "http://fs0.gamenet.ru/update/bs/", "BS");
  this->initService("300005010000000000", "http://fs0.gamenet.ru/update/warinc/", "FireStorm");
  this->initService("300006010000000000", "http://fs0.gamenet.ru/update/mw2/", "MW2");
  this->initService("300009010000000000", "http://fs0.gamenet.ru/update/ca/", "CombatArms");
  this->initGAService();

  this->_gameDownloaderBuilder->gameDownloader().registerHook("300005010000000000", 0, 10, &this->_installDependencyHook);
  this->_gameDownloaderBuilder->gameDownloader().registerHook("300006010000000000", 0, 10, &this->_installDependencyHook);
}

GGS::Core::Service* ServiceLoader::getService(const QString& id)
{
  if (!this->_serviceMap.contains(id))
    return 0;

  return this->_serviceMap[id];
}

void ServiceLoader::initService(const QString& id, const QString& torrentUrl, const QString& name)
{
  GGS::Core::Service *service = new GGS::Core::Service();
  service->setId(id);
  service->setArea(this->_gameArea);
  service->setIsDownloadable(true);
  service->setName(name);

  bool hasDownloadPath = (id == "300002010000000000" || id == "300009010000000000");
  service->setHashDownloadPath(hasDownloadPath);

  QString root = QCoreApplication::applicationDirPath();

  QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);

  settings.beginGroup(id);

  QString currentInstallPath = settings.value("InstallPath").toString();
  if (currentInstallPath.isEmpty()) {
    service->setIsDefaultInstallPath(true);
    currentInstallPath = QString("%1/Games/%2").arg(root, name); 
  } else {
    service->setIsDefaultInstallPath(false);
  }

  QString currentDownloadPath = settings.value("DownloadPath").toString();
  if (currentDownloadPath.isEmpty()) 
    currentDownloadPath = QString("%1/Downloads/%2/").arg(root, name);
  
  service->setDownloadPath(hasDownloadPath ? currentDownloadPath : currentInstallPath);
  service->setInstallPath(currentInstallPath);
  service->setTorrentFilePath(hasDownloadPath ? currentDownloadPath : currentInstallPath);

  if (id == "300002010000000000" || id == "300009010000000000")
    service->setExtractorType("D9E40EE5-806F-4B7D-8D5C-B6A4BF0110E9");
  else
    service->setExtractorType("3A3AC78E-0332-45F4-A466-89C2B8E8BB9C");

  service->setTorrentUrl(torrentUrl);

  this->initHooks(id, service);
  this->_serviceMap[id] = service;

  this->setExecuteUrl(id, currentInstallPath);
  this->migrateInstallDate(id);
}

void ServiceLoader::initGAService()
{
  using namespace GGS::Core;
  QString id("300007010000000000");
  Service *service = new Service();
  service->setArea(Service::Live);
  service->setIsDownloadable(false);
  service->setName(id);
  service->setId(id);
  service->setGameId("83");
  service->setUrl(QUrl("http://www.playga.ru/"));

  this->_serviceMap[id] = service;
}

void ServiceLoader::setExecutor(GGS::GameExecutor::GameExecutorService *executor)
{
  this->_gameExecutorService = executor;
}

void ServiceLoader::setDownloadBuilder(GGS::GameDownloader::Builder *builder)
{
  this->_gameDownloaderBuilder = builder;
}

void ServiceLoader::setExecuteUrl(const QString& id, QString currentInstallPath)
{
  // HACK тут везде предполагается что сервис в зоне live. Если это не так надо все пути делать правильно вычисляемыми.
  // https://jira.gamenet.ru:8443/browse/GN-6523 - вот вообщем то и тикет для этого
  GGS::Core::Service *service = this->getService(id);
  QUrl url;

  if (id == "300002010000000000") {
    url.setScheme("exe");
    url.setPath(QString("%1/%2/aikaru.exe").arg(currentInstallPath, service->areaString()));
    url.addQueryItem("workingDir", QString("%1/%2/").arg(currentInstallPath, service->areaString()));
    url.addQueryItem("args", "%login% %token% 300002010000000000 login");
    service->setGameId("631");
  } else if (id == "300003010000000000") {
    url.setScheme("exe");
    url.setPath(QString("%1/%2/client/client.exe").arg(currentInstallPath, service->areaString()));
    url.addQueryItem("workingDir", QString("%1/%2/").arg(currentInstallPath, service->areaString()));
    url.addQueryItem("args", "%userId% %appKey% %token%");
    url.addQueryItem("downloadCustomFile", 
      "launcher/serverinfo_back.xml,http://files.gamenet.ru/update/bs/,1,config/lastlogin.xml,http://files.gamenet.ru/update/bs/,0");
    service->setGameId("71");

    GGS::Settings::Settings settings;
    settings.beginGroup("gameExecutor");
    settings.beginGroup("serviceInfo");
    settings.beginGroup(id);
    
    bool ok;
    int overlayEnabled = settings.value("overlayEnabled", 1).toInt(&ok);
    if (overlayEnabled != 0 || !ok) {
      // HACK
      //url.addQueryItem("injectDll", "D:\\Prog\\Qt\\!GIT\\QGNA Components\\overlay\\!build\\Overlay\\Debug\\OverlayX86d.dll");

      // Выключили 01.03.2013
      //#ifdef _DEBUG
      //    QString injectedDll = QCoreApplication::applicationDirPath() + "/OverlayX86d.dll"; 
      //#else
      //    QString injectedDll = QCoreApplication::applicationDirPath() + "/OverlayX86.dll";
      //#endif
      //
      //url.addQueryItem("injectDll", injectedDll);

    }

  } else if (id == "300005010000000000") {
    url.setScheme("exe");
    url.setPath(QString("%1/%2/WarInc.exe").arg(currentInstallPath, service->areaString()));
    url.addQueryItem("workingDir", QString("%1/%2/").arg(currentInstallPath, service->areaString()));
    url.addQueryItem("args", "-WOUpdatedOk -gna %userId% %appKey% %token%");
    service->setGameId("70");
  } else if (id == "300006010000000000") {
    url.setScheme("exe");
    url.setPath(QString("%1/%2/mw2_bin/mw2.exe").arg(currentInstallPath, service->areaString()));
    url.addQueryItem("workingDir", QString("%1/%2/").arg(currentInstallPath, service->areaString()));
    url.addQueryItem("args", "%serverinfo% %userId% %token% %appKey%");
    url.addQueryItem("downloadCustomFile", "mw2_bin/cfg_engine.xml,http://files.gamenet.ru/update/mw2/,0");

    service->setGameId("84");
  } else if (id == "300009010000000000") {
    url.setScheme("exe");
    url.setPath(QString("%1/%2/engine.exe").arg(currentInstallPath, service->areaString()));
    url.addQueryItem("workingDir", QString("%1/%2/").arg(currentInstallPath, service->areaString()));
    url.addQueryItem("args", "-windowtitle \"CombatArms\" -rez Engine.REZ -rez Game -authip 31.25.225.205 -authport 10002 -pcroom 0 -Ver Ver_RU_2.1207.03 -UserID %userid% -Password %appkey%:%token%");

//#ifdef _DEBUG
//    QString injectedDll = QCoreApplication::applicationDirPath() + "/OverlayX86d.dll"; 
//#else
//    QString injectedDll = QCoreApplication::applicationDirPath() + "/OverlayX86.dll";
//#endif
//
//    url.addQueryItem("injectDll", injectedDll);

    service->setGameId("92");
  }

  service->setUrl(url);
}

void ServiceLoader::initHooks(const QString& id, GGS::Core::Service* service)
{
  using namespace GGS::GameExecutor::Hook;
  using namespace GGS::GameDownloader::Hooks;

  if (id != "300007010000000000")
    this->_gameDownloaderBuilder->gameDownloader().registerHook(id, 100, 0, &this->_oldGameClientMigrate);

  if (id == "300003010000000000") {
    this->_gameExecutorService->addHook(*service, new DisableDEP(service), 0);
    this->_gameExecutorService->addHook(*service, new DownloadCustomFile(service), 100);
  }

  if (id == "300002010000000000") {
    this->_gameExecutorService->addHook(*service, new DisableIEDefalutProxy(service), 0);
    this->_gameExecutorService->addHook(*service, new RestoreResolution(service), 0);
    this->_gameExecutorService->addHook(*service, new DefaultAikaSettings(service), 0);
  }

  if (id == "300006010000000000") {
    this->_gameExecutorService->addHook(*service, new Mw2DownloadAndCheckXmlConfig(service), 100);
    this->_gameExecutorService->addHook(*service, new RestoreFileModification(service), 0);
    service->setExternalDependencyList("vcredist_x86.exe,/Q");
  }

  if (id == "300009010000000000") {
    Features::CASettingsFix *hook = new Features::CASettingsFix(service);
    hook->setResolution(QApplication::desktop()->screenGeometry(QApplication::desktop()->primaryScreen()));
    SIGNAL_CONNECT_CHECK(QObject::connect(
      &this->_gameDownloaderBuilder->gameDownloader(), SIGNAL(serviceInstalled(const GGS::Core::Service *)), 
      hook, SLOT(install(const GGS::Core::Service *))));

    this->_gameExecutorService->addHook(*service, hook);
    this->_gameExecutorService->addHook(*service, new DisableIEDefalutProxy(service), 0);
    this->_gameExecutorService->addHook(*service, new BannerDownload(service), 0);


    DWORD verion = GetVersion();
    DWORD dwMajorVersion = (DWORD)(LOBYTE(LOWORD(verion)));
    DWORD dwMinorVersion = (DWORD)(HIBYTE(LOWORD(verion)));
    if (dwMajorVersion == 6 && dwMinorVersion >=0 && dwMinorVersion <= 2)
      this->_gameExecutorService->addHook(*service, new GGS::GameExecutor::Hook::DisableAeroHook(service));

    // TODO change to next code after switch to 4.8.3 or grater
    //QSysInfo::WinVersion version = QSysInfo::windowsVersion();
    //if (version == QSysInfo::WV_VISTA || version == QSysInfo::WV_WINDOWS7 || version == QSysInfo::WV_WINDOWS8)
    //  this->_gameExecutorService->addHook(*service, new GGS::GameExecutor::Hook::DisableAeroHook(service));
  }

  if (id == "300005010000000000")
    service->setExternalDependencyList("dxwebsetup.exe,/Q");
}

void ServiceLoader::migrateInstallDate(const QString& serviceId)
{
  if (!this->_gameDownloaderBuilder->gameDownloader().isInstalled(serviceId))
    return;

  GGS::Settings::Settings settings;
  settings.beginGroup("GameDownloader");
  settings.beginGroup(serviceId);

  if (settings.contains("installDate"))
    return;

  settings.setValue("installDate", QDateTime::currentDateTime());
}

QString ServiceLoader::getExpectedInstallPath(const QString& serviceId)
{
  GGS::Core::Service *service = this->getService(serviceId);
  Q_CHECK_PTR(service);

  if (!this->hasDefaultInstallPath(serviceId))
    return service->installPath();

  QString defaultDownloadPath = QString("%1Games").arg(this->getBestDrive(serviceId));
  return QDir::cleanPath(QString("%1/%2/").arg(defaultDownloadPath, service->name()));
}

int ServiceLoader::getDiskFreeSpaceInMb(LPCWSTR drive)
{
  ULARGE_INTEGER freeBytes;
  freeBytes.QuadPart = 0L;

  if (!GetDiskFreeSpaceEx(drive, &freeBytes, NULL, NULL))
    return 0;

  return freeBytes.QuadPart / 1048576;
}

bool ServiceLoader::hasEnoughSpace(const QString& serviceId, int free)
{
  if (serviceId == "300002010000000000")
    return free > 2800;

  if (serviceId == "300003010000000000")
    return free > 2400;

  if (serviceId == "300005010000000000")
    return free > 2500;

  if (serviceId == "300006010000000000")
    return free > 4400;

  if (serviceId == "300009010000000000")
    return free > 4800;

  return free > 1000;
}

QString ServiceLoader::getBestDrive(const QString& serviceId)
{
  DWORD size = GetLogicalDriveStrings(0, NULL);
  wchar_t *tmp = new wchar_t[size+1]();
  DWORD size1 = GetLogicalDriveStringsW(size, tmp);

  QString systemDrive = QDir::toNativeSeparators(QDir::rootPath());
  QString secondDrive;
  int systemDriveSize = 0;
  int secondDriveSize = 0;

  for (int i = 0; i < size; i+=4) {
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

bool ServiceLoader::hasDefaultDownloadPath(const QString& id)
{
  QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);
  settings.beginGroup(id);
  return settings.value("DownloadPath", "").toString().isEmpty();
}

bool ServiceLoader::hasDefaultInstallPath(const QString& id)
{
  QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);
  settings.beginGroup(id);
  return settings.value("InstallPath", "").toString().isEmpty();
}

bool ServiceLoader::anyLicenseAccepted()
{
  QStringList ids;
  ids << "300002010000000000"
    << "300003010000000000"
    << "300004010000000000"
    << "300005010000000000"
    << "300006010000000000"
    << "300009010000000000";

  QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);
  Q_FOREACH(QString id, ids) {
    settings.beginGroup(id);
    QString license = settings.value("LicenseHash").toString();
    settings.endGroup();

    if (!license.isEmpty()) 
      return true;
  }

  QString license = settings.value("LicenseHash").toString();
  if (!license.isEmpty())
    return true;

  return false;
}