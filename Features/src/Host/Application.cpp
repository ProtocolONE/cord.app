#include <Host/Connection.h>

#include <Host/ApplicationStatistic.h>
#include <Host/ApplicationRestarter.h>
#include <Host/Application.h>
#include <Host/Updater.h>
#include <Host/ServiceLoader.h>
#include <Host/ServiceSettings.h>
#include <Host/ServiceDescription.h>
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

#include <Integration/ZZima/ZzimaGameExecutor.h>
#include <Integration/ZZima/ZZimaConnection.h>

#include <Features/GameDownloader/GameDownloadStatistics.h>
#include <Features/StopDownloadServiceWhileExecuteAnyGame.h>
#include <Features/Thetta/ThettaInstaller.h>

#include <GameDownloader/GameDownloadService.h>

#include <Application/SingleApplication.h>

#include <RestApi/RestApiManager.h>
#include <RestApi/FakeCache.h>

#include <Settings/Settings.h>

#include <Marketing/MarketingTarget.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QMetaType>
#include <QtCore/QUrl>
#include <QtCore/QCoreApplication>

using GGS::Application::SingleApplication;
using GGS::GameDownloader::GameDownloadService;
using ::GameNet::Integration::ZZima::ZZimaConnection;

namespace GameNet {
  namespace Host {

    Application::Application(QObject *parent /*= 0*/)
      : _singleApplication(nullptr)
      , _shutdown(new ShutdownManager(this))
      , _serviceLoader(new ServiceLoader(this))
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
      , _initFinished(false)
      , _updateFinished(false)
      , QObject(parent)
    {
    }

    Application::~Application()
    {
      delete this->_restApiCache;
    }

    void Application::setInitFinished() 
    {
      this->_initFinished = true;

      if (this->_initFinished && this->_updateFinished)
        emit this->initCompleted();
    }

    void Application::setUpdateFinished() 
    {
      if (this->_updateFinished)
        return;

      this->_updateFinished = true;

      if (this->_initFinished && this->_updateFinished)
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

      QObject::connect(value, &SingleApplication::commandRecieved, 
        this->_commandLineManager, &CommandLineManager::commandRecieved, Qt::QueuedConnection); 

      QObject::connect(this, &Application::initCompleted, 
        value, &SingleApplication::initializeFinished);
    }

    void Application::init()
    {
      this->_messageAdapter->setHasUiProcess(std::bind(&UIProcess::isRunning, this->_uiProcess));
      QObject::connect(this->_uiProcess, &UIProcess::closed, this->_messageAdapter, 
        &MessageAdapter::uiProcessClosed);

      GGS::Core::UI::Message::setAdapter(this->_messageAdapter);

      this->initRestApi();
      this->_translation->init();

      this->initGameDownloader();

      this->_applicationStatistic->setDownloader(this->_gameDownloader);
      this->_applicationStatistic->setStartingGame(this->_commandLineManager->startingService());
      this->_applicationStatistic->init();

      this->_thetta->setApplication(this);
      this->_thetta->init();

      this->_executor->setServices(this->_serviceLoader);
      this->_executor->setServiceSettings(this->_serviceSettings);
      this->_executor->setThetta(this->_thetta);
      this->_executor->init();
      
      GameNet::Integration::ZZima::ZzimaGameExecutor * zzimaExecutor = 
        new GameNet::Integration::ZZima::ZzimaGameExecutor(this);
      zzimaExecutor->setZzimaConnection(this->_zzimaConnection);

      this->_executor->mainExecutor()->registerExecutor(zzimaExecutor);

      this->_downloaderHookFactory->setServiceLoader(this->_serviceLoader);
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

      this->registerServices();

      if (this->_updater->applicationArea() == GGS::Core::Service::Tst)
        this->_thetta->installer()->connectToDriver();

      StopDownloadOnExecuteInit stopDownloadOnExecuteInit;
      stopDownloadOnExecuteInit.setDownloader(this->_gameDownloader);
      stopDownloadOnExecuteInit.setServices(this->_serviceLoader);
      stopDownloadOnExecuteInit.setTarget(this->_stopDownloadServiceOnExecuteGame);
      stopDownloadOnExecuteInit.setExecutor(this->_executor);
      stopDownloadOnExecuteInit.init();

      this->registerDbusServices();

      this->_shutdown->setApplication(this);
      this->_shutdown->setDownloader(this->_gameDownloader);
      this->_shutdown->setExecutor(this->_executor);
      this->_shutdown->setThetta(this->_thetta);
      this->_shutdown->setSingleApplication(this->_singleApplication);
      this->_shutdown->setConnectionManager(this->_connectionManager);
      
      this->_applicationRestarter->setShutdownManager(this->_shutdown);
      
      this->_uiProcess->setDirectory(QCoreApplication::applicationDirPath());
      this->_uiProcess->setFileName("gamenet.ui.exe");

      QObject::connect(this->_commandLineManager, &CommandLineManager::shutdown,
        this, &Application::shutdown);

      QObject::connect(this->_commandLineManager, &CommandLineManager::uiCommand, 
        this->_uiProcess, &UIProcess::sendCommand);

      /*
        INFO Если (когда) поменяется схема инициализации, обратить внимание на эту строчку
             которая должна вызыватся после всего.
             Т.е. например после получения списка сервисов.
      */
      this->setInitFinished();
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
        if (!this->isInitCompleted() || !this->_uiProcess->isRunning()) {
          this->restartApplication(true, false);
          return;
        }

        emit this->restartUIRequest();
      }

      this->setUpdateFinished();
    }

    void Application::registerDbusServices()
    {
      this->_connectionManager->setApplication(this);

      if (!this->_connectionManager->init()) {
        MessageBoxW(0, L"Could not create DBusServer.", L"Error", MB_OK);
        QCoreApplication::quit();
        return;
      }
    }

    void Application::registerServices()
    {
      DownloadHookDescription dependencyHook;
      dependencyHook.first = "B4910801-2FA4-455E-AEAE-A2BAA2D3E4CA";
      dependencyHook.second.first = 0;
      dependencyHook.second.second = 10;

      ServiceDescription aikaGame;
      aikaGame.setId("300002010000000000");
      aikaGame.setGameId("631");
      aikaGame.setName("Aika2");
      aikaGame.setTorrentUrl("http://fs0.gamenet.ru/update/aika/");
      aikaGame.setIsDownloadable(true);
      aikaGame.setHasDownloadPath(true);
      aikaGame.setExtractorType("D9E40EE5-806F-4B7D-8D5C-B6A4BF0110E9");
      aikaGame.setExecuteUrl(
        "exe:%gamePath%/aikaru.exe"
        "?workingDir=%gamePath%/"
        "&args=%login% %token% 300002010000000000 login"
        "&executorHelper=1"
        "&downloadCustomFile=UI/GuildMarkWorld1.tga,http://files.gamenet.ru/update/aika,2");
      aikaGame.setGameSize(2800);
      
      QList<ExecutorHookDescription> aikaExecutorHooks;
      aikaExecutorHooks << ExecutorHookDescription("150EDADF-4B58-4E14-82D8-203832E57D3E", 0); // DisableIEDefalutProxy
      aikaExecutorHooks << ExecutorHookDescription("D6B4E0F4-1CE3-4226-967F-1C04A09BC59D", 0); // RestoreResolution
      aikaExecutorHooks << ExecutorHookDescription("9E25E28D-C06D-47C8-AC95-E139F35035FC", 0); // DefaultAikaSettings
      aikaExecutorHooks << ExecutorHookDescription("5E2D9B5B-D8C8-460A-A048-F7F4D18C7A37", 100); // DownloadCustomFile
      aikaGame.setExecutorHooks(aikaExecutorHooks);
      
      this->_serviceLoader->registerService(aikaGame);

      ServiceDescription bsGame;
      bsGame.setId("300003010000000000");
      bsGame.setGameId("71");
      bsGame.setName("BS");
      bsGame.setTorrentUrl("http://fs0.gamenet.ru/update/bs/");
      bsGame.setIsDownloadable(true);
      bsGame.setHasDownloadPath(false);
      bsGame.setExtractorType("3A3AC78E-0332-45F4-A466-89C2B8E8BB9C");
      bsGame.setExecuteUrl(
        "exe:%gamePath%/client/client.exe"
        "?workingDir=%gamePath%/"
        "&args=%userId% %appKey% %token%"
        "&executorHelper=1"
        "&injectOverlay=1"
        "&downloadCustomFile=launcher/serverinfo_back.xml,http://files.gamenet.ru/update/bs/,1,"
                             "config/lastlogin.xml,http://files.gamenet.ru/update/bs/,0");
      bsGame.setGameSize(2400);

      QList<ExecutorHookDescription> bsExecutorHooks;
      bsExecutorHooks << ExecutorHookDescription("54B0860B-215C-462F-A80E-F7664DEA984F", 0); // DisableDEP
      bsExecutorHooks << ExecutorHookDescription("5E2D9B5B-D8C8-460A-A048-F7F4D18C7A37", 100); // DownloadCustomFile
      bsGame.setExecutorHooks(bsExecutorHooks);

      this->_serviceLoader->registerService(bsGame);

      ServiceDescription rebornGame;
      rebornGame.setId("300012010000000000");
      rebornGame.setGameId("760");
      rebornGame.setName("Reborn");
      rebornGame.setTorrentUrl("http://fs0.gamenet.ru/update/reborn/");
      rebornGame.setIsDownloadable(true);
      rebornGame.setHasDownloadPath(false);
      rebornGame.setExtractorType("3A3AC78E-0332-45F4-A466-89C2B8E8BB9C");
      rebornGame.setExecuteUrl(
        "exe:%gamePath%/client/Client.exe"
        "?workingDir=%gamePath%/"
        "&args=%userId% %token%"
        "&executorHelper=1"
        "&downloadCustomFile=launcher/serverinfo_back.xml,http://files.gamenet.ru/update/reborn/,1");
      rebornGame.setGameSize(3300);

      QList<ExecutorHookDescription> rebornExecutorHooks;
      rebornExecutorHooks << ExecutorHookDescription("54B0860B-215C-462F-A80E-F7664DEA984F", 0); // DisableDEP
      rebornExecutorHooks << ExecutorHookDescription("5E2D9B5B-D8C8-460A-A048-F7F4D18C7A37", 100); // DownloadCustomFile
      rebornGame.setExecutorHooks(rebornExecutorHooks);

      this->_serviceLoader->registerService(rebornGame);

      ServiceDescription warincGame;
      warincGame.setId("300005010000000000");
      warincGame.setGameId("70");
      warincGame.setName("FireStorm");
      warincGame.setTorrentUrl("http://fs0.gamenet.ru/update/warinc/");
      warincGame.setIsDownloadable(true);
      warincGame.setHasDownloadPath(false);
      warincGame.setExtractorType("3A3AC78E-0332-45F4-A466-89C2B8E8BB9C");
      warincGame.setExecuteUrl(
        "exe:%gamePath%/WarInc.exe"
        "?workingDir=%gamePath%/"
        "&args=-WOUpdatedOk -gna %userId% %appKey% %token%");
      warincGame.setGameSize(2500);
      warincGame.setDependencyList("dxwebsetup.exe,/Q");

      QList<DownloadHookDescription> warincDownloadHooks;
      warincDownloadHooks << dependencyHook;
      warincGame.setDownloadHooks(warincDownloadHooks);
      this->_serviceLoader->registerService(warincGame);

      ServiceDescription caGame;
      caGame.setId("300009010000000000");
      caGame.setGameId("92");
      caGame.setName("CombatArms");
      caGame.setTorrentUrl("http://fs0.gamenet.ru/update/ca/");
      caGame.setIsDownloadable(true);
      caGame.setHasDownloadPath(true);
      caGame.setExtractorType("D9E40EE5-806F-4B7D-8D5C-B6A4BF0110E9");
      caGame.setExecuteUrl(
        "exe:%gamePath%/engine.exe"
        "?workingDir=%gamePath%/"
        "&args=-windowtitle \"CombatArms\" -rez Engine.REZ -rez Game -authip 31.25.225.205 -authport 10002 "
           "-pcroom 0 -Ver Ver_RU_2.1207.03 -UserID %userid% -Password %appkey%:%token%"
           
        "&executorHelper=1"
        "&downloadCustomFile=Profiles/player.txt,http://files.gamenet.ru/update/ca/,0");
      caGame.setGameSize(4800);
      
      QList<DownloadHookDescription> caDownloadHooks;
      DownloadHookDescription distribIntegrity;
      distribIntegrity.first = QLatin1String("36003110-6DC9-4D16-8076-D84FFAFC36B8");
      distribIntegrity.second.first = -1;
      distribIntegrity.second.second = -1;
      caDownloadHooks << distribIntegrity;
      caGame.setDownloadHooks(caDownloadHooks);

      QList<ExecutorHookDescription> caExecutorHooks;
      caExecutorHooks << ExecutorHookDescription("59E0D256-27E2-4D38-BBA6-DE9201DF53D7", 0); // CASettingsFix
      caExecutorHooks << ExecutorHookDescription("150EDADF-4B58-4E14-82D8-203832E57D3E", 0); // DisableIEDefalutProxy
      caExecutorHooks << ExecutorHookDescription("5E2D9B5B-D8C8-460A-A048-F7F4D18C7A37", 100); // DownloadCustomFile
      caExecutorHooks << ExecutorHookDescription("B0215EBC-27F0-4C3C-BC21-0C3611AFEEF4", 0); // DisableAeroHook
      caExecutorHooks << ExecutorHookDescription("D2352B73-1EEA-4522-AF4C-5F3A859FADFB", -1); // DistrIntegrityExecutorHook
      caGame.setExecutorHooks(caExecutorHooks);

      this->_serviceLoader->registerService(caGame);

      ServiceDescription rotGame;
      rotGame.setId("300004010000000000");
      rotGame.setGameId("72");
      rotGame.setName("RageofTitans");
      rotGame.setTorrentUrl("http://fs0.gamenet.ru/update/rot/");
      rotGame.setIsDownloadable(true);
      rotGame.setHasDownloadPath(true);
      rotGame.setExtractorType("D9E40EE5-806F-4B7D-8D5C-B6A4BF0110E9");
      rotGame.setExecuteUrl(
        "exe:%gamePath%/bin/tyj.exe"
        "?workingDir=%gamePath%/bin/"
        "&args=-sa UserId=%userId%%appKey% -sa Token=%token%");
      rotGame.setGameSize(1000);
      rotGame.setDependencyList("dxwebsetup.exe,/Q");

      QList<DownloadHookDescription> rotDownloadHooks;
      rotDownloadHooks << dependencyHook;
      rotGame.setDownloadHooks(rotDownloadHooks);
      this->_serviceLoader->registerService(rotGame);

      ServiceDescription gaGame;
      gaGame.setId("300007010000000000");
      gaGame.setGameId("83");
      gaGame.setName("300007010000000000");
      gaGame.setIsDownloadable(false);
      gaGame.setHasDownloadPath(false);
      gaGame.setExecuteUrl("http://www.playga.ru/");
      this->_serviceLoader->registerService(gaGame);

      ServiceDescription fjGame;
      fjGame.setId("300011010000000000");
      fjGame.setGameId("759");
      fjGame.setName("300011010000000000");
      fjGame.setIsDownloadable(false);
      fjGame.setHasDownloadPath(false);
      fjGame.setExecuteUrl("http://www.gamenet.ru/games/ferma/play/?fullscreen=1");
      this->_serviceLoader->registerService(fjGame);

      ServiceDescription bdGame;
      bdGame.setId("30000000000");
      bdGame.setGameId("1021");
      bdGame.setName("30000000000");
      bdGame.setIsDownloadable(false);
      bdGame.setHasDownloadPath(false);
      bdGame.setExecuteUrl("http://blackdesert.ru/");
      this->_serviceLoader->registerService(bdGame);

      // HACK DA game
      ServiceDescription daGame;
      daGame.setId("60000000000");
      daGame.setGameId("1030");
      daGame.setName("DarkAge");
      daGame.setTorrentUrl("http://torrents.zzima.net/fwclient.torrent");
      daGame.setIsDownloadable(true);
      daGame.setHasDownloadPath(false);
      daGame.setExtractorType("3A3AC78E-0332-45F4-A466-89C2B8E8BB9C");
      daGame.setExecuteUrl("zzima:start");
      daGame.setGameSize(8400);

      QList<DownloadHookDescription> daDownloaderHooks;
      DownloadHookDescription daInstallHook;
      daInstallHook.first = "9F6083BB-D03D-45A9-89FE-2D6EF098544A";
      daInstallHook.second.first = 999;
      daInstallHook.second.second = 0;
      daDownloaderHooks << daInstallHook;
      daGame.setDownloadHooks(daDownloaderHooks);

      //QList<ExecutorHookDescription> daExecutorHooks;
      //daExecutorHooks << ExecutorHookDescription("54B0860B-215C-462F-A80E-F7664DEA984F", 0); // DisableDEP
      //daExecutorHooks << ExecutorHookDescription("5E2D9B5B-D8C8-460A-A048-F7F4D18C7A37", 100); // DownloadCustomFile
      //daGame.setExecutorHooks(daExecutorHooks);

      this->_serviceLoader->registerService(daGame);
    }

    void Application::initGameDownloader()
    {
      this->_downloaderSettings->setDownloader(this->_gameDownloader);
      this->_downloaderSettings->init();

      this->_gameDownloader->init();

      this->_downloadStatistics->init(this->_gameDownloader);
      this->_shutdown->setGameDownloadInitialized();
    }
    
    void Application::initRestApi()
    {
        QStringList ports;
        ports << "443" << "7443" << "8443" << "9443" << "10443" << "11443";
        QString randomPort = ports.takeAt(qrand() % ports.count());
        QString apiUrl = QString("https://gnapi.com:%1/restapi").arg(randomPort);
        //apiUrl = "http://api.gamenet.stg/restapi";

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

    void Application::restartApplication(bool shouldStartWithSameArguments, bool isMinimized)
    {
      this->_applicationRestarter->restartApplication(shouldStartWithSameArguments, isMinimized);
    }

    void Application::finalize()
    {
      this->_shutdown->finalize();
    }

    void Application::shutdown()
    {
      QObject::connect(this->_shutdown, &ShutdownManager::shutdownCompleted,
        []() {
          QCoreApplication::quit();
      });

      this->_shutdown->shutdown();
    }

    void Application::startUi()
    {
      if (this->_commandLineManager->skipUi())
        return;

      qDebug() << "Starting qGNA UI";
      QStringList args = QCoreApplication::arguments();
      args.removeFirst(); // INFO first argument always self execute path

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

  }
}