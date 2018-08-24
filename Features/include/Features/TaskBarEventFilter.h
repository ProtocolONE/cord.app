#pragma once

#include <QtCore/QObject>
#include <QtCore/QAbstractNativeEventFilter>

namespace P1 {
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