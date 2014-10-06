#pragma once

#include <GameExecutor/GameExecutorService.h>

#include <QtCore/QObject>
#include <QtCore/QString>

namespace Features {
  class PremiumExecutor;
}

namespace GGS {
  namespace GameExecutor {
    class ServiceInfoCounter;
  }

  namespace Core {
    class Service;
  }

  namespace RestApi {
    class GameNetCredential;
  }
}

namespace GameNet {
  namespace Host {

    class ServiceLoader;
    class Thetta;
    class ServiceSettings;

    class GameExecutor : public QObject
    {
      Q_OBJECT
    public:
      explicit GameExecutor(QObject *parent = 0);
      virtual ~GameExecutor();

      void setServices(ServiceLoader *value);
      void setThetta(Thetta *value);
      void setServiceSettings(ServiceSettings* value);

      virtual void init();
      void shutdown();
    
      virtual void execute(
        const QString& serviceId, 
        const GGS::RestApi::GameNetCredential& credetial);

      virtual void executeSecond(
        const QString& serviceId, 
        const GGS::RestApi::GameNetCredential& credetial, 
        const GGS::RestApi::GameNetCredential& secondCredetial);

      virtual bool isGameStarted(const QString& serviceId) const;
      virtual bool isAnyGameStarted() const;
      virtual bool canExecuteSecond(const QString& serviceId) const;
      virtual void shutdownSecond();

      GGS::GameExecutor::GameExecutorService *mainExecutor();

    signals:
      void serviceStarted(const QString& serviceId);
      void serviceFinished(const QString& serviceId, int finishState);
      void secondServiceStarted(const QString& serviceId);
      void secondServiceFinished(const QString& serviceId, int finishState);

    private:
      void onServiceStarted(const GGS::Core::Service &service);
      void onServiceFinished(const GGS::Core::Service &service, GGS::GameExecutor::FinishState state);

      void onSecondServiceStarted(const GGS::Core::Service &service);
      void onSecondServiceFinished(const GGS::Core::Service &service, GGS::GameExecutor::FinishState state);

      void prepairExecuteUrl(GGS::Core::Service *service);

      GGS::GameExecutor::GameExecutorService *_mainExecutor;
      Features::PremiumExecutor *_premiumExecutor;
      GGS::GameExecutor::ServiceInfoCounter *_gameExecutorServiceInfoCounter;
      ServiceLoader *_services;
      ServiceSettings* _serviceSettings;
      Thetta *_thetta;
    };

  }
}