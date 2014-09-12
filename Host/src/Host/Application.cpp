#include <Host/Updater.h>
#include <Host/Application.h>
#include <Host/ServiceLoader.h>
#include <Host/ServiceSettings.h>
#include <Host/ServiceDescription.h>

#include <Host/Dbus/DownloaderBridgeAdaptor.h>
#include <Host/Dbus/DownloaderSettingsBridgeAdaptor.h>
#include <Host/Dbus/ServiceSettingsBridgeAdaptor.h>
#include <Host/Dbus/UpdateManagerBridgeAdaptor.h>
#include <Host/Dbus/ApplicationBridgeAdaptor.h>

#include <Host/Bridge/DownloaderBridge.h>
#include <Host/Bridge/DownloaderSettingsBridge.h>
#include <Host/Bridge/ServiceSettingsBridge.h>
#include <Host/Bridge/UpdateManagerBridge.h>
#include <Host/Bridge/ApplicationBridge.h>

#include <Host/DownloaderSettings.h>
#include <Host/UIProcess.h>
#include <Host/ApplicationRestarter.h>

#include <Features/GameDownloader/GameDownloadStatistics.h>

#include <GameDownloader/GameDownloadService.h>
#include <Application/ArgumentParser.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QMetaType>

#include <QtDBus/QDBusMetaType>

using GGS::GameDownloader::GameDownloadService;

namespace GameNet {
  namespace Host {

    Application::Application(QObject *parent /*= 0*/)
      : _serviceLoader(new ServiceLoader(this))
      , _gameDownloader(new GameDownloadService(this))
      , _downloaderBridge(new Bridge::DownloaderBridge(this))
      , _downloaderSettings(new DownloaderSettings(this))
      , _downloaderSettingsBridge(new Bridge::DownloaderSettingsBridge(this))
      , _serviceSettings(new ServiceSettings(this))
      , _serviceSettingsBridge(new Bridge::ServiceSettingsBridge(this))
      , _downloadStatistics(new Features::GameDownloader::GameDownloadStatistics(this))
      , _updater(new Updater(this))
      , _updateManagerBridge(new Bridge::UpdateManagerBridge(this))
      , _applicationBridge(new Bridge::ApplicationBridge(this))
      , _uiProcess(new UIProcess(this))
      , _commandLineArguments(new GGS::Application::ArgumentParser(this))
      , _applicationRestarter(new ApplicationRestarter(this))
      , _initFinished(false)
      , _updateFinished(false)
      , QObject(parent)
    {
      this->_commandLineArguments->parse(QCoreApplication::arguments());
    }

    Application::~Application()
    {
    }

    void Application::setInitFinished() {
      this->_initFinished = true;

      if (this->_initFinished && this->_updateFinished) {
        emit this->initCompleted();
      }
    }

    void Application::setUpdateFinished() 
    {
      if (this->_updateFinished)
        return;

      this->_updateFinished = true;

      if (this->_initFinished && this->_updateFinished) {
        emit this->initCompleted();
      }
    }

    bool Application::isInitCompleted()
    {
      return this->_initFinished && this->_updateFinished;
    }

    void Application::init()
    {
      // init dbus 
      // create adaptors 

      this->initTranslations();

      this->initGameDownloader();

      this->_applicationBridge->setApplcation(this);
      this->_downloaderBridge->setServiceLoader(this->_serviceLoader);
      this->_downloaderBridge->setDownloader(this->_gameDownloader);
      this->_downloaderSettingsBridge->setDownloaderSettings(this->_downloaderSettings);
      this->_updateManagerBridge->setUpdateManager(this->_updater);

      qRegisterMetaType<GameNet::Host::Bridge::DownloadProgressArgs>("GameNet::Host::Bridge::DownloadProgressArgs");
      qDBusRegisterMetaType<GameNet::Host::Bridge::DownloadProgressArgs>();

      this->_serviceLoader->setApplicationPath(QCoreApplication::applicationDirPath());

      // UNDONE порчитать откуда-то зоны для приложения и игр
      this->_serviceLoader->setApplicationArea(GGS::Core::Service::Live);
      this->_serviceLoader->setGameArea(GGS::Core::Service::Live);
      this->_serviceLoader->setDownloader(this->_gameDownloader);
      this->_serviceLoader->setServiceSettings(this->_serviceSettings);

      this->_serviceSettings->setServices(this->_serviceLoader);
      this->_serviceSettings->setDownloader(this->_gameDownloader);
      this->_serviceSettingsBridge->setSettings(this->_serviceSettings);

      this->registerServices();

      QDBusConnection connection = QDBusConnection::sessionBus();

      new DownloaderBridgeAdaptor(this->_downloaderBridge);
      new DownloaderSettingsBridgeAdaptor(this->_downloaderSettingsBridge);
      new ServiceSettingsBridgeAdaptor(this->_serviceSettingsBridge);
      new UpdateManagerBridgeAdaptor(this->_updateManagerBridge);
      new ApplicationBridgeAdaptor(this->_applicationBridge);

      connection.registerObject("/application", this->_applicationBridge);
      connection.registerObject("/downloader", this->_downloaderBridge);
      connection.registerObject("/downloader/settings", this->_downloaderSettingsBridge);
      connection.registerObject("/serviceSettings", this->_serviceSettingsBridge);
      connection.registerObject("/updater", this->_updateManagerBridge);
      connection.registerService("com.gamenet.dbus");

      /*
        INFO Если (когда) поменяется схема инициализации, обратить внимание на эту строчку
             которая должна вызыватся после всего
      */
      this->setInitFinished();

      if (!QCoreApplication::arguments().contains("--skip-ui")) {
        qDebug() << "Starting qGNA UI";
        QStringList args = QCoreApplication::arguments();
        args.removeFirst(); // INFO first argument always self execute path
        this->_uiProcess->start(QCoreApplication::applicationDirPath(), "qGNA.exe", args);
      }

      QObject::connect(this->_updater, &Updater::allCompleted, this, &Application::updateCompletedSlot);
      this->_updater->setDownloader(this->_gameDownloader);
      this->_updater->startCheckUpdate();
    }

    void Application::updateCompletedSlot(bool needRestart)
    {
      if (needRestart) {
        if (!this->isInitCompleted() || !this->_uiProcess->isRunning()) {
          this->restartApplication(true, false);
          return;
        }

        emit this->_applicationBridge->restartUIRequest();
      }

      this->setUpdateFinished();
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
      aikaGame.setExecuteUrl(""); // undone
      aikaGame.setGameSize(2800);
      this->_serviceLoader->registerService(aikaGame);

      ServiceDescription bsGame;
      bsGame.setId("300003010000000000");
      bsGame.setGameId("71");
      bsGame.setName("BS");
      bsGame.setTorrentUrl("http://fs0.gamenet.ru/update/bs/");
      bsGame.setIsDownloadable(true);
      bsGame.setHasDownloadPath(false);
      bsGame.setExtractorType("3A3AC78E-0332-45F4-A466-89C2B8E8BB9C");
      bsGame.setExecuteUrl(""); // undone
      bsGame.setGameSize(2400);
      this->_serviceLoader->registerService(bsGame);

      ServiceDescription rebornGame;
      rebornGame.setId("300012010000000000");
      rebornGame.setGameId("760");
      rebornGame.setName("Reborn");
      rebornGame.setTorrentUrl("http://fs0.gamenet.ru/update/reborn/");
      rebornGame.setIsDownloadable(true);
      rebornGame.setHasDownloadPath(false);
      rebornGame.setExtractorType("3A3AC78E-0332-45F4-A466-89C2B8E8BB9C");
      rebornGame.setExecuteUrl(""); // undone
      rebornGame.setGameSize(3300);
      this->_serviceLoader->registerService(rebornGame);

      ServiceDescription warincGame;
      warincGame.setId("300005010000000000");
      warincGame.setGameId("70");
      warincGame.setName("FireStorm");
      warincGame.setTorrentUrl("http://fs0.gamenet.ru/update/warinc/");
      warincGame.setIsDownloadable(true);
      warincGame.setHasDownloadPath(false);
      warincGame.setExtractorType("3A3AC78E-0332-45F4-A466-89C2B8E8BB9C");
      warincGame.setExecuteUrl(""); // undone
      warincGame.setGameSize(2500);

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
      caGame.setExecuteUrl(""); // undone
      caGame.setGameSize(4800);
      
      // UNDONE 04.09.2014 При заполнении хуков запускатора не забыть связать с этим хуком
      // его может связать прям фабрика попросив по тому же гуиду хук скачивания
      // ибо хуки для даунлоадера создаются в единственном экземпляре
      QList<DownloadHookDescription> caDownloadHooks;
      DownloadHookDescription distribIntegrity;
      distribIntegrity.first = QLatin1String("36003110-6DC9-4D16-8076-D84FFAFC36B8");
      distribIntegrity.second.first = -1;
      distribIntegrity.second.second = -1;
      caDownloadHooks << distribIntegrity;
      caGame.setDownloadHooks(caDownloadHooks);
      this->_serviceLoader->registerService(caGame);

      ServiceDescription rotGame;
      rotGame.setId("300004010000000000");
      rotGame.setGameId("72");
      rotGame.setName("RageofTitans");
      rotGame.setTorrentUrl("http://fs0.gamenet.ru/update/rot/");
      rotGame.setIsDownloadable(true);
      rotGame.setHasDownloadPath(true);
      rotGame.setExtractorType("D9E40EE5-806F-4B7D-8D5C-B6A4BF0110E9");
      rotGame.setExecuteUrl(""); // undone
      rotGame.setGameSize(1000);

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
    }

    void Application::initGameDownloader()
    {
      this->_downloaderSettings->setDownloader(this->_gameDownloader);
      this->_downloaderSettings->init();
      this->_gameDownloader->init();
      this->_downloadStatistics->init(this->_gameDownloader);
      this->_applicationRestarter->setGameDownloadInitialized();

      QObject::connect(this->_applicationRestarter, &ApplicationRestarter::shutdownRequest,
        this->_gameDownloader, &GameDownloadService::shutdown);

      QObject::connect(this->_gameDownloader, &GameDownloadService::shutdownCompleted, 
        this, &Application::shutdownCompleted);
    }

    void Application::initTranslations()
    {
      // UNDONE 04.09.2014 Посмотреть можно ли свести переводы в 1 файл и грузить их оба с хоста и уи
      QDir dir(QCoreApplication::applicationDirPath() + "/Languages/");
      dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
      dir.setSorting(QDir::Size | QDir::Reversed);

      QFileInfoList list = dir.entryInfoList();

      for (int i = 0; i < list.size(); ++i) { // TODO Немного хардкод, обсудить это
        QFileInfo fileInfo = list.at(i);

        QString fileName = fileInfo.fileName();
        if (!fileName.startsWith("host_", Qt::CaseInsensitive))
          continue;

        fileName.remove("host_");
        fileName.remove(".qm");

        if (fileName.size() == 2) {  
          this->_translators[fileName] = new QTranslator(this);
          // TODO да да, знаю, сделаю красиво позже.
          this->_translators[fileName]->load(fileInfo.fileName(), QCoreApplication::applicationDirPath() + "/Languages/");
        }
      }

      // UNDONE 04.09.2014 определиться как мы получаем язхык и выбираем его.
      QString defaultLanguage("ru");
      if (this->_translators.contains(defaultLanguage))
        QCoreApplication::installTranslator(this->_translators[defaultLanguage]);
      
    }

    void Application::shutdownCompleted()
    {
      // UNDONE Когда Илья закончит с запускатором раскоментить ниже

      // INFO Этот способ обойти проблему с падением на удалении GameExecutorService
      // Там куча потоков ждет заверешния процессов игры, и падает при удалении.
      // Этим мы закрываем игры до того как начнем удалять запусктор.

/*      this->_serviceLoader.thettaInstaller()->disconnectFromDriver();*/

      // INFO Это пока относительно легкий способ побороть проблему падения qGNA после его закрытия, если запущена игра
      // Убрать после переписывания запускатора

//       Sleep(500);
//       this->_premiumExecutor.shutdown();
      DEBUG_LOG << "shutdownCompleted";
      QCoreApplication::quit();
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

  }
}

