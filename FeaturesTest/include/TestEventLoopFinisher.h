#pragma once


#include <QtCore/QObject>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
class TestEventLoopFinisher : public QObject
{
  Q_OBJECT
public:
  TestEventLoopFinisher(QEventLoop *loop, int timeout);
  ~TestEventLoopFinisher();
  
  bool isKilledByTimeout() const { return this->_timeoutKill; }
  bool setTerminateSignal(QObject *sender, const char* signal);

public slots:
  void terminateLoop();
  
private slots:
  void timeoutTick();

private:
  bool _timeoutKill;
  QTimer _timer;
  QEventLoop *_loop;
};
