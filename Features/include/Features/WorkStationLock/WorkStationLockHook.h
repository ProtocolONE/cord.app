#pragma once

#include <GameExecutor/HookInterface.h>

#include <QtCore/QObject>
#include <QtCore/QRect>

namespace P1 {
  namespace Core {
    class Service;
  }
}

namespace Features {
  namespace WorkStationLock {
    class RegisterSessionNotificationFilter;

    class WorkStationLockHook: public P1::GameExecutor::HookInterface
    {
      Q_OBJECT
    public:
      explicit WorkStationLockHook(QObject *parent = 0);
      virtual ~WorkStationLockHook();

      static QString id();

      virtual void PreExecute(P1::Core::Service &service) override;
      virtual void CanExecute(P1::Core::Service &service) override;

      void setFilter(RegisterSessionNotificationFilter * value);
    private:
      RegisterSessionNotificationFilter *_filter;
    };
  }
}