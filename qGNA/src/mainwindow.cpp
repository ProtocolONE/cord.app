#include <mainwindow.h>
#include <Player.h>
#include <BestInstallPath.h>

#include <viewmodel/UpdateViewModel.h>
#include <viewmodel/ApplicationStatisticViewModel.h>

#include <Host/Dbus/DbusConnection.h>
#include <Host/Dbus/DownloaderBridgeProxy.h>
#include <Host/Dbus/DownloaderSettingsBridgeProxy.h>
#include <Host/Dbus/ServiceSettingsBridgeProxy.h>
#include <Host/Dbus/ExecutorBridgeProxy.h>
#include <Host/Dbus/ApplicationBridgeProxy.h>
#include <Host/Dbus/ApplicationStatisticBridgeProxy.h>

#include <Core/UI/Message>
#include <Core/Marketing.h>
#include <Core/System/FileInfo.h>
#include <Core/System/HardwareId.h>

#include <RestApi/Commands/Service/GetLicense.h>
#include <RestApi/Commands/Service/GetServices.h>
#include <RestApi/Commands/User/GetUserMainInfo>
#include <RestApi/RequestFactory.h>

#include <Application/WindowHelper.h>

#include <Settings/settings.h>

#include <HookEngine/HookEngine.h>

#include <QtCore/QTranslator>
#include <QtCore/QSysInfo>
#include <QtCore/QFlags>
#include <QtGui/QBoxLayout>
#include <QtGui/QDesktopWidget>
#include <QtDeclarative/QDeclarativeProperty>

#define SIGNAL_CONNECT_CHECK(X) { bool result = X; Q_ASSERT_X(result, __FUNCTION__ , #X); }

using GameNet::Host::Bridge::Credential;
using GameNet::Host::DBus::DBusConnection;

Credential createDbusCredential(const GGS::RestApi::GameNetCredential& credential) 
{
  Credential result;
  result.userId = credential.userId();
  result.appKey = credential.appKey();
  result.cookie = credential.cookie();
  return result;
}

MainWindow::MainWindow(QWidget *parent) 
  : QMainWindow(parent)
  , _gameArea(GGS::Core::Service::Live)
  , _downloader(nullptr)
  , _downloaderSettings(nullptr)
  , _serviceSettings(nullptr)
  , _executor(nullptr)
{
  this->hide();
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

  this->_applicationProxy = new ApplicationBridgeProxy(dbusService, "/application", connection, this);
  this->_downloader = new DownloaderBridgeProxy(dbusService, "/downloader", connection, this);
  this->_downloaderSettings = new DownloaderSettingsBridgeProxy(dbusService, "/downloader/settings", connection, this);
  this->_serviceSettings = new ServiceSettingsBridgeProxy(dbusService, "/serviceSettings", connection, this);
  this->_executor = new ExecutorBridgeProxy(dbusService, "/executor", connection, this);
  this->_applicationStatistic = new ApplicationStatisticBridgeProxy(dbusService, "/applicationStatistic", connection, this);


  this->_bestInstallPath = new BestInstallPath(this);
  this->_bestInstallPath->setServiceSettings(this->_serviceSettings);

  this->initializeUpdateSettings();
  this->initRestApi();

  this->_commandLineArguments.parse(QCoreApplication::arguments());

  if (this->_commandLineArguments.contains("gamepts"))
    this->_gameArea = GGS::Core::Service::Pts;

  if (this->_commandLineArguments.contains("gametest"))
    this->_gameArea = GGS::Core::Service::Tst;

  this->setFileVersion(GGS::Core::System::FileInfo::version(QCoreApplication::applicationFilePath())); 
  this->setWindowTitle("qGNA " + this->_fileVersion);

  this->setWindowFlags(Qt::Window 
    | Qt::FramelessWindowHint 
    | Qt::WindowMinimizeButtonHint 
    | Qt::WindowSystemMenuHint); //Этот код уберет все внешние элементы формы

  this->translatorsParse();

  GGS::Settings::Settings settings;
  this->selectLanguage(settings.value("qGNA/language").toString());
  this->checkDesktopDepth();

  this->settingsViewModel = new SettingsViewModel(this);
  this->settingsViewModel->setDownloaderSettings(this->_downloaderSettings);
  
  this->initAutorun();

  qmlRegisterType<UpdateViewModel>("qGNA.Library", 1, 0, "UpdateViewModel");
  qmlRegisterType<Player>("qGNA.Library", 1, 0, "Player");
  qmlRegisterType<GGS::Core::UI::Message>("qGNA.Library", 1, 0, "Message");
  qmlRegisterType<ApplicationStatisticViewModel>("qGNA.Library", 1, 0, "ApplicationStatistic");

  qmlRegisterUncreatableType<GGS::Downloader::DownloadResultsWrapper>("qGNA.Library", 1, 0,  "DownloadResults", "");
  qmlRegisterUncreatableType<GGS::UpdateSystem::UpdateInfoGetterResultsWrapper>("qGNA.Library", 1, 0,  "UpdateInfoGetterResults", "");

  qRegisterMetaType<Features::Thetta::ThettaInstaller::Result>("Features::Thetta::ThettaInstaller::Result");

  this->initMarketing();
  
  //next 2 lines QGNA-60
  this->nQMLContainer = new MQDeclarativeView(this);
  SIGNAL_CONNECT_CHECK(connect(nQMLContainer, SIGNAL(leftMouseClick(int, int)), this, SIGNAL(leftMouseClick(int, int))));

  QStringList importPaths;
  importPaths << ":/";
  importPaths << (QCoreApplication::applicationDirPath() + "/plugins/");
  this->nQMLContainer->engine()->setImportPathList(importPaths);

  QStringList pluginsPath;
  pluginsPath << (QCoreApplication::applicationDirPath() + "/plugins/");
  pluginsPath << "./plugins";
  pluginsPath << QCoreApplication::applicationDirPath() + "/plugins/QtWebKit";
  this->nQMLContainer->engine()->setPluginPathList(pluginsPath);

  this->loadPlugin("QmlExtensionX86");
  this->loadPlugin("QmlOverlayX86");
  this->loadPlugin("qxmpp-declarative");

  this->nQMLContainer->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  
  this->_enterNickViewModel = new EnterNickNameViewModel(this);

  SIGNAL_CONNECT_CHECK(QObject::connect(&this->_restapiManager, SIGNAL(genericError(GGS::RestApi::CommandBase::Error, QString)), 
    this, SLOT(restApiGenericError(GGS::RestApi::CommandBase::Error, QString))));

  messageAdapter = new QmlMessageAdapter(this);
  
  this->_gameSettingsViewModel = new GameSettingsViewModel(this);
  this->_gameSettingsViewModel->setDownloader(this->_downloader);
  this->_gameSettingsViewModel->setServiceSettings(this->_serviceSettings);
  
  // HACK - уточнить что это полезно и зачем это
  nQMLContainer->setAttribute(Qt::WA_OpaquePaintEvent);
  nQMLContainer->setAttribute(Qt::WA_NoSystemBackground);
  nQMLContainer->viewport()->setAttribute(Qt::WA_OpaquePaintEvent);
  nQMLContainer->viewport()->setAttribute(Qt::WA_NoSystemBackground);
  // END of HACK
  
  nQMLContainer->rootContext()->setContextProperty("keyboardHook", &this->_keyboardLayoutHelper);
  nQMLContainer->rootContext()->setContextProperty("mainWindow", this);
  nQMLContainer->rootContext()->setContextProperty("installPath", "file:///" + QCoreApplication::applicationDirPath() + "/");
  nQMLContainer->rootContext()->setContextProperty("settingsViewModel", settingsViewModel);
  nQMLContainer->rootContext()->setContextProperty("messageBox", messageAdapter);
  nQMLContainer->rootContext()->setContextProperty("enterNickNameViewModel", this->_enterNickViewModel);
  nQMLContainer->rootContext()->setContextProperty("gameSettingsModel", this->_gameSettingsViewModel);
  
  nQMLContainer->setSource(QUrl("qrc:/Main.qml"));
  nQMLContainer->setAlignment(Qt::AlignCenter);
  nQMLContainer->setResizeMode(QDeclarativeView::SizeRootObjectToView);

  QObject *item = nQMLContainer->rootObject();
  QDeclarativeItem *rootItem = qobject_cast<QDeclarativeItem*>(item);

  SIGNAL_CONNECT_CHECK(QObject::connect(item, SIGNAL(dragWindowPressed(int,int)), this, SLOT(onSystemBarPressed(int,int))));
  SIGNAL_CONNECT_CHECK(QObject::connect(item, SIGNAL(dragWindowReleased(int,int)), this, SLOT(onSystemBarReleased(int,int))));
  SIGNAL_CONNECT_CHECK(QObject::connect(item, SIGNAL(dragWindowPositionChanged(int,int)), this, SLOT(onSystemBarPositionChanged(int,int))));

  SIGNAL_CONNECT_CHECK(QObject::connect(this->nQMLContainer->engine(), SIGNAL(quit()), this, SLOT(onWindowClose())));
  connect(this, &MainWindow::quit, this, &MainWindow::onWindowClose);



  this->setCentralWidget(nQMLContainer);
  this->setAttribute(Qt::WA_TranslucentBackground); //Эти две строчки позволят форме становиться прозрачной 
  this->setStyleSheet("background:transparent;");
  this->setFixedSize(rootItem->width(), rootItem->height()); 
  this->setMediumAvatarUrl("file:///" + QCoreApplication::applicationDirPath() + "/" + "Assets/Images/avatar.png"); 

  Message::setAdapter(messageAdapter);

  if (!this->_commandLineArguments.contains("minimized")) 
    this->activateWindow();

  if (!this->_commandLineArguments.contains("startservice")) {
    QObject::connect(this, &MainWindow::updateFinished,
      &this->_rememberGameFeature, &RememberGameDownloading::update);
  }
  
  DWORD verion = GetVersion();
  int dwMajorVersion = (int)(LOBYTE(LOWORD(verion)));
  int dwMinorVersion = (int)(HIBYTE(LOWORD(verion)));

  QVariantMap params;
  params["windowsMajorVersion"] = dwMajorVersion;
  params["windowsMinorVersion"] = dwMinorVersion;

  GGS::Core::Marketing::send(GGS::Core::Marketing::AnyStartQGna, params);
  GGS::Core::Marketing::sendOnce(GGS::Core::Marketing::FirstRunGna);

  QObject::connect(this, SIGNAL(windowActivate()), &this->_keyboardLayoutHelper, SLOT(update()));

  this->_keyboardLayoutHelper.update();
}

void MainWindow::restartUIRequestSlot() 
{
  if (this->isVisible()) {
    GGS::Core::UI::Message::information(tr("INFO_CAPTION"), tr("UPDATE_FOUND_MESSAGE"), GGS::Core::UI::Message::Ok);
    this->_applicationProxy->restartApplication(false);
    return;
  }

  this->_applicationProxy->restartApplication(true);
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

  return QMainWindow::winEvent(message, result);
}

void MainWindow::setTechName(QString& techName){
  if (this->_techName != techName){
    this->_techName = techName;
    emit this->techNameChanged();
    emit this->techNameValueChanged(this->_techName);
  }
}

void MainWindow::setNickName(QString& nickName) {
  if (this->_nickName != nickName) {
    this->_nickName = nickName;
    emit this->nickNameChanged();
    emit this->nickNameValueChanged(this->_nickName);
  }
}

void MainWindow::setMediumAvatarUrl(const QString& mediumAvatarUrl) {
  if (this->_mediumAvatarUrl != mediumAvatarUrl) {
    this->_mediumAvatarUrl = mediumAvatarUrl;
    emit this->mediumAvatarUrlChanged(); 
  }
}

void MainWindow::activateWindow()
{
  DEBUG_LOG << "activateWindow";
  // Это нам покажет окно
  this->setFocusPolicy(Qt::StrongFocus);
  this->setWindowState(Qt::WindowActive); 
  this->showNormal();

  // Эта функция активирует окно и поднмиает его повех всех окон
  GGS::Application::WindowHelper::activate(this->winId());

  this->repaint();
}

bool MainWindow::isDownloading(QString serviceId)
{
  return this->_downloader->isInProgress(serviceId);
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

    if (fileName.size() == 2) {  
      translators[fileName] = new QTranslator(this);
      // TODO да да, знаю, сделаю красиво позже.
      translators[fileName]->load(fileInfo.fileName(), QCoreApplication::applicationDirPath() + "/Languages/"); 
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
  move(pos() + (QPoint(MouseX,MouseY) - mLastMousePosition));
}

void MainWindow::onWindowClose()
{
  DEBUG_LOG << "Shutting down";

  this->repaint();
  this->hide();
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
  using GGS::RestApi::Commands::User::GetUserMainInfo;
  GetUserMainInfo *getUserMainInfo = new GetUserMainInfo(this);

  QObject::connect(getUserMainInfo, &GetUserMainInfo::result,
    this, &MainWindow::userMainInfoResult);
  
  getUserMainInfo->execute();
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
  this->_applicationProxy->restartApplication(shouldStartWithSameArguments);
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

  this->setTechName(QString(""));
  this->setNickName(QString(""));

  this->_restapiManager.setCridential(this->_credential);
}

void MainWindow::prepairGameDownloader()
{
  QString root = QCoreApplication::applicationDirPath();

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

  QObject::connect(this->_downloader, &DownloaderBridgeProxy::stopping, 
    this, &MainWindow::gameDownloaderStopping);

  QObject::connect(this->_downloader, &DownloaderBridgeProxy::failed, 
    this, &MainWindow::gameDownloaderFailed);

  QObject::connect(this->_downloader, &DownloaderBridgeProxy::statusMessageChanged, 
    this, &MainWindow::gameDownloaderStatusMessageChanged);

  QObject::connect(this->_downloader, &DownloaderBridgeProxy::serviceInstalled, 
    this, &MainWindow::gameDownloaderServiceInstalled);
  
  QObject::connect(this->_downloader, &DownloaderBridgeProxy::serviceUpdated, 
    this, &MainWindow::gameDownloaderServiceUpdated);

  QObject::connect(this->_downloader, &DownloaderBridgeProxy::started,
    &this->_rememberGameFeature, &RememberGameDownloading::started);

  QObject::connect(this->_downloader, &DownloaderBridgeProxy::finished,
    &this->_rememberGameFeature, &RememberGameDownloading::finished);

  QObject::connect(
    &this->_rememberGameFeature, &RememberGameDownloading::startGameRequest,
    this, &MainWindow::downloadButtonStart);
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
  if (args.status == static_cast<int>(GGS::Libtorrent::EventArgs::ProgressEventArgs::CheckingFiles)) {
      emit this->rehashProgressChanged(serviceId, progress, args.progress * 100);
      return;
  }

  emit this->downloadProgressChanged(serviceId,
    progress,
    args.totalWantedDone, 
    args.totalWanted, 
    args.directTotalDownload, 
    args.peerTotalDownload, 
    args.payloadTotalDownload, 
    args.peerPayloadDownloadRate, 
    args.payloadDownloadRate, 
    args.directPayloadDownloadRate, 
    args.payloadUploadRate, 
    args.totalPayloadUpload);
}

void MainWindow::gameDownloaderStarted(const QString& serviceId, int startType)
{
  // GGS::GameDownloader::StartType
  emit this->downloaderStarted(serviceId);
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

  this->activateWindow();
  emit showLicense(serviceId);
}

void MainWindow::downloadButtonPause(QString serviceId)
{
  if (this->_serviceSettings->isDownloadable(serviceId)) {
    this->_downloader->stop(serviceId);
    return;
  }

  GGS::RestApi::GameNetCredential baseCredential = 
    GGS::RestApi::RestApiManager::commonInstance()->credential();
    
  this->_executor->execute(serviceId, createDbusCredential(baseCredential));
}

bool MainWindow::isLicenseAccepted(const QString& serviceId)
{
  QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);
  settings.beginGroup(serviceId);
  QString hash = settings.value("LicenseHash", "").toString();

  if (serviceId == "100009010000000000" || serviceId == "100003010000000000")  // TMP
    hash = "fake_hash";

  if (!hash.isEmpty()) {
    return true;
  }

  return false;
}

void MainWindow::startGame(const QString& serviceId)
{
  if (this->_executor->isGameStarted(serviceId))
    return;

  if (this->_serviceSettings->isDownloadable(serviceId)) {
    this->_downloader->start(serviceId, static_cast<int>(GGS::GameDownloader::Normal));
    return;
  } 
  
  bool isAuthed = !this->_restapiManager.credential().userId().isEmpty();
  if (!isAuthed) {
    emit this->authBeforeStartGameRequest(serviceId);
    return;
  }

  GGS::RestApi::GameNetCredential baseCredential = 
    GGS::RestApi::RestApiManager::commonInstance()->credential();

  this->_executor->execute(serviceId, createDbusCredential(baseCredential));
}

void MainWindow::commandRecieved(QString name, QStringList arguments)
{
  if (name == "quit") {
    this->onWindowClose();
    return;
  } 

  if (name == "settings") {
    emit this->navigate("ApplicationSettings");
    return;
  } 

  if (name == "activate") {
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
    emit this->needAuth();
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

void MainWindow::closeEvent(QCloseEvent* event)
{
  this->hide();
  event->ignore();
}

void MainWindow::gameDownloaderStatusMessageChanged(const QString& serviceId, const QString& message)
{
  emit this->downloaderServiceStatusMessageChanged(serviceId, message);
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
  if (hour >= 14)
    return 10800000;

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

    if (!this->_executor->isAnyGameStarted()
      && !this->_downloader->isAnyServiceInProgress()) {

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
  using GGS::RestApi::CommandBase;
  switch(error) {
  case CommandBase::AuthorizationFailed: // break пропущен не спроста
  case CommandBase::AccountNotExists:
  case CommandBase::AuthorizationLimitExceed:
  case CommandBase::UnknownAccountStatus:
    emit this->needAuth();
    break;
  }
}

void MainWindow::showEvent(QShowEvent* event)
{
  this->_taskBarHelper.prepare(this->effectiveWinId());
  emit this->taskBarButtonMsgRegistered(this->_taskBarHelper.getTaskbarCreatedMessageId());
  
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

void MainWindow::gameDownloaderServiceInstalled(const QString& serviceId)
{
  emit this->serviceInstalled(serviceId);
}

void MainWindow::gameDownloaderServiceUpdated(const QString& serviceId)
{
  this->activateWindow();
  emit this->selectService(serviceId);
}

void MainWindow::initAutorun()
{
  int autoStart = this->settingsViewModel->autoStart();
  this->settingsViewModel->addToAutoStart(autoStart == 1 || autoStart == 2, autoStart == 2);
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

bool MainWindow::anyLicenseAccepted()
{
  QStringList ids;
  ids << "300002010000000000"
    << "300003010000000000"
    << "300004010000000000"
    << "300005010000000000"
    << "300009010000000000"
    << "300012010000000000";

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
  using GGS::Core::Marketing;

  QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);
  settings.beginGroup(serviceId);
  Marketing::sendInstallerStepOnce(Marketing::InstallAcceptLicense);
  settings.setValue("LicenseHash", "1");

  this->initFinished(); 
}

void MainWindow::initFinished()
{
  emit this->updateFinished();
}

void MainWindow::initRestApi()
{
  //Port selection due to https://jira.gamenet.ru:8443/browse/QGNA-285

  QStringList ports;
  ports << "443" << "7443" << "8443" << "9443" << "10443" << "11443";
  QString randomPort = ports.takeAt(qrand() % ports.count());
  QString apiUrl = QString("https://gnapi.com:%1/restapi").arg(randomPort);

  GGS::Settings::Settings settings;
  settings.setValue("qGNA/restApi/url", apiUrl);

  qDebug() << "Using rest api url " << apiUrl;

  this->_restapiManager.setUri(apiUrl);
  this->_restapiManager.setRequest(GGS::RestApi::RequestFactory::Http);
  this->_restapiManager.setCache(&_fakeCache);
  GGS::RestApi::RestApiManager::setCommonInstance(&this->_restapiManager);
}

bool MainWindow::event(QEvent* event) 
{
  switch(event->type()) {
  case QEvent::WindowActivate:
    emit this->windowActivate();
    break;
  case QEvent::WindowDeactivate:
    emit this->windowDeactivate();
    break;
  }

  return QMainWindow::event(event);
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

void MQDeclarativeView::mousePressEvent(QMouseEvent* event){
  if (event->buttons() & Qt::LeftButton)
    emit this->leftMouseClick(event->x(), event->y()); 

  QDeclarativeView::mousePressEvent(event);
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
  } else if (status == "Paused") {
    newStatus = TaskBarHelper::StatusPaused;
  } else if (status == "Error") {
    newStatus = TaskBarHelper::StatusError;
  }

  this->_taskBarHelper.setProgress(progressValue);
  this->_taskBarHelper.setStatus(newStatus);
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