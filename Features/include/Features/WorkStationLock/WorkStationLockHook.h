/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (c) 2011 - 2015, Syncopate Limited and/or affiliates. 
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/
#pragma once

#include <GameExecutor/HookInterface.h>

#include <QtCore/QObject>
#include <QtCore/QRect>

namespace GGS {
  namespace Core {
    class Service;
  }
}

namespace Features {
  namespace WorkStationLock {
    class RegisterSessionNotificationFilter;

    class WorkStationLockHook: public GGS::GameExecutor::HookInterface
    {
      Q_OBJECT
    public:
      explicit WorkStationLockHook(QObject *parent = 0);
      virtual ~WorkStationLockHook();

      static QString id();

      virtual void PreExecute(GGS::Core::Service &service) override;
      virtual void CanExecute(GGS::Core::Service &service) override;

      void setFilter(RegisterSessionNotificationFilter * value);
    private:
      RegisterSessionNotificationFilter *_filter;
    };
  }
}