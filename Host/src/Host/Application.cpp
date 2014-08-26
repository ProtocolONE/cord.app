#include <Host/Application.h>
#include <Host/ServiceLoader.h>
#include <Host/ServiceDescription.h>

#include <Host/Dbus/DownloaderBridgeAdaptor.h>
#include <Host/Bridge/DownloaderBridge.h>

#include <GameDownloader/GameDownloadService.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QMetaType>

#include <QtDBus/QDBusMetaType>

using GGS::GameDownloader::GameDownloadService;

namespace GameNet {
  namespace Host {

    Application::Application(QObject *parent /*= 0*/)
      : _serviceLoader(new ServiceLoader(this))
      , _gameDownloader(new GameDownloadService(this))
      , _downloadBridge(new Bridge::DownloaderBridge(this))
    {
    }

    Application::~Application()
    {
    }

    void Application::init()
    {
      // init dbus 
      // create adaptors 

      this->_downloadBridge->setServiceLoader(this->_serviceLoader);
      this->_downloadBridge->setDownloader(this->_gameDownloader);

      qRegisterMetaType<GameNet::Host::Bridge::DownloadProgressArgs>("GameNet::Host::Bridge::DownloadProgressArgs");
      qDBusRegisterMetaType<GameNet::Host::Bridge::DownloadProgressArgs>();

      this->_serviceLoader->setApplicationPath(QCoreApplication::applicationDirPath());

      // UNDONE порчитать откуда-то зоны для приложения и игр
      this->_serviceLoader->setApplicationArea(GGS::Core::Service::Live);
      this->_serviceLoader->setGameArea(GGS::Core::Service::Live);

      this->registerServices();

      QDBusConnection connection = QDBusConnection::sessionBus();
      new DownloaderBridgeAdaptor(this->_downloadBridge);

      connection.registerObject("/downloader", this->_downloadBridge);
      connection.registerService("GameNet.GameDownloader");
    }

    void Application::registerServices()
    {
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

    }

    void Application::initGameDownloader()
    {
      // UNDONE откуда то получить порт.
      //bool ok = false;
      //unsigned short port = settingsViewModel->incomingPort().toUShort(&ok);
      //if (ok)
      //  this->_gameDownloader->setListeningPort(port);

      QString torrentConfigPath = QCoreApplication::applicationDirPath();
      torrentConfigPath.append("/torrents");
      this->_gameDownloader->setTorrentConfigDirectoryPath(torrentConfigPath);

      this->_gameDownloader->setTimeoutForResume(600);
      this->_gameDownloader->init();

    }

  }
}
