/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2014, Syncopate Limited and/or affiliates. 
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/
#pragma once

#include <QtCore/QObject>
#include <QtCore/QAbstractNativeEventFilter>

namespace GGS {
  namespace Application {
    class TaskBarEventFilter: public QObject, public QAbstractNativeEventFilter
    {
      Q_OBJECT

    public:
      TaskBarEventFilter(QObject *parent = 0);
      virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;

    public slots:
      void onTaskBarButtonMsgRegistered(unsigned int);

    signals:
      void taskBarButtonCreated();
    
    private:
      unsigned int _taskBarCreatedMsgId;
    };
  }
}