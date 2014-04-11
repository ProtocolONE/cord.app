#include <Features/TaskBarEventFilter.h>


#include <QtCore/QObject>
#include <QtCore/QCoreApplication>
#include <QAbstractNativeEventFilter>
#include <Windows.h>

#include <QDebug>

namespace GGS {
  namespace Application {
    TaskBarEventFilter::TaskBarEventFilter(QObject *parent)
      : QObject(parent)
    {
    }

    bool TaskBarEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
    {
      MSG* msg = (MSG*)(message);
      if (this->_taskBarCreatedMsgId && this->_taskBarCreatedMsgId == msg->message) {
        emit this->taskBarButtonCreated();
        return true;
      }
      return false;
    }

    //void TaskBarEventFilter::setTaskBarCreatedMsgId(unsigned int msgId)
    //{
    //  this->_taskBarCreatedMsgId = msgId;
    //}


    void TaskBarEventFilter::onTaskBarButtonMsgRegistered(unsigned int msgId)
    {
      this->_taskBarCreatedMsgId = msgId;
    }
  }
}