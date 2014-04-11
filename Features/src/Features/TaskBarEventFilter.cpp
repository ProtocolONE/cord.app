/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2014, Syncopate Limited and/or affiliates. 
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/
#include <Features/TaskBarEventFilter.h>

#include <QtCore/QObject>
#include <QtCore/QCoreApplication>
#include <Windows.h>

namespace GGS {
  namespace Application {
    TaskBarEventFilter::TaskBarEventFilter(QObject *parent)
      : QObject(parent)
      , _taskBarCreatedMsgId(0)
    {
    }

    bool TaskBarEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
    {
      MSG* msg = reinterpret_cast<MSG*>(message);
      if (this->_taskBarCreatedMsgId && this->_taskBarCreatedMsgId == msg->message) {
        emit this->taskBarButtonCreated();
        return true;
      }
      return false;
    }

    void TaskBarEventFilter::onTaskBarButtonMsgRegistered(unsigned int msgId)
    {
      this->_taskBarCreatedMsgId = msgId;
    }
  }
}