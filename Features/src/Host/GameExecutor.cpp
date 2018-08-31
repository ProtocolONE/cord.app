#include <Host/GameExecutor.h>
#include <Host/ServiceProcess/ServiceLoader.h>
#include <Host/ServiceSettings.h>

#include <Features/PremiumExecutor.h>
#include <Helper/SystemInfo.h>

#include <GameExecutor/ServiceInfoCounter.h>

#include <RestApi/ProtocolOneCredential.h>

#include <Core/Service.h>

#include <QtCore/QUrlQuery>
#include <QtCore/QCoreApplication>

using Features::PremiumExecutor;
using P1::GameExecutor::GameExecutorService;
using P1::GameExecutor::ServiceInfoCounter;
using P1::Core::Service;
using P1::RestApi::ProtocolOneCredential;

namespace P1 {
  namespace Host {
    
    GameExecutor::GameExecutor(QObject *parent /*= 0*/)
      : QObject(parent)
      , _premiumExecutor(new PremiumExecutor(this))
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

      this->_premiumExecutor->setMainExecutor(this->_mainExecutor);
      this->_premiumExecutor->init();

      QObject::connect(this->_premiumExecutor, &PremiumExecutor::serviceStarted,
        this, &GameExecutor::onServiceStarted);

      QObject::connect(this->_premiumExecutor, &PremiumExecutor::serviceFinished,
        this, &GameExecutor::onServiceFinished);

      QObject::connect(this->_premiumExecutor, &PremiumExecutor::secondServiceStarted,
        this, &GameExecutor::onSecondServiceStarted);

      QObject::connect(this->_premiumExecutor, &PremiumExecutor::secondServiceFinished,
        this, &GameExecutor::onSecondServiceFinished);

      QObject::connect(this->_mainExecutor, &GameExecutorService::started,
        this->_gameExecutorServiceInfoCounter, &ServiceInfoCounter::started);
      
      QObject::connect(this->_mainExecutor, &GameExecutorService::finished,
        this->_gameExecutorServiceInfoCounter, &ServiceInfoCounter::finished);
    }

    void GameExecutor::shutdown()
    {
      this->_premiumExecutor->shutdown();
    }

    void GameExecutor::execute(
      const QString& serviceId, const ProtocolOneCredential& credetial)
    {
      Q_ASSERT(this->_services);
      Q_ASSERT(this->_premiumExecutor);

      Service *service = this->_services->getService(serviceId);
      if (!service)
        return;

      this->prepairExecuteUrl(service);
      this->_premiumExecutor->executeMain(service, credetial);
    }

    void GameExecutor::executeSecond(
      const QString& serviceId, 
      const ProtocolOneCredential& credetial, 
      const ProtocolOneCredential& secondCredetial)
    {
      Q_ASSERT(this->_services);
      Q_ASSERT(this->_premiumExecutor);
      P1::Core::Service *service = this->_services->getService(serviceId);
      if (!service)
        return;

      this->_premiumExecutor->executeSecond(service, credetial, secondCredetial);
    }

    bool GameExecutor::isGameStarted(const QString& serviceId) const
    {
      return this->_premiumExecutor->isGameStarted(serviceId);
    }

    bool GameExecutor::isSecondGameStarted(const QString& serviceId) const
    {
      return this->_premiumExecutor->isSecondGameStarted(serviceId);
    }

    bool GameExecutor::isAnyGameStarted() const
    {
      return this->_premiumExecutor->isAnyGameStarted();
    }

    bool GameExecutor::canExecuteSecond(const QString& serviceId) const
    {
      Q_ASSERT(this->_premiumExecutor);
      return this->_premiumExecutor->canExecuteSecond(serviceId);
    }

    void GameExecutor::onServiceStarted(const Service &service)
    {
      emit this->serviceStarted(service.id());
    }

    void GameExecutor::onServiceFinished(const Service &service, P1::GameExecutor::FinishState state)
    {
      emit this->serviceFinished(service.id(), static_cast<int>(state));
    }

    void GameExecutor::onSecondServiceStarted(const Service &service)
    {
      emit this->secondServiceStarted(service.id());
    }

    void GameExecutor::onSecondServiceFinished(const Service &service, P1::GameExecutor::FinishState state)
    {
      emit this->secondServiceFinished(service.id(), static_cast<int>(state));
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

    void GameExecutor::shutdownSecond()
    {
      this->_premiumExecutor->shutdownSecond();
    }

    QString GameExecutor::executedGame() const
    {
      QString result = this->_premiumExecutor->firstRunningGame();

      if (result.isEmpty())
        result = this->_premiumExecutor->firstRunningSecondGame();

      return result;
    }

    P1::GameExecutor::GameExecutorService* GameExecutor::secondExecutor()
    {
      return this->_premiumExecutor->secondExecutor();
    }

    P1::GameExecutor::GameExecutorService* GameExecutor::simpleMainExecutor()
    {
      return this->_premiumExecutor->simpleMainExecutor();
    }

    void GameExecutor::terminateAll(const QString& serviceId /*= QString()*/)
    {
      this->_premiumExecutor->terminateAll(serviceId);
    }

    void GameExecutor::terminateSecond(const QString& serviceId /*= QString()*/)
    {
      this->_premiumExecutor->shutdownSecond(serviceId);
    }

  }
}