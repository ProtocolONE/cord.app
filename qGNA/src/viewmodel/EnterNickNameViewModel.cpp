#include <viewmodel/EnterNickNameViewModel.h>

EnterNickNameViewModel::EnterNickNameViewModel(QObject *parent)
  : GGS::GameExecutor::HookInterface(parent)
{
}

EnterNickNameViewModel::~EnterNickNameViewModel()
{
}

void EnterNickNameViewModel::CanExecute(GGS::Core::Service &service)
{
  this->_service = service;
  emit this->startCheck();
}

void EnterNickNameViewModel::success()
{
  emit this->canExecuteCompleted(this->_service, GGS::GameExecutor::Success);
}

void EnterNickNameViewModel::failed()
{
  emit this->canExecuteCompleted(this->_service, GGS::GameExecutor::CanExecutionHookBreak);
}
