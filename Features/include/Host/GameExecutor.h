#pragma once

#include <GameExecutor/GameExecutorService.h>

#include <QtCore/QObject>
#include <QtCore/QString>

#include <QtCore/QSet>
#include <QtCore/QMutex>

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

      virtual bool isGameStarted(const QString& serviceId) const;
      virtual bool isAnyGameStarted() const;

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

      void terminateAll(const QString& serviceId = QString());
      void terminateSecond(const QString& serviceId = QString());

    signals:
      void serviceStarted(const QString& serviceId);
      void serviceFinished(const QString& serviceId, int finishState);

    private:
      void onServiceStarted(const P1::Core::Service &service);
      void onServiceFinished(const P1::Core::Service &service, P1::GameExecutor::FinishState state);

      void prepairExecuteUrl(P1::Core::Service *service);

      P1::GameExecutor::GameExecutorService *_mainExecutor;
      P1::GameExecutor::ServiceInfoCounter *_gameExecutorServiceInfoCounter;
      ServiceProcess::ServiceLoader *_services;
      ServiceSettings* _serviceSettings;


      QSet<QString> _mainGameStarted;
      QMutex _mutex;
    };

  }
}