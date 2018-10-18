#include <Host/GameExecutor.h>
#include <Host/ServiceProcess/ServiceLoader.h>
#include <Host/ServiceSettings.h>

#include <Helper/SystemInfo.h>

#include <GameExecutor/ServiceInfoCounter.h>

#include <RestApi/ProtocolOneCredential.h>

#include <Core/Service.h>

#include <QtCore/QUrlQuery>
#include <QtCore/QCoreApplication>

#include <GameExecutor/Executor/ExecutableFile.h>
#include <GameExecutor/Executor/WebLink.h>
#include <GameExecutor/Executor/WebLinkSsl.h>

//#include <Host/ElectronExecutor.h>
//#include <Host/ElectronsExecutor.h>

using P1::GameExecutor::GameExecutorService;
using P1::GameExecutor::ServiceInfoCounter;
using P1::Core::Service;
using P1::RestApi::ProtocolOneCredential;

namespace P1 {
  namespace Host {
    
    GameExecutor::GameExecutor(QObject *parent /*= 0*/)
      : QObject(parent)
      , _mainExecutor(new GameExecutorService(this))
      , _gameExecutorServiceInfoCounter(new ServiceInfoCounter(this))
      , _services(nullptr)
      , _serviceSettings(nullptr)
    {
    }

    GameExecutor::~GameExecutor()
    {
    }

    void GameExecutor::setServices(ServiceProcess::ServiceLoader *value)
    {
      Q_ASSERT(value);
      this->_services = value;
    }

    void GameExecutor::setServiceSettings(ServiceSettings* value)
    {
      Q_ASSERT(value);
      this->_serviceSettings = value;
    }

    void GameExecutor::init()
    {
      Q_ASSERT(this->_mainExecutor);
      Q_ASSERT(this->_serviceSettings);

      using namespace P1::GameExecutor::Executor;
      using namespace P1::Host;

      this->_mainExecutor->registerExecutor(new ExecutableFile(this));
      //this->_mainExecutor->registerExecutor(new ElectronExecutor(this));
      //this->_mainExecutor->registerExecutor(new ElectronsExecutor(this));
      this->_mainExecutor->registerExecutor(new WebLink(this));
      this->_mainExecutor->registerExecutor(new WebLinkSsl(this));

      QObject::connect(this->_mainExecutor, &GameExecutorService::started,
        this, &GameExecutor::onServiceStarted, Qt::QueuedConnection);

      QObject::connect(this->_mainExecutor, &GameExecutorService::finished,
        this, &GameExecutor::onServiceFinished, Qt::QueuedConnection);

      QObject::connect(this->_mainExecutor, &GameExecutorService::started,
        this->_gameExecutorServiceInfoCounter, &ServiceInfoCounter::started);
      
      QObject::connect(this->_mainExecutor, &GameExecutorService::finished,
        this->_gameExecutorServiceInfoCounter, &ServiceInfoCounter::finished);
    }

    void GameExecutor::shutdown()
    {
      this->_mainExecutor->shutdown();
    }

    void GameExecutor::execute(const QString& serviceId, const ProtocolOneCredential& credetial)
    {
      Q_ASSERT(this->_services);
      Q_ASSERT(this->_mainExecutor);

      Service *service = this->_services->getService(serviceId);
      if (!service)
        return;

      this->prepairExecuteUrl(service);

      QMutexLocker locker(&this->_mutex);

      this->_mainExecutor->execute(*service, credetial);
      this->_mainGameStarted.insert(serviceId);
    }

    bool GameExecutor::isGameStarted(const QString& serviceId) const
    {
      return this->_mainExecutor->isGameStarted(serviceId);
    }

    bool GameExecutor::isAnyGameStarted() const
    {
      return this->_mainExecutor->isAnyGameStarted();
    }

    void GameExecutor::onServiceStarted(const Service &service)
    {
      this->_mainGameStarted.insert(service.id());
      emit this->serviceStarted(service.id());
    }

    void GameExecutor::onServiceFinished(const Service &service, P1::GameExecutor::FinishState state)
    {
      {
        QMutexLocker locker(&this->_mutex);
        this->_mainGameStarted.remove(service.id());
      }

      emit this->serviceFinished(service.id(), static_cast<int>(state));
    }

    GameExecutorService* GameExecutor::mainExecutor()
    {
      return this->_mainExecutor;
    }

    void GameExecutor::prepairExecuteUrl(P1::Core::Service *service)
    {
      Q_ASSERT(this->_serviceSettings);

      if (!service->isDownloadable()) {
        service->setUrl(service->urlTemplate());
        return;
      }

      const QString& serviceId(service->id());
      QUrl result(service->urlTemplate());

      bool force32 = this->_serviceSettings->isPrefer32Bit(serviceId);

      QUrlQuery exe64Query(result);
      bool use64 = exe64Query.hasQueryItem("exe64") && Features::isWow64() && !force32;
      
      QString path = use64 ? exe64Query.queryItemValue("exe64", QUrl::FullyDecoded) : result.path();

      QString gamePath = QString("%1/%2").arg(service->installPath(), service->areaString());
      path.replace("%gamePath%", gamePath, Qt::CaseInsensitive);
      result.setPath(path);

      QUrlQuery urlQuery(result);
      QString workingDir = urlQuery.queryItemValue("workingDir", QUrl::FullyDecoded);
      workingDir.replace("%gamePath%", gamePath, Qt::CaseInsensitive);

      urlQuery.removeAllQueryItems("workingDir");
      urlQuery.addQueryItem("workingDir", workingDir);

      QString start64 = "0";

      if (use64)
        start64 = "1";

      urlQuery.addQueryItem("start64", start64);

      result.setQuery(urlQuery);
      service->setUrl(result);
    }

    QString GameExecutor::executedGame() const
    {
      if (this->_mainGameStarted.isEmpty())
        return QString();

      return *this->_mainGameStarted.begin();
    }

    void GameExecutor::terminateAll(const QString& serviceId /*= QString()*/)
    {
      if (this->_mainExecutor->isAnyGameStarted())
        this->_mainExecutor->terminate(serviceId);
    }

  }
}