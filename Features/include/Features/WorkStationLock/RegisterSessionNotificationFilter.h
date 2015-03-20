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

#include <QtCore/QObject>
#include <QtCore/QAbstractNativeEventFilter>

#include <Windows.h>

namespace Features {
  namespace WorkStationLock {

    /*!
      \class RegisterSessionNotificationFilter

      \brief Register session notification filter. 

      \sa https://msdn.microsoft.com/en-us/library/aa376875(VS.85).aspx
    */
    class RegisterSessionNotificationFilter: public QObject, public QAbstractNativeEventFilter
    {
      Q_OBJECT
    public:
      RegisterSessionNotificationFilter(QObject *parent, HWND hwnd);
      virtual ~RegisterSessionNotificationFilter();

      virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;
      bool isLocked() const;
    signals:
      void workStationLocked();
      void workStationUnlocked();

    private:
      HWND _hwnd;
      bool _isLocked;

    };
  }
}