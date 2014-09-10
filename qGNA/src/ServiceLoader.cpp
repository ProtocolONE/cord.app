#include <ServiceLoader.h>

#include <Settings/Settings>
#include <Features/CASettingsFix.h>

#include <Features/Thetta/ThettaMonitor.h>
#include <Features/Thetta/Protector.h>
#include <Features/Thetta/DistrIntegrityExecutorHook.h>

#include <GameExecutor/Extension.h>

#include <Dbus/ServiceSettingsBridgeProxy.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QSettings>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QUrlQuery>

#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>

ServiceLoader::ServiceLoader(QObject *parent) 
  : QObject(parent)
  , _serviceSettings(nullptr)
{
}

ServiceLoader::~ServiceLoader()
{
}

void ServiceLoader::init(GGS::Core::Service::Area gameArea, GGS::Core::Service::Area applicationArea)
{
  this->_gameArea = gameArea;
  this->_applicationArea = applicationArea;

  this->initService("300002010000000000", "http://fs0.gamenet.ru/update/aika/", "Aika2");
  this->initService("300003010000000000", "http://fs0.gamenet.ru/update/bs/", "BS");
  this->initService("300012010000000000", "http://fs0.gamenet.ru/update/reborn/", "Reborn");
  this->initService("300005010000000000", "http://fs0.gamenet.ru/update/warinc/", "FireStorm");
  this->initService("300009010000000000", "http://fs0.gamenet.ru/update/ca/", "CombatArms");
  this->initService("300004010000000000", "http://fs0.gamenet.ru/update/rot/", "RageofTitans");
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

  bool hasDownloadPath = (id == "300004010000000000" || id == "300002010000000000" || id == "300009010000000000" || id == "100009010000000000");
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

  MemoryProtector_CheckFunction3(0x0A08C78A, 0x171DD8F4, 0x0DD60760, 0x37e12d6a);

  if (id == "300002010000000000" || id == "300009010000000000" || id == "100009010000000000" || id == "300004010000000000")
    service->setExtractorType("D9E40EE5-806F-4B7D-8D5C-B6A4BF0110E9");
  else
    service->setExtractorType("3A3AC78E-0332-45F4-A466-89C2B8E8BB9C");

  service->setTorrentUrl(torrentUrl);

  this->_serviceMap[id] = service;

  this->setExecuteUrl(id, currentInstallPath);
}

void ServiceLoader::setGameDownloader(GGS::GameDownloader::GameDownloadService *gameDownloader)
{
  this->_gameDownloader = gameDownloader;
}

void ServiceLoader::setExecuteUrl(const QString& id, QString currentInstallPath)
{
  GGS::Core::Service *service = this->getService(id);
  QUrl url;

  // HACK !!!!!!!!!!!!!
  //url.addQueryItem("injectDll", "D:\\Prog\\Qt\\!GIT\\QGNA Components\\overlay\\!build\\Overlay\\Release\\OverlayX86.dll");

  if (id == "300002010000000000") { // aika
    url.setScheme("exe");
    url.setPath(QString("%1/%2/aikaru.exe").arg(currentInstallPath, service->areaString()));
    url.addQueryItem("workingDir", QString("%1/%2/").arg(currentInstallPath, service->areaString()));
    url.addQueryItem("args", "%login% %token% 300002010000000000 login");

    url.addQueryItem("downloadCustomFile", "UI/GuildMarkWorld1.tga,http://files.gamenet.ru/update/aika,2");

    service->setGameId("631");

    // 26.08.2031 HACK Выключено из-за проблемы на XP

    //    GGS::Settings::Settings settings;
    //    settings.beginGroup("gameExecutor");
    //    settings.beginGroup("serviceInfo");
    //    settings.beginGroup(id);
    //
    //    bool ok;
    //    int overlayEnabled = settings.value("overlayEnabled", 1).toInt(&ok);
    //    if (overlayEnabled != 0 || !ok) {
    //#ifdef _DEBUG
    //      QString injectedDll = QCoreApplication::applicationDirPath() + "/OverlayX86d.dll"; 
    //#else
    //      QString injectedDll = QCoreApplication::applicationDirPath() + "/OverlayX86.dll";
    //#endif
    //
    //      url.addQueryItem("injectDll", injectedDll);
    //    }

#ifdef _DEBUG
    QString injectedDll2 = QCoreApplication::applicationDirPath() + "/GameExecutorHelperX86d.dll"; 
#else
    QString injectedDll2 = QCoreApplication::applicationDirPath() + "/GameExecutorHelperX86.dll";
#endif

    url.addQueryItem("executorHelper", injectedDll2);

  } else if (id == "300003010000000000") { // bs
    url.setScheme("exe");
    url.setPath(QString("%1/%2/client/client.exe").arg(currentInstallPath, service->areaString()));

    QUrlQuery query;
    query.addQueryItem("workingDir", QString("%1/%2/").arg(currentInstallPath, service->areaString()));
    query.addQueryItem("args", "%userId% %appKey% %token%");
    query.addQueryItem("downloadCustomFile", 
      "launcher/serverinfo_back.xml,http://files.gamenet.ru/update/bs/,1,config/lastlogin.xml,http://files.gamenet.ru/update/bs/,0");
    
    service->setGameId("71");

    GGS::Settings::Settings settings;
    settings.beginGroup("gameExecutor");
    settings.beginGroup("serviceInfo");
    settings.beginGroup(id);
    
    bool ok;
    int overlayEnabled = settings.value("overlayEnabled", 1).toInt(&ok);
    if (overlayEnabled != 0 || !ok) {
#ifdef _DEBUG
      QString injectedDll = QCoreApplication::applicationDirPath() + "/OverlayX86d.dll"; 
#else
      QString injectedDll = QCoreApplication::applicationDirPath() + "/OverlayX86.dll";
#endif
      url.addQueryItem("injectDll", injectedDll);
    }

#ifdef _DEBUG
    QString injectedDll2 = QCoreApplication::applicationDirPath() + "/GameExecutorHelperX86d.dll"; 
#else
    QString injectedDll2 = QCoreApplication::applicationDirPath() + "/GameExecutorHelperX86.dll";
#endif

    url.addQueryItem("executorHelper", injectedDll2);

  } else if (id == "300005010000000000") { // warinc
    url.setScheme("exe");
    url.setPath(QString("%1/%2/WarInc.exe").arg(currentInstallPath, service->areaString()));

    QUrlQuery query;
    query.addQueryItem("workingDir", QString("%1/%2/").arg(currentInstallPath, service->areaString()));
    query.addQueryItem("args", "-WOUpdatedOk -gna %userId% %appKey% %token%");
    url.setQuery(query);

    service->setGameId("70");

    // INFO 07.02.2014 вроде работает но надо уточнить
//#ifdef _DEBUG
//    QString injectedDll2 = QCoreApplication::applicationDirPath() + "/GameExecutorHelperX86d.dll"; 
//#else
//    QString injectedDll2 = QCoreApplication::applicationDirPath() + "/GameExecutorHelperX86.dll";
//#endif
//
//    url.addQueryItem("executorHelper", injectedDll2);

  } else if (id == "300012010000000000") { // reborn
    url.setScheme("exe");
    url.setPath(QString("%1/%2/client/Client.exe").arg(currentInstallPath, service->areaString()));
    url.addQueryItem("workingDir", QString("%1/%2/").arg(currentInstallPath, service->areaString()));
    url.addQueryItem("args", "%userId% %token%");

    query.addQueryItem("downloadCustomFile", 
      "launcher/serverinfo_back.xml,http://files.gamenet.ru/update/reborn/,1");

#ifdef _DEBUG
    QString injectedDll2 = QCoreApplication::applicationDirPath() + "/GameExecutorHelperX86d.dll"; 
#else
    QString injectedDll2 = QCoreApplication::applicationDirPath() + "/GameExecutorHelperX86.dll";
#endif

    url.addQueryItem("executorHelper", injectedDll2);

    service->setGameId("760");
  } else if (id == "300009010000000000") { // CA
    url.setScheme("exe");
    url.setPath(QString("%1/%2/engine.exe").arg(currentInstallPath, service->areaString()));
    url.addQueryItem("workingDir", QString("%1/%2/").arg(currentInstallPath, service->areaString()));
    url.addQueryItem("args", "-windowtitle \"CombatArms\" -rez Engine.REZ -rez Game -authip 31.25.225.205 -authport 10002 -pcroom 0 -Ver Ver_RU_2.1207.03 -UserID %userid% -Password %appkey%:%token%");
    url.addQueryItem("downloadCustomFile", "Profiles/player.txt,http://files.gamenet.ru/update/ca/,0");

    //#ifdef _DEBUG
    //    QString injectedDll = QCoreApplication::applicationDirPath() + "/OverlayX86d.dll"; 
    //#else
    //    QString injectedDll = QCoreApplication::applicationDirPath() + "/OverlayX86.dll";
    //#endif
    //
    //    url.addQueryItem("injectDll", injectedDll);

    url.setQuery(query);
    service->setGameId("92");

#ifdef _DEBUG
    QString injectedDll2 = QCoreApplication::applicationDirPath() + "/GameExecutorHelperX86d.dll"; 
#else
    QString injectedDll2 = QCoreApplication::applicationDirPath() + "/GameExecutorHelperX86.dll";
#endif

    url.addQueryItem("executorHelper", injectedDll2);

  } else if (id == "300004010000000000") { // RoT
    url.setScheme("exe");
    url.setPath(QString("%1/%2/bin/tyj.exe").arg(currentInstallPath, service->areaString()));
    url.addQueryItem("workingDir", QString("%1/%2/bin/").arg(currentInstallPath, service->areaString()));
    url.addQueryItem("args", "-sa UserId=%userId%%appKey% -sa Token=%token%");
    service->setGameId("72");
  } else if (id == "100009010000000000") {
    url.setScheme("exe");
    url.setPath(QString("%1/%2/sample.exe").arg(currentInstallPath, service->areaString()));
    url.addQueryItem("workingDir", QString("%1/%2/").arg(currentInstallPath, service->areaString()));

    service->setGameId("92");
  } else if (id == "100003010000000000") {
    url.setScheme("exe");
    url.setPath(QString("%1/%2/sample.exe").arg(currentInstallPath, service->areaString()));
    url.addQueryItem("workingDir", QString("%1/%2/").arg(currentInstallPath, service->areaString()));

    service->setGameId("71");
  }

  MemoryProtector_CheckFunction3(0x0A18C78A, 0x171D48F4, 0x0D860760, 0x37a14d6a);

  service->setUrl(url);
}

void ServiceLoader::setServiceSettings(ServiceSettingsBridgeProxy *value)
{
  Q_ASSERT(value);
  this->_serviceSettings = value;
}
