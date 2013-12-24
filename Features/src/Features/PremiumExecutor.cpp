#include <Features/PremiumExecutor.h>

#include <GameExecutor/Executor/ExecutableFile.h>
#include <GameExecutor/Executor/WebLink.h>

#include <QtCore/QCoreApplication>

#define SIGNAL_CONNECT_CHECK(X) { bool result = X; Q_ASSERT_X(result, __FUNCTION__ , #X); }

namespace Features {

  PremiumExecutor::PremiumExecutor(QObject *parent)
    : QObject(parent)
  {
  }

  PremiumExecutor::~PremiumExecutor()
  {
  }

  void PremiumExecutor::executeMain(GGS::Core::Service* service)
  {
    if (this->isSecondStarted(service->id()))
      this->_simpleMainExecutor.execute(*service);
    else
      this->_mainExecutor->execute(*service);
  }

  void PremiumExecutor::executeSecond(GGS::Core::Service* service, const GGS::RestApi::GameNetCredential& credetial)
  {
    if (this->canExecuteSecond(service->id()))
      this->_secondExecutor.execute(*service, credetial);
  }
  
  void PremiumExecutor::init()
  {
    SIGNAL_CONNECT_CHECK(connect(&this->_simpleMainExecutor, SIGNAL(started(const GGS::Core::Service &)),
      this, SLOT(internalServiceStarted(const GGS::Core::Service &))));

    SIGNAL_CONNECT_CHECK(connect(&this->_simpleMainExecutor, SIGNAL(finished(const GGS::Core::Service &, GGS::GameExecutor::FinishState)),
      this, SLOT(internalServiceFinished(const GGS::Core::Service &, GGS::GameExecutor::FinishState))));

    SIGNAL_CONNECT_CHECK(connect(this->_mainExecutor, SIGNAL(started(const GGS::Core::Service &)),
      this, SLOT(internalServiceStarted(const GGS::Core::Service &))));

    SIGNAL_CONNECT_CHECK(connect(this->_mainExecutor, SIGNAL(finished(const GGS::Core::Service &, GGS::GameExecutor::FinishState)),
      this, SLOT(internalServiceFinished(const GGS::Core::Service &, GGS::GameExecutor::FinishState))));

    SIGNAL_CONNECT_CHECK(connect(&this->_secondExecutor, SIGNAL(started(const GGS::Core::Service &)),
      this, SLOT(internalSecondServiceStarted(const GGS::Core::Service &))));

    SIGNAL_CONNECT_CHECK(connect(&this->_secondExecutor, SIGNAL(finished(const GGS::Core::Service &, GGS::GameExecutor::FinishState)),
      this, SLOT(internalSecondServiceFinished(const GGS::Core::Service &, GGS::GameExecutor::FinishState))));

    this->registerExecutors(&this->_simpleMainExecutor);
    this->registerExecutors(&this->_secondExecutor);
    // UNDONE register hook for MW2
  }

  void PremiumExecutor::internalSecondServiceStarted(const GGS::Core::Service &service)
  {
    this->_secondGameStarted.insert(service.id());
    emit this->secondServiceStarted(service);
  }

  void PremiumExecutor::internalSecondServiceFinished(const GGS::Core::Service &service, GGS::GameExecutor::FinishState state)
  {
    this->_secondGameStarted.remove(service.id());
    emit this->secondServiceFinished(service, state);
  }

  void PremiumExecutor::internalServiceStarted(const GGS::Core::Service &service)
  {
    this->_mainGameStarted.insert(service.id());
    emit this->serviceStarted(service);
  }

  void PremiumExecutor::internalServiceFinished(const GGS::Core::Service &service, GGS::GameExecutor::FinishState state)
  {
    this->_mainGameStarted.remove(service.id());
    emit this->serviceFinished(service, state);
  }

  bool PremiumExecutor::isSecondStarted(const QString& id)
  {
    return this->_secondGameStarted.contains(id);
  }

  bool PremiumExecutor::isMainStarted(const QString& id)
  {
    return this->_mainGameStarted.contains(id);
  }

  bool PremiumExecutor::canSimpleExecuteMain(const QString& id)
  {
    return this->isSecondStarted(id);
  }

  bool PremiumExecutor::canExecuteSecond(const QString& id)
  {
    return this->isMainStarted(id);
  }

  void PremiumExecutor::registerExecutors(GGS::GameExecutor::GameExecutorService *executor)
  {
    GGS::GameExecutor::Executor::ExecutableFile *gameExecutorByLauncher = new GGS::GameExecutor::Executor::ExecutableFile(this);
    //gameExecutorByLauncher->setWorkingDirectory(QCoreApplication::applicationDirPath());
    executor->registerExecutor(gameExecutorByLauncher);

    GGS::GameExecutor::Executor::WebLink *webLinkExecutor = new GGS::GameExecutor::Executor::WebLink(this);
    executor->registerExecutor(webLinkExecutor);
  }

  void PremiumExecutor::shutdown()
  {
    this->_mainExecutor->shutdown();
    this->_simpleMainExecutor.shutdown();
    this->_secondExecutor.shutdown();
  }

  bool PremiumExecutor::isGameStarted(const QString& id)
  {
    return this->_mainExecutor->isGameStarted(id) || this->_simpleMainExecutor.isGameStarted(id);
  }

  bool PremiumExecutor::isSecondGameStarted(const QString& id)
  {
    return this->_secondExecutor.isGameStarted(id);
  }

  bool PremiumExecutor::isAnyGameStarted()
  {
    return this->_mainExecutor->isAnyGameStarted() 
      || this->_simpleMainExecutor.isAnyGameStarted()
      || this->_secondExecutor.isAnyGameStarted();
  }

  GGS::GameExecutor::GameExecutorService* PremiumExecutor::secondExecutor()
  {
    return &this->_secondExecutor;
  }

  GGS::GameExecutor::GameExecutorService* PremiumExecutor::simpleMainExecutor()
  {
    return &this->_simpleMainExecutor;
  }

}