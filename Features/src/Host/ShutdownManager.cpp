#include <Host/ShutdownManager.h>
#include <Host/Thetta.h>
#include <Host/GameExecutor.h>
#include <Host/Application.h>

#include <GameDownloader/GameDownloadService.h>

#include <Application/SingleApplication.h>

using GGS::GameDownloader::GameDownloadService;
using GGS::Application::SingleApplication;

namespace GameNet {
  namespace Host {

    ShutdownManager::ShutdownManager(QObject *parent /*= 0*/)
      : QObject(parent)
      , _thetta(nullptr)
      , _executor(nullptr)
      , _downloader(nullptr)
      , _application(nullptr)
      , _singleApplication(nullptr)
      , _gameDownloadInitialized(false)
    {
    }

    ShutdownManager::~ShutdownManager()
    {
    }

    void ShutdownManager::setThetta(Thetta *value)
    {
      Q_ASSERT(value);
      this->_thetta = value;
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

    void ShutdownManager::setGameDownloadInitialized()
    {
      this->_gameDownloadInitialized = true;
    }

    void ShutdownManager::shutdown()
    {
      this->_application->unregisterDbusServices();
      this->shutdownThetta();
    }

    void ShutdownManager::shutdownThettaCompleted()
    {
      // UNDONE 19.09.2014 Раньше тут падало из-за приколов с потоками в запускаторе
      // но теперь потоков там нет и есть шанс что теперь не падает - надо проверить
      Q_ASSERT(this->_executor);
      this->_executor->shutdown();
      this->shutdownDownloader();
    }

    void ShutdownManager::shutdownThetta()
    {
      Q_ASSERT(this->_thetta);
      if (!this->_thetta->installer()) {
        this->shutdownThettaCompleted();
        return;
      }

      using Features::Thetta::ThettaInstaller;

      QObject::connect(this->_thetta->installer(), &ThettaInstaller::disconnected,
        [this](ThettaInstaller::Result result) {
          if (result == Features::Thetta::ThettaInstaller::Running)
            return;

          this->shutdownThettaCompleted();
      });

      this->_thetta->installer()->disconnectFromDriver();
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