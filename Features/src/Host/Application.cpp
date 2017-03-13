#include <Host/Connection.h>

#include <Host/ApplicationStatistic.h>
#include <Host/ApplicationRestarter.h>
#include <Host/Application.h>
#include <Host/Updater.h>
#include <Host/ServiceProcess/ServiceLoader.h>
#include <Host/ServiceSettings.h>
#include <Host/ServiceProcess/ServiceDescription.h>
#include <Host/GameExecutor.h>
#include <Host/Thetta.h>
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
#include <Host/Installer/Migration.h>
#include <Host/Installer/UninstallResult.h>
#include <Host/LicenseManager.h>

#include <Host/Dbus/DBusServer.h>

#include <Integration/ZZima/ZzimaGameExecutor.h>
#include <Integration/ZZima/ZZimaConnection.h>

#include <Features/GameDownloader/GameDownloadStatistics.h>
#include <Features/StopDownloadServiceWhileExecuteAnyGame.h>
#include <Features/Thetta/ThettaInstaller.h>
#include <Features/Marketing/SystemInfo/SystemInfoManager.h>
#include <Features/Thetta/ModuleScanner.h>
#include <Features/Thetta/AppDistrIntegrity.h>
#include <Features/Thetta/KernelStatistics.h>

#include <GameDownloader/GameDownloadService.h>

#include <Application/SingleApplication.h>

#include <RestApi/RestApiManager.h>
#include <RestApi/FakeCache.h>

#include <Settings/Settings.h>

#include <Marketing/MarketingTarget.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QMetaType>
#include <QtCore/QUrl>
#include <QtCore/QMutexLocker>
#include <QtCore/QCryptographicHash>

using GGS::Application::SingleApplication;
using GGS::GameDownloader::GameDownloadService;
using ::GameNet::Integration::ZZima::ZZimaConnection;
using GameNet::Host::DBus::DBusServer;
using GameNet::Host::Installer::UninstallResult;

namespace GameNet {
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
      , _thetta(new Thetta(this))
      , _stopDownloadServiceOnExecuteGame(new Features::StopDownloadServiceWhileExecuteAnyGame(this))
      , _restApiManager(new GGS::RestApi::RestApiManager(this))
      , _restApiCache(new GGS::RestApi::FakeCache())
      , _updater(new Updater(this))
      , _uiProcess(new UIProcess(this))
      , _applicationRestarter(new ApplicationRestarter(this))
      , _downloaderHookFactory(new HookFactory(this))
      , _executorHookFactory(new ExecutorHookFactory(this))
      , _applicationStatistic(new ApplicationStatistic(this))
      , _marketingStatistic(new MarketingStatistic(this))
      , _marketingTarget(new GGS::Marketing::MarketingTarget(this))
      , _commandLineManager(new CommandLineManager(this))
      , _messageAdapter(new MessageAdapter(this))
      , _translation(new Translation(this))
      , _connectionManager(new ConnectionManager(this))
      , _serviceHandle(new ServiceHandle(this))
      , _zzimaConnection(new ZZimaConnection(this))
      , _autoRunManager(new AutoRunManager(this))
      , _dbusServer(nullptr)
      , _servicesListRequest(new ServiceProcess::ServicesListRequest(this))
      , _initFinished(false)
      , _updateFinished(false)
      , _closing(false)
      , _applicationDistrMon(new Features::Thetta::AppDistrIntegrity(this))
      , _licenseManager(new LicenseManager(this))
      , _kernelStat(new Features::Thetta::KernelStatistics(this))
      , QObject(parent)
    {

    }

    Application::~Application()
    {
      delete this->_restApiCache;
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

      if (!GameNet::Host::Installer::Migration::isMigrated()) {
        GameNet::Host::Installer::Migration installerMigration;
        installerMigration.setServices(this->_serviceLoader);
        installerMigration.setDownloader(this->_gameDownloader);
        installerMigration.migrate();
      }
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

      this->_connectionManager->setStopDownloadServiceWhileExecuteAnyGame(this->_stopDownloadServiceOnExecuteGame);

      this->_messageAdapter->setHasUiProcess(std::bind(&ConnectionManager::hasQGNA, this->_connectionManager));
      QObject::connect(this->_uiProcess, &UIProcess::closed, this->_messageAdapter, 
        &MessageAdapter::uiProcessClosed);

      QObject::connect(this, &Application::restartApplicationRequest,
        this, &Application::internalRestartApplication, Qt::QueuedConnection);

      QObject::connect(this, &Application::internalShutdownUIResult,
        this, &Application::internalShutdown, Qt::QueuedConnection);

      GGS::Core::UI::Message::setAdapter(this->_messageAdapter);

      this->initRestApi();
      this->_translation->init();
      this->_autoRunManager->init();

      this->initGameDownloader();

      this->_servicesListRequest->setServiceLoader(this->_serviceLoader);
      this->_servicesListRequest->setApplicationArea(this->_updater->applicationArea());

      this->_applicationStatistic->setDownloader(this->_gameDownloader);
      this->_applicationStatistic->setStartingGame(this->_commandLineManager->startingService());
      this->_applicationStatistic->init();

      this->_thetta->setApplication(this);
      this->_thetta->init();

      //Connect service termination
      QObject::connect(this->_thetta->scanner(), &Features::Thetta::ModuleScanner::scannerFoundModule,
        this->_executor, &GameNet::Host::GameExecutor::terminateAll);
      
      QObject::connect(this->_thetta, &Thetta::closeGamesRequest, 
        this->_executor, &GameNet::Host::GameExecutor::terminateAll);

      this->_executor->setServices(this->_serviceLoader);
      this->_executor->setServiceSettings(this->_serviceSettings);
      this->_executor->setThetta(this->_thetta);
      this->_executor->init();

      GameNet::Integration::ZZima::ZzimaGameExecutor * zzimaExecutor = 
        new GameNet::Integration::ZZima::ZzimaGameExecutor(this);
      zzimaExecutor->setZzimaConnection(this->_zzimaConnection);

      this->_executor->mainExecutor()->registerExecutor(zzimaExecutor);

      this->_downloaderHookFactory->setServiceLoader(this->_serviceLoader);
      this->_downloaderHookFactory->setServiceHandle(this->_serviceHandle);
      this->_downloaderHookFactory->setServiceSettings(this->_serviceSettings);
      this->_downloaderHookFactory->setZzimaConnection(this->_zzimaConnection);

      this->_executorHookFactory->setDownloader(this->_gameDownloader);
      this->_executorHookFactory->setExecutor(this->_executor->mainExecutor());
      this->_executorHookFactory->setDownloaderHookFactory(this->_downloaderHookFactory);
      this->_executorHookFactory->setThetta(this->_thetta);
      this->_executorHookFactory->init();

      this->_serviceLoader->setApplicationPath(QCoreApplication::applicationDirPath());
      this->_serviceLoader->setGameArea(this->_commandLineManager->gameArea());
      this->_serviceLoader->setDownloader(this->_gameDownloader);
      this->_serviceLoader->setExecutor(this->_executor->mainExecutor());
      this->_serviceLoader->setSimpleMainExecutor(this->_executor->simpleMainExecutor());
      this->_serviceLoader->setSecondExecutor(this->_executor->secondExecutor());
      this->_serviceLoader->setDownloaderHookFactory(this->_downloaderHookFactory);
      this->_serviceLoader->setExecuterHookFactory(this->_executorHookFactory);

      this->_serviceSettings->setServices(this->_serviceLoader);
      this->_serviceSettings->setDownloader(this->_gameDownloader);

      this->initMarketing();

      this->_shutdown->setApplication(this);
      this->_shutdown->setDownloader(this->_gameDownloader);
      this->_shutdown->setExecutor(this->_executor);
      this->_shutdown->setThetta(this->_thetta);
      this->_shutdown->setSingleApplication(this->_singleApplication);
      this->_shutdown->setConnectionManager(this->_connectionManager);

      this->_applicationRestarter->setShutdownManager(this->_shutdown);
      
      this->_uiProcess->setDirectory(QCoreApplication::applicationDirPath());
      this->_uiProcess->setFileName("gamenet.ui.exe");

      this->_applicationDistrMon->setExecutor(this->_executor->mainExecutor());
      this->_applicationDistrMon->setArea(this->_updater->applicationArea());

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

      QObject::connect(this->_commandLineManager, &CommandLineManager::openBrowser, 
        this->_thetta, &Thetta::openBrowser);

      QObject::connect(this->_commandLineManager, &CommandLineManager::updateRequested,
        [this](){
        QString updateArea = QString("live");
        QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\GGS\\QGNA", QSettings::NativeFormat);
        bool ok = false;
        int area = settings.value("Repository", 0).toInt(&ok);
        if (!ok)
          area = 0;

        this->_applicationDistrMon->setArea((GGS::Core::Service::Area)area);
        this->updateCompletedSlot(true);
      });

      QObject::connect(this, &Application::initCompleted,
        this->_applicationDistrMon, &Features::Thetta::AppDistrIntegrity::onAppStarted);

      QObject::connect(this->_applicationDistrMon, &Features::Thetta::AppDistrIntegrity::restartRequest,
        this, &Application::restartApplicationRequest);

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
        this->_applicationDistrMon->disable();
        if (!this->isInitCompleted() || !this->_connectionManager->hasQGNA()) {
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

      QObject::connect(this->_connectionManager, &ConnectionManager::zzimaDisabled, [this](){
        this->_thetta->openBrowser("https://support.gamenet.ru/kb/articles/1188-reborn-zzima");
      });

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
        QStringList ports;
        ports << "443" << "7443" << "8443" << "9443" << "10443" << "11443";
        QString randomPort = ports.takeAt(qrand() % ports.count());
        QString apiUrl = QString("https://gnapi.com:%1/restapi").arg(randomPort);

        GGS::Settings::Settings settings;
        settings.setValue("qGNA/restApi/url", apiUrl);
        qDebug() << "Using rest api url " << apiUrl;

        this->_restApiManager->setUri(apiUrl);
        this->_restApiManager->setRequest(GGS::RestApi::RequestFactory::Http);
        this->_restApiManager->setCache(this->_restApiCache);
        GGS::RestApi::RestApiManager::setCommonInstance(this->_restApiManager);
    }

    void Application::switchClientVersion()
    {
      QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\GGS\\QGNA", QSettings::NativeFormat);
      bool ok = false;
      int area = settings.value("Repository", 0).toInt(&ok);
      if (!ok)
        area = 0;

      area = area == 0 ? 1 : 0;

      settings.setValue("Repository", area);
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
      if (!this->isInitCompleted() || !this->_connectionManager->hasQGNA()) {
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

      qDebug() << "Starting qGNA UI";
      QStringList args = QCoreApplication::arguments();
      args.removeFirst(); // INFO first argument always self execute path
      
      this->_uiProcess->destroyProcess();
      this->_uiProcess->start(args);
    }

    void Application::initMarketing()
    {
      QSettings midSettings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);
      QString mid = midSettings.value("MID", "").toString();
      this->_marketingTarget->init("qGNA", mid);

      int installerKey = midSettings.value("InstKey").toInt();
      this->_marketingTarget->setInstallerKey(installerKey);
      this->_marketingTarget->setRequestInterval(1000);

      this->_systemInfoManager->setMid(mid);
      this->_systemInfoManager->init();
    }

    bool Application::executedGameCredential(GGS::RestApi::GameNetCredential& credetial, QString& name)
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

      if (connection->applicationName() == "QGNA") {
        this->setUiCommandConnection();

        if (this->_updater->applicationArea() == GGS::Core::Service::Tst)
          this->_thetta->installer()->connectToDriver();

        auto qgnaLogout = [this]() {
          this->_systemInfoManager->setCredential(GGS::RestApi::GameNetCredential());
          this->_gameDownloader->resetCredentials();
        };

        QObject::connect(connection, &Connection::logoutMain, qgnaLogout);
        QObject::connect(connection, &Connection::disconnected, qgnaLogout);
        QObject::connect(connection, &Connection::mainCredentialChanged, [this, connection]() {
          GGS::RestApi::RestApiManager::commonInstance()->setCridential(connection->credential());
          this->_systemInfoManager->setCredential(connection->credential());
          this->_thetta->setCredential(connection->credential());
          this->setDownloaderCredential(connection->credential());
        });

        // Kernel patch detect
        QObject::connect(connection, &Connection::mainCredentialChanged, [this, connection]() {
          this->_kernelStat->execute(connection->credential());
        });
      }
    }

    void Application::setDownloaderCredential(const GGS::RestApi::GameNetCredential &creds)
    {
      QString userId = creds.userId();

      QCryptographicHash hash(QCryptographicHash::Sha1);
      hash.addData(userId.toLocal8Bit());
      hash.addData("t5mPWw25K8FIpZaQ"); //INFO QGNA-1319
      
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
