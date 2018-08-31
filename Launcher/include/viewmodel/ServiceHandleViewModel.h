#pragma once

#include <QtCore/QObject>

class ServiceHandleBridgeProxy;

class ServiceHandleViewModel : public QObject
{
  Q_OBJECT
public:
  explicit ServiceHandleViewModel(QObject *parent = 0);
  virtual ~ServiceHandleViewModel();

signals:
  void serviceLocked(const QString &serviceId);
  void serviceUnlocked(const QString &serviceId);

private:
  ServiceHandleBridgeProxy *_serviceHandleProxy;
};