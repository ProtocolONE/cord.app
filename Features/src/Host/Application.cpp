#include <Host/Connection.h>

#include <Host/ApplicationStatistic.h>
#include <Host/ApplicationRestarter.h>
#include <Host/Application.h>
#include <Host/Updater.h>
#include <Host/ServiceProcess/ServiceLoader.h>
#include <Host/ServiceSettings.h>
#include <Host/ServiceProcess/ServiceDescription.h>
#include <Host/GameExecutor.h>
#include <Host/StopDownloadOnExecuteInit.h>
#include <Host/ShutdownManager.h>
#include <Host/HookFactory.h>
#include <Host/ExecutorHookFactory.h>
#include <Host/DownloaderSettings.h>
#include <Host/UIProcess.h>
#include <Host/ApplicationStatistic.h>
#include <Host/MarketingStatistic.h>
#include <Host/CommandLineManager.h>
#include <Host/MessageAdapter.h>
#include <Host/Translation.h>
#include <Host/ConnectionManager.h>
#include <Host/ServiceHandle.h>
#include <Host/AutoRunManager.h>
#include <Host/ServiceProcess/ServicesListRequest.h>
#include <Host/Installer/UninstallResult.h>
#include <Host/LicenseManager.h>

#include <Host/GameDownloader/DownloaderSettingsRoulette.h>

#include <Host/Dbus/DBusServer.h>

#include <QtYaml/ConfigManager.h>

#include <Features/GameDownloader/GameDownloadStatistics.h>
#include <Features/StopDownloadServiceWhileExecuteAnyGame.h>
#include <Features/Marketing/SystemInfo/SystemInfoManager.h>

#include <Features/PlainFileCache.h>

#include <GameDownloader/GameDownloadService.h>

#include <Application/SingleApplication.h>

#include <RestApi/RestApiManager.h>
#include <RestApi/FakeCache.h>

#include <Settings/Settings.h>

#include <Helper/TerminateProcess.h>
#include <Helper/SystemInfo.h>

#include <Core/Service.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QMetaType>
#include <QtCore/QUrl>
#include <QtCore/QMutexLocker>
#include <QtCore/QCryptographicHash>
#include <QtDBus/QDBusError>

using P1::Application::SingleApplication;
using P1::GameDownloader::GameDownloadService;
using P1::Host::DBus::DBusServer;
using P1::Host::Installer::UninstallResult;

namespace P1 {
  namespace Host {

    Application::Application(QObject *parent /*= 0*/)
      : _singleApplication(nullptr)
      , _shutdown(new ShutdownManager(this))
      , _serviceLoader(new ServiceProcess::ServiceLoader(this))
      , _gameDownloader(new GameDownloadService(this))
      , _downloaderSettings(new DownloaderSettings(this))
      , _serviceSettings(new ServiceSettings(this))
      , _downloadStatistics(new Features::GameDownloader::GameDownloadStatistics(this))
      , _executor(new GameExecutor(this))
      , _stopDownloadServiceOnExecuteGame(new Features::StopDownloadServiceWhileExecuteAnyGame(this))
      , _restApiManager(new P1::RestApi::RestApiManager(this))
      , _restApiCache(new P1::RestApi::FakeCache())
      , _updater(new Updater(this))
      , _uiProcess(new UIProcess(this))
      , _applicationRestarter(new ApplicationRestarter(this))
      , _downloaderHookFactory(new HookFactory(this))
      , _executorHookFactory(new ExecutorHookFactory(this))
      , _applicationStatistic(new ApplicationStatistic(this))
      , _marketingStatistic(new MarketingStatistic(this))
//      , _marketingTarget(new P1::Marketing::MarketingTarget(this))
      , _commandLineManager(new CommandLineManager(this))
      , _messageAdapter(new MessageAdapter(this))
      , _translation(new Translation(this))
      , _connectionManager(new ConnectionManager(this))
      , _serviceHandle(new ServiceHandle(this))
      , _autoRunManager(new AutoRunManager(this))
      , _dbusServer(nullptr)
      , _servicesListRequest(new ServiceProcess::ServicesListRequest(this))
      , _systemInfoManager(new Features::Marketing::SystemInfo::SystemInfoManager(this))
      , _initFinished(false)
      , _updateFinished(false)
      , _closing(false)
      , _licenseManager(new LicenseManager(this))
      , _configManager(new P1::QtYaml::ConfigManager)
      , QObject(parent)
    {

    }

    Application::~Application()
    {
      delete this->_restApiCache;
      delete this->_configManager;
    }

    void Application::setInitFinished() 
    {
      if (this->_initFinished)
        return;

      this->_initFinished = true;
     
      this->sendInitFinished();
    }

    void Application::setUpdateFinished() 
    {
      if (this->_updateFinished)
        return;

      this->_updateFinished = true;

      this->sendInitFinished();
    }

    void Application::sendInitFinished()
    {
      if (!this->_initFinished || !this->_updateFinished)
        return;

      this->_licenseManager->setServices(this->_serviceLoader->servicesMap().keys());
      emit this->initCompleted();
    }

    bool Application::isInitCompleted()
    {
      return this->_initFinished && this->_updateFinished;
    }

    void Application::setSingleApplication(SingleApplication *value)
    {
      Q_ASSERT(value);
      this->_singleApplication = value;

      QObject::connect(value, &SingleApplication::forceQuit, 
        this, &Application::internalShutdown, Qt::DirectConnection);
      
      QObject::connect(value, &SingleApplication::commandRecieved, 
        this->_commandLineManager, &CommandLineManager::commandRecieved, Qt::QueuedConnection);

      QObject::connect(this, &Application::initCompleted, 
        value, &SingleApplication::initializeFinished);
    }

    void Application::init()
    {

#ifndef USE_SESSION_DBUS
      this->_dbusServer = new DBusServer(this);
      if (!this->_dbusServer->isConnected()) {
        qDebug() << "DbusServer failed: " << this->_dbusServer->lastError();
        MessageBoxW(0, L"Could not create DBusServer.", L"Error", MB_OK);
        QCoreApplication::quit();
        return;
      }
#endif

      // INFO Необходимо инициализировать первым. Там определяется первый ли раз запукается приложение.
      this->_applicationStatistic->init();

      QString path = QCoreApplication::applicationDirPath();
      path += "/Config.yaml";
      if (!this->_configManager->load(path))
        qWarning() << "Cannot read application config file: " << path;

      this->_connectionManager->setStopDownloadServiceWhileExecuteAnyGame(this->_stopDownloadServiceOnExecuteGame);

      this->_messageAdapter->setHasUiProcess(std::bind(&ConnectionManager::hasLauncher, this->_connectionManager));
      QObject::connect(this->_uiProcess, &UIProcess::closed, this->_messageAdapter, 
        &MessageAdapter::uiProcessClosed);

      QObject::connect(this, &Application::restartApplicationRequest,
        this, &Application::internalRestartApplication, Qt::QueuedConnection);

      QObject::connect(this, &Application::internalShutdownUIResult,
        this, &Application::internalShutdown, Qt::QueuedConnection);

      P1::Core::UI::Message::setAdapter(this->_messageAdapter);
      
      this->initRestApi();
      this->_translation->init();
      this->_autoRunManager->init();

      this->initGameDownloader();

      this->_servicesListRequest->setServiceLoader(this->_serviceLoader);

      if (this->_commandLineManager->forceElectronExecutor())
        this->_servicesListRequest->setOverrideWebExecutor(true);

      if (this->_commandLineManager->forceWebBrowserExecutor())
        this->_servicesListRequest->setOverrideWebExecutor(false);

      this->_applicationStatistic->setDownloader(this->_gameDownloader);
      this->_applicationStatistic->setStartingGame(this->_commandLineManager->startingService());
      QObject::connect(this, &Application::initCompleted,
        this->_applicationStatistic, &ApplicationStatistic::applcationStarted);

      this->_executor->setServices(this->_serviceLoader);
      this->_executor->setServiceSettings(this->_serviceSettings);
      this->_executor->init();

      this->_downloaderHookFactory->setServiceLoader(this->_serviceLoader);
      this->_downloaderHookFactory->setServiceHandle(this->_serviceHandle);
      this->_downloaderHookFactory->setServiceSettings(this->_serviceSettings);

      this->_executorHookFactory->setDownloader(this->_gameDownloader);
      this->_executorHookFactory->setExecutor(this->_executor->mainExecutor());
      this->_executorHookFactory->setDownloaderHookFactory(this->_downloaderHookFactory);
      this->_executorHookFactory->init();

      this->_serviceLoader->setApplicationPath(QCoreApplication::applicationDirPath());
      this->_serviceLoader->setGameArea(this->_commandLineManager->gameArea());
      this->_serviceLoader->setDownloader(this->_gameDownloader);
      this->_serviceLoader->setExecutor(this->_executor->mainExecutor());
      this->_serviceLoader->setDownloaderHookFactory(this->_downloaderHookFactory);
      this->_serviceLoader->setExecuterHookFactory(this->_executorHookFactory);

      this->_serviceSettings->setServices(this->_serviceLoader);
      this->_serviceSettings->setDownloader(this->_gameDownloader);

      this->initMarketing();

      this->_shutdown->setApplication(this);
      this->_shutdown->setDownloader(this->_gameDownloader);
      this->_shutdown->setExecutor(this->_executor);
      this->_shutdown->setSingleApplication(this->_singleApplication);
      this->_shutdown->setConnectionManager(this->_connectionManager);
      
      this->_applicationRestarter->setShutdownManager(this->_shutdown);
      
      this->_uiProcess->setDirectory(QCoreApplication::applicationDirPath());
      this->_uiProcess->setFileName("launcher.ui.exe");

      QObject::connect(this->_servicesListRequest, &ServiceProcess::ServicesListRequest::finished, [this](){
        StopDownloadOnExecuteInit stopDownloadOnExecuteInit;
        stopDownloadOnExecuteInit.setDownloader(this->_gameDownloader);
        stopDownloadOnExecuteInit.setServices(this->_serviceLoader);
        stopDownloadOnExecuteInit.setTarget(this->_stopDownloadServiceOnExecuteGame);
        stopDownloadOnExecuteInit.setExecutor(this->_executor);
        stopDownloadOnExecuteInit.init();
      });

      QObject::connect(this->_servicesListRequest, &ServiceProcess::ServicesListRequest::finished,
        this, &Application::setInitFinished);

      QObject::connect(this->_servicesListRequest, &ServiceProcess::ServicesListRequest::additionalResourcesReady,
        this, &Application::additionalResourcesReady);

      QObject::connect(this->_commandLineManager, &CommandLineManager::shutdown,
        this, &Application::shutdown);

      QObject::connect(this->_commandLineManager, &CommandLineManager::uninstallService, 
        this, &Application::onUninstallRequestSlot);

      QObject::connect(this->_commandLineManager, &CommandLineManager::updateRequested,
        [this](){
        this->updateCompletedSlot(true);
      });

      this->_commandLineManager->setExecutedGameCredential(
        std::bind(&Application::executedGameCredential, this, std::placeholders::_1, std::placeholders::_2));

      if (!this->registerDbusServices())
        return;

      this->startUi();
      
      QObject::connect(this->_updater, &Updater::allCompleted, this, &Application::updateCompletedSlot);

      this->_updater->setCanRestart([this] () -> bool {
        return !this->_executor->isAnyGameStarted() && 
          !this->_gameDownloader->isAnyServiceInProgress();
      });

      this->_updater->startCheckUpdate();
    }

    void Application::updateCompletedSlot(bool needRestart)
    {
      if (needRestart) {
        if (!this->isInitCompleted() || !this->_connectionManager->hasLauncher()) {
          this->restartApplication(true, false);
          return;
        }

        emit this->restartUIRequest();
      }

      if (!this->_initFinished) {
        this->_servicesListRequest->request();
        this->setUpdateFinished();
      }
    }

    void Application::onUninstallRequestSlot(const QString &serviceId)
    {
      UninstallResult *uninstallResult = new UninstallResult(serviceId, this);
      QObject::connect(this->_gameDownloader, &GameDownloadService::serviceUninstalled, uninstallResult, &UninstallResult::onUninstallFinished);
      QObject::connect(this->_gameDownloader, &GameDownloadService::failed, uninstallResult, &UninstallResult::onUninstallFailed);
      QObject::connect(this, &Application::internalCancelUninstallRequest, uninstallResult, &UninstallResult::onUninstallCancelled);

      emit this->uninstallServiceRequest(serviceId);
   }

    bool Application::registerDbusServices()
    {
#ifndef USE_SESSION_DBUS
      this->_connectionManager->setDbusServer(this->_dbusServer);
#endif
      this->_connectionManager->setApplication(this);

      if (!this->_connectionManager->init()) {
        MessageBoxW(0, L"Could not create DBusServer.", L"Error", MB_OK);
        this->shutdown();
        return false;
      }

      QObject::connect(this->_connectionManager, &ConnectionManager::newConnection, 
        this, &Application::onNewConnection);

      return true;
    }

    void Application::initGameDownloader()
    {
      this->_gameDownloader->setListeningPort(this->_downloaderSettings->listeningPort());
      this->_gameDownloader->init();
      
      this->_downloaderSettings->setDownloader(this->_gameDownloader);
      this->_downloaderSettings->init();
      
      this->_downloadStatistics->init(this->_gameDownloader);
      this->_shutdown->setGameDownloadInitialized();
    }
    
    void Application::initRestApi()
    {
      QString apiUrl = this->_configManager->value<QString>("api\\url", "https://api.tst.protocol.one/");
      qDebug() << "Using RestApi url " << apiUrl;

      this->_restApiManager->setUri(apiUrl);
      this->_restApiManager->setCache(new Features::PlainFileCache(this->_restApiManager));
      
      bool debugLogEnabled = this->_configManager->value<bool>("api\\debug", false);
      this->_restApiManager->setDebugLogEnabled(debugLogEnabled);

      P1::RestApi::RestApiManager::setCommonInstance(this->_restApiManager);
    }

    void Application::switchClientVersion()
    {
      P1::ApplicationArea area;
      area.load();

      area = area.isLive()
        ? P1::ApplicationArea::SupportedArea::Pts
        : P1::ApplicationArea::SupportedArea::Live;

      area.save();

      this->restartApplication(true, false);
    }

    void Application::cancelUninstallServiceRequest(const QString &serviceId)
    {
      emit this->internalCancelUninstallRequest(serviceId);
    }

    void Application::restartApplication(bool shouldStartWithSameArguments, bool isMinimized)
    {
      // INFO Необходимо выйти из запроса на рестарт приложения, 
      // иначе dbus вызвавший метод упадет на попытке вернуть результат
      emit this->restartApplicationRequest(shouldStartWithSameArguments, isMinimized);
    }

    void Application::finalize()
    {
      this->_shutdown->finalize();
    }

    void Application::shutdown()
    {
      if (!this->isInitCompleted() || !this->_connectionManager->hasLauncher()) {
        this->internalShutdown();
        return;
      }

      emit shutdownUIRequest();
    }

    void Application::shutdownUIResult()
    {
      // INFO В Qt 5.4 заменить на QTimer::singleShot
      emit this->internalShutdownUIResult();
    }

    void Application::startUi()
    {
      if (this->_commandLineManager->skipUi()) {
        this->setUiCommandConnection();
        return;
      }

      qDebug() << "Starting Launcher UI";
      QStringList args = QCoreApplication::arguments();
      args.removeFirst(); // INFO first argument always self execute path

      this->_uiProcess->destroyProcess();
      this->_uiProcess->start(args);
    }

    void Application::initMarketing()
    {
      //QSettings midSettings(
      //  QSettings::NativeFormat,
      //  QSettings::UserScope,
      //  QCoreApplication::organizationName(),
      //  QCoreApplication::applicationName());

      //QString mid = midSettings.value("MID", "").toString();
      //this->_marketingTarget->init("Launcher", mid);

      //int installerKey = midSettings.value("InstKey").toInt();
      //this->_marketingTarget->setInstallerKey(installerKey);
      //this->_marketingTarget->setRequestInterval(1000);

      //this->_systemInfoManager->setMid(mid);

      this->_systemInfoManager->init();
    }

    bool Application::executedGameCredential(P1::RestApi::ProtocolOneCredential& credetial, QString& name)
    {
      QString executedGame = this->_executor->executedGame();
      if (executedGame.isEmpty())
        return false;

      Connection *connection = this->_serviceHandle->connectionLockedService(executedGame);
      if (!connection)
        return false;

      credetial = connection->credential();
      name = connection->applicationName();
      return true;
    }

    void Application::internalRestartApplication(bool shouldStartWithSameArguments, bool isMinimized)
    {
      this->_uiProcess->closeUI();
      this->_applicationRestarter->restartApplication(shouldStartWithSameArguments, isMinimized);
    }

    void Application::internalShutdown()
    {
      {
        QMutexLocker locker(&this->_closeMutex);
        if (this->_closing)
          return;

        this->_closing = true;
      }

      this->_uiProcess->closeUI();

      QObject::connect(this->_shutdown, &ShutdownManager::shutdownCompleted,
        []() {
          QCoreApplication::quit();
      });

      this->_shutdown->shutdown();
    }

    void Application::onNewConnection(Connection *connection)
    {
      QObject::connect(connection, &Connection::logoutMain,
        this, &Application::onConnectionLogoutMain);

      if (connection->applicationName() == "Launcher") {
        this->setUiCommandConnection();

        auto launcherLogout = [this]() {
          this->_systemInfoManager->setCredential(P1::RestApi::ProtocolOneCredential());
          this->_gameDownloader->resetCredentials();
        };

        QObject::connect(connection, &Connection::logoutMain, launcherLogout);
        QObject::connect(connection, &Connection::disconnected, launcherLogout);
        QObject::connect(connection, &Connection::mainCredentialChanged, [this, connection]() {
          P1::RestApi::RestApiManager::commonInstance()->setCridential(connection->credential());
          this->_systemInfoManager->setCredential(connection->credential());
          this->setDownloaderCredential(connection->credential());
        });
      }
    }

    void Application::setDownloaderCredential(const P1::RestApi::ProtocolOneCredential &creds)
    {
      QString userId = creds.userId();

      QCryptographicHash hash(QCryptographicHash::Sha1);
      hash.addData(userId.toLatin1());
      hash.addData("t5mPWw25K8FIpZaQ");
      
      QString authHash(hash.result().toHex());

      this->_gameDownloader->setCredentials(userId, authHash);
    }

    void Application::setUiCommandConnection()
    {
      QObject::connect(this->_commandLineManager, &CommandLineManager::uiCommand,
        this->_uiProcess, &UIProcess::sendCommand, Qt::UniqueConnection);
    }

    void Application::onConnectionLogoutMain()
    {
      Connection *connection = qobject_cast<Connection *>(QObject::sender());
      if (!connection)
        return;

      QList<QString> lockedGames = this->_serviceHandle->lockedServices(connection);
      Q_FOREACH(const QString& serviceId, lockedGames) {
        this->_executor->terminateAll(serviceId);
      }
    }

  }
}
