/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates. 
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/
#include <Features/PremiumExecutor.h>

#include <GameExecutor/Executor/ExecutableFile.h>
#include <GameExecutor/Executor/WebLink.h>

#include <RestApi/RestApiManager.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QMutexLocker>

#define SIGNAL_CONNECT_CHECK(X) { bool result = X; Q_ASSERT_X(result, __FUNCTION__ , #X); }

namespace Features {

  PremiumExecutor::PremiumExecutor(QObject *parent)
    : QObject(parent)
  {
  }

  PremiumExecutor::~PremiumExecutor()
  {
  }

  void PremiumExecutor::init()
  {
    SIGNAL_CONNECT_CHECK(connect(&this->_simpleMainExecutor, SIGNAL(started(const GGS::Core::Service &)),
      this, SLOT(internalServiceStarted(const GGS::Core::Service &)), Qt::QueuedConnection));

    SIGNAL_CONNECT_CHECK(connect(&this->_simpleMainExecutor, SIGNAL(finished(const GGS::Core::Service &, GGS::GameExecutor::FinishState)),
      this, SLOT(internalServiceFinished(const GGS::Core::Service &, GGS::GameExecutor::FinishState)), Qt::QueuedConnection));

    SIGNAL_CONNECT_CHECK(connect(this->_mainExecutor, SIGNAL(started(const GGS::Core::Service &)),
      this, SLOT(internalServiceStarted(const GGS::Core::Service &)), Qt::QueuedConnection));

    SIGNAL_CONNECT_CHECK(connect(this->_mainExecutor, SIGNAL(finished(const GGS::Core::Service &, GGS::GameExecutor::FinishState)),
      this, SLOT(internalServiceFinished(const GGS::Core::Service &, GGS::GameExecutor::FinishState)), Qt::QueuedConnection));

    SIGNAL_CONNECT_CHECK(connect(&this->_secondExecutor, SIGNAL(started(const GGS::Core::Service &)),
      this, SLOT(internalSecondServiceStarted(const GGS::Core::Service &)), Qt::QueuedConnection));

    SIGNAL_CONNECT_CHECK(connect(&this->_secondExecutor, SIGNAL(finished(const GGS::Core::Service &, GGS::GameExecutor::FinishState)),
      this, SLOT(internalSecondServiceFinished(const GGS::Core::Service &, GGS::GameExecutor::FinishState)), Qt::QueuedConnection));

    this->registerExecutors(&this->_simpleMainExecutor);
    this->registerExecutors(&this->_secondExecutor);
  }

  void PremiumExecutor::executeMain(GGS::Core::Service* service)
  {
    QMutexLocker locker(&this->_mutex);
    
    this->setNoInjectItem(service, "0");
    if (this->isSecondStarted(service->id()))
      this->_simpleMainExecutor.execute(*service);
    else
      this->_mainExecutor->execute(*service);

    this->_mainGameStarted.insert(service->id());
  }

  void PremiumExecutor::executeSecond(GGS::Core::Service* service, const GGS::RestApi::GameNetCredential& credetial)
  {
    QMutexLocker locker(&this->_mutex);

    if (!this->isMainStarted(service->id()))
      return;
    
    this->setNoInjectItem(service, "1");

    this->_secondExecutor.execute(*service, credetial);
    this->_secondGameStarted.insert(service->id());
  }
  
  bool PremiumExecutor::canSimpleExecuteMain(const QString& id)
  {
    QMutexLocker locker(&this->_mutex);
    return this->isSecondStarted(id);
  }

  bool PremiumExecutor::canExecuteSecond(const QString& id)
  {
    QMutexLocker locker(&this->_mutex);
    return this->isMainStarted(id);
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

  void PremiumExecutor::setMainExecutor(GGS::GameExecutor::GameExecutorService * value)
  {
    this->_mainExecutor = value;
  }

  void PremiumExecutor::internalSecondServiceStarted(const GGS::Core::Service &service)
  {
    QMutexLocker locker(&this->_mutex);
    this->_secondGameStarted.insert(service.id());
    emit this->secondServiceStarted(service);
  }

  void PremiumExecutor::internalSecondServiceFinished(const GGS::Core::Service &service, GGS::GameExecutor::FinishState state)
  {
    QMutexLocker locker(&this->_mutex);
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
    QMutexLocker locker(&this->_mutex);
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

  void PremiumExecutor::registerExecutors(GGS::GameExecutor::GameExecutorService *executor)
  {
    GGS::GameExecutor::Executor::ExecutableFile *gameExecutorByLauncher = new GGS::GameExecutor::Executor::ExecutableFile(this);
    executor->registerExecutor(gameExecutorByLauncher);

    GGS::GameExecutor::Executor::WebLink *webLinkExecutor = new GGS::GameExecutor::Executor::WebLink(this);
    executor->registerExecutor(webLinkExecutor);
  }

  void PremiumExecutor::setNoInjectItem(GGS::Core::Service* service, const QString& value)
  {
    QUrl url(service->url());
    url.removeAllQueryItems("noinject");
    url.addQueryItem("noinject", value);
    service->setUrl(url);
  }

}