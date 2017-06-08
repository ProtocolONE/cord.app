#include <mainwindow.h>
#include <Player.h>
#include <BestInstallPath.h>
#include <HostMessageAdapter.h>

#include <Helper/CacheNetworkManagerFactory.h>
#include <Helper/ConfigLoader.hpp>

#include <Features/RestApi/ServiceHasAccess.h>
#include <Features/RenderRateHack.h>

#include <Features/Marketing/MarketingIntegrationMarker.h>

#include <viewmodel/UpdateViewModel.h>
#include <viewmodel/ApplicationStatisticViewModel.h>
#include <viewmodel/SettingsViewModel.h>
#include <viewmodel/GameSettingsViewModel.h>
#include <viewmodel/ServiceHandleViewModel.h>

#include <Host/CredentialConverter.h>
#include <Host/Translation.h>
#include <Host/ClientConnection.h>

#include <Host/Dbus/DbusConnection.h>
#include <Host/Dbus/DownloaderBridgeProxy.h>
#include <Host/Dbus/DownloaderSettingsBridgeProxy.h>
#include <Host/Dbus/ServiceSettingsBridgeProxy.h>
#include <Host/Dbus/ExecutorBridgeProxy.h>
#include <Host/Dbus/ApplicationBridgeProxy.h>
#include <Host/Dbus/ApplicationStatisticBridgeProxy.h>
#include <Host/Dbus/LicenseManagerBridgeProxy.h>

#include <Core/UI/Message>
#include <Core/Marketing.h>
#include <Core/System/FileInfo.h>
#include <Core/System/HardwareId.h>

#include <GameExecutor/GameExecutorService.h>

#include <UpdateSystem/UpdateInfoGetterResultInterface.h>

#include <RestApi/RequestFactory.h>

#include <Application/WindowHelper.h>

#include <Settings/settings.h>

#include <QtCore/QTranslator>
#include <QtCore/QSysInfo>
#include <QtCore/QFlags>
#include <QtCore/QStringList>
#include <QtCore/QSysInfo>

#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QDesktopWidget>

#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQmlError>

#include <QMetaType>

#define SIGNAL_CONNECT_CHECK(X) { bool result = X; Q_ASSERT_X(result, __FUNCTION__ , #X); }

using GameNet::Host::Bridge::Credential;
using GameNet::Host::DBus::DBusConnection;
using GameNet::Host::ClientConnection;

using GameNet::Host::Bridge::createDbusCredential;
using GameNet::Host::Bridge::createGameNetCredential;

MainWindow::MainWindow(QWindow *parent)
  : QQuickView(parent)
  , _gameArea(GGS::Core::Service::Live)
  , _downloader(nullptr)
  , _downloaderSettings(nullptr)
  , _serviceSettings(nullptr)
  , _executor(nullptr)
  , _applicationProxy(nullptr)
  , _applicationStatistic(nullptr)
  , _clientConnection(nullptr)
  , _bestInstallPath(nullptr)
  , _renderRateHack(nullptr)
{
  this->hide();

  if (QString::fromLatin1(qgetenv("QT_OPENGL")) == "software") {
    this->_renderRateHack = new Features::RenderRateHack(this);
    this->_renderRateHack->init(this);
    QObject::connect(qApp, &QGuiApplication::applicationStateChanged, this, &MainWindow::onApplicationStateChanged);

  }
}

MainWindow::~MainWindow()
{
}

void MainWindow::initialize()
{
  qRegisterMetaType<GameNet::Host::Bridge::DownloadProgressArgs>("GameNet::Host::Bridge::DownloadProgressArgs");
  qDBusRegisterMetaType<GameNet::Host::Bridge::DownloadProgressArgs>();
  
  qRegisterMetaType<GameNet::Host::Bridge::Credential>("GameNet::Host::Bridge::Credential");
  qDBusRegisterMetaType<GameNet::Host::Bridge::Credential>();

  // DBUS...
  QDBusConnection &connection = DBusConnection::bus();
  QString dbusService("com.gamenet.dbus");
  
  this->_clientConnection = new ClientConnection("QGNA", this);
  this->_clientConnection->init();
  
  QObject::connect(this->_clientConnection, &ClientConnection::disconnected,
    this, &MainWindow::onWindowClose);
  
  QObject::connect(this->_clientConnection, &ClientConnection::wrongCredential,
    this, &MainWindow::wrongCredential);
  
  this->_applicationProxy = new ApplicationBridgeProxy(dbusService, "/application", connection, this);
  this->_downloader = new DownloaderBridgeProxy(dbusService, "/downloader", connection, this);
  this->_downloaderSettings = new DownloaderSettingsBridgeProxy(dbusService, "/downloader/settings", connection, this);
  this->_serviceSettings = new ServiceSettingsBridgeProxy(dbusService, "/serviceSettings", connection, this);
  this->_executor = new ExecutorBridgeProxy(dbusService, "/executor", connection, this);
  this->_applicationStatistic = new ApplicationStatisticBridgeProxy(dbusService, "/applicationStatistic", connection, this);
  this->_licenseManager = new LicenseManagerBridgeProxy(dbusService, "/licenseManager", connection, this);
    
  QObject::connect(this->_applicationProxy, &ApplicationBridgeProxy::languageChanged,
    this, &MainWindow::languageChanged);

  this->_bestInstallPath = new BestInstallPath(this);
  this->_bestInstallPath->setServiceSettings(this->_serviceSettings);

  QObject::connect(this->_applicationProxy, &ApplicationBridgeProxy::initCompleted,
    this, &MainWindow::initCompleted);

  QObject::connect(this->_applicationProxy, &ApplicationBridgeProxy::restartUIRequest,
    this, &MainWindow::restartUIRequest);

  QObject::connect(this->_applicationProxy, &ApplicationBridgeProxy::shutdownUIRequest,
    this, &MainWindow::shutdownUIRequest);

  QObject::connect(this->_applicationProxy, &ApplicationBridgeProxy::uninstallServiceRequest,
    this, &MainWindow::uninstallServiceRequest);

  QObject::connect(this->_applicationProxy, &ApplicationBridgeProxy::additionalResourcesReady,
    this, &MainWindow::additionalResourcesReady);

  qRegisterMetaType<GameNet::Host::Bridge::DownloadProgressArgs>("GameNet::Host::Bridge::DownloadProgressArgs");
  qDBusRegisterMetaType<GameNet::Host::Bridge::DownloadProgressArgs>();

  new HostMessageAdapter(this);

  this->initRestApi();
  
  this->_commandLineArguments.parse(QCoreApplication::arguments());
  
  if (this->_commandLineArguments.contains("gamepts"))
    this->_gameArea = GGS::Core::Service::Pts;
  
  if (this->_commandLineArguments.contains("gametest"))
    this->_gameArea = GGS::Core::Service::Tst;
  
  this->setFileVersion(GGS::Core::System::FileInfo::version(QCoreApplication::applicationFilePath())); 
  
  this->setTitle("GameNet " + this->_fileVersion);
  this->setColor(QColor(0, 0, 0, 0));

  this->setFlags(Qt::Window
    | Qt::FramelessWindowHint 
    | Qt::WindowMinimizeButtonHint 
    | Qt::WindowSystemMenuHint); //Этот код уберет все внешние элементы формы
  
  GameNet::Host::Translation::load(this->translators, this);
  this->selectLanguage(this->_applicationProxy->language());
  
  this->checkDesktopDepth();
  
  this->settingsViewModel = new SettingsViewModel(this);
  this->settingsViewModel->setDownloaderSettings(this->_downloaderSettings);
  this->settingsViewModel->setApplicationProxy(this->_applicationProxy);
  
  qmlRegisterType<UpdateViewModel>("qGNA.Library", 1, 0, "UpdateViewModel");
  qmlRegisterType<Player>("qGNA.Library", 1, 0, "Player");
  qmlRegisterType<GGS::Core::UI::Message>("qGNA.Library", 1, 0, "Message");
  qmlRegisterType<ApplicationStatisticViewModel>("qGNA.Library", 1, 0, "ApplicationStatistic");
  qmlRegisterType<ServiceHandleViewModel>("qGNA.Library", 1, 0, "ServiceHandle");
  
  qmlRegisterUncreatableType<GGS::Downloader::DownloadResultsWrapper>("qGNA.Library", 1, 0,  "DownloadResults", "");
  qmlRegisterUncreatableType<GGS::UpdateSystem::UpdateInfoGetterResultsWrapper>("qGNA.Library", 1, 0,  "UpdateInfoGetterResults", "");
  
  qRegisterMetaType<Features::Thetta::ThettaInstaller::Result>("Features::Thetta::ThettaInstaller::Result");
  
  this->initMarketing();
  
  this->engine()->setNetworkAccessManagerFactory(new CacheNetworkManagerFactory(this));
  this->engine()->addImportPath(":/");
  this->engine()->addImportPath((QCoreApplication::applicationDirPath() + "/plugins5/"));
  this->engine()->addPluginPath(QCoreApplication::applicationDirPath() + "/plugins5/");

  QObject::connect(&this->_restapiManager, &GGS::RestApi::RestApiManager::genericErrorEx,
    this, &MainWindow::restApiGenericError);
  
  messageAdapter = new QmlMessageAdapter(this);
    
  this->_gameSettingsViewModel = new GameSettingsViewModel(this);
  this->_gameSettingsViewModel->setDownloader(this->_downloader);
  this->_gameSettingsViewModel->setServiceSettings(this->_serviceSettings);
    
  this->rootContext()->setContextProperty("keyboardHook", &this->_keyboardLayoutHelper);
  this->rootContext()->setContextProperty("mainWindow", this);
  this->rootContext()->setContextProperty("installPath", "file:///" + QCoreApplication::applicationDirPath() + "/");
  this->rootContext()->setContextProperty("settingsViewModel", settingsViewModel);
  this->rootContext()->setContextProperty("messageBox", messageAdapter);
  this->rootContext()->setContextProperty("gameSettingsModel", this->_gameSettingsViewModel);

  this->setResizeMode(QQuickView::SizeRootObjectToView);
  this->setSource(QUrl("qrc:/Main.qml"));

  if (this->status() == QQuickView::Status::Error) {
    Q_FOREACH(const QQmlError& error, this->errors()) {
      DEBUG_LOG << error;
    }

    // UNDONE решить что делать в случаи фейла верстки
  }
  
  SIGNAL_CONNECT_CHECK(QObject::connect(this->engine(), SIGNAL(quit()), this, SLOT(onWindowClose())));
  connect(this, &MainWindow::quit, this, &MainWindow::onWindowClose);

  Message::setAdapter(messageAdapter);

  if (this->_commandLineArguments.contains("minimized")) {
    //this->showMinimized();
    this->hide();
  }
  else {
    DEBUG_LOG;
    this->activateWindow();
  }
  
  this->sendStartingMarketing();
  
  this->_keyboardLayoutHelper.update();
}

void MainWindow::sendStartingMarketing()
{
  int dwMajorVersion = 6;
  int dwMinorVersion = 1;

  switch (QSysInfo::windowsVersion()) {
  case QSysInfo::WV_5_1: dwMajorVersion = 5; dwMinorVersion = 1; break;
  case QSysInfo::WV_6_0: dwMajorVersion = 6; dwMinorVersion = 0; break;
  case QSysInfo::WV_6_1: dwMajorVersion = 6; dwMinorVersion = 1; break;
  case QSysInfo::WV_6_2: dwMajorVersion = 6; dwMinorVersion = 2; break;
  case QSysInfo::WV_6_3: dwMajorVersion = 6; dwMinorVersion = 3; break;
  case QSysInfo::WV_10_0: dwMajorVersion = 10; dwMinorVersion = 0; break;
  }

  QVariantMap params;
  params["windowsMajorVersion"] = dwMajorVersion;
  params["windowsMinorVersion"] = dwMinorVersion;
  params["windowsVersion"] = QSysInfo::productVersion();
  params["updateArea"] = this->settingsViewModel->updateArea();
  params["version"] = this->_fileVersion;    

  GGS::Core::Marketing::send(GGS::Core::Marketing::AnyStartQGna, params);
  GGS::Core::Marketing::sendOnce(GGS::Core::Marketing::FirstRunGna);
}

void MainWindow::restartUISlot(bool minimized)
{
  this->_applicationProxy->restartApplication(minimized);
}

void MainWindow::shutdownUISlot()
{
  this->_applicationProxy->shutdownUIResult();
  this->onWindowClose();
}

bool MainWindow::isInitCompleted()
{
  return this->_applicationProxy->isInitCompleted();
}

void MainWindow::checkDesktopDepth() {
  QDesktopWidget widget;
  if (widget.depth() == 16) {
      QString info = QObject::tr("SCREEN_DEPTH_LOVER_THAN_16_INFO");
      QString caption = QObject::tr("SCREEN_DEPTH_LOVER_THAN_16_CAPTION");

      MessageBoxW(0, 
          info.toStdWString().c_str(), 
          caption.toStdWString().c_str(),
          MB_OK | MB_ICONINFORMATION); 
  }
}

bool MainWindow::nativeEvent(const QByteArray & eventType, void * message, long * result)
{
  if (message->message == WM_KEYUP) 
    this->_keyboardLayoutHelper.update();

  return QQuickView::nativeEvent(eventType, message, result);
}

void MainWindow::activateWindow()
{
  DEBUG_LOG << "activateWindow";
  this->show();
  
  // Это нам покажет окно
  //this->setFocusPolicy(Qt::StrongFocus);
  //this->setWindowState(Qt::WindowActive); 
  this->showNormal();

  // Эта функция активирует окно и поднмиает его повех всех окон
  GGS::Application::WindowHelper::activate(this->winId());

  this->_taskBarHelper.restore();
  
  //this->repaint();
}

bool MainWindow::isDownloading(QString serviceId)
{
  return this->_downloader->isInProgress(serviceId);
}

QString MainWindow::language()
{
  return this->_applicationProxy->language();
}

void MainWindow::saveLanguage(const QString& language)
{
  this->_applicationProxy->setLanguage(language);
}

void MainWindow::selectLanguage(const QString& language) 
{ 
  if (this->translators[language])
    QApplication::installTranslator(this->translators[language]);  

  emit languageChanged();
}

void MainWindow::onWindowClose()
{
  DEBUG_LOG << "Shutting down";

  //this->repaint();
  this->hide();
  this->_clientConnection->close();
  QCoreApplication::quit();
}

void MainWindow::authSuccessSlot(const QString& userId, const QString& appKey, const QString& cookie)
{
  qDebug() << "Auth success with userId " << userId;

  GGS::RestApi::GameNetCredential credential;
  credential.setUserId(userId);
  credential.setAppKey(appKey);
  credential.setCookie(cookie);

  this->_credential = credential;
  this->_restapiManager.setCridential(credential);

  this->_clientConnection->setCredential(credential);
}

void MainWindow::restartApplicationAfterDriverDisconnect(Features::Thetta::ThettaInstaller::Result result)
{
  if (result == Features::Thetta::ThettaInstaller::Running) // походу 2ой дисконнект
    return;

  this->internalRestartApplication(this->_restartArguments);
}

void MainWindow::restartApplication(bool shouldStartWithSameArguments)
{
  if (!this->_serviceLoader.thettaInstaller()) {
    this->internalRestartApplication(shouldStartWithSameArguments);
    return;
  }

  QObject::connect(
    this->_serviceLoader.thettaInstaller(), &Features::Thetta::ThettaInstaller::disconnected,
    this, &MainWindow::restartApplicationAfterDriverDisconnect);

  this->_restartArguments = shouldStartWithSameArguments;
  this->_serviceLoader.thettaInstaller()->disconnectFromDriver();
}

void MainWindow::openExternalUrlWithAuth(const QString& url)
{
  QString authUrl;
  if(this->_credential.appKey().isEmpty()) {
    authUrl = url;
  } else {
    authUrl = "https://gnlogin.ru/?auth=";
    authUrl.append(this->_credential.cookie());
    authUrl.append("&rp=");
    authUrl.append(QUrl::toPercentEncoding(url));
  }

  authUrl.append('\0');

  this->openExternalUrl(authUrl);
}

void MainWindow::openExternalUrl(const QString& url)
{
  this->_applicationProxy->openBrowser(url);
}

void MainWindow::logout()
{
  this->_credential.setAppKey("");
  this->_credential.setUserId("");
  this->_credential.setCookie("");

  this->_restapiManager.setCridential(this->_credential);
  this->_premiumExecutor.terminateAll();
}

  this->_clientConnection->setCredential(this->_credential);
}

void MainWindow::prepairGameDownloader()
{
  using GGS::GameDownloader::GameDownloadService;
  
  QObject::connect(this->_downloader, &DownloaderBridgeProxy::totalProgress, 
    this, &MainWindow::downloadGameTotalProgressChanged);

  QObject::connect(this->_downloader, &DownloaderBridgeProxy::downloadProgress, 
    this, &MainWindow::downloadGameProgressChanged);

  QObject::connect(this->_downloader, &DownloaderBridgeProxy::started, 
    this, &MainWindow::gameDownloaderStarted);

  QObject::connect(this->_downloader, &DownloaderBridgeProxy::finished, 
    this, &MainWindow::gameDownloaderFinished);

  QObject::connect(this->_downloader, &DownloaderBridgeProxy::stopped, 
    this, &MainWindow::gameDownloaderStopped);

  // HACK отключено 03.10.2013. Чтобы можно было запустить процесс с флагом CREATE_BREAKAWAY_FROM_JOB у текущего джоба процесса
  // должен в лимит флагах стоять соотвествующий флаг JOB_OBJECT_LIMIT_BREAKAWAY_OK 
  //auto hook = HookEngine::createHook<HookEngine::Stdcall, BOOL, LPCWSTR, LPWSTR, void*, void*, BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFO, LPPROCESS_INFORMATION>("Kernel32.dll", "CreateProcessW");
  //hook->assignHook([hook] (LPCWSTR a1, LPWSTR a2, void* a3, void* a4, BOOL a5, DWORD dwCreationFlags, LPVOID a6, LPCWSTR a7, LPSTARTUPINFO a8, LPPROCESS_INFORMATION a9) -> BOOL {
  //  if (isBreakAwayAvailable()) 
  //    return hook->original(a1, a2, a3, a4, a5, dwCreationFlags | CREATE_BREAKAWAY_FROM_JOB, a6, a7, a8, a9);
  //  else
  //    return hook->original(a1, a2, a3, a4, a5, dwCreationFlags, a6, a7, a8, a9);
  //});
  
  SIGNAL_CONNECT_CHECK(QObject::connect(&this->_gameDownloader, SIGNAL(started(const GGS::Core::Service *, GGS::GameDownloader::StartType)), 
    &this->_rembrGameFeature, SLOT(started(const GGS::Core::Service *))));
  SIGNAL_CONNECT_CHECK(QObject::connect(&this->_gameDownloader, SIGNAL(finished(const GGS::Core::Service *)),  
    &this->_rembrGameFeature, SLOT(finished(const GGS::Core::Service *))));

  QObject::connect(this->_downloader, &DownloaderBridgeProxy::statusMessageChanged, 
    this, &MainWindow::gameDownloaderStatusMessageChanged);

  QObject::connect(this->_downloader, &DownloaderBridgeProxy::serviceInstalled, 
    this, &MainWindow::gameDownloaderServiceInstalled);
  
  QObject::connect(this->_downloader, &DownloaderBridgeProxy::serviceUpdated, 
    this, &MainWindow::gameDownloaderServiceUpdated);

  QObject::connect(this->_downloader, &DownloaderBridgeProxy::accessRequired,
    this, &MainWindow::gameDownloaderAccessRequired);
}

void MainWindow::downloadGameTotalProgressChanged(const QString& serviceId, int progress)
{
  emit totalProgressChanged(serviceId, progress);
}

void MainWindow::downloadGameProgressChanged(
  const QString& serviceId,
  int progress, 
  GameNet::Host::Bridge::DownloadProgressArgs args)
{
  if (args.status() == GGS::Libtorrent::EventArgs::ProgressEventArgs::CheckingFiles) {
      emit this->rehashProgressChanged(service->id(), progress, args.progress() * 100);
      return;
  }

  emit this->downloadProgressChanged(service->id(), 
    progress, 
    args.totalWantedDone(), 
    args.totalWanted(), 
    args.directTotalDownload(), 
    args.peerTotalDownload(), 
    args.payloadTotalDownload(), 
    args.peerPayloadDownloadRate(), 
    args.payloadDownloadRate(), 
    args.directPayloadDownloadRate(), 
    args.payloadUploadRate(), 
    args.totalPayloadUpload());
}

void MainWindow::progressChanged(QString serviceId, qint8 progress)
{
  emit this->progressbarChange(serviceId, progress, -1, -1, 0, 0, 0 ,0, 0, 0, 0, 0);
}

void MainWindow::gameDownloaderStarted(const QString& serviceId, int startType)
{
  emit this->downloaderStarted(serviceId, startType);
}

void MainWindow::gameDownloaderFinished(const QString& serviceId) 
{
  emit this->downloaderFinished(serviceId);
}

bool MainWindow::executeService(QString id)
{
  if (this->_executor->isGameStarted(id))
    return false;

  if (!this->isWindowVisible()) {
    emit this->selectService(id);
    return false;
  }

  if (this->_restapiManager.credential().userId().isEmpty()) {
    emit this->authBeforeStartGameRequest(id);
    return false;
  }

  GGS::RestApi::GameNetCredential baseCredential =
    GGS::RestApi::RestApiManager::commonInstance()->credential();

  this->_executor->execute(id, createDbusCredential(baseCredential));
  return true;
}

void MainWindow::gameDownloaderStopped(const QString& serviceId)
{
  emit this->downloaderStopped(serviceId);
}

void MainWindow::gameDownloaderStopping(const QString& serviceId)
{
  emit this->downloaderStopping(serviceId);
}

void MainWindow::updateFinishedSlot() 
{
  this->postUpdateInit();
}

void MainWindow::gameDownloaderFailed(const QString& serviceId)
{
  emit this->downloaderFailed(serviceId);
}

void MainWindow::removeStartGame(QString serviceId)
{
  if (this->_silentMode.isEnabled()) {
    static bool firstCall = true;

    if (firstCall) {
      firstCall = false;
      this->forceDownload(serviceId);
      return;
    }
  }

  int totalCount = this->_applicationStatistic->executeGameTotalCount(serviceId);
  if (totalCount > 0) {
    this->selectService(serviceId);
    return;
  }

  this->downloadButtonStart(serviceId);
}

void MainWindow::downloadButtonStart(QString serviceId)
{
  qDebug() << "downloadButtonStart " << serviceId;

  GGS::Core::Service *service = this->getService(serviceId);
  if (!service)
    return;
  
  emit this->downloadButtonStartSignal(serviceId); 

  if (!this->_serviceSettings->isDownloadable(serviceId)) {
    GGS::RestApi::GameNetCredential baseCredential = 
      GGS::RestApi::RestApiManager::commonInstance()->credential();

    this->_executor->execute(serviceId, createDbusCredential(baseCredential));
    return;
  }

  if (this->isLicenseAccepted(serviceId)) {
    this->startGame(serviceId);
    return;
  }

  DEBUG_LOG;
  this->activateWindow();
  emit this->showLicense(serviceId);
}

void MainWindow::forceDownload(QString serviceId)
{
  if (this->_serviceSettings->isDownloadable(serviceId)) {
    this->_downloader->start(serviceId, static_cast<int>(GGS::GameDownloader::Normal));
  }
}

void MainWindow::downloadButtonPause(QString serviceId)
{
  if (this->_serviceSettings->isDownloadable(serviceId)) {
    this->_downloader->stop(serviceId);
    return;

  if (service->isDownloadable())
    this->_gameDownloader.stop(service);
  else
    this->_premiumExecutor.executeMain(service);
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
    this->_applicationArea = GGS::Core::Service::Pts;
    break;
  case 2:
    this->_updateArea = QString("tst");
    this->_applicationArea = GGS::Core::Service::Tst;
    break;
    /*
  INFO Раскомментируйте этот блок, если вам нужно использовать специальную версию для тестирования на живых людях.
  case 3:
    this->_updateArea = QString("2live");
    this->_applicationArea = GGS::Core::Service::Live;
    break;
    */
  default:
    this->_updateArea = QString("live");  
    this->_applicationArea = GGS::Core::Service::Live;
  }

  GGS::RestApi::GameNetCredential baseCredential = 
    GGS::RestApi::RestApiManager::commonInstance()->credential();
    
  this->_executor->execute(serviceId, createDbusCredential(baseCredential));
}

void MainWindow::uninstallService(const QString serviceId)
{
  this->_downloader->start(serviceId, GGS::GameDownloader::Uninstall);
}

void MainWindow::cancelServiceUninstall(const QString serviceId)
{
  this->_applicationProxy->cancelUninstallServiceRequest(serviceId);
}

bool MainWindow::isLicenseAccepted(const QString& serviceId)
{
  return this->_licenseManager->hasAcceptedLicense(serviceId);
}

void MainWindow::startGame(const QString& serviceId)
{
  GGS::Core::Service *service = this->getService(serviceId);
  if (!service)
    return;

  if (this->_premiumExecutor.isMainGameStarted())
    return;

  if (this->_premiumExecutor.isSecondGameStarted() && !this->_premiumExecutor.isSecondGameStarted(serviceId))
    return;
 
  if (service->isDownloadable()) {
    QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);
    settings.beginGroup(serviceId);
    settings.setValue("DownloadPath", service->downloadPath());
    settings.setValue("InstallPath", service->installPath());
    service->setIsDefaultInstallPath(true);
    settings.endGroup();
    this->_serviceLoader.setExecuteUrl(serviceId, service->installPath());
    this->_gameDownloader.start(service, GGS::GameDownloader::Normal);
  } else {
    bool isAuthed = !this->_restapiManager.credential().userId().isEmpty();
    if (!isAuthed) {
      emit this->authBeforeStartGameRequest(serviceId);
      return;
    }

    this->_premiumExecutor.executeMain(service);
  }

  GGS::RestApi::GameNetCredential baseCredential = 
    GGS::RestApi::RestApiManager::commonInstance()->credential();

  this->_executor->execute(serviceId, createDbusCredential(baseCredential));
}

void MainWindow::commandRecieved(QString name, QStringList arguments)
{
  DEBUG_LOG << name << arguments;
  if (name == "quit") {
    this->onWindowClose();
    return;
  } 

  if (name == "settings") {
    emit this->navigate("ApplicationSettings");
    return;
  } 

  if (name == "activate") {
    DEBUG_LOG;
    this->activateWindow();
    return;
  } 

  if (name == "gogamenethelper" && arguments.size() > 0) {
    QString gameId = arguments.at(0);
    QString url = QString("http://www.gamenet.ru/games/%1/helper").arg(gameId);
    this->openExternalUrlWithAuth(url);
    return;
  } 

  if (name == "gogamenetmoney") {
      this->navigate("gogamenetmoney");

    return;
  }

  if (name == "gocombatarmsrating") {
    this->openExternalUrlWithAuth("http://www.combatarms.ru/ratings/user/");
    return;
  } 

  if (name == "uninstall" && arguments.size() > 0) {
    QString serviceId = arguments.at(0);
    emit this->uninstallServiceRequest(serviceId);
  }
}

void MainWindow::onServiceStarted(const QString &serviceId)
{
  emit this->serviceStarted(serviceId);
}

void MainWindow::onServiceFinished(const QString &serviceId, int state) 
{
  // GGS::GameExecutor::FinishState state
  emit this->serviceFinished(serviceId, state);

  switch(state) {
  case GGS::GameExecutor::AuthorizationError:
    emit this->wrongCredential(this->_credential.userId());
    break;
  case GGS::GameExecutor::ServiceAccountBlockedError:
    GGS::Core::UI::Message::warning(tr("INFO_CAPTION"), tr("SERVICE_ACCOUNT_BLOCKED_INFO")); 
    break;
  case GGS::GameExecutor::ServiceAuthorizationImpossible:
    //INFO Handled in qml 
    break;
  case GGS::GameExecutor::PakkanenPermissionDenied:
    GGS::Core::UI::Message::warning(
      tr("INFO_CAPTION"), 
      tr("SERVICE_ACCOUNT_CBT_PERMISSION_INFO").arg(this->_serviceSettings->name(serviceId)));
    break;
  case GGS::GameExecutor::PakkanenPhoneVerification:
    emit this->needPakkanenVerification(serviceId);
    break;
  case GGS::GameExecutor::GuestAccountExpired:
    emit this->authGuestConfirmRequest(serviceId);
    break;
  case GGS::GameExecutor::PakkanenGeoIpBlocked:
    GGS::Core::UI::Message::warning(
      tr("INFO_CAPTION"),
      tr("SERVICE_ACCOUNT_GEO_IP_BLOCKED_INFO").arg(this->_serviceSettings->name(serviceId)));
    break;
  }
}

void MainWindow::onSecondServiceStarted(const QString &serviceId)
{
  emit this->secondServiceStarted(serviceId);
}

void MainWindow::onSecondServiceFinished(const QString &serviceId, int state) 
{
  emit this->secondServiceFinished(serviceId, state);

  DEBUG_LOG << "Finish state" << state;

  switch(state) {
  case GGS::GameExecutor::AuthorizationError:
    GGS::Core::UI::Message::warning(tr("INFO_CAPTION"), tr("SECOND_SERVICE_AUTH_ERROR")); 
    break;
  case GGS::GameExecutor::ServiceAccountBlockedError:
    GGS::Core::UI::Message::warning(tr("INFO_CAPTION"), tr("SERVICE_ACCOUNT_BLOCKED_INFO")); 
    break;
  case GGS::GameExecutor::ServiceAuthorizationImpossible:
    //INFO Handled in qml 
    break;
  }
}

void MainWindow::gameDownloaderStatusMessageChanged(const QString& serviceId, const QString& message)
{
  emit this->downloaderServiceStatusMessageChanged(serviceId, message);
}

void MainWindow::restApiGenericError(
  GGS::RestApi::CommandBase::Error error,
  QString message,
  GGS::RestApi::CommandBase *command)
{
  using GGS::RestApi::CommandBase;

  switch(error) {
  case CommandBase::AuthorizationFailed: // break пропущен не спроста
  case CommandBase::AccountNotExists:
  case CommandBase::AuthorizationLimitExceed:
  case CommandBase::UnknownAccountStatus:
    break;
  default:
    return; // ignore all other error 
  }

  const QMap<QString, QString>* params = command->commandParameters();
  QString userId = params->value("userId", "");
  if (userId.isEmpty())
    return;

  emit this->wrongCredential(userId);
}

void MainWindow::showEvent(QShowEvent* event)
{
  //this->_taskBarHelper.prepare(reinterpret_cast<HWND>(this->effectiveWinId()));
  this->_taskBarHelper.prepare(reinterpret_cast<HWND>(this->winId()));
  emit this->taskBarButtonMsgRegistered(this->_taskBarHelper.getTaskbarCreatedMessageId());

  // UNDONE  !!!!!!!!!!!!!!!!Next one overloaded due to QGNA-128
  // посмотреть надо ли теперь форсированно репейнтить при разворачивани
  //this->repaint();
  //this->requestUpdate();
  //QApplication::postEvent(this, new QEvent(QEvent::UpdateRequest), Qt::LowEventPriority);
  QQuickView::showEvent(event);
}

bool MainWindow::isWindowVisible()
{
  return this->isVisible() && this->windowState() != Qt::WindowMinimized;
}

// UNDONE Больше мы не грузим руками плагины - удлить функцию
void MainWindow::loadPlugin(QString pluginName, QString uri)
{
  QString message = QString("Couldn't load plugin %1").arg(pluginName);

#ifdef _DEBUG
  QString pluginPath = QString("%1/%2d.dll").arg(QCoreApplication::applicationDirPath(), pluginName);
#else
  QString pluginPath = QString("%1/%2.dll").arg(QCoreApplication::applicationDirPath(), pluginName);
#endif

  QList<QQmlError> errors;
  this->engine()->importPlugin(pluginPath, uri, &errors);

  Q_FOREACH(const QQmlError& error, errors) {
    DEBUG_LOG << error.toString() << "\n" << error.description();
  }
}

void MainWindow::gameDownloaderServiceInstalled(const QString& serviceId)
{
  emit this->serviceInstalled(serviceId);
}

void MainWindow::gameDownloaderServiceUpdated(const QString& serviceId)
{
  DEBUG_LOG;
  if (this->_silentMode.isEnabled()) {
    static bool firstTime = true;
    if (firstTime) {
      firstTime = false;
      emit this->selectService(serviceId);
      return;
    }
  }

  this->activateWindow();
  emit this->selectService(serviceId);
}

void MainWindow::postUpdateInit()
{
  this->prepairGameDownloader();

  QObject::connect(this->_executor, &ExecutorBridgeProxy::serviceStarted,
    this, &MainWindow::onServiceStarted);

  QObject::connect(this->_executor, &ExecutorBridgeProxy::serviceFinished,
    this, &MainWindow::onServiceFinished);

  QObject::connect(this->_executor, &ExecutorBridgeProxy::secondServiceStarted,
    this, &MainWindow::onSecondServiceStarted);

  QObject::connect(this->_executor, &ExecutorBridgeProxy::secondServiceFinished,
    this, &MainWindow::onSecondServiceFinished);

  QObject::connect(this->_executor, &ExecutorBridgeProxy::serviceStarted,
    &this->_silentMode, &Features::SilentMode::gameStarted);
}

void MainWindow::onApplicationStateChanged(Qt::ApplicationState state)
{
  if (!this->_renderRateHack)
    return;

  bool active = state == Qt::ApplicationActive;
  this->_renderRateHack->setVsyncDelay(active ? 40 : 200);
}

bool MainWindow::anyLicenseAccepted()
{
  return this->_licenseManager->hasAcceptedLicense();
}

QString MainWindow::startingService()
{
  if (!this->_commandLineArguments.contains("startservice"))
    return "0";

  QStringList arguments = this->_commandLineArguments.commandArguments("startservice");
  if (arguments.count() > 0)
    return arguments.at(0);

  return "0";
}

QString MainWindow::getExpectedInstallPath(const QString& serviceId)
{
  return this->_bestInstallPath->expectedPath(serviceId);
}

QString MainWindow::getBestInstallPath(const QString& serviceId)
{
  return this->_bestInstallPath->bestInstallPath(serviceId);
}

void MainWindow::setServiceInstallPath(const QString& serviceId, const QString& path)
{
  this->_serviceSettings->setInstallPath(serviceId, path);

  if (!this->_serviceSettings->hasDownloadPath(serviceId)) {
    this->_serviceSettings->setDownloadPath(serviceId, path);
    return;
  }

  QString downloadPath = this->_serviceSettings->isDefaultDownloadPath(serviceId)
    ? QString("%1/dist").arg(path)
    : this->_serviceSettings->downloadPath(serviceId);

  this->_serviceSettings->setDownloadPath(serviceId, downloadPath);
}

void MainWindow::acceptFirstLicense(const QString& serviceId)
{
  this->_licenseManager->acceptLicense(serviceId, "1");
}

void MainWindow::initFinished()
{
  emit this->updateFinished();
}

void MainWindow::initRestApi()
{
  //Port selection due to https://jira.gamenet.ru:8443/browse/QGNA-285

  Features::Helper::DebugConfigLoader debugConfig;
  debugConfig.init();

  QString overrideApiUrl;
  bool overrideApi = debugConfig.apiConfig(overrideApiUrl);

  QString apiUrl;

  if (overrideApi) {
    apiUrl = overrideApiUrl;
  } else {
    QStringList ports;
    ports << "443" << "7443" << "8443" << "9443" << "10443" << "11443";
    QString randomPort = ports.takeAt(qrand() % ports.count());
    apiUrl = QString("https://gnapi.com:%1/restapi").arg(randomPort);
  }

  GGS::Settings::Settings settings;
  settings.setValue("qGNA/restApi/url", apiUrl);

  qDebug() << "Using rest api url " << apiUrl;

  this->_restapiManager.setUri(apiUrl);
  this->_restapiManager.setRequest(GGS::RestApi::RequestFactory::Http);
  this->_restapiManager.setCache(&_fakeCache);

  bool debugLogEnabled = false;
  if (debugConfig.debugApiEnabled(debugLogEnabled))
    this->_restapiManager.setDebugLogEnabled(debugLogEnabled);

  GGS::RestApi::RestApiManager::setCommonInstance(&this->_restapiManager);
}

bool MainWindow::event(QEvent* event)
{
  switch(event->type()) {
  case QEvent::Close:
    this->hide();
    event->ignore();
    break;
  }

  return QQuickView::event(event);
}

void MainWindow::initMarketing()
{
  QSettings midSettings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);
  QString mid = midSettings.value("MID", "").toString();
  this->_marketingTargetFeatures.init("qGNA", mid);

  int installerKey = midSettings.value("InstKey").toInt();
  this->_marketingTargetFeatures.setInstallerKey(installerKey);
  this->_marketingTargetFeatures.setRequestInterval(1000);
}

bool MainWindow::executeSecondService(QString id, QString userId, QString appKey)
{
  if (!this->_executor->canExecuteSecond(id))
    return false;

  GGS::RestApi::GameNetCredential baseCredential = 
    GGS::RestApi::RestApiManager::commonInstance()->credential();

  GGS::RestApi::GameNetCredential credential;
  credential.setUserId(userId);
  credential.setAppKey(appKey);
  // set cookie if needed 

  this->_clientConnection->setSecondCredential(credential);

  this->_executor->executeSecond(
    id,
    createDbusCredential(baseCredential),
    createDbusCredential(credential));

  return true;
}

void MainWindow::terminateSecondService()
{
  this->_executor->shutdownSecond();
}

void MainWindow::mousePressEvent(QMouseEvent* event) 
{
  if (event->button() & Qt::LeftButton)
    emit this->leftMousePress(event->x(), event->y());

  QQuickView::mousePressEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
  if (event->button() & Qt::LeftButton)
    emit this->leftMouseRelease(event->x(), event->y());

  QQuickView::mouseReleaseEvent(event);
}

void MainWindow::onTaskbarButtonCreated()
{
  this->_taskBarHelper.init();
}

void MainWindow::onProgressUpdated(int progressValue, const QString &status)
{
  TaskBarHelper::Status newStatus = TaskBarHelper::StatusUnknown;
  if (status == "Normal") {
    newStatus = TaskBarHelper::StatusNormal;
  }
  else if (status == "Paused") {
    newStatus = TaskBarHelper::StatusPaused;
  }
  else if (status == "Error") {
    newStatus = TaskBarHelper::StatusError;
  }

  this->_taskBarHelper.setProgress(progressValue);
  this->_taskBarHelper.setStatus(newStatus);
}

void MainWindow::setTaskbarIcon(const QString &iconSource)
{
  this->_taskBarHelper.setIcon(iconSource);
}

void MainWindow::onLanguageChanged()
{
  this->_taskBarHelper.setIcon(iconSource);
}

void MainWindow::onLanguageChanged()
{
  this->_keyboardLayoutHelper.update();
}

bool MainWindow::silent()
{
  return this->_silentMode.isEnabled();
}

void MainWindow::switchClientVersion()
{
  this->_applicationProxy->switchClientVersion();
}
