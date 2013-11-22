#include "TestEventLoopFinisher.h"
#include <QtCore/QDebug>
#include <QtCore/QTimer>

TestEventLoopFinisher::TestEventLoopFinisher(QEventLoop *loop, int timeout)
  : _loop(loop)
    , _timeoutKill(false)
{
  connect(&this->_timer, SIGNAL(timeout()), this, SLOT(timeoutTick()));
  this->_timer.start(timeout);
}

TestEventLoopFinisher::~TestEventLoopFinisher(void)
{
}

void TestEventLoopFinisher::terminateLoop()
{
  this->_timer.stop();
  if (this->_loop) {

    QTimer::singleShot(100, this->_loop, SLOT(quit()));
    this->_loop->quit();
  } else {
    qCritical() << "loop was not initialized";
  }
}
 
void TestEventLoopFinisher::timeoutTick()
{
  this->_timeoutKill = true;
  this->terminateLoop();
}

bool TestEventLoopFinisher::setTerminateSignal(QObject *sender, const char* signal)
{
  return QObject::connect(sender, signal, this, SLOT(terminateLoop()));
}
