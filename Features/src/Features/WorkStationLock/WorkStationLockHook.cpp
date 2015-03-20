#include <Features/WorkStationLock/WorkStationLockHook.h>
#include <Features/WorkStationLock/RegisterSessionNotificationFilter.h>

#include <Core/Service>
#include <QtCore/QDebug>

namespace Features {
  namespace WorkStationLock {

    WorkStationLockHook::WorkStationLockHook(QObject *parent) 
      : HookInterface(parent)
      , _filter(nullptr)
    {
    }

    WorkStationLockHook::~WorkStationLockHook()
    {
    }

    void WorkStationLockHook::PreExecute(GGS::Core::Service &service)
    {
      Q_ASSERT(this->_filter);
      emit this->preExecuteCompleted(
        service,
        this->_filter->isLocked() ? GGS::GameExecutor::PreExecutionHookBreak : GGS::GameExecutor::Success
      );
    }

    void WorkStationLockHook::CanExecute(GGS::Core::Service &service)
    {
      Q_ASSERT(this->_filter);
      emit this->canExecuteCompleted(
        service, 
        this->_filter->isLocked() ? GGS::GameExecutor::CanExecutionHookBreak : GGS::GameExecutor::Success
      );
    }

    QString WorkStationLockHook::id()
    {
      return "F5768A09-524A-407B-8974-BC8A7104BFA6";
    }

    void WorkStationLockHook::setFilter(RegisterSessionNotificationFilter* value)
    {
      Q_ASSERT(value);
      this->_filter = value;
    }
  }
}