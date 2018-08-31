#include <Host/StopDownloadOnExecuteInit.h>
#include <Host/ServiceProcess/ServiceLoader.h>
#include <Host/GameExecutor.h>

#include <GameDownloader/GameDownloadService.h>

#include <Features/StopDownloadServiceWhileExecuteAnyGame.h>

#include <Core/Service.h>

using P1::GameDownloader::GameDownloadService;
using P1::Core::Service;
using Features::StopDownloadServiceWhileExecuteAnyGame;

namespace P1 {
  namespace Host {

    StopDownloadOnExecuteInit::StopDownloadOnExecuteInit()
      : _target(nullptr)
      , _downloader(nullptr)
      , _services(nullptr)
      , _excutor(nullptr)
    {
    }

    StopDownloadOnExecuteInit::~StopDownloadOnExecuteInit()
    {
    }

    void StopDownloadOnExecuteInit::setDownloader(GameDownloadService *value)
    {
      Q_ASSERT(value);
      this->_downloader = value;
    }

    void StopDownloadOnExecuteInit::setServices(ServiceProcess::ServiceLoader *value)
    {
      Q_ASSERT(value);
      this->_services = value;
    }

    void StopDownloadOnExecuteInit::setTarget(StopDownloadServiceWhileExecuteAnyGame *value)
    {
      Q_ASSERT(value);
      this->_target = value;
    }

    void StopDownloadOnExecuteInit::setExecutor(GameExecutor *value)
    {
      Q_ASSERT(value);
      this->_excutor = value;
    }

    void StopDownloadOnExecuteInit::init()
    {
      Q_ASSERT(this->_target);
      Q_ASSERT(this->_downloader);
      Q_ASSERT(this->_services);
      Q_ASSERT(this->_excutor);

      Q_FOREACH(const Service* service, this->_services->servicesMap()) {
        if (service->isDownloadable())
          continue;

        this->_target->ignoreService(service->id());
      }

      QObject::connect(this->_target, &StopDownloadServiceWhileExecuteAnyGame::downloadStopRequest,
        this->_downloader, &GameDownloadService::stop, Qt::QueuedConnection);

      QObject::connect(this->_target, &StopDownloadServiceWhileExecuteAnyGame::downloadStartRequest,
        this->_downloader, &GameDownloadService::start, Qt::QueuedConnection);

      QObject::connect(this->_target, &StopDownloadServiceWhileExecuteAnyGame::torrentSessionPauseRequest,
        this->_downloader, &GameDownloadService::pauseSession, Qt::QueuedConnection);

      QObject::connect(this->_target, &StopDownloadServiceWhileExecuteAnyGame::torrentSessionResumeRequest,
        this->_downloader, &GameDownloadService::resumeSession, Qt::QueuedConnection);

      QObject::connect(this->_downloader, &GameDownloadService::started,
        this->_target, &StopDownloadServiceWhileExecuteAnyGame::onServiceStartDownload, Qt::QueuedConnection);

      QObject::connect(this->_downloader, &GameDownloadService::finished,
        this->_target, &StopDownloadServiceWhileExecuteAnyGame::onServiceFinishDownload, Qt::QueuedConnection);

      QObject::connect(this->_downloader, &GameDownloadService::stopped,
        this->_target, &StopDownloadServiceWhileExecuteAnyGame::onServiceFinishDownload, Qt::QueuedConnection);

      QObject::connect(this->_downloader, &GameDownloadService::failed,
        this->_target, &StopDownloadServiceWhileExecuteAnyGame::onServiceFinishDownload, Qt::QueuedConnection);

      QObject::connect(this->_excutor, &GameExecutor::serviceStarted,
        this->_target, &StopDownloadServiceWhileExecuteAnyGame::onGameExecuted, Qt::QueuedConnection);

      QObject::connect(this->_excutor, &GameExecutor::serviceFinished,
        this->_target, &StopDownloadServiceWhileExecuteAnyGame::onGameFinished, Qt::QueuedConnection);
    }
  }
}