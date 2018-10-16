#pragma once

#include <GameExecutor/GameExecutorService.h>

#include <Core/Service.h>

#include <RestApi/ProtocolOneCredential.h>

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
    void setMainExecutor(P1::GameExecutor::GameExecutorService * value);
    bool canSimpleExecuteMain(const QString& id);
    bool canExecuteSecond(const QString& id);

    void executeMain(
      P1::Core::Service* service, 
      const P1::RestApi::ProtocolOneCredential& credetial);

    void executeSecond(
      P1::Core::Service* service, 
      const P1::RestApi::ProtocolOneCredential& credetial, 
      const P1::RestApi::ProtocolOneCredential& secondCredetial);

    void shutdownSecond(const QString& serviceId = QString());

    void shutdown();
    void terminateAll(const QString& serviceId = QString());

    bool isGameStarted(const QString& id);
    bool isSecondGameStarted(const QString& id);
    bool isAnyGameStarted();

    QString firstRunningGame();
    QString firstRunningSecondGame();

    bool isMainGameStarted();
    bool isSecondGameStarted();

    P1::GameExecutor::GameExecutorService* secondExecutor();
    P1::GameExecutor::GameExecutorService* simpleMainExecutor();

  signals:
    void serviceStarted(const P1::Core::Service &service);
    void serviceFinished(const P1::Core::Service &service, P1::GameExecutor::FinishState serviceState);

    void secondServiceStarted(const P1::Core::Service &service);
    void secondServiceFinished(const P1::Core::Service &service, P1::GameExecutor::FinishState serviceState);

  private slots:
    void internalSecondServiceStarted(const P1::Core::Service &service);
    void internalSecondServiceFinished(const P1::Core::Service &service, P1::GameExecutor::FinishState state);

    void internalServiceStarted(const P1::Core::Service &service);
    void internalServiceFinished(const P1::Core::Service &service, P1::GameExecutor::FinishState state);

  private:
    P1::GameExecutor::GameExecutorService _secondExecutor;
    P1::GameExecutor::GameExecutorService _simpleMainExecutor;
    P1::GameExecutor::GameExecutorService *_mainExecutor;
    
    QSet<QString> _mainGameStarted;
    QSet<QString> _secondGameStarted;
    QMutex _mutex;

    bool isMainStarted(const QString& id);
    bool isSecondStarted(const QString& id);
    void registerExecutors(P1::GameExecutor::GameExecutorService *executor);
    void setNoInjectItem(P1::Core::Service* service, const QString& value);

  };

}