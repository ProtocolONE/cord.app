#pragma once

#include <QtCore/QObject>
#include <QAbstractNativeEventFilter>

namespace GGS {
  namespace Application {
    class TaskBarEventFilter: public QObject, public QAbstractNativeEventFilter
    {
      Q_OBJECT

    public:
      TaskBarEventFilter(QObject *parent = 0);
      virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *result);
      //void setTaskBarCreatedMsgId(unsigned int msgId);

    public slots:
      void onTaskBarButtonMsgRegistered(unsigned int);

signals:
      void taskBarButtonCreated();
    private:
      unsigned int _taskBarCreatedMsgId;
    };
  }
}