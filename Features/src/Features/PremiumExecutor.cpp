/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (�) 2011 - 2012, Syncopate Limited and/or affiliates. 
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

using GGS::GameExecutor::GameExecutorService;
using GGS::Core::Service;
using GGS::RestApi::GameNetCredential;

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
    QObject::connect(&this->_simpleMainExecutor, &GameExecutorService::started,
      this, &PremiumExecutor::internalServiceStarted, Qt::QueuedConnection);

    QObject::connect(&this->_simpleMainExecutor, &GameExecutorService::finished,
      this, &PremiumExecutor::internalServiceFinished, Qt::QueuedConnection);

    QObject::connect(this->_mainExecutor, &GameExecutorService::started,
      this, &PremiumExecutor::internalServiceStarted, Qt::QueuedConnection);

    QObject::connect(this->_mainExecutor, &GameExecutorService::finished,
      this, &PremiumExecutor::internalServiceFinished, Qt::QueuedConnection);
    
    QObject::connect(&this->_secondExecutor, &GameExecutorService::started,
      this, &PremiumExecutor::internalSecondServiceStarted, Qt::QueuedConnection);

    QObject::connect(&this->_secondExecutor, &GameExecutorService::finished,
      this, &PremiumExecutor::internalSecondServiceFinished, Qt::QueuedConnection);
    
    this->registerExecutors(&this->_simpleMainExecutor);
    this->registerExecutors(&this->_secondExecutor);
  }

  void PremiumExecutor::executeMain(
    GGS::Core::Service* service, const GGS::RestApi::GameNetCredential& credetial)
  {
    QMutexLocker locker(&this->_mutex);
    
    this->setNoInjectItem(service, "0");
    if (this->isSecondStarted(service->id()))
      this->_simpleMainExecutor.executeEx(*service, credetial);
    else
      this->_mainExecutor->executeEx(*service, credetial);

    this->_mainGameStarted.insert(service->id());
  }

  void PremiumExecutor::executeSecond(
    GGS::Core::Service* service, 
    const GGS::RestApi::GameNetCredential& credetial, 
    const GGS::RestApi::GameNetCredential& secondCredetial)
  {
    QMutexLocker locker(&this->_mutex);

    if (!this->isMainStarted(service->id()))
      return;
    
    this->setNoInjectItem(service, "1");

    this->_secondExecutor.executeEx(*service, credetial, secondCredetial);
    this->_secondGameStarted.insert(service->id());
  }

  void PremiumExecutor::shutdownSecond(const QString& serviceId /* = QString()*/)
  {
    QMutexLocker locker(&this->_mutex);

    if (this->_secondExecutor.isAnyGameStarted()) {
      this->_secondExecutor.terminate(serviceId);
    }
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

  GameExecutorService* PremiumExecutor::secondExecutor()
  {
    return &this->_secondExecutor;
  }

  GameExecutorService* PremiumExecutor::simpleMainExecutor()
  {
    return &this->_simpleMainExecutor;
  }

  void PremiumExecutor::setMainExecutor(GameExecutorService* value)
  {
    Q_ASSERT(value);
    this->_mainExecutor = value;
    this->registerExecutors(value);
  }

  void PremiumExecutor::internalSecondServiceStarted(const Service &service)
  {
    QMutexLocker locker(&this->_mutex);
    this->_secondGameStarted.insert(service.id());
    emit this->secondServiceStarted(service);
  }

  void PremiumExecutor::internalSecondServiceFinished(const Service &service, GGS::GameExecutor::FinishState state)
  {
    QMutexLocker locker(&this->_mutex);
    this->_secondGameStarted.remove(service.id());
    emit this->secondServiceFinished(service, state);
  }

  void PremiumExecutor::internalServiceStarted(const Service &service)
  {
    this->_mainGameStarted.insert(service.id());
    emit this->serviceStarted(service);
  }

  void PremiumExecutor::internalServiceFinished(const Service &service, GGS::GameExecutor::FinishState state)
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

  void PremiumExecutor::registerExecutors(GameExecutorService *executor)
  {
    using GGS::GameExecutor::Executor::ExecutableFile;
    ExecutableFile *gameExecutorByLauncher = new ExecutableFile(this);
    executor->registerExecutor(gameExecutorByLauncher);

    using GGS::GameExecutor::Executor::WebLink;
    WebLink *webLinkExecutor = new WebLink(this);
    executor->registerExecutor(webLinkExecutor);
  }

  void PremiumExecutor::setNoInjectItem(Service* service, const QString& value)
  {
    if (!service->isDownloadable())
      return;

    QUrl url(service->url());
    url.removeAllQueryItems("noinject");
    url.addQueryItem("noinject", value);
    service->setUrl(url);
  }

  bool PremiumExecutor::isMainGameStarted()
  {
    return this->_mainGameStarted.count() > 0;
  }

  bool PremiumExecutor::isSecondGameStarted()
  {
    return this->_secondGameStarted.count() > 0;
  }

  void PremiumExecutor::terminateAll(const QString& serviceId /* = QString()*/)
  {
    if (this->_mainExecutor->isAnyGameStarted())
      this->_mainExecutor->terminate(serviceId);

    if (this->_simpleMainExecutor.isAnyGameStarted())
      this->_simpleMainExecutor.terminate(serviceId);

    if (this->_secondExecutor.isAnyGameStarted())
      this->_secondExecutor.terminate(serviceId);
  }

}