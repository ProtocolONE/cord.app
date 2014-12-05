/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2014, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

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