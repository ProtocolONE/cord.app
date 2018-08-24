#include <Features/TaskBarEventFilter.h>

#include <QtCore/QObject>
#include <QtCore/QCoreApplication>
#include <Windows.h>

namespace P1 {
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