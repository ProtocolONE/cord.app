#include <viewmodel/SelectMw2Server/SelectMw2ServerListModel.h>

#include "mainwindow.h"

#include <Core/UI/Message>
#include <Core/Marketing.h>
#include <Core/System/FileInfo.h>

#include <Features/CASettingsFix.h>

#include <GameExecutor/Hook/RestoreResolution>
#include <GameExecutor/Hook/DisableIEDefalutProxy>
#include <GameExecutor/Hook/DisableDEP>
#include <GameExecutor/Hook/DownloadCustomFile>
#include <GameExecutor/Hook/Mw2DownloadAndCheckXmlConfig.h>
#include <GameExecutor/Hook/DisableAeroHook.h>
#include <GameExecutor/Hook/DefaultAikaSettings.h>
#include <GameExecutor/Hook/BannerDownload.h>

#include <GameExecutor/Executor/ExecutableFile>
#include <GameExecutor/Executor/WebLink>

#include <RestApi/Commands/Service/GetLicense.h>
#include <RestApi/Commands/User/GetUserMainInfo>
#include <RestApi/RequestFactory.h>

#include <Application/WindowHelper.h>

#include <Settings/settings.h>

#include <QtCore/QTranslator>
#include <QtCore/QSysInfo>
#include <QtCore/QFlags>
#include <QtGui/QBoxLayout>
#include <QtGui/QDesktopWidget>
#include <QtDeclarative/QDeclarativeProperty>

// TODO сделать место поточнее и вывод ошибки с фразой вроде "Couldn't connect to"
#define SIGNAL_CONNECT_CHECK(X) { bool result = X; Q_ASSERT_X(result, __FUNCTION__ , #X); }

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
  this->initializeUpdateSettings();

  this->_restapiManager.setUri("https://gnapi.com:8443/restapi");
  this->_restapiManager.setRequest(GGS::RestApi::RequestFactory::Http);
  this->_restapiManager.setCache(&_fakeCache);
  GGS::RestApi::RestApiManager::setCommonInstance(&this->_restapiManager);
  
  setFileVersion(GGS::Core::System::FileInfo::version(QCoreApplication::applicationFilePath())); 
   
  this->setWindowTitle("qGNA " + this->_fileVersion);

  setWindowFlags(Qt::Window 
    | Qt::FramelessWindowHint 
    | Qt::WindowMinimizeButtonHint 
    | Qt::WindowMaximizeButtonHint 
    | Qt::WindowSystemMenuHint); //Этот код уберет все внешние элементы формы       

  translatorsParse();
  
  GGS::Settings::Settings settings;
  selectLanguage(settings.value("qGNA/language").toString());                                                             
  
  qRegisterMetaType<TrayWindow::MenuLabel>("MenuLabel");
  qRegisterMetaType<TrayWindow::ContextMenuType>("ContextMenuType"); 
   
  qmlRegisterType<GGS::UpdateSystem::UpdateManagerViewModel>("qGNA.Library", 1, 0, "UpdateManagerViewModel");             
  qmlRegisterType<SelectMw2ServerListModel>("qGNA.Library", 1, 0, "SelectMw2ServerListModel"); 

  qmlRegisterUncreatableType<GGS::Downloader::DownloadResultsWrapper>("qGNA.Library", 1, 0,  "DownloadResults", "");
  qmlRegisterUncreatableType<GGS::UpdateSystem::UpdateInfoGetterResultsWrapper>("qGNA.Library", 1, 0,  "UpdateInfoGetterResults", "");

  qmlRegisterType<GGS::Core::UI::Message>("qGNA.Library", 1, 0, "Message");     
  qmlRegisterType<TrayWindow>("qGNA.Library", 1, 0,  "TrayWindow");   

  this->_trayWindow = new TrayWindow();
  this->_trayWindow->setMenuType(TrayWindow::OnlyQuitMenu);

  QSettings midSettings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);
  QString mid = midSettings.value("MID", "").toString();
  this->_marketingTargetFeatures.init("qGNA", mid);
  
  //next 2 lines QGNA-60
  nQMLContainer = new MQDeclarativeView();
  connect(nQMLContainer, SIGNAL(leftMouseClick(int, int)), this, SIGNAL(leftMouseClick(int, int)));

  this->loadPlugin("QmlExtensionX86");

  nQMLContainer->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

  if (QGLFormat::hasOpenGL() && QGLFormat::openGLVersionFlags().testFlag(QGLFormat::OpenGL_Version_2_0) && QCoreApplication::arguments().contains("-opengl"))
  {
    QGLFormat format; 
    format.setVersion(2,0);
    QGLFormat::setDefaultFormat(format);
    QGLWidget *glWidget = new QGLWidget(format);
    nQMLContainer->setViewport(glWidget);
    this->setFixedSize(800,400); 
    qDebug() << "Render: Use OpenGL 2.0";
  } else {
    setAttribute(Qt::WA_TranslucentBackground); //Эти две строчки позволят форме становиться прозрачной 
    setStyleSheet("background:transparent;");
    this->setFixedSize(808,408); 
    qDebug() << "Render: Use rester engine.";
  }
  
  licenseModel = new LicenseViewModel(this);
  settingsViewModel = new SettingsViewModel(this);
  this->_selectMw2ServerViewModel = new SelectMw2ServerViewModel(this);

  this->_enterNickViewModel = new EnterNickNameViewModel(this);
    
  SIGNAL_CONNECT_CHECK(QObject::connect(settingsViewModel, SIGNAL(incomingPortChanged()), this, SLOT(settingsIncomingPortChangedSlot())));
  SIGNAL_CONNECT_CHECK(QObject::connect(settingsViewModel, SIGNAL(numConnectionsChanged()), this, SLOT(settingsNumConnectionsChangedSlot())));
  SIGNAL_CONNECT_CHECK(QObject::connect(settingsViewModel, SIGNAL(downloadSpeedChanged()), this, SLOT(settingsDownloadSpeedChangedSlot())));
  SIGNAL_CONNECT_CHECK(QObject::connect(settingsViewModel, SIGNAL(uploadSpeedChanged()), this, SLOT(settingsUploadSpeedChangedSlot())));

  SIGNAL_CONNECT_CHECK(QObject::connect(licenseModel, SIGNAL(result()), this, SLOT(licenseOkPressed())));

  SIGNAL_CONNECT_CHECK(QObject::connect(&this->_restapiManager, SIGNAL(genericError(GGS::RestApi::CommandBase::Error, QString)), 
    this, SLOT(restApiGenericError(GGS::RestApi::CommandBase::Error, QString))));

  messageAdapter = new QmlMessageAdapter(this);

  this->_gameSettingsViewModel = new GameSettingsViewModel(this);
  this->_gameSettingsViewModel->setGameDownloader(&this->_gameDownloaderBuilder.gameDownloader());
  
  nQMLContainer->rootContext()->setContextProperty("mainWindow", this);
  nQMLContainer->rootContext()->setContextProperty("installPath", "file:///" + QCoreApplication::applicationDirPath() + "/");
  nQMLContainer->rootContext()->setContextProperty("licenseModel", licenseModel);
  nQMLContainer->rootContext()->setContextProperty("settingsViewModel", settingsViewModel);
  nQMLContainer->rootContext()->setContextProperty("selectMw2ServerModel", this->_selectMw2ServerViewModel);
  nQMLContainer->rootContext()->setContextProperty("messageBox", messageAdapter);
  nQMLContainer->rootContext()->setContextProperty("trayMenu", this->_trayWindow);   
  nQMLContainer->rootContext()->setContextProperty("enterNickNameViewModel", this->_enterNickViewModel);

  nQMLContainer->rootContext()->setContextProperty("gameSettingsModel", this->_gameSettingsViewModel);

  nQMLContainer->setSource(QUrl("qrc:/qGNA_Main.qml"));

  nQMLContainer->setAlignment(Qt::AlignCenter);                                                                               
  nQMLContainer->setResizeMode(QDeclarativeView::SizeRootObjectToView);                                                         

  QObject *item = nQMLContainer->rootObject();

  SIGNAL_CONNECT_CHECK(QObject::connect(item, SIGNAL(onWindowPressed(int,int)), this, SLOT(onSystemBarPressed(int,int))));
  SIGNAL_CONNECT_CHECK(QObject::connect(item, SIGNAL(onWindowReleased(int,int)), this, SLOT(onSystemBarReleased(int,int))));
  SIGNAL_CONNECT_CHECK(QObject::connect(item, SIGNAL(onWindowPositionChanged(int,int)), this, SLOT(onSystemBarPositionChanged(int,int))));
  SIGNAL_CONNECT_CHECK(QObject::connect(item, SIGNAL(onWindowClose()), this, SLOT(onWindowClose())));

  setCentralWidget(nQMLContainer);

  nQMLContainer->setAlignment(Qt::AlignCenter);
  nQMLContainer->setResizeMode(QDeclarativeView::SizeRootObjectToView); 
  
  this->hide();

  this->setMediumAvatarUrl("file:///" + QCoreApplication::applicationDirPath() + "/" + "images/avatar.png");                  
  _fileVersion = QString();

  this->prepairGameDownloader();

  Message::setAdapter(messageAdapter);

  SIGNAL_CONNECT_CHECK(QObject::connect(this->_trayWindow, SIGNAL(activate()), this, SLOT(activateWindow())));
  SIGNAL_CONNECT_CHECK(QObject::connect(this->_trayWindow, SIGNAL(menuClick(int)), this, SLOT(menuItemTrigger(int))));

  SIGNAL_CONNECT_CHECK(connect(&this->_gameExecutorService, SIGNAL(started(const GGS::Core::Service &)),
    this, SLOT(onServiceStarted(const GGS::Core::Service &))));

  SIGNAL_CONNECT_CHECK(connect(&this->_gameExecutorService, SIGNAL(finished(const GGS::Core::Service &, GGS::GameExecutor::FinishState)),
    this, SLOT(onServiceFinished(const GGS::Core::Service &, GGS::GameExecutor::FinishState))));

  SIGNAL_CONNECT_CHECK(connect(&this->_gameExecutorService, SIGNAL(started(const GGS::Core::Service &)),
    &this->_gameExecutorServiceInfoCounter, SLOT(started(const GGS::Core::Service &))));

  SIGNAL_CONNECT_CHECK(connect(&this->_gameExecutorService, SIGNAL(finished(const GGS::Core::Service &, GGS::GameExecutor::FinishState)),
    &this->_gameExecutorServiceInfoCounter, SLOT(finished(const GGS::Core::Service &, GGS::GameExecutor::FinishState))));

  this->initializeStopDownloadServiceOnExecuteGameFeature();
}

MainWindow::~MainWindow()
{
  delete this->_trayWindow;
  this->_trayWindow = 0;
}

void MainWindow::setTechName(QString& techName){
  if (this->_techName != techName){
    this->_techName = techName;
    emit this->techNameChanged();
    emit this->techNameValueChanged(this->_techName);
  }
}

void MainWindow::setNickName(QString& nickName) {
  if(this->_nickName != nickName) {
    this->_nickName = nickName;
    emit this->nickNameChanged();
    emit this->nickNameValueChanged(this->_nickName);
  }
}

void MainWindow::setMediumAvatarUrl(const QString& mediumAvatarUrl) {
  if(this->_mediumAvatarUrl != mediumAvatarUrl) {
    this->_mediumAvatarUrl = mediumAvatarUrl;
    emit this->mediumAvatarUrlChanged(); 
  }
}

void MainWindow::activateWindow()
{
  // Это нам покажет окно
  this->setFocusPolicy(Qt::StrongFocus);
  this->setWindowState(Qt::WindowActive); 
  this->showNormal();

  // Эта функция активирует окно и поднмиает его повех всех окон
  GGS::Application::WindowHelper::activate(this->winId());
}

void MainWindow::menuItemTrigger(int index)
{
  if (index == TrayWindow::QuitMenu){
    emit this->closeMainWindow();  
  }
}

bool MainWindow::isDownloading(QString serviceId)
{
  GGS::Core::Service *service = this->getService(serviceId);
  if (!service)
    return false;

  return this->_gameDownloaderBuilder.gameDownloader().isInProgress(service);
}

QString MainWindow::getDate( quint64 unixTimeStamp )
{
  QDateTime timestamp;
  timestamp.setTime_t(unixTimeStamp);
  return timestamp.toString("dd.MM.yyyy");
}

void MainWindow::translatorsParse()
{
  QDir dir(QCoreApplication::applicationDirPath() + "/Languages/");
  dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
  dir.setSorting(QDir::Size | QDir::Reversed);

  QFileInfoList list = dir.entryInfoList();

  for (int i = 0; i < list.size(); ++i) { // TODO Немного хардкод, обсудить это
    QFileInfo fileInfo = list.at(i);

    QString fileName = fileInfo.fileName();
    fileName.remove("qgna_");
    fileName.remove(".qm");

    if (fileName.size() == 2){  
      translators[fileName] = new QTranslator(this);
      translators[fileName]->load(fileInfo.fileName(), QCoreApplication::applicationDirPath() + "/Languages/"); // TODO да да, знаю, сделаю красиво позже.
    }
  } 
}

QString MainWindow::language()
{
  GGS::Settings::Settings settings;
  return settings.value("qGNA/language").toString(); 
}

void MainWindow::saveLanguage(const QString& language)
{
  GGS::Settings::Settings settings;
  settings.setValue("qGNA/language", language);
}

void MainWindow::selectLanguage(const QString& language) 
{ 
  if (this->translators[language])
    QApplication::installTranslator(this->translators[language]);  

  emit languageChanged();
}

void MainWindow::onSystemBarPressed(int MouseX, int MouseY)
{
  mLastMousePosition = QPoint(MouseX,MouseY);
}

void MainWindow::onSystemBarReleased(int MouseX, int MouseY)  
{
}

void MainWindow::onSystemBarPositionChanged(int MouseX, int MouseY) 
{
  move( pos() + (QPoint(MouseX,MouseY) - mLastMousePosition));   
}

void MainWindow::onWindowClose()
{
  this->repaint();
  this->hide();
  this->_gameDownloaderBuilder.gameDownloader().shutdown(); 
}

void MainWindow::authSuccessSlot(const QString& userId, const QString& appKey, const QString& cookie) 
{
  qDebug() << "[DEBUG] userId " << userId;// не хочу палить авторизацию в логе << " appKey " << appKey;

  this->_trayWindow->setMenuType(TrayWindow::FullMenu);

  GGS::RestApi::GameNetCredential credential;
  credential.setUserId(userId);
  credential.setAppKey(appKey);
  credential.setCookie(cookie);

  this->_credential = credential;
  this->_restapiManager.setCridential(credential);
  using GGS::RestApi::Commands::User::GetUserMainInfo;
  GetUserMainInfo *getUserMainInfo = new GetUserMainInfo(this);
  SIGNAL_CONNECT_CHECK(QObject::connect(getUserMainInfo, SIGNAL(result(GGS::RestApi::CommandBase::CommandResults)), 
      this, SLOT(userMainInfoResult(GGS::RestApi::CommandBase::CommandResults)))); 
  getUserMainInfo->execute();

  this->_jabber.authSuccess(userId, appKey);
}

void MainWindow::userMainInfoResult(GGS::RestApi::CommandBase::CommandResults code)
{
  GGS::RestApi::Commands::User::GetUserMainInfo *getUserMainInfo = 
    qobject_cast<GGS::RestApi::Commands::User::GetUserMainInfo *>(QObject::sender());

  if (!getUserMainInfo)
    return;

  getUserMainInfo->deleteLater();
  
  if (code != GGS::RestApi::CommandBase::NoError) {
      qDebug() << "GetUserMainInfo execute command error " << code;
      return;
  }

  this->setNickName(getUserMainInfo->response()->nickname());
  this->setTechName(getUserMainInfo->response()->nametech());
  this->setMediumAvatarUrl(getUserMainInfo->response()->mediumAvatarUrl());
}

void MainWindow::restartApplication(bool shouldStartWithSameArguments)
{
  emit this->secondInstanceExecuteRequest();
  QString dir = QCoreApplication::applicationDirPath();
  QString exe = QCoreApplication::applicationFilePath();

  QString commandLineArgs("");
  if (shouldStartWithSameArguments) {
    QStringList args = QCoreApplication::arguments();
    args.removeFirst();

    if (!this->isVisible())
      args << "/minimized";

    if (args.size() > 0) {
      commandLineArgs = args.join("\" \"");
      commandLineArgs.prepend(L'"');
      commandLineArgs.append(L'"');
    }

  } else {
    if (!this->isVisible())
      commandLineArgs = "/minimized";

  }

  QString open("open");

  SHELLEXECUTEINFOW shex;
  ZeroMemory(&shex, sizeof(shex));

  shex.cbSize			  = sizeof( SHELLEXECUTEINFO ); 
  shex.fMask			  = 0; 
  shex.hwnd			    = 0;
  shex.lpVerb			  = reinterpret_cast<const WCHAR*>(open.utf16()); 
  shex.lpFile			  = reinterpret_cast<const WCHAR*>(exe.utf16()); 
  shex.lpParameters	= reinterpret_cast<const WCHAR*>(commandLineArgs.utf16()); 
  shex.lpDirectory	= reinterpret_cast<const WCHAR*>(dir.utf16()); 
  shex.nShow			  = SW_NORMAL; 
  
  if (::ShellExecuteExW(&shex)) {
    QCoreApplication::exit();
    return;
  }

  qWarning() << "Can't restart qGNA";
}

void MainWindow::openExternalBrowser(const QString& url)
{
  QString authUrl;
  if(this->_credential.appKey().isEmpty())
  {
    authUrl = url;
  } else {
    //"https://gnlogin.ru/?auth={0}&rp={1}";
    authUrl = "https://gnlogin.ru/?auth=";
    authUrl.append(this->_credential.cookie());
    authUrl.append("&rp=");
    authUrl.append(QUrl::toPercentEncoding(url));
  }

  authUrl.append('\0');
  ShellExecuteW(0, 0, reinterpret_cast<const WCHAR*>(authUrl.utf16()), 0, 0, SW_NORMAL);
}

void MainWindow::logout()
{
  this->_trayWindow->setMenuType(TrayWindow::OnlyQuitMenu);

  this->_credential.setAppKey("");
  this->_credential.setUserId("");
  this->_credential.setCookie("");

  this->setTechName(QString(""));
  this->setNickName(QString(""));

  this->_restapiManager.setCridential(this->_credential);
  this->_jabber.logout();
}

void MainWindow::initServices()
{
  // QGNA-183 выпиливание всех следов RoT в qGNA
  this->_gameSettingsViewModel->removeShortCutByName("RageofTitans");

  this->initService("300002010000000000", "http://fs0.gamenet.ru/update/aika/", "Aika2");
  this->initService("300003010000000000", "http://fs0.gamenet.ru/update/bs/", "BS");
  this->initService("300005010000000000", "http://fs0.gamenet.ru/update/warinc/", "FireStorm");
  this->initService("300006010000000000", "http://fs0.gamenet.ru/update/mw2/", "MW2");
  this->initService("300009010000000000", "http://fs0.gamenet.ru/update/ca/", "CombatArms");
  this->initGAService("300007010000000000");

  this->_gameDownloaderBuilder.gameDownloader().registerHook("300005010000000000", 0, 10, &this->_installDependencyHook);
  this->_gameDownloaderBuilder.gameDownloader().registerHook("300006010000000000", 0, 10, &this->_installDependencyHook);
  
  this->_gameSettingsViewModel->setServiceList(&this->_serviceMap);
}

void MainWindow::initService(const QString& id, const QString& torrentUrl, const QString& name)
{
  using namespace GGS::Core; 
  Service *service = new Service();
  service->setArea(Service::Live);
  service->setIsDownloadable(true);
  service->setName(name);

  QString root = QCoreApplication::applicationDirPath();

  QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);
  
  QString defaultDownloadPath = settings.value("DownloadPath", "").toString();
  
  if (defaultDownloadPath.isEmpty())
    defaultDownloadPath = root + "/Downloads/";

  bool hasDownloadPath = (id == "300002010000000000" || id == "300009010000000000");
  service->setHashDownloadPath(hasDownloadPath);
  
  settings.beginGroup(id);
  QString currentDownloadPath = settings.value("DownloadPath").toString();
  if (currentDownloadPath.isEmpty())
    currentDownloadPath = QString("%1/%2/").arg(defaultDownloadPath, name);

  QString currentInstallPath = settings.value("InstallPath").toString();
  
  if (currentInstallPath.isEmpty()) {
    currentInstallPath = QString("%1/Games/%2/").arg(root, name);
    service->setIsDefaultInstallPath(false);
  } else {
    service->setIsDefaultInstallPath(true);
  }

  settings.endGroup();

  service->setDownloadPath(hasDownloadPath ? currentDownloadPath : currentInstallPath);
  service->setInstallPath(currentInstallPath);
  service->setTorrentFilePath(hasDownloadPath ? currentDownloadPath : currentInstallPath);

  if (id == "300002010000000000" || id == "300009010000000000")
    service->setExtractorType("D9E40EE5-806F-4B7D-8D5C-B6A4BF0110E9");
  else
    service->setExtractorType("3A3AC78E-0332-45F4-A466-89C2B8E8BB9C");

  service->setId(id);
  service->setTorrentUrl(torrentUrl);

  if (id != "300007010000000000") {
    this->_gameDownloaderBuilder.gameDownloader().registerHook(id, 100, 0, &this->_oldGameClientMigrate);
  }

  this->_serviceMap[id] = service;
  
  if (id == "300003010000000000") {
    GGS::GameExecutor::Hook::DisableDEP *dep = new GGS::GameExecutor::Hook::DisableDEP(service);
    this->_gameExecutorService.addHook(*service, dep, 0);
  }

  if (id == "300002010000000000") {
    GGS::GameExecutor::Hook::DisableIEDefalutProxy *ieproxy = new GGS::GameExecutor::Hook::DisableIEDefalutProxy(service);
    this->_gameExecutorService.addHook(*service, ieproxy, 0);

    GGS::GameExecutor::Hook::RestoreResolution *resolution = new GGS::GameExecutor::Hook::RestoreResolution(service);
    this->_gameExecutorService.addHook(*service, resolution, 0);
    this->_gameExecutorService.addHook(*service, new GGS::GameExecutor::Hook::DefaultAikaSettings(service), 0);
  }

  if (id == "300003010000000000") {
    GGS::GameExecutor::Hook::DownloadCustomFile *hook = new GGS::GameExecutor::Hook::DownloadCustomFile(service);
    this->_gameExecutorService.addHook(*service, hook, 100);
  }
  
  if (id == "300006010000000000") {
     this->_gameExecutorService.addHook(*service, this->_selectMw2ServerViewModel);
     this->_gameExecutorService.addHook(*service, new GGS::GameExecutor::Hook::Mw2DownloadAndCheckXmlConfig(service), 100);
  }

  if (id == "300009010000000000") {
    Features::CASettingsFix *hook = new Features::CASettingsFix(service);
    hook->setResolution(QApplication::desktop()->screenGeometry(QApplication::desktop()->primaryScreen()));
    SIGNAL_CONNECT_CHECK(QObject::connect(&this->_gameDownloaderBuilder.gameDownloader(), SIGNAL(serviceInstalled(const GGS::Core::Service *)), 
      hook, SLOT(install(const GGS::Core::Service *))));
    this->_gameExecutorService.addHook(*service, hook);

    this->_gameExecutorService.addHook(*service, new GGS::GameExecutor::Hook::DisableIEDefalutProxy(service), 0);
    this->_gameExecutorService.addHook(*service, new GGS::GameExecutor::Hook::BannerDownload(service), 0);
      
    QSysInfo::WinVersion version = QSysInfo::windowsVersion();
    if (version == QSysInfo::WV_VISTA || version == QSysInfo::WV_WINDOWS7)
      this->_gameExecutorService.addHook(*service, new GGS::GameExecutor::Hook::DisableAeroHook(service));
  }

  this->setExecuteUrl(id, currentInstallPath);
  
  if (id == "300005010000000000") {
    service->setExternalDependencyList("dxwebsetup.exe,/Q");
  } else if (id == "300006010000000000") {
    service->setExternalDependencyList("vcredist_x86.exe,/Q");
  }

  if (id == "300005010000000000") 
    this->_gameExecutorService.addHook(*service, this->_enterNickViewModel, 0);  
    
  this->migrateInstallDate(id);
}

void MainWindow::initGAService(const QString& id)
{
  using namespace GGS::Core;
  Service *service = new Service();
  service->setArea(Service::Live);
  service->setIsDownloadable(false);
  service->setName(id);
  service->setId(id);

  QUrl url("http://www.playga.ru/");
  service->setGameId("83");
  service->setUrl(url);

  this->_serviceMap[id] = service;
}

GGS::Core::Service* MainWindow::getService(const QString& id)
{
  if (!this->_serviceMap.contains(id))
    return 0;

  return this->_serviceMap[id];
}

void MainWindow::prepairGameDownloader()
{
  QString root = QCoreApplication::applicationDirPath();
  this->initServices();

  this->_gameDownloaderBuilder.torrentWrapper().setListeningPort(11789);
  QString torrentConfigPath = root;
  torrentConfigPath.append("/torrents");
  this->_gameDownloaderBuilder.torrentWrapper().setTorrentConfigDirectoryPath(torrentConfigPath);
  this->_gameDownloaderBuilder.torrentWrapper().initEngine();

  this->_gameDownloaderBuilder.build();
  this->_gameDownloaderBuilder.gameDownloader().setTimeoutForResume(120);

  SIGNAL_CONNECT_CHECK(QObject::connect(&this->_gameDownloaderBuilder.gameDownloader(), SIGNAL(progressChanged(QString, qint8)), 
      this, SLOT(progressChanged(QString, qint8))));
  SIGNAL_CONNECT_CHECK(QObject::connect(&this->_gameDownloaderBuilder.gameDownloader(), SIGNAL(progressDownloadChanged(QString, qint8, GGS::Libtorrent::EventArgs::ProgressEventArgs)), 
    this, SLOT(progressDownloadChanged(QString, qint8, GGS::Libtorrent::EventArgs::ProgressEventArgs))));
  SIGNAL_CONNECT_CHECK(QObject::connect(&this->_gameDownloaderBuilder.gameDownloader(), SIGNAL(progressExtractionChanged(QString, qint8, qint64, qint64)), 
    this, SLOT(progressExtractionChanged(QString, qint8, qint64, qint64))));


  SIGNAL_CONNECT_CHECK(QObject::connect(&this->_gameDownloaderBuilder.gameDownloader(), SIGNAL(started(const GGS::Core::Service *, GGS::GameDownloader::StartType)), 
    this, SLOT(gameDownloaderStarted(const GGS::Core::Service *, GGS::GameDownloader::StartType))));
  SIGNAL_CONNECT_CHECK(QObject::connect(&this->_gameDownloaderBuilder.gameDownloader(), SIGNAL(finished(const GGS::Core::Service *)), 
    this, SLOT(gameDownloaderFinished(const GGS::Core::Service *))));
  SIGNAL_CONNECT_CHECK(QObject::connect(&this->_gameDownloaderBuilder.gameDownloader(), SIGNAL(stopped(const GGS::Core::Service *)), 
    this, SLOT(gameDownloaderStopped(const GGS::Core::Service *))));
  SIGNAL_CONNECT_CHECK(QObject::connect(&this->_gameDownloaderBuilder.gameDownloader(), SIGNAL(stopping(const GGS::Core::Service *)), 
    this, SLOT(gameDownloaderStopping(const GGS::Core::Service *))));
  SIGNAL_CONNECT_CHECK(QObject::connect(&this->_gameDownloaderBuilder.gameDownloader(), SIGNAL(failed(const GGS::Core::Service *)), 
    this, SLOT(gameDownloaderFailed(const GGS::Core::Service *))));


  SIGNAL_CONNECT_CHECK(QObject::connect(&this->_gameDownloaderBuilder.gameDownloader(), SIGNAL(shutdownCompleted()),
    this, SLOT(shutdownCompleted())));

  SIGNAL_CONNECT_CHECK(QObject::connect(&this->_gameDownloaderBuilder.torrentWrapper(), SIGNAL(listeningPortChanged(unsigned short)),
    this, SLOT(torrentListenPortChangedSlot(unsigned short))));

  GGS::GameExecutor::Executor::ExecutableFile *gameExecutorByLauncher = new GGS::GameExecutor::Executor::ExecutableFile(this);
  gameExecutorByLauncher->setWorkingDirectory(QCoreApplication::applicationDirPath());
  this->_gameExecutorService.registerExecutor(gameExecutorByLauncher);

  GGS::GameExecutor::Executor::WebLink *webLinkExecutor = new GGS::GameExecutor::Executor::WebLink(this);
  this->_gameExecutorService.registerExecutor(webLinkExecutor);

  SIGNAL_CONNECT_CHECK(QObject::connect(&this->_gameDownloaderBuilder.gameDownloader(), SIGNAL(started(const GGS::Core::Service *, GGS::GameDownloader::StartType)), 
    &this->_rembrGameFeature, SLOT(started(const GGS::Core::Service *))));
  SIGNAL_CONNECT_CHECK(QObject::connect(&this->_gameDownloaderBuilder.gameDownloader(), SIGNAL(finished(const GGS::Core::Service *)), 
    &this->_rembrGameFeature, SLOT(finished(const GGS::Core::Service *))));
  SIGNAL_CONNECT_CHECK(QObject::connect(&this->_rembrGameFeature, SIGNAL(startGameRequest(QString)), this, SLOT(downloadButtonStart(QString))));

  SIGNAL_CONNECT_CHECK(QObject::connect(&this->_gameDownloaderBuilder.gameDownloader(), SIGNAL(statusMessageChanged(const GGS::Core::Service *, const QString&)), 
    this, SLOT(gameDownloaderStatusMessageChanged(const GGS::Core::Service *, const QString&))));

  SIGNAL_CONNECT_CHECK(QObject::connect(&this->_gameDownloaderBuilder.gameDownloader(), SIGNAL(serviceInstalled(const GGS::Core::Service *)), 
    this, SLOT(gameDownloaderServiceInstalled(const GGS::Core::Service *))));

  SIGNAL_CONNECT_CHECK(QObject::connect(&this->_gameDownloaderBuilder.gameDownloader(), SIGNAL(serviceUpdated(const GGS::Core::Service *)), 
    this, SLOT(gameDownloaderServiceUpdated(const GGS::Core::Service *))));
}

void MainWindow::progressChanged(QString serviceId, qint8 progress)
{
  emit this->progressbarChange(serviceId, progress, -1, -1); 
}

void MainWindow::progressDownloadChanged(QString serviceId, qint8 progress, GGS::Libtorrent::EventArgs::ProgressEventArgs args)
{
  emit this->progressbarChange(serviceId, progress, args.totalWantedDone(), args.totalWanted());
}

void MainWindow::progressExtractionChanged(QString serviceId, qint8 progress, qint64 current, qint64 total)
{
  emit this->progressbarExtractionChange(serviceId, progress, current, total);
}

void MainWindow::gameDownloaderStarted(const GGS::Core::Service *service, GGS::GameDownloader::StartType startType)
{
  emit this->downloaderStarted(service->id());
}

void MainWindow::gameDownloaderFinished(const GGS::Core::Service *service) 
{
  QString id = service->id();
  emit this->downloaderFinished(id);
  emit this->progressbarChange(id, 100, -1, -1);     

  if (!this->isWindowVisible()) {
    emit this->selectService(id);
    return;
  }
  
  if (this->_restapiManager.credential().userId().isEmpty()) {
    emit this->authBeforeStartGameRequest(id);
    return;
  }

  if (id == "300002010000000000" 
    || id == "300003010000000000" 
    || id == "300005010000000000"  
    || id == "300006010000000000"
    || id == "300009010000000000") {
      this->_gameExecutorService.execute(*service);
  }
}

void MainWindow::gameDownloaderStopped(const GGS::Core::Service *service)
{
  emit this->downloaderStopped(service->id());
}

void MainWindow::gameDownloaderStopping(const GGS::Core::Service *service)
{
  emit this->downloaderStopping(service->id());
}

void MainWindow::updateFinishedSlot() {
  emit this->updateFinished();
}

void MainWindow::gameDownloaderFailed(const GGS::Core::Service *service)
{
  emit this->downloaderFailed(service->id());
}

void MainWindow::shutdownCompleted()
{
  QCoreApplication::quit();
}

void MainWindow::downloadButtonStart(QString serviceId) 
{
  GGS::Core::Service *service = this->getService(serviceId);
  if (!service)
    return;

  emit this->downloadButtonStartSignal(serviceId); 

  if (service->isDownloadable())
    this->checkLicense(serviceId);
  else
    this->_gameExecutorService.execute(*service);
}

void MainWindow::downloadButtonPause(QString serviceId)
{
  GGS::Core::Service *service = this->getService(serviceId);
  if (!service)
    return;

  if (service->isDownloadable())
    this->_gameDownloaderBuilder.gameDownloader().stop(service);
  else
    this->_gameExecutorService.execute(*service);
}

void MainWindow::initializeUpdateSettings()
{
  this->_updateArea = QString("live");
  QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\GGS\\QGNA", QSettings::NativeFormat);
  bool ok = false;
  int area = settings.value("Repository", 0).toInt(&ok);
  if (!ok)
    area = 0;

  switch(area)
  {
  case 1:
    this->_updateArea = QString("pts");
    break;
  case 2:
    this->_updateArea = QString("tst");
    break;
  default:
    this->_updateArea = QString("live");  
  }

#ifdef QGNA_NO_UPDATE
  this->_installUpdateGnaPath = QString("tst");
#else
  this->_installUpdateGnaPath = QString("");
#endif

  _updateUrl = QString("http://fs0.gamenet.ru/update/qgna/%1/").arg(_updateArea);
  QString updateCrc = QString("%1update.crc.7z").arg(this->_updateUrl);
  this->_checkUpdateHelper.setUpdateUrl(updateCrc);

  SIGNAL_CONNECT_CHECK(QObject::connect(&this->_checkUpdateHelper, SIGNAL(finished(GGS::UpdateSystem::CheckUpdateHelper::Results)),
    this, SLOT(checkUpdateHelperFinished(GGS::UpdateSystem::CheckUpdateHelper::Results))));
}

void MainWindow::licenseOkPressed()
{
  QString serviceId = this->licenseModel->serviceId();
  
  bool licenseAcceppted = this->licenseModel->licenseAccepted(); 
  if (!licenseAcceppted)
    return;

  GGS::Core::Service *service = this->getService(serviceId);  

  // UNDONE: перенести код в GameSettingsViewModel
  bool shurtCutInDesktop = this->licenseModel->shurtCutInDesktop();
  if (shurtCutInDesktop)
    this->_gameSettingsViewModel->createShortcutOnDesktop(service);

  bool shurtCutInStart = this->licenseModel->shurtCutInStart();  
  if (shurtCutInStart)
    this->_gameSettingsViewModel->createShortcutInMainMenu(service);

  // UNDONE: тут поменять пути до инсталяторов
  QString hash = this->licenseModel->hash();
  QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);         
  settings.beginGroup(serviceId);
  settings.setValue("LicenseHash", hash);

  QString pathToInstall = licenseModel->pathToInstall();

  service->setInstallPath(pathToInstall);
  if (!service->hashDownloadPath()) {
    service->setDownloadPath(pathToInstall);
    service->setTorrentFilePath(pathToInstall); 
  }

  this->licenseModel->closeLicense();
  this->startGame(serviceId);
}

void MainWindow::checkLicense(const QString& serviceId)
{
  QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);
  settings.beginGroup(serviceId);
  QString hash = settings.value("LicenseHash", "").toString();

  if (hash.isEmpty()) {
    GGS::RestApi::Commands::Service::GetLicense *cmd = new GGS::RestApi::Commands::Service::GetLicense(serviceId);
    connect(cmd, SIGNAL(result(GGS::RestApi::CommandBase::CommandResults)), this, SLOT(licenseResult(GGS::RestApi::CommandBase::CommandResults)));

    cmd->setHash(hash);
    cmd->execute();
    return;
  }

  this->startGame(serviceId);
}

void MainWindow::licenseResult(GGS::RestApi::CommandBase::CommandResults result)
{
  GGS::RestApi::Commands::Service::GetLicense *cmd = qobject_cast<GGS::RestApi::Commands::Service::GetLicense *>(QObject::sender());
  if (!cmd)
    return;
   
  cmd->deleteLater();

  if (result != GGS::RestApi::CommandBase::NoError) {
    // UNDONE: set error state or retry
    return;
  }

  QString serviceId = cmd->serviceId();
  if (!cmd->isLicenseNew()) {
    this->startGame(serviceId);
    return;
  }

  this->activateWindow();
  GGS::Core::Service *service = this->getService(serviceId);
  QString hash = cmd->responseHash();
  QString license = cmd->licenseText();

  this->licenseModel->setPathToInstall(service->installPath());
  this->licenseModel->setLicense(license);
  this->licenseModel->openLicense(serviceId, hash);
}

void MainWindow::startGame(const QString& serviceId)
{
  GGS::Core::Service *service = this->getService(serviceId);
  if (!service)
    return;

  if (service->isDownloadable()) {
    QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);
    settings.beginGroup(serviceId);
    settings.setValue("DownloadPath", service->downloadPath());
    settings.setValue("InstallPath", service->installPath());
    service->setIsDefaultInstallPath(true);
    settings.endGroup();
    this->setExecuteUrl(serviceId, service->installPath());
    this->_gameDownloaderBuilder.gameDownloader().start(service, GGS::GameDownloader::Normal);
  } else {
    bool isAuthed = !this->_restapiManager.credential().userId().isEmpty();
    if (!isAuthed) {
      emit this->authBeforeStartGameRequest(serviceId);
      return;
    }

    this->_gameExecutorService.execute(*service);
  }
}

void MainWindow::setExecuteUrl(const QString& id, QString currentInstallPath)
{
  // HACK тут везде предполагается что сервис в зоне live. Если это не так надо все пути делать правильно вычисляемыми.
  GGS::Core::Service *service = this->getService(id);
  if (id == "300002010000000000") {
    QUrl url;
    url.setScheme("exe");
    url.setPath(QString("%1/live/aikaru.exe").arg(currentInstallPath));
    url.addQueryItem("workingDir", QString("%1/live/").arg(currentInstallPath));
    url.addQueryItem("args", "%login% %token% 300002010000000000 login");
    service->setGameId("631");
    service->setUrl(url);
  } else if (id == "300003010000000000") {
    QUrl url;
    url.setScheme("exe");
    url.setPath(QString("%1/live/client/client.exe").arg(currentInstallPath));
    url.addQueryItem("workingDir", QString("%1/live/").arg(currentInstallPath));
    url.addQueryItem("args", "%userId% %appKey% %token%");
    url.addQueryItem("downloadCustomFile", 
      "launcher/serverinfo_back.xml,http://files.gamenet.ru/update/bs/,1,config/lastlogin.xml,http://files.gamenet.ru/update/bs/,0");
    service->setGameId("71");
    service->setUrl(url);
  } else if (id == "300005010000000000") {
    QUrl url;
    url.setScheme("exe");
    url.setPath(QString("%1/live/WarInc.exe").arg(currentInstallPath));
    url.addQueryItem("workingDir", QString("%1/live/").arg(currentInstallPath));
    url.addQueryItem("args", "-WOUpdatedOk -gna %userId% %appKey% %token%");
    service->setGameId("70");
    service->setUrl(url);
  } else if (id == "300006010000000000") {
    QUrl url;
    url.setScheme("exe");
    url.setPath(QString("%1/live/mw2_bin/mw2.exe").arg(currentInstallPath));
    url.addQueryItem("workingDir", QString("%1/live/").arg(currentInstallPath));
    url.addQueryItem("args", "%serverinfo% %userId% %token% %appKey%");
    url.addQueryItem("downloadCustomFile", "mw2_bin/cfg_engine.xml,http://files.gamenet.ru/update/mw2/,0");
    
    service->setGameId("84");
    service->setUrl(url);
  } else if (id == "300009010000000000") {
    QUrl url;
    url.setScheme("exe");
    url.setPath(QString("%1/live/engine.exe").arg(currentInstallPath));
    url.addQueryItem("workingDir", QString("%1/live/").arg(currentInstallPath));
    url.addQueryItem("args", "-windowtitle \"CombatArms\" -rez Engine.REZ -rez Game -authip 31.25.225.205 -authport 10002 -pcroom 0 -Ver Ver_RU_2.1207.03 -UserID %userid% -Password %appkey%:%token%");

#ifdef _DEBUG
    QString injectedDll = QCoreApplication::applicationDirPath() + "/OverlayX86d.dll"; 
#else
    QString injectedDll = QCoreApplication::applicationDirPath() + "/OverlayX86.dll";
#endif

    url.addQueryItem("injectDll", injectedDll);

    service->setGameId("92");
    service->setUrl(url);
  }
}

void MainWindow::settingsIncomingPortChangedSlot()
{
  QString port = this->settingsViewModel->incomingPort();
  bool ok;
  unsigned short tmp = port.toUShort(&ok);
  if (ok)
    this->_gameDownloaderBuilder.torrentWrapper().changeListeningPort(tmp);
}

void MainWindow::settingsNumConnectionsChangedSlot()
{
  QString numConnection = this->settingsViewModel->numConnections();
  bool ok;
  qint32 tmp = numConnection.toInt(&ok);
  if (ok)
    this->_gameDownloaderBuilder.torrentWrapper().setMaxConnection(tmp);
}

void MainWindow::settingsDownloadSpeedChangedSlot()
{
  QString speed = this->settingsViewModel->downloadSpeed();
  bool ok;
  qint32 tmp = speed.toInt(&ok);
  if (ok)
    this->_gameDownloaderBuilder.torrentWrapper().setDownloadRateLimit(tmp * 1024);
}

void MainWindow::settingsUploadSpeedChangedSlot()
{
  QString speed = this->settingsViewModel->uploadSpeed();
  bool ok;
  qint32 tmp = speed.toInt(&ok);
  if (ok)
    this->_gameDownloaderBuilder.torrentWrapper().setUploadRateLimit(tmp * 1024);
}

void MainWindow::torrentListenPortChangedSlot(unsigned short port)
{
  QString tmp = QString::number(port);
  this->settingsViewModel->setIncomingPort(tmp);
}

void MainWindow::commandRecieved(QString name, QStringList arguments)
{
  this->_rembrGameFeature.commandRecieved(name, arguments);

  if (name == "activate") {
    this->activateWindow();
  } else if (name == "gogamenethelper" && arguments.size() > 0) {
    QString gameId = arguments.at(0);
    QString url = QString("http://www.gamenet.ru/games/%1/helper").arg(gameId);
    this->openExternalBrowser(url);
  } else if (name == "gogamenetmoney") {
    this->openExternalBrowser("http://www.gamenet.ru/money");
  } else if (name == "uninstall") {
    this->onWindowClose();
  }
}

void MainWindow::onServiceStarted(const GGS::Core::Service &service)
{
  emit this->serviceStarted(service.id());
}

void MainWindow::onServiceFinished(const GGS::Core::Service &service, GGS::GameExecutor::FinishState state)
{
  emit this->serviceFinished(service.id(), state);

  switch(state) {
  case GGS::GameExecutor::AuthorizationError:
    emit this->needAuth();
    break;
  case GGS::GameExecutor::ServiceAccountBlockedError:
    GGS::Core::UI::Message::warning(tr("INFO_CAPTION"), tr("SERVICE_ACCOUNT_BLOCKED_INFO"));
    break;
  case GGS::GameExecutor::ServiceAuthorizationImpossible:
    GGS::Core::UI::Message::warning(tr("INFO_CAPTION"), tr("SERVICE_AUTHORIZATION_IMPOSSIBLE_INFO"));
    break;
  case GGS::GameExecutor::PakkanenPermissionDenied:
    GGS::Core::UI::Message::warning(tr("INFO_CAPTION"), tr("SERVICE_ACCOUNT_CBT_PERMISSION_INFO").arg(service.name()));
    break;
  case GGS::GameExecutor::PakkanenPhoneVerification:
    emit this->needPakkanenVerification(service.id());
    break;
  case GGS::GameExecutor::GuestAccountExpired:
    emit this->authGuestConfirmRequest(service.id());
    break;
  }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
  delete this->_trayWindow;
  this->_trayWindow = 0;
}

void MainWindow::gameDownloaderStatusMessageChanged(const GGS::Core::Service *service, const QString& message)
{
  Q_CHECK_PTR(service);
  emit this->downloaderServiceStatusMessageChanged(service->id(), message);
}

void MainWindow::startBackgroundCheckUpdate()
{
  this->_checkUpdateHelper.checkUpdate();
}

int MainWindow::checkUpdateInterval()
{
  QDateTime now = QDateTime::currentDateTimeUtc();
  now = now.addSecs(14400); // Moscow time UTC+4
  int hour = now.time().hour();
  if (hour >= 14) {
    return 7200000;
  }

  return 1800000;
}

void MainWindow::checkUpdateHelperFinished(GGS::UpdateSystem::CheckUpdateHelper::Results result)
{
  switch(result)
  {
  case GGS::UpdateSystem::CheckUpdateHelper::Error:
    QTimer::singleShot(300000, &this->_checkUpdateHelper, SLOT(checkUpdate()));
    break;
  case GGS::UpdateSystem::CheckUpdateHelper::FoundUpdate: {
      DEBUG_LOG << "New update found. Restart required.";

      if (!this->_gameExecutorService.isAnyGameStarted() 
        && !this->_gameDownloaderBuilder.gameDownloader().isAnyServiceInProgress()) {

        if (this->isVisible())
          GGS::Core::UI::Message::information(tr("INFO_CAPTION"), tr("UPDATE_FOUND_MESSAGE"), GGS::Core::UI::Message::Ok);
        
        this->restartApplication(false);
        return;
      }

      QTimer::singleShot(this->checkUpdateInterval(), &this->_checkUpdateHelper, SLOT(checkUpdate()));
    }
    break;
  case GGS::UpdateSystem::CheckUpdateHelper::NotFound:
    QTimer::singleShot(this->checkUpdateInterval(), &this->_checkUpdateHelper, SLOT(checkUpdate()));
    break;
  default:
    DEBUG_LOG << "Unknown result " << result;
    break;
  }
}

void MainWindow::restApiGenericError(GGS::RestApi::CommandBase::Error error, QString message)
{
  switch(error) {
  case RestApi::CommandBase::AuthorizationFailed: // break пропущен не спроста
  case RestApi::CommandBase::AccountNotExists:
  case RestApi::CommandBase::AuthorizationLimitExceed:
  case RestApi::CommandBase::UnknownAccountStatus:
      emit this->needAuth();
  break;
  }
}

void MainWindow::showEvent(QShowEvent* event)
{
  this->repaint();
}

bool MainWindow::isWindowVisible()
{
  return this->isVisible() && !this->isMinimized();
}

void MainWindow::loadPlugin(QString pluginName)
{
  QString message = QString("Couldn't load plugin %1").arg(pluginName);

#ifdef _DEBUG
  QString pluginPath = QString("%1/%2d.dll").arg(QCoreApplication::applicationDirPath(), pluginName);
#else
  QString pluginPath = QString("%1/%2.dll").arg(QCoreApplication::applicationDirPath(), pluginName);
#endif

  nQMLContainer->engine()->importPlugin(pluginPath, "Tulip", &message);
}

void MainWindow::initializeStopDownloadServiceOnExecuteGameFeature()
{
  SIGNAL_CONNECT_CHECK(connect(&this->_stopDownloadServiceOnExecuteGame, 
    SIGNAL(downloadStopRequest(const GGS::Core::Service *)),
    &this->_gameDownloaderBuilder.gameDownloader(), 
    SLOT(stop(const GGS::Core::Service *)), Qt::QueuedConnection));

  SIGNAL_CONNECT_CHECK(connect(&this->_stopDownloadServiceOnExecuteGame, 
    SIGNAL(downloadStartRequest(const GGS::Core::Service *, GGS::GameDownloader::StartType)),
    &this->_gameDownloaderBuilder.gameDownloader(), 
    SLOT(start(const GGS::Core::Service *, GGS::GameDownloader::StartType)), Qt::QueuedConnection));

  SIGNAL_CONNECT_CHECK(connect(&this->_stopDownloadServiceOnExecuteGame,
    SIGNAL(torrentSessionPauseRequest()), 
    &this->_gameDownloaderBuilder.torrentWrapper(), 
    SLOT(pauseSession()), Qt::QueuedConnection));

  SIGNAL_CONNECT_CHECK(connect(&this->_stopDownloadServiceOnExecuteGame,
    SIGNAL(torrentSessionResumeRequest()), 
    &this->_gameDownloaderBuilder.torrentWrapper(), 
    SLOT(resumeSession()), Qt::QueuedConnection));

  SIGNAL_CONNECT_CHECK(connect(this, SIGNAL(serviceStarted(QString)), 
    &this->_stopDownloadServiceOnExecuteGame, SLOT(onGameExecuted()), Qt::QueuedConnection));

  SIGNAL_CONNECT_CHECK(connect(this, SIGNAL(serviceFinished(QString, int)), 
    &this->_stopDownloadServiceOnExecuteGame, SLOT(onGameFinished()), Qt::QueuedConnection));

  SIGNAL_CONNECT_CHECK(connect(&this->_gameDownloaderBuilder.gameDownloader(),
    SIGNAL(started(const GGS::Core::Service *, GGS::GameDownloader::StartType)),
    &this->_stopDownloadServiceOnExecuteGame, SLOT(onServiceStartDownload(const GGS::Core::Service *, GGS::GameDownloader::StartType)), Qt::QueuedConnection));

  SIGNAL_CONNECT_CHECK(connect(&this->_gameDownloaderBuilder.gameDownloader(),
    SIGNAL(finished(const GGS::Core::Service *)),
    &this->_stopDownloadServiceOnExecuteGame, SLOT(onServiceFinishDownload(const GGS::Core::Service *)), Qt::QueuedConnection));

  SIGNAL_CONNECT_CHECK(connect(&this->_gameDownloaderBuilder.gameDownloader(),
    SIGNAL(stopped(const GGS::Core::Service *)),
    &this->_stopDownloadServiceOnExecuteGame, SLOT(onServiceFinishDownload(const GGS::Core::Service *)), Qt::QueuedConnection));

  SIGNAL_CONNECT_CHECK(connect(&this->_gameDownloaderBuilder.gameDownloader(),
    SIGNAL(failed(const GGS::Core::Service *)),
    &this->_stopDownloadServiceOnExecuteGame, SLOT(onServiceFinishDownload(const GGS::Core::Service *)), Qt::QueuedConnection));
}

void MainWindow::gameDownloaderServiceInstalled(const GGS::Core::Service *service)
{
  Q_CHECK_PTR(service);
  emit this->serviceInstalled(service->id());
}

void MainWindow::gameDownloaderServiceUpdated(const GGS::Core::Service *service)
{
   Q_CHECK_PTR(service);
  this->activateWindow();
  emit this->selectService(service->id());
}

void MainWindow::migrateInstallDate(const QString& serviceId)
{
  if (!this->_gameDownloaderBuilder.gameDownloader().isInstalled(serviceId))
    return;

  GGS::Settings::Settings settings;
  settings.beginGroup("GameDownloader");
  settings.beginGroup(serviceId);

  if (settings.contains("installDate"))
    return;

  settings.setValue("installDate", QDateTime::currentDateTime());
}
