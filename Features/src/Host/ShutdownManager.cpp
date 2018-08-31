#include <Host/ShutdownManager.h>
#include <Host/GameExecutor.h>
#include <Host/Application.h>
#include <Host/ConnectionManager.h>

#include <GameDownloader/GameDownloadService.h>

#include <Application/SingleApplication.h>

using P1::GameDownloader::GameDownloadService;
using P1::Application::SingleApplication;

namespace P1 {
  namespace Host {

    ShutdownManager::ShutdownManager(QObject *parent /*= 0*/)
      : QObject(parent)
      , _executor(nullptr)
      , _downloader(nullptr)
      , _application(nullptr)
      , _singleApplication(nullptr)
      , _gameDownloadInitialized(false)
      , _connectionManager(nullptr)
    {
    }

    ShutdownManager::~ShutdownManager()
    {
    }

    void ShutdownManager::setExecutor(GameExecutor *value)
    {
      Q_ASSERT(value);
      this->_executor = value;
    }

    void ShutdownManager::setDownloader(GameDownloadService *value)
    {
      Q_ASSERT(value);
      this->_downloader = value;
      QObject::connect(this->_downloader, &GameDownloadService::shutdownCompleted, 
        this, &ShutdownManager::shutdownDownloaderCompleted);
    }

    void ShutdownManager::setApplication(Application *value)
    {
      Q_ASSERT(value);
      this->_application = value;
    }

    void ShutdownManager::setSingleApplication(SingleApplication *value)
    {
      Q_ASSERT(value);
      this->_singleApplication = value;
    }

    void ShutdownManager::setConnectionManager(ConnectionManager *value)
    {
      Q_ASSERT(value);
      this->_connectionManager = value;
    }

    void ShutdownManager::setGameDownloadInitialized()
    {
      this->_gameDownloadInitialized = true;
    }

    void ShutdownManager::shutdown()
    {
      Q_ASSERT(this->_connectionManager);
      this->_connectionManager->shutdown();
      
      Q_ASSERT(this->_executor);
      this->_executor->shutdown();
      this->shutdownDownloader();
    }

    void ShutdownManager::shutdownDownloader()
    {
      Q_ASSERT(this->_downloader);
      if (!this->_gameDownloadInitialized) {
        this->shutdownDownloaderCompleted();
        return;
      }

      this->_downloader->shutdown();
    }

    void ShutdownManager::shutdownDownloaderCompleted()
    {
      Q_ASSERT(this->_singleApplication);
      this->_singleApplication->allowSecondInstance();
      emit this->shutdownCompleted();
    }

    void ShutdownManager::finalize()
    {
      // INFO релиз торрента должен быть очень поздно: после того как выйдем из app.exec()
      if (this->_gameDownloadInitialized)
        this->_downloader->release();
    }

  }
}