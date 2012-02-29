#ifndef ENTERNICKNAMEVIEWMODEL_H
#define ENTERNICKNAMEVIEWMODEL_H

#include <Core/Service>
#include <GameExecutor/Hookinterface>

#include <QObject>
#include <QtCore/QString>

class EnterNickNameViewModel : public GGS::GameExecutor::HookInterface
{
  Q_OBJECT
    
public:
  explicit EnterNickNameViewModel(QObject *parent);
  ~EnterNickNameViewModel();

  virtual void CanExecute(GGS::Core::Service &service);

signals:
  void startCheck();

public slots:
  void success();
  void failed();

private:
    GGS::Core::Service _service;

};

#endif // ENTERNICKNAMEVIEWMODEL_H
