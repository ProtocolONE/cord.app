/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates. 
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/
#pragma once

#include <GameExecutor/GameExecutorService.h>

#include <Core/Service.h>

#include <RestApi/GameNetCredential.h>

#include <QtCore/QObject>
#include <QtCore/QSet>
#include <QtCore/QMutex>

namespace Features {

  class PremiumExecutor : public QObject
  {
    Q_OBJECT
  public:
    PremiumExecutor(QObject *parent = 0);
    ~PremiumExecutor();

    void init();
    void setMainExecutor(GGS::GameExecutor::GameExecutorService * value);
    bool canSimpleExecuteMain(const QString& id);
    bool canExecuteSecond(const QString& id);

    void executeMain(GGS::Core::Service* service);
    void executeSecond(GGS::Core::Service* service, const GGS::RestApi::GameNetCredential& credetial);

    void shutdown();

    bool isGameStarted(const QString& id);
    bool isSecondGameStarted(const QString& id);
    bool isAnyGameStarted();

    GGS::GameExecutor::GameExecutorService* secondExecutor();
    GGS::GameExecutor::GameExecutorService* simpleMainExecutor();

  signals:
    void serviceStarted(const GGS::Core::Service &service);
    void serviceFinished(const GGS::Core::Service &service, GGS::GameExecutor::FinishState serviceState);
    
    void secondServiceStarted(const GGS::Core::Service &service);
    void secondServiceFinished(const GGS::Core::Service &service, GGS::GameExecutor::FinishState serviceState);

  private slots:
    void internalSecondServiceStarted(const GGS::Core::Service &service);
    void internalSecondServiceFinished(const GGS::Core::Service &service, GGS::GameExecutor::FinishState state);
                 
    void internalServiceStarted(const GGS::Core::Service &service);
    void internalServiceFinished(const GGS::Core::Service &service, GGS::GameExecutor::FinishState state);

  private:
    GGS::GameExecutor::GameExecutorService _secondExecutor;
    GGS::GameExecutor::GameExecutorService _simpleMainExecutor;
    GGS::GameExecutor::GameExecutorService *_mainExecutor;
    
    QSet<QString> _mainGameStarted;
    QSet<QString> _secondGameStarted;
    QMutex _mutex;

    bool isMainStarted(const QString& id);
    bool isSecondStarted(const QString& id);
    void registerExecutors(GGS::GameExecutor::GameExecutorService *executor);
  };

}