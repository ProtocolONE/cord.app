#include <Host/GameExecutor.h>
#include <Host/Thetta.h>
#include <Host/ServiceLoader.h>

#include <Features/PremiumExecutor.h>

#include <GameExecutor/ServiceInfoCounter.h>

#include <RestApi/GameNetCredential.h>

#include <Core/Service.h>

#include <QtCore/QUrlQuery>
#include <QtCore/QCoreApplication>

using Features::PremiumExecutor;
using GGS::GameExecutor::GameExecutorService;
using GGS::GameExecutor::ServiceInfoCounter;
using GGS::Core::Service;
using GGS::RestApi::GameNetCredential;

namespace GameNet {
  namespace Host {
    
    GameExecutor::GameExecutor(QObject *parent /*= 0*/)
      : QObject(parent)
      , _premiumExecutor(new PremiumExecutor(this))
      , _mainExecutor(new GameExecutorService(this))
      , _gameExecutorServiceInfoCounter(new ServiceInfoCounter(this))
      , _services(nullptr)
      , _thetta(nullptr)
    {
    }

    GameExecutor::~GameExecutor()
    {
    }

    void GameExecutor::setServices(ServiceLoader *value)
    {
      Q_ASSERT(value);
      this->_services = value;
    }

    void GameExecutor::setThetta(Thetta *value)
    {
      Q_ASSERT(value);
      this->_thetta = value;
    }

    void GameExecutor::init()
    {
      Q_ASSERT(this->_mainExecutor);
      Q_ASSERT(this->_thetta);

      this->_premiumExecutor->setMainExecutor(this->_mainExecutor);
      this->_premiumExecutor->init();

      this->_thetta->initExtensions(this->_mainExecutor);
      this->_thetta->initExtensions(this->_premiumExecutor->secondExecutor());
      this->_thetta->initExtensions(this->_premiumExecutor->simpleMainExecutor());

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
      const QString& serviceId, const GameNetCredential& credetial)
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
      const GameNetCredential& credetial, 
      const GameNetCredential& secondCredetial)
    {
      Q_ASSERT(this->_services);
      Q_ASSERT(this->_premiumExecutor);
      GGS::Core::Service *service = this->_services->getService(serviceId);
      if (!service)
        return;

      this->_premiumExecutor->executeSecond(service, credetial, secondCredetial);
    }

    bool GameExecutor::isGameStarted(const QString& serviceId) const
    {
      return this->_premiumExecutor->isGameStarted(serviceId);
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

    void GameExecutor::onServiceFinished(const Service &service, GGS::GameExecutor::FinishState state)
    {
      emit this->serviceFinished(service.id(), static_cast<int>(state));
    }

    void GameExecutor::onSecondServiceStarted(const Service &service)
    {
      emit this->secondServiceStarted(service.id());
    }

    void GameExecutor::onSecondServiceFinished(const Service &service, GGS::GameExecutor::FinishState state)
    {
      emit this->secondServiceFinished(service.id(), static_cast<int>(state));
    }

    GameExecutorService* GameExecutor::mainExecutor()
    {
      return this->_mainExecutor;
    }

    void GameExecutor::prepairExecuteUrl(GGS::Core::Service *service)
    {
      if (!service->isDownloadable()) {
        service->setUrl(service->urlTemplate());
        return;
      }

      QUrl result(service->urlTemplate());
      QString path = result.path();
      QString gamePath = QString("%1/%2").arg(service->installPath(), service->areaString());
      path.replace("%gamePath%", gamePath, Qt::CaseInsensitive);
      result.setPath(path);

      QUrlQuery urlQuery(result);
      QString workingDir = urlQuery.queryItemValue("workingDir", QUrl::FullyDecoded);
      workingDir.replace("%gamePath%", gamePath, Qt::CaseInsensitive);

      urlQuery.removeAllQueryItems("workingDir");
      urlQuery.addQueryItem("workingDir", workingDir);

      if (urlQuery.hasQueryItem("executorHelper")) {
        urlQuery.removeAllQueryItems("executorHelper");
#ifdef _DEBUG
        QString executorHelper = QCoreApplication::applicationDirPath() + "/GameExecutorHelperX86d.dll"; 
#else
        QString executorHelper = QCoreApplication::applicationDirPath() + "/GameExecutorHelperX86.dll";
#endif
        urlQuery.addQueryItem("executorHelper", executorHelper);
      }

      if (urlQuery.hasQueryItem("injectOverlay")) {
        urlQuery.removeAllQueryItems("injectOverlay");

        // UNDONE 16.09.2014 Спросить у ServiceSettings доступен ли оверлей
        // https://jira.gamenet.ru:8443/browse/QGNA-963
        bool overlayEnabled = true;
        if (overlayEnabled) {
#ifdef _DEBUG
          QString injectedDll = QCoreApplication::applicationDirPath() + "/OverlayX86d.dll"; 
#else
          QString injectedDll = QCoreApplication::applicationDirPath() + "/OverlayX86.dll";
#endif
          urlQuery.addQueryItem("injectDll", injectedDll);
        }
      }

      result.setQuery(urlQuery);
      service->setUrl(result);
    }

    void GameExecutor::shutdownSecond()
    {
      this->_premiumExecutor->shutdownSecond();
    }

    

  }
}