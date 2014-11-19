#include <Integration/TimeOut.h>

namespace GameNet {
  namespace Integration {

    TimeOut::TimeOut(QObject *parent /*= 0*/)
      : QObject(parent)
    {
      QObject::connect(&this->_timeoutTimer, &QTimer::timeout,
        this, &TimeOut::timeoutTick);
    }

    TimeOut::~TimeOut()
    {
    }

    void TimeOut::start(int msec)
    {
      this->_timeoutTimer.setInterval(msec);
      this->_timeoutTimer.start();
    }

    void TimeOut::ping()
    {
      this->_timeoutTimer.start();
      emit this->pong();
    }

    void TimeOut::timeoutTick()
    {
      this->_timeoutTimer.stop();
      emit this->timeout();
    }

  }
}