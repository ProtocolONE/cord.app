#pragma once

#include <GameExecutor/GameExecutorService.h>

#include <QtCore/QObject>
#include <QtCore/QString>

namespace Features {
  class PremiumExecutor;
}

namespace P1 {
  namespace GameExecutor {
    class ServiceInfoCounter;
  }

  namespace Core {
    class Service;
  }

  namespace RestApi {
    class ProtocolOneCredential;
  }
}

namespace P1 {
  namespace Host {
    namespace ServiceProcess {
      class ServiceLoader;
    }
    class ServiceSettings;

    class GameExecutor : public QObject
    {
      Q_OBJECT
    public:
      explicit GameExecutor(QObject *parent = 0);
      virtual ~GameExecutor();

      void setServices(ServiceProcess::ServiceLoader *value);
      void setServiceSettings(ServiceSettings* value);

      virtual void init();
      void shutdown();
    
      virtual void execute(
        const QString& serviceId, 
        const P1::RestApi::ProtocolOneCredential& credetial);

      virtual void executeSecond(
        const QString& serviceId, 
        const P1::RestApi::ProtocolOneCredential& credetial, 
        const P1::RestApi::ProtocolOneCredential& secondCredetial);

      virtual bool isGameStarted(const QString& serviceId) const;
      virtual bool isSecondGameStarted(const QString& serviceId) const;
      virtual bool isAnyGameStarted() const;
      virtual bool canExecuteSecond(const QString& serviceId) const;
      virtual void shutdownSecond();

      /**
       * \fn  QString GameExecutor::executedGame() const;
       *
       * \brief Возвращает serviceId запущенной игры.
       *
       * \author  Ilya Tkachenko
       * \date  30.10.2014
       *
       * \return serviceId.
       */

      QString executedGame() const;

      P1::GameExecutor::GameExecutorService *mainExecutor();
      P1::GameExecutor::GameExecutorService* secondExecutor();
      P1::GameExecutor::GameExecutorService* simpleMainExecutor();

      void terminateAll(const QString& serviceId = QString());
      void terminateSecond(const QString& serviceId = QString());

    signals:
      void serviceStarted(const QString& serviceId);
      void serviceFinished(const QString& serviceId, int finishState);
      void secondServiceStarted(const QString& serviceId);
      void secondServiceFinished(const QString& serviceId, int finishState);

    private:
      void onServiceStarted(const P1::Core::Service &service);
      void onServiceFinished(const P1::Core::Service &service, P1::GameExecutor::FinishState state);

      void onSecondServiceStarted(const P1::Core::Service &service);
      void onSecondServiceFinished(const P1::Core::Service &service, P1::GameExecutor::FinishState state);

      void prepairExecuteUrl(P1::Core::Service *service);

      P1::GameExecutor::GameExecutorService *_mainExecutor;
      Features::PremiumExecutor *_premiumExecutor;
      P1::GameExecutor::ServiceInfoCounter *_gameExecutorServiceInfoCounter;
      ServiceProcess::ServiceLoader *_services;
      ServiceSettings* _serviceSettings;
    };

  }
}