#pragma once

#include <QtCore/QObject>
#include <QtCore/QTimer>

namespace GameNet {
  namespace Integration {

    class TimeOut : public QObject
    {
      Q_OBJECT
    public:
      explicit TimeOut(QObject *parent = 0);
      virtual ~TimeOut();

      void start(int msec);

    public slots:
      void ping();

    signals:
      void pong();
      void timeout();

    private:
      void timeoutTick();

      QTimer _timeoutTimer;


    };

  }
}